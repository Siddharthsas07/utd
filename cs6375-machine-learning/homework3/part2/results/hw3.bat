@echo off

set wekaPath=C:\Program Files\Weka-3-6\weka.jar
set libSVMPath=C:\Program Files\Weka-3-6\libsvm.jar

set enron1Train=C:\Users\Gary\Documents\School\school_svn\trunk\projects\graduate\cs6375\homework3\part2\text_to_arff\data\enron1_train.arff
set enron1Test=C:\Users\Gary\Documents\School\school_svn\trunk\projects\graduate\cs6375\homework3\part2\text_to_arff\data\enron1_test.arff
set enron4Train=C:\Users\Gary\Documents\School\school_svn\trunk\projects\graduate\cs6375\homework3\part2\text_to_arff\data\enron4_train.arff
set enron4Test=C:\Users\Gary\Documents\School\school_svn\trunk\projects\graduate\cs6375\homework3\part2\text_to_arff\data\enron4_test.arff
set hw2Train=C:\Users\Gary\Documents\School\school_svn\trunk\projects\graduate\cs6375\homework3\part2\text_to_arff\data\hw2_train.arff
set hw2Test=C:\Users\Gary\Documents\School\school_svn\trunk\projects\graduate\cs6375\homework3\part2\text_to_arff\data\hw2_test.arff

set resultOut=C:\Users\Gary\Documents\School\school_svn\trunk\projects\graduate\cs6375\homework3\part2\results

REM enron1
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%" weka.classifiers.functions.MultilayerPerceptron -L 0.3 -M 0.2 -N 75 -V 0 -S 0 -E 20 -H 3 -B -C -I -t "%enron1Train%" -T "%enron1Test%" -o > "%resultOut%\enron1_NN_3.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%" weka.classifiers.functions.MultilayerPerceptron -L 0.3 -M 0.2 -N 75 -V 0 -S 0 -E 20 -H 5 -B -C -I -t "%enron1Train%" -T "%enron1Test%" -o > "%resultOut%\enron1_NN_5.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%" weka.classifiers.functions.MultilayerPerceptron -L 0.3 -M 0.2 -N 75 -V 0 -S 0 -E 20 -H 7 -B -C -I -t "%enron1Train%" -T "%enron1Test%" -o > "%resultOut%\enron1_NN_7.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%" weka.classifiers.functions.MultilayerPerceptron -L 0.3 -M 0.2 -N 75 -V 0 -S 0 -E 20 -H 10 -B -C -I -t "%enron1Train%" -T "%enron1Test%" -o > "%resultOut%\enron1_NN_10.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%;%libSVMPath%" weka.classifiers.functions.LibSVM -S 0 -K 0 -D 3 -G 0.1 -R 1.0 -N 0.5 -M 2000.0 -C 1.0 -E 1.0E-4 -P 0.1 -t "%enron1Train%" -T "%enron1Test%" -o > "%resultOut%\enron1_SVM_linear.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%;%libSVMPath%" weka.classifiers.functions.LibSVM -S 0 -K 1 -D 3 -G 0.1 -R 1.0 -N 0.5 -M 2000.0 -C 1.0 -E 1.0E-4 -P 0.1 -t "%enron1Train%" -T "%enron1Test%" -o > "%resultOut%\enron1_SVM_polynomial.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%;%libSVMPath%" weka.classifiers.functions.LibSVM -S 0 -K 3 -D 3 -G 0.1 -R 1.0 -N 0.5 -M 2000.0 -C 1.0 -E 1.0E-4 -P 0.1 -t "%enron1Train%" -T "%enron1Test%" -o > "%resultOut%\enron1_SVM_sigmoid.txt"

