// $Id: Associators_load.cpp,v 1.3 2004-06-11 12:31:11 cattanem Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
//#include "Event/MCHit.h"
//#include "Event/MCParticle.h"
#include "Event/MCVeloHit.h"
#include "Event/L0CaloCandidate.h"

// LHCbKernel
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

IMPLEMENT_Relation1D( MCParticle, MCHit);
IMPLEMENT_Associator( MCParticle, MCHit);

IMPLEMENT_Relation1D( MCParticle, MCVeloHit);
IMPLEMENT_Associator( MCParticle, MCVeloHit);

IMPLEMENT_RelationW1D( L0CaloCandidate, MCParticle, double );
IMPLEMENT_WAssociator( L0CaloCandidate, MCParticle, double );

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
  
  DECLARE_RelationW1D( L0CaloCandidate, MCParticle, double );
  DECLARE_WAssociator( L0CaloCandidate, MCParticle, double );
}  
