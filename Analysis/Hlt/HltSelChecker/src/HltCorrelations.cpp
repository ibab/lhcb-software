// $Id: HltCorrelations.cpp,v 1.7 2010-01-18 12:21:52 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Kernel/IAlgorithmCorrelations.h"            // Interface
#include "Kernel/ICheckSelResults.h"
#include "LoKi/AlgFunctors.h"
#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"
#include "Kernel/ReadRoutingBits.h"

// local
#include "HltCorrelations.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltCorrelations
//
// 2007-11-23 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltCorrelations )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltCorrelations::HltCorrelations( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltSelectionsBase ( name , pSvcLocator )
   , m_algoCorr()
  , m_first(true)
{
  

  declareProperty( "Algorithms", m_moreAlgorithms, 
                   "List of additional algorithms appended to Hlt selections" );
  declareProperty( "FirstBit", m_firstBit = 32 );
  declareProperty( "LastBit", m_lastBit = 95 );
}
//=============================================================================
// Destructor
//=============================================================================
HltCorrelations::~HltCorrelations() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltCorrelations::initialize() {
  StatusCode sc = HltSelectionsBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltSelectionsBase

  if (msgLevel(MSG::DEBUG)) debug() << "HltCorrelations ==> Initialize" << endmsg;
  m_selTool = tool<ICheckSelResults>("CheckSelResultsTool",this);
  m_algoCorr = tool<IAlgorithmCorrelations>("AlgorithmCorrelations",this);

  return sc;
}

//=========================================================================
//  
//=========================================================================
StatusCode HltCorrelations::createSelections (unsigned int TCK ) {
  strings algos ;   // algorithms to be considered in correlations

  // Fill Hlt selections
  const hltPairs& sels = HltSelectionsBase::selections(TCK);
  algos.push_back("L0");
  
  // trigger bits
  if ( m_firstBit > m_lastBit ) Exception("Inconsistent bit range");
  
  for ( unsigned int i = m_firstBit ; i<=m_lastBit ; i++){
    algos.push_back(bitX(i));
  }
  // trigger selections
  for ( hltPairs::const_iterator p = sels.begin() ; p!= sels.end() ; ++p){
    bool duplicate = false ;
    for ( strings::const_iterator i=algos.begin() ; i!=algos.end() ; ++i){
      if ( (*i)==p->first ){
        duplicate = true ;
        break ;
      } 
    }
    if (duplicate) continue ;
    algos.push_back(p->first);    
    if (msgLevel(MSG::DEBUG)) debug() << "Hlt selection " << *p << endmsg ;
  }

  for ( strings::const_iterator i = m_moreAlgorithms.begin() ;
        i!=m_moreAlgorithms.end();++i){
    algos.push_back(*i);
    if (msgLevel(MSG::DEBUG)) debug() << "Added " << *i << endmsg ;
  }

  if (msgLevel(MSG::DEBUG)) debug() << "Algorithms to check correlations: " << algos << endmsg ;
  return  m_algoCorr->algorithms(algos);
  
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode HltCorrelations::execute() {
  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS ;
  if (m_first){
    // decreports
    const LHCb::HltDecReports* decReports = 
        getIfExists<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default );
    if (!decReports) return StatusCode::FAILURE;
    sc = createSelections( decReports->configuredTCK() );
    if (!sc) return sc;
    m_first = false;
  }

  bool l0yes = false ;
  LHCb::L0DUReport* report = getIfExists<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
  if( NULL!=report )
  {
    StatusCode sc = m_algoCorr->fillResult("L0",report->decision());
    l0yes = report->decision() ;
    if (!sc) return sc;
  }

  // bits
  if (msgLevel(MSG::DEBUG)) debug() << "Reading routing bits" << endmsg;
  if (l0yes)
  {
    LHCb::RawEvent* rawEvent = getIfExists<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
    if(NULL!=rawEvent)
    {
      std::vector<unsigned int> yes = Hlt::firedRoutingBits(rawEvent,m_firstBit,m_lastBit);
      for (std::vector<unsigned int>::const_iterator i = yes.begin() ; i!= yes.end() ; ++i)
      {
        if (!m_algoCorr->fillResult(bitX(*i),true))
          return StatusCode::FAILURE;
      }
    }
    
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << "Read routing bits" << endmsg;
  
  
  // decreports
  const LHCb::HltDecReports* decReports = 
      getIfExists<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default );
  if( decReports )
  { 
    
    hltPairs sels = HltSelectionsBase::selections( decReports->configuredTCK() );
    
    // for map look in summary
    for ( hltPairs::const_iterator p = sels.begin() ; p!= sels.end() ; ++p ){
    if (!m_algoCorr->fillResult(p->first,(decReports->decReport(p->first))? 
                                         (decReports->decReport(p->first)->decision()):0))
      return StatusCode::FAILURE;
    }
  } 
  else 
  {
    if ( l0yes) 
    {
      err() << "No HltDecReports found. Run HltDecReportsMaker" << endmsg;
      return StatusCode::FAILURE;
    } 
    else 
    {
      Warning("No HltDecReports found. Run HltDecReportsMaker",StatusCode::SUCCESS,1);
    }
  }
  
  sc = moreAlgorithms() ;
  if (!sc) return sc;
 
  if ( msgLevel(MSG::VERBOSE)) verbose() << "Calling endEvent" << endmsg ;
  sc = m_algoCorr->endEvent();
  if ( msgLevel(MSG::VERBOSE)) verbose() << "Called endEvent " << sc << endmsg ;  
  return sc ;
  
}

//=============================================================================
// more algorithms
//=============================================================================
StatusCode HltCorrelations::moreAlgorithms(){
  
  if (m_moreAlgorithms.empty()) return StatusCode::SUCCESS ;

  for ( strings::const_iterator a = m_moreAlgorithms.begin() ; 
        a!= m_moreAlgorithms.end() ; a++){
    bool pass = m_selTool->isSelected(*a);
    if (msgLevel(MSG::DEBUG)) debug() << *a << " gets result "  
                                           << pass << endmsg ;
    m_algoCorr->fillResult(*a,pass) ;
  }
  return StatusCode::SUCCESS ;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltCorrelations::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg ;
  StatusCode sc = m_algoCorr->printTable() ;
  if (sc) return sc;

  return HltSelectionsBase::finalize();  // must be called after all other actions
}

