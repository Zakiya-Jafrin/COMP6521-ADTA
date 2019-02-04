package claim;

public class Claim {
	
	private int CNumber;
	private String date;
	private int CID;
	private String CName;
	private String CAddress;
	private String CEmail;
	private int Insured_item;
	private float Amount_Damage;
	private float Amount_paid;
	
	Claim(int cnum, String d, int cid, String name, String address, String email, int item, float damage, float paid)
	{
		this.CNumber = cnum;
		this.date = d;
		this.CID =cid;
		this.CName = name;
		this.CAddress = address;
		this.CEmail = email;
		this.Insured_item = item;
		this.Amount_Damage = damage;
		this.Amount_paid = paid;
	}
	
	public int getCNumber()
	{
		return CNumber;
	}
	
	public String getDate()
	{
		return date;
	}
	
	public int getCID()
	{
		return CID;
	}
	
	public String getName()
	{
		return CName;
	}
	
	public String getAddress()
	{
		return CAddress;
	}
	
	public String getEmail()
	{
		return CEmail;
	}
	
	public int getItem()
	{
		return Insured_item;
	}
	
	public float getAmountDamage()
	{
		return Amount_Damage;
	}
	
	public float getAmountPaid()
	{
		return Amount_paid;
	}
	
	public String getClaim()
	{
		String claimString = Integer.toString(CNumber) + "\t"
							 + date + "\t" 
							 + Integer.toString(CID) + "\t"
							 + CName + "\t"
							 + CAddress + "\t"
							 + CEmail + "\t"
							 + Integer.toString(Insured_item) + "\t"
							 + Float.toString(Amount_Damage) + "\t"
							 + Float.toString(Amount_paid) + "\r\n";
		
		return claimString;
	}

}
