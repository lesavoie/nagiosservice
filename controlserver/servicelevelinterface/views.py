from rest_framework import viewsets
from servicelevelinterface.models import Monitor, Contact, Command
from servicelevelinterface.serializers import MonitorSerializer, ContactSerializer, CommandSerializer


class MonitorViewSet(viewsets.ModelViewSet):
   queryset = Monitor.objects.all()
   serializer_class = MonitorSerializer


class ContactViewSet(viewsets.ModelViewSet):
   queryset = Contact.objects.all()
   serializer_class = ContactSerializer
   
   # The procedures below are possible places that we could insert hooks to
   # siphon off data into our own storage.
   # Another option is decopling this completely and just putting triggers on
   # the database.
   def list(self, request):
      print 'List'
      return super(ContactViewSet, self).list(request)

   def create(self, request):
      print 'Create'
      return super(ContactViewSet, self).create(request)

   def retrieve(self, request, pk=None):
      print 'Retrieve'
      return super(ContactViewSet, self).retrieve(request, pk)

   def update(self, request, pk=None):
      print 'Update'
      return super(ContactViewSet, self).update(request, pk)

   def partial_update(self, request, pk=None):
      print 'Partial update'
      return super(ContactViewSet, self).partial_update(request, pk)

   def destroy(self, request, pk=None):
      print 'Destroy'
      return super(ContactViewSet, self).destroy(request, pk)
      
   def post_save(self, obj, created=False):
      print "Post Save"
      return super(ContactViewSet, self).post_save(obj, created)
      
   def post_delete(self, obj):
      print "Post Delete"
      return super(ContactViewSet, self).post_delete(obj)


class CommandViewSet(viewsets.ModelViewSet):
   queryset = Command.objects.all()
   serializer_class = CommandSerializer
