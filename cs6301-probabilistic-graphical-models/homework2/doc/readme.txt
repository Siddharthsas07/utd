The program takes up to two command line arguments:
[0] path to a .uai file.
[1] path to a .uai.evid file.

You need to supply at least a .uai file. The .uai.evid file is optional.

Notes: 
I tried to store values in log space and have a LogNumber class to handle the
conversions to/from it, as it seemed practical and cool.

You can change the storage mode of the GraphModel as shown in the main function,
by setting the GraphModel.storeMode variable BEFORE reading. When reading, the
values are converted into the storeMode. Similarly you may also change the
eliminationAlgorithm and the orderHeuristic. You can set the network type, but
this is overwritten when a network is read from file.

I have pretty thoroughly tested my FunctionTable
instantiate, multiply, sumOut, and multiplySumOut, my GraphModel read, instantiate,
createMinDegreeOrder, and createBucketsFromOrder. I am pretty sure these work.

I have tested, but not as thoroughly my LogNumber functions. There appears to be 
some slight logic error somewhere in these, which can lead to incorrect answers.