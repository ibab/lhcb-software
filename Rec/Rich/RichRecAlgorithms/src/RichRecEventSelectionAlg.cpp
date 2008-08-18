
//--------------------------------------------------------------------------
/** @file RichRecEventSelectionAlg.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::EventSelectionAlg
 *
 *  CVS Log :-
 *  $Id: RichRecEventSelectionAlg.cpp,v 1.2 2008-08-18 19:28:37 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecEventSelectionAlg.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( EventSelectionAlg );

// Standard constructor, initializes variables
EventSelectionAlg::EventSelectionAlg( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator )
{
  declareProperty( "MinPixels", m_minPixels = 10 );
}

// Destructor
EventSelectionAlg::~EventSelectionAlg() {}

StatusCode EventSelectionAlg::execute()
{
  // Event Status
  bool OK = richStatus()->eventOK();
  
  // Pixels
  if ( OK )
  {
    if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
    // enough hits ?
    OK = ( richPixels()->size() >= m_minPixels );
  }

  // set if this events is selected
  setFilterPassed(OK);

  return StatusCode::SUCCESS;
}
