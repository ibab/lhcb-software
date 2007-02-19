
//-----------------------------------------------------------------------------
/** @file GlobalReco_load.cpp
 *
 * Implementation file for classes in the GlobalReco Component Library
 *
 * CVS Log :-
 * $Id: GlobalReco_load.cpp,v 1.15 2007-02-19 11:38:05 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( GlobalReco ) 
{
  DECLARE_ALGORITHM( ChargedProtoPAlg ) ;
  DECLARE_ALGORITHM( ChargedProtoCombineDLLsAlg );
  DECLARE_ALGORITHM( NeutralProtoPAlg ) ;
  DECLARE_ALGORITHM( RichPIDsFromProtoParticlesAlg ) ;
  DECLARE_ALGORITHM( MuonPIDsFromProtoParticlesAlg ) ;
  DECLARE_ALGORITHM( ChargedProtoParticleTupleAlg );
}
