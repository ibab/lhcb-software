// $Id: HltCorrelations.cpp,v 1.2 2009-11-06 13:49:40 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Kernel/IAlgorithmCorrelations.h"            // Interface
#include "Kernel/ICheckSelResults.h"
#include "LoKi/AlgFunctors.h"
#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"
#include "Kernel/ReadRoutingBits.h"
#include "GaudiKernel/xtoa.h"


// local
#include "HltCorrelations.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltCorrelations
//
// 2007-11-23 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltCorrelations );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltCorrelations::HltCorrelations( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltSelectionsBase ( name , pSvcLocator )
   , m_algoCorr()
{
  declareProperty( "Algorithms", m_moreAlgorithms, 
                   "List of additional algorithms appended to Hlt selections" );
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

  strings algos ;   // algorithms to be considered in correlations

  // Fill Hlt selections
  const hltPairs& sels = HltSelectionsBase::selections();

  algos.push_back("L0");

  // trigger bits
  for ( unsigned int i = 32 ; i!=96 ; i++){
    algos.push_back(bitX(i));
  }
  // trigger lines
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
  sc =  m_algoCorr->algorithms(algos);

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltCorrelations::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  if( exist<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default) ){
    LHCb::L0DUReport* report = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
    StatusCode sc = m_algoCorr->fillResult("L0",report->decision());
    if (!sc) return sc;
  }

  // bits
  if (exist<LHCb::RawEvent>(LHCb::RawEventLocation::Default)){
    LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
    std::vector<unsigned int> yes = Hlt::firedRoutingBits(rawEvent,32,95);
    for (std::vector<unsigned int>::const_iterator i = yes.begin() ; i!= yes.end() ; ++i){
      if (!m_algoCorr->fillResult(bitX(*i),true));
    } 
  }
  
  // decreports
  if( exist<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default ) ){ 
    const LHCb::HltDecReports* decReports = 
      get<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default );
    
    hltPairs sels = HltSelectionsBase::selections();
    
    // for map look in summary
    for ( hltPairs::const_iterator p = sels.begin() ; p!= sels.end() ; ++p ){
    if (!m_algoCorr->fillResult(p->first,(decReports->decReport(p->first))? 
                                (decReports->decReport(p->first)->decision()):0))
      return StatusCode::FAILURE;
    }
  } else {
    Error("No HltDecReports found. Please run HltDecReportsMaker");
    return StatusCode::FAILURE;
  }
  
  StatusCode sc = moreAlgorithms() ;
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

