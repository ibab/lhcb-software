// $Id: Associators_load.cpp,v 1.2 2003-03-20 07:50:02 gcorti Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
//#include "Event/MCHit.h"
//#include "Event/MCParticle.h"
#include "Event/MCVeloHit.h"
#include "Event/L0CaloCandidate.h"
#include "Event/L1Track3d.h"
#include "Event/L1Track2d.h"

// LHCbKernel
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

IMPLEMENT_Relation1D( MCParticle, MCHit);
IMPLEMENT_Associator( MCParticle, MCHit);

IMPLEMENT_Relation1D( MCParticle, MCVeloHit);
IMPLEMENT_Associator( MCParticle, MCVeloHit);

IMPLEMENT_RelationW1D( L0CaloCandidate, MCParticle, double );
IMPLEMENT_WAssociator( L0CaloCandidate, MCParticle, double );

IMPLEMENT_RelationW1D( L1Track3d, MCParticle, double );
IMPLEMENT_WAssociator( L1Track3d, MCParticle, double );

IMPLEMENT_RelationW1D( L1Track2d, MCParticle, double );
IMPLEMENT_WAssociator( L1Track2d, MCParticle, double );

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

  DECLARE_RelationW1D( L1Track3d, MCParticle, double );
  DECLARE_WAssociator( L1Track3d, MCParticle, double );

  DECLARE_RelationW1D( L1Track2d, MCParticle, double );
  DECLARE_WAssociator( L1Track2d, MCParticle, double );

}  
