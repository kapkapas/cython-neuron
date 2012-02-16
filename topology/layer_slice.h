#ifndef LAYER_SLICE_H
#define LAYER_SLICE_H

/*
 *  layer_slice.h
 *
 *  This file is part of NEST
 *
 *  Copyright (C) 2010 by
 *  The NEST Initiative
 *
 *  See the file AUTHORS for details.
 *
 *  Permission is granted to compile and modify
 *  this file for non-commercial use.
 *  See the file LICENSE for details.
 *
 */

/*
    This file is part of the NEST topology module.
    Authors: Kittel Austvoll, Håkon Enger

*/

#include "subnet.h"
#include "iostreamdatum.h"
#include "nest.h"
#include "communicator_impl.h"
#include "network.h"
#include "proxynode.h"

#include "nodewrapper.h"
#include "selector.h"
#include "layer.h"
#include "layer_regular.h"
#include "layer_unrestricted.h"
#include "layer_3d.h"

namespace nest {
  template<class LayerType>
  class LayerSlice : public LayerType {
  public:
    /**
     * Copy constructor
     */
    LayerSlice(const LayerSlice& other);

    /**
     * Create a slice from another layer. The layer to slice will
     * normally be the same type as the slice (ie. the template
     * argument), but a common special case is when an unrestricted
     * slice is created from a regular layer.
     * @param layer Layer to slice
     * @param dict Dictionary containing the criterias 
     *             that the function extracts nodes based on. See the 
     *             documentation for the SLI topology/ConnectLayer 
     *             function.
     */
    template<class FromLayerType>
    LayerSlice(FromLayerType& layer, const DictionaryDatum& dict);

    /**
     * Destructor. Deletes the subnets and proxynodes created when slicing.
    */
    ~LayerSlice();

    const Position<double_t> get_position(index lid) const;

    /**
     * Function that retrieves nodes in the layer that covers a selected 
     * mask region centered around an input driver position.
     * @param driver  Position of the center of the mask region in the pool
     *                topological space (i.e. the position of the driver
     *                node overlapping this center).
     * @param region  Mask region where nodes are retrieved from.
     * @returns vector containing the node pointers and the node positions 
     *          of the nodes covering the input region centered around the 
     *          input driver coordinate.
     */
    lockPTR<std::vector<NodeWrapper> >
      get_pool_nodewrappers(const Position<double_t>& driver_coo,
			    AbstractRegion* region);

  protected:
    /**
     * Layer type specific initialization.
     */
    void init_internals();
    const Layer * original_layer_;
  };


  template<class LayerType> template<class FromLayerType>
  LayerSlice<LayerType>::LayerSlice(FromLayerType &layer,
				    const DictionaryDatum &layer_connection_dict):
  LayerType(layer),
  original_layer_(&layer)
  {
    this->nodes_.clear();

    // Create Selector object used to pick desired nodes from layer.
    Selector selector(layer_connection_dict);

    // iterate over the top level (layer level) and put all nodes at
    // one layer location into a subnet, then collect these subnets
    // in the results vector
    Subnet* layer_as_subnet = dynamic_cast<Subnet*>(&layer);
    assert(layer_as_subnet);
    LocalChildList local_layer_kids(*layer_as_subnet);
    vector<Communicator::NodeAddressingData> layer_kids;
    nest::Communicator::communicate(local_layer_kids, layer_kids, true);
    Network& netw = *Node::network();

    for ( std::vector<Communicator::NodeAddressingData>::iterator it = layer_kids.begin() ;
          it != layer_kids.end() ; ++it )
    {
      index gid = it->get_gid();
      Subnet* dest_subnet = new Subnet;

      Node* node = 0;
      if ( netw.is_local_gid(gid) && dynamic_cast<Subnet*>(netw.get_node(gid)) )
        node = netw.get_node(gid);
      else
        node = new proxynode(gid, it->get_parent_gid(), netw.get_model_id_of_gid(gid),
                             it->get_vp());

      selector.slice_node(*dest_subnet, *node);
      this->nodes_.push_back(dest_subnet);
    }

    // Do layer type specific initialization
    init_internals();
  }

  template<class LayerType>
  void inline LayerSlice<LayerType>::init_internals()
  {
    // No layer type specific initialization by default.
  }

  template<>
  void inline LayerSlice<LayerUnrestricted>::init_internals()
  {
    make_tree();
  }

  template<>
  void inline LayerSlice<Layer3D>::init_internals()
  {
    make_tree();
  }

  template<class LayerType>
  LayerSlice<LayerType>::~LayerSlice()
  {
    for(std::vector<Node*>::iterator it = this->local_begin();
        it != this->local_end(); ++it)
    {
      Subnet* c = dynamic_cast<Subnet*>(*it);
      assert(c);

      // delete any proxynodes in subnet, we know it is flat
      for ( std::vector<Node*>::iterator sit = c->local_begin();
            sit != c->local_end(); ++sit )
        if ( dynamic_cast<proxynode*>(*it) )
          delete *it;

      delete c;
    }
  }

  template<class LayerType>
  const Position<double_t> 
  inline LayerSlice<LayerType>::get_position(index lid) const
  {
    return LayerType::get_position(lid);
  }

  template<class LayerType>
  lockPTR<std::vector<NodeWrapper> > inline
  LayerSlice<LayerType>::get_pool_nodewrappers(const Position<double_t>& driver_coo,
			       AbstractRegion* region)
  {
    return LayerType::get_pool_nodewrappers(driver_coo, region);
  }


}

#endif