REM enron4
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%" weka.classifiers.functions.MultilayerPerceptron -L 0.3 -M 0.2 -N 75 -V 0 -S 0 -E 20 -H 3 -B -C -I -t "%enron4Train%" -T "%enron4Test%" -o > "%resultOut%\enron4_NN_3.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%" weka.classifiers.functions.MultilayerPerceptron -L 0.3 -M 0.2 -N 75 -V 0 -S 0 -E 20 -H 5 -B -C -I -t "%enron4Train%" -T "%enron4Test%" -o > "%resultOut%\enron4_NN_5.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%" weka.classifiers.functions.MultilayerPerceptron -L 0.3 -M 0.2 -N 75 -V 0 -S 0 -E 20 -H 7 -B -C -I -t "%enron4Train%" -T "%enron4Test%" -o > "%resultOut%\enron4_NN_7.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%" weka.classifiers.functions.MultilayerPerceptron -L 0.3 -M 0.2 -N 75 -V 0 -S 0 -E 20 -H 10 -B -C -I -t "%enron4Train%" -T "%enron4Test%" -o > "%resultOut%\enron4_NN_10.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%;%libSVMPath%" weka.classifiers.functions.LibSVM -S 0 -K 0 -D 3 -G 0.1 -R 1.0 -N 0.5 -M 2000.0 -C 1.0 -E 1.0E-4 -P 0.1 -t "%enron4Train%" -T "%enron4Test%" -o > "%resultOut%\enron4_SVM_linear.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%;%libSVMPath%" weka.classifiers.functions.LibSVM -S 0 -K 1 -D 3 -G 0.1 -R 1.0 -N 0.5 -M 2000.0 -C 1.0 -E 1.0E-4 -P 0.1 -t "%enron4Train%" -T "%enron4Test%" -o > "%resultOut%\enron4_SVM_polynomial.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%;%libSVMPath%" weka.classifiers.functions.LibSVM -S 0 -K 3 -D 3 -G 0.1 -R 1.0 -N 0.5 -M 2000.0 -C 1.0 -E 1.0E-4 -P 0.1 -t "%enron4Train%" -T "%enron4Test%" -o > "%resultOut%\enron4_SVM_sigmoid.txt"

REM hw2
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%" weka.classifiers.functions.MultilayerPerceptron -L 0.3 -M 0.2 -N 75 -V 0 -S 0 -E 20 -H 3 -B -C -I -t "%hw2Train%" -T "%hw2Test%" -o > "%resultOut%\hw2_NN_3.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%" weka.classifiers.functions.MultilayerPerceptron -L 0.3 -M 0.2 -N 75 -V 0 -S 0 -E 20 -H 5 -B -C -I -t "%hw2Train%" -T "%hw2Test%" -o > "%resultOut%\hw2_NN_5.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%" weka.classifiers.functions.MultilayerPerceptron -L 0.3 -M 0.2 -N 75 -V 0 -S 0 -E 20 -H 7 -B -C -I -t "%hw2Train%" -T "%hw2Test%" -o > "%resultOut%\hw2_NN_7.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%" weka.classifiers.functions.MultilayerPerceptron -L 0.3 -M 0.2 -N 75 -V 0 -S 0 -E 20 -H 10 -B -C -I -t "%hw2Train%" -T "%hw2Test%" -o > "%resultOut%\hw2_NN_10.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%;%libSVMPath%" weka.classifiers.functions.LibSVM -S 0 -K 0 -D 3 -G 0.1 -R 1.0 -N 0.5 -M 2000.0 -C 1.0 -E 1.0E-4 -P 0.1 -t "%hw2Train%" -T "%hw2Test%" -o > "%resultOut%\hw2_SVM_linear.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%;%libSVMPath%" weka.classifiers.functions.LibSVM -S 0 -K 1 -D 3 -G 0.1 -R 1.0 -N 0.5 -M 2000.0 -C 1.0 -E 1.0E-4 -P 0.1 -t "%hw2Train%" -T "%hw2Test%" -o > "%resultOut%\hw2_SVM_polynomial.txt"
java -Xmx2g -cp "%CLASSPATH%;%wekaPath%;%libSVMPath%" weka.classifiers.functions.LibSVM -S 0 -K 3 -D 3 -G 0.1 -R 1.0 -N 0.5 -M 2000.0 -C 1.0 -E 1.0E-4 -P 0.1 -t "%hw2Train%" -T "%hw2Test%" -o > "%resultOut%\hw2_SVM_sigmoid.txt"
