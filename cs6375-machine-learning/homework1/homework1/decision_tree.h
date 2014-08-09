#ifndef H_DECISION_TREE
#define H_DECISION_TREE

#include <vector>
#include <map>
#include "dnode.h"
#include "dataset.h"
#include "d_tree_heuristic.h"

using namespace std;

/**
 * \brief Contains storage and functions for creating a decision tree. 
 */
template <class T>
class DecisionTree
{
  public:
    /**
     * \brief Default constructor. Sets contained pointers to NULL.
     */
    DecisionTree<T>();

    /** 
     * \brief Default destructor. Clears memory used by tree.
     */
    ~DecisionTree<T>();    

    /**
     * \brief Deep copy rhs into this tree.
     * \param rhs -- The DecisionTree to copy.
     * \return Reference to the newly copied tree.
     */
    DecisionTree<T>& operator=( const DecisionTree<T>& rhs );

    /**
     * \brief Generates a decision tree using the ID3 algorithm. 
     * \pre target is in [0, data.m_d.size()).
     * \param data -- The DataSet to use as training data to generate the tree. 
     * \param target -- The target attribute in data to create the decision tree for. 
     * \note Assumes that all attributes in data are available for branching on.
     */
    void ID3( const DataSet<T>& data, unsigned int target );

    /**
     * \brief Prunes a generated tree.
     * \pre target is in [0, data.m_d.size()).
     * \param data -- The data to use as the validation set for pruning. 
     * \param target -- The target attribute in data to prune the decision tree for.
     * \param numTrees -- The number of candidate trees to consider.
     * \param K -- A random number for the algorithm to use.
     */
    void prune( const DataSet<T>& trainingData, 
      const DataSet<T>& validationData,
      unsigned int target, 
      unsigned int numTrees, 
      unsigned int K );

    /**
     * \brief Tests the accuracy of the tree against the given DataSet.
     * \pre data has been populated properly with headers, POS, NEG, etc.
     * \param data -- The DataSet to test against the tree.
     * \param target -- The class attribute column in data.
     * \return The accuracy of the tree's classification.
     */
    float test( const DataSet<T>& data, unsigned int target );

    /**
     * \brief Sets the heuristic to use for tree generation.
     * \param h -- The heuristic to use for tree generation. At each decision
     *  for branching, this will tell the tree which attribute to branch on.
     */
    void setHeuristic( DTreeHeuristic<T>* h );

    /**
     * \brief Returns the pointer to this object's root node.
     * \return Pointer to the tree's root node.
     */
    DNode<T>* root();

    /** 
     * \brief Clears the memory used by the tree and resets everything to
     *  default values.
     */
    void clear();

    /** 
     * \brief Returns whether the tree has any nodes.
     * \retval true -- The tree has zero nodes.
     * \retval false -- The tree has at least one node.
     */
    bool empty();

    /**
     * \brief Returns the number of nodes in the tree.
     * \return The number of nodes in the tree.
     */
    unsigned int size();

    /**
     * \brief Prints the tree to standard out.
     */
    void print();
    
  protected:
    
    /**
     * \brief Clears the memory used by the tree and resets everything to
     *  default values.
     */
    void f_deleteTree( DNode<T>* root );

    /**
     * \brief Copy tree pointed to in rhs to lhs!
     * \param rhs -- The tree rooted at rhs to copy into this tree.
     * \note This is a deep copy.
     */
    void copy( const DecisionTree<T>* rhs );

    /**
     * \brief Performs a deep copy of the tree rooted at rhs into lhs.
     * \param lhs -- The root for the tree to copy into.
     * \param rhs -- The root for the tree to copy from.
     */
    void f_copyTree( DNode<T>* lhs, DNode<T>* rhs );

