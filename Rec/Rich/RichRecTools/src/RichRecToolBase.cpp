// $Id: RichRecToolBase.cpp,v 1.1 2002-11-14 13:54:25 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "RichRecTools/RichRecToolBase.h"

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

  MsgStream log( msgSvc(), name() );
  StatusCode sc = StatusCode::SUCCESS;

  // Get pointer to EDS
  if ( !serviceLocator()->service( "EventDataSvc", m_evtDataSvc, true ) ) {
    log << MSG::ERROR << "EventDataSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecTrack Tool
  if ( !toolSvc()->retrieveTool( "RichRecTrackTool", m_richRecTrackTool) ) {
    log << MSG::ERROR << "RichRecTrackTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecSegment Tool
  if ( !toolSvc()->retrieveTool( "RichRecSegmentTool", m_richRecSegmentTool) ) {
    log << MSG::ERROR << "RichRecSegmentTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecPixel Tool
  if ( !toolSvc()->retrieveTool( "RichRecPixelTool", m_richRecPixelTool) ) {
    log << MSG::ERROR << "RichRecPixelTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecPhoton Tool
  if ( !toolSvc()->retrieveTool( "RichRecPhotonTool", m_richRecPhotonTool) ) {
    log << MSG::ERROR << "RichRecPhotonTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichDetInterface
  if ( !toolSvc()->retrieveTool( "RichDetInterface", m_richDetInterface ) ) {
    log << MSG::ERROR << "RichDetInterface not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  return sc;
}
