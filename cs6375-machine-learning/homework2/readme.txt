The program folders should be structured as such:
homework2/
  homework2.exe
data/
  ignore.txt
  stopwords.txt
  train.txt
  test.txt
  train/<input files>
  test/<input files>

The program expects 3 parameters on the command line:

homework2.exe <path to input files> <name of training index file> <name of testing index file>
Example:
homework2.exe ../data/ train.txt test.txt

NOTE:
  1) The path to input files MUST end in a path delimiter (/ on linux).
  2) The program must have write permissions for generating a temporary file!

train.txt and test.txt should have the format:
<className>
<path to class' input files in the supplied data directory>
Example:
notSpam
train/ham/
spam
train/spam/

You can specify any number of classes and a directory for each class. 

The program will then look for input files from the supplied data directory
and then in the specified class directories.
ie:
notSpam in ../data/train/ham
spam in ../data/train/spam

NOTE:
Please look at the submitted file/directory structure structure if the above is not clear.

The results for runs for Naive Bayes and for Logistic Regression accuracy and
run times are in results.xlsx or results.pdf (it's the same table).The answers to 
the questions posed in the instructions are in results_analysis.txt
The running times were generated using Visual Studio 9.0 with no debugging and
with compiler speed optimizations enabled. 

PROGRAM CONFIGURATION:
To configure the parameters listed in the results, alter the variables at the
top of the files:
  Classifier.cpp: vocabDelimiters, minTokenSize, useIgnoreList, useStopWordsList
    + These variables will affect both NB and LR.
  LogisticRegressionClassifier.cpp: lambda, gradientAscentIterationLimit
    + These variables will only affect LR.