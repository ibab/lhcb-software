// $Id: DeRichRadiator.cpp,v 1.4 2003-11-21 22:46:10 jonesc Exp $
#define DERICHRADIATOR_CPP

// Include files
#include "RichDet/DeRichRadiator.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "Kernel/CLHEPStreams.h"

// CLHEP files
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

/// Detector description classes
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBoolean.h"
#include "DetDesc/SolidTrd.h"


//----------------------------------------------------------------------------
//
// Implementation of class :  DeRichRadiator
//
//----------------------------------------------------------------------------

const CLID& CLID_DeRichRadiator = 12040;  // User defined

// Standard Constructor
DeRichRadiator::DeRichRadiator() {}

// Standard Destructor
DeRichRadiator::~DeRichRadiator() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichRadiator::classID() {
  return CLID_DeRichRadiator;
}

StatusCode DeRichRadiator::initialize() {

  StatusCode sc = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), "DeRichRadiator" );
  log << MSG::DEBUG <<"Starting initialisation for DeRichRadiator"<< endreq;

  m_solid = geometry()->lvolume()->solid();
  std::string tempName = name();

  const std::string::size_type pos = tempName.find("Rich2");
  if( std::string::npos != pos )
    m_radiatorID = Rich::CF4;
  else {
    const std::string::size_type pos = tempName.find("Aerogel");
    if( std::string::npos != pos )
      m_radiatorID = Rich::Aerogel;
    else {
      const std::string::size_type pos = tempName.find("C4F10");
      if( std::string::npos != pos )
        m_radiatorID = Rich::C4F10;
      else {
        log << MSG::ERROR << "Cannot find radiator type" << endreq;
        sc = StatusCode::FAILURE;
      }
    }
  }

  HepPoint3D zero(0.0, 0.0, 0.0);
  log << MSG::DEBUG << "Centre of radiator " << tempName << "  " 
      << geometry()->toGlobal(zero) << endreq;
  log << MSG::DEBUG <<"Finished initialisation for DeRichRadiator"<< endreq;

  return sc;
}

StatusCode DeRichRadiator::nextIntersectionPoint( const HepPoint3D&  pGlobal,
                                                  const HepVector3D& vGlobal,
                                                  HepPoint3D&  returnPoint ) {

  HepPoint3D pLocal = geometry()->toLocal(pGlobal);
  HepVector3D vLocal = vGlobal;
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (0 == noTicks) {
    return StatusCode::FAILURE;
  }
  else {
    HepPoint3D tempPointLocal = pLocal + ticks[0] * vLocal;
    returnPoint = geometry()->toGlobal(tempPointLocal);
    return StatusCode::SUCCESS;
  }

}


unsigned int DeRichRadiator::intersectionPoints( const HepPoint3D& pGlobal,
                                                 const HepVector3D& vGlobal,
                                                 std::vector<HepPoint3D>& 
                                                 points) {

  HepPoint3D pLocal = geometry()->toLocal(pGlobal);
  HepVector3D vLocal = vGlobal;
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (noTicks != 0) {
    for (ISolid::Ticks::iterator tick_it = ticks.begin();
         tick_it != ticks.end();
         ++tick_it) {
      HepPoint3D tempPointLocal = pLocal + (*tick_it) * vLocal;
      HepPoint3D tempPointGlobal = geometry()->toGlobal(tempPointLocal);
      points.push_back(tempPointGlobal);
    }
  }
  return noTicks;
}
