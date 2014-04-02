from django.conf.urls import patterns, url, include
from rest_framework.routers import DefaultRouter
from servicelevelinterface import views

# Create a router and register our viewsets with it.
router = DefaultRouter()
router.register(r'monitors', views.MonitorViewSet)
router.register(r'contacts', views.ContactViewSet)
router.register(r'commands', views.CommandViewSet)
router.register(r'users', views.UserViewSet)
router.register(r'register', views.CreateUserViewSet)

urlpatterns = patterns('',
    url(r'^', include(router.urls)),
    
    # Login page
    url(r'^auth/', include('rest_framework.urls', namespace='rest_framework')),
    
    # Allows retrieval of tokens via a POST that specifies the user name and
    # password.
    url(r'^token/', 'rest_framework.authtoken.views.obtain_auth_token'),
)
