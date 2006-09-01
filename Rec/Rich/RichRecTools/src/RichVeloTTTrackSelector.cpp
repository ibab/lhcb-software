
//-----------------------------------------------------------------------------
/** @file RichVeloTTTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichVeloTTTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichVeloTTTrackSelector.cpp,v 1.2 2006-09-01 06:18:48 jonrob Exp $
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
  m_minPCut     = 1.0;
}

//=============================================================================
// Destructor
//=============================================================================
Rich::RichVeloTTTrackSelector::~RichVeloTTTrackSelector() {}

//=============================================================================


