from django.contrib.auth.models import User
from rest_framework import serializers
from servicelevelinterface.models import Monitor, Contact, Command


class MonitorSerializer(serializers.ModelSerializer):
   owner = serializers.CharField(source='owner.username', read_only=True)

   class Meta:
      model = Monitor


class ContactSerializer(serializers.ModelSerializer):
   owner = serializers.CharField(source='owner.username', read_only=True)

   class Meta:
      model = Contact


class CommandSerializer(serializers.ModelSerializer):
   class Meta:
      model = Command
      

# Serializer used just when creating users.  It only provides a subset of the
# fields.
class CreateUserSerializer(serializers.ModelSerializer):
   class Meta:
      model = User
      fields = ('id', 'username', 'password', 'email')

