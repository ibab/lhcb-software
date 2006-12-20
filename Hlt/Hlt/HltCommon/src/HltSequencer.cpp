// $Id: HltSequencer.cpp,v 1.5 2006-12-20 09:34:48 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"
#include "HltSequencer.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "HltBase/ParserDescriptor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltSequencer
//
// 2004-05-13 : Olivier Callot
//-----------------------------------------------------------------------------

// using namespace Gaudi;
DECLARE_ALGORITHM_FACTORY( HltSequencer );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSequencer::HltSequencer( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
  , m_timerTool( 0 )
{
  declareProperty( "Members"             , m_names                  );
  declareProperty( "ModeOR"              , m_modeOR         = false );
  declareProperty( "IgnoreFilterPassed"  , m_ignoreFilter   = false );
  declareProperty( "MeasureTime"         , m_measureTime    = false );
  declareProperty( "ReturnOK"            , m_returnOK       = false );

  m_names.declareUpdateHandler (& HltSequencer::membershipHandler, this );
}
//=============================================================================
// Destructor
//=============================================================================
HltSequencer::~HltSequencer() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode HltSequencer::initialize() {
  GaudiAlgorithm::initialize();

  debug() << "==> Initialise" << endreq;
  
  StatusCode status = decodeNames();
  if ( !status.isSuccess() ) return status;

  if ( m_measureTime ) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timer = m_timerTool->addTimer( name() );
    m_timerTool->increaseIndent();
  }

  //== Initialize the algorithms
  int i = 0;
  std::vector<AlgorithmEntry>::iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++, i++ ) {
    if ( m_measureTime ) {
      const std::string algname = (*itE).algorithm()->name();
      (*itE).setTimer( m_timerTool->addTimer( algname ));
    }
    // if (!m_propHisto.hasHisto(algname))
    status = (*itE).algorithm()->sysInitialize();
    if ( !status.isSuccess() ) {
      return Error( "Can not initialize " + (*itE).algorithm()->name(), 
                    status );
    }
  }

  // generic histograms
  m_histoTime = NULL;
  m_histoRate = NULL;
  initializeHistosFromDescriptor();
  m_histoTime = book1D("time",0.,50.,500);
  m_histoTime0 = book1D("time0",0.,50.,500);
  
  int nalgs = m_entries.size();
  m_histoRate = book1D("rate",0.,1.*(nalgs+1),nalgs+1);

  if (!m_histoTime || !m_histoRate) 
    error() << " NO HISTOS!" << endreq;
  
  if ( m_measureTime ) m_timerTool->decreaseIndent();

  setInitialized();
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSequencer::execute() {

  if ( m_measureTime ) m_timerTool->start( m_timer );
  double ttot = 0.;
  
  debug() << "==> Execute" << endreq;

  StatusCode result = StatusCode::SUCCESS;
  
  std::vector<AlgorithmEntry>::const_iterator itE;
  bool passed = m_modeOR? false:true;
  int ialg = 0;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++, ialg++ ) {
    Algorithm* myAlg = (*itE).algorithm();
    if ( myAlg->isEnabled() ) {
      if ( ! myAlg->isExecuted() ) {
        if ( m_measureTime ) m_timerTool->start( (*itE).timer() );
        result = myAlg->sysExecute();
        if ( m_measureTime) {
          double t0 = m_timerTool->stop( (*itE).timer() );
          ttot += t0;
          fill( histo1D(myAlg->name()),t0,1.);
        }
        myAlg->setExecuted( true );
        if ( ! result.isSuccess() ) break;  //== Abort and return bad status
      }
      bool algPassed = myAlg->filterPassed();
      debug() << " algorithm " << myAlg->name() << " " 
              << " passed? " << algPassed << endreq;
      if (algPassed) fill(m_histoRate,1.*ialg,1.);
      passed  = m_modeOR? (passed || algPassed) : (passed && algPassed);
      if (!m_ignoreFilter) 
        if ((!passed) && (!m_modeOR)) break;
    }
  }
  if (!m_ignoreFilter) setFilterPassed(passed);
  else setFilterPassed( true );
  debug() << " HltSequencer passed? " << passed << endreq;
  if (passed) fill(m_histoRate,ialg,1.);
  setExecuted( true );

  if ( m_measureTime ) {
    double t = m_timerTool->stop( m_timer );
    fill(m_histoTime,t,1.);
    fill(m_histoTime0,ttot,1.);
  }
  
  
  if ( m_returnOK ) return StatusCode::SUCCESS;
  else              return result;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSequencer::finalize() {

  debug() << "==> Finalize" << endreq;
  
  std::vector<AlgorithmEntry>::const_iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++ ) {
    Algorithm* myAlg = (*itE).algorithm();
    StatusCode status = myAlg->sysFinalize();
    if ( !status.isSuccess() ) {
      return Error( "Can not finalize " + (*itE).algorithm()->name(), 
                    status );
    }
  }
  return  GaudiAlgorithm::finalize();
}

