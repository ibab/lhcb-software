// Include files 

#include "Relations/AssociatorMACROs.h"
#include "Event/VeloCluster.h"
#include "Event/MCParticle.h"

//IMPLEMENT_Associator( VeloCluster, MCParticle );


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(VeloAlgorithms) {
  DECLARE_ALGORITHM( VeloSim );
  DECLARE_ALGORITHM( VeloDataProcessor );
  DECLARE_ALGORITHM( VeloClusterMaker );
}

