#ifndef DATASET_H
#define DATASET_H

#include <vector>
#include <string>
#include "Document.h"

using namespace std;

class DataSet {
public:
  bool loadData( string dataPath, string fileName );

  vector<vector<Document> > m_docs;  

  // <class label, class data directory path>
  vector<pair<string,string> > m_classes;
};

#endif
