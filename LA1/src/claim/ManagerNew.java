package claim;

import java.io.*;
import java.nio.*;
import java.util.*;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.stream.Collectors;
import java.text.DecimalFormat;

public class ManagerNew {
	
	private static DecimalFormat df2 = new DecimalFormat(".##");
	
	private int cid;
	private double Amt_paid;
	public List<Claim> tuples = new ArrayList<Claim>();
	Map<Integer, Double> u_sublist = new HashMap<Integer, Double>();
	Map<Integer, Double> s_sublist = new HashMap<Integer, Double>();
	public List<Map<Integer, Double>> main_output = new ArrayList<Map<Integer, Double>>();
	public Map<Integer, Double> output = new HashMap<Integer, Double>(); 
	
	public void phase1() throws IOException
	{
		String block = new String();
		RandomAccessFile inputFile = new RandomAccessFile("input1.txt", "r");
		FileChannel inChannel = inputFile.getChannel();
		File outputFile = new File("outPhase1.txt");
		FileOutputStream fos = null;
		fos = new FileOutputStream(outputFile, true);
		
		//Allocating Memory for 252*15 (1 Block)
		ByteBuffer buffer = ByteBuffer.allocate(3780);
		int counter = 1;
		while(inChannel.read(buffer) > 0)
	        {
			 	buffer.flip(); //buffer ready to be read from
			 	String newBlock = "New Block	" + counter + "\r";
			 	System.out.println(newBlock);
			 	block = new String();
			 	//Reading Block
	            for (int i = 0; i < buffer.limit(); i++)
	            {   
	                char charac= (char) buffer.get();
	                block +=charac;               
	            }
	            String[] line = new String[15];
	            line = block.split("\n"); //Splitting block at ever '\n' to get 1 line
	            for(int i = 0; i<line.length; i++)
	            {
	            	cid = Integer.parseInt(line[i].substring(18, 27));
	            	Amt_paid = Double.valueOf(line[i].substring(241, 250)).floatValue();
	            	Claim claim = new Claim(cid, Amt_paid);
	            	tuples.add(claim); //creating list of ojects
	            }
	            u_sublist = groupBy(tuples); //group by CID and add Amount-Paid
            	s_sublist = sortByValue(u_sublist); //Sort resulting map by value
            	tuples = new ArrayList<Claim>(); // clearing list to read next block
            	String output = MaptoString(s_sublist); // block of 15 tuples <cid Amount-paid> grouped and sorted
            	fos.write(output.getBytes());
	            counter += 1;
	           
	            buffer.clear();
	        }
		 //Phase 1 complete
	        inChannel.close();
	        inputFile.close();
	        fos.close();
	}
	
	public void phase2() throws IOException
	{
		System.out.println("Starting phase 2 \n\n");
		String block = new String();
		RandomAccessFile inputFile = new RandomAccessFile("outPhase1.txt", "r"); // read output of phase1
		FileChannel inChannel = inputFile.getChannel();
		File outputFile = new File("outPhase2.txt");
		FileOutputStream fos = null;
		fos = new FileOutputStream(outputFile, true);
		//Allocating memory to read 15 lines(1 block) at a time
		ByteBuffer buffer = ByteBuffer.allocate(20*15);
		int counter = 1;
		while(inChannel.read(buffer) > 0)
	    {
			 	
		 	buffer.flip();
		 	String newBlock = "New Block	" + counter + "\r";
		 	System.out.println(newBlock);
		 	block = new String();
		 	//read block
            for (int i = 0; i < buffer.limit(); i++)
            {
                char charac= (char) buffer.get();
                block +=charac;
            }
            String[] line = new String[15];
            line = block.split("\n"); //Split block aat ever '\n' to get 1 line
            s_sublist = new HashMap<Integer, Double>();
            for(int i = 0; i<line.length; i++)
            {
            	cid = Integer.parseInt(line[i].substring(0,9));
            	Amt_paid = Double.valueOf(line[i].substring(10, line[i].length())).floatValue();
            	s_sublist.put(cid, Amt_paid); //creating map of 15 tuples
            }
            main_output.add(s_sublist); //add map of 15 tuples to a list
            counter += 1;
	           
            buffer.clear();
	     }
		//main_output now is a list of blocks( 15 tuples grouped and sorted)
		 output = mergeSublists(main_output); // merging all blocks in main_output
		 String out = finalString(output);
		 fos.write(out.getBytes()); // write sorted <cid amt-paid> to outphase2.txt
		 //topClients
		 String[] outline = out.split("\n");
		 System.out.println("Top 10 clients");
		 //getting top 10 clients
		 for( int k = 10; k>0; k--)
		 {
			 System.out.println(outline[outline.length-k]);
		 }
		 inChannel.close();
	     inputFile.close();
	     fos.close();
	}
	
