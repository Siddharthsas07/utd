#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

// Path to file containing data, data table, vector of column headers
template <class T> 
bool DataSet<T>::load( const string& path )
{
  cout << "Loading data set from " << path << endl;

  string line;
  ifstream dataFile( path.c_str() );

  if ( dataFile.is_open() )
  {
    // Load attribute labels.
    if ( dataFile.good() )
    {
      m_n.clear();
      getline( dataFile, line );
      unsigned int lhs = 0; 
      unsigned int rhs = 0;

      while ( true )
      {
        // rhs of interval is the next comma/end of string after lhs
        rhs = line.find( ",", lhs );

        if ( rhs != string::npos )
        {
          m_n.push_back( line.substr( lhs, rhs-lhs ) );
          lhs = rhs + 1;
        }

        else
        {
          m_n.push_back( line.substr( lhs, line.size() ) );
          break;
        }        
      }
    }

    m_d.clear();

    while ( dataFile.good() )
    {
      getline( dataFile, line );
      vector<bool> featureVector;

      if ( line == "\n" || line == "" )
      {
        continue;
      }

      // SPECIFIC TO BOOLEAN TYPES IN DATA. Will not work with other types.
      // Tokenize line from file (separated on ,), convert to boolean values
      for ( unsigned int i = 0; i < line.length(); i++ )
      { 
        if ( line[i] != ',' )
        {
          featureVector.push_back( ((line[i] == '0') ? false : true) );
        }
      }

      m_d.push_back( featureVector );
    }

    // No data in files :(  
    if ( m_d.empty() || m_n.empty() )
    {
      return false;
    }
  }

  // If data file could not be opened
  else
  {
    return false;
  }  

  return true;
}

template <class T>
void DataSet<T>::print()
{
  for ( unsigned int i = 0; i < m_n.size(); i++ )
  {
    cout << m_n[i] << ",";
  }
  cout << endl;

  for ( unsigned int i = 0; i < m_d.size(); i++ )
  {
    for ( unsigned int j = 0; j < m_d[i].size(); j++ )
    {
      cout << m_d[i][j] << ",";
    }
    cout << endl;
  }
}

template <class T>
DataSet<T>& DataSet<T>::operator=( const DataSet<T>& rhs )
{
  // Only do assignment if RHS is a different object from this.
  if ( this != &rhs ) 
  {
    copyData( rhs );
    copyHeaders( rhs );
  }

  return *this;
}

template <class T>
void DataSet<T>::copyData( const DataSet<T>& rhs )
{
  if ( this != &rhs ) 
  {
    m_d.clear();
    m_d.assign( rhs.m_d.begin(), rhs.m_d.end() );
    POS = rhs.POS;
    NEG = rhs.NEG;
  }
}

template <class T>
void DataSet<T>::copyHeaders( const DataSet<T>& rhs )
{
  if ( this != &rhs ) 
  {
    m_n.clear();
    m_n.assign( rhs.m_n.begin(), rhs.m_n.end() );
    POS = rhs.POS;
    NEG = rhs.NEG;
  }
}

template <class T>
bool DataSet<T>::pure( unsigned int attr, T match ) const
{
  if ( attr >= m_n.size() || m_d.empty() )
  {
    return false;
  }

  // Search each feature vector at the specified column.
  for ( unsigned int i = 0; i < m_d.size(); i++ )
  {
    if ( m_d[i][attr] != match )
    {
      return false;
    }
  }

  return true;
}

template <class T>
T DataSet<T>::getMostCommonValue( unsigned int attr ) const
{
  if ( attr >= m_n.size() )
  {
    return static_cast<T>(-1);
  }

  map<T, unsigned int> valueCounts;
  getTValueCounts( attr, valueCounts );
  if ( valueCounts.empty() )
  {
    return static_cast<T>(-1);
  }

  unsigned int maxCount = 0;
  T maxElem = valueCounts.begin()->first;

  map<T, unsigned int>::iterator it;
  for ( it = valueCounts.begin(); it != valueCounts.end(); it++ )
  {
    if ( it->second > maxCount )
    {
      maxCount = it->second;
      maxElem = it->first;
    }
  }

  return maxElem;
}

template <class T>
void DataSet<T>::getUniqueValues( unsigned int attr, vector<T>& values ) const
{
  values.clear();

  if ( attr >= m_n.size() )
  {
    return;
  }

  map<T, unsigned int> valueCounts;
  getTValueCounts( attr, valueCounts );

  map<T, unsigned int>::iterator it;
  for ( it = valueCounts.begin(); it != valueCounts.end(); it++ )
  {
    values.push_back( it->first );
  }
}

template <class T>
void DataSet<T>::getMatchingVectors( DataSet<T>& dataMatch, 
                                    unsigned int attr, 
                                    T match ) const
{
  dataMatch.m_d.clear();

  if ( attr >= m_n.size() )
  {
    return;
  }

  for ( unsigned int i = 0; i < m_d.size(); i++ )
  {
    if ( m_d[i][attr] == match )
    {
      dataMatch.m_d.push_back( m_d[i] );
    }
  }
}

template <class T>
void DataSet<T>::getTValueCounts( unsigned int attr, 
                                 map<T, unsigned int>& counts ) const
{
  counts.clear();

  if ( attr >= m_n.size() )
  {
    return;
  }

  for ( unsigned int i = 0; i < m_d.size(); i++ )
  {
    // Increment the number of appearances in counts for attribute attr in the
    // ith feature vector
    counts[ m_d[i][attr] ]++;
  }
}

/*template <class T>
bool TCountsComparator( const pair<T, unsigned int>& lhs,
                          const pair<T, unsigned int>& rhs )
{
  return ( lhs.second < rhs.second );
}*/

/*bool TCountsComparator( const pair<bool, unsigned int>& lhs,
                          const pair<bool, unsigned int>& rhs )
{
  return ( lhs.second < rhs.second );
}*/

template <class T>
float DataSet<T>::entropy( unsigned int attr ) const
{
  if ( attr >= m_d.size() )
  {
    return -1.0f;
  }

  float sum = 0;
  map<T, unsigned int> valueCounts;
  getTValueCounts( attr, valueCounts );

  map<T, unsigned int>::iterator it;
  for ( it = valueCounts.begin(); it != valueCounts.end(); it++ )
  {
    float propI = static_cast<float>(it->second) / static_cast<float>(m_d.size());
    sum += ( -propI * (log(propI) / log(2.0f)) );
  }

  return sum;
}

template <class T>
float DataSet<T>::varianceImpurity( unsigned int target ) const
{
  unsigned int countPOS = 0;
  unsigned int countNEG = 0;

  for ( unsigned int i = 0; i < m_d.size(); i++ )
  {
    // Increment the number of appearances in counts for attribute attr in the
    // ith feature vector
    if ( m_d[i][target] == POS )
    {
      countPOS++;
    }

    else if ( m_d[i][target] == NEG )
    {
      countNEG++;
    }
  }

  return ( countPOS * countNEG / (m_d.size() * m_d.size()) );
}
