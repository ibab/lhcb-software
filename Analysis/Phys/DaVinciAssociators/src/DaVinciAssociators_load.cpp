// $Id: DaVinciAssociators_load.cpp,v 1.6 2002-10-02 07:06:28 phicharp Exp $
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

DECLARE_TOOL_FACTORY( Particle2MCWeightedAsct);
DECLARE_TOOL_FACTORY( Particle2MCAsct);
DECLARE_TOOL_FACTORY( ProtoParticle2MCAsct);

// Declare factory for the relations table
#ifndef DECLARE_OBJECT_FACTORY
#define DECLARE_OBJECT_FACTORY(x) \
static const DataObjectFactory<x> s_##x##Factory; \
const IFactory& x##Factory = s_##x##Factory;
#endif

DECLARE_OBJECT_FACTORY( Particle2MCWeightedTable);
DECLARE_OBJECT_FACTORY( Particle2MCTable);
DECLARE_OBJECT_FACTORY( ProtoParticle2MCTable);


// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject )
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( PhysAssociators ) {

  DECLARE_TOOL( Particle2MCWeightedAsct );
  DECLARE_TOOL( Particle2MCAsct );
  DECLARE_TOOL( ProtoParticle2MCAsct );
 
  DECLARE_OBJECT( Particle2MCWeightedTable );
  DECLARE_OBJECT( Particle2MCTable );
  DECLARE_OBJECT( ProtoParticle2MCTable );
 
  DECLARE_ALGORITHM( Particle2MCWithChi2 );
  DECLARE_ALGORITHM( Particle2MCChi2 );
  DECLARE_ALGORITHM( Particle2MCLinks );

  DECLARE_ALGORITHM( ChargedPP2MC );
  DECLARE_ALGORITHM( NeutralPP2MC ) ;

  DECLARE_ALGORITHM( TestAssociators ) ;
  
}
