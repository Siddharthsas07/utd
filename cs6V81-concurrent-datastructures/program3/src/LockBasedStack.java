import java.util.Stack;

public class LockBasedStack<T> implements MyStack<T>
{
	private Stack<T> stack;
	private TTASLock lock;
	LockBasedStack()
	{
		stack = new Stack<T>();
		lock = new TTASLock();
	}
	@Override
	public T pop() throws Exception, InterruptedException 
	{
		T item;
		lock.lock();
		item = stack.pop();
		lock.unlock();
		return item;
	}

	@Override
	public String getStackInfo() 
	{
		return "LockBasedStack";
	}
	@Override
	public void push(T value) throws InterruptedException 
	{
		lock.lock();
		stack.push(value);
		lock.unlock();
	}
	
}