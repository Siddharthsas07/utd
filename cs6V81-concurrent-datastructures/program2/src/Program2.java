import java.util.ArrayList;

public class Program2 
{
  static double totalTimeToLock = 0;
  static double totalTimeToUnLock = 0;
  
  public static void main( String[] args ) 
  {
    System.out.println("starting");
    int iterations = 960400, min = 1, max = 100;
    
    for(int numThreads = 2; numThreads < 16; numThreads *= 2) //2,4,8,16 threads
    {
      MyLock theLock = new ALock(numThreads);
      testLock(theLock, numThreads, iterations);
    }
    
    for(int numThreads = 2; numThreads < 32; numThreads *= 2)
    {
      MyLock theLock = new TASLock();
      testLock(theLock, numThreads, iterations);
    }
    
    for(int numThreads = 2; numThreads < 32; numThreads *= 2)
    {
      MyLock theLock = new TTASLock();
      testLock(theLock, numThreads, iterations);
    }
    
    for(int numThreads = 2; numThreads < 32; numThreads *= 2)
    {
      for(int testMax = 10; testMax <= max; testMax += 5)
      {
        MyLock theLock = new BackoffLock(min, testMax);
        testLock(theLock, numThreads, iterations);
      }
      for(int testMin = 1; testMin <= max; testMin += 5)
      {
        MyLock theLock = new BackoffLock(testMin, max);
        testLock(theLock, numThreads, iterations);
      }
    }
    System.out.println("done");
  }
  
  public static void testLock(final MyLock theLock, int numThreads, final int numLockUnlock)
  {
    totalTimeToLock = 0;
    totalTimeToUnLock = 0;
    ArrayList<Thread> theThreads = new ArrayList<Thread>();
    
    for(int i = 0; i < numThreads; i++)
    {
      final Thread ta1 = new Thread(new Runnable()
      {
        double timeToLock = 0;
        double timeToUnLock = 0;
        
        @Override
        public void run() 
        {  
          // The critical section here is very small, as should be used with spin locks
          for(int b = 0; b < numLockUnlock; b++)
          {
            long startTimeLock = System.nanoTime();
            theLock.lock();
            timeToLock += System.nanoTime() - startTimeLock;
            long startTimeUnLock = System.nanoTime();
            theLock.unlock();
            timeToUnLock += System.nanoTime() - startTimeUnLock;
          }
          
          double avrTimeToLock = ((double)timeToLock)/numLockUnlock*10e-6;
          double avrTimeToUnLock = ((double)timeToUnLock)/numLockUnlock*10e-6;
          
          totalTimeToLock += avrTimeToLock;
          totalTimeToUnLock += avrTimeToUnLock;
        }
      });
      
      theThreads.add(ta1);
      }
    
      for(int thread = 0; thread<theThreads.size(); thread++)
      {
        theThreads.get(thread).start();
      }
      
      for(int thread = 0; thread<theThreads.size(); thread++)
      {
        try
        {
          theThreads.get(thread).join();
        } 
        catch (InterruptedException e) 
        {
          e.printStackTrace();
        }
      }
    System.out.println(theLock.getLockInfo() + "," + numThreads + "," + totalTimeToLock/numThreads + "," + totalTimeToUnLock/numThreads);
  }
}
