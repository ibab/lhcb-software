
//-----------------------------------------------------------------------------
/** @file RichVeloTTTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::VeloTTTrackSelector
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// local
#include "RichVeloTTTrackSelector.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VeloTTTrackSelector )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloTTTrackSelector::VeloTTTrackSelector( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : BaseTrackSelector ( type, name , parent ) { }

//=============================================================================
// Destructor
//=============================================================================
VeloTTTrackSelector::~VeloTTTrackSelector() {}

//=============================================================================


