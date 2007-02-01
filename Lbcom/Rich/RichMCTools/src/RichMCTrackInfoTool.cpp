
//-----------------------------------------------------------------------------
/** @file RichMCTrackInfoTool.cpp
 *
 * Implementation file for class : RichMCTrackInfoTool
 *
 * CVS Log :-
 * $Id: RichMCTrackInfoTool.cpp,v 1.12 2007-02-01 17:50:13 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 15/03/2002
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

// local
#include "RichMCTrackInfoTool.h"

// namespace
using namespace Rich::MC;

DECLARE_TOOL_FACTORY( MCTrackInfoTool );

// Standard constructor
MCTrackInfoTool::MCTrackInfoTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichToolBase  ( type, name, parent ),
    m_rayTrace    ( NULL ),
    m_smartIDTool ( NULL ),
    m_traceMode   ( LHCb::RichTraceMode::IgnoreHPDAcceptance )
{
  declareInterface<IMCTrackInfoTool>(this);
}

StatusCode MCTrackInfoTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",   m_rayTrace             );
  acquireTool( "RichSmartIDTool",  m_smartIDTool, 0, true );

  // Configure the ray-tracing mode
  info() << "Track " << m_traceMode << endreq;

  return sc;
}

StatusCode MCTrackInfoTool::finalize()
{
  // Execute base class method
  return RichToolBase::finalize();
}

const bool
MCTrackInfoTool::panelIntersectGlobal( const LHCb::MCRichSegment * segment,
                                       Gaudi::XYZPoint & hitPoint ) const
{
  return ( 0 != segment &&
           m_rayTrace->traceToDetector( segment->rich(),
                                        segment->bestPoint(0.5),
                                        segment->bestMomentum(0.5),
                                        hitPoint,
                                        m_traceMode ) );
}

const bool
MCTrackInfoTool::panelIntersectLocal( const LHCb::MCRichSegment * segment,
                                      Gaudi::XYZPoint & hitPoint ) const
{

  // find global point
  Gaudi::XYZPoint globalPoint;
  if ( !panelIntersectGlobal( segment, globalPoint ) ) return false;

  // convert global to local position
  hitPoint = m_smartIDTool->globalToPDPanel(globalPoint);

  return true; // all OK
}
