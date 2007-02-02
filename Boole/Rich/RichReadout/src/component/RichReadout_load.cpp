
//-----------------------------------------------------------------------------
/** @file RichReadout_load.cpp
 *
 *  Declaration file for objects in component library RichReadout
 *
 *  CVS Log :-
 *  $Id: RichReadout_load.cpp,v 1.3 2007-02-02 10:13:42 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichReadout )
{

  // Process the MCHit signals
  DECLARE_ALGORITHM( Signal );

  // Simulate the frontend response
  DECLARE_ALGORITHM( SimpleFrontEndResponse );
  DECLARE_ALGORITHM( DetailedFrontEndResponse );

  // Algorithm to convert MCRichDigits to Raw Buffer
  DECLARE_ALGORITHM( MCRichDigitsToRawBufferAlg );

  // Simple probabilistic charge sharing
  DECLARE_ALGORITHM( SimpleChargeSharing );

  // Creates summed deposits from deposits
  DECLARE_ALGORITHM( SummedDeposits );

  // Monitors
  DECLARE_ALGORITHM( AlgMoni );

}

