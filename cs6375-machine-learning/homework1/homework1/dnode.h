#ifndef H_DNODE
#define H_DNODE

#include <vector>
#include <string>

using namespace std;

/**
 * \brief A node to use in the decision tree.
 */
template <class T>
class DNode
{
  public:
    /**
     * \brief Default constructor.
     */
    DNode<T>() : m_label(0)
    {
      m_parent = NULL;
    }

    /**
     * \brief Constructor specifying a parent node.
     * \param parent -- A pointer to the parent DNode. Must be of same data type.
     */
    DNode<T>( DNode<T>* parent )
    {
      m_parent = parent;
    }

    /**
     * \brief Constructor specifying a parent node, label, and branching attribute.
     * \param parent -- A pointer to the parent DNode. Must be of same data type.
     * \param branch -- The attribute index this node was branched on. 
     */

    DNode<T>( DNode<T>* parent, unsigned int branch ) : m_branchAttr(branch)
    {
      m_parent = parent;
    }

    /**
     * \brief The index of the attribute this node. Corresponds directly to 
     * the attribute (column number) in the DataSet this node was generated from.
     * \note A label of -1 implies this is a leaf node.
     */
    unsigned int m_branchAttr;

    /**
     * \brief The human readable label for the attribute this node branched on.
     */
    string m_label;

    /**
     * \brief A pointer to this node's parent node.
     */
    DNode<T> * m_parent;

    /**
     * \brief A vector of 2-tuples containing pointers to children and branching 
     *  values.
     *
     * The first element of each pair is a pointer to a child node of this node.
     * The second element of each pair is the attribute value for which attribute
     * m_label was branched on for this child.
     */    
    vector<pair<DNode<T>*, T> > m_branches;
};

#endif 
