from servicelevelinterface.models import Command

# This function generates a set of default commands.
def generate_default_commands():
   if Command.objects.count() == 0:
      # Check CPU command
      check_cpu = Command()
      check_cpu.command_name = 'check_cpu'
      check_cpu.command_line_parameters = ''
      check_cpu.save()
