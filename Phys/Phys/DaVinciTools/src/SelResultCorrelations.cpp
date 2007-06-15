// $Id: SelResultCorrelations.cpp,v 1.3 2007-06-15 13:51:28 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/SelResult.h"

#include "Kernel/IAlgorithmCorrelations.h"            // Interface
// local
#include "SelResultCorrelations.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : SelResultCorrelations
//
// 2004-09-01 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( SelResultCorrelations );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SelResultCorrelations::SelResultCorrelations( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_selResults      (SelResultLocation::Default)
    , m_algorithmsRow ()
    , m_algorithmsColumn ()
    , m_algoCorr()
{
  m_algorithmsRow.clear();
  m_algorithmsColumn.clear();
  declareProperty( "Algorithms", m_algorithmsColumn );
  declareProperty( "AlgorithmsRow", m_algorithmsRow );
  declareProperty( "PrintTable", m_printTable = true );
  declareProperty( "PrintList", m_printList = false );
}
//=============================================================================
// Destructor
//=============================================================================
SelResultCorrelations::~SelResultCorrelations() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode SelResultCorrelations::initialize() {
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
    info() << "Algorithms to check correlations:" << m_algorithmsColumn << endmsg ;
    sc =  m_algoCorr->algorithms(m_algorithmsColumn);
    if (!sc) return sc ;
  }
  
  if (m_algorithmsRow.empty()){
    info() << "No algorithms row defined." << endmsg;
  } else {
    info() << "Algorithms to check correlations against:" << m_algorithmsRow << endmsg ;
    sc = m_algoCorr->algorithmsRow(m_algorithmsRow); // resets stuff
    if (!sc) return sc ;
  }  

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode SelResultCorrelations::execute() {

  debug() << "==> Execute" << endmsg;

  if (!exist<SelResults>(m_selResults)){
    setFilterPassed(false);
    Warning("SelResult container not found at "+m_selResults) ;
    return StatusCode::SUCCESS;   
  }
  SelResults* SelResCtr = get<SelResults>(m_selResults);
  if (!SelResCtr ) {
    err() << "No valid data at " << m_selResults << endreq;
    setFilterPassed(false);
    return StatusCode::FAILURE; 
  }

  for ( SelResults::const_iterator iselRes  = SelResCtr->begin() ; 
        iselRes != SelResCtr->end(); ++iselRes ) { 
    debug() << "Filling result for " << (*iselRes)->location() << endmsg ;
    StatusCode sc = m_algoCorr->fillResult((*iselRes)->location(),(*iselRes)->found()) ;
    if (!sc) return sc;
  }

  return m_algoCorr->endEvent();
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SelResultCorrelations::finalize() {

  debug() << "==> Finalize" << endmsg;

  StatusCode sc = StatusCode::SUCCESS ;

  if (m_printTable) sc = m_algoCorr->printTable() ;
  if (m_printList && sc)  sc = m_algoCorr->printList() ;

  return (sc && GaudiAlgorithm::finalize());  // must be called after all other actions
}
