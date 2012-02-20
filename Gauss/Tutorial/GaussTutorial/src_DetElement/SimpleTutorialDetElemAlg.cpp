// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IDataManagerSvc.h"

#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/TabulatedProperty.h"


// local
#include "SimpleTutorialDetElemAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SimpleTutorialDetElemAlg
//
// 2012-02-13 : Sajan Easo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SimpleTutorialDetElemAlg )



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimpleTutorialDetElemAlg::SimpleTutorialDetElemAlg( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
SimpleTutorialDetElemAlg::~SimpleTutorialDetElemAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode SimpleTutorialDetElemAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize Simple Tutorial Detelem Alg" << endmsg;

  info() << "Initialization starting..." << endmsg;
  info() << "///////////////////////////////////////////////////" << endmsg;
  info() << "// Very basic test of Detector Element retrieval //" << endmsg;
  info() << "///////////////////////////////////////////////////" << endmsg;

  IDetectorElement* cave = getDet<IDetectorElement>( "/dd/Structure/LHCb" );
  info() << "Top detector element of LHCb was successfully retrieved." << endmsg;
  info() << "Here are some informations about it : " << endmsg;

  const ILVolume* stvol = cave->geometry()->lvolume();
  info() << "LHCb detector is made of " << stvol->materialName() << endmsg;

  dumpTutorialPVs (stvol, cave->name());
   

   DetectorElement* TMasterDet=  getDet<DetectorElement>( "/dd/Structure/LHCb/AfterMagnetRegion/TutorialMasterDet");
   const ILVolume* TutorialMasterLogVol = TMasterDet->geometry()->lvolume();
 
   info() << "Tutorial Master  is made of " << TutorialMasterLogVol->materialName() << endmsg;
  
   dumpTutorialPVs (TutorialMasterLogVol, TMasterDet->name());
   int myTestParam = TMasterDet->param<int>("MyTutorialSpecialParamTestA");
   std::vector<double> myParamVect = TMasterDet->param<std::vector<double> >("MyTutorialSpecialVectorTestA");
   
   std::vector<int> NumPixelsArray =  TMasterDet->param<std::vector<int> >("NumPixInArray");
   
   info()<<" User Parameters from TutorialMaster "<<myTestParam<<"   "<<myParamVect<<"   "<<NumPixelsArray<<endmsg;
   


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SimpleTutorialDetElemAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SimpleTutorialDetElemAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
void SimpleTutorialDetElemAlg::dumpTutorialPVs (const ILVolume* lv, std::string de) {

  unsigned long noppv = lv->noPVolumes();
  info() << de << " log. volume has " << noppv    << " phys. volumes: ";

  unsigned int ppvc;
  for (ppvc = 0; ppvc < noppv; ppvc++) {
    info() << lv->pvolume(ppvc)->name() << " ";
  }
  
  info() << endmsg;
}