//=========================================================================
//  Execute the beginRun of every algorithm 
//=========================================================================
StatusCode HltSequencer::beginRun ( ) {

  if ( !isEnabled() ) return StatusCode::SUCCESS;
  
  debug() << "==> beginRun" << endreq;
  std::vector<AlgorithmEntry>::const_iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++ ) {
    Algorithm* myAlg = (*itE).algorithm();
    StatusCode result =  myAlg->sysInitialize();
    if ( !result.isSuccess() ) {
      return Error( "Can not initialize " + myAlg->name(), result );
    }
    if ( myAlg->isEnabled() ) {
      result = myAlg->beginRun();
      if ( !result.isSuccess() ) {
        return Error( "Can not beginRun for " + myAlg->name(), result );
      }
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Execute the endRun() of every algorithm
//=========================================================================
StatusCode HltSequencer::endRun ( ) {

  if ( !isEnabled() ) return StatusCode::SUCCESS;
  
  debug() << "==> endRun" << endreq;
  std::vector<AlgorithmEntry>::const_iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++ ) {
    Algorithm* myAlg = (*itE).algorithm();
    if ( myAlg->isEnabled() ) {
      StatusCode result = myAlg->endRun();
      if ( !result.isSuccess() ) {
        return Error( "Can not beginRun for " + myAlg->name(), result );
      }
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
// reset the executed status of all members 
//=========================================================================
StatusCode HltSequencer::resetExecuted ( ) {
  Algorithm::resetExecuted();
  std::vector<AlgorithmEntry>::const_iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++ ) {
    Algorithm* myAlg = (*itE).algorithm();
    StatusCode result = myAlg->resetExecuted();
    if ( !result.isSuccess() ) {
      return Error( "Can not resetExcuted for " + myAlg->name(), result );
    }
  }
  return StatusCode::SUCCESS;
}
//=========================================================================
//  Decode the input names and fills the m_algs vector.
//=========================================================================
StatusCode HltSequencer::decodeNames( )  {

  StatusCode final = StatusCode::SUCCESS;
  m_entries.clear();

  //= Get the Application manager, to see if algorithm exist
  IAlgManager* appMgr;
  StatusCode result = service( "ApplicationMgr", appMgr );
  if ( !result.isSuccess() ) {
    return Error( "Unable to locate the ApplicationMgr", result);
  }
  const std::vector<std::string>& nameVector = m_names.value();
  std::vector<std::string>::const_iterator it;
  for ( it = nameVector.begin(); nameVector.end() != it; it++ ) {

    std::string theName = (*it);
    std::string theType = (*it);
    int slash = (*it).find_first_of( "/" );
    if ( 0 < slash ) {
      theType = (*it).substr( 0, slash );
      theName = (*it).substr( slash+1 );
      debug() << " defined algorithm " << theName 
              << " type " <<  theType << endreq;
    }
    
    IAlgorithm* myIAlg;
    Algorithm*  myAlg;
    result = appMgr->getAlgorithm( theName, myIAlg );
    if ( !result.isSuccess() ) {
      result = appMgr->createAlgorithm( theType, theName, myIAlg );
      debug() << " created algorithm " << theName << theType << endreq;
    }

    myAlg = dynamic_cast<Algorithm*>( myIAlg );
    AlgorithmEntry temp( myAlg );
    m_entries.push_back( temp );
    myAlg->addRef();                  //== Indicate it is used.
    debug () << "Added algorithm " << theName << endreq;
    
  }

  //== Print the list of algorithms
  MsgStream& msg = info();
  if ( m_modeOR ) msg << "OR ";
  msg << "Member list: ";
  std::vector<AlgorithmEntry>::iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++ ) {
    Algorithm* myAlg = (*itE).algorithm();
    if ( myAlg->name() == System::typeinfoName( typeid( *myAlg) ) ) {
      msg << myAlg->name();
    } else {
      msg << System::typeinfoName( typeid(*myAlg) ) << "/" << myAlg->name();
    }
    if ( itE+1 != m_entries.end() ) msg << ", ";
  }
  msg << endreq;

  appMgr->release();
  
  return final;
  
}

//=========================================================================
//  Interface for the Property manager
//=========================================================================
void HltSequencer::membershipHandler ( Property& ) {
  if ( isInitialized() ) decodeNames();
}
//=============================================================================
