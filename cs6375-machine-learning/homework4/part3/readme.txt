1) Algorithms:
     Improved: LibSVM, J48. 
     Didn't Improve: NaiveBayesMultinomial.
   Data:
     Improved: re0.wc.arff, oh0.wc.arff, tr11.wc.arff.
     Didn't Improve: N/A.

2) Algorithms:
     Improved: J48.
     Didn't Improve: NaiveBayesMultinomial, LibSVM.
   Data:
     Improved: tr11.wc.arff
     Didn't Improve: re0.wc.arff, oh0.wc.arff. (Did in some cases, but not in most)
     
3) The results show that all of the algorithms performed reasonably well. However,
   since NaiveBayes assumes conditional independence of all data points, it has a bias.
   LibSVM assumes that data is at some point linearly separable. And J48, a decision
   tree, assumes that the data can be partitioned non linearly.
   
   None of the chosen learners for this domain are unbaised.