
//-----------------------------------------------------------------------------
/** @file RichTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::TrackSelector
 *
 *  CVS Log :-
 *  $Id: RichTrackSelector.cpp,v 1.5 2007-02-02 10:10:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichTrackSelector.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackSelector::TrackSelector( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : TrackSelectorBase ( type, name, parent ) { }

//=============================================================================
// Destructor
//=============================================================================
TrackSelector::~TrackSelector() { }
