//====================================================================
// veloEventInstantiation.cpp
//--------------------------------------------------------------------
//
//	Package   : VeloEvent
//
//	Author    : Chris Parkes
//  History   :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 02/04/02| Initial version for OO DST                   | CJP
// +---------+----------------------------------------------+---------
//====================================================================
#define LHCBEVENT_FACTORIES_MCINSTANTIATION_CPP  1

// Include files
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ObjectList.h"

#define IMPLEMENT_VELO_CONTAINED_FACTORIES(x)                             \
  static ContainedObjectFactory< x > s_##x##Factory;                    \
  static DataObjectFactory< ObjectVector < x > > s_##x##VectorFactory;  \
  static DataObjectFactory< ObjectList < x > > s_##x##ListFactory;

#define DECLARE_VELO_CONTAINEDOBJECT_FACTORIES(x)  \
  s_##x##Factory.addRef();                  \
  s_##x##VectorFactory.addRef();            \
  s_##x##VectorFactory.addRef();


// ====================================================================
// Object factory implementation for objects of class MCVeloHit
// ====================================================================
#include "Event/MCVeloHit.h"
IMPLEMENT_VELO_CONTAINED_FACTORIES(MCVeloHit)

// ====================================================================
// Object factory implementation for objects of class MCVeloFE
// ====================================================================
#include "Event/MCVeloFE.h"
IMPLEMENT_VELO_CONTAINED_FACTORIES(MCVeloFE)

// ====================================================================
// Object factory implementation for objects of class VeloFullDigit
// ====================================================================
#include "Event/VeloFullDigit.h"
IMPLEMENT_VELO_CONTAINED_FACTORIES(VeloFullDigit)

// ====================================================================
// Object factory implementation for objects of class VeloCluster
// ====================================================================
#include "Event/VeloCluster.h"
IMPLEMENT_VELO_CONTAINED_FACTORIES(VeloCluster)

void VeloEvent_load()  {
  // Declaration of contained object factories
  DECLARE_VELO_CONTAINEDOBJECT_FACTORIES( MCVeloHit );
  DECLARE_VELO_CONTAINEDOBJECT_FACTORIES( MCVeloFE );
  DECLARE_VELO_CONTAINEDOBJECT_FACTORIES( VeloFullDigit );
  DECLARE_VELO_CONTAINEDOBJECT_FACTORIES( VeloCluster );
}

extern "C" void VeloEvent_loadRef()   {
  VeloEvent_load();
}


