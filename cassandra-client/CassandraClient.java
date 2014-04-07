import com.datastax.driver.core.Cluster;
import com.datastax.driver.core.Host;
import com.datastax.driver.core.Metadata;
import com.datastax.driver.core.ResultSet;
import com.datastax.driver.core.Row;
import com.datastax.driver.core.Session;

public class CassandraClient {

	private Cluster cluster;
	private Session session;

	public void connect(String node) {
		cluster = Cluster.builder().addContactPoint(node).build();
		Metadata metadata = cluster.getMetadata();
		System.out.printf("Connected to cluster: %s\n", metadata.getClusterName());
		for (Host host : metadata.getAllHosts()) {
			System.out.printf("Datacenter: %s; Host: %s; Rack: %s\n",
					host.getDatacenter(), host.getAddress(), host.getRack());
		}
		
		session = cluster.connect();
	}
	
	public String queryServerIp(String openid) {
		ResultSet results = session.execute(
				"SELECT * " +
				"FROM nagioscontrol.monitor_servers " +
				"WHERE openid = '" + openid +
				"';");
		
		Row result = results.one();
		if (result.isNull(1)) {
			return "";
		}
		else {
			return result.getString(1);
		}
	}
	
	public String queryUserToken(String openid) {
		ResultSet results = session.execute(
				"SELECT * " +
				"FROM nagioscontrol.users " +
				"WHERE openid = '" + openid +
				"';");
		
		Row result = results.one();
		if (result.isNull(1)) {
			return "";
		}
		else {
			return result.getString(1);
		}
	}	
	
	public boolean insertUser(String openid, String openid_token) {

			session.execute("INSERT INTO nagioscontrol.users " +
					"(openid, openid_token)" +
					"VALUES (" + openid + ", " + openid_token + ");");


		
		return true;
	}
	
	public boolean insertMonitorServer(String openid, String ip_addr) {
	
			// TODO check if user exists?
			
			session.execute("INSERT INTO nagioscontrol.monitor_servers " +
					"(openid, ip_addr) " +
					"VALUES ('" + openid + "', '" + ip_addr + "');");

		
		return true;
	}
	
	public void close() {
		cluster.close();;
	}
	
	public static void main(String[] args) {
		CassandraClient client = new CassandraClient();
		client.connect("127.0.0.1");
		
		//client.insertUser("admin", "1234567889");
		client.insertMonitorServer("admin", "127.0.0.1");
		System.out.println(client.queryUserToken("admin"));
		System.out.println(client.queryServerIp("admin"));
		client.close();
	}

}
