from rest_framework import permissions


class IsOwner(permissions.BasePermission):
    def has_object_permission(self, request, view, obj):
        # Only allow owners of an object to view or edit it
        return obj.owner == request.user
