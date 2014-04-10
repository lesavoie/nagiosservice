from servicelevelinterface.models import Command

# This function generates a set of default commands.
def generate_default_commands():

   # Only generate the commands once
   if Command.objects.count() == 0:
   
      '''
      Commands that could be added:
      check_ssh
      check_hda1 (free space on hda1)
      check_total_procs
      check_zombie_procs
      check_dns
      check_smtp (or other forms of email)
      check_tcp
      check_mysql
      '''
      
      # Check host alive command
      host_alive = Command()
      host_alive.command_name = 'check_host_alive'
      host_alive.command_line = '/usr/lib/nagios/plugins/check_ping -H $HOSTADDRESS$ -w 5000,100% -c 5000,100% -p 1'
      host_alive.save()
   
      # Check CPU command
      check_cpu = Command()
      check_cpu.command_name = 'check_cpu'
      check_cpu.command_line = 'check_nrpe -H $HOSTADDRESS$ -c check_load -a $ARG2$'
      check_cpu.save()
      
      # Check HTTP command
      check_http = Command()
      check_http.command_name = 'check_http'
      check_http.command_line = 'check_http -I $HOSTADDRESS$ -p $ARG1$ -w $ARG2$ -c $ARG3$'
      check_http.save()
