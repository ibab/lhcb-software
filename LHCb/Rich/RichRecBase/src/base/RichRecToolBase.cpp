// $Id: RichRecToolBase.cpp,v 1.2 2003-04-09 12:37:13 cattanem Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "RichRecBase/RichRecToolBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecToolBase
//
// 2002-07-26 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Standard constructor, initializes variables
RichRecToolBase::RichRecToolBase( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : AlgTool ( type, name, parent ) {

}

StatusCode RichRecToolBase::initialize() {

  MsgStream msg( msgSvc(), name() );
  StatusCode sc = StatusCode::SUCCESS;

  // Get pointer to EDS
  if ( !serviceLocator()->service( "EventDataSvc", m_evtDataSvc, true ) ) {
    msg << MSG::ERROR << "EventDataSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecTrack Tool
  if ( !toolSvc()->retrieveTool( "RichRecTrackTool", m_richRecTrackTool) ) {
    msg << MSG::ERROR << "RichRecTrackTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecSegment Tool
  if ( !toolSvc()->retrieveTool( "RichRecSegmentTool", m_richRecSegmentTool) ) {
    msg << MSG::ERROR << "RichRecSegmentTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecPixel Tool
  if ( !toolSvc()->retrieveTool( "RichRecPixelTool", m_richRecPixelTool) ) {
    msg << MSG::ERROR << "RichRecPixelTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecPhoton Tool
  if ( !toolSvc()->retrieveTool( "RichRecPhotonTool", m_richRecPhotonTool) ) {
    msg << MSG::ERROR << "RichRecPhotonTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichDetInterface
  if ( !toolSvc()->retrieveTool( "RichDetInterface", m_richDetInterface ) ) {
    msg << MSG::ERROR << "RichDetInterface not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get the current message service printout level
  IntegerProperty msgLevel;
  IProperty* algIProp;
  this->queryInterface( IID_IProperty, (void**)&algIProp );
  msgLevel.assign( algIProp->getProperty( "OutputLevel" ) );
  m_msgLevel = msgLevel;
  algIProp->release();

  return sc;
}

StatusCode RichRecToolBase::finalize() {

  // Release all tools
  if ( m_richRecPhotonTool )  toolSvc()->releaseTool( m_richRecPhotonTool );
  if ( m_richRecSegmentTool ) toolSvc()->releaseTool( m_richRecSegmentTool );
  if ( m_richRecTrackTool )   toolSvc()->releaseTool( m_richRecTrackTool );
  if ( m_richRecPixelTool )   toolSvc()->releaseTool( m_richRecPixelTool );
  if ( m_richDetInterface )   toolSvc()->releaseTool( m_richDetInterface );
  
  return StatusCode::SUCCESS;  
}
