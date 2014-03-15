from rest_framework import viewsets
from servicelevelinterface.models import Contact
from servicelevelinterface.serializers import ContactSerializer

class ContactViewSet(viewsets.ModelViewSet):
   queryset = Contact.objects.all()
   serializer_class = ContactSerializer

