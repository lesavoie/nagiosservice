#!/usr/bin/python
import os
import sys
import json

command_template="define command{\n\tcommand_name\t%s\n\tcommand_line\t%s %s\n\t}"
contact_template="define contact{\n\tcontact_name\t%s\n\tuse\tgeneric-contact\n\talias\t%s\n\temail\t%s\n\t}"

if (len(sys.argv) != 2) :
    sys.exit("Usage: mapper.py [commands|contacts]")

if (sys.argv[1] == 'commands') :
    json_str = sys.stdin.read()
    json_array = json.loads(json_str)
    print(command_template % (json_array['command_name'], json_array['binary'], json_array['command_line_parameters']))

if (sys.argv[1] == 'contacts') :
    json_str = sys.stdin.read()
    json_array = json.loads(json_str)
    print(contact_template % (json_array['contact_name'], json_array['human_readable_name'], json_array['email_address']))
