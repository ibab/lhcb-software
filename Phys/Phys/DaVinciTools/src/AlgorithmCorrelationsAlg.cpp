// $Id$
// Include files

#include "Kernel/ICheckSelResults.h"
#include "Kernel/IAlgorithmCorrelations.h"            // Interface
#include "LoKi/AlgFunctors.h"
// local
#include "AlgorithmCorrelationsAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlgorithmCorrelationsAlg
//
// 2004-09-01 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( AlgorithmCorrelationsAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  AlgorithmCorrelationsAlg::AlgorithmCorrelationsAlg( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
    , m_algorithmsRow ()
    , m_algorithmsColumn ()
    , m_algoCorr()
{
  declareProperty( "Algorithms", m_algorithmsColumn,
                   "List of algorithms to check for efficiency.");
  declareProperty( "AlgorithmsRow", m_algorithmsRow,
                   "List of algorithms for conditional efficiencies. If not set, use ``Algorithms''" );
  declareProperty( "PrintTable", m_printTable = true,
                   "Print Correlation table");
  declareProperty( "PrintList", m_printList = false,
                   "Print Correlation List (for debugging or grep-ing)" );
}
//=============================================================================
// Destructor
//=============================================================================
AlgorithmCorrelationsAlg::~AlgorithmCorrelationsAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode AlgorithmCorrelationsAlg::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_algoCorr = tool<IAlgorithmCorrelations>("AlgorithmCorrelations",this);
  m_selTool = tool<ICheckSelResults>("CheckSelResultsTool",this);

  if (m_algorithmsColumn.empty()){
    err() << "No algorithms defined. Use Algorithms option." << endmsg;
    return StatusCode::FAILURE ;
  }

  if (msgLevel(MSG::DEBUG)) debug() << "Algorithms to check correlations: "
                                    << m_algorithmsColumn << endmsg ;
  sc =  m_algoCorr->algorithms(m_algorithmsColumn);
  if (!sc) return sc ;
  // all algorithms
  m_algorithms = m_algorithmsColumn ;

  if (m_algorithmsRow.empty()){
    if (msgLevel(MSG::DEBUG)) debug() << "No algorithms row defined. -> square matrix."
                                      << endmsg;
  } else {
    if (msgLevel(MSG::DEBUG)) debug() << "Algorithms to check correlations against:"
                                      << m_algorithmsRow << endmsg ;
    sc = m_algoCorr->algorithmsRow(m_algorithmsRow); // resets stuff
    // now add algorithmsRow to algorithms for further processing
    for ( std::vector<std::string>::const_iterator r = m_algorithmsRow.begin() ;
          r!= m_algorithmsRow.end() ; r++){
      bool found = false ;
      for ( std::vector<std::string>::const_iterator c = m_algorithmsColumn.begin() ;
            c!= m_algorithmsColumn.end() ; c++){
        if ( *c == *r ) {
          found = true ;
          break ;
        }
        if (!found) m_algorithms.push_back(*r);
      }
    }

    if (!sc) return sc ;
  }

  if (msgLevel(MSG::DEBUG)) debug() << "All algorithms to be considered : "
                                    <<  m_algorithms << endmsg ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlgorithmCorrelationsAlg::execute() 
{
  if (msgLevel(MSG::DEBUG)) 
    debug() << "==> Execute" << endmsg;

  for ( std::vector<std::string>::const_iterator a = m_algorithms.begin() ;
        a!= m_algorithms.end() ; a++){
    bool pass = m_selTool->isSelected(*a);
    if (msgLevel(MSG::DEBUG)) debug() << *a << " gets result "
                                      << pass << endmsg ;
    m_algoCorr->fillResult(*a,pass) ;
  }

  counter("Events")++ ;
  return m_algoCorr->endEvent();
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode AlgorithmCorrelationsAlg::finalize()
{
  if (msgLevel(MSG::DEBUG)) 
    debug() << "==> Finalize" << endmsg;

  StatusCode sc = StatusCode::SUCCESS ;

  if (m_printTable) sc = m_algoCorr->printTable() ;
  if (m_printList && sc)  sc = m_algoCorr->printList() ;

  return (sc && GaudiAlgorithm::finalize());  // must be called after all other actions
}
