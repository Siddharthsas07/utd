import java.util.concurrent.atomic.AtomicBoolean;

public class TASLock implements MyLock {
  AtomicBoolean state = new AtomicBoolean(false);
  
  @Override
  public void lock() {
    while (state.getAndSet( true )) {}    
  }

  @Override
  public void unlock() {
    state.set( false );    
  }

  @Override
  public String getLockInfo() {
    return "TASLock lock";
  }
}