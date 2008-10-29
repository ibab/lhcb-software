// $Id: AlgorithmCorrelationsAlg.cpp,v 1.1 2008-10-29 13:56:04 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/SelResult.h"

#include "Kernel/IAlgorithmCorrelations.h"            // Interface
#include "LoKi/AlgFunctors.h"
// local
#include "AlgorithmCorrelationsAlg.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : AlgorithmCorrelationsAlg
//
// 2004-09-01 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( AlgorithmCorrelationsAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlgorithmCorrelationsAlg::AlgorithmCorrelationsAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_selResults      (SelResultLocation::Default)
    , m_algorithmsRow ()
    , m_algorithmsColumn ()
    , m_algoCorr()
{
  declareProperty( "UseSelResults", m_useSelResults = true );
  declareProperty( "Algorithms", m_algorithmsColumn );
  declareProperty( "AlgorithmsRow", m_algorithmsRow );
  declareProperty( "PrintTable", m_printTable = true );
  declareProperty( "PrintList", m_printList = false );
}
//=============================================================================
// Destructor
//=============================================================================
AlgorithmCorrelationsAlg::~AlgorithmCorrelationsAlg() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode AlgorithmCorrelationsAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_algoCorr = tool<IAlgorithmCorrelations>("AlgorithmCorrelations",this);
  if (!m_algoCorr){
    err() << "Could not retrieve AlgorithmCorrelations tool" << endmsg ;
    return StatusCode::FAILURE;
  }

  if (m_algorithmsColumn.empty()){
    err() << "No algorithms defined. Use Algorithms option." << endmsg;
    return StatusCode::FAILURE ;
  } else {
    if (msgLevel(MSG::DEBUG)) debug() << "Algorithms to check correlations:" << m_algorithmsColumn << endmsg ;
    sc =  m_algoCorr->algorithms(m_algorithmsColumn);
    if (!sc) return sc ;
  }
  
  if (m_algorithmsRow.empty()){
    if (msgLevel(MSG::DEBUG)) debug() << "No algorithms row defined." << endmsg;
  } else {
    if (msgLevel(MSG::DEBUG)) debug() << "Algorithms to check correlations against:" << m_algorithmsRow << endmsg ;
    sc = m_algoCorr->algorithmsRow(m_algorithmsRow); // resets stuff
    // now add algorithmsRow to algorithms for further processing
    m_algorithms = m_algorithmsColumn ;
    for ( std::vector<std::string>::const_iterator r = m_algorithmsRow.begin() ; r!= m_algorithmsRow.end() ; r++){
      bool found = false ;
      for ( std::vector<std::string>::const_iterator c = m_algorithmsColumn.begin() ; c!= m_algorithmsColumn.end() ; c++){
        if ( *c == *r ) {
          found = true ;
          break ;
        }
        if (!found) m_algorithms.push_back(*r);
      }
    }
    
    if (!sc) return sc ;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlgorithmCorrelationsAlg::execute() {

  debug() << "==> Execute" << endmsg;

  SelResults* SelResCtr = 0 ;
  if (m_useSelResults){
    if (!exist<SelResults>(m_selResults)){
      setFilterPassed(false);
      Warning("SelResult container not found at "+m_selResults) ;
      return StatusCode::SUCCESS;   
    }
    SelResCtr = get<SelResults>(m_selResults);
  }
  
  for ( std::vector<std::string>::const_iterator a = m_algorithms.begin() ; a!= m_algorithms.end() ; a++){
    if (msgLevel(MSG::VERBOSE)) verbose() << "Looking at " << *a << endmsg ;
    LoKi::Algorithms::Passed pass(*a) ;
    if ( pass()) {
      if (msgLevel(MSG::DEBUG)) debug() << *a << " passed during this processing " << endmsg ;
      m_algoCorr->fillResult(*a,true) ;
    } else if ( m_useSelResults ){
      m_algoCorr->fillResult(*a,selResultPassed(*a,SelResCtr)) ;
    }
  }
  
  return m_algoCorr->endEvent();
};

//=========================================================================
// method to test if algorithm passed selresult
//=========================================================================
bool AlgorithmCorrelationsAlg::selResultPassed( std::string algo, const LHCb::SelResults* SelResCtr) {
  for ( SelResults::const_iterator iselRes  = SelResCtr->begin() ; 
        iselRes != SelResCtr->end(); ++iselRes ) { 
    if ("/Event/Phys/"+algo == (*iselRes)->location()){
      if (msgLevel(MSG::DEBUG)) debug() << "Found SelResult for " << algo << " with result " 
                                        << (*iselRes)->found() << endmsg ;
      return (*iselRes)->found() ;
    }
  }
  if (msgLevel(MSG::DEBUG)) debug() << "Did not find SelResult for " << algo << endmsg ;
  return false ;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode AlgorithmCorrelationsAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  StatusCode sc = StatusCode::SUCCESS ;

  if (m_printTable) sc = m_algoCorr->printTable() ;
  if (m_printList && sc)  sc = m_algoCorr->printList() ;

  return (sc && GaudiAlgorithm::finalize());  // must be called after all other actions
}
