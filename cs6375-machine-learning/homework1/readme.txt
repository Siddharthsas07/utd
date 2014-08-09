The zip file should have multiple directores:
/data/
  Contains the 3 supplied dat sets. 
/doc/
  HTML documentation of the project code. Open index.html.
/homework1/
  The source code for the project. This includes a couple of Windows-only 
  files: hr_timer.h and hr_timer.cpp. For portability purposes the lines which 
  used these are commented out. If you want to use them to time how long the 
  trees take to create and prune uncomment the following:
    main.cpp:
      #include "hr_timer.h"
      CStopWatch timer;
      timer.startTimer();
      timer.stopTimer();
      Any associated cout statements including the timer.
  
  If you do not want to use the timer (it is only for informational purposes) 
  then do not change anything.
/results/
  accuracies.txt - The request accuracy reports.
  output.txt - A sample output from the program (discluding timer use).