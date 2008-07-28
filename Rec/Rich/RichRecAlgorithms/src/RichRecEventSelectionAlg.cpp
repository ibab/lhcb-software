
//--------------------------------------------------------------------------
/** @file RichRecEventSelectionAlg.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::EventSelectionAlg
 *
 *  CVS Log :-
 *  $Id: RichRecEventSelectionAlg.cpp,v 1.1 2008-07-28 16:08:11 jonrob Exp $
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

StatusCode EventSelectionAlg::initialize()
{
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  return sc;
}

StatusCode EventSelectionAlg::execute()
{
  // select event ?
  bool OK = true;

  // Event Status
  if ( !richStatus()->eventOK() ) OK = false;

  // Pixels
  if ( OK )
  {
    if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
    // enough hits ?
    if ( richPixels()->size() < m_minPixels ) OK = false;
  }

  // set if this events is selected
  setFilterPassed(OK);

  return StatusCode::SUCCESS;
}
