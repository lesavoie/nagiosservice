#!/bin/bash
# NRPE install script for Nagios service.  This script is based on the
# instructions found in http://nagios.sourceforge.net/docs/nrpe/NRPE.pdf.

# Note: this script will create a user called "nagios".  If such a user already exists, it
# assumes that the nagios user was created for NRPE.

# TODO: add statements that describe what is going on

if [[ $EUID -ne 0 ]]; then
   echo 'This script must be run as root'
   exit 1
fi

USERNAME=nagios

# Set up the nagios user
echo '-----------------------------------------------------------------------'
echo 'Creating Nagios user...'
echo '-----------------------------------------------------------------------'
useradd $USERNAME
if [[ $? -eq 0 ]]; then
   # Only set the password if the user was created successfully
   echo "A user called $USERNAME has been created for NRPE.  Please select a password"
   echo "for this user."
   passwd $USERNAME
fi

echo '-----------------------------------------------------------------------'
echo 'Installing dependencies...'
echo '-----------------------------------------------------------------------'
apt-get install gcc make libssl-dev libssl0.9.8 xinetd

echo '-----------------------------------------------------------------------'
echo 'Installing nagios plugins...'
echo '-----------------------------------------------------------------------'
tar xvf nagios-plugins-*.tar.gz
cd nagios-plugins-*

# Compile and install the plugins
./configure
make
make install
cd ..

# Fix permissions on the plugins directory and the plugins
chown $USERNAME.$USERNAME /usr/local/nagios
chown -R $USERNAME.$USERNAME /usr/local/nagios/libexec

echo '-----------------------------------------------------------------------'
echo 'Installing NRPE...'
echo '-----------------------------------------------------------------------'
tar xvf nrpe-*.tar.gz
cd nrpe-*

# Compile and install NRPE
./configure --with-ssl=/usr/bin/openssl --with-ssl-lib=/usr/lib/x86_64-linux-gnu
make all
make install-plugin
make install-daemon
make install-daemon-config
make install-xinetd
cd ..

# Add the appropriate entry to /etc/services
echo 'nrpe		5666/tcp			# NRPE' >> /etc/services

# Restart xinetd
service xinetd restart

# Create a firewall rule for NRPE
#iptables -I RH-Firewall-1-INPUT -p tcp -m tcp -dport 5666 -j ACCEPT
#service iptables save

# Make sure the NRPE daemon is running
# TODO: add code to test the output.  It should be something like:
#tcp	0	0 *:nrpe *:*	LISTEN
#netstat -at | grep nrpe

# Perform a functional test of NRPE
# TODO: add some checks (this should return the version of NRPE running)
#/usr/local/nagios/libexec/check_nrpe -H localhost

# The last step is manual for now.
echo '-----------------------------------------------------------------------'
echo "Please add your monitoring server's IP address to the file"
echo "/etc/xinetd.d/nrpe at the end of the line starting with"
echo "'only_from = 127.0.0.1'"
echo '-----------------------------------------------------------------------'
