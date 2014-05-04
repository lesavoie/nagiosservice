from servicelevelinterface.models import Monitor, Contact, Command
import os
import shutil
import subprocess

class Mapper:
   single_command_template="define command{\n\tcommand_name\t%s\n\tcommand_line\t%s %s\n\t}\n"
   single_contact_template="define contact{\n\tcontact_name\t%s\n\tuse\tgeneric-contact\n\talias\t%s\n\temail\t%s\n\t}\n"
   single_contactgroup_template="define contactgroup{\n\tcontactgroup_name\t%s\n\talias\t%s\n\tmembers\t%s\n\t}\n"
   single_host_template="define host{\n\tuse\tgeneric-host\n\thost_name\t%s\n\talias\t%s\n\taddress\t%s\n\tmax_check_attempts\t%d\n\tcontact_groups\t%s\n\t}\n"
   single_service_template="define service{\n\tuse\tgeneric-service\n\thost_name\t%s\n\tservice_description\t%s\n\tcheck_command\t%s\n\tmax_check_attempts\t%d\n\tnormal_check_interval\t%d\n\tcontact_groups\t%s\n\tnotification_interval\t%d\n\t}\n"
   
   def constructSingleContact(self, contact):
      snippet = (self.single_contact_template % (contact.contact_name, contact.human_readable_name,  contact.email_address))
      return snippet
   
   def constructContactgroup(self, name_prefix, contacts, contactsFile):
      name = name_prefix
      alias = ""
      members = ""
      if (type(contacts) is list):
         contactList = contacts
      else:
         contactList = contacts.all()
      for contact in contactList:
         name += "_" + contact.contact_name
         alias += contact.human_readable_name + "_"
         members += contact.contact_name + ","
      alias = alias[:-1]
      members = members[:-1]
      snippet = (self.single_contactgroup_template % (name, alias, members))
      contactsFile.write(snippet)
      return name

   # TODO: contact_groups 
   def processMonitor(self, monitor, cfgFile):
      
      maxCheckAttempts = monitor.max_check_attempts
      checkInterval = monitor.check_interval
      notificationInterval = monitor.notification_interval
         
      # construct command string
      # TODO: will need to change when we support command line parameters
      command_str = "checknrpe_1arg!" + monitor.command.command_name
      
      contactgroups = self.constructContactgroup(monitor.monitor_name, monitor.contacts, cfgFile)
      
      host_str = (self.single_host_template % (monitor.monitor_name, monitor.human_readable_name, monitor.address, maxCheckAttempts, contactgroups))
      cfgFile.write(host_str)
      
      service_str = (self.single_service_template % (monitor.monitor_name, monitor.command.command_name + " on " + monitor.monitor_name, command_str, maxCheckAttempts, checkInterval, contactgroups, notificationInterval))
      cfgFile.write(service_str)

   def do_map(self, user, monitors, contacts, commands):
      parentDir = "/tmp/" + user.username + "/"
      try:
         os.makedirs(parentDir)
      except OSError:
         print "Recreating %s because it exists already." % parentDir
         shutil.rmtree(parentDir)
         os.makedirs(parentDir)
      cfgFile = open(parentDir + user.username + ".cfg", "w")
      print "Mapping user:"
      print user
      print "Contacts:"
      print contacts
      for contact in contacts:
         result = self.constructSingleContact(contact)
         cfgFile.write(result)
      self.constructContactgroup("all-contact", contacts, cfgFile)
      print "Monitors:"
      print monitors
      for monitor in monitors:
         self.processMonitor(monitor, cfgFile)
      print "Commands:"
      print commands
      cfgFile.close()
      
      # call tool (Timeout only works for python3.3 or this code won't compile)
      ret = subprocess.call(["ns_tools -t 1 -c 127.0.0.1 -p 5600 -i " + user.username + " -f " + parentDir + "cfgFile.cfg"], shell=True)
      
      # clean up temp folder
      shutil.rmtree(parentDir)


class MapperInterface:
   # Interface to the mapper component.  Whenever there is a change to a user's
   # data, the map function is called.  It queries the database to get the
   # user's info and passes that data to map on the actual mapper.
   
   mapper = Mapper()
   
   def do_map(self, user):
      # Query for the user's newest data and pass it to the mapper.
      monitors = Monitor.objects.filter(owner__exact=user)
      contacts = Contact.objects.filter(owner__exact=user)
      # For now, all commands are default commands and are not associated with
      # a user.
      commands = Command.objects.all()
      self.mapper.do_map(user, monitors, contacts, commands)
