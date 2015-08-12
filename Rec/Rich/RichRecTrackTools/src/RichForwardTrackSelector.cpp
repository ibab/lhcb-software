
//-----------------------------------------------------------------------------
/** @file RichForwardTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::ForwardTrackSelector
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// local
#include "RichForwardTrackSelector.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ForwardTrackSelector )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ForwardTrackSelector::ForwardTrackSelector( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : BaseTrackSelector ( type, name , parent ) { }

//=============================================================================
// Destructor
//=============================================================================
ForwardTrackSelector::~ForwardTrackSelector() {}

//=============================================================================


