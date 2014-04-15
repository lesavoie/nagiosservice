from servicelevelinterface.models import Monitor, Contact, Command
import os
import shutil
import subprocess

class Mapper:
   single_command_template="define command{\n\tcommand_name\t%s\n\tcommand_line\t%s %s\n\t}\n"
   single_contact_template="define contact{\n\tcontact_name\t%s\n\tuse\tgeneric-contact\n\talias\t%s\n\temail\t%s\n\t}\n"
   single_host_template="define host{\n\tuse\tgeneric-host\n\thost_name\t%s\n\talias\t%s\n\taddress\t%s\n\t}\n"
   single_service_template="define service{\n\tuse\tgenaric-service\n\thost_name\t%s\n\tservice_description\t%s\n\tcheck_command\t%s\n\tmax_check_attempts\t%d\n\tnormal_check_interval\t%d\n\tnotification_interval\t%d\n\t}\n"
   
   def constructSingleContact(self, contact):
      snippet = (self.single_contact_template % (contact.contact_name, contact.human_readable_name,  contact.email_address))
      return snippet

   # TODO: contact_groups 
   def processMonitor(self, monitor, hostsFile, servicesFile):
      host_str = (self.single_host_template % (monitor.monitor_name, monitor.human_readable_name, monitor.address))
      hostsFile.write(host_str)
      
      # default max check attempts 3 times to determine its final(hard) state
      maxCheckAttempts = 3
      if monitor.max_check_attempts != 0:
         maxCheckAttempts = monitor.max_check_attempts
      
      # default check every 10 minutes in normal condition
      checkInterval = 10
      if monitor.check_interval != 0:
         checkInterval = monitor.check_interval
      
      # default re-notify the user every 1 hour = 60 minutes
      notificationInterval = 60
      if monitor.notification_interval != 0:
         notificationInterval = monitor.notification_interval
         
      # construct command string
      # TODO: will need to change when we support command line parameters
      command_str = "checknrpe_1arg!" + monitor.command.command_name
      
      service_str = (self.single_service_template % (monitor.monitor_name, monitor.command.command_name + " on " + monitor.monitor_name, command_str, maxCheckAttempts, checkInterval, notificationInterval))
      servicesFile.write(service_str)

   def do_map(self, user, monitors, contacts, commands):
      parentDir = "/tmp/" + user.username + "/"
      os.makedirs(parentDir)
      contactsFile = open(parentDir + "contacts.cfg", "w")
      hostsFile = open(parentDir + "hosts.cfg", "w")
      servicesFile = open(parentDir + "services.cfg", "w")
      print "Mapping user:"
      print user
      print "Monitors:"
      print monitors
      for monitor in monitors:
         self.processMonitor(monitor, hostsFile, servicesFile)
      print "Contacts:"
      print contacts
      for contact in contacts:
         result = self.constructSingleContact(contact)
         contactsFile.write(result)
      print "Commands:"
      print commands
      contactsFile.close()
      hostsFile.close()
      servicesFile.close()
      
      # call tool
      # ret = subprocess.call(["tool", "-i", user.username, "-t", "1", "-f", hostsFile.abspath], shell=True)
      # ret = subprocess.call(["tool", "-i", user.username, "-t", "2", "-f", servicesFile.abspath], shell=True)
      # ret = subprocess.call(["tool", "-i", user.username, "-t", "3", "-f", contactsFile.abspath], shell=True)
      
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
