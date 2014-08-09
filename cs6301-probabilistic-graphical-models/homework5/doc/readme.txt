This submission contains an eclipse project (source). There is no automated run for this program due to time constraints.

### Manual Run ###
This program expects a number of command line parameters:
[0] = Input UAI file path
[1] = Train file path
[2] = Test file path
[3] = Output UAI file path
[4] = #
      0 = MLE FOD Learning (Task 1)
      1 = EM POD Learning (Task 2)
      2 = CL FOD Learning (Task 3)
[5] = #
      k = Number of iterations to run EM. This is only used for EM.    
   
With the eclipse project simply specify these parameters in the run configurations. 

Be sure to give the Java VM the command line parameter -Xmx2000M to ensure enough memory is allocated for a smooth run.

### Example Run ###
data/3.uai.data/3.uai data/3.uai.data/train-f-5.txt data/3.uai.data/test.txt output/3.uai/BN_MLE_FOD-learned-f-5.txt 0

### Results ###
Results of the runs are collated and nicely formatted with graphs in results.(xlsx|pdf) 
in the /output/ folder.
The learned networks are saved here as well, first with their task name, then the data set on which they were trained. 
And lastly, the answers to the book work questions about the runs are found in the report
in the /doc/ folder.