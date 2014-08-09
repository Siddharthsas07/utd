#ifndef H_DATASET
#define H_DATASET

#include <vector>
#include <map>

using namespace std;

/**
 * \brief A table of data.
 */ 
template <class T>
class DataSet
{
  public:
    /**
     * \brief Loads a data set from file.
     * 
     * This function expects that the file located at path the format:
     * First line of file is a comma-separated list of attribute labels.
     * Subsequent lines of file are comma-separated lists of attribute values.
     * These subsequent lines are the feature vectors. 
     * For example:
     * [0] : A,B,C,D,E,Class
     * [1] : 0,1,1,1,1,0
     * [2] : 1,1,0,0,0,0
     * \param path -- A file path to the file containing the data. 
     */
    bool load( const string& path );

    /**
     * \brief Prints the DataSet to standard out.
     * \note Assumes row-major storage.
     */
    void print();

    /**
     * \brief Copies contents from rhs into this DataSet.
     * \param rhs -- Right hand side of the equation.
     */
    DataSet<T>& operator=( const DataSet<T>& rhs );

    /**
     * \brief Copies the data table from rhs into this DataSet.
     * \param rhs -- DataSet to copy from. 
     */
    void copyData( const DataSet<T>& rhs );

    /**
     * \brief Copies the headers from rhs into this DataSet.
     * \param rhs -- DataSet to copy from.
     */
    void copyHeaders( const DataSet<T>& rhs );

    /**
     * \brief Checks if all values of attribute attr in this DataSet == match.
     * \pre attr is in [0, m_d.size()).
     * \param attr -- The attribute (column) to check.
     * \param match -- The value to check for matches against.
     * \retval true -- All values for attribute attr == match.
     * \retval false -- Not all values for attribute attr == match.
     */
    bool pure( unsigned int attr, T match ) const;

    /** 
     * \brief Returns the most common value for attribute attr in this DataSet.
     * \pre attr is in [0, m_d.size()).
     * \param attr -- The attribute (column) to check.
     * \return The most common value for attribute attr.
     * \retval (T)(-1) A cast of -1 to type T if there exist no values for 
     *  attribute attr in the data. 
     */
    T getMostCommonValue( unsigned int attr ) const;

    /**
     * \brief Stores unique values for attribute attr from this DataSet in values.
     * \pre attr is in [0, m_d.size()).
     * \param attr -- The attribute (column) to check.
     * \param values -- The vector to store the values in. 
     */
    void getUniqueValues( unsigned int attr, vector<T>& values ) const ;

    /**
     * \brief Stores feature vectors with values for attribute attr equaling match.
     * \pre attr is in [0, m_d.size()).
     * \param dataMatch -- The DataSet to store the matched feature vectors in.
     * \param attr -- The attribute (column) to check.
     * \param match -- The value to check for matches against.
     */
    void getMatchingVectors( DataSet<T>& dataMatch, unsigned int attr, T match ) const;

    /**
     * \brief Gets the number of appearances for each unique value of attribute
     *  attr in this DataSet.
     * \pre attr is in [0, m_d.size()).
     * \param attr -- The attribute (column) to get the counts of.
     * \param counts -- The map to store the counts in. The counts are stored
     *  in as <T, count> 2-tuples where each T is a unique value assigned to attr
     *  in the column.
     */
    void getTValueCounts( unsigned int attr, map<T, unsigned int>& counts ) const;

    /**
     * \brief Calculates the entropy of this DataSet.
     * \pre attr is in [0, m_d.size()).
     * \param attr -- The attribute (column) to calculate the entropy for.
     * \return The entropy value for the data with respect to attribute attr.
     */
    float entropy( unsigned int attr ) const;

    /**
     * \brief Calculates the variance of the impurity of this DataSet.
     * \pre attr is in [0, m_d.size()).
     * \return The Variance Impurity of the DataSet with respect to the class.
     */
    float varianceImpurity( unsigned int target ) const;

    /**
     * \brief The data for the table stored as a 2D vector.
     *
     * This data can be considered in row-major or column-major form depending
     * on how it is accessed.
     */
    vector<vector<T> > m_d;

    /**
     * \brief A vector of column headers for the data table.
     * 
     * This is expected to contain the same number of elements as a feature 
     * vector contained in m_d.
     */
    vector<string> m_n;

    /**
     * \brief The value representing a POSITIVE feature vector (yes).
     */
    T POS;

    /**
     * \brief The value representing a NEGATIVE feature vector (no).
     */
    T NEG;
};

#include "dataset.hpp"

#endif