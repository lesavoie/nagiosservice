from django.conf.urls import patterns, url, include
from servicelevelinterface import views
from rest_framework.routers import DefaultRouter

# Create a router and register our viewsets with it.
router = DefaultRouter()
router.register(r'monitors', views.MonitorViewSet)
router.register(r'contacts', views.ContactViewSet)
router.register(r'commands', views.CommandViewSet)

urlpatterns = patterns('',
    url(r'^', include(router.urls)),
)
