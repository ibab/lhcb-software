// $Id: RichMCTrackInfoTool.cpp,v 1.1.1.1 2004-01-29 16:39:23 jonesc Exp $

// local
#include "RichMCTrackInfoTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichMCTrackInfoTool
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichMCTrackInfoTool>          s_factory ;
const        IToolFactory& RichMCTrackInfoToolFactory = s_factory ;

// Standard constructor
RichMCTrackInfoTool::RichMCTrackInfoTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : RichToolBase( type, name, parent ),
    m_richDetInt ( 0 ),
    m_smartIDTool( 0 ) {

  declareInterface<IRichMCTrackInfoTool>(this);

}

StatusCode RichMCTrackInfoTool::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool( "RichDetInterface", m_richDetInt  );
  acquireTool( "RichSmartIDTool",  m_smartIDTool );

  return StatusCode::SUCCESS;
}

StatusCode RichMCTrackInfoTool::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release services and tools
  releaseTool( m_richDetInt  );
  releaseTool( m_smartIDTool );

  // Execute base class method
  return RichToolBase::finalize();
}

const bool RichMCTrackInfoTool::panelIntersectGlobal( const MCRichSegment * segment,
                                                      HepPoint3D & hitPoint ) const
{
  const HepVector3D trackDir  = segment->bestMomentum(0.5);
  const HepPoint3D trackPoint = segment->bestPoint(0.5);
  if ( !m_richDetInt->traceToDetectorWithoutEff( segment->rich(),
                                                 trackPoint,
                                                 trackDir,
                                                 hitPoint,
                                                 DeRichPDPanel::loose ) ) return false;
  return true; // all OK
}

const bool RichMCTrackInfoTool::panelIntersectLocal( const MCRichSegment * segment,
                                                     HepPoint3D & hitPoint ) const
{

  // find global point
  HepPoint3D globalPoint;
  if ( !panelIntersectGlobal( segment, globalPoint ) ) return false;
  // convert global to local position
  hitPoint = m_smartIDTool->globalToPDPanel(globalPoint); 

  return true; // all OK
}
