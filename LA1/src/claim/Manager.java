package claim;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class Manager {
	
	private FileInputStream input;
	private BufferedReader buffer;
	private int CNum;
	private String date;
	private int cid;
	private String name;
	private String address;
	private String email;
	private int item;
	private float Amt_Damage;
	private float Amt_paid;
	public List<Claim> tuples;
	
	public void readInput()throws Exception
	{
		try
		{
			buffer = new BufferedReader(new FileReader("input1.txt"));
		}catch(Exception e)
		{
			e.printStackTrace();
		}
		tuples = new ArrayList<Claim>();
		String line = null;
		//Read one line. Split the line where we find a tab '\t' and store the attributes in Claim object
		while((line = buffer.readLine())!= null)
		{
			//String[] row = line.split("\t");
			CNum = Integer.parseInt(line.substring(0, 8));
			date = line.substring(8, 18);
			cid = Integer.parseInt(line.substring(18, 27));
			name = line.substring(27, 52);
			address = line.substring(52,202);
			email = line.substring(202,230);
			item = Integer.parseInt(line.substring(230, 232));
			Amt_Damage = Float.valueOf(line.substring(232, 241)).floatValue();
			Amt_paid = Float.valueOf(line.substring(241, 250)).floatValue();
			Claim claim = new Claim(CNum, date, cid, name, address, email, item, Amt_Damage, Amt_paid);
			tuples.add(claim);
		}
		
		System.out.println("Read Complete\n");
		
		FileWriter writer = new FileWriter("output.txt"); 
		for(Claim cl:tuples)
		{
			writer.write(cl.getClaim());
			writer.write("\n");
		}
		
		writer.close();
		System.out.println("Write Complete\n");
	}
	
	public static void main(String args[])
	{
		Manager mg = new Manager();
		try
		{
			mg.readInput();
		} catch (Exception e) 
		{
			e.printStackTrace();
		}
	}
}
