import java.io.*; 
import java.util.*; 

class Buddy { 

	// Inner class to store lower 
	// and upper bounds of the allocated memory 
	class Pair { 
		
		int lb, ub; 
		Pair(int a, int b) 
		{ 
			lb = a; 
			ub = b; 
		} 
	} 

	// Size of main memory 
	int size; 

	// Array to track all 
	// the free nodes of various sizes 
	ArrayList<Pair> arr[]; 
	
	// Hashmap to store the starting 
	// address and size of allocated segment 
	// Key is starting address, size is value 
	HashMap<Integer, Integer> hm; 
	
	// Else compiler will give warning 
	// about generic array creation 
	@SuppressWarnings("unchecked") 
	Buddy(int s) 
	{ 
		
		size = s; 
		hm = new HashMap<>(); 
		
		// Gives us all possible powers of 2 
		int x = (int)Math.ceil(Math.log(s) / Math.log(2)); 

		// One extra element is added 
		// to simplify arithmetic calculations 
		arr = new ArrayList[x + 1]; 

		for (int i = 0; i <= x; i++) 
			arr[i] = new ArrayList<>(); 

		// Initially, only the largest block is free 
		// and hence is on the free list 
		arr[x].add(new Pair(0, size - 1)); 
	} 
	
	void allocate(int s) 
	{ 

		// Calculate which free list to search to get the 
		// smallest block large enough to fit the request 
		int x = (int)Math.ceil(Math.log(s) / Math.log(2)); 

		int i; 
		Pair temp = null; 

		// We already have such a block 
		if (arr[x].size() > 0) { 

			// Remove from free list 
			// as it will be allocated now 
			temp = (Pair)arr[x].remove(0); 
			System.out.println("Memory from " + temp.lb 
							+ " to " + temp.ub + " allocated"); 
							
			// Store in HashMap 
			hm.put(temp.lb, temp.ub - temp.lb + 1); 
			return; 
		} 

		// If not, search for a larger block 
		for (i = x + 1; i < arr.length; i++) { 

			if (arr[i].size() == 0) 
				continue; 

			// Found a larger block, so break 
			break; 
		} 

		// This would be true if no such block was found 
		// and array was exhausted 
		if (i == arr.length) { 
			
			System.out.println("Sorry, failed to allocate memory"); 
			return; 
		} 

		// Remove the first block 
		temp = (Pair)arr[i].remove(0); 

		i--; 

		// Traverse down the list 
		for (; i >= x; i--) { 

			// Divide the block in two halves 
			// lower index to half-1 
			Pair newPair = new Pair(temp.lb, temp.lb 
									+ (temp.ub - temp.lb) / 2); 

			// half to upper index 
			Pair newPair2 = new Pair(temp.lb 
									+ (temp.ub - temp.lb + 1) / 2, 
									temp.ub); 

			// Add them to next list 
			// which is tracking blocks of smaller size 
			arr[i].add(newPair); 
			arr[i].add(newPair2); 

			// Remove a block to continue the downward pass 
			temp = (Pair)arr[i].remove(0); 
		} 

		// Finally inform the user 
		// of the allocated location in memory 
		System.out.println("Memory from " + temp.lb 
						+ " to " + temp.ub + " allocated"); 

		// Store in HashMap 
		hm.put(temp.lb, temp.ub - temp.lb + 1); 
	} 
	void deallocate(int s) 
	{ 
		// Invalid reference, as this was never allocated 
		if (!hm.containsKey(s)) { 
			System.out.println("Sorry, invalid free request"); 
			return; 
		} 

		// Get the list which will track free blocks 
		// of this size 
		int x = (int)Math.ceil(Math.log(hm.get(s)) 
										/ Math.log(2)); 
		int i, buddyNumber, buddyAddress; 

		// Add it to the free list 
		arr[x].add(new Pair(s, s + (int)Math.pow(2, x) - 1)); 
		System.out.println("Memory block from " + s + " to "
						+ (s + (int)Math.pow(2, x) - 1) + " freed"); 


		// Calculate it's buddy number and buddyAddress. The 
		// base address is implicitly 0 in this program, so no 
		// subtraction is necessary for calculating buddyNumber 
		buddyNumber = s / hm.get(s); 
	
		if (buddyNumber % 2 != 0) { 
			buddyAddress = s - (int)Math.pow(2, x); 
		} 
		
		else { 
			buddyAddress = s + (int)Math.pow(2, x); 
		} 
		
		
		// Search in the free list for buddy 
		for (i = 0; i < arr[x].size(); i++) { 
			
			
			// This indicates the buddy is also free 
			if (arr[x].get(i).lb == buddyAddress) { 
				
				// Buddy is the block after block 
				// with this base address 
				if (buddyNumber % 2 == 0) { 
					
					// Add to appropriate free list 
					arr[x + 1].add(new Pair(s, s 
								+ 2 * ((int)Math.pow(2, x)) - 1)); 
					System.out.println("Coalescing of blocks starting at "
											+ s + " and "
											+ buddyAddress + " was done"); 
				} 
				
				// Buddy is the block before block 
				// with this base address 
				else { 
					
					// Add to appropriate free list 
					arr[x + 1].add(new Pair(buddyAddress, 
									buddyAddress + 2 * ((int)Math.pow(2, x)) 
																		- 1)); 
					System.out.println("Coalescing of blocks starting at "
												+ buddyAddress + " and "
												+ s + " was done"); 
				} 

				// Remove the individual segements 
				// as they have coalesced 
				arr[x].remove(i); 
				arr[x].remove(arr[x].size() - 1); 
				break; 
			} 
		} 

		// Remove entry from HashMap 
		hm.remove(s); 
	} 
	
	public static void main(String args[]) throws IOException 
	{ 
		int initialMemory = 0, type = -1, val = 0; 
		
		// Uncomment below section for interactive I/O 
		/*Scanner sc=new Scanner(System.in); 
		initialMemory = sc.nextInt(); 
		Buddy obj=new Buddy(initialMemory); 
		while(true) 
		{ 
			type = sc.nextInt(); 
			if(type==-1) 
			break; 
			else if(type==1) 
			{ 
				val=sc.nextInt(); 
				obj.allocate(val); 
			} 
			else 
			{ 
				val=sc.nextInt(); 
				obj.deallocate(val); 
			} 
		}*/
		
		initialMemory = 128; 
		Buddy obj = new Buddy(initialMemory); 
		obj.allocate(16); 
		obj.allocate(16); 
		obj.allocate(16); 
		obj.allocate(16); 
		obj.deallocate(0); 
		obj.deallocate(9); 
		obj.deallocate(32); 
		obj.deallocate(16); 
	} 
} 

