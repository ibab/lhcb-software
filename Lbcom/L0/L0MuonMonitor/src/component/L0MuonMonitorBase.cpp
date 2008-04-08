// $Id: L0MuonMonitorBase.cpp,v 1.1 2008-04-08 11:31:03 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "L0MuonMonitorBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitorBase
//
// 2008-04-08 : 
//-----------------------------------------------------------------------------

// // Declaration of the Algorithm Factory
// DECLARE_ALGORITHM_FACTORY( L0MuonMonitorBase );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMonitorBase::L0MuonMonitorBase( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  std::vector<int> time_slots;
  for (int i=-7;i<=7;++i) time_slots.push_back(i);
  declareProperty( "TimeSlots"  , m_time_slots = time_slots);

  std::vector<int> quarters;
  for (int i=0;i<4;++i) quarters.push_back(i);
  declareProperty( "Quarters"  , m_quarters = quarters);
  
  std::vector<int> stations;
  for (int i=0;i<5;++i) stations.push_back(i);
  declareProperty( "Stations"  , m_stations = stations);

  std::vector<int> regions;
  for (int i=0;i<5;++i) regions.push_back(i);
  declareProperty( "Regions"  , m_regions = regions);

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonMonitorBase::~L0MuonMonitorBase() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonMonitorBase::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonMonitorBase::execute() {

  debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonMonitorBase::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
