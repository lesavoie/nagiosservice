from rest_framework import serializers
from servicelevelinterface.models import Contact

class ContactSerializer(serializers.ModelSerializer):
   class Meta:
      model = Contact
      fields = ('id', 'name', 'email_address')

