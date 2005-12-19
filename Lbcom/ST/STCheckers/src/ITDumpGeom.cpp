//
// This File contains the definition of the OTSmearer -class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

#include <iostream>

// Gaudi
#include "GaudiKernel/AlgFactory.h"


#include "ITDumpGeom.h"

// xml geometry
#include "DetDesc/IGeometryInfo.h"
#include "STDet/DeITDetector.h"
#include "STDet/DeITStation.h"
#include "STDet/DeSTStation.h"
#include "STDet/DeITLayer.h"
#include "STDet/DeITBox.h"
#include "STDet/DeITSector.h"

static const AlgFactory<ITDumpGeom> s_Factory;
const IAlgFactory& ITDumpGeomFactory = s_Factory;

//--------------------------------------------------------------------
//
//  ITDumpGeom : Check digitization procedure for the outer tracker
//
//--------------------------------------------------------------------

ITDumpGeom::ITDumpGeom(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator),
  m_tracker(0)
{
  // constructer
}

ITDumpGeom::~ITDumpGeom(){
  // destructer
}

StatusCode ITDumpGeom::initialize(){


  // initialize  
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  m_tracker =  getDet<DeITDetector>(DeSTDetLocation::IT);

  std::vector<DeSTStation*>::const_iterator iterStation = m_tracker->stations().begin();
  for (; iterStation != m_tracker->stations().end() ; ++iterStation){
    DeITStation* tStation =  dynamic_cast<DeITStation*>(*iterStation);
    std::cout << tStation << std::endl;
    DeITStation::Children::const_iterator iterBox = tStation->boxes().begin();
    for (;iterBox != tStation->boxes().end(); ++iterBox){
      DeITBox* tBox = *iterBox;
      std::cout << tBox << std::endl;
      DeITBox::Children::const_iterator iterLayer = tBox->layers().begin();
      for (;iterLayer != tBox->layers().end(); ++iterLayer){
        DeITLayer* tLayer = *iterLayer;
	std::cout << tLayer << std::endl;
        DeITLayer::Children::const_iterator iterSector = tLayer->sectors().begin();
        for ( ; iterSector !=  tLayer->sectors().end() ; ++iterSector ){
          DeSTSector* tSector = *iterSector;
	  std::cout << tSector << std::endl;
	} //sectors     
      } // half module
    }  // iterLayer
  } // iterStation

  return StatusCode::SUCCESS;

}

StatusCode ITDumpGeom::execute(){

  // execute once per event
  return StatusCode::SUCCESS;
}







