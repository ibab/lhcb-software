
//--------------------------------------------------------------------------
/** @file RichRecInit.cpp
 *
 *  Implementation file for algorithm class : RichRecInit
 *
 *  CVS Log :-
 *  $Id: RichRecInit.cpp,v 1.1 2006-02-16 16:03:21 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecInit.h"

// namespaces
using namespace LHCb;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichRecInit>          s_factory ;
const        IAlgFactory& RichRecInitFactory = s_factory ;

// Standard constructor, initializes variables
RichRecInit::RichRecInit( const std::string& name,
                          ISvcLocator* pSvcLocator )
  : RichRecAlgBase ( name, pSvcLocator )
{
}

// Destructor
RichRecInit::~RichRecInit() {}

// Initialize
StatusCode RichRecInit::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return Error( "Failed to initialize base class", sc ); }

  return sc;
}

StatusCode RichRecInit::execute()
{

  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // check for a ProcStat object
  // needed when running special modes where it is not available
  if ( !exist<ProcStatus>(ProcStatusLocation::Default) )
  {
    Warning( "No ProcStatus at " + ProcStatusLocation::Default + " : Creating one",
             StatusCode::SUCCESS );
    ProcStatus * procS = new ProcStatus();
    put( procS, ProcStatusLocation::Default );
  }

  return StatusCode::SUCCESS;
}

StatusCode RichRecInit::finalize()
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}
