
//----------------------------------------------------------------------------
/** @file DeRichRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichRadiator
 *
 *  $Id: DeRichRadiator.cpp,v 1.16 2006-04-03 08:57:11 jonrob Exp $
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
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"

//----------------------------------------------------------------------------

//=========================================================================
//  default constructor
//=========================================================================
DeRichRadiator::DeRichRadiator() :
  DetectorElement       (),
  m_radiatorID          ( Rich::InvalidRadiator ),
  m_rich                ( Rich::InvalidDetector ),
  m_refIndex            ( 0                     ),
  m_refIndexTabProp     ( 0                     ),
  m_chkvRefIndexTabProp ( 0                     ),
  m_rayleigh            ( 0                     ),
  m_rayleighTabProp     ( 0                     ),
  m_absorptionTabProp   ( 0                     ),
  m_firstUpdate         ( true                  ),
  m_name                ( "UnInitialized"       )
{ }


//=========================================================================
//  destructor
//=========================================================================
DeRichRadiator::~DeRichRadiator()
{}


StatusCode DeRichRadiator::initialize()
{
  // store the name of the radiator
  const std::string::size_type pos = name().find("Rich");
  m_name = ( std::string::npos != pos ? name().substr(pos) : "DeRichRadiator_NO_NAME" );

  MsgStream msg( msgSvc(), myName() );
  msg << MSG::DEBUG << "Starting initialisation" << endreq;

  if ( std::string::npos != name().find("Rich2") ) {
    m_radiatorID = Rich::Rich2Gas;
    m_rich = Rich::Rich2;
  }
  else {
    if ( std::string::npos != name().find("Aerogel") ) {
      m_radiatorID = Rich::Aerogel;
      m_rich = Rich::Rich1;
    }
    else {
      if ( std::string::npos != name().find("Rich1Gas") ) {
        m_radiatorID = Rich::Rich1Gas;
        m_rich = Rich::Rich1;
      }
      else {
        msg << MSG::ERROR << "Cannot find radiator type for " << name() << endreq;
        return StatusCode::FAILURE;
      }
    }
  }

  msg << MSG::DEBUG << "Initializing Radiator : " << rich() << " " << radiatorID() << endreq;

  return StatusCode::SUCCESS;
}
