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


#include "TTDumpGeom.h"

// xml geometry
#include "DetDesc/IGeometryInfo.h"
#include "STDet/DeTTDetector.h"
#include "STDet/DeTTStation.h"
#include "STDet/DeSTStation.h"
#include "STDet/DeTTLayer.h"
#include "STDet/DeTTHalfModule.h"
#include "STDet/DeTTSector.h"

static const AlgFactory<TTDumpGeom> s_Factory;
const IAlgFactory& TTDumpGeomFactory = s_Factory;

//--------------------------------------------------------------------
//
//  TTDumpGeom : Check digitization procedure for the outer tracker
//
//--------------------------------------------------------------------

TTDumpGeom::TTDumpGeom(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator),
  m_tracker(0)
{
  // constructer
}

TTDumpGeom::~TTDumpGeom(){
  // destructer
}

StatusCode TTDumpGeom::initialize(){

  // initialize  
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }


  m_tracker =  getDet<DeTTDetector>(DeSTDetLocation::TT);
  if (m_tracker != 0 ){
    DeSTDetector::Sectors tSectors = m_tracker->sectors();
    DeSTDetector::Sectors::const_iterator iterSector = tSectors.begin();
    for (; iterSector != tSectors.end() ; ++iterSector ){
      std::cout << *iterSector <<std::endl; 
    }
 }


  return StatusCode::SUCCESS;

}

StatusCode TTDumpGeom::execute(){

  // execute once per event
  return StatusCode::SUCCESS;
}







