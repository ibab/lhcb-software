
//--------------------------------------------------------------------------
/** @file RichRecClearEvent.cpp
 *
 *  Implementation file for algorithm class : RichRecClearEvent
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecClearEvent.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Standard constructor, initializes variables
ClearEvent::ClearEvent( const std::string& name,
                        ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator ) { }

// Destructor
ClearEvent::~ClearEvent() { }

StatusCode ClearEvent::execute()
{
  // Event Status
  if ( richStatus()->eventOK() ) 
  {
    // Clear the transient event objects
    massHypoRingCreator()->clear();
    photonCreator()->clear();
    trackCreator()->clear();
    segmentCreator()->clear();
    pixelCreator()->clear();
  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ClearEvent )

//--------------------------------------------------------------------------
