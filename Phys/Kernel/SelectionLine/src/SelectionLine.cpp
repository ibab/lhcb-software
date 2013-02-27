// $Id: SelectionLine.cpp,v 1.3 2010-02-27 09:08:24 graven Exp $
// ============================================================================
// Include files
// ============================================================================
#include <cmath>
#include <vector>
#include <iterator>
// ============================================================================
// Boost
// ============================================================================
#include "boost/foreach.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram.h"
#include "AIDA/IAxis.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/ISequencerTimerTool.h"
// ============================================================================
// HLT
// ============================================================================
#include "HistogramUtilities.h"
using namespace HistogramUtilities;
// ============================================================================
// HltEvent
// ============================================================================
#include "Event/HltDecReports.h"
// ============================================================================
// Local
// ============================================================================
#include "Kernel/SelectionLine.h"
// ============================================================================

//-----------------------------------------------------------------------------
// Implementation file for class : HltLine
//
// 2008-09-13 : Gerhard Raven
//-----------------------------------------------------------------------------
void
Selection::Line::Stage::updateHandler(Property&) {
  if (!m_initialized) {
    m_dirty=true;
    return;
  }
  if (m_algorithm!=0) m_algorithm->release();
  if (!m_property.value().empty()) {
    m_algorithm = m_parent.getSubAlgorithm(m_property.value());
    if (!m_algorithm) throw GaudiException( "could not obtain algorithm for " , m_property.value(), StatusCode::FAILURE);
    m_algorithm->addRef();
  }
  m_dirty = false;
}

StatusCode
Selection::Line::Stage::initialize(ISequencerTimerTool* timer) {
  m_initialized=true;
  if ( m_dirty ) updateHandler(m_property);
  // TODO: bind timer call...
  if ( timer!=0 && algorithm()!=0 ) setTimer( timer->addTimer( algorithm()->name() ) );
  // empty transition is allowed...
  return algorithm()!=0 ? algorithm()->sysInitialize() : StatusCode::SUCCESS;
}

StatusCode
Selection::Line::Stage::execute(ISequencerTimerTool* timertool) {
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
  } catch (...) { }
  if ( timertool ) timertool->stop( timer() );
  return result;
}

Selection::Line::SubAlgos
Selection::Line::retrieveSubAlgorithms() const {
  typedef std::list<std::pair<const Algorithm*,unsigned> > SubAlgoList;
  SubAlgoList subAlgo;
  subAlgo.push_back( std::make_pair(this,0));
  SubAlgoList::iterator i = subAlgo.begin();
  while ( i != subAlgo.end() ) {
    std::vector<Algorithm*> *subs = i->first->subAlgorithms();
    if (!subs->empty()) {
      unsigned depth = i->second+1;
      SubAlgoList::iterator j = i;
      ++j;
      for (std::vector<Algorithm*>::const_iterator k = subs->begin();k!=subs->end();++k)
        subAlgo.insert(j, std::make_pair( *k, depth ) );
    }
    ++i;
  }
  subAlgo.pop_front(); // remove ourselves...
  debug() << " dumping sub algorithms: " << endmsg;
  for (SubAlgoList::const_iterator i = subAlgo.begin(); i!= subAlgo.end();++i) {
    debug() << std::string(3+3*i->second,' ') << i->first->name() << endmsg;
  }
  // transform map such that it has algo, # of sub(sub(sub()))algorightms

  SubAlgos table;
  for (SubAlgoList::const_iterator i = subAlgo.begin(); i!= subAlgo.end();++i) {
    SubAlgoList::const_iterator j = i; ++j;
    while ( j!=subAlgo.end() && j->second > i->second ) ++j;
    table.push_back(std::make_pair( i->first, std::distance(i,j) ) );
  }
  return table;
}


