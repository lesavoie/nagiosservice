from servicelevelinterface.models import Monitor, Contact, Command


class Mapper:
   # TODO: this class needs to be filled in with the mapper code (or changed
   # to call it)
   def do_map(self, user, monitors, contacts, commands):
      print "Mapping user:"
      print user
      print "Monitors:"
      print monitors
      print "Contacts:"
      print contacts
      print "Commands:"
      print commands


class MapperInterface:
   # Interface to the mapper component.  Whenever there is a change to a user's
   # data, the map function is called.  It queries the database to get the
   # user's info and passes that data to map on the actual mapper.
   
   mapper = Mapper()
   
   def do_map(self, user):
      # Query for the user's newest data and pass it to the mapper.
      # TODO: this really should be done in a different thread.
      monitors = Monitor.objects.filter(owner__exact=user)
      contacts = Contact.objects.filter(owner__exact=user)
      # For now, all commands are default commands and are not associated with
      # a user.s
      commands = Command.objects.none()
      self.mapper.do_map(user, monitors, contacts, commands)
