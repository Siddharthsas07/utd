#include "AnomalyDetector.h"

AnomalyDetector::AnomalyDetector() : 
m_eps(0.0), m_alg(UNIVARIATE_GAUSSIAN), m_mode(UNSUPERVISED), m_valid(false), m_cvPercent(0.0), m_folds(0) {}

AnomalyDetector::~AnomalyDetector() {}

void AnomalyDetector::setAlg( LEARNING_ALGORITHM alg ) {
  m_alg = alg;
  m_valid = false;
}

LEARNING_ALGORITHM AnomalyDetector::getAlg() const {
  return m_alg;
}

void AnomalyDetector::setMode( LEARNING_MODE mode ) {
  m_mode = mode;
  m_valid = false;
}

LEARNING_MODE AnomalyDetector::getMode() const {
  return m_mode;
}

void AnomalyDetector::setCVPercent( double p ) {
  if ( p < 0.0 ) {
    p = 0.0;
  }

  else if ( p > 1.0 ) {
    p = 1.0;
  }

  m_cvPercent = p;
}

double AnomalyDetector::getCVPercent() const {
  return m_cvPercent;
}

void AnomalyDetector::setNumFolds( unsigned int f ) {
  m_folds = f; 
}

unsigned int AnomalyDetector::getNumFolds() const {
  return m_folds;
}

void AnomalyDetector::readData( const string& file ) {
#ifdef WINDOWS
  CStopWatch timer;
  timer.startTimer();
#endif

  cout << "===Loading===" << endl;

  // With new data read, must retrain
  m_valid = false; 

  string line = "";               // String for the current line in the file
  string  tok = "";               // Individual token on the line
  double  num = 0.0f;             // Individual token as a number
  deque<double> instance;         // numberized version of the line
  bool anomalousInstance = false; // Whether the current instance is anomalous or not

  unsigned int lhs = 0;       // Used for indexing along a line when tokenizing
  unsigned int rhs = 0;       // Used for indexing along a line when tokenizing
  string delimiter = " \n";   // Delimiter for tokens on a line

  try {
    ifstream inFile( file.c_str() );    
    if ( inFile.is_open() ) {
      clearData();   

      while ( getline( inFile, line ) ) {
        if ( line == "" || line == "\n" ) {
          continue;
        }

        instance.clear();
        anomalousInstance = false;

        lhs = 0;
        rhs = 0;

        do {
          rhs = line.find_first_of( " ", lhs );
          tok = line.substr( lhs, 
                             (rhs == string::npos ? line.size() : rhs) - lhs);
          num = atof( tok.c_str() );

          if (tok == "NaN" || tok == "INF") {
            num = DBL_MAX;
            anomalousInstance = true;
          }

          // Parsing this token is complete, push back the number and find the
          // next token.
          instance.push_back( num );

          // Slide to next window
          lhs = rhs + 1;

        } while ( rhs != string::npos );

        // Parsing this line is complete, push back the instance. 
        if ( (m_mode == SEMI_SUPERVISED || m_mode == SUPERVISED) && anomalousInstance ) {
          m_ad.push_back( instance );
        }
        else if ( m_mode == UNSUPERVISED && anomalousInstance ) {
          // Let the instance fall into the nether
        }
        else {
          m_d.push_back( instance ); 
        }
      } 
    }

    else {
      cout << "ERROR: Could not open input data file." << endl;
    }

    // Even if mode was SEMI_SUPERVISED or SUPERVISED, if there was no 
    // anomalous data pre-classified the algorithms will essentially be running 
    // in unsupervised mode. 
    if ( m_ad.empty() ) {
      m_mode = UNSUPERVISED;
    }

    inFile.close();
  }

  catch ( exception& e ) {
    cout << e.what() << endl;
  }

#ifdef WINDOWS
  timer.stopTimer();
  cout << "Data load completed in " << timer.getElapsedTime() << " seconds." << endl;
#endif
}

