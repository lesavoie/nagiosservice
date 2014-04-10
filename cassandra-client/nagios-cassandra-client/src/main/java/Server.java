import java.io.IOException;
import java.net.ServerSocket;


public class Server {
	final static int portNumber = 6666;
    
	public static void main(String[] args) {
	    try (ServerSocket serverSocket = new ServerSocket(portNumber)) { 
	        while (true) {
	            new ServerThread(serverSocket.accept()).start();
	        }
	    } catch (IOException e) {
	        System.err.println("Could not listen on port " + portNumber);
	        System.exit(-1);
	    }
	}
}
