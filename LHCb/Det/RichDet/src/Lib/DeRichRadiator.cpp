
/** @file DeRichRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichRadiator.cpp,v 1.6 2004-07-27 08:55:23 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

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

StatusCode DeRichRadiator::initialize() {

  MsgStream msg(msgSvc(), "DeRichRadiator" );
  msg << MSG::DEBUG <<"Starting initialisation for DeRichRadiator"<< endreq;

  std::string tempName = name();

  const std::string::size_type pos = tempName.find("Rich2");
  if( std::string::npos != pos ) {
    m_radiatorID = Rich::CF4;
    m_rich = Rich::Rich2;
  }
  else {
    const std::string::size_type pos = tempName.find("Aerogel");
    if( std::string::npos != pos ) {
      m_radiatorID = Rich::Aerogel;
      m_rich = Rich::Rich1;
    }
    else {
      const std::string::size_type pos = tempName.find("C4F10");
      if( std::string::npos != pos ) {
        m_radiatorID = Rich::C4F10;
        m_rich = Rich::Rich1;
      }
      else {
        msg << MSG::ERROR << "Cannot find radiator type" << endreq;
        return StatusCode::FAILURE;
      }
    }
  }

  msg << MSG::DEBUG <<"Finished initialisation for DeRichRadiator"<< endreq;

  return StatusCode::SUCCESS;
}
