
//-----------------------------------------------------------------------------
/** @file RichKsTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichKsTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichKsTrackSelector.cpp,v 1.1 2006-08-28 11:34:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichKsTrackSelector.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RichKsTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RichKsTrackSelector::RichKsTrackSelector( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : RichBaseTrackSelector ( type, name , parent )
{
  // interface
  declareInterface<Rich::IRichBaseTrackSelector>(this);
  // redefine default cut values for Ks tracks

}

//=============================================================================
// Destructor
//=============================================================================
Rich::RichKsTrackSelector::~RichKsTrackSelector() {}

//=============================================================================


