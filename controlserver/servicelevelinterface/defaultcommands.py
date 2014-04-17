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
      
      host_alive = Command()
      host_alive.command_name = 'check_host_alive'
      host_alive.description = 'Determines if a given host is up or down.  This command does not accept a critical or warning level.'
      host_alive.save()
   
      check_cpu = Command()
      check_cpu.command_name = 'check_cpu'
      check_cpu.description = 'Checks the CPU utilization of the given host.  Critical and warning levels are specified in the format LOAD1,LOAD5,LOAD15, where LOADX is the average CPU utilization over the last X minutes.  Note: in order to use this command, NRPE must be installed on the monitored host.'
      check_cpu.save()
      
      check_http = Command()
      check_http.command_name = 'check_http'
      check_http.description = 'Checks for a live HTTP interface on the given host.  Critical and warning levels represent response times in seconds that should not be exceeded.'
      check_http.save()
