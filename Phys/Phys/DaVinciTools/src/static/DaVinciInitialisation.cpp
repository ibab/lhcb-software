// $Id: DaVinciInitialisation.cpp,v 1.1.1.1 2001-07-09 09:28:42 gcorti Exp $
#define DAVINCI_INITIALISATION_CPP

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "PhysEvent/PhysEvent.h"
#include "LHCbEvent/AxPartCandidate.h"
#include "LHCbEvent/MCParticle.h"
#include "LHCbEvent/RefTable.h"
#include "DaVinciInitialisation.h"

//----------------------------------------------------------------------------
//
// Implementation file for class :  DaVinciInitialisation
//
// 10/06/2001 : Gloria Corti
//----------------------------------------------------------------------------

// wrapped fortran routines
extern "C" {
#ifdef WIN32
  void __stdcall DAVINCIINIT( void );
#else
  void davinciinit_( void );
#define DAVINCIINIT davinciinit_
#endif
}

// Declaration of the Algorithm Factory
static const AlgFactory<DaVinciInitialisation>  s_factory;
const IAlgFactory& DaVinciInitialisationFactory = s_factory;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
DaVinciInitialisation::DaVinciInitialisation( const std::string& name, 
                                              ISvcLocator* pSvcLocator )
 : Algorithm( name, pSvcLocator ),
   m_useSICB(true)
{
  declareProperty( "useSICB", m_useSICB );
}

//=============================================================================
// Standard destructor
//=============================================================================
DaVinciInitialisation::~DaVinciInitialisation() {
}

//=============================================================================
// Initialisation. Check parameters, call fortran part
//=============================================================================
StatusCode DaVinciInitialisation::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Initializing DaVinci" << endreq;
  
  // Job initialization of SICB analysis routines
  if( m_useSICB ) {
    log << MSG::INFO << "    Initializing SICB part of the analysis" << endreq;
    DAVINCIINIT();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DaVinciInitialisation::execute() {

  MsgStream log(msgSvc(), name());
  // Event by event initialization of SICB analysis routines: 
  // some counters/banks
  // need to be initialized every time we read a new event
  if( m_useSICB ) {
    log << MSG::DEBUG << "    Initializing FORTRAN part of DaVinci" << endreq;
    DAVINCIINIT();
  }

  log << MSG::DEBUG << ">>> Event by event initialization" << endreq;

  // Create the PhysEvent structure for this event and put it in the 
  // transient store
  PhysEvent* phevent = new PhysEvent;
  StatusCode sc = eventSvc()->registerObject("/Event","Phys",phevent);
  log << MSG::DEBUG << "    Creating new /Event/Phys" << endreq;
  if ( sc.isFailure() ) {
    delete phevent;
    log << MSG::FATAL 
        << "    Unable to register physics event tree /Event/Phys " << endreq;
    return StatusCode::FAILURE;
  }

  // In order to allow C++ algorithms to start with the same AxPartCandidate as
  // the Fortran it is necessary to load them in the transient
  // store at the beginning, because sometimes the Fortran analysis UPDATE the 
  // AXTK bank
  SmartDataPtr<AxPartCandidateVector>
    candidates (eventSvc(),"/Event/Anal/AxPartCandidates");
  if( ! candidates ) {
    log << MSG::WARNING << "    AxPartCandidates missing in the event" 
        << endreq;
  }

  // the same for association table ?
  DataObject* pObject;
  sc = eventSvc()->retrieveObject("/Event/Anal/AxPart2MCParticle",pObject);
  if ( sc.isFailure() ) {
    log << MSG::WARNING << "    SICB association missing in data " << endreq;
  }

  typedef RefTable1toN<AxPartCandidate,MCParticle> AsctTable;
  
  AsctTable* table = dynamic_cast<AsctTable*> (pObject);
  if ( 0 == table ) { 
    log << MSG::WARNING << "    SICB association missing in data " << endreq;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DaVinciInitialisation::finalize() {
  return StatusCode::SUCCESS;
}