    /**
     * \brief Generates a decision tree using the ID3 algorithm. 
     * \pre target is in [0, data.m_d.size()).
     * \param data -- The DataSet to use as training data to generate the tree. 
     * \param target -- The target attribute in data to create the decision tree for. 
     * \param attrAvail -- The list of attributes the ID3 algorithm will consider as
     *  available to create branches for.
     * \param root -- The node the ID3 algorithm will consider for labeling and 
     *  branching from.
     */
    void f_ID3( const DataSet<T>& data, 
      unsigned int target, 
      const vector<unsigned int>& attrAvail, 
      DNode<T>* root );

    /**
     * \brief Prints out the node pointed to by root and proceeds to print
     *  out any children root may have.
     * \param root -- The root node of tree to print. This may be a subtree.
     * \param depth -- The depth of the root node in a given tree.
     */
    void f_print( DNode<T>* root, unsigned int depth );
    
    /**
     * \brief Classifies the feature vector v against the tree and returns the
     *  classification.
     * \pre target is in [0, v.size()).
     * \param v -- The feature vector to classify.
     * \param root -- The root of the tree to start using to classify v.
     * \return The tree's classification value for v.
     */
    T classify( const vector<T>& v, DNode<T>* root );

    /**
     * \brief Randomly chooses and prunes a subtree, replacing it with a 
     *  child node whose value is the majority value for target in data
     *  at the time of pruning.
     * \param tree -- The tree to search for internal nodes.
     * \param root -- The root of thre tree to search.
     * \param data -- The training data that created the tree.
     * \param target -- The class column of data.
     * \param depth -- The depth of the search thus far in the tree for
     *  nodes to prune.
     */
    void chooseAndPrune( DecisionTree<T>& tree,
      DNode<T>* root,
      const DataSet<T>& data, 
      unsigned int target,
      unsigned int depth );

    /**
     * \brief Prunes the subtree rooted at root.
     * \param tree -- The DecisionTree root is part of.
     * \param root -- The root of the tree to prune.
     */
    void f_prune( DecisionTree<T>& tree, DNode<T>* root );

    /**
     * \brief Creates a new branch to an empty child node from the specified parent.
     * \param parent -- The node to treat as the parent. The new branch will
     *  be created in this node and the branch will point to a new, empty child.
     * \return A pointer to the newly created child.
     */
    DNode<T>* addBranch( DNode<T>* parent );

    /**
     * \brief Creates a new branch to an empty child node from the specified parent.
     * \param parent -- The node to treat as the parent. The new branch will
     *  be created in this node and the branch will point to a new, empty child.
     * \param attrBranch -- The index in m_attrAvail for which this child 
     *  was branched on. 
     * \return A pointer to the newly created child.
     */
    DNode<T>* addBranch( DNode<T>* parent, T attrBranch );

    /**
     * \brief Creates a new branch to the specified child from the specified parent.
     * \param parent -- The node to treat as the parent. The new branch will be
     *  created in this node and the branch will point to the specified child.
     * \param child -- The node to treat as the child. The branch will point
     *  from the parent to this child.
     * \return A pointer to the specified child. 
     */
    DNode<T>* addBranch( DNode<T>* parent, DNode<T>* child );

    /**
     * \brief Converts a node to a leaf node.
     * \param node -- Pointer to the node to be converted.
     * \param value -- The value for the leaf node to hold.
     * \note This assumes that node is a newly created node. Expects that 
     *  there are zero branches and the parent pointer has already been set.
     */
    void convertToLeaf( DNode<T>* node, T value );

    /**
     * \brief Tells whether a node is a leaf node or not.
     * \param node -- Pointer to the node to check.
     * \retval true If the node is a leaf node.
     * \retval false If the node is not a leaf node. 
     */
    bool isLeaf( DNode<T>* node );


    /**
     * \brief A pointer to the root node of the tree.
     */
    DNode<T> * m_root;

    /**
     * \brief A pointer to the heuristic the tree will use when creating the
     *  tree.
     */
    DTreeHeuristic<T> * m_heuristic;
    
  private:

    /**
     * \brief The number of nodes in the tree.
     */
    unsigned int m_size;

    /**
     * \brief The number of leaf nodes in the tree.
     */
    unsigned int m_num_leaves;
};

#include "decision_tree.hpp"

#endif