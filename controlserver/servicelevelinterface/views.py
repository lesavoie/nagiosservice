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


# TODO: the current permissions aren't quite working - sometimes users can view
# things they shouldn't be able to view.
# I think this has to do with the fact that calling list on a viewset doesn't
# invoke object level permissions.


class MapperViewSet(viewsets.ModelViewSet):
   # Base class that notifies the mapper whenever there is a change to a
   # user's data.
   mapper_interface = MapperInterface()


   def list(self, request):
      return super(MapperViewSet, self).list(request)
	
   def create(self, request):
      ret = super(MapperViewSet, self).create(request)
      self.mapper_interface.do_map(self.request.user)
      return ret

   def retrieve(self, request, pk=None):
      ret = super(MapperViewSet, self).retrieve(request, pk)
      self.mapper_interface.do_map(self.request.user)
      return ret

   def update(self, request, pk=None):
      ret = super(MapperViewSet, self).update(request, pk)
      self.mapper_interface.do_map(self.request.user)
      return ret

   def partial_update(self, request, pk=None):
      ret = super(MapperViewSet, self).partial_update(request, pk)
      self.mapper_interface.do_map(self.request.user)
      return ret

   def destroy(self, request, pk=None):
      ret = super(MapperViewSet, self).destroy(request, pk)
      self.mapper_interface.do_map(self.request.user)
      return ret


class MonitorViewSet(MapperViewSet):
   queryset = Monitor.objects.all()
   serializer_class = MonitorSerializer
   permission_classes = (permissions.IsAuthenticated,
                         IsOwner)
   
   def pre_save(self, obj):
      # Save the user that created this object
      obj.owner = self.request.user


class ContactViewSet(MapperViewSet):
   queryset = Contact.objects.all()
   serializer_class = ContactSerializer
   permission_classes = (permissions.IsAuthenticated,
                         IsOwner,)
      
   def pre_save(self, obj):
      # Save the user that created this object
      obj.owner = self.request.user


class CommandViewSet(viewsets.ReadOnlyModelViewSet):
   queryset = Command.objects.all()
   serializer_class = CommandSerializer


# This view is only used to create new users.
# This code is based on code from: http://stackoverflow.com/questions/16857450/how-to-register-users-in-django-rest-framework
# TODO: this always returns an error when the user is created but it seems to
# work anyway.
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
         token = Token.objects.create(user=user)
         return Response(serialized.data,
	                 status=status.HTTP_201_CREATED,
			 headers={'Token':token})
      else:
         return Response(serialized._errors, status=status.HTTP_400_BAD_REQUEST)
