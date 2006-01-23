
//-----------------------------------------------------------------------------
/** @file RichReadout_load.cpp
 *
 *  Declaration file for objects in component library RichReadout
 *
 *  CVS Log :-
 *  $Id: RichReadout_load.cpp,v 1.2 2006-01-23 14:05:15 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichReadout )
{

  // Process the MCHit signals
  DECLARE_ALGORITHM( RichSignal );

  // Simulate the frontend response
  DECLARE_ALGORITHM( RichSimpleFrontEndResponse );
  DECLARE_ALGORITHM( RichDetailedFrontEndResponse );

  // Algorithm to convert MCRichDigits to Raw Buffer
  DECLARE_ALGORITHM( MCRichDigitsToRawBufferAlg );

  // Simple probabilistic charge sharing
  DECLARE_ALGORITHM( RichSimpleChargeSharing );

  // Creates summed deposits from deposits
  DECLARE_ALGORITHM( RichSummedDeposits );

  // Monitors
  DECLARE_ALGORITHM( RichDigiAlgMoni );

}

