// $Id: DaVinciAssociators_load.cpp,v 1.10 2004-06-11 15:26:16 phicharp Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

// Event
#include "Event/MCParticle.h"
#include "Event/Particle.h"

// local
#include "DaVinciAssociators/Particle2MCAsct.h"
#include "DaVinciAssociators/Particle2MCLinksAsct.h"
#include "DaVinciAssociators/Particle2MCWithChi2Asct.h"
#include "DaVinciAssociators/ProtoParticle2MCAsct.h"

// LHCbKernel
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

DECLARE_TOOL_FACTORY( Particle2MCLinksAsct);
DECLARE_TOOL_FACTORY( Particle2MCWithChi2Asct);
DECLARE_TOOL_FACTORY( Particle2MCAsct);
DECLARE_TOOL_FACTORY( ProtoParticle2MCAsct);

DECLARE_DATAOBJECT_FACTORY( Particle2MCLinksTable);
//DECLARE_DATAOBJECT_FACTORY( Particle2MCWithChi2Table);
DECLARE_DATAOBJECT_FACTORY( Particle2MCTable);
DECLARE_DATAOBJECT_FACTORY( ProtoParticle2MCTable);


// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject )
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( DaVinciAssociators ) {

  DECLARE_TOOL( Particle2MCLinksAsct );
  DECLARE_TOOL( Particle2MCWithChi2Asct );
  DECLARE_TOOL( Particle2MCAsct );
  DECLARE_TOOL( ProtoParticle2MCAsct );
 
  DECLARE_DATAOBJECT( Particle2MCLinksTable );
  // The next one is not necessary as it is of the same type as for Links
//  DECLARE_DATAOBJECT( Particle2MCWithChi2Table );
  DECLARE_DATAOBJECT( Particle2MCTable );
  DECLARE_DATAOBJECT( ProtoParticle2MCTable );
 
  DECLARE_ALGORITHM( Particle2MCWithChi2 );
  DECLARE_ALGORITHM( Particle2MCChi2 );
  DECLARE_ALGORITHM( Particle2MCLinks );
  DECLARE_ALGORITHM( CompositeParticle2MCLinks );

  DECLARE_ALGORITHM( ChargedPP2MC );
  DECLARE_ALGORITHM( NeutralPP2MC ) ;

  DECLARE_ALGORITHM( TestAssociators ) ;
  DECLARE_ALGORITHM( TestLinker ) ;
  
}
