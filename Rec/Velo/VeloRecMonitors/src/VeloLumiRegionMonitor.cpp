// $Id: VeloLumiRegionMonitor.cpp,v 1.1 2008-08-31 15:52:08 krinnert Exp $
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
  sc = m_rndm.initialize  ( rndm , Rndm::Gauss ( 29 , 0.1 ) ) ;
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
							, "Beam X"
							, "X position of luminous region (currently dummy)"
							, -0.5, 9000.5, 9001 ) );

  if( counter( "# events" ).flag()/100.-int(counter( "# events" ).flag()/100.)==0 ){
    d_vertexDistributionMeanX = hX->GetMean();
    d_vertexDistributionSigmaX= hX->GetRMS();
    debug() << entry << "  ==>  " << d_vertexDistributionMeanX<<"    "<<d_vertexDistributionSigmaX<< endmsg;
  }

}

//=============================================================================
