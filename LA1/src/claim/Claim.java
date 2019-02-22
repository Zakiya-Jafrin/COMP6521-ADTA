package claim;

public class Claim {
	
	private int CID;
	private double Amount_paid;
	
	Claim(int cid, double paid)
	{
		this.CID =cid;
		this.Amount_paid = paid;
	}
	
	public int getCID()
	{
		return CID;
	}
	public double getAmountPaid()
	{
		return Amount_paid;
	}
}
