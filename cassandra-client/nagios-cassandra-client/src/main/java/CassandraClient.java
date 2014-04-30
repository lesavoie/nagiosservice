import com.datastax.driver.core.Cluster;
import com.datastax.driver.core.Host;
import com.datastax.driver.core.Metadata;
import com.datastax.driver.core.ResultSet;
import com.datastax.driver.core.Row;
import com.datastax.driver.core.Session;
import com.datastax.driver.core.exceptions.NoHostAvailableException;
import com.datastax.driver.core.exceptions.QueryExecutionException;
import com.datastax.driver.core.exceptions.QueryValidationException;

public class CassandraClient {

	private Cluster cluster;
	private Session session;

	public void connect(String node, int port, String userName, String password) {
		Cluster.Builder clusterBuilder = Cluster.builder();
		clusterBuilder.addContactPoint(node);
		
		if (port != -1) 
			clusterBuilder.withPort(port);
		
		if (!userName.isEmpty() && !password.isEmpty())
			clusterBuilder.withCredentials(userName, password);
		
		cluster = clusterBuilder.build();
		Metadata metadata = cluster.getMetadata();
		System.out.printf("Connected to cluster: %s\n", metadata.getClusterName());
		for (Host host : metadata.getAllHosts()) {
			System.out.printf("Datacenter: %s; Host: %s; Rack: %s\n",
					host.getDatacenter(), host.getAddress(), host.getRack());
		}
		
		session = cluster.connect();
	}
	
	public String getServer(String openid) {
		ResultSet results = session.execute(
				"SELECT * " +
				"FROM nagioscontrol.control_servers " +
				"WHERE openid = '" + openid +
				"';");
		
		Row result = results.one();
		if (result == null || result.isNull(1)) {
			return "";
		}
		else {
			return result.getString(1);
		}
	}
	
	public boolean putServer(String openid, String ip_addr) {
		try {
			session.execute("INSERT INTO nagioscontrol.control_servers " +
					"(openid, ip_addr)" +
					"VALUES ('" + openid + "', '" + ip_addr + "');");
		}
		catch (NoHostAvailableException e1) {
			e1.printStackTrace();
			return false;
		}
		catch (QueryExecutionException e2) {
			e2.printStackTrace();
			return false;
		}
		catch (QueryValidationException e3) {
			e3.printStackTrace();
			return false;
		}
		
		return true;
	}
	
	public boolean updateServer(String openid, String ip_addr) {
		try {
			session.execute("UPDATE nagioscontrol.control_servers " +
					"SET ip_addr = '" + ip_addr + "' " +
					"WHERE openid = '" + openid + "';");
		}
		catch (NoHostAvailableException e1) {
			e1.printStackTrace();
			return false;
		}
		catch (QueryExecutionException e2) {
			e2.printStackTrace();
			return false;
		}
		catch (QueryValidationException e3) {
			e3.printStackTrace();
			return false;
		}
		
		return true;
	}
	
	public boolean deleteServer(String openid) {
		try {
			session.execute("DELETE FROM nagioscontrol.control_servers " +
					"WHERE openid = '" + openid + "';");
		}
		catch (NoHostAvailableException e1) {
			e1.printStackTrace();
			return false;
		}
		catch (QueryExecutionException e2) {
			e2.printStackTrace();
			return false;
		}
		catch (QueryValidationException e3) {
			e3.printStackTrace();
			return false;
		}
		
		return true;		
	}
	
	public boolean initKeyspace() {
		try {
			session.execute(
					"CREATE KEYSPACE nagioscontrol " +
					"WITH REPLICATION = {'class' : 'SimpleStrategy', 'replication_factor' : 1}; ");
			
			session.execute("USE nagioscontrol; ");
			
			session.execute("CREATE TABLE control_servers( " +
						"openid  varchar, " +
						"ip_addr varchar, " +
						"PRIMARY KEY (openid) " +
					");");
		}
		catch (NoHostAvailableException e1) {
			e1.printStackTrace();
			return false;
		}
		catch (QueryExecutionException e2) {
			e2.printStackTrace();
			return false;
		}
		catch (QueryValidationException e3) {
			e3.printStackTrace();
			return false;
		}
		
		return true;			
	}
	
	public void close() {
		cluster.close();;
	}
}
