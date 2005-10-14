
//----------------------------------------------------------------------------
/** @file DeRichRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichRadiator
 *
 *  $Id: DeRichRadiator.cpp,v 1.13 2005-10-14 08:21:37 jonrob Exp $
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

  msg << MSG::DEBUG << "Initializing Radiator : " << rich() << " " << radiatorID() << endreq;

  // Trick to load up the RichConditions tool for this detector element
  // need to make sure it is created before this object is initialised
  const std::string richConName =
    ( Rich::Rich1 ==  rich() ? "Rich1PresTempMonitor" : "Rich2PresTempMonitor" );
  StatusCode sc = StatusCode::FAILURE;
  // get the Gaudi service locator
  static ISvcLocator * svcLoc = Gaudi::svcLocator();
  if (svcLoc)
  {
    // get the tool service
    IToolSvc * toolSvc;
    svcLoc->service( "ToolSvc", toolSvc, true );
    if (toolSvc)
    {
      // load the RICH conditions tool for this element
      msg << MSG::DEBUG << "Loading tool " << richConName << endreq;
      sc = toolSvc->retrieveTool ( richConName , m_condTool , 0 , true ) ;
    }
    else
    {
      msg << MSG::ERROR << "Failed to find ToolSvc" << endreq;
      return StatusCode::FAILURE;
    }
  } 
  else
  {
    msg << MSG::ERROR << "Failed to find Gaudi::svcLocator()" << endreq;
    return StatusCode::FAILURE;
  }
  if (!sc)
  {
    static bool errPrinted = false;
    if ( !errPrinted )
    {
      msg << MSG::WARNING 
          << "Failed to load RichCondition tools" << endreq
          << "Some conditions data will not be updated properly" << endreq
          << "Please add 'RichCondition' to the list of DLLs" << endreq;
      errPrinted = true;
    }
  }
  
  return StatusCode::SUCCESS;
}

DeRichRadiator::~DeRichRadiator()
{
  // CRJ : Note sure if this should be done ??
  //     : Probably should be in a finalize() method but DetectorElements
  //     : do not have this method. If done here causes a crash at the end
  //     : of the job :(
  // if ( m_condTool ) { m_condTool->release(); }
}
