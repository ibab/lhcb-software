// $Id: DaVinciAssociators_load.cpp,v 1.3 2002-05-17 17:07:51 phicharp Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "DaVinciAssociators/Particle2MCAsct.h"
#include "DaVinciAssociators/Particle2MCWithChi2Asct.h"

static const ToolFactory<Particle2MCAsct>     s_Particle2MCAsctFactory; 
const       IToolFactory& Particle2MCAsctFactory = s_Particle2MCAsctFactory;

static const ToolFactory<Particle2MCWithChi2Asct>     
             s_Particle2MCWithChi2AsctFactory; 
const       IToolFactory& 
            Particle2MCWithChi2AsctFactory = s_Particle2MCWithChi2AsctFactory;

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( PhysAssociators ) {

  DECLARE_TOOL( Particle2MCWithChi2Asct );
  DECLARE_TOOL( Particle2MCAsct );
  
  DECLARE_ALGORITHM( Particle2MCWithChi2 );
  DECLARE_ALGORITHM( Particle2MCChi2 );
  DECLARE_ALGORITHM( Particle2MCLinks );

}
