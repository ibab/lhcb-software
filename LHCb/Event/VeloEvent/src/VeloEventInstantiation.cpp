//====================================================================
// veloEventInstantiation.cpp
//--------------------------------------------------------------------
//
//	Package   : VeloEvent
//
//	Author    : Markus Frank
//  History   :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 21/07/99| Initial version                              | MF
// +---------+----------------------------------------------+---------
//====================================================================
#define LHCBEVENT_FACTORIES_MCINSTANTIATION_CPP  1

// RCS Id for identification of object version
static const char* rcsid = "$Id: VeloEventInstantiation.cpp,v 1.1.1.1 2000-11-23 13:39:24 ranjard Exp $";

// Include files
#include "GaudiKernel/ObjectFactory.h"
#include "LHCbEvent/ObjectVector.h"
#include "LHCbEvent/ObjectList.h"

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
#include "VeloEvent/MCVeloHit.h"
IMPLEMENT_VELO_CONTAINED_FACTORIES(MCVeloHit)

// ====================================================================
// Object factory implementation for objects of class RawVeloHit
// ====================================================================
#include "VeloEvent/RawVeloHit.h"
IMPLEMENT_VELO_CONTAINED_FACTORIES(RawVeloHit)

// ====================================================================
// Object factory implementation for objects of class VeloCluster
// ====================================================================
#include "VeloEvent/VeloCluster.h"
IMPLEMENT_VELO_CONTAINED_FACTORIES(VeloCluster)

// ====================================================================
// Object factory implementation for objects of class RecVeloEvent
// ===================================================================
#include "VeloEvent/RecVeloEvent.h"
static DataObjectFactory< RecVeloEvent > s_RecVeloEventFactory;

void VeloEventFactories_load()  {
  // Declaration of contained object factories
  DECLARE_VELO_CONTAINEDOBJECT_FACTORIES( MCVeloHit );
  DECLARE_VELO_CONTAINEDOBJECT_FACTORIES( RawVeloHit );
  DECLARE_VELO_CONTAINEDOBJECT_FACTORIES( VeloCluster );
  // Declaration of Data object factories
  s_RecVeloEventFactory.addRef();
}

extern "C" void VeloEventFactories_loadRef()   {
  VeloEventFactories_load();
}
