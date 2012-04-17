#ifndef TOPOLOGY3MODULE_H
#define TOPOLOGY3MODULE_H

/*
 *  topology3module.h
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

#include "slimodule.h"
#include "layer.h"
#include "position.h"
#include "quadrant.h"
#include "exceptions.h"

namespace nest
{
   class Topology3Module: public SLIModule
   {
    public:

     Topology3Module(Network&);
     ~Topology3Module();

     /**
      * Initialize module by registering models with the network.
      * @param SLIInterpreter* SLI interpreter, must know modeldict
      */
     void init(SLIInterpreter*);

     const std::string name(void) const;
     const std::string commandstring(void) const;

     static SLIType MaskType;    ///< SLI type for masks

     /*
      * SLI functions: See source file for documentation
      */

     class CreateLayer_DFunction: public SLIFunction
     {
     public:
       void execute(SLIInterpreter *) const;
     } createlayer_Dfunction;

     class GetPosition_iFunction: public SLIFunction
     {
     public:
       void execute(SLIInterpreter *) const;
     } getposition_ifunction;

     class Displacement_a_iFunction: public SLIFunction
     {
     public:
       void execute(SLIInterpreter *) const;
     } displacement_a_ifunction;

     class Distance_a_iFunction: public SLIFunction
     {
     public:
       void execute(SLIInterpreter *) const;
     } distance_a_ifunction;

     class GetGlobalChildren_i_MFunction: public SLIFunction
     {
     public:
       void execute(SLIInterpreter *) const;
     } getglobalchildren_i_Mfunction;

     class CreateMask_DFunction: public SLIFunction
     {
     public:
       void execute(SLIInterpreter *) const;
     } createmask_Dfunction;

     class Inside_M_aFunction: public SLIFunction
     {
     public:
       void execute(SLIInterpreter *) const;
     } inside_M_afunction;

     class And_M_MFunction: public SLIFunction
     {
     public:
       void execute(SLIInterpreter *) const;
     } and_M_Mfunction;

     class Or_M_MFunction: public SLIFunction
     {
     public:
       void execute(SLIInterpreter *) const;
     } or_M_Mfunction;

     class Sub_M_MFunction: public SLIFunction
     {
     public:
       void execute(SLIInterpreter *) const;
     } sub_M_Mfunction;

     /**
      * Return a reference to the network managed by the topology module.
      */
     static Network &get_network();

   private:

     /**
      * GID for the single layer for which we cache global position information
      */
     index cached_layer_;

     /**
      * Global position information for a single layer
      */
     Quadrant<index> * cached_positions_;

     /**
      * - @c net must be static, so that the execute() members of the
      *   SliFunction classes in the module can access the network.
      */
     static Network* net_;
   };

  /**
   * Exception to be thrown if the wrong argument type
   * is given to a function
   * @ingroup KernelExceptions
   */
  class LayerExpected: public KernelException
  {
  public:
  LayerExpected()
    : KernelException("LayerExpected") {}
    ~LayerExpected() throw () {}

    std::string message();
  };

  inline
  Network &Topology3Module::get_network()
  {
    assert(net_ != 0);
    return *net_;
  }

} // namespace nest

#endif