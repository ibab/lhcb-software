// Include files
#include "GaussInitialisation.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"
#include "Event/EventHeader.h"
#include "Event/ProcStatus.h"
#include <vector>
#include <ctime>

//----------------------------------------------------------------------------
// Implementation of class :  GaussInitialisation
//-----------------------------------------------------------------------------

static const AlgFactory<GaussInitialisation>  s_factory;
const IAlgFactory& GaussInitialisationFactory = s_factory;

//-----------------------------------------------------------------------------
GaussInitialisation::GaussInitialisation( const std::string& name, 
                                            ISvcLocator* pSvcLocator )
                     : Algorithm( name, pSvcLocator ) { 
  m_engine = 0;
  declareProperty( "InitRndmOnce",    m_initRndm = false );
  declareProperty( "RndmNumberSeed",  m_theSeed  = -1 );
  declareProperty( "RunNumber", m_runNumb = 1 );
  declareProperty( "FirstEventNumber", m_firstEvent = 1);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
GaussInitialisation::~GaussInitialisation() { }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
StatusCode GaussInitialisation::initialize() { 
//-----------------------------------------------------------------------------

  std::string version = (std::string)getenv("GAUSSVERS");
  time_t ltime;
  time(&ltime);
  MsgStream msg( msgSvc(), name() );
  msg << MSG::INFO << endmsg;
  msg << MSG::INFO
      << "==================================================================== \n"
      << endmsg;
  msg << MSG::INFO
      << "                 Welcome to Gauss version " << version 
      << endmsg;
  msg << MSG::INFO 
      << "                 job running on  "  << ctime(&ltime) << endmsg;
  msg << MSG::INFO
      << "===================================================================="
      << endmsg;
  msg << MSG::INFO << endmsg;

  // Set event number to the first event
  m_eventNumb=m_firstEvent;
  
  // Get the random number engine
  m_engine = randSvc()->engine();
  if( 0 == m_engine ) 
    {
      MsgStream log( msgSvc(), name() );
      log << MSG::ERROR << "Random number engine not found!" << endmsg;
      return StatusCode::FAILURE;
    } 
  else 
    {
      std::vector<long> seeds;
      seeds.push_back( 12345678 );
      m_engine->setSeeds( seeds );
      
      return StatusCode::SUCCESS;
    }
}

//-----------------------------------------------------------------------------
StatusCode GaussInitialisation::execute() {
//-----------------------------------------------------------------------------

  MsgStream log( msgSvc(), name() );

  // Create an EventHeader and register it to the store
  EventHeader* eventHead = new EventHeader();

  eventHead->setEvtNum(m_eventNumb);
  eventHead->setRunNum(m_runNumb);

  StatusCode sc = eventDataService()->
    registerObject(EventHeaderLocation::Default, eventHead);
  
  if (sc != StatusCode::SUCCESS) 
    {
      MsgStream log(messageService(), name());
      log << MSG::ERROR << "Event header could not be registered!" << endmsg;
      return StatusCode::FAILURE;
    }
  else 
    {
      log << MSG::INFO << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<< endmsg;

      log << MSG::ALWAYS << "Processing event " << m_eventNumb <<
        ",     Run "         << m_runNumb << endmsg;
    }
  
  // Create a ProcStatus for all events
  SmartDataPtr<ProcStatus> procSt ( eventSvc(),
                                    ProcStatusLocation::Default);
  ProcStatus* procStat = procSt;
  if ( procStat == 0 ) {
    procStat = new ProcStatus();
    eventSvc()->registerObject( ProcStatusLocation::Default, procStat );
  }

  // Set the random number seed either once per event or once per job
  std::vector<long> seeds;
  if( m_initRndm ) {
    if( m_firstEvent==m_eventNumb ){
      if( -1 == m_theSeed ) { m_theSeed = m_runNumb; }
      seeds.push_back( m_theSeed );
      m_engine->setSeeds( seeds );
      log << MSG::INFO 
          << "Random number sequence initialised for this run with seed "
          << m_theSeed << endmsg; 
    }
  }  
  else {
    // Get random number seed by hashing string containing event & run nos.
    char seedTxt[32];
    sprintf( seedTxt, "Gauss_evt_%.8lx_run_%.8lx", m_eventNumb, m_runNumb);
    
    //--> Hash32 algorithm from Pere Mato
    const char* k;
    long hash;
    for (hash = 0, k = seedTxt; *k; k++) {
      hash += *k; hash += (hash << 10); hash ^= (hash >> 6);
    }
    hash += (hash << 3); hash ^= (hash >> 11); hash += (hash << 15);
    //<--
    m_theSeed = abs(hash); // CLHEP cannot cope with -ve seeds
    
    seeds.push_back( m_theSeed );
    m_engine->setSeeds( seeds );
    log << MSG::INFO 
        << "Random number sequence initialised for this event with seed "
        << m_theSeed << endmsg;
  }

  // Increase the event number
  m_eventNumb++;

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
StatusCode GaussInitialisation::finalize() { return StatusCode::SUCCESS; }
//-----------------------------------------------------------------------------
