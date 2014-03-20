/* Author : Nagios Service Team.
 * The ultimate purpose of this file is to generate and
 * maintain log files to document various function performed
 * by this service, for admins and debug reasonds.
 */

#include <stdio.h>

#include <log.h>

void inline mLog(char *tag, char *log){

	fprintf(stderr, "%s :: %s\n", tag, log);

	return;
}
