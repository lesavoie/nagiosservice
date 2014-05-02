#! /bin/bash

# Author : Nagios Service Team
# This is an extremely basic script that can be used to
# deploy a controller.

echo "Installing dependencies ...."
sudo apt-get -y install make gcc git python-pip openjdk-7-jre openjdk-7-jdk
sudo pip install django
sudo pip install djangorestframework

echo "Switching java version to java 7"
id='update-java-alternatives -l | grep 1.7 | cut -d " " -f 1'
update-java-alternatives -l $id

echo "Downloading latest and stable source code ...."
git clone https://github.com/lesavoie/nagiosservice.git

echo "Configuring / installing dependencies needed by north-south bridge ..."
cd nagiosservice/dep/libcfu-0.03
./configure
cd src
make
sudo make install
cd ../../../

echo "Compiling north-south bridge ..."
cd nagiosservice/north_south/src
make
sudo make install
cd ../../../

echo "Configuring django interface ..."
cd nagiosservice/controlserver
python manage.py syncdb
cd ../../

echo "Enter Cassandra server IP : "
read cassandra_ip

echo "Starting cassandra-client ..."
cd nagiosservice/cassandra-client
nohup java -jar nagios-cassandra-client.jar -l cassandra -i cassandra -h $cassandra_ip -p 9160 127.0.0.1 5777 > /dev/null 2> /dev/null < dev/null &
cd ../../

echo "Starting controller daemon ..."
ip='ifconfig eth0 | grep "inet addr" | cut -d ":" -f 2 | cut -d " " -f 1'
port=5666
ns_controller -c $ip -p $port -d 127.0.0.1 -e 5777 -i $ip


echo "Starting controller interface ..."
cd nagiosservice/controlserver
python manage.py runserver
