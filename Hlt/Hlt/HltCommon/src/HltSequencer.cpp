// $Id: HltSequencer.cpp,v 1.16 2007-12-11 16:28:08 hernando Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include "HltSequencer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltSequencer
//
// 2004-05-13 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
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

  m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
  if ( m_timerTool->globalTiming() ) m_measureTime = true;

  if ( m_measureTime ) {
    m_timer = m_timerTool->addTimer( name() );
    m_timerTool->increaseIndent();
  } else {
    release( m_timerTool );
  }

  //== Initialize the algorithms
  std::vector<AlgorithmEntry>::iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++ ) {
    if ( m_measureTime ) {
      (*itE).setTimer( m_timerTool->addTimer( (*itE).algorithm()->name() ) );
    }

    status = (*itE).algorithm()->sysInitialize();
    if ( !status.isSuccess() ) {
      return Error( "Can not initialize " + (*itE).algorithm()->name(),
                    status );
    }
  }
  if ( m_measureTime ) m_timerTool->decreaseIndent();

  setInitialized();
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSequencer::execute() {

  if ( m_measureTime ) m_timerTool->start( m_timer );

  debug() << "==> Execute" << endreq;

  StatusCode result = StatusCode::SUCCESS;

  std::vector<AlgorithmEntry>::const_iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++ ) {
    Algorithm* myAlg = (*itE).algorithm();
    std::string algoname = myAlg->name();
    debug() << " starting algorithm " << algoname << endreq;
    if ( myAlg->isEnabled() ) {
      // debug() << " algo enabled " << algoname << endreq;
      if ( ! myAlg->isExecuted() ) {
        // debug() << " algo  sysExecute " << algoname << endreq;
        if ( m_measureTime ) m_timerTool->start( (*itE).timer() );
        result = myAlg->sysExecute();
        debug() << " algo result " << algoname << endreq;
        if ( m_measureTime ) m_timerTool->stop( (*itE).timer() );
        myAlg->setExecuted( true );

        if ( ! result.isSuccess() ) break;  //== Abort and return bad status

      }
      //== Check the returned status
      if ( !m_ignoreFilter ) {
        bool passed = myAlg->filterPassed();
        if ( (*itE).reverse() ) passed = !passed;

        //== indicate our own result. For OR, exit as soon as true.
        //        If no more, will exit with false.
        //== for AND, exit as soon as false. Else, will be true (default)

        // TODO
        if (!m_modeOR)  
          setFilterPassed( passed );   
        else 
          if (passed) setFilterPassed(passed);
        
        if ( m_modeOR ) {
          // TODO: here do not return
          // if ( passed ) break;
          debug() << " algo has passed " << endreq;
        } else {
          if ( !passed ) break;
        }
      }
    }
  }
  setExecuted( true );

  if ( m_measureTime ) m_timerTool->stop( m_timer );

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
  bool addedContext = false;  //= Have we added the context ?
  bool addedRootInTES = false;  //= Have we added the rootInTES ?
  bool addedGlobalTimeOffset = false;  //= Have we added the globalTimeOffset ?
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
      if ( ""  != context() ||
           ""  != rootInTES() ||
           0.0 != globalTimeOffset() ) {
        bool foundContext = false;
        bool foundRootInTES = false;
        bool foundGlobalTimeOffset = false;
        properties = jos->getProperties( theName );
        if ( 0 != properties ) {
          // Iterate over the list to set the options
          for( itProp = properties->begin();
               itProp != properties->end();
               itProp++ )   {
            const StringProperty* sp = dynamic_cast<const StringProperty*>(*itProp);
            if ( 0 != sp )    {
              if ( "Context" == (*itProp)->name() ) {
                foundContext = true;
              }
              if ( "RootInTES" == (*itProp)->name() ) {
                foundRootInTES = true;
              }
              if ( "GlobalTimeOffset" == (*itProp)->name() ) {
                foundGlobalTimeOffset = true;
              }
            }
          }
        }
        if ( !foundContext && "" != context() ) {
          StringProperty contextProperty( "Context", context() );
          jos->addPropertyToCatalogue( theName, contextProperty ).ignore();
          addedContext = true;
        }
        if ( !foundRootInTES && "" != rootInTES() ) {
          StringProperty rootInTESProperty( "RootInTES", rootInTES() );
          jos->addPropertyToCatalogue( theName, rootInTESProperty ).ignore();
          addedRootInTES = true;
        }
        if ( !foundGlobalTimeOffset && 0.0 != globalTimeOffset() ) {
          DoubleProperty globalTimeOffsetProperty( "GlobalTimeOffset", globalTimeOffset() );
          jos->addPropertyToCatalogue( theName, globalTimeOffsetProperty ).ignore();
          addedGlobalTimeOffset = true;
        }
      }
      result = appMgr->createAlgorithm( theType, theName, myIAlg );
    }

    //== Remove the property, in case this is not a GaudiAlgorithm...
    if ( addedContext ) {
      jos->removePropertyFromCatalogue( theName, "Context" ).ignore();
      addedContext = false;
    }
    if ( addedRootInTES ) {
      jos->removePropertyFromCatalogue( theName, "RootInTES" ).ignore();
      addedRootInTES = false;
    }
    if ( addedGlobalTimeOffset ) {
      jos->removePropertyFromCatalogue( theName, "GlobalTimeOffset" ).ignore();
      addedGlobalTimeOffset = false;
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
  if ( "" != context() ) msg << ", with context '" << context() << "'";
  if ( "" != rootInTES() ) msg << ", with rootInTES '" << rootInTES() << "'";
  if ( 0.0 != globalTimeOffset() ) msg << ", with globalTimeOffset " << globalTimeOffset();
  msg << endreq;

  appMgr->release();
  release( jos ).ignore();
  return final;

}

//=========================================================================
//  Interface for the Property manager
//=========================================================================
void HltSequencer::membershipHandler ( Property& ) {
  if ( isInitialized() ) decodeNames();
}
//=============================================================================
