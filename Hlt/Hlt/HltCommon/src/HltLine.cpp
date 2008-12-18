// $Id: HltLine.cpp,v 1.6 2008-12-18 12:34:18 graven Exp $
// Include files
#include "HltLine.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include "Event/HltDecReports.h"

#include "boost/foreach.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltLine );

//-----------------------------------------------------------------------------
// Implementation file for class : HltLine
//
// 2008-09-13 : Gerhard Raven
//-----------------------------------------------------------------------------
void
HltLine::HltStage::updateHandler(Property&) {
    if (!m_initialized) { 
        m_dirty=true;
        return;
    } 
    if (m_algorithm!=0) m_algorithm->release();
    if (!m_property.value().empty()) {
        m_algorithm = m_parent.getSubAlgorithm(m_property.value());
        if (!m_algorithm) throw GaudiException( "could not obtain algorithm for " , m_property.value(), StatusCode::FAILURE);
        m_algorithm->addRef();
    } else {
    }
    m_dirty = false;
}

StatusCode
HltLine::HltStage::initialize(ISequencerTimerTool* timer) {
    m_initialized=true;
    if ( m_dirty ) updateHandler(m_property);
    // TODO: bind timer call...
    if ( timer!=0 && algorithm()!=0 ) setTimer( timer->addTimer( algorithm()->name() ) );
    // empty transition is allowed...
    return algorithm()!=0 ? algorithm()->sysInitialize() : StatusCode::SUCCESS;
}

StatusCode
HltLine::HltStage::execute(ISequencerTimerTool* timertool) {
    assert(!m_dirty);
    if (!algorithm()              ) return StatusCode::SUCCESS;
    if (!algorithm()->isEnabled() ) return StatusCode::SUCCESS;
    if ( algorithm()->isExecuted()) return StatusCode::SUCCESS;
    // TODO: bind timer at init time
    if ( timertool ) timertool->start( timer() );
    StatusCode result = StatusCode::FAILURE;
    try {
        result = algorithm()->sysExecute();
        algorithm()->setExecuted( true );
    } catch (...) {
        result = StatusCode::FAILURE;
    }
    if ( timertool ) timertool->stop( timer() );
    return result;
}

