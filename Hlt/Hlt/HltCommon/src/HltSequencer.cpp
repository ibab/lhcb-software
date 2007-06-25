// $Id: HltSequencer.cpp,v 1.13 2007-06-25 21:35:43 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"
#include "HltSequencer.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "HltBase/EParser.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "HltBase/HltTypes.h"
#include "Event/HltSummary.h"

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
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_timerTool( 0 )
{
  declareProperty( "Members"             , m_names                  );
  declareProperty( "ModeOR"              , m_modeOR         = false );
  declareProperty( "IgnoreFilterPassed"  , m_ignoreFilter   = false );
  declareProperty( "MeasureTime"         , m_measureTime    = false );
  declareProperty( "ReturnOK"            , m_returnOK       = false );
  declareProperty( "HistoDescriptor"       , m_hisDescriptor);
  declareProperty( "DataSummaryLocation", 
                   m_dataSummaryLocation = LHCb::HltSummaryLocation::Default);

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

  // generic histograms
  m_histoTime = NULL;
  m_histoTime0 = NULL;
  m_histoRate = NULL;
  if (m_measureTime) {
    m_histoTime  = book1D("time",0.,50.,500);
    m_histoTime0 = book1D("time0",0.,50.,500);
    int nalgs = m_entries.size();
    m_histoRate = book1D("rate",0.,1.*(nalgs+1),nalgs+1);
  }
  
  // external booking algorithms
  if (m_measureTime) {
    const std::vector<std::string>& hdes = m_hisDescriptor.value();
    for (std::vector<std::string>::const_iterator it = hdes.begin();
         it != hdes.end(); it++){
      std::string title = "";
      int n = 100;
      float x0 = 0.;
      float xf = 1.;
      if (EParser::parseHisto1D(*it,title,n,x0,xf)) 
        book1D(title,x0,xf,n);
    }
  }

  //== Initialize the algorithms
  int i = 0;
  std::vector<AlgorithmEntry>::iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++, i++ ) {
    const std::string algname = (*itE).algorithm()->name();
    if ( m_measureTime ) {
      (*itE).setTimer( m_timerTool->addTimer( algname ));
      AIDA::IHistogram1D* h = histo1D(algname);
      debug() << " found histogram of " << algname << " ? " 
              << h << endreq;
      m_histoTimeAlgs.push_back(h);
    }

    debug() << " initialize " << algname << endreq;
    status = (*itE).algorithm()->sysInitialize();
    if ( !status.isSuccess() ) 
      return Error( "Can not initialize " + (*itE).algorithm()->name(), 
                    status );
  }

  saveConfiguration();
  
  if ( m_measureTime ) m_timerTool->decreaseIndent();

  setInitialized();
  return StatusCode::SUCCESS;
};

void HltSequencer::saveConfiguration() {

  IDataProviderSvc* hltsvc = NULL;
  StatusCode sc = serviceLocator()->service("HltDataSvc/EventDataSvc",hltsvc);
  if (!hltsvc) error() << " not able to retrieve Hlt Svc provider " 
                       << endreq;
  
  std::string loca = m_dataSummaryLocation+"/Configuration";
  Hlt::DataHolder<Hlt::Configuration>* holder = 
    get<Hlt::DataHolder<Hlt::Configuration> > (hltsvc,loca);
  if (holder == NULL) error()<<" not able to get configuration " << endreq;
  Estd::dictionary& conf = holder->object();
  
  std::string myname = name();

  std::string key = "Type";
  std::string value = "HltSequencer";
  std::string mykey = name()+"/"+key; 
  conf.add(mykey,value);
  info() << " HLT [" << mykey << "] = " 
         << conf.retrieve<std::string>(mykey) << endreq;     

  key = "ModeOr";
  int ivalue = (int) m_modeOR;
  mykey = name()+"/"+key; 
  conf.add(mykey,ivalue);
  info() << " HLT [" << mykey << "] = " 
         << conf.retrieve<int>(mykey) << endreq;

  std::vector<std::string> algosnames;
  std::vector<AlgorithmEntry>::iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++) {
    const std::string algoname = (*itE).algorithm()->name();
    algosnames.push_back(algoname);
  }
  
  key = "Members";
  mykey = name()+"/"+key; 
  conf.add(mykey,algosnames);
  info() << " HLT [" << mykey << "] = " 
         << conf.retrieve< std::vector<std::string> >(mykey) << endreq;
  for (std::vector<std::string>::iterator it = algosnames.begin();
       it != algosnames.end(); ++it) {
    mykey = (*it)+"/HltSequencer";
    conf.add(mykey,name());
    info() << " HLT [" << mykey << "] = " 
           << conf.retrieve<std::string> (mykey) << endreq;  
  }
  
}


