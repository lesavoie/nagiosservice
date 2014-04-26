/* Author : Nagios Services team */
/* A simple deamon code which sits on top of the monitor service
 * facing north, communicating with the control servers. */

#include <stdlib.h>
#include <unistd.h>
#include <merrors.h>
#include <stdint.h>
#include <stdio.h>
#include <netinet/in.h>

#include <monitor.h>
#include <mconnection.h>

char *id;

char *myip;
char *dbip;
char *dbport;
char *tablename;

int main(int argc, char *argv[]){
	int opt;
	char *ip, *port;
	uint8_t fg = 0;	

	while((opt = getopt(argc, argv, "fc:p:hi:")) != -1){
		switch(opt) {
			case 'f':
				/* Run this as a foreground application. */
				fg = 1;
				break;
			case 'c':
				/* IP address of the control server to connect to. */
				ip = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case 'i':
				id = optarg;
				break;
			case 'h':

			case '?':

			default:
				/* Print the usage message. */
				return -1;
		}
	}

	/* Start the daemon service. */
	monitor_Start(fg, ip, port);	
	
	return 0;
}

/*
 * Starts up a daemon and makes call to the the connection routines.
 * arg0 - Run as a foreground operation.
 * arg1 - IP address of the control server.
 * arg2 - Port number at the control server to connect to.
 */
static int monitor_Start(uint8_t fg, char *ip, char *port){
	
	int pid = -1; 

	if(ip == NULL || port == NULL)
		return ERR;

	if(fg)
		goto child;

	if((pid = fork()) < 0){
		return ERR; 
	}

	if(pid != 0){
		/* Print some messages on the stdout and die. */
		fprintf(stdout, "Monitor daemon started with PID : %d\n", pid);
	}

child:
	/* Establish the connection to the control server. */
	monitor_StartComm(ip, port);

	return 0;
}

