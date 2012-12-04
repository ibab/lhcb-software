// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiKernel/IJobOptionsSvc.h"

// local
#include "FstSequencer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FstSequencer
//
// 2012-10-08 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FstSequencer )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FstSequencer::FstSequencer( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_timerTool( 0 )
  , m_nEvent( 0 )
  , m_nAccepted( 0 )
{
  declareProperty( "Members"             , m_names                  );
  declareProperty( "ForcePassOK"         , m_forcePassOK    = false );
  declareProperty( "MeasureTime"         , m_measureTime    = false );
}
//=============================================================================
// Destructor
//=============================================================================
FstSequencer::~FstSequencer() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode FstSequencer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  sc = decodeNames();
  if ( !sc.isSuccess() ) return sc;

  //== Get the timer tool, to see if a global timing is requested.
  m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
  if ( m_timerTool->globalTiming() ) m_measureTime = true;

  if ( m_measureTime ) {
    m_timer = m_timerTool->addTimer( name() );
    m_timerTool->increaseIndent();
  } else {
    release( m_timerTool );
    m_timerTool = 0;
  }

  //== Initialize the algorithms
  std::vector<AlgorithmEntry>::iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++ ) {
    if ( m_measureTime ) {
      itE->setTimer( m_timerTool->addTimer( itE->algorithm()->name() ) );
    }

    sc = itE->algorithm()->sysInitialize();
    if ( !sc.isSuccess() ) {
      return Error( "Can not initialize " + itE->algorithm()->name(), sc );
    }
  }
  if ( m_measureTime ) m_timerTool->decreaseIndent();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FstSequencer::execute() {
  if ( m_measureTime ) m_timerTool->start( m_timer );

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  bool seqPass = true;
  ++m_nEvent;

  std::vector<AlgorithmEntry>::const_iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; ++itE ) {
    Algorithm* myAlg = itE->algorithm();
    if ( ! myAlg->isEnabled() ) continue;
    if ( ! myAlg->isExecuted() ) {
      if ( m_measureTime ) m_timerTool->start( itE->timer() );
      StatusCode result = myAlg->sysExecute();
      if ( m_measureTime ) m_timerTool->stop( itE->timer() );
      myAlg->setExecuted( true );
      if ( ! result.isSuccess() ) break;  //== Abort and return bad status
    }
    //== Check the returned status
    bool passed = myAlg->filterPassed();
    if ( !passed ) {
      seqPass = passed;
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "SeqPass is now " << (seqPass ? "true" : "false") << endmsg;
      break;
    }
  }
  if ( seqPass ) ++m_nAccepted;

  if ( msgLevel(MSG::VERBOSE) ) verbose() << "SeqPass is " << (seqPass ? "true" : "false") << endmsg;
  if ( m_forcePassOK ) seqPass = true;
  if ( !m_entries.empty() ) setFilterPassed( seqPass );
  setExecuted( true );

  if ( m_measureTime ) m_timerTool->stop( m_timer );

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FstSequencer::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  if ( 0 < m_nEvent ) {
    info() << format( "== Accepted %8d events from %8d (%7.2f %%) ==",
                      m_nAccepted, m_nEvent, 100. * float( m_nAccepted) / float( m_nEvent ) );
    info() << endmsg;
  }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Reset the executed flag of all children
//=========================================================================
void FstSequencer::resetExecuted ( ) {
  Algorithm::resetExecuted();
  // algorithm doesn't call resetExecuted of subalgos! should it???
  std::vector<AlgorithmEntry>::const_iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; ++itE ) {
    itE->algorithm()->resetExecuted();
  }
}

