from django.contrib.auth.models import User
from rest_framework import mixins
from rest_framework import permissions
from rest_framework.authtoken.models import Token
from rest_framework import viewsets
from rest_framework.response import Response
from rest_framework import status
from servicelevelinterface.models import Monitor, Contact, Command
from servicelevelinterface.permissions import IsOwner
from servicelevelinterface.serializers import MonitorSerializer, ContactSerializer, CommandSerializer, CreateUserSerializer
from servicelevelinterface.mapperinterface import MapperInterface


class BaseNagiosViewSet(viewsets.ModelViewSet):
   '''
   Base class that customizes the default ModelViewSet according to our needs.
   It does the following:
   1) Restricts the queryset to only those items owned by the user
   2) Updates the mapper any time a change is made
   '''
   
   mapper_interface = MapperInterface()

   def get_queryset(self):
      # Restrict the queryset to only those objects owned by the user
      user = self.request.user
      return self.serializer_class.Meta.model.objects.filter(owner=user)

   def create(self, request):
      ret = super(BaseNagiosViewSet, self).create(request)
      # Notify the mapper of the update
      self.mapper_interface.do_map(self.request.user)
      return ret

   def update(self, request, pk=None):
      ret = super(BaseNagiosViewSet, self).update(request, pk)
      # Notify the mapper of the update
      self.mapper_interface.do_map(self.request.user)
      return ret

   def partial_update(self, request, pk=None):
      ret = super(BaseNagiosViewSet, self).partial_update(request, pk)
      # Notify the mapper of the update
      self.mapper_interface.do_map(self.request.user)
      return ret

   def destroy(self, request, pk=None):
      ret = super(BaseNagiosViewSet, self).destroy(request, pk)
      # Notify the mapper of the update
      self.mapper_interface.do_map(self.request.user)
      return ret


class MonitorViewSet(BaseNagiosViewSet):
   queryset = Monitor.objects.all()
   serializer_class = MonitorSerializer
   permission_classes = (permissions.IsAuthenticated,
                         IsOwner)
   
   def pre_save(self, obj):
      # Save the user that created this object
      obj.owner = self.request.user


class ContactViewSet(BaseNagiosViewSet):
   queryset = Contact.objects.all()
   serializer_class = ContactSerializer
   permission_classes = (permissions.IsAuthenticated,
                         IsOwner,)
      
   def pre_save(self, obj):
      # Save the user that created this object
      obj.owner = self.request.user


# Currently, only default commands are provided and users are not allowed to
# generate their own custom commands.  Consequently, no permissions are required
# to view commands, and create/update/delete are not allowed.
class CommandViewSet(viewsets.ReadOnlyModelViewSet):
   queryset = Command.objects.all()
   serializer_class = CommandSerializer


# This view is only used to create new users.
# This code is based on code from: http://stackoverflow.com/questions/16857450/how-to-register-users-in-django-rest-framework
class CreateUserViewSet(mixins.CreateModelMixin,
                        viewsets.GenericViewSet):
   model = User
   queryset = User.objects.all()
   serializer_class = CreateUserSerializer
   
   def create(self, request):
      serialized = self.get_serializer(data=request.DATA)
      if serialized.is_valid():
         user = User.objects.create_user(
            username=serialized.init_data['username'],
            password=serialized.init_data['password'],
            email=serialized.init_data['email'],)
	    
	 # Generate a token for this user
         token = Token.objects.create(user=user)
	 
	 # The token is included in the header for the response
         return Response(serialized.data,
	                 status=status.HTTP_201_CREATED,
			 headers={'Token':token})
      else:
         return Response(serialized._errors, status=status.HTTP_400_BAD_REQUEST)