//=======================================================================
// Main execution
//=============================================================================
StatusCode HltSequencer::execute() {

  if ( m_measureTime ) m_timerTool->start( m_timer );
  double ttot = 0.;
  
  debug() << "==> Execute" << endreq;

  StatusCode result = StatusCode::SUCCESS;
  
  std::vector<AlgorithmEntry>::const_iterator itE;
  std::vector<AIDA::IHistogram1D*>::iterator itHis;
  bool passed = m_modeOR? false:true;
  int ialg = 0;
  for ( itE = m_entries.begin(), itHis = m_histoTimeAlgs.begin(); 
        m_entries.end() != itE; itE++, ialg++, itHis++ ) {
    Algorithm* myAlg = (*itE).algorithm();
    if ( m_measureTime ) m_timerTool->start( (*itE).timer() );
    result = myAlg->sysExecute();
    if ( m_measureTime) {
      double t0 = m_timerTool->stop( (*itE).timer() );
      ttot += t0;
      if (*itHis) fill( *itHis,t0,1.);
    }
    myAlg->setExecuted( true );
    if ( ! result.isSuccess() ) break;  //== Abort and return bad status
    bool algPassed = myAlg->filterPassed();
    debug() << " algorithm " << myAlg->name() << " " 
            << " passed? " << algPassed << endreq;
    if (algPassed && m_measureTime) fill(m_histoRate,1.*ialg,1.);
    passed  = m_modeOR? (passed || algPassed) : (passed && algPassed);
    if (!m_ignoreFilter) 
      if ((!passed) && (!m_modeOR)) break;
  }

  if (!m_ignoreFilter) setFilterPassed(passed);
  else setFilterPassed( true );
  debug() << " HltSequencer passed? " << passed << endreq;
  if (passed && m_measureTime) fill(m_histoRate,ialg,1.);
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
void HltSequencer::resetExecuted ( ) {
// void HltSequencer::resetExecuted ( ) {
  Algorithm::resetExecuted();
  std::vector<AlgorithmEntry>::const_iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++ ) {
    Algorithm* myAlg = (*itE).algorithm();
    myAlg->resetExecuted();
  }
}
//=========================================================================
//  Decode the input names and fills the m_algs vector.
//=========================================================================
StatusCode HltSequencer::decodeNames( )  {

  StatusCode final = StatusCode::SUCCESS;
  m_entries.clear();

  //== Get the "Context" option if in the file...
  IJobOptionsSvc* jos = svc<IJobOptionsSvc>( "JobOptionsSvc" );
  Property* contextProperty = 0;  //= Have we added a property ?  
  const std::vector<const Property*>* properties;
  std::vector<const Property*>::const_iterator itProp;

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
    }
    //== handling of extensions to the name ???

    //== Check wether the specified algorithm already exists. If not, create it

    IAlgorithm* myIAlg;
    Algorithm*  myAlg;
    result = appMgr->getAlgorithm( theName, myIAlg );
    if ( !result.isSuccess() ) {
      //== Set the Context if not in the jobOptions list
      if ( "" != context() ) {
        bool found = false;
        properties = jos->getProperties( theName );
        if ( 0 != properties ) {
          // Iterate over the list to set the options
          for( itProp = properties->begin();
               itProp != properties->end();
               itProp++ )   {
            const StringProperty* sp = dynamic_cast<const StringProperty*>(*itProp);
            if ( 0 != sp )    {
              if ( "Context" == (*itProp)->name() ) {
                found = true;
                break;
              }
            }
          }
        }
        if ( !found ) {
          StringProperty cp = StringProperty( "Context", '"'+context()+'"' );
          jos->addPropertyToCatalogue( theName, cp );
        }
      }
      result = appMgr->createAlgorithm( theType, theName, myIAlg );
    }

    //== Remove the property, in case this is not a GaudiAlgorithm...
    if ( 0 != contextProperty ) {
      jos->removePropertyFromCatalogue( theName, "Context" );
      contextProperty = 0;
    }
    
    //== Is it an Algorithm ?  Strange test...
    if ( result.isSuccess() ) {
      try {
        myAlg = dynamic_cast<Algorithm*>( myIAlg );
        AlgorithmEntry temp( myAlg );
        m_entries.push_back( temp );
        myAlg->addRef();                  //== Indicate it is used.
        debug () << "Added algorithm " << theName << endreq;
      } catch (...) {
        warning() << theName << " is not an Algorithm - failed dynamic_cast"
                  << endreq;
        final = StatusCode::FAILURE;
      }
    } else {
      warning() << "Unable to find or create " << theName << endreq;
      final = result;
    }
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
  release( jos );
  return final;
  
}

//=========================================================================
//  Interface for the Property manager
//=========================================================================
void HltSequencer::membershipHandler ( Property& ) {
  if ( isInitialized() ) decodeNames();
}
//=============================================================================
