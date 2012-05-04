#ifndef NTREE_H
#define NTREE_H

/*
 *  ntree.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2012 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

#include <vector>
#include <utility>
#include "position.h"

namespace nest
{

  class AbstractMask;

  template<int D>
  class Mask;

  /**
   * Abstract base class for all Ntrees containing items of type T
   */
  template<class T>
  class AbstractNtree
  {
  public:
    /**
     * @returns member nodes in ntree without positions.
     */
    virtual std::vector<T> get_nodes_only() = 0;

    /**
     * Applies a Mask to this ntree.
     * @returns member nodes in ntree inside mask without positions.
     */
    virtual std::vector<T> get_nodes_only(const AbstractMask &mask, const std::vector<double_t> &anchor) = 0;
  };

  /**
   * A Ntree object represents a subtree or leaf in a Ntree structure. Any
   * ntree covers a specific region in space. A leaf ntree contains a list
   * of items and their corresponding positions. A branch ntree contains a
   * list of N=1<<D other ntrees, each covering a region corresponding to the
   * upper-left, lower-left, upper-right and lower-left corner of their
   * mother ntree.
   *
   */
  template<int D, class T, int max_capacity=100>
  class Ntree : public AbstractNtree<T>
  {
  public:

    static const int N = 1<<D;

    typedef Position<D> key_type;
    typedef T mapped_type;
    typedef std::pair<Position<D>,T> value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    /**
     * Iterator iterating the nodes in a Quadtree.
     */
    class iterator {
    public:
      /**
       * Initialize an invalid iterator.
       */
      iterator() : ntree_(0), top_(0), node_(0) {}

      /**
       * Initialize an iterator to point to the first node in the first
       * non-empty leaf within the tree below this Ntree.
       */
      iterator(Ntree& q);

      /**
       * Initialize an iterator to point to the nth node in this Ntree,
       * which must be a leaf. The top of the tree is the first ancestor of
       * the Ntree.
       */
      iterator(Ntree& q, index n);

      value_type & operator*() { return ntree_->nodes_[node_]; }
      value_type * operator->() { return &ntree_->nodes_[node_]; }

      /**
       * Move the iterator to the next node within the tree. May cause the
       * iterator to become invalid if there are no more nodes.
       */
      iterator & operator++();

      /**
       * Postfix increment operator.
       */
      iterator & operator++(int)
        {
          iterator tmp = *this;
          ++*this;
          return tmp;
        }

      /**
       * Iterators are equal if they point to the same node in the same
       * ntree.
       */
      bool operator==(const iterator &other) const
        { return (other.ntree_==ntree_) && (other.node_==node_); }
      bool operator!=(const iterator &other) const
        { return (other.ntree_!=ntree_) || (other.node_!=node_); }

    protected:

      /**
       * Move to the next leaf quadrant, or set ntree_ to 0 if there are no
       * more leaves.
      */
      void next_leaf_();

      Ntree *ntree_;
      Ntree *top_;
      index node_;
    };

    /**
     * Iterator iterating the nodes in a Quadtree inside a Mask.
     */
    class masked_iterator {
    public:
      /**
       * Initialize an invalid iterator.
       */
      masked_iterator() : ntree_(0), top_(0), allin_top_(0), node_(0), mask_(0) {}

      /**
       * Initialize an iterator to point to the first leaf node inside the
       * mask within the tree below this Ntree.
       */
      masked_iterator(Ntree& q, const Mask<D> &mask, const Position<D> &anchor);

      value_type & operator*() { return ntree_->nodes_[node_]; }
      value_type * operator->() { return &ntree_->nodes_[node_]; }

      /**
       * Move the iterator to the next node inside the mask within the
       * tree. May cause the iterator to become invalid if there are no
       * more nodes.
       */
      masked_iterator & operator++();

      /**
       * Postfix increment operator.
       */
      masked_iterator & operator++(int)
        {
          masked_iterator tmp = *this;
          ++*this;
          return tmp;
        }

      /**
       * Iterators are equal if they point to the same node in the same
       * ntree.
       */
      bool operator==(const masked_iterator &other) const
        { return (other.ntree_==ntree_) && (other.node_==node_); }
      bool operator!=(const masked_iterator &other) const
        { return (other.ntree_!=ntree_) || (other.node_!=node_); }

    protected:

      /**
       * Find the next leaf which is not outside the mask.
       */
      void next_leaf_();

      /**
       * Find the first leaf which is not outside the mask. If no leaf is
       * found below the current quadrant, will continue to next_leaf_().
       */
      void first_leaf_();

      /**
       * Set the allin_top_ to the current quadrant, and find the first
       * leaf below the current quadrant.
       */
      void first_leaf_inside_();

      Ntree *ntree_;
      Ntree *top_;
      Ntree *allin_top_;
      index node_;
      const Mask<D> *mask_;
      Position<D> anchor_;
    };

    /**
     * Create a Ntree that covers the region defined by the two
     * input positions.
     * @param lower_left  Lower left corner of ntree.
     * @param extent      Size (width,height) of ntree.
     */
    Ntree(const Position<D>& lower_left,
	     const Position<D>& extent, Ntree *parent, int subquad);

    /**
     * Traverse quadtree structure from current ntree.
     * Inserts node in correct leaf in quadtree.
     * @returns iterator pointing to inserted node.
     */
    iterator insert(const Position<D>& pos, const T& node);

    /**
     * std::multimap like insert method
     */
    iterator insert(const value_type& val);

    /**
     * STL container compatible insert method (the first argument is ignored)
     */
    iterator insert(iterator iter, const value_type& val);

    /**
     * @returns member nodes in ntree and their position.
     */
    std::vector<value_type> get_nodes();

    /**
     * Applies a Mask to this ntree.
     * @param mask    mask to apply.
     * @param anchor  position to center mask in.
     * @returns member nodes in ntree inside mask.
     */
    std::vector<value_type> get_nodes(const Mask<D> &mask, const Position<D> &anchor);

    /**
     * @returns member nodes in ntree without positions.
     */
    std::vector<T> get_nodes_only();

    /**
     * Applies a Mask to this ntree.
     * @param mask    mask to apply.
     * @param anchor  position to center mask in.
     * @returns member nodes in ntree inside mask without positions.
     */
    std::vector<T> get_nodes_only(const AbstractMask &mask, const std::vector<double_t> &anchor);

    /**
     * This function returns a node iterator which will traverse the
     * subtree below this Ntree.
     * @returns iterator for nodes in quadtree.
     */
    iterator begin()
      { return iterator(*this); }

    iterator end()
      { return iterator(); }

    /**
     * This function returns a masked node iterator which will traverse the
     * subtree below this Ntree, skipping nodes outside the mask.
     * @returns iterator for nodes in quadtree.
     */
    masked_iterator masked_begin(const Mask<D> &mask, const Position<D> &anchor)
      { return masked_iterator(*this,mask,anchor); }

    masked_iterator masked_end()
      { return masked_iterator(); }

    /**
     * @returns true if ntree is a leaf.
     */
    bool is_leaf() const;

  protected:
    /**
     * Change a leaf ntree to a regular ntree with four
     * children regions.
     */
    void split_();

    /**
     * Append this ntree's nodes to the vector
     */
    void append_nodes_(std::vector<value_type>&);

    /**
     * Append this ntree's nodes inside the mask to the vector
     */
    void append_nodes_(std::vector<value_type>&, const Mask<D> &, const Position<D> &);

    /**
     * @returns the subquad number for this position
     */
    int subquad_(const Position<D>&);

    Position<D> lower_left_;
    Position<D> extent_;

    bool leaf_;

    std::vector<value_type> nodes_;

    Ntree* parent_;
    int my_subquad_;    ///< This Ntree's subquad number within parent
    Ntree* children_[N];

    friend class iterator;
    friend class masked_iterator;
  };

  template<int D, class T, int max_capacity>
  Ntree<D,T,max_capacity>::Ntree(const Position<D>& lower_left,
                                     const Position<D>& extent,
                                     Ntree<D,T,max_capacity>* parent=0,
                                     int subquad=0) :
    lower_left_(lower_left),
    extent_(extent),
    leaf_(true),
    parent_(parent),
    my_subquad_(subquad)
  {
  }

  template<int D, class T, int max_capacity>
  Ntree<D,T,max_capacity>::iterator::iterator(Ntree& q, index n):
    ntree_(&q), top_(&q), node_(n)
  {
    assert(ntree_->leaf_);

    // First ancestor
    while(top_->parent_)
      top_ = top_->parent_;
  }

  template<int D, class T, int max_capacity>
  bool Ntree<D,T,max_capacity>::is_leaf() const
  {
    return leaf_;
  }


  template<int D, class T, int max_capacity>
  std::vector<std::pair<Position<D>,T> > Ntree<D,T,max_capacity>::get_nodes()
  {
    std::vector<std::pair<Position<D>,T> > result;
    append_nodes_(result);
    return result;
  }

  template<int D, class T, int max_capacity>
  std::vector<std::pair<Position<D>,T> > Ntree<D,T,max_capacity>::get_nodes(const Mask<D> &mask, const Position<D> &anchor)
  {
    std::vector<std::pair<Position<D>,T> > result;
    append_nodes_(result,mask,anchor);
    return result;
  }

  template<int D, class T, int max_capacity>
  typename Ntree<D,T,max_capacity>::iterator Ntree<D,T,max_capacity>::insert(const std::pair<Position<D>,T>& val)
  {
    return insert(val.first,val.second);
  }

  template<int D, class T, int max_capacity>
  typename Ntree<D,T,max_capacity>::iterator Ntree<D,T,max_capacity>::insert(iterator iter, const std::pair<Position<D>,T>& val)
  {
    return insert(val.first,val.second);
  }

} // namespace nest

#endif
