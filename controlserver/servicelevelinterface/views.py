from django.contrib.auth.models import User
from rest_framework import mixins
from rest_framework import permissions
from rest_framework.authtoken.models import Token
from rest_framework import viewsets
from rest_framework.response import Response
from rest_framework import status
from servicelevelinterface.models import Monitor, Contact, Command
from servicelevelinterface.permissions import IsOwner
from servicelevelinterface.serializers import MonitorSerializer, ContactSerializer, CommandSerializer, UserSerializer, CreateUserSerializer


# TODO: the current permissions aren't quite working - sometimes users can view
# things they shouldn't be able to view.


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


# This is for viewing the list of users.  It should probably be taken out of
# production but is useful for debugging.
class UserViewSet(viewsets.ReadOnlyModelViewSet):
   queryset = User.objects.all()
   serializer_class = UserSerializer
	 

# This view is only used to create new users.
# This code is based on code from: http://stackoverflow.com/questions/16857450/how-to-register-users-in-django-rest-framework
# TODO: I think users can be created even when there is a problem.
class CreateUserViewSet(mixins.CreateModelMixin,
                        viewsets.GenericViewSet):
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