//=========================================================================
//  Decode the list of members, instantiate and initialize them.
//=========================================================================
StatusCode FstSequencer::decodeNames ( ) {
  StatusCode final = StatusCode::SUCCESS;
  m_entries.clear();

  //== Get the "Context" option if in the file...
  IJobOptionsSvc* jos = svc<IJobOptionsSvc>( "JobOptionsSvc" );
  bool addedContext = false;  //= Have we added the context ?
  bool addedRootInTES = false;  //= Have we added the rootInTES ?
 
  //= Get the Application manager, to see if algorithm exist
  IAlgManager* appMgr = svc<IAlgManager>("ApplicationMgr");
  const std::vector<std::string>& nameVector = m_names.value();
  std::vector<std::string>::const_iterator it;
  for ( it = nameVector.begin(); nameVector.end() != it; it++ ) {
    const Gaudi::Utils::TypeNameString typeName(*it);
    const std::string &theName = typeName.name();
    const std::string &theType = typeName.type();

    //== Check wether the specified algorithm already exists. If not, create it
    StatusCode result = StatusCode::SUCCESS;
    SmartIF<IAlgorithm> myIAlg = appMgr->algorithm(typeName, false); // do not create it now
    if ( !myIAlg.isValid() ) {

      //== Set the Context if not in the jobOptions list
      if ( ""  != context() ||
           ""  != rootInTES()  ) {
        bool foundContext = false;
        bool foundRootInTES = false;
        const std::vector<const Property*>* properties = jos->getProperties( theName );
        if ( 0 != properties ) {
          // Iterate over the list to set the options
          for ( std::vector<const Property*>::const_iterator itProp = properties->begin();
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
      }

      Algorithm *myAlg = 0;
      result = createSubAlgorithm( theType, theName, myAlg );
      if (result.isSuccess()) myIAlg = myAlg;
    } else {
      Algorithm *myAlg = dynamic_cast<Algorithm*>(myIAlg.get());
      if (myAlg) {
        subAlgorithms()->push_back(myAlg);
        // when the algorithm is not created, the ref count is short by one, so we have to fix it.
        myAlg->addRef();
      }
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
 
    // propagate the sub-algorithm into own state.
    if ( result.isSuccess () &&
         Gaudi::StateMachine::INITIALIZED <= FSMState() &&
         myIAlg.isValid   () &&
         Gaudi::StateMachine::INITIALIZED  > myIAlg->FSMState() ) {
      StatusCode sc = myIAlg->sysInitialize() ;
      if  ( sc.isFailure() ) { result = sc ; }
    }

    // propagate the sub-algorithm into own state.
    if ( result.isSuccess () &&
         Gaudi::StateMachine::RUNNING <= FSMState() &&
         myIAlg.isValid   () &&
         Gaudi::StateMachine::RUNNING  > myIAlg->FSMState() ) {
      StatusCode sc = myIAlg->sysStart () ;
      if  ( sc.isFailure() ) { result = sc ; }
    }

    //== Is it an Algorithm ?  Strange test...
    if ( result.isSuccess() ) {
      // TODO: (MCl) it is possible to avoid the dynamic_cast in most of the
      //             cases by keeping the result of createSubAlgorithm.
      Algorithm*  myAlg = dynamic_cast<Algorithm*>(myIAlg.get());
      if (myAlg!=0) {
        // Note: The reference counting is kept by the system of sub-algorithms
        m_entries.push_back( AlgorithmEntry( myAlg ) );
        if ( msgLevel(MSG::DEBUG) ) debug () << "Added algorithm " << theName << endmsg;
      } else {
        warning() << theName << " is not an Algorithm - failed dynamic_cast" << endmsg;
        final = StatusCode::FAILURE;
      }
    } else {
      warning() << "Unable to find or create " << theName << endmsg;
      final = result;
    }
  }

  release(appMgr).ignore();
  release(jos).ignore();

  //== Print the list of algorithms
  MsgStream& msg = info();
  msg << "Member list: ";
  std::vector<AlgorithmEntry>::iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++ ) {
    Algorithm* myAlg = (*itE).algorithm();
    std::string myAlgType = System::typeinfoName( typeid( *myAlg) ) ;
    if ( myAlg->name() == myAlgType ) {
      msg << myAlg->name();
    } else {
      msg << myAlgType << "/" << myAlg->name();
    }
    if ( itE+1 != m_entries.end() ) msg << ", ";
  }
  if ( "" != context() ) msg << ", with context '" << context() << "'";
  if ( "" != rootInTES() ) msg << ", with rootInTES '" << rootInTES() << "'";
  msg << endmsg;

  return final;
}
//=============================================================================
