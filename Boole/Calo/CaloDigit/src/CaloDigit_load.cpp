// $Id: CaloDigit_load.cpp,v 1.5 2004-01-07 09:57:33 cattanem Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/MCParticle.h"
#include "Event/CaloDigit.h"

// LHCbKernel
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

IMPLEMENT_RelationW1D( CaloDigit, MCParticle, double );
IMPLEMENT_WAssociator( CaloDigit, MCParticle, double );
 
// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(CaloDigit) {

  DECLARE_RelationW1D( CaloDigit, MCParticle, double );
  DECLARE_WAssociator( CaloDigit, MCParticle, double );
 
  DECLARE_ALGORITHM( CaloSignalAlg );
  DECLARE_ALGORITHM( CaloDigitAlg  );
  DECLARE_ALGORITHM( CaloZSupAlg   );

  DECLARE_ALGORITHM( CaloDigitToMCParticleAsct );
}