IANNSvc& HltLine::annSvc() const {
  if (m_hltANNSvc == 0) {
    StatusCode sc = serviceLocator()->service("HltANNSvc", m_hltANNSvc);
    Assert( sc.isSuccess() && m_hltANNSvc != 0, " no HltANNSvc??");
  }
  return *m_hltANNSvc;
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltLine::HltLine( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_timerTool( 0 )
  , m_jos(0)
  , m_algMgr(0)
  , m_stageHisto(0)
  , m_errorHisto(0)
  , m_hltANNSvc(0)
{
  for (unsigned i=0; i<m_stages.size(); ++i) {
    m_stages[i] = new HltStage(*this, transition(stage(i)));
    declareProperty( m_stages[i]->property().name() , m_stages[i]->property() );
  }
  declareProperty( "HltDecReportsLocation", m_outputContainerName   = LHCb::HltDecReportsLocation::Default );
  declareProperty( "DecisionName"         , m_decision       = name+"Decision");
  declareProperty( "IgnoreFilterPassed"   , m_ignoreFilter   = false );
  declareProperty( "MeasureTime"          , m_measureTime    = false );
  declareProperty( "ReturnOK"             , m_returnOK       = false );
  declareProperty( "AcceptOnError"        , m_acceptOnError  = false );
}
//=============================================================================
// Destructor
//=============================================================================
HltLine::~HltLine() { };

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode HltLine::initialize() {
  StatusCode status = GaudiAlgorithm::initialize();
  if ( !status.isSuccess() ) return status;

  debug() << "==> Initialize" << endreq;
  m_jos    = svc<IJobOptionsSvc>( "JobOptionsSvc"  );
  m_algMgr = svc<IAlgManager>   ( "ApplicationMgr" );

  m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
  if ( m_timerTool->globalTiming() ) m_measureTime = true;

  if ( m_measureTime ) {
    m_timer = m_timerTool->addTimer( name() );
    m_timerTool->increaseIndent();
  } else {
    release( m_timerTool );
    m_timerTool = 0;
  }

  //== Initialize the stages
  StatusCode sc;
  BOOST_FOREACH( HltStage* i, m_stages) {
    sc = i->initialize(m_timerTool);
    if (!sc.isSuccess()) Error( "Failed to initialize " + i->name(), sc );
  }
  //== Create the monitoring histogram
  m_errorHisto = book1D(name()+" error",name()+" error",-0.5,7.5,8);
  m_stageHisto = book1D(m_decision,m_decision,-0.5,7.5,8);

  //== and the counters
  declareInfo("#accept","",&counter("#accept"),0,std::string("Events accepted by ") + m_decision);

  if ( m_measureTime ) m_timerTool->decreaseIndent();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltLine::execute() {

  if ( m_measureTime ) m_timerTool->start( m_timer );

  debug() << "==> Execute" << endreq;
  StatusCode result = StatusCode::SUCCESS;
  LHCb::HltDecReports* reports = getOrCreate<LHCb::HltDecReports,LHCb::HltDecReports>(m_outputContainerName);
  if (reports->hasSelectionName( m_decision ) ) { 
    error() << "HltDecReports already contains report" << endmsg;
    return StatusCode::FAILURE;
  }
  boost::optional<IANNSvc::minor_value_type> key = annSvc().value("Hlt1SelectionID",m_decision);
  //TODO: add c'tor with only selID
  LHCb::HltDecReport report( 0, 0, 0, key->second );
  bool accept = true; // !m_stages.empty()??
  for (unsigned i=0;i<m_stages.size();++i) {
     result = m_stages[i]->execute();
     if (result.isFailure()) {
        report.setErrorBits(1); //TODO: different value depending on error type..
        accept = m_acceptOnError; //TODO: don't allow infinite # of accepts on error...
        break;
     }
     accept = m_stages[i]->passed();
     if ( !accept ) break;
     report.setExecutionStage( i+1 );
  }


  if (accept) report.setDecision(accept ? 1u : 0u);
  counter("#accept") += accept;
  if ( !m_ignoreFilter ) setFilterPassed( accept );
  setExecuted( true );

  //TODO: allow insert at the beginning, and non-const access to update...
  reports->insert( key->first , report );

  // update monitoring
  fill( m_stageHisto, report.executionStage(),1.0);
  fill( m_errorHisto, report.errorBits(),1.0);

  if ( m_measureTime ) m_timerTool->stop( m_timer );

  return m_returnOK ? StatusCode::SUCCESS : result;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltLine::finalize() {

  debug() << "==> Finalize" << endreq;
  //TODO: report statistics
  return  GaudiAlgorithm::finalize();
}

//=========================================================================
// reset the executed status of all members
//=========================================================================
void HltLine::resetExecuted ( ) {
  Algorithm::resetExecuted();
  // algorithm doesn't call resetExecuted of subalgos! should it???
  BOOST_FOREACH( HltStage* i, m_stages) i->resetExecuted();
}

//=========================================================================
//  Obtain pointer to an instance of the requested algorithm.
//=========================================================================
Algorithm* HltLine::getSubAlgorithm(const std::string& algname) {
    ListItem name(algname);
    IAlgorithm* myIAlg(0);
    Algorithm *myAlg(0);
    //== Check wether the specified algorithm already exists. If not, create it
    StatusCode result = m_algMgr->getAlgorithm( name.name(), myIAlg );
    if ( result.isSuccess() ) {
        myAlg = dynamic_cast<Algorithm*>(myIAlg);
        subAlgorithms()->push_back(myAlg) ;
        return myAlg;
    }
    bool addedContext = false;  //= Have we added the context ?
    bool addedRootInTES = false;  //= Have we added the rootInTES ?
    bool addedGlobalTimeOffset = false;  //= Have we added the globalTimeOffset ?

    //== Set the Context if not in the jobOptions list
    if ( ""  != context() ||
         ""  != rootInTES() ||
         0.0 != globalTimeOffset() ) {
      bool foundContext = false;
      bool foundRootInTES = false;
      bool foundGlobalTimeOffset = false;
      const std::vector<const Property*>* properties = m_jos->getProperties( name.name() );
      if ( 0 != properties ) {
        // Iterate over the list to set the options
        for ( std::vector<const Property*>::const_iterator itProp = properties->begin();
             itProp != properties->end();
             itProp++ )   {
            if (!foundContext) foundContext = ( "Context" == (*itProp)->name() ) ;
            if (!foundRootInTES) foundRootInTES = ( "RootInTES" == (*itProp)->name() ) ;
            if (!foundGlobalTimeOffset) foundGlobalTimeOffset = ( "GlobalTimeOffset" == (*itProp)->name() );
        }
      }
      if ( !foundContext && "" != context() ) {
        m_jos->addPropertyToCatalogue( name.name(), StringProperty( "Context", context() ) ).ignore();
        addedContext = true;
      }
      if ( !foundRootInTES && "" != rootInTES() ) {
        m_jos->addPropertyToCatalogue( name.name(), StringProperty( "RootInTES", rootInTES() ) ).ignore();
        addedRootInTES = true;
      }
      if ( !foundGlobalTimeOffset && 0.0 != globalTimeOffset() ) {
        m_jos->addPropertyToCatalogue( name.name(), DoubleProperty( "GlobalTimeOffset", globalTimeOffset() ) ).ignore();
        addedGlobalTimeOffset = true;
      }
    }

    result = createSubAlgorithm( name.type(), name.name(), myAlg );
    if (result.isFailure()) { return 0; }

    //== Remove the property, in case this is not a GaudiAlgorithm...
    // TODO: weird: we add it, and if we added it, we remove it again???
    if ( addedContext ) {
      m_jos->removePropertyFromCatalogue( name.name(), "Context" ).ignore();
    }
    if ( addedRootInTES ) {
      m_jos->removePropertyFromCatalogue( name.name(), "RootInTES" ).ignore();
    }
    if ( addedGlobalTimeOffset ) {
      m_jos->removePropertyFromCatalogue( name.name(), "GlobalTimeOffset" ).ignore();
    }
    return myAlg;
}
//=============================================================================