IANNSvc& Selection::Line::annSvc() const {
  if (m_hltANNSvc == 0) {
    StatusCode sc = serviceLocator()->service(s_ANNSvc, m_hltANNSvc);
    Assert( sc.isSuccess() && m_hltANNSvc != 0, " no ANNSvc??");
  }
  return *m_hltANNSvc;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Selection::Line::Line( const std::string& name,
                       ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_timerTool( 0 )
  , m_jos(0)
  , m_algMgr(0)
  , m_errorHisto(0)
  , m_timeHisto(0)
  , m_stepHisto(0)
  , m_candHisto(0)
  , m_hltANNSvc(0)
  , m_acceptCounter(0)
  , m_errorCounter(0)
  , m_slowCounter(0)
  , m_timer(0)
  , m_nAcceptOnError(0)
{
  for (unsigned i=0; i<m_stages.size(); ++i) {
    m_stages[i] = new Stage(*this, transition(stage(i)));
    declareProperty( m_stages[i]->property().name() , m_stages[i]->property() );
  }
  declareProperty( "HltDecReportsLocation", m_outputContainerName   = LHCb::HltDecReportsLocation::Default );
  //TODO: install updateHandler, refuse changes after initialize...
  declareProperty( "DecisionName"         , m_decision       = name+"Decision");
  declareProperty( "ANNSvc"               , s_ANNSvc         = "HltANNSvc");
  declareProperty( "IgnoreFilterPassed"   , m_ignoreFilter   = false );
  declareProperty( "MeasureTime"          , m_measureTime    = false );
  declareProperty( "ReturnOK"             , m_returnOK       = false );
  declareProperty( "AcceptOnError"        , m_acceptOnError  = true );
  declareProperty( "AcceptOnIncident"     , m_acceptOnIncident = true );
  declareProperty( "AcceptIfSlow"         , m_acceptIfSlow   = false );
  declareProperty( "MaxAcceptOnError"     , m_maxAcceptOnError   = -1 ); // -1: no quota # TODO: make this a throttelable rate...
  declareProperty( "FlagAsSlowThreshold"  , m_slowThreshold  = 500000, "microseconds"  );
  declareProperty( "IncidentsToBeFlagged" , m_incidents);

}
//=============================================================================
// Destructor
//=============================================================================
Selection::Line::~Line() { }



//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Selection::Line::initialize() {
  /// initialize the base:

  StatusCode status = GaudiHistoAlg::initialize();
  if ( !status.isSuccess() ) return status;

  /// lock the context
  Gaudi::Utils::AlgContext lock1 ( this , contextSvc() ) ;

  debug() << "==> Initialize" << endreq;
  m_jos    = svc<IJobOptionsSvc>( "JobOptionsSvc"  );
  m_algMgr = svc<IAlgManager>   ( "ApplicationMgr" );

  // register for incidents...
  IIncidentSvc* incidentSvc = svc<IIncidentSvc>( "IncidentSvc" );
  BOOST_FOREACH( const std::string& s, m_incidents ) {
    bool rethrow = false; bool oneShot = false; long priority = 0;
    incidentSvc->addListener(this,s,priority,rethrow,oneShot);
  }

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
  BOOST_FOREACH( Stage* i, m_stages) {
    sc = i->initialize(m_timerTool);
    if (!sc.isSuccess()) Error( "Failed to initialize " + i->name(), sc );
  }

  //== pick up (recursively!) our sub algorithms and their depth count
  //   so we can figure out in detail where we stalled...
  m_subAlgo = retrieveSubAlgorithms();

  //NOTE: when checking filterPassed: a sequencer can be 'true' even if some member is false...
  //ANSWER: in case positive, we skip checking all algos with depth count > current one...
  //        in case negative, we descend, and repeat there, or, if next entry has no
  //        depth count larger, we fill bin at current position..

  //== Create the monitoring histograms
  m_errorHisto = book1D("error",name()+" error",-0.5,7.5,8);
  m_timeHisto  = book1D("walltime",name()+" log(wall time/ms)",-3,6);
  m_stepHisto  = book1D("rejection stage", name()+ " rejection stage",-0.5,m_subAlgo.size()-0.5,m_subAlgo.size() );
  m_candHisto  = bookProfile1D("candidates accepted", name()+" candidates accepted",-0.5,m_subAlgo.size()-0.5,m_subAlgo.size() );
  // if possible, add labels to axis...
  // Remove common part of the name for easier label reading (assumes name is suff. descriptive)
  std::vector<std::string> stepLabels;
  std::string common =name();
  for (SubAlgos::const_iterator i = m_subAlgo.begin();i!=m_subAlgo.end();++i) {
    std::string stepname = i->first->name();
    const std::string::size_type j = stepname.find(common);
    if (j!=std::string::npos) stepname.erase(j,common.size());
    stepLabels.push_back( stepname );
  }
  if (!setBinLabels( m_stepHisto, stepLabels )) {
    error() << " Could not set bin labels in step histo " << endmsg;
  }
  if (!setBinLabels( m_candHisto, stepLabels )) {
    error() << " Could not set bin labels in cand histo " << endmsg;
  }


  //== and the counters
  m_acceptCounter = &counter("#accept");
  declareInfo("COUNTER_TO_RATE[#accept]",*m_acceptCounter,std::string("Events accepted by ") + m_decision);
  m_errorCounter = &counter("#errors"); // do not export to DIM -- we have a histogram for this..
  //declareInfo("#errors",counter("#errors"),std::string("Errors seen by ") + m_decision);
  m_slowCounter = &counter("#slow events");// do not export to DIM -- we have a histogram for this..

  m_nAcceptOnError = 0;

  if ( m_measureTime ) m_timerTool->decreaseIndent();

  return status;
}

//TODO: on a runchange, reset all counters like m_nAcceptOnError...

//=============================================================================
// Main execution
//=============================================================================
StatusCode Selection::Line::execute() {
  longlong startClock = System::currentTime( System::microSec );

  /// lock the context
  Gaudi::Utils::AlgContext lock1 ( this , contextSvc() ) ;

  if ( m_measureTime ) m_timerTool->start( m_timer );

  debug() << "==> Execute" << endreq;
  StatusCode result = StatusCode::SUCCESS;
  LHCb::HltDecReports* reports = getOrCreate<LHCb::HltDecReports,LHCb::HltDecReports>(m_outputContainerName);
  if (reports->hasDecisionName( m_decision ) ) {
    error() << "HltDecReports already contains report" << endmsg;
    return StatusCode::FAILURE;
  }


  std::pair<std::string,unsigned> key = id();


  //TODO: add c'tor with only ID
  LHCb::HltDecReport report( 0, 0, 0, key.second );
  if (report.invalidIntDecisionID()) {
    warning() << " DecisionName=" << key.first << " has invalid intDecisionID=" << key.second << endmsg;
  }
  bool accept = !m_stages.empty(); // make sure an empty line always rejects events...
  m_caughtIncident = false; // only interested in incidents during stages->execute...

  for (unsigned i=0;i<m_stages.size();++i) {
    result = m_stages[i]->execute();
    if (m_caughtIncident) {
      report.setErrorBits(report.errorBits() | 0x02);
      m_caughtIncident = false;
    }
    if (result.isFailure()) {
      report.setErrorBits(report.errorBits() | 0x01);
      break;
    }
    accept = m_stages[i]->passed();
    if ( !accept ) break;
    report.setExecutionStage( i+1 );
  }
  // plot the wall clock time spent...
  double elapsedTime = double(System::currentTime( System::microSec ) - startClock);
  fill( m_timeHisto, log10(elapsedTime)-3 ,1.0); // convert to millisec
  if (elapsedTime>m_slowThreshold) report.setErrorBits( report.errorBits() | 0x4 );

  // did not(yet) accept, but something bad happened...
  if ( !accept && report.errorBits()!=0 && ( m_nAcceptOnError < m_maxAcceptOnError || m_maxAcceptOnError<0) ) {
    accept =  ( m_acceptOnError    && ( (report.errorBits()&0x01)!=0) )
      || ( m_acceptOnIncident && ( (report.errorBits()&0x02)!=0) )
      || ( m_acceptIfSlow     && ( (report.errorBits()&0x04)!=0) );
    if (accept) ++m_nAcceptOnError;
  }

  report.setDecision(accept ? 1u : 0u);
  report.setNumberOfCandidates( numberOfCandidates() );
  if ( !m_ignoreFilter ) setFilterPassed( accept );
  setExecuted( true );

  //TODO: allow insert at the beginning, and non-const access to update...
  reports->insert( key.first , report );

  // update monitoring
  *m_acceptCounter += accept;
  // don't flag slow events as error, so mask the bit
  *m_errorCounter  += ( (report.errorBits()&~0x4)!=0 ) ;
  *m_slowCounter   += ( (report.errorBits()& 0x4)!=0 ) ;

  fill( m_errorHisto, report.errorBits(), 1.0);
  // make stair plot
  SubAlgos::iterator last = m_subAlgo.begin();
  while ( last != m_subAlgo.end() ) {
    if (last->first->filterPassed()) {
      last+=last->second;
    } else {
      if (last->second==1) break; // don't have subalgos, so this is where we stopped
      ++last; // descend into subalgorithms, figure out which one failed.....
      // Note: what to do if subalgos pass, but parent failed??  (yes, this is possible!)
      // actually need to invert parent/daughters, such that if daughters OK,
      // but parent isn't, we enter the plot at the _parent_, but that should appear
      // _after_ the daughters (which may be confusing)...
    }
  }
  fill( m_stepHisto, std::distance(m_subAlgo.begin(),last), 1.0);
  for(SubAlgos::iterator i = m_subAlgo.begin();i!=last;++i) {
    fill(m_candHisto,std::distance(m_subAlgo.begin(),i),numberOfCandidates(i->first),1.0);
  }
  if ( m_measureTime ) m_timerTool->stop( m_timer );


  return m_returnOK ? StatusCode::SUCCESS : result;
}


std::vector< const Algorithm* > Selection::Line::algorithms() const {
  std::vector< const Algorithm* > subs;
  for( SubAlgos::const_iterator i = m_subAlgo.begin(); i!=m_subAlgo.end(); ++i) {
    subs.push_back(i->first);
  }
  return subs;
}
//=========================================================================
// reset the executed status of all members
//=========================================================================
void Selection::Line::resetExecuted ( ) {
  Algorithm::resetExecuted();
  // algorithm doesn't call resetExecuted of subalgos! should it???
  BOOST_FOREACH( Stage* i, m_stages) i->resetExecuted();
}


//=========================================================================
// listen for incident during processing...
//=========================================================================
void Selection::Line::handle( const Incident& ) {
  m_caughtIncident = true;
}
//=========================================================================
//  Obtain pointer to an instance of the requested algorithm.
//=========================================================================
Algorithm* Selection::Line::getSubAlgorithm(const std::string& algname) {
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
