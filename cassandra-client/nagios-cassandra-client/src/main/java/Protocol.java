
public class Protocol {
	
	CassandraClient cassClient;
	final String delim = ":";
	
	public Protocol(CassandraClient cassClient) {
		this.cassClient = cassClient;
	}
	
	public String processGet(String[] values) {
		// Expects two additional arguments
		if (values.length != 3) {
			return "error" + delim + values[0] + ":incorrect_usage";
		}
		
		if (values[1].equals("control_servers")) {
			return "get" + delim + cassClient.getServer(values[2]);	
		}
		else {
			return "error" + delim + "unknown_table" + delim + values[1];
		}
	}
	
	public String processPut(String[] values) {
		// Expects three additional arguments
		if (values.length != 4) {
			return "error" + delim + values[0] + ":incorrect_usage";
		}			
		
		if (values[1].equals("control_servers")) {
			boolean status = cassClient.putServer(values[2], values[3]);
			return "put" + delim + Boolean.toString(status);			
		}
		else {
			return "error" + delim + values[0] + delim + "unknown_table" + delim + values[1];
		}
	}
	
	public String processUpdate(String[] values) {
		// Expects three additional arguments
		if (values.length != 4) {
			return "error" + delim + values[0] + ":incorrect_usage";
		}			
		
		if (values[1].equals("control_servers")) {
			boolean status = cassClient.updateServer(values[2], values[3]);
			return "update" + delim + Boolean.toString(status);			
		}
		else {
			return "error" + delim + values[0] + delim + "unknown_table" + delim + values[1];
		}
	}	
	
	public String processDelete(String[] values) {
		// Expects two additional arguments
		if (values.length != 3) {
			return "error" + delim + values[0] + ":incorrect_usage";
		}			
		
		if (values[1].equals("control_servers")) {
			boolean status = cassClient.deleteServer(values[2]);
			return "delete" + delim + Boolean.toString(status);			
		}
		else {
			return "error" + delim + values[0] + delim + "unknown_table" + delim + values[1];
		}
	}		
	
	public String processInput(String input) {
		String[] values = input.split(delim);
		
		// Parse command type
		if (values[0].equals("get")) {
			return processGet(values);
		}
		else if (values[0].equals("put")) {
			return processPut(values);
		}
		else if (values[0].equals("update")) {
			return processUpdate(values);	
		}
		else if (values[0].equals("delete")) {
			return processDelete(values);				
		}
		else if (values[0].equals("init")) {
			return values[0] + delim + Boolean.toString(cassClient.initKeyspace());
		}
		// Undefined command
		else {
			return "error" + delim + "unknown_op" + delim + values[0];
		}
	}
}
