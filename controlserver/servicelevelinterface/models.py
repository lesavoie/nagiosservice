from django.db import models

# This is arbitrary - it may need to be changed
MAX_HUMAN_READABLE_NAME_LENGTH = 254

MAX_EMAIL_ADDRESS_LENGTH = 254

MAX_COMMAND_LINE_PARAMS_LENGTH = 100


class Contact(models.Model):
   '''
   Contains information about how to contact a Nagios user.
   '''
   
   owner = models.ForeignKey('auth.User',
      help_text='The user that created this contact')
   
   contact_name = models.SlugField(
      help_text='A unique name for this contact')
   
   human_readable_name = models.CharField(
      max_length=MAX_HUMAN_READABLE_NAME_LENGTH,
      blank=True,
      help_text='Human-readable version of the contact name for convenience (optional)')
   
   email_address = models.EmailField(max_length=MAX_EMAIL_ADDRESS_LENGTH,
      help_text='Email address to send notifications to in case of failures')

   # Handles how this model is displayed in drop down lists
   def __unicode__(self):
      return self.contact_name
	
   class Meta:
      unique_together = ("owner", "contact_name")


class Command(models.Model):
   '''
   Commands to run when performing checks
   '''

   command_name = models.SlugField(
      help_text='The unique name for this command')
   
   description = models.TextField(
      help_text='Help text for this command')
   
   # Handles how this model is displayed in drop down lists
   def __unicode__(self):
      return self.command_name
	

class Monitor(models.Model):
   '''
   Contains information about what to monitor, how to monitor it, and who to
   contact when a check fails.
   '''
   
   owner = models.ForeignKey('auth.User',
      help_text='The user that created this monitor')
   
   monitor_name = models.SlugField(
      help_text='A unique name for this monitor')
   
   human_readable_name = models.CharField(
      max_length=MAX_HUMAN_READABLE_NAME_LENGTH,
      blank=True,
      help_text='Human-readable version of the monitor name for convenience (optional)')
      
   address = models.GenericIPAddressField(
      help_text='The IP address of the host to monitor')
   
   max_check_attempts = models.IntegerField(
      help_text='Number of times to retry a failed check before sending an alert. If you enter zero in this field, it will revert to the default value of 3.')
   
   check_interval = models.IntegerField(
      help_text='How often (in minutes) to run this check. If you enter zero in this field, it will revert to the default value of 10.')
   
   contacts = models.ManyToManyField(Contact,
      help_text='List of contacts to notify when this check fails')
   
   notification_interval = models.IntegerField(
      help_text='Amount of time (in minutes) between successive alerts if this check continues to fail. If you enter zero in this field, it will revert to the default value of 60.')
   
   command = models.ForeignKey(Command,
      help_text='The command to run when performing the check')
   
   warning_level = models.CharField(
      max_length=MAX_COMMAND_LINE_PARAMS_LENGTH,
      blank=True,
      help_text='The level at which to send a warning for this check. Specifying a warning level is optional.')

   critical_level = models.CharField(
      max_length=MAX_COMMAND_LINE_PARAMS_LENGTH,
      blank=True,
      help_text='The level at which to send a critical alert for this check. This field is optional because it is not required for all commands.')
      
   miscellaneous = models.CharField(
      max_length=MAX_COMMAND_LINE_PARAMS_LENGTH,
      blank=True,
      help_text='The additional arguments needed by some commands.')
   
   class Meta:
      unique_together = ("owner", "monitor_name")
