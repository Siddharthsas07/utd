This submission contains both an eclipse project (source) and a jar (executable).

### Automated Run ###
Included with this submission is a python script for running the various tests. Make sure
the folder, file, and executables are in the same directories as this submission and then
simply execute "test.py". The easiest way is to do the following command on the command line:
> python test.py

NOTE: This will create a file called homework3_5output.txt which contains multiple copies
each of 2 lines representing the results of the runs. The format is:
[Calculated Partition Function]
[Elimination time in seconds]

### Manual Run ###
The program expects a number of command line parameters:
[0] = UAI file path
[1] = UAI.evid file path

Following that you may specify algorithms and their parameters. 
If you don't, it defaults to Bucket elimination + min-degree ordering.
[2] = #
      0 = Bucket elimination
      1 = W-cutset Bucket elimination
[3] = #
      0 = Min-degree bucket ordering

If you specify w-cutset Bucket elimination:
[4] = #
      # = Maximum cluster size after w-cutset
[5] = #
      # = Number of samplings after w-cutset
[6] = #
      0 = Uniform distribution sampling
      1 = Dynamic-learning distribution sampling    
   
With the eclipse project simply specify these parameters in the run configurations.

To run the executable from the command line:
> java -Xmx1000M -jar homework3_5.jar [0] [1] [2] [3] [4] [5] [6]

NOTE: Please be sure to allot at least 1GB of RAM otherwise some test cases run out of memory!

### Other ###
My sample output is contained in multiple files (as I had to adjust the Java heap
request size) and can be found in /output/. 

### Results ###
Results of the runs are collated and nicely formatted with the average relative error
in log10 space, standard deviation of the relative error in log10, and average and standard
deviation for elimination time.

There is a .xlsx Microsoft Excel spreadsheet containing the tabulated results in /output/.

The average relative error (AVG Err) and relative error standard deviation (STD Err) are 
recorded over the results of the 10 runs per configuration.
The average execution time (AVG Tim) and execution time standard deviation (STD Tim) are 
recorded over the results of the 10 runs per configuration.

And lastly, the answers to the questions about the runs are found in the report
in the /doc/ folder.