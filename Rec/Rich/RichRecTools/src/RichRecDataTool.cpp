// $Id: RichRecDataTool.cpp,v 1.1.1.1 2002-07-28 10:46:20 jonesc Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "RichRecTools/RichRecDataTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecDataTool
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRecDataTool>          s_factory ;
const        IToolFactory& RichRecDataToolFactory = s_factory ;

// Standard constructor
RichRecDataTool::RichRecDataTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : AlgTool ( type, name , parent ) {

  declareInterface<IRichRecDataTool>(this);

  // Get pointer to EDS
  StatusCode sc = serviceLocator()->service( "EventDataSvc",
                                             m_eventDataService, true );
  if( sc.isFailure() ) { 
    throw GaudiException( "EventDataSvc not found",
                          "RichRecDataToolException", sc );
  }

}

const SmartDataPtr<RichRecTracks>
RichRecDataTool::getRichTracks ( const std::string location ) {

  MsgStream  log( msgSvc(), name() );

  SmartDataPtr<RichRecTracks> richTracks(m_eventDataService, location);
  if ( !richTracks ) {
    log << MSG::ERROR << "Failed to locate RichRecTracks at "
        << location << endreq;
  } else {
    log << MSG::DEBUG << "Successfully located " << richTracks->size()
        << " RichRecTracks at " << location << endreq;
  }

  return richTracks;
}

const SmartDataPtr<RichRecSegments>
RichRecDataTool::getRichSegments (const std::string location) {

  MsgStream  log( msgSvc(), name() );

  SmartDataPtr<RichRecSegments> richSegments(m_eventDataService, location);
  if ( !richSegments ) {
    log << MSG::ERROR << "Failed to locate RichRecSegments at "
        << location << endreq;
  } else {
    log << MSG::DEBUG << "Successfully located " << richSegments->size()
        << " RichRecSegments at " << location << endreq;
  }

  return richSegments;
}

const SmartDataPtr<RichRecPixels>
RichRecDataTool::getRichPixels (const std::string location) {

  MsgStream  log( msgSvc(), name() );

  SmartDataPtr<RichRecPixels> richPixels(m_eventDataService, location);
  if ( !richPixels ) {
    log << MSG::ERROR << "Failed to locate RichRecPixels at "
        << location << endreq;
  } else {
    log << MSG::DEBUG << "Successfully located " << richPixels->size()
        << " RichRecPixels at " << location << endreq;
  }

  return richPixels;
}
