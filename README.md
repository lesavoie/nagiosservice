Nagios Service (CS652 project)
==============================

To set up your environment (assuming you already have python and pip installed):

```
pip install django
pip install djangorestframework
cd controlserver
python manage.py syncdb
```

To run the development server:

```
python manage.py runserver
```

You should now be able to connect to http://localhost:8000/.

You also need to create a directory called /home/media (to store binaries uploaded by users).

