//====================================================================
// L0EventInstantiation.cpp
//--------------------------------------------------------------------
//
//      Package   : L0Event
//
//      Author    : Bruce Hay (after original for L1 by Markus Frank)
//====================================================================
#define LHCBEVENT_FACTORIES_MCINSTANTIATION_CPP  1

// Include files
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ObjectList.h"

#define IMPLEMENT_L0_CONTAINED_FACTORIES(x)                             \
  static ContainedObjectFactory< x > s_##x##Factory;                    \
  static DataObjectFactory< ObjectVector < x > > s_##x##VectorFactory;  \
  static DataObjectFactory< ObjectList < x > >    s_##x##ListFactory;

#define DECLARE_L0_CONTAINEDOBJECT_FACTORIES(x)   \
  s_##x##Factory.addRef();                        \
  s_##x##VectorFactory.addRef();                  \
  s_##x##VectorFactory.addRef();


// ====================================================================
// Object factory implementation for objects of class Level0Calo2x2Sum
// ====================================================================
#include "L0Event/Level0Calo2x2Sum.h"
IMPLEMENT_L0_CONTAINED_FACTORIES(Level0Calo2x2Sum)

// ====================================================================
// Object factory implementation for objects of class Level0CaloClus
// ====================================================================
#include "L0Event/Level0CaloClus.h"
IMPLEMENT_L0_CONTAINED_FACTORIES(Level0CaloClus)

// ====================================================================
// Object factory implementation for objects of class Level0MuonTrack
// ====================================================================
#include "L0Event/Level0MuonTrack.h"
IMPLEMENT_L0_CONTAINED_FACTORIES(Level0MuonTrack)

// ====================================================================
// Object factory implementation for objects of class Level0PileUpVeto
// ====================================================================
#include "L0Event/Level0PileUpVeto.h"
static DataObjectFactory<Level0PileUpVeto> s_Level0PileUpVetoFactory;

void L0Event_load()  {
  // Declaration of contained object factories
  DECLARE_L0_CONTAINEDOBJECT_FACTORIES(Level0Calo2x2Sum);
  DECLARE_L0_CONTAINEDOBJECT_FACTORIES(Level0CaloClus);
  DECLARE_L0_CONTAINEDOBJECT_FACTORIES(Level0MuonTrack);
  // Declaration of Data object factories
  s_Level0PileUpVetoFactory.addRef();
}

extern "C" void L0Event_loadRef()   {
  L0Event_load();
}



