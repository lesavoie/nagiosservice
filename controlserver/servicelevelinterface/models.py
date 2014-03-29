from django.db import models

# This is arbitrary - it may need to be changed
MAX_HUMAN_READABLE_NAME_LENGTH = 254

MAX_EMAIL_ADDRESS_LENGTH = 254

MAX_COMMAND_LINE_PARAMS_LENGTH = 1000
   

class Contact(models.Model):
   '''
   Contains information about how to contact a Nagios user.
   '''
   # TODO: need a way to sync the canonical state with the actual state
   # TODO: add support for other types of contacts (pagers are built in,
   # could also include things like pings to servers, IMs, etc).
   
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
      

class Command(models.Model):
   '''
   Allows the user to create custom commands to perform checks.
   '''

   # Name of the command - used to reference it in other objects   
   command_name = models.SlugField()
   
   # Command line arguments to pass to the command when it runs
   command_line_parameters = models.CharField(
         max_length=MAX_COMMAND_LINE_PARAMS_LENGTH,
	 blank=True)

   # Binary executable that is run for this command.
   # The binaries are storied in this location under MEDIA_ROOT (defined in
   # controlserver/settings.py).  In our case, the binaries will be put into
   # /home/media/bin/.
   # TODO: files aren't deleted when the associated object is deleted from the
   # database.  Need to come up with a way to sync them from time to time.
   binary = models.FileField(upload_to='bin/')


class Monitor(models.Model):
   '''
   Contains information about what to monitor, how to monitor it, and who to
   contact when a check fails.
   '''
   
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
