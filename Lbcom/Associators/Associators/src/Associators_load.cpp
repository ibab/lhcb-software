// $Id: Associators_load.cpp,v 1.4 2004-07-29 16:37:09 ibelyaev Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/MCVeloHit.h"
#include "Event/L0CaloCandidate.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/TrStoredTrack.h"

// LHCbKernel
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

IMPLEMENT_Relation1D( MCParticle, MCHit);
IMPLEMENT_Associator( MCParticle, MCHit);

IMPLEMENT_Relation1D( MCParticle, MCVeloHit);
IMPLEMENT_Associator( MCParticle, MCVeloHit);

IMPLEMENT_RelationW1D( L0CaloCandidate, MCParticle, double );
IMPLEMENT_WAssociator( L0CaloCandidate, MCParticle, double );

/// factory for relations between CaloClusters and MCParticles 
IMPLEMENT_RelationW1D ( CaloCluster   , MCParticle    , float ) ;
IMPLEMENT_RelationW2D ( CaloCluster   , MCParticle    , float ) ;
/// factory for relations between clusters  and tracks
IMPLEMENT_RelationW1D ( CaloCluster   , TrStoredTrack , float ) ;
IMPLEMENT_RelationW2D ( CaloCluster   , TrStoredTrack , float ) ;
/// factory for relations between hypos     and tracks
IMPLEMENT_RelationW1D ( CaloHypo      , TrStoredTrack , float ) ;
IMPLEMENT_RelationW2D ( CaloHypo      , TrStoredTrack , float ) ;
/// add some weight ( namely the ID estimator ) for tracks 
IMPLEMENT_Relation1D  (                 TrStoredTrack , float ) ;
/// associators 
IMPLEMENT_WAssociator ( CaloCluster   , MCParticle    , float ) ;
IMPLEMENT_WAssociator ( CaloCluster   , TrStoredTrack , float ) ;
IMPLEMENT_WAssociator ( CaloHypo      , TrStoredTrack , float ) ;
IMPLEMENT_Associator  (                 TrStoredTrack , float ) ;


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

  DECLARE_RelationW1D ( CaloCluster , MCParticle    , float ) ;
  DECLARE_RelationW2D ( CaloCluster , MCParticle    , float ) ;
  DECLARE_RelationW1D ( CaloCluster , TrStoredTrack , float ) ;
  DECLARE_RelationW2D ( CaloCluster , TrStoredTrack , float ) ;
  DECLARE_RelationW1D ( CaloHypo    , TrStoredTrack , float ) ;
  DECLARE_RelationW2D ( CaloHypo    , TrStoredTrack , float ) ;
  DECLARE_Relation1D  (               TrStoredTrack , float ) ;

  DECLARE_WAssociator ( CaloCluster , MCParticle    , float ) ;
  DECLARE_WAssociator ( CaloCluster , TrStoredTrack , float ) ;
  DECLARE_WAssociator ( CaloHypo    , TrStoredTrack , float ) ;
  DECLARE_Associator  (               TrStoredTrack , float ) ;
  
} ;

