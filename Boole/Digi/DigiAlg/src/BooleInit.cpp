// $Id: BooleInit.cpp,v 1.2 2003-10-10 13:38:54 cattanem Exp $

// Include files
#include "BooleInit.h"
#include "GaudiKernel/Memory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/DataObject.h"
#include "AIDA/IHistogram1D.h"
#include "Event/EventHeader.h"
#include "Event/L1Buffer.h"
#include "Event/HLTBuffer.h"
#include <vector>

//----------------------------------------------------------------------------
// Implementation of class :  BooleInit
//-----------------------------------------------------------------------------

static const AlgFactory<BooleInit>  s_factory;
const IAlgFactory& BooleInitFactory = s_factory;

//-----------------------------------------------------------------------------
BooleInit::BooleInit( const std::string& name, 
                                            ISvcLocator* pSvcLocator )
                     : Algorithm( name, pSvcLocator ) { 
  m_eventCounter = 0;
  m_engine = 0;
  m_hMemMapped = 0;
  m_hMemVirtual = 0;
  
  declareProperty( "initRndmOnce",    m_initRndm = false );
  declareProperty( "rndmNumberSeed",  m_theSeed  = -1 );
  declareProperty( "doHistos",        m_doHistos = true );
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
BooleInit::~BooleInit() { }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
StatusCode BooleInit::initialize() { 
//-----------------------------------------------------------------------------

  std::string version = (std::string)getenv("BOOLEVERS");
  MsgStream msg( msgSvc(), name() );
  msg << MSG::INFO
      << "===================================================================="
      << endmsg;
  msg << MSG::INFO
      << "                 Welcome to Boole version " << version << endmsg;
  msg << MSG::INFO
      << "===================================================================="
      << endmsg;

  // Get the random number engine
  m_engine = randSvc()->engine();
  if( 0 == m_engine ) {
    msg << MSG::ERROR << "Random number engine not found!" << endmsg;
    return StatusCode::FAILURE;
  } 

  // Book the monitoring histograms
  if( m_doHistos ) {
    IProperty* appMgrP;
    StatusCode sc = service("ApplicationMgr", appMgrP );
    if ( !sc.isSuccess() )    {
      msg << MSG::ERROR
          << "Unable to retrieve IProperty interface of ApplicationMgr"
          << endmsg;
      return sc;
    }
    IntegerProperty numBins;
    numBins.assign( appMgrP->getProperty( "EvtMax" ) );
    appMgrP->release();

    if( -1 == numBins ) numBins = 100;
    m_hMemMapped  = histoSvc()->book( "Boole", 1, "Mapped memory (kB)",
                                      numBins, 0.5, numBins+0.5 );
    m_hMemVirtual = histoSvc()->book( "Boole", 2, "Virtual memory (kB)",
                                      numBins, 0.5, numBins+0.5 );
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
StatusCode BooleInit::execute() {
//-----------------------------------------------------------------------------

  MsgStream msg( msgSvc(), name() );
  
  // Fill the memory usage histograms before the next event is loaded
  if( m_doHistos ) {
    long mem = System::mappedMemory();
    m_hMemMapped->fill( m_eventCounter+1, (double)mem );
    mem = System::virtualMemory();
    m_hMemVirtual->fill( m_eventCounter+1, (double)mem );
  }

  SmartDataPtr<EventHeader> evt( eventSvc(), EventHeaderLocation::Default );
  if (0 == evt){
    msg << MSG::ERROR << "Event header not found!" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    ++m_eventCounter;
    msg << MSG::INFO << "Evt " << evt->evtNum() 
        << ",  Run " << evt->runNum() 
        << ",  Nr. in job = " << m_eventCounter;
  }
  
  // Set the random number seed either once per event or once per job
  std::vector<long> seeds;
  if( !m_initRndm || ( 1 == m_eventCounter ) ) {
    if( (1 != m_eventCounter) || (-1 == m_theSeed) ){
      // Get random number seed by hashing string containing event & run nos.
      char seedTxt[32];
      sprintf( seedTxt, "Boole_evt_%.8lx_run_%.8lx",
               evt->evtNum(), evt->runNum() );

      //--> Hash32 algorithm from Pere Mato
      const char* k;
      long hash;
      for (hash = 0, k = seedTxt; *k; k++) {
        hash += *k; hash += (hash << 10); hash ^= (hash >> 6);
      }
      hash += (hash << 3); hash ^= (hash >> 11); hash += (hash << 15);
      //<--
      m_theSeed = abs(hash); // CLHEP cannot cope with -ve seeds
    }
    seeds.push_back( m_theSeed );
    m_engine->setSeeds( seeds );
    if( m_initRndm ) {
      msg << MSG::INFO << " Random number sequence initialised with seed "
          << m_theSeed;
    }
    else {
      msg << MSG::INFO << ",  random seed = " << m_theSeed;
    }
  }
  msg << MSG::INFO << endmsg;

  StatusCode sc = this->createL1Buffer();
  if( sc.isFailure() ) return sc;

  sc = this->createHLTBuffer();
  if( sc.isFailure() ) return sc;  

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
StatusCode BooleInit::finalize() { 
//-----------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  msg << MSG::INFO
      << "===================================================================="
      << endmsg;
  msg << MSG::INFO
      << "                 " << m_eventCounter << " events processed" << endmsg;
  msg << MSG::INFO
      << "===================================================================="
      << endmsg;

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
StatusCode BooleInit::createL1Buffer() {
//-----------------------------------------------------------------------------
  
  MsgStream  msg( msgSvc(), name() );

  // Create and register in the TES the L1Buffer:
  L1Buffer * l1Buffer = new L1Buffer();
  if (NULL == l1Buffer ) {
    msg << MSG::ERROR << "Unable to allocate memory to L1Buffer" << endmsg;
    return StatusCode::FAILURE;
  }

  StatusCode sc =
    eventSvc()->registerObject(L1BufferLocation::Default,l1Buffer);

  if( sc.isFailure() ) {
    delete l1Buffer;
    msg << MSG::ERROR << "Unable to register L1Buffer in TES" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
StatusCode BooleInit::createHLTBuffer() {
//-----------------------------------------------------------------------------

  MsgStream msg(msgSvc(), name());

  SmartDataPtr<EventHeader> evt( eventSvc(), EventHeaderLocation::Default );
  if ( 0 == evt ) {
    msg << MSG::ERROR << "Unable to retrieve event header" << endmsg;
    return StatusCode::FAILURE;
  }

  // Create and register in the TES a new HLTBuffer:
  HLTBuffer * hltBuffer = new HLTBuffer();
  if (NULL == hltBuffer ) {
    msg << MSG::ERROR << " Unable to allocate memory to HLTBuffer" << endmsg;
    return StatusCode::FAILURE;
  }

  StatusCode sc =
    eventSvc()->registerObject(HLTBufferLocation::Default,hltBuffer);

  if( sc.isFailure() ) {
    delete hltBuffer;
    msg << MSG::ERROR << "Unable to register HLTBuffer in TES" << endmsg;
    return StatusCode::FAILURE;
  }

  // Add the event header to the HLTBuffer (will this be done by the DAQ?)
  // Suppose this information is manipulated by class ID 101 and has Source
  //(detector?) info 1;
  hlt_int head[4];
  head[0] = hlt_int(evt->runNum());
  head[1] = hlt_int(evt->evtNum());
  head[2] = 0;  // reserved for event time
  head[3] = 0;  // reserved for event time

  // Add this block to HLTBuffer:
  hltBuffer->addBank(1,101,head,2 );

  return StatusCode::SUCCESS;
  
}