void AnomalyDetector::train() {
#ifdef WINDOWS
  CStopWatch timer;
  timer.startTimer();
#endif

  cout << "===Training===" << endl;

  if ( m_d.empty() ) {
    cout << "ERROR: Training data set is empty. Use readData()." << endl;
    return;
  }

  if ( m_folds < 1 ) {
    cout << "ERROR: Need at least 1 fold to train. Use setNumFolds()." << endl;
    return;
  }

  // Now that training has commenced, mark as valid
  m_valid = true;  

  // Keep 70% of samples for training purposes. Don't touch rest until test()!
  partitionDataSet( m_d, m_traind, m_testd, 0.7 );
  partitionDataSet( m_ad, m_trainad, m_testad, 0.7 );

  // The training and cross validation sets.
  deque<deque<double> > okTrainSet;
  deque<deque<double> > okCVSet;
  deque<deque<double> > anomTrainSet;
  deque<deque<double> > anomCVSet;

  // Best accuracy obtained from a fold.
  double bestAccuracy = 0.0;

  // Best KMeans array obtained from a fold.
  deque<deque<KMeans> > bestK;

  // Run training for the specified number of folds
  for ( unsigned int fold = 0; fold < m_folds; fold++ ) {
    cout << "Fold " << fold << "..." << endl;

    // Partition the data sets randomly
    partitionDataSet( m_traind, okTrainSet, okCVSet, m_cvPercent );   
    if ( m_mode == SUPERVISED || m_mode == SEMI_SUPERVISED ) {
      partitionDataSet( m_trainad, anomTrainSet, anomCVSet, m_cvPercent );
    }

    if ( m_alg == UNIVARIATE_GAUSSIAN ) {
      m_ug.clear();
      deque<double> col;

      // Grab a column of data and populate a gaussian on it
      for ( unsigned int j = 0; j < okTrainSet[0].size(); j++ ) {
        col.clear();
        for ( unsigned int i = 0; i < okTrainSet.size(); i++ ) {
          col.push_back( okTrainSet[i][j] );
        }

        UnivariateGaussian g;
        g.populate( col );
        m_ug.push_back( g );
      }

      // Calculate this fold's epsilon
      double currEps = calculateEpsilon( okTrainSet, anomTrainSet );

      // Test accuracy using this fold's epsilon, on the cross validation sets
      double accuracy = crossValidate( okCVSet, anomCVSet, currEps );

      if ( accuracy > bestAccuracy ) {
        m_eps = currEps;
      }      
    }

    else if ( m_alg == MULTIVARIATE_GAUSSIAN ) {
      m_mg.clearData();

      // Regardless of mode, train on the ok set
      m_mg.populate( okTrainSet );

      double currEps = calculateEpsilon( okTrainSet, anomTrainSet );

      double accuracy = crossValidate( okCVSet, anomCVSet, currEps );

      if ( accuracy > bestAccuracy ) {
        m_eps = currEps;
      }
    }

    else if ( m_alg == KMEANS ) {
      // If running in non-unsupervised mode, it means that there's anomalous
      // data available to train on. Make another set of classifiers for it.
      m_k.clear();
      if ( m_mode == SUPERVISED || m_mode == SEMI_SUPERVISED ) {
        m_k.resize(2);
      }

      else {
        m_k.resize(1);
      }
      deque<double> col;

      // Grab a column of data and populate a kmeans on it
      for ( unsigned int j = 0; j < okTrainSet[0].size(); j++ ) {
        KMeans k;
        k.setNumClusters(1);

        // Always do the OK data
        col.clear();
        for ( unsigned int i = 0; i < okTrainSet.size(); i++ ) {
          col.push_back( okTrainSet[i][j] );
        }
        k.populate( col );
        m_k[0].push_back( k );

        // Also do the anomalous data if there is any
        if ( m_k.size() == 2 ) {
          col.clear();
          for ( unsigned int i = 0; i < anomTrainSet.size(); i++ ) {
            col.push_back( anomTrainSet[i][j] );
          }
          k.populate( col );
          m_k[1].push_back( k );
        }
      }

      // Test accuracy using this fold's KMeans arrays
      double accuracy = crossValidate( okCVSet, anomCVSet, -1.0 );

      if ( accuracy > bestAccuracy ) {
        bestK = m_k;
        bestAccuracy = accuracy;
      }
    }

  } 

  m_k = bestK;

#ifdef WINDOWS
  timer.stopTimer();
  cout << "Training completed in " << timer.getElapsedTime() << " seconds." << endl;
#endif
}

