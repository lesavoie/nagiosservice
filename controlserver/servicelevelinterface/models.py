from django.db import models

# This is arbitrary - it may need to be changed
MAX_HUMAN_READABLE_NAME_LENGTH = 254

MAX_EMAIL_ADDRESS_LENGTH = 254

MAX_COMMAND_LINE_PARAMS_LENGTH = 1000
   

class Contact(models.Model):
   '''
   Contains information about how to contact a Nagios user.
   '''
   # TODO: add support for other types of contacts (pagers are built in,
   # could also include things like pings to servers, IMs, etc).
   
   # The user that owns this Contact
   # TODO: setting editable to false doesn't seem to do anything - it still
   # shows up on the form.
   owner = models.ForeignKey('auth.User', editable=False)
   
   # The name that allows the contact to be referenced in other objects
   # SlugField only accepts letters, numbers, underscores, and hyphens
   contact_name = models.SlugField()
   
   # Human readable version of the contact name
   # The max_length used here may need to be adjusted
   human_readable_name = models.CharField(
      max_length=MAX_HUMAN_READABLE_NAME_LENGTH,
      blank=True)
   
   # Email address to contact the person at
   email_address = models.EmailField(max_length=MAX_EMAIL_ADDRESS_LENGTH)

   # Handles how this model is displayed in drop down lists
   def __unicode__(self):
      return self.contact_name
	
   class Meta:
      unique_together = ("owner", "contact_name")


class Command(models.Model):
   '''
   Commands to run when performing checks
   '''

   # Name of the command - used to reference it in other objects   
   command_name = models.SlugField()
   
   # Default command line options that should be passed every time the command
   # is executed.  Can include variables that will be filled in based on
   # Monitor.command_line_parameters (see Nagios documentation for info on
   # how to specify variables in command lines).
   command_line = models.CharField(
         max_length=MAX_COMMAND_LINE_PARAMS_LENGTH,
	 blank=True)

   # Handles how this model is displayed in drop down lists
   def __unicode__(self):
      return self.command_name
	

class Monitor(models.Model):
   '''
   Contains information about what to monitor, how to monitor it, and who to
   contact when a check fails.
   '''
   
   # The user that owns this Monitor
   owner = models.ForeignKey('auth.User', editable=False)
   
   # Name of the monitor
   monitor_name = models.SlugField()
   
   # Human readable name for the monitor for convenience
   human_readable_name = models.CharField(
      max_length=MAX_HUMAN_READABLE_NAME_LENGTH,
      blank=True)
      
   # IP address to monitor
   # TODO: also allow DNS addresses
   address = models.GenericIPAddressField()
   
   # Number of times to retry a failed check before sending an alert
   max_check_attempts = models.IntegerField()
   
   # How often (in minutes) to check this monitor
   check_interval = models.IntegerField()
   
   # List of contacts to notify when a check fails
   contacts = models.ManyToManyField(Contact)
   
   # How often to notify the contacts that the check is still failing
   notification_interval = models.IntegerField()
   
   # The command to run to perform the check
   # TODO: need to add a set of default commands
   command = models.ForeignKey(Command)
   
   # Command line arguments to pass when the command is executed for this
   # monitor.  This should include values to pass in for the variables in
   # Command.command_line_parameters (see Nagios documentation for info on
   # how to specify variables in command lines).
   command_line_parameters = models.CharField(
         max_length=MAX_COMMAND_LINE_PARAMS_LENGTH,
	 blank=True)

   class Meta:
      unique_together = ("owner", "monitor_name")
