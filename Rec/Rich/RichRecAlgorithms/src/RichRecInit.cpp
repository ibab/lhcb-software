
//--------------------------------------------------------------------------
/** @file RichRecInit.cpp
 *
 *  Implementation file for algorithm class : RichRecInit
 *
 *  CVS Log :-
 *  $Id: RichRecInit.cpp,v 1.3 2007-02-02 10:05:51 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecInit.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Initialise );

// Standard constructor, initializes variables
Initialise::Initialise( const std::string& name,
                        ISvcLocator* pSvcLocator )
  : RichRecAlgBase ( name, pSvcLocator )
{
}

// Destructor
Initialise::~Initialise() {}

StatusCode Initialise::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // check for a ProcStat object
  // needed when running special modes where it is not available
  if ( !exist<LHCb::ProcStatus>(LHCb::ProcStatusLocation::Default) )
  {
    Warning( "No ProcStatus at " + LHCb::ProcStatusLocation::Default + " : Creating one",
             StatusCode::SUCCESS );
    LHCb::ProcStatus * procS = new LHCb::ProcStatus();
    put( procS, LHCb::ProcStatusLocation::Default );
  }

  return StatusCode::SUCCESS;
}
