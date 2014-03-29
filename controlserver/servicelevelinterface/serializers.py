from rest_framework import serializers
from servicelevelinterface.models import Monitor, Contact, Command


class MonitorSerializer(serializers.ModelSerializer):
   class Meta:
      model = Monitor


class ContactSerializer(serializers.ModelSerializer):
   class Meta:
      model = Contact


class CommandSerializer(serializers.ModelSerializer):
   class Meta:
      model = Command

