from rest_framework import permissions


class IsOwner(permissions.BasePermission):
   def has_permission(self, request, view):
      # TODO: may need to override this class to prevent users from seeing
      # things they shouldn't when they call list().
      return True

   def has_object_permission(self, request, view, obj):
      # Only allow owners of an object to view or edit it
      return obj.owner == request.user