double AnomalyDetector::test() const {
#ifdef WINDOWS
  CStopWatch timer;
  timer.startTimer();
#endif

  cout << "===Testing===" << endl;

  if ( !m_valid ) {
    cout << "ERROR: Cannot test due to invalid state. Use train()." << endl;
    return 0.0;
  }
  double accuracy = crossValidate( m_testd, m_testad, m_eps );

#ifdef WINDOWS
  timer.stopTimer();
  cout << "Testing completed in " << timer.getElapsedTime() << " seconds." << endl;
#endif
  cout.precision(20);
  cout << "Accuracy on test set: " << (accuracy * 1.0) << "%" << endl;

  return accuracy;
}

double AnomalyDetector::evaluate( const deque<double>& x ) const {
  if ( !m_valid ) {
    cout << "ERROR: Cannot evaluate due to invalid state. Use train()." << endl;
    return -1.0;
  }

  if ( m_alg == UNIVARIATE_GAUSSIAN ) { 
    double p = 1.0;
    for ( unsigned int i = 0; i < m_ug.size(); i++ ) {
      p *= m_ug[i].sample( x[i] );
    }
    return p;
  }

  else if ( m_alg == MULTIVARIATE_GAUSSIAN ) { 
    return m_mg.sample( x );
  }

  else if ( m_alg == KMEANS ) {
    if ( m_k.empty() ) { return -1.0; }

    for ( unsigned int i = 1; i < m_k[0].size(); i++ ) {
      deque<double> okCenters;
      deque<double> anomCenters;
      deque<double> okMaxDists;

      // Compute closest cluster
      if ( m_mode == SUPERVISED || m_mode == SEMI_SUPERVISED ) {
        okCenters = m_k[0][i].getCenters();
        anomCenters = m_k[1][i].getCenters();

        double distToOk = fun::makeFinite( fun::dist(okCenters[0], x[i]) );
        double distToAnom = fun::makeFinite( fun::dist(anomCenters[0], x[i]) );
        if ( distToAnom < distToOk ) {
          return 1.0;
        }
      }

      // Use max distance 
      else if ( m_mode == UNSUPERVISED ) {
        okCenters = m_k[0][i].getCenters();
        okMaxDists = m_k[0][i].getMaxDistances();

        double distToOk = fun::makeFinite( fun::dist(okCenters[0], x[i]) );
        if ( distToOk > okMaxDists[0] ) {
          return 1.0;
        }
      }
    }

    return -1.0;
  }
  
  return 1.0;
}

// true for anomaly, false for OK
bool AnomalyDetector::classify( const deque<double>& x ) const {
  if ( !m_valid ) {
    cout << "Invalid state, cannot classify." << endl;
    return false;
  }

  if ( m_alg == UNIVARIATE_GAUSSIAN || m_alg == MULTIVARIATE_GAUSSIAN ) {
    if ( evaluate( x ) < m_eps ) {
      return true;
    }
    return false;
  }

  else if ( m_alg == KMEANS ) {
    if ( evaluate( x ) > 0.0 ) {
      return true;
    }
    return false;
  }

  return false;
}

void AnomalyDetector::print() const {
  cout << "===Model===" << endl;
  cout << "data size: " << m_d.size() << endl;
  cout << "anomalous data size: " << m_ad.size() << endl;
  cout << "mode: " << m_alg << endl;
  cout << "valid: " << m_valid << endl;

  if ( m_alg == UNIVARIATE_GAUSSIAN ) {
    cout << "univariate gaussian" << endl;
    cout << "n: " << m_ug.size() << endl;      
    for ( unsigned int i = 0; i < m_ug.size(); i++ ) {
      m_ug[i].print();
    }
    cout << "e: " << m_eps << endl;
  }

  else if ( m_alg == MULTIVARIATE_GAUSSIAN ) {
    cout << "multivariate gaussian" << endl;
    m_mg.print();
    cout << "e: " << m_eps << endl;
  }

  else if ( m_alg == KMEANS ) {
    cout << "kmeans" << endl;
    for ( unsigned int i = 0; i < m_k.size(); i++ ) {
      if ( i == 0 ) { cout << "ok c:" << endl; }
      else if ( i == 1 ) { cout << "anom c:" << endl; }
      for ( unsigned int j = 0; j < m_k[i].size(); j++ ) {
        m_k[i][j].print();
      }
    }
  }
}
  
