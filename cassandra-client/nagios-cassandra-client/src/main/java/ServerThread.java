import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;


public class ServerThread extends Thread {
	private Socket socket = null;
	String userName;
	String password;
	String cassIp;
	int cassPort;

    public ServerThread(Socket socket, String userName, String password, String cassIp, int cassPort) {
        super("ServerThread");
        this.socket = socket;
        this.userName = userName;
    	this.password = password;
    	this.cassIp = cassIp;
    	this.cassPort = cassPort;
    }
    
    public void run() {
        try (
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(
                new InputStreamReader(
                    socket.getInputStream()));
        ) {
            String inputLine, outputLine;
    		CassandraClient cassClient = new CassandraClient();
    		cassClient.connect(cassIp, cassPort, userName, password);
            Protocol protocol = new Protocol(cassClient);

            while ((inputLine = in.readLine()) != null) {
                outputLine = protocol.processInput(inputLine);
                out.println(outputLine);
                if (outputLine.equals("Bye"))
                    break;
            }
            
            cassClient.close();
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
