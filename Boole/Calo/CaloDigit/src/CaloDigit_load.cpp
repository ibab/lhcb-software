// $Id: CaloDigit_load.cpp,v 1.4 2003-11-18 10:20:24 ocallot Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"

// Event
#include "Event/KeyedObject.h"
#include "Event/MCParticle.h"

// TrFitEvent
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
