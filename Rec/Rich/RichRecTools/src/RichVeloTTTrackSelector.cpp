
//-----------------------------------------------------------------------------
/** @file RichVeloTTTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichVeloTTTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichVeloTTTrackSelector.cpp,v 1.1 2006-08-28 11:34:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichVeloTTTrackSelector.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RichVeloTTTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RichVeloTTTrackSelector::RichVeloTTTrackSelector( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent )
  : RichBaseTrackSelector ( type, name , parent )
{
  // interface
  declareInterface<Rich::IRichBaseTrackSelector>(this);
  // redefine default cut values for VeloTT tracks

}

//=============================================================================
// Destructor
//=============================================================================
Rich::RichVeloTTTrackSelector::~RichVeloTTTrackSelector() {}

//=============================================================================


