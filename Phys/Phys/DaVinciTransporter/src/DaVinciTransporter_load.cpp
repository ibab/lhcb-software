// $Id:
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

DECLARE_FACTORY_ENTRIES( DaVinciTransporter ) {

   DECLARE_TOOL( ParticleTransporter );

// Old Transporters

//  DECLARE_TOOL( CombinedTransporter );
//  DECLARE_TOOL( LinearTransporter );
//  DECLARE_TOOL( NsctVeloTransporter );
//  DECLARE_TOOL( ParabolicTransporter );
//  DECLARE_TOOL( TrTransporter );


//  Geometrical tools
/// @todo Restore GeomDispCalculator
//   DECLARE_TOOL( GeomDispCalculator );
   DECLARE_TOOL( TrgDispCalculator ); 
}
