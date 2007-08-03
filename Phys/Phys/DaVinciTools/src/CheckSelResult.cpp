// $Id: CheckSelResult.cpp,v 1.4 2007-08-03 11:41:56 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/SelResult.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "Event/RecHeader.h"

// local
#include "CheckSelResult.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckSelResult
//
// 2004-07-14 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( CheckSelResult );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckSelResult::CheckSelResult( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_selResults      (SelResultLocation::Default)
  , m_algorithms ()
  , m_ANDmode(false)
  , m_avoidSelResult(true) // As in DVAlgorithm baseclass, Dummy. 
  , m_nEvents(0)
  , m_totEvents(0)
  , m_headerPath(RecHeaderLocation::Default)
{
  m_algorithms.clear();
  declareProperty( "Algorithms", m_algorithms );
  declareProperty( "Andmode", m_ANDmode );
  declareProperty( "AvoidSelResult", m_avoidSelResult); 
}
//=============================================================================
// Destructor
//=============================================================================
CheckSelResult::~CheckSelResult() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CheckSelResult::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (msgLevel(MSG::VERBOSE)) verbose() << "==> Initialize" << endmsg;
  if (m_algorithms.empty()){
    warning() << "No algorithms defined. Use Algorithms option." << endmsg;;    
  }
  if ((m_ANDmode) && (m_algorithms.size()>1)){
    warning() << "You have selected the 'AND' mode: "
              << "ALL algorithms are required to pass!" << endmsg;
  } else info() << "You have selected the default 'OR' mode." << endmsg;
  if (!m_avoidSelResult) info() 
    << "You have set AvoidSelResult to false -> will write out to TES" << endmsg; 

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheckSelResult::execute() {

  if (msgLevel(MSG::VERBOSE)) verbose() << "==> Execute" << endmsg;
  setFilterPassed(false);

  m_totEvents++ ;

  SelResults* SelResCtr = readSelResult() ; // read in or create
  if ( NULL!=SelResCtr ) { 
    StatusCode sc = algoLoop(SelResCtr);   // loop
    if (!sc) return sc;
    if ( !m_avoidSelResult ) sc = writeSelResult(SelResCtr) ; // write out
    if (!sc) return sc;
  }
  // some stats
  if ( filterPassed() ){
    m_nEvents++;
    if ((msgLevel(MSG::DEBUG)) && (exist<RecHeader>(m_headerPath))) {
      RecHeader* rh = get<RecHeader>(m_headerPath);
      debug() << "Run " << rh->runNumber() << ", Event " << rh->evtNumber() << endmsg;
    }
  }

  return StatusCode::SUCCESS;
};

//=========================================================================
//  Make loop on algos
//=========================================================================
StatusCode CheckSelResult::algoLoop(LHCb::SelResults*& SelResCtr){
  bool m_allpassed = true ;
  bool m_onepassed = m_algorithms.empty() ;
  // loop on algorithms
  std::vector<std::string>::iterator ialg ;
  for( ialg = m_algorithms.begin(); ialg != m_algorithms.end(); ialg++ ){
    if (msgLevel(MSG::VERBOSE)) verbose() << "Looping on " << (*ialg) << endmsg ;
    std::string location = "/Event/Phys/"+(*ialg) ;
    bool m_thisfound = false ;
    // loop on selresult objects
    if (msgLevel(MSG::VERBOSE)) verbose() << "Looping " << endmsg ;
    for ( SelResults::const_iterator iselRes  = SelResCtr->begin() ;
          iselRes != SelResCtr->end(); iselRes++ ) {     
      if ( location == (*iselRes)->location() ){
        m_thisfound = true ;
        if ((*iselRes)->found()){
          if (msgLevel(MSG::VERBOSE)) verbose() << "Algorithm " <<(*ialg) << " - Status: Passed." << endmsg ;
          m_onepassed = true ;
        } else {
          if (msgLevel(MSG::VERBOSE)) verbose() << "Algorithm " <<(*ialg) << " - Status: Not passed." 
                                                << endmsg ;
          m_allpassed = false ;
        }
      } else if (msgLevel(MSG::VERBOSE)) verbose() << "Looping on SelResult location " 
                                                   << (*iselRes)->location() << endmsg ;
    }
    if ((!m_thisfound) &&  (m_ANDmode)){
      if (msgLevel(MSG::VERBOSE)) verbose() << "Cound not find algorithm " << (*ialg) << endmsg ;
      m_allpassed = false ;
    }    
  }
  if (m_ANDmode) setFilterPassed(m_allpassed);
  else setFilterPassed(m_onepassed);
  if (msgLevel(MSG::VERBOSE)) verbose() << "Result is: " << filterPassed() << endmsg ;
  return StatusCode::SUCCESS ;
}
//=========================================================================
// Read in container. Create it if needed. 
//=========================================================================
SelResults* CheckSelResult::readSelResult ( ) {
  SelResults* SelResCtr  = NULL ;
  if ( !exist<SelResults>(m_selResults)){
    Warning("SelResults container not found!",StatusCode::SUCCESS,1).ignore();
    if ( !m_avoidSelResult ){ // need container
      SelResCtr = new SelResults();
      put(SelResCtr,m_selResults);   
      if (msgLevel(MSG::DEBUG)) debug() << "SelResult container created at " << m_selResults << endmsg;
    } else { // no container, nothing to do.
      return NULL ; 
    }
  } else { // already there
    if (msgLevel(MSG::VERBOSE)) verbose() << "SelResult container found at " << m_selResults << endmsg;
    SelResCtr = get<SelResults>(m_selResults);
  }
  
  return SelResCtr ;
}
//=============================================================================
//  write selResult
//=============================================================================
StatusCode CheckSelResult::writeSelResult(SelResults*& SelResCtr) {

  // Create and fill selection result object
  SelResult* myResult = new SelResult();
  myResult->setFound(filterPassed());
  myResult->setLocation( ("/Event/Phys/"+name()));
  if (msgLevel(MSG::VERBOSE)) verbose() << "Selresult location set to " << "/Event/Phys/"+name() << endmsg;
  myResult->setDecay("No decay");
  SelResCtr->insert(myResult);

  if (msgLevel(MSG::VERBOSE)) verbose() << "Wrote SelResult " << filterPassed() << " to " << "/Event/Phys/"+name() << endmsg ;
  
  return StatusCode::SUCCESS ;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CheckSelResult::finalize() {

  if (msgLevel(MSG::VERBOSE)) verbose() << "==> Finalize" << endmsg;
  info() << "Filtered " << m_nEvents << " from " << m_totEvents << " events" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
