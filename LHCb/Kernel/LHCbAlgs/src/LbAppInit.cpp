// $Id: LbAppInit.cpp,v 1.4 2004-09-06 09:26:40 cattanem Exp $

// Include files
#include "LbAppInit.h"
#include "GaudiKernel/Memory.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/DataObject.h"
#include "AIDA/IHistogram1D.h"
#include "Event/EventHeader.h"
#include "Event/ProcStatus.h"
#include "Tools/INormalizeTool.h"
#include <vector>

#include "boost/format.hpp"

//----------------------------------------------------------------------------
// Implementation of class :  LbAppInit
//-----------------------------------------------------------------------------

static const AlgFactory<LbAppInit>  s_factory;
const IAlgFactory& LbAppInitFactory = s_factory;

//-----------------------------------------------------------------------------
LbAppInit::LbAppInit( const std::string& name, ISvcLocator* pSvcLocator )
                                    : GaudiAlgorithm( name, pSvcLocator ) { 
  m_eventCounter = 0;
  m_engine       = 0;
  m_hMemVirtual  = 0;
  
  declareProperty( "FirstEventNumber", m_firstEvent = 1    );
  declareProperty( "RunNumber",        m_runNumber  = 1    );
  declareProperty( "doHistos",         m_doHistos   = true );
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
LbAppInit::~LbAppInit() { }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
StatusCode LbAppInit::initialize() { 
//-----------------------------------------------------------------------------

  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) return Error( "Failed to initialize base class", sc );
  
  // Get the timing normalisation
  INormalizeTool* timingTool = tool<INormalizeTool>( "TimingTool" );
  double timeNorm = timingTool->normalize() / 1.e+09;
  release( timingTool );
  
  char* pver = getenv("APPVERSION");
  if( NULL == pver ) {
    return Error( "APPVERSION environment variable not defined" );
  }

  std::string version = (std::string)pver;
  
  // Get the current time
  time_t t;
  std::time( &t );
  tm* localt = std::localtime( &t );
  
  always() 
    << std::endl
    << "=================================================================="
    << "=================================================================="
    << std::endl
    << "                                "
    << "                   Welcome to " << name() << " version " << version
    << std::endl
    << "                                "
    << "          running on " << System::hostName()
    << " on " << std::asctime( localt )
    << "                                "
    << "     CPU time normalisation = " << timeNorm
    << " (c.f. 1GHz PIII, gcc 3.2 -o2)"
    << std::endl
    << "=================================================================="
    << "=================================================================="
    << endmsg;
  
  // Get the random number engine
  m_engine = randSvc()->engine();
  if( 0 == m_engine ) {
    return Error( "Random number engine not found!" );
  }
  else {
    std::vector<long> seeds;
    seeds.push_back( 12345678 );
    m_engine->setSeeds( seeds );
  }
  
  // Book the monitoring histograms
  if( m_doHistos ) {
    IProperty* appMgrP = svc<IProperty>( "ApplicationMgr" );
    IntegerProperty numBins;
    numBins.assign( appMgrP->getProperty( "EvtMax" ) );
    release( appMgrP );

    if( -1 == numBins ) numBins = 500;
    m_hMemVirtual = histoSvc()->book( name(), 2, "Virtual memory (kB)",
                                      numBins, 0.5, numBins+0.5 );
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
StatusCode LbAppInit::execute() {
//-----------------------------------------------------------------------------

  // Debug mode: print memory at each event
  debug() << "At event " << m_eventCounter+1 << ", memory usage is " 
          << System::virtualMemory() << endreq;  

  // Fill the memory usage histograms before the next event is loaded
  if( m_doHistos ) {
    long mem = System::virtualMemory();
    m_hMemVirtual->fill( m_eventCounter+1, (double)mem );
  }

  // Retrieve the event header if it exists, otherwise create it
  unsigned int eventNumber;
  SmartDataPtr<EventHeader> evt( eventSvc(), EventHeaderLocation::Default );
  if( 0 != evt ) {
    eventNumber = evt->evtNum();
    m_runNumber = evt->runNum();
  }
  else {
    eventNumber = m_firstEvent + m_eventCounter;
    EventHeader* newEvt = new EventHeader;
    newEvt->setEvtNum( eventNumber );
    newEvt->setEvtNum( m_runNumber );
    put( newEvt, EventHeaderLocation::Default );
  }
  ++m_eventCounter;

  info() << "Evt " << eventNumber << ",  Run " << m_runNumber 
         << ",  Nr. in job = " << m_eventCounter;

  // Initialise the random number seed  
  long theSeed = initRndmNum( eventNumber, m_runNumber );
  debug() << "  random seed = " << theSeed;
  info() << endmsg;

  // Create a ProcStatus object if it does not exist
  this->createProcStatus();

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
StatusCode LbAppInit::finalize() { 
//-----------------------------------------------------------------------------

  always()
    << std::endl
    << "=================================================================="
    << "=================================================================="
    << std::endl
    << "                                                 "
    << m_eventCounter << " events processed" << std::endl
    << "=================================================================="
    << "=================================================================="
    << endmsg;

  return GaudiAlgorithm::finalize();
}

//-----------------------------------------------------------------------------
long LbAppInit::initRndmNum( unsigned long evt, unsigned long run ) {
//-----------------------------------------------------------------------------

  // Set the random number seed either once per event or once per job
  std::vector<long> seeds;

  // Get random number seed by hashing string containing event & run nos.

  std::string s = name() + boost::io::str( boost::format( "_evt_%1%_run_%2%" )
    % boost::io::group( std::setfill('0'), std::hex, std::setw(8), evt )
    % boost::io::group( std::setfill('0'), std::hex, std::setw(8), run ) );
  
  //--> Hash32 algorithm from Pere Mato
  long hash = 0;
  for( std::string::const_iterator iC = s.begin(); s.end() != iC; ++iC ) {
    hash += *iC; hash += (hash << 10); hash ^= (hash >> 6);
  }
  hash += (hash << 3); hash ^= (hash >> 11); hash += (hash << 15);
  //<--
  long theSeed = abs(hash);  // CLHEP cannot cope with -ve seeds
  seeds.push_back( theSeed );
  m_engine->setSeeds( seeds );

  return theSeed;
}

//-----------------------------------------------------------------------------
StatusCode LbAppInit::createProcStatus() {
//-----------------------------------------------------------------------------

  // Create a ProcStatus if it does not already exist
  SmartDataPtr<ProcStatus> procSt ( eventSvc(),
                                    ProcStatusLocation::Default);
  if ( 0 == procSt ) {
    ProcStatus* procStat = new ProcStatus();
    put( procStat, ProcStatusLocation::Default );
  }

  return StatusCode::SUCCESS;
}

