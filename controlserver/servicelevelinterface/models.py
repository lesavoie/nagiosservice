from django.db import models

class Contact(models.Model):
   '''
   Contains information about how and when to contact a Nagios user.
   '''
   name = models.SlugField()   # Only accepts letters, numbers, underscores, and hyphens
   email_address = models.EmailField(max_length=254)
