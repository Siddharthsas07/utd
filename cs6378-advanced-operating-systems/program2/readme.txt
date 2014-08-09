To compile the program:

$> make

This should have no warnings or errors.

To execute the program: You require one ssh connection for each node in the network.
Then on each instance perform:

./driver <node ID> <node config file path>

This tells the executable which node ID it is assigned (0-9).
The server location for a node with a given ID must match the server location in
the configuration file for the node with the same ID. That is, if you ssh into
net01.utdallas.edu and run it with node ID 0, then the configuration file must
also contain net01.utdallas.edu as the address for node 0. 

The configuration file is formatted as following:
<communication port>
<node ID 1>
<node ID 1 comma separated list of group IDs> 
<node ID 1 address string>
<node ID 2>
<node ID 2 comma separated list of group IDs> 
<node ID 2 address string>
.
.
.

Do NOT leave any blank lines ANYWHERE!

To add a node to the server group, give it a group assignment of 2.
To add a node to the client group, give it a group assignment of 1. 

There is a sample configuration file (the one used to test correctness) in:
addresses.txt