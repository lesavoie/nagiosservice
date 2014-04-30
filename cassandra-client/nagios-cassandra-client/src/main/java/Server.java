import gnu.getopt.Getopt;

import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;


public class Server {
    
	public static void main(String[] args) {
		String userName, password, cassIp;
		userName = password = "";
		cassIp = "127.0.0.1";
		int cassPort = -1;
		
		Getopt g = new Getopt("nagios-cassandra-client", args, "l:i:h:p:");
		int c;
		while ((c = g.getopt()) != -1) {
			switch(c) {
			case 'l':
				userName = g.getOptarg();
				break;
			case 'i':
				password = g.getOptarg();
				break;
			case 'h':
				cassIp = g.getOptarg();
				break;
			case 'p':
				cassPort = Integer.parseInt(g.getOptarg());
				break;
			case '?':
		        System.err.println("Usage: java -jar nagios-cassandra-client.jar [-l username] "
		        		+ "[-i password] [-h cassandra_ip] [-p cassandra_port] <listen ip address> <port number>");
		        System.exit(1);				
			default:
				System.out.print("getopt() returned " + c + "\n");
			}
		}
		
	    if ((args.length - g.getOptind()) < 2) {
	        System.err.println("Usage: java -jar nagios-cassandra-client.jar [-l username] "
	        		+ "[-i password] [-h cassandra_ip] [-p cassandra_port] <listen ip address> <port number>");
	        System.exit(1);
	    }
	    
	    System.out.println(args[g.getOptind()] + ":" + args[g.getOptind() + 1]);

	    int portNumber = Integer.parseInt(args[g.getOptind() + 1]);
	    try (ServerSocket serverSocket = new ServerSocket(portNumber, 0, InetAddress.getByName(args[g.getOptind()]))) { 
	        while (true) {
	            new ServerThread(serverSocket.accept(), userName, password, cassIp, cassPort).start();
	        }
	    } catch (IOException e) {
	        System.err.println("Could not listen on port " + portNumber);
	        System.exit(-1);
	    }
	}
}
