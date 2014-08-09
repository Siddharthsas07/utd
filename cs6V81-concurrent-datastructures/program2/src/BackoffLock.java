import java.util.Random;
import java.util.concurrent.atomic.AtomicBoolean;

public class BackoffLock implements MyLock {
  private AtomicBoolean state = new AtomicBoolean(false);
  private static int MIN_DELAY;
  private static int MAX_DELAY;
  
  BackoffLock(int min, int max)
  {
    BackoffLock.MAX_DELAY = max;
    BackoffLock.MIN_DELAY = min;
  }
  
  @Override
  public void lock() {
    Backoff backoff = new Backoff(MIN_DELAY, MAX_DELAY);
    while(true)
    {
      while(state.get()) {};
      if(!state.getAndSet(true))
        return;
      else
      {
        try {
          backoff.backoff();
        } catch (InterruptedException e) {
          e.printStackTrace();
        }
      }
    }
  }

  @Override
  public void unlock() {
    state.set(false);
  }

  @Override
  public String getLockInfo() {
    return "Backoff lock," + MIN_DELAY + "," + MAX_DELAY;
  }

}

class Backoff
{
  final int minDelay, maxDelay;
  final Random random;
  int limit;
  
  Backoff(int min, int max)
  {
    minDelay = min;
    maxDelay = max;
    limit = minDelay;
    random = new Random();
  }
  
  public void backoff() throws InterruptedException
  {
    int delay = random.nextInt(limit);
    limit = Math.min(maxDelay, 2 * limit);
    Thread.sleep(delay);
  }
}
