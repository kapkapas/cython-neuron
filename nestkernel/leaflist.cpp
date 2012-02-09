/*
 *  leaflist.cpp
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2004 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include "leaflist.h"
#include "nodelist.h"

#ifdef UNDEFINEDSTUFF
namespace nest{

  bool LeafList::empty() const
  {
    // A LeafList is empty, if all members of the NodeList are
    // not leaves.

    for (LocalNodeList::iterator i=LocalNodeList::begin(); i!=LocalNodeList::end(); ++i)
      {
        if (is_leaf_(*i))
          return false;
      }
    return true;
  }

  LeafList::iterator LeafList::begin() const
  { // we traverse the NodeList, and return the first element that is
    // a leaf, or end() if none exitsts.
    for (LocalNodeList::iterator i=LocalNodeList::begin(); i!=LocalNodeList::end(); ++i)
      {
        if (is_leaf_(*i))
          return iterator(i, *this);
      }
    return end();
  }


  /** 
   * LeafList::iterator::operator++()
   * Operator++ advances the iterator to the right neighbor
   * in a post-order tree traversal, excluding the non-leaf
   * nodes.
   * Note that this is the standard NEST counting order for element
   * counting in multidimensional subnets.
   */
  LeafList::iterator LeafList::iterator::operator++()
  {
    /**
     * We can assume that this operator is not called on
     * end(). For this case, the result is undefined!
     */

    // we use NodeList::operator++ for this, and call it until it
    // returns an element that is a leaf, or end()
    
    LocalNodeList::iterator i;

    do
      ++i;
    while ( (i != the_container_.end()) && !LeafList::is_leaf_(*i) );
    
    return iterator(i, the_container_);
  }


}
	
#endif
	