	//Taking a list of maps
	//merging consecutive two maps in the list (map0 + map1) (map2 + map3)....
	//recursively merge until we have a lit containing 1 large map
	public Map<Integer, Double> mergeSublists(List<Map<Integer, Double>> sublists)
	{
		System.out.println("Merging Sublists");
		Map<Integer, Double> mergeMap = new HashMap<Integer, Double>();
		List<Map<Integer, Double>> newList = new ArrayList<Map<Integer, Double>>();
		int size = sublists.size();
		
		while(size> 1)
		{
			System.out.println("New Size = " + size);
			int i = 0;
			while(i < size)
			{
				System.out.println("Merging block " + i +" + block " + (i+1) );
				if(i == size-1)
				{
					System.out.println("adding last list");
					newList.add(sublists.get(i));
				}
				else
				{
					Map<Integer, Double> map1 = sublists.get(i);
					Map<Integer, Double> map2 = sublists.get(i+1);
					mergeMap = mergeMaps(map1, map2);
					newList.add(mergeMap);
				}
				i = i+2;
			}
			sublists = new ArrayList<Map<Integer, Double>>();
			for(int j =0; j<newList.size(); j++)
			{
				sublists.add(newList.get(j));
			}
			newList = new ArrayList<Map<Integer, Double>>();
			size = sublists.size();
		}
		return sublists.get(0);
	}
	
	//Take two maps. 
	//Add entries of map1 to a List<Claim>. 
	//Add entries of map2 to same list. 
	//Group the list to get a single map. Sort map by value
	public Map<Integer, Double> mergeMaps(Map<Integer, Double> map1, Map<Integer, Double> map2)
	{
		Map<Integer, Double> newMap = new HashMap<Integer, Double>();
		tuples = new ArrayList<Claim>();
		for(Map.Entry<Integer, Double> entry : map1.entrySet())
		{
			Claim claim = new Claim(entry.getKey(), entry.getValue());
			tuples.add(claim);
		}
		for(Map.Entry<Integer, Double> entry : map2.entrySet())
		{
			Claim claim = new Claim(entry.getKey(), entry.getValue());
			tuples.add(claim);
		}
		//tuples is a list of objects from map1 and map2
		newMap = groupBy(tuples);
		newMap = sortByValue(newMap);
		return newMap;
	}
	
	//Converting a Map of <cid, amt-paid> to a block of string.
	//Note: we maintain that Amount paid has exactly 9 characters with 2 digits or less after decimal point
	public String MaptoString(Map<Integer, Double> map)
	{
		String out = new String();
		for(Map.Entry<Integer, Double> entry : map.entrySet())
		{
			String val = df2.format(entry.getValue());
			if(val.length()<9)
			{
				while(val.length() != 9)
				{
					val += "0";
				}
			}
			if(val.length()>9)
			{
				while(val.length() != 9)
				{
					val = val.substring(0, val.length()-1);
				}
			}
			out += Integer.toString(entry.getKey()) + " " + val + "\n";
	
		}
		return out;
	}
	
	public String finalString(Map<Integer, Double> map)
	{
		String out = new String();
		for(Map.Entry<Integer, Double> entry : map.entrySet())
		{
			String val = df2.format(entry.getValue());
			out += Integer.toString(entry.getKey()) + " " + val + "\n";
		}
		return out;
	}
	
	public Map<Integer, Double> groupBy(List<Claim> sublist)
	{
		Map<Integer, Double> output = sublist.stream().collect(
				Collectors.groupingBy(Claim::getCID, Collectors.summingDouble(Claim::getAmountPaid) ));
		return output;
	}

	public Map<Integer, Double> sortByValue(Map<Integer, Double> unsorted)
	{
		Map<Integer, Double> sortedMap = unsorted.entrySet()
                .stream()
                .sorted((Map.Entry.<Integer, Double>comparingByValue()))
                .collect(Collectors.toMap(Map.Entry::getKey, Map.Entry::getValue, 
                		(e1, e2) -> e1, LinkedHashMap::new));
		
		return sortedMap;
	}
	
	public static void main(String[] args) throws IOException 
	{
		long start = System.currentTimeMillis();
		ManagerNew mg = new ManagerNew();
		mg.phase1();
		mg.phase2();
		long end = System.currentTimeMillis();
		long time = end-start;
		System.out.println("Execution time in milliseconds = " + time);
	}

}
