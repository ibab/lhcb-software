
//-----------------------------------------------------------------------------
/** @file RichMCAssociators_load.cpp
 *
 * Declaration of entries in the RichMCAssociators component library
 *
 * CVS Log :-
 * $Id: RichMCAssociators_load.cpp,v 1.1.1.1 2005-10-13 15:13:40 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 04/10/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( RichMCAssociators ) 
{

  // Builds association table for MCParticles to MCRichTracks
  DECLARE_ALGORITHM( MCPartToMCRichTrackAlg );
  
  // builds the association table for MCRichHits to MCRichOpticalPhotons
  DECLARE_ALGORITHM( MCRichHitToMCRichOpPhotAlg ); 

  // Create links between RichDigits and MCRichDigits
  DECLARE_ALGORITHM( BuildMCRichDigitLinks );

  // Create summary objects for RichDigits (RichSmartIDs)
  DECLARE_ALGORITHM( RichDigitSummaryAlg );

}
