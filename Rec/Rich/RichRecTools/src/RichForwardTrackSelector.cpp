
//-----------------------------------------------------------------------------
/** @file RichForwardTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichForwardTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichForwardTrackSelector.cpp,v 1.1 2006-08-28 11:34:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichForwardTrackSelector.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RichForwardTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RichForwardTrackSelector::RichForwardTrackSelector( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
  : RichBaseTrackSelector ( type, name , parent )
{
  // interface
  declareInterface<Rich::IRichBaseTrackSelector>(this);
  // redefine default cut values for forward tracks

}

//=============================================================================
// Destructor
//=============================================================================
Rich::RichForwardTrackSelector::~RichForwardTrackSelector() {}

//=============================================================================


