
//----------------------------------------------------------------------------
/** @file DeRichRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichRadiator
 *
 *  $Id: DeRichRadiator.cpp,v 1.11 2005-02-23 10:26:00 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#define DERICHRADIATOR_CPP

// Include files
#include "RichDet/DeRichRadiator.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

//----------------------------------------------------------------------------

StatusCode DeRichRadiator::initialize() 
{

  // store the name of the radiator
  const std::string::size_type pos = name().find("Rich");
  m_name = ( std::string::npos != pos ? name().substr(pos) : "DeRichRadiator_NO_NAME" );

  MsgStream msg( msgSvc(), myName() );
  msg << MSG::DEBUG << "Starting initialisation" << endreq;

  if ( std::string::npos != name().find("Rich2") ) {
    m_radiatorID = Rich::CF4;
    m_rich = Rich::Rich2;
  }
  else {
    if ( std::string::npos != name().find("Aerogel") ) {
      m_radiatorID = Rich::Aerogel;
      m_rich = Rich::Rich1;
    }
    else {
      if ( std::string::npos != name().find("C4F10") ) {
        m_radiatorID = Rich::C4F10;
        m_rich = Rich::Rich1;
      }
      else {
        msg << MSG::ERROR << "Cannot find radiator type for " << name() << endreq;
        return StatusCode::FAILURE;
      }
    }
  }

  msg << MSG::INFO << "Initializing Radiator : " << m_rich << " " << m_radiatorID << endreq;

  return StatusCode::SUCCESS;
}
