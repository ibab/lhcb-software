
//-----------------------------------------------------------------------------
/** @file RichVeloTTTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::VeloTTTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichVeloTTTrackSelector.cpp,v 1.4 2007-02-06 16:18:34 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichVeloTTTrackSelector.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( VeloTTTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloTTTrackSelector::VeloTTTrackSelector( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : BaseTrackSelector ( type, name , parent )
{
  // interface
  declareInterface<IBaseTrackSelector>(this);
  // redefine default cut values for VeloTT tracks
  m_minPCut     = 1.0;
}

//=============================================================================
// Destructor
//=============================================================================
VeloTTTrackSelector::~VeloTTTrackSelector() {}

//=============================================================================