double AnomalyDetector::calculateEpsilon( const deque<deque<double> >& okSet, 
                                          const deque<deque<double> >& anomSet ) const {
  double okEps = DBL_MAX;
  double anomEps = 0.0;
  double currp = 0.0;

  double anomMaxP = 0.0;
  double okMinP = DBL_MAX;

  // Get minimum probability from the okSet
  for ( unsigned int i = 0; i < okSet.size(); i++ ) {
    currp = evaluate( okSet[i] );
    if ( currp < okEps ) { okEps = currp; }
    if ( currp < okMinP ) { okMinP = currp; }
  }
  if ( okMinP == DBL_MAX ) { okMinP = 0.0; }

  // Get maximum probability from the anomSet
  for ( unsigned int i = 0; i < anomSet.size(); i++ ) {
    currp = evaluate( anomSet[i] );
    if ( currp > anomEps ) { anomEps = currp; }
    if ( currp > anomMaxP ) { anomMaxP = currp; }
  }
  if ( anomMaxP == DBL_MAX ) { anomMaxP = 0.0; }

  if ( okSet.empty() && anomSet.empty() ) {
    return 0.0;
  }

  else if ( okSet.empty() ) {
    return anomMaxP;;
  }

  else if ( anomSet.empty() ) {
    return okMinP;
  }

  double numer = anomMaxP + okMinP;
  double denom = ( anomSet.empty() ? okSet.size() : ((double)okSet.size() / anomSet.size()) );
  return numer / denom;
}

void AnomalyDetector::clearData() {
  // Clear the univariate gaussian
  for ( unsigned int i = 0; i < m_ug.size(); i++ ) {
    m_ug[i].clearData();
  }

  // Clear the multivariate gaussian
  m_mg.clearData();

  m_eps = 0.0;  

  m_valid = false;
}

void AnomalyDetector::partitionDataSet( const deque<deque<double> >& d,
                                        deque<deque<double> >& main,
                                        deque<deque<double> >& rest,
                                        double mainPercent ) {
  if ( d.empty() ) {
    main = deque<deque<double> >();
    rest = deque<deque<double> >();
    return;
  }

  RNG<unsigned int> rng;
  // Calculate size and starting position for main partition from d
  unsigned int size = static_cast<unsigned int>( (double)d.size() * mainPercent );
  unsigned int start = rng.rand(d.size() - size);

  // Assign main partition
  main.assign( d.begin()+start, d.begin()+start+size );

  // Assign rest partitions. Beginning up to main, after main to end.
  rest.assign( d.begin(), d.begin()+start );
  rest.insert( rest.end(), d.begin()+start+size, d.end() );
}

double AnomalyDetector::crossValidate( const deque<deque<double> >& okSet, 
                                       const deque<deque<double> >& anomSet,
                                       double eps ) const {
  unsigned int okCorrectClassifies = 0;
  unsigned int anomCorrectClassifies = 0;

  if ( m_alg == UNIVARIATE_GAUSSIAN || m_alg == MULTIVARIATE_GAUSSIAN ) {
    double currp = 0.0;

    // loop over ok set, count number of misclassifies
    for ( unsigned int i = 0; i < okSet.size(); i++ ) {
      currp = evaluate( okSet[i] );
      if ( currp > eps ) {
        okCorrectClassifies++;
      }
    }

    for ( unsigned int i = 0; i < anomSet.size(); i++ ) {
      currp = evaluate( anomSet[i] );
      if ( currp > eps ) {
        anomCorrectClassifies++;
      }
    }
  }

  else if ( m_alg == KMEANS ) {
    for ( unsigned int i = 0; i < okSet.size(); i++ ) {
      if ( classify( okSet[i] ) == false ) {
        okCorrectClassifies++;
      }
    }

    for ( unsigned int i = 0; i < anomSet.size(); i++ ) {
      if ( classify( anomSet[i] ) == true ) {
        anomCorrectClassifies++;
      }
    }
  }  

  double np = okSet.size()+anomSet.size(); // number of points classified
  return ( np == 0 ? 1.0 : (double)(okCorrectClassifies + anomCorrectClassifies) / np );
}