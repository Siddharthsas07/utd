This submission contains both an eclipse project (source) and a jar (executable).

### Automated Run ###
Included with this submission is a python script for running the various tests. Make sure
the folder, file, and executables are in the same directories as this submission and then
simply execute "test.py". The easiest way is to do the following command on the command line:
> python test.py

NOTE: This will create a file called AOThing.output.txt

### Manual Run ###
The program expects a number of command line parameters:
[0] = UAI file path
[1] = UAI.evid file path
[2] = # of samples to generate
   
With the eclipse project simply specify these parameters in the run configurations.

To run the executable from the command line:
> java -Xmx1000M -jar aosampling.jar [0] [1] [2] 

NOTE: Please be sure to allot at least 1GB of RAM otherwise some test cases run out of memory!

### Other ###
Sample output is contained in multiple files data/output/. 

### Results ###
Results of the runs are collated and nicely formatted with the average error
in log10 space, and average computation time.

There is a .xlsx Microsoft Excel spreadsheet containing the tabulated results in data/output/.