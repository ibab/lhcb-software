// $Id: Hlt2StatisticsBase.cpp,v 1.3 2009-01-26 17:30:02 pkoppenb Exp $
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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2StatisticsBase::Hlt2StatisticsBase( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  ,   m_algoCorr()
{
  declareProperty( "Hlt2SelectionID", m_hlt2SelectionID = "Hlt2SelectionID");
}
//=============================================================================
// Destructor
//=============================================================================
Hlt2StatisticsBase::~Hlt2StatisticsBase() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt2StatisticsBase::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "Hlt2StatisticsBase ==> Initialize" << endmsg;
  m_algoCorr    = tool<IAlgorithmCorrelations>("AlgorithmCorrelations",this);
  return StatusCode::SUCCESS;
}


//=========================================================================
//  get selections
//=========================================================================
strings Hlt2StatisticsBase::getSelections( ) const 
{
  return svc<IANNSvc>("HltANNSvc")->keys(m_hlt2SelectionID);
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode Hlt2StatisticsBase::finalize()
{
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
StatusCode Hlt2StatisticsBase::execute()
{
  err() << "Hlt2StatisticsBase:: This is a base class. Do not invoke." << endmsg ;
  return StatusCode::FAILURE ;
}
