There is an included .bat script in /results/hw3.bat that will execute all of the
runs and output the result files on a Window machine with Weka installed.

To make the .bat run properly on your machine you should change 4 things:
1) Change the wekaPath variable to point to your weka.jar, ie:
    set wekaPath=C:\Program Files\Weka-3-6\weka.jar
    to
    set wekaPath=C:\I installed weka to here\Weka-3-6\weka.jar
2) Change the libSVMPath variable to point to your libsvm.jar, ie:
    libSVMPath=C:\Program Files\Weka-3-6\libsvm.jar
    to
    libSVMPath=C:\I installed libsvm to here\libsvm.jar
3) Change the locations to the input data, ie:
    set hw2Test=C:\Users\Gary\Documents\School\school_svn\trunk\projects\graduate\cs6375\homework3\part2\text_to_arff\data\hw2_test.arff
    to
    set hw2Test=C:\weka_data_files\hw2_test.arff
4) Change the data output directory to a writeable directory, ie: 
    set resultOut=C:\Users\Gary\Documents\School\school_svn\trunk\projects\graduate\cs6375\homework3\part2\results
    to
    set resultOut=C:\weka_data_output\results
