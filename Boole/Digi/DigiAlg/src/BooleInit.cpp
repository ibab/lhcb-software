// $Id: BooleInit.cpp,v 1.7 2004-06-23 12:39:12 cattanem Exp $

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
#include "Event/RawBuffer.h"
#include "Tools/ITimingTool.h"
#include <vector>

//----------------------------------------------------------------------------
// Implementation of class :  BooleInit
//-----------------------------------------------------------------------------

static const AlgFactory<BooleInit>  s_factory;
const IAlgFactory& BooleInitFactory = s_factory;

//-----------------------------------------------------------------------------
BooleInit::BooleInit( const std::string& name, ISvcLocator* pSvcLocator )
                                    : GaudiAlgorithm( name, pSvcLocator ) { 
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

  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) return Error( "Failed to initialize base class", sc );
  
  // Get the timing normalisation
  ITimingTool* timingTool = tool<ITimingTool>( "TimingTool" );
  double timeNorm = timingTool->normalize() / 1.e+09;
  release( timingTool );
  
  std::string version = (std::string)getenv("BOOLEVERS");
  always() 
      << std::endl
      << "===================================================================="
      << std::endl
      << "                 Welcome to Boole version " << version 
      << std::endl
      << "===================================================================="
      << std::endl
      << "CPU time normalisation = " << timeNorm
      << " (c.f. 1GHz PIII, gcc 3.2 -o2)"
      << std::endl
      << "===================================================================="
      << endmsg;
  
  // Get the random number engine
  m_engine = randSvc()->engine();
  if( 0 == m_engine ) {
    return Error( "Random number engine not found!" );
  } 

  // Book the monitoring histograms
  if( m_doHistos ) {
    IProperty* appMgrP = svc<IProperty>( "ApplicationMgr" );
    IntegerProperty numBins;
    numBins.assign( appMgrP->getProperty( "EvtMax" ) );
    release( appMgrP );

    if( -1 == numBins ) numBins = 500;
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

  // Fill the memory usage histograms before the next event is loaded
  if( m_doHistos ) {
    long mem = System::mappedMemory();
    m_hMemMapped->fill( m_eventCounter+1, (double)mem );
    mem = System::virtualMemory();
    m_hMemVirtual->fill( m_eventCounter+1, (double)mem );
  }

  EventHeader* evt = get<EventHeader>( EventHeaderLocation::Default );
  ++m_eventCounter;
  info() << "Evt " << evt->evtNum() << ",  Run " << evt->runNum() 
         << ",  Nr. in job = " << m_eventCounter;
  
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
      info() << " Random number sequence initialised with seed "
          << m_theSeed;
    }
    else {
      info() << ",  random seed = " << m_theSeed;
    }
  }
  info() << endmsg;

  StatusCode sc = this->createL1Buffer();
  if( sc.isFailure() ) return sc;

  sc = this->createRawBuffer();
  if( sc.isFailure() ) return sc;  

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
StatusCode BooleInit::finalize() { 
//-----------------------------------------------------------------------------

  always()
      << std::endl
      << "===================================================================="
      << std::endl
      << "                 " << m_eventCounter << " events processed"
      << std::endl
      << "===================================================================="
      << endmsg;

  return GaudiAlgorithm::finalize();
}

//-----------------------------------------------------------------------------
StatusCode BooleInit::createL1Buffer() {
//-----------------------------------------------------------------------------
  

  // Create and register in the TES the L1Buffer:
  L1Buffer * l1Buffer = new L1Buffer();
  if (NULL == l1Buffer ) {
    return Error( "Unable to allocate memory to L1Buffer" );
  }

  put( l1Buffer, L1BufferLocation::Default );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
StatusCode BooleInit::createRawBuffer() {
//-----------------------------------------------------------------------------

  EventHeader* evt = get<EventHeader>( EventHeaderLocation::Default );

  // Create and register in the TES a new RawBuffer:
  RawBuffer * rawBuffer = new RawBuffer();
  if (NULL == rawBuffer ) {
    return Error( " Unable to allocate memory to RawBuffer" );
  }

  put( rawBuffer, RawBufferLocation::Default );

  // Add the event header to the RawBuffer (will this be done by the DAQ?)
  // Suppose this information is manipulated by class ID 101 and has Source
  //(detector?) info 1;
  raw_int head[4];
  head[0] = raw_int(evt->runNum());
  head[1] = raw_int(evt->evtNum());
  head[2] = 0;  // reserved for event time
  head[3] = 0;  // reserved for event time

  // Add this block to HltBuffer:
  rawBuffer->addBank(1,101,head,2 );

  return StatusCode::SUCCESS;
  
}
