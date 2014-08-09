The supplied results are calculated:
  - Each thread makes 960400 calls to push().
  - Total time taken for push() operation to succeed is stored. 
  - Average time for all push() operations / number of threads.
  - Same for pop().

It can be seen from the results in results.(pdf|ods) that the following holds:
  - For all 3 implementations pop() completes faster than push() (sometimes by a lot).
  - LockBasedStack is the slowest of all 3 for both push and pop.
  - EliminationBackoffStack is the middle for both push and pop.
  - LockFreeStack is the fastest for all 3, especially when pushing.
  - LockFreeStack and EliminationBackoffStack are almost the same when popping.