// $Id: RichRecTrackTool.cpp,v 1.1.1.1 2002-07-28 10:46:19 jonesc Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "RichRecTools/RichRecTrackTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecTrackTool
//
// 15/03/2002 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRecTrackTool>          s_factory ;
const        IToolFactory& RichRecTrackToolFactory = s_factory ;

// Standard constructor
RichRecTrackTool::RichRecTrackTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : AlgTool( type, name, parent ) {

  declareInterface<IRichRecTrackTool>(this);

  // Get Pointer to ToolSvc
  StatusCode scTool = serviceLocator()->service("ToolSvc",
                                                m_toolService, true);
  if( scTool.isFailure() ) {
    throw GaudiException( "ToolSvc not found",
                          "RichRecTrackToolException", scTool );
  }

  // Get pointer to RichRecSegment Tool
  StatusCode scSegment = m_toolService->retrieveTool( "RichRecSegmentTool",
                                                      m_richRecSegmentTool );
  if ( scSegment.isFailure() ) {
    throw GaudiException( "RichRecSegmentTool not found",
                          "RichRecTrackToolException", scSegment );
  }

}

// May be able to rewrite the following two methods as a single one with some
// clever templating. Leave that for the future though.

void RichRecTrackTool::parent ( const SmartRef<RichRecTrack>& track,
                                SmartRef<MCParticle>& pParent ) {

  if ( Rich::RecTrack::MCParticle == track->parentType() ) {
    pParent =
      dynamic_cast<MCParticle*>( (ContainedObject*)track->parentTrack() );
  } else {
    MsgStream log( msgSvc(), name() );
    log << MSG::WARNING << "Track does not derive from an MCParticle" << endreq;
    pParent = 0;
  }

}

void RichRecTrackTool::parent ( const SmartRef<RichRecTrack>& track,
                                SmartRef<TrStoredTrack>& pParent ) {

  if ( Rich::RecTrack::TrStoredTrack == track->parentType() ) {
    pParent =
      dynamic_cast<TrStoredTrack*>( (ContainedObject*)track->parentTrack() );
  } else {
    MsgStream log( msgSvc(), name() );
    log << MSG::WARNING << "Track does not derive from a TrStoredTrack"
        << endreq;
    pParent = 0;
  }

}

double
RichRecTrackTool::expObsPhotonSignal ( const SmartRef<RichRecTrack>& track,
                                       const Rich::ParticleIDType id ) {

  // The total expected number of photons for this track is the summation
  // over the number for each segment for this track.

  double signal = 0.0;
  SmartRefVector<RichRecSegment> segments = track->radiatorSegments();
  for ( SmartRefVector<RichRecSegment>::iterator segment = segments.begin();
        segment != segments.end();
        ++segment ) {
    signal += m_richRecSegmentTool->expObsPhotonSignal( *segment, id );
  }

  return signal;
}

double RichRecTrackTool::expPhotons ( const SmartRef<RichRecTrack>& track,
                                      const Rich::ParticleIDType id ) {

  double signal = 0.0;
  SmartRefVector<RichRecSegment> segments = track->radiatorSegments();
  for ( SmartRefVector<RichRecSegment>::iterator segment = segments.begin();
        segment != segments.end();
        ++segment ) {
    signal += m_richRecSegmentTool->expPhotons( *segment, id );
  }

  return signal;
}
