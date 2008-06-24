// $Id: Hlt2StatisticsBase.cpp,v 1.1 2008-06-24 11:05:59 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Kernel/IANNSvc.h"

// local
#include "Hlt2StatisticsBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2StatisticsBase
//
// 2008-06-23 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Hlt2StatisticsBase );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2StatisticsBase::Hlt2StatisticsBase( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  ,   m_summaryTool()
  ,   m_algoCorr()
{

}
//=============================================================================
// Destructor
//=============================================================================
Hlt2StatisticsBase::~Hlt2StatisticsBase() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt2StatisticsBase::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "Hlt2StatisticsBase ==> Initialize" << endmsg;
  m_summaryTool = tool<IHltSummaryTool>("HltSummaryTool",this);
  m_algoCorr    = tool<IAlgorithmCorrelations>("AlgorithmCorrelations",this);
  return StatusCode::SUCCESS;
}


//=========================================================================
//  get selections
//=========================================================================
strings Hlt2StatisticsBase::getSelections( ) const {
  IANNSvc *ann = svc<IANNSvc>("HltANNSvc");
  std::vector<IANNSvc::minor_value_type> selections2 = ann->items("Hlt2SelectionID");
  strings selections ;

  for (std::vector<IANNSvc::minor_value_type>::const_iterator i =
         selections2.begin(); i!=selections2.end(); ++i) {
    selections.push_back(i->first) ;
  }
  
  return selections ;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode Hlt2StatisticsBase::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  StatusCode sc = m_algoCorr->printTable() ;
  if (!sc) return sc;
  //  sc = m_algoCorr->printList() ;
  //  if (!sc) return sc;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
//=============================================================================
// Main execution
//=============================================================================
StatusCode Hlt2StatisticsBase::execute() {
  err() << "Hlt2StatisticsBase:: This is a base class. Do not invoke." << endmsg ;
  return StatusCode::FAILURE ;
}

