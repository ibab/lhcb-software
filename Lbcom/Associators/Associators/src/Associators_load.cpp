// $Id: Associators_load.cpp,v 1.1.1.1 2002-07-10 12:12:38 hierck Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
//#include "Event/MCHit.h"
#include "Event/MCVeloHit.h"
//#include "Event/MCParticle.h"

// LHCbKernel
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

IMPLEMENT_Relation1D( MCParticle, MCHit);
IMPLEMENT_Associator( MCParticle, MCHit);

IMPLEMENT_Relation1D( MCParticle, MCVeloHit);
IMPLEMENT_Associator( MCParticle, MCVeloHit);

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(Associators) {


  DECLARE_Relation1D( MCParticle, MCHit);
  DECLARE_Associator( MCParticle, MCHit);

  DECLARE_Relation1D( MCParticle, MCVeloHit);
  DECLARE_Associator( MCParticle, MCVeloHit);

  DECLARE_ALGORITHM( MCParticle2MCHitAlg );
  DECLARE_ALGORITHM( MCParticle2MCVeloHitAlg );
  
}  
