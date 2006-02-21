// $Id: Associators_load.cpp,v 1.5 2006-02-21 08:17:28 mneedham Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/MCHit.h"
#include "Event/MCParticle.h"

// LHCbKernel
//#include "Relations/RelationMACROs.h"
//#include "Relations/AssociatorMACROs.h"

//IMPLEMENT_Relation1D( LHCb::MCParticle, LHCb::MCHit);
//IMPLEMENT_Associator( LHCb::MCParticle, LHCb::MCHit);

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(Associators) {



  DECLARE_ALGORITHM( MCParticle2MCHitAlg );
  
} ;

