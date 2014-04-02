from django.contrib.auth.models import User
from rest_framework import viewsets
from rest_framework import permissions
from servicelevelinterface.models import Monitor, Contact, Command
from servicelevelinterface.permissions import IsOwner
from servicelevelinterface.serializers import MonitorSerializer, ContactSerializer, CommandSerializer, UserSerializer


# TODO: the current permissions allow any authenticated user to view everything,
# but only the owner to view/update the specific object.


class MonitorViewSet(viewsets.ModelViewSet):
   queryset = Monitor.objects.all()
   serializer_class = MonitorSerializer
   permission_classes = (permissions.IsAuthenticated,
                         IsOwner)
   
   def pre_save(self, obj):
      # Save the user that created this object
      obj.owner = self.request.user


class ContactViewSet(viewsets.ModelViewSet):
   queryset = Contact.objects.all()
   serializer_class = ContactSerializer
   permission_classes = (permissions.IsAuthenticated,
                         IsOwner,)
      
   def pre_save(self, obj):
      # Save the user that created this object
      obj.owner = self.request.user


class CommandViewSet(viewsets.ModelViewSet):
   queryset = Command.objects.all()
   serializer_class = CommandSerializer
   permission_classes = (permissions.IsAuthenticated,
                         IsOwner,)

   def pre_save(self, obj):
      # Save the user that created this object
      obj.owner = self.request.user


class UserViewSet(viewsets.ReadOnlyModelViewSet):
   queryset = User.objects.all()
   serializer_class = UserSerializer
