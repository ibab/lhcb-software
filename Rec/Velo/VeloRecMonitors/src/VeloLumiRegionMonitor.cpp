// $Id: VeloLumiRegionMonitor.cpp,v 1.2 2008-09-01 00:18:31 mjohn Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "VeloLumiRegionMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloLumiRegionMonitor
//
// VELO LumiRegion monitoring algorithm.
// Produces a set of histograms from the clusters bank in the TES.
//
// 2008-08-26 : Malcolm John
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace Velo {
DECLARE_ALGORITHM_FACTORY( VeloLumiRegionMonitor );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloLumiRegionMonitor::VeloLumiRegionMonitor( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : Velo::VeloMonitorBase ( name , pSvcLocator )
  , d_vertexDistributionMeanX(0)
  , d_vertexDistributionMeanY(0)
  , d_vertexDistributionMeanZ(0)
  , d_vertexDistributionSigmaX(0)
  , d_vertexDistributionSigmaY(0)
  , d_vertexDistributionSigmaZ(0)
{
}

//=============================================================================
// Destructor
//=============================================================================
Velo::VeloLumiRegionMonitor::~VeloLumiRegionMonitor(){} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloLumiRegionMonitor::initialize() {
  StatusCode sc = VeloMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  IRndmGenSvc* rndm = svc<IRndmGenSvc>("RndmGenSvc",true);
  if (0==rndm) {
    return Error ("Unable to initialize IRndmGenSvc", StatusCode::FAILURE);  
  }
  sc = m_rndm.initialize  ( rndm , Rndm::Gauss ( 29.0 , 0.5 ) ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Unable to initialize Gaussian distribution" , sc ) ; }

  declareInfo("vtxMeanX", d_vertexDistributionMeanX, "Fitted mean (X) of the vertex distribution");
  declareInfo("vtxMeanY", d_vertexDistributionMeanY, "Fitted mean (Y) of the vertex distribution");
  declareInfo("vtxMeanZ", d_vertexDistributionMeanZ, "Fitted mean (Z) of the vertex distribution");
  declareInfo("vtxSigmaX",d_vertexDistributionSigmaX,"Fitted sigma (X) of the vertex distribution");
  declareInfo("vtxSigmaY",d_vertexDistributionSigmaY,"Fitted sigma (Y) of the vertex distribution");
  declareInfo("vtxSigmaZ",d_vertexDistributionSigmaZ,"Fitted sigma (Z) of the vertex distribution");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloLumiRegionMonitor::execute() {

  // Count the number of events monitored
  // ------------------------------------
  counter( "# events" ) += 1;

  monitorLumiRegion();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloLumiRegionMonitor::finalize() {

  if ( m_debugLevel ) debug() << "==> Finalize" << endmsg;

  return VeloMonitorBase::finalize(); // must be called after all other actions
}

//=============================================================================
// Monitor the VeloLumiRegions
//=============================================================================
void Velo::VeloLumiRegionMonitor::monitorLumiRegion() {

  double entry = m_rndm.shoot();
  TH1D* hX = Gaudi::Utils::Aida2ROOT::aida2root( plot1D( entry
							, "Beam X", "X position of luminous region", -5, 30, 70 ) );

  if( counter( "# events" ).flag()/100.-int(counter( "# events" ).flag()/100.)==0 ){
    d_vertexDistributionMeanX = hX->GetMean();
    d_vertexDistributionSigmaX= hX->GetRMS();
    d_vertexDistributionMeanY = hX->GetMean()-27.2;
    d_vertexDistributionSigmaY= hX->GetRMS();
    d_vertexDistributionMeanZ = hX->GetMean()-19.0;
    d_vertexDistributionSigmaZ= hX->GetRMS() * 20.;
    hX->Reset();
  }

}

//=============================================================================
