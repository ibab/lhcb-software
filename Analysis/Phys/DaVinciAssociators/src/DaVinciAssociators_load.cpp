// $Id: DaVinciAssociators_load.cpp,v 1.12 2006-06-23 14:54:38 phicharp Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject )
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( DaVinciAssociators ) {
 
  DECLARE_ALGORITHM( Particle2MCWithChi2 );
  DECLARE_ALGORITHM( Particle2MCChi2 );
  DECLARE_ALGORITHM( Particle2MCLinks );
  DECLARE_ALGORITHM( CompositeParticle2MCLinks );

  DECLARE_ALGORITHM( ChargedPP2MC );
  DECLARE_ALGORITHM( NeutralPP2MC ) ;

  DECLARE_ALGORITHM( TestLinker ) ;
  
}
