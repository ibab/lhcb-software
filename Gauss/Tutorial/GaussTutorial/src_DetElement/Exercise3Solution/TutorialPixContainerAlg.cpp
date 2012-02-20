// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "TutorialPixContainerAlg.h"
#include "DeTutorialPixelContainer.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/TabulatedProperty.h"



//-----------------------------------------------------------------------------
// Implementation file for class : TutorialPixContainerAlg
//
// 2012-02-13 : Sajan Easo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TutorialPixContainerAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TutorialPixContainerAlg::TutorialPixContainerAlg( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
TutorialPixContainerAlg::~TutorialPixContainerAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TutorialPixContainerAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize TutorialPixContainerAlg" << endmsg;

  DeTutorialPixelContainer* DePixA= 
       getDet<DeTutorialPixelContainer> ("/dd/Structure/LHCb/AfterMagnetRegion/TutorialMasterDet/PixelContainerA0");
  if(  DePixA ) {
    
    DePixA->FindGlobalPointFromPixelContainer (0.0, 0.0, 10.0);
    const ILVolume* TutorialPixLogVolA = DePixA->geometry()->lvolume();
    unsigned long noppv =TutorialPixLogVolA ->noPVolumes();
    info() << DePixA->name() << "  log. volume has " << noppv    << " phys. volumes: "<<endmsg;
    std::string ManufactA= DePixA->param<std::string>( "ManufacturerName" );
    info() << "The manufacturer for Pixels in "<<DePixA->name()<<"  is "<<ManufactA<<endmsg;
    
    std::vector<double> BiasVoltA= DePixA->param<std::vector<double> >( "BiasVoltage"   );
    info()<<" Bias voltages in "<< DePixA->name() <<"  are "<<BiasVoltA<<endmsg;
  }

 DeTutorialPixelContainer* DePixB= 
       getDet<DeTutorialPixelContainer> ("/dd/Structure/LHCb/AfterMagnetRegion/TutorialMasterDet/PixelContainerB1");

   if(  DePixB ) {
    
    DePixB->FindGlobalPointFromPixelContainer (0.0, 0.0, 10.0);
    const ILVolume* TutorialPixLogVolB = DePixB->geometry()->lvolume();
    unsigned long noppvB =TutorialPixLogVolB ->noPVolumes();
    info() << DePixB->name() << "  log. volume has " << noppvB    << " phys. volumes: "<<endmsg;
    std::string ManufactB= DePixB->param<std::string>( "ManufacturerName" );
    info() << "The manufacturer for Pixels in "<<DePixB->name()<<"  is "<<ManufactB<<endmsg;
    
    std::vector<double> BiasVoltB= DePixB->param<std::vector<double> >( "BiasVoltage"   );
    info()<<" Bias voltages in "<< DePixB->name() <<"  are "<<BiasVoltB<<endmsg;
  }

  DeTutorialPixelContainer* DePixC= 
       getDet<DeTutorialPixelContainer> ("/dd/Structure/LHCb/AfterMagnetRegion/TutorialMasterDet/PixelContainerC2");

   if(  DePixC ) {
    
    DePixC->FindGlobalPointFromPixelContainer (0.0, 0.0, 10.0);
    const ILVolume* TutorialPixLogVolC = DePixC->geometry()->lvolume();
    unsigned long noppvC =TutorialPixLogVolC ->noPVolumes();
    info() << DePixC->name() << "  log. volume has " << noppvC    << " phys. volumes: "<<endmsg;
    std::string ManufactC= DePixC->param<std::string>( "ManufacturerName" );
    info() << "The manufacturer for Pixels in "<<DePixC->name()<<"  is "<<ManufactC<<endmsg;
    
    std::vector<double> BiasVoltC= DePixC->param<std::vector<double> >( "BiasVoltage"   );
    info()<<" Bias voltages in "<< DePixC->name() <<"  are "<<BiasVoltC<<endmsg;
  }
  



  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TutorialPixContainerAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TutorialPixContainerAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
