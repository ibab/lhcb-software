// $Id: DaVinciAssociators_load.cpp,v 1.5 2002-09-12 12:16:12 gcorti Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

// local
#include "DaVinciAssociators/Particle2MCAsct.h"
#include "DaVinciAssociators/Particle2MCWithChi2Asct.h"
#include "DaVinciAssociators/ProtoParticle2MCAsct.h"

// LHCbKernel
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

IMPLEMENT_RelationW2D( ProtoParticle , MCParticle , float );
IMPLEMENT_RelationW1D( ProtoParticle , MCParticle , float );
IMPLEMENT_WAssociator( ProtoParticle , MCParticle , float );


DECLARE_TOOL_FACTORY( Particle2MCAsct);

  //static const ToolFactory<Particle2MCAsct>     s_Particle2MCAsctFactory; 
  //const       IToolFactory& Particle2MCAsctFactory = s_Particle2MCAsctFactory;

DECLARE_TOOL_FACTORY( ProtoParticle2MCAsct);
//static const ToolFactory<ProtoParticle2MCAsct>     
//             s_ProtoParticle2MCAsctFactory; 
//const       IToolFactory& ProtoParticle2MCAsctFactory =
//            s_ProtoParticle2MCAsctFactory;

DECLARE_TOOL_FACTORY( Particle2MCWithChi2Asct);
//static const ToolFactory<Particle2MCWithChi2Asct>     
//             s_Particle2MCWithChi2AsctFactory; 
//const       IToolFactory& 
//            Particle2MCWithChi2AsctFactory = s_Particle2MCWithChi2AsctFactory;

// Declare factory for the relations table

//DECLARE_OBJECT_FACTORY( Particle2MCWithChi2Table);
static const DataObjectFactory<Particle2MCWithChi2Table>
             s_Particle2MCWithChi2TableFactory;
const IFactory& Particle2MCWithChi2TableFactory =
                s_Particle2MCWithChi2TableFactory;

//DECLARE_OBJECT_FACTORY( Particle2MCTable);
static const DataObjectFactory<Particle2MCTable> 
             s_Particle2MCTableFactory;
const IFactory& Particle2MCTableFactory =
                s_Particle2MCTableFactory;

//DECLARE_OBJECT_FACTORY( ProtoParticle2MCTable);
static const DataObjectFactory<ProtoParticle2MCTable> 
             s_ProtoParticle2MCTableFactory;
const IFactory& ProtoParticle2MCTableFactory =
                s_ProtoParticle2MCTableFactory;


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
  DECLARE_TOOL( ProtoParticle2MCAsct );
 
  DECLARE_OBJECT( Particle2MCWithChi2Table );
  DECLARE_OBJECT( Particle2MCTable );
  DECLARE_OBJECT( ProtoParticle2MCTable );
 
  DECLARE_RelationW1D( ProtoParticle , MCParticle , float ) ;
  DECLARE_RelationW2D( ProtoParticle , MCParticle , float ) ;
  DECLARE_WAssociator( ProtoParticle , MCParticle , float ) ;
  

  DECLARE_ALGORITHM( Particle2MCWithChi2 );
  DECLARE_ALGORITHM( Particle2MCChi2 );
  DECLARE_ALGORITHM( Particle2MCLinks );
  DECLARE_ALGORITHM( ProtoParticle2MCLinks );
  
  DECLARE_ALGORITHM( NeutralPP2MC ) ;

}
