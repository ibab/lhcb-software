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


#include "STDumpGeom.h"

// xml geometry
#include "DetDesc/IGeometryInfo.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

static const AlgFactory<STDumpGeom> s_Factory;
const IAlgFactory& STDumpGeomFactory = s_Factory;

//--------------------------------------------------------------------
//
//  STDumpGeom : Check digitization procedure for the outer tracker
//
//--------------------------------------------------------------------

STDumpGeom::STDumpGeom(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator),
  m_tracker(0)
{
  // constructer
  declareProperty("detType", m_detType="TT");
}

STDumpGeom::~STDumpGeom(){
  // destructer
}

StatusCode STDumpGeom::initialize(){

  // initialize  
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }


  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));
  if (m_tracker != 0 ){
    DeSTDetector::Sectors tSectors = m_tracker->sectors();
    DeSTDetector::Sectors::const_iterator iterSector = tSectors.begin();
    for (; iterSector != tSectors.end() ; ++iterSector ){
      std::cout << *iterSector <<std::endl; 
    }
 }


  return StatusCode::SUCCESS;

}

StatusCode STDumpGeom::execute(){

  // execute once per event
  return StatusCode::SUCCESS;
}







