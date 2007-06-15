// $Id: CheckSelResult.cpp,v 1.3 2007-06-15 13:51:28 jpalac Exp $
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

  verbose() << "==> Initialize" << endmsg;
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

  verbose() << "==> Execute" << endmsg;

  bool m_allpassed = true ;
  bool m_onepassed = m_algorithms.empty() ;
  m_totEvents++ ;

  if ( !exist<SelResults>(m_selResults)){
    Warning("SelResults container not found!").ignore();
    setFilterPassed(false);
    return StatusCode::SUCCESS; // ?
  }
  
  SelResults* m_SelResCtr = get<SelResults>(m_selResults);
  if(!m_SelResCtr ) {
    warning() << "SelResult container not found at " << m_selResults << endreq;
    setFilterPassed(false);
    return StatusCode::FAILURE;
  } else verbose() << "SelResult container found at " << m_selResults << endreq;

  // loop on algorithms
  std::vector<std::string>::iterator ialg ;
  for( ialg = m_algorithms.begin(); ialg != m_algorithms.end(); ialg++ ){
    verbose() << "Looping on " << (*ialg) << endreq ;
    std::string location = "/Event/Phys/"+(*ialg) ;
    bool m_thisfound = false ;
    // loop on selresult objects
    verbose() << "Looping " << endreq ;
    for ( SelResults::const_iterator iselRes  = m_SelResCtr->begin() ;
        iselRes != m_SelResCtr->end(); iselRes++ ) {     
      if ( location == (*iselRes)->location() ){
        m_thisfound = true ;
        if ((*iselRes)->found()){
          verbose() << "Algorithm " <<(*ialg) << " - Status: Passed." << endreq ;
          m_onepassed = true ;
        } else {
          verbose() << "Algorithm " <<(*ialg) << " - Status: Not passed." 
                 << endreq ;
          m_allpassed = false ;
        }
      } else verbose() << "Looping on SelResult location " 
                       << (*iselRes)->location() << endreq ;
    }
    if ((!m_thisfound) &&  (m_ANDmode)){
      verbose() << "Cound not find algorithm " << (*ialg) << endreq ;
      m_allpassed = false ;
    }    
  }
  if (m_ANDmode) setFilterPassed(m_allpassed);
  else setFilterPassed(m_onepassed);
  verbose() << "Result is: " << filterPassed() << endreq ;
  if ( filterPassed() ){
    m_nEvents++;
    SmartDataPtr<RecHeader> header(eventSvc(), m_headerPath);
    if( header ) {
      debug() << "Run " << header->runNumber() << ", Event " << header->evtNumber() 
             << endreq;
    }
  }
  
  if ( !m_avoidSelResult ) return writeSelResult(m_SelResCtr) ;
  else return StatusCode::SUCCESS;
};

//=============================================================================
//  write selResult
//=============================================================================
StatusCode CheckSelResult::writeSelResult(SelResults* SelResCtr) {

  // Create and fill selection result object
  SelResult* myResult = new SelResult();
  myResult->setFound(filterPassed());
  myResult->setLocation( ("/Event/Phys/"+name()));
  verbose() << "Selresult location set to " << "/Event/Phys/"+name() << endreq;
  myResult->setDecay("No decay");
  SelResCtr->insert(myResult);

  verbose() << "Wrote SelResult " << filterPassed() << " to " << "/Event/Phys/"+name() << endmsg ;
  
  return StatusCode::SUCCESS ;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CheckSelResult::finalize() {

  verbose() << "==> Finalize" << endmsg;
  info() << "Filtered " << m_nEvents << " from " << m_totEvents << " events" << endreq;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
