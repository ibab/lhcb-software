
//-----------------------------------------------------------------------------
/** @file RichForwardTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::ForwardTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichForwardTrackSelector.cpp,v 1.2 2007-02-02 10:10:40 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichForwardTrackSelector.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ForwardTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ForwardTrackSelector::ForwardTrackSelector( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : BaseTrackSelector ( type, name , parent )
{
  // interface
  declareInterface<IBaseTrackSelector>(this);
  // redefine default cut values for forward tracks

}

//=============================================================================
// Destructor
//=============================================================================
ForwardTrackSelector::~ForwardTrackSelector() {}

//=============================================================================


