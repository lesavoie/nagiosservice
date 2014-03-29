from django.db import models

# TODO: this is arbitrary - it may need to be changed
MAX_HUMAN_READABLE_NAME_LENGTH = 254

MAX_EMAIL_ADDRESS_LENGTH = 254

MAX_COMMAND_LINE_PARAMS_LENGTH = 1000


class Monitor(models.Model):
   '''
   Contains information about what to monitor, how to monitor it, and who to
   contact when a check fails.
   '''
   
   # The name that allows the monitor to be referenced by other objects
   # TODO: need a way to make sure this relationship is enforced.
   monitor_name = models.SlugField()
   
   # Human readable version of the monitor name for convenience
   human_readable_name = models.CharField(
      max_length=MAX_HUMAN_READABLE_NAME_LENGTH)
      
   # IP address to monitor
   # TODO: also allow DNS addresses
   address = models.GenericIPAddressField()
   
   # Number of times to retry a failed check before sending an alert
   max_check_attempts = models.IntegerField()
   
   # How often (in minutes) to check this monitor
   check_interval = models.IntegerField()
   
   # List of contacts to notify when a check fails
   # This should be a list of Contact.contact_names
   contacts = models.TextField()
   
   # How often to notify the contacts that the check is still failing
   notification_interval = models.IntegerField()
   
   # The command to run to perform the check
   # Should be a list of Command.command_names
   command = models.SlugField()
   


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
      max_length=MAX_HUMAN_READABLE_NAME_LENGTH)
   
   # Email address to contact the person at
   email_address = models.EmailField(max_length=MAX_EMAIL_ADDRESS_LENGTH)
   
   def save(self, *args, **kwargs):
      '''
      Called when the object is saved to the database.
      Possible place to intercept the data and send it to the mapper.
      '''
      super(Contact, self).save(*args, **kwargs)
      
   def delete(self, *args, **kwargs):
      '''
      Called when the object is deleted.
      Possible place to intercept the data and send it to the mapper.
      '''
      # TODO: May not be called for bulk deletes - I don't know of a way to handle this
      super(Contact, self).delete(*args, **kwargs)
      

class Command(models.Model):
   '''
   Allows the user to create custom commands to perform checks.
   '''

   # Name of the command - used to reference it in other objects   
   command_name = models.SlugField()
   
   # Command line arguments to pass to the command when it runs
   command_line_parameters = models.CharField(
   		max_length=MAX_COMMAND_LINE_PARAMS_LENGTH)

   # Binary executable that is run for this command
   # TODO: change this to a FileField and set up and define where to upload
   # the files to.
   binary = models.TextField()
