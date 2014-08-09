import java.util.ArrayList;
import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Threads
{
	public static void main(String[] args)
	{
		System.out.println("No locks:");
		System.out.println("time: " + runThreads(1));
		System.out.println("time: " + runThreads(4));
		System.out.println("time: " + runThreads(8));
		System.out.println("time: " + runThreads(16));
	
		System.out.println("\nUsing locks:");
		System.out.println("time: " + runThreadsWithLock(1));
		System.out.println("time: " + runThreadsWithLock(4));
		System.out.println("time: " + runThreadsWithLock(8));
		System.out.println("time: " + runThreadsWithLock(16));
		
		runCompareLockInQueues(5000);
		runCompareLockInQueues(5000);
		runCompareLockInQueues(5000);
	}
	public static void runCompareLockInQueues(final int timeToRun)
	{
		System.out.println("\n/////////////////////");
		System.out.println("Testing locks");
		System.out.println("/////////////////////");
		
		System.out.println("\nTesting WaitFreeQueue");
		
		ArrayList<Thread> theThreads = new ArrayList<Thread>();
		
		final WaitFreeQueue<Integer> waitFreeQueue = new WaitFreeQueue<Integer>(100000);
		Thread ta1 = new Thread(new Runnable()
		{
			@Override
			public void run() 
			{	
				long totalTimeStart = System.currentTimeMillis();
				
				while(true)
				{
					try {
						waitFreeQueue.enq(1);
					} catch (Exception e) {
						//e.printStackTrace();
					}
					
					if( (System.currentTimeMillis() - totalTimeStart) > timeToRun)
						break;
				}
			}
			
		});
		theThreads.add(ta1);
		
		Thread ta2 = new Thread(new Runnable()
		{
			@Override
			public void run() 
			{	
				long totalTimeStart = System.currentTimeMillis();
				
				while(true)
				{
					try {
						waitFreeQueue.deq();
					} catch (Exception e) {
						//e.printStackTrace();
					}
					
					if( (System.currentTimeMillis() - totalTimeStart) > timeToRun)
						break;
				}
			}
			
		});
		theThreads.add(ta2);

		for(int thread = 0; thread<theThreads.size(); thread++)
		{
			theThreads.get(thread).start();
		}
		for(int thread = 0; thread<theThreads.size(); thread++)
		{
			try {
				theThreads.get(thread).join();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		System.out.println("Total test time: " + timeToRun);
		System.out.println("Items in queue: " + waitFreeQueue.size());
		System.out.println("Total method calls: " + (waitFreeQueue.insertedTotal + waitFreeQueue.removedTotal));
		System.out.println("Percent successful method calls: " + (double)(waitFreeQueue.inserted + waitFreeQueue.removed)*100/(waitFreeQueue.insertedTotal + waitFreeQueue.removedTotal));
		System.out.println("Ratio of enq/deq method calls: " + (double)waitFreeQueue.insertedTotal/waitFreeQueue.removedTotal );
		
		System.out.println("\nEnq attempts: " + waitFreeQueue.insertedTotal);
		System.out.println("Enq failed percent: " + (double)(waitFreeQueue.insertedTotal - waitFreeQueue.inserted)*100/waitFreeQueue.insertedTotal);
		System.out.println("Enq success percent: " + (double)waitFreeQueue.inserted*100/waitFreeQueue.insertedTotal);

		System.out.println("\nDeq attempts: " + waitFreeQueue.removedTotal);
		System.out.println("Deq failed attempts: " + (double)(waitFreeQueue.removedTotal - waitFreeQueue.removed)*100/waitFreeQueue.removedTotal);
		System.out.println("Deq success attempts: " + (double)waitFreeQueue.removed*100/waitFreeQueue.removedTotal);
		
		theThreads.clear();
		
		System.out.println("\nTesting LockBasedQueue");
		
		final LockBasedQueue<Integer> lockBasedQueue = new LockBasedQueue<Integer>(100000);
		Thread tb1 = new Thread(new Runnable()
		{
			@Override
			public void run() 
			{	
				long totalTimeStart = System.currentTimeMillis();
				
				while(true)
				{
					try {
						lockBasedQueue.enq(1);
					} catch (Exception e) {
						//e.printStackTrace();
					}
					
					if( (System.currentTimeMillis() - totalTimeStart) > timeToRun)
						break;
				}
			}
			
		});
		theThreads.add(tb1);
		
		Thread tb2 = new Thread(new Runnable()
		{
			@Override
			public void run() 
			{	
				long totalTimeStart = System.currentTimeMillis();
				
				while(true)
				{
					try {
						lockBasedQueue.deq();
					} catch (Exception e) {
						//e.printStackTrace();
					}
					
					if( (System.currentTimeMillis() - totalTimeStart) > timeToRun)
						break;
				}
			}
			
		});
		theThreads.add(tb2);

		for(int thread = 0; thread<theThreads.size(); thread++)
		{
			theThreads.get(thread).start();
		}
		for(int thread = 0; thread<theThreads.size(); thread++)
		{
			try {
				theThreads.get(thread).join();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		System.out.println("Total test time: " + timeToRun);
		System.out.println("Items in queue: " + lockBasedQueue.size());
		System.out.println("Total method calls: " + (lockBasedQueue.insertedTotal + lockBasedQueue.removedTotal));
		System.out.println("Percent successful method calls: " + (double)(lockBasedQueue.inserted + lockBasedQueue.removed)*100/(lockBasedQueue.insertedTotal + lockBasedQueue.removedTotal));
		System.out.println("Ratio of enq/deq method calls: " + (double)lockBasedQueue.insertedTotal/lockBasedQueue.removedTotal );
		
		System.out.println("\nEnq attempts: " + lockBasedQueue.insertedTotal);
		System.out.println("Enq failed percent: " + (double)(lockBasedQueue.insertedTotal - lockBasedQueue.inserted)*100/lockBasedQueue.insertedTotal);
		System.out.println("Enq success percent: " + (double)lockBasedQueue.inserted*100/lockBasedQueue.insertedTotal);

		System.out.println("\nDeq attempts: " + lockBasedQueue.removedTotal);
		System.out.println("Deq failed percent: " + (double)(lockBasedQueue.removedTotal - lockBasedQueue.removed)*100/lockBasedQueue.removedTotal);
		System.out.println("Deq success percent: " + (double)lockBasedQueue.removed*100/lockBasedQueue.removedTotal);
	}
	public static long runThreadsWithLock(int numThreads)
	{
		final MyLockedInt theInt = new MyLockedInt();
		
		long start = System.currentTimeMillis();
		
		ArrayList<Thread> theThreads = new ArrayList<Thread>();
		for(int thread = 0; thread<numThreads; thread++)
		{
			//final int pid = thread;
			Thread t = new Thread(new Runnable()
			{
				@Override
				public void run() 
				{	
					for(int i=0; i<10000; i++)
					{
						int temp =theInt.get();
						theInt.set(temp + 1);
					}
					
				}
				
			});
			theThreads.add(t);
		}
		for(int thread = 0; thread<numThreads; thread++)
		{
			theThreads.get(thread).start();
		}
		for(int thread = 0; thread<numThreads; thread++)
		{
			try {
				theThreads.get(thread).join();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		System.out.println("The counter value: " + theInt.get());
		
		return System.currentTimeMillis()-start;
	}
	public static long runThreads(int numThreads)
	{
		long start = System.currentTimeMillis();
		
		ArrayList<Thread> theThreads = new ArrayList<Thread>();
		for(int thread = 0; thread<numThreads; thread++)
		{
			//final int pid = thread;
			Thread t = new Thread(new Runnable()
			{
				@Override
				public void run() 
				{	
					for(int i=0; i<10000; i++)
					{
						int temp = MyInt.value;
						MyInt.value = temp + 1;
					}
					
				}
				
			});
			theThreads.add(t);
		}
		for(int thread = 0; thread<numThreads; thread++)
		{
			theThreads.get(thread).start();
		}
		for(int thread = 0; thread<numThreads; thread++)
		{
			try {
				theThreads.get(thread).join();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		System.out.println("The counter value: " + MyInt.value);
		
		return System.currentTimeMillis()-start;
	}
}
class WaitFreeQueue<T>
{
	int inserted, insertedTotal, removed, removedTotal;
	volatile int head, tail;
	T[] items;

	public WaitFreeQueue(int capacity)
	{
		items = (T[]) new Object[capacity];
	}
	public void enq(T x) throws Exception
	{
		try
		{
			if(tail - head == items.length)
				throw new Exception();
			
			items[tail % items.length] = x;
			tail++;
			inserted++;
		}
		finally
		{
			insertedTotal++;
		}
	}
	public T deq() throws Exception
	{
		try
		{
			if(tail == head)
				throw new Exception();
			
			T x = items[head % items.length];
			head++;
			removed++;
			return x;
		}
		finally
		{
			removedTotal++;
		}
	}
	public int size()
	{
		return tail - head;
	}
}
class LockBasedQueue<T>
{
	int head, tail;
	int inserted, insertedTotal, removed, removedTotal;
	T[] items;
	Lock lock;
	public LockBasedQueue(int capacity)
	{
		lock = new ReentrantLock();
		items = (T[]) new Object[capacity];
	}
	public void enq(T x) throws Exception
	{
		lock.lock();
		try
		{
			if(tail - head == items.length)
				throw new Exception();
			
			items[tail % items.length] = x;
			tail++;
			inserted++;
		}
		finally
		{
			lock.unlock();
			insertedTotal++;
		}
		
	}
	public T deq() throws Exception
	{
		lock.lock();
		try
		{
			if(tail == head)
				throw new Exception();
			
			T x = items[head % items.length];
			head++;
			removed++;
			return x;
		}
		finally
		{
			lock.unlock();
			removedTotal++;
		}	
	}
	public int size()
	{
		return tail - head;
	}
}
class MyLockedInt
{
	private Semaphore s = new Semaphore(1);
	private int value=0;
	MyLockedInt(){ }
	public void set(int value)
	{
		try {
			s.acquire();
			this.value = value;
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		s.release();
	}
	public int get()
	{
		int temp =0;
		try {
			s.acquire();
			temp = value;
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		s.release();
		
		return temp;
	}
}
class MyInt
{
	public static int value=0;
	MyInt(){ }
}