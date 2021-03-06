/* Author : Nagios Services team */
/* A simple deamon code which sits on top of the controller service
 * facing south, communicating with the monitoring servers. */

#include <stdlib.h>
#include <unistd.h>
#include <merrors.h>
#include <stdint.h>
#include <stdio.h>
#include <netinet/in.h>

#include <controller.h>
#include <cconnection.h>
#include <lib.h>

char *id;

char *myip;
char *dbip;
char *dbport;
char *tablename;

int main(int argc, char *argv[]){
	int opt;
	char *ip, *port;
	char *dip, *dport;
	uint8_t fg = 0;	

	while((opt = getopt(argc, argv, "fc:p:hi:d:e:")) != -1){
		switch(opt) {
			case 'f':
				/* Run this as a foreground application. */
				fg = 1;
				break;
			case 'c':
				/* IP address to bind controller daemon to.
				 * This option is provided rather than just bind 
				 * at 127.0.0.1, as the machine might have multiple
				 * interfaces we wish to bind at. */
				ip = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case 'i':
				id = optarg;
				break;
			case 'd':
				/* Cassandra deamon IP address. */
				dip = optarg;
				break;
			case 'e':
				/* Cassandra deamon port number. */
				dport = optarg;
				break;
			case 'h':

			case '?':

			default:
				/* Print the usage message. */
				return -1;
		}
	}
	
	/* Start the daemon service. */
	controller_Start(fg, ip, port, dip, dport);	
	
	return 0;
}

/*
 * Starts up a daemon and makes call to the the listen routines.
 * arg0 - Run as a foreground operation.
 * arg1 - IP address of the control server.
 * arg2 - Port number at the control server to connect to.
 */
static int controller_Start(uint8_t fg, char *ip, char *port,
								char *dip, char *dport){
	
	int pid = -1; 

	if(ip == NULL || port == NULL || dip == NULL || 
											dport == NULL)
		return ERR;

	if(fg)
		goto child;

	if((pid = fork()) < 0){
		return ERR; 
	}

	if(pid != 0){
		/* Print some messages on the stdout and die. */
		fprintf(stdout, "Controller daemon started with PID : %d\n", pid);
	}

child:
	myip = ip;
	dbip = dip;
	dbport = dport;
	tablename = "control_servers";


	/* Establish the connection to the control server. */
	controller_StartComm(ip, port);

	return 0;
}

