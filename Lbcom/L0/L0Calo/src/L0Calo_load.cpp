// $Id: L0Calo_load.cpp,v 1.4 2002-07-01 10:57:01 ocallot Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"

// Event
#include "Event/KeyedObject.h"
#include "Event/MCParticle.h"

// TrFitEvent
#include "Event/L0CaloCandidate.h"

// LHCbKernel
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

IMPLEMENT_RelationW1D( L0CaloCandidate, MCParticle, double );
IMPLEMENT_WAssociator( L0CaloCandidate, MCParticle, double );
 
// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(L0Calo) {

  DECLARE_RelationW1D( L0CaloCandidate, MCParticle, double );
  DECLARE_WAssociator( L0CaloCandidate, MCParticle, double );
 

  DECLARE_ALGORITHM( L0CaloDigit );
  DECLARE_ALGORITHM( L0CaloAlg   );
  DECLARE_ALGORITHM( L0CaloMonit );
  DECLARE_ALGORITHM( L0CaloToMCParticleAsct );
}
