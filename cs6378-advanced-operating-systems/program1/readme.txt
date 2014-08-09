To compile the program:

$> make

This should have no warnings or errors.

To execute the program:

./driver <node ID> addresses.txt

This tells the executable which node it is (0-9).
addresses.txt MUST match the node ID supplied to the program.

addresses.txt is formatted as:
<communication port>
<data collection node ID>
<node ID 1>
<address for node ID 1>
<node ID 2>
<address for node ID 2>
.
.
.

Do NOT leave any blank lines. 