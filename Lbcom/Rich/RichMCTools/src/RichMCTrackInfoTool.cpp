
//-----------------------------------------------------------------------------
/** @file RichMCTrackInfoTool.cpp
 *
 * Implementation file for class : RichMCTrackInfoTool
 *
 * CVS Log :-
 * $Id: RichMCTrackInfoTool.cpp,v 1.9 2005-12-17 14:18:15 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichMCTrackInfoTool.h"

// namespaces
using namespace LHCb;

// Declaration of the Tool Factory
static const  ToolFactory<RichMCTrackInfoTool>          s_factory ;
const        IToolFactory& RichMCTrackInfoToolFactory = s_factory ;

// Standard constructor
RichMCTrackInfoTool::RichMCTrackInfoTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : RichToolBase  ( type, name, parent ),
    m_rayTrace    ( 0 ),
    m_smartIDTool ( 0 )
{
  declareInterface<IRichMCTrackInfoTool>(this);
}

StatusCode RichMCTrackInfoTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",   m_rayTrace             );
  acquireTool( "RichSmartIDTool",  m_smartIDTool, 0, true );

  // Configure the ray-tracing mode
  m_traceMode.setDetPrecision      ( RichTraceMode::circle );
  m_traceMode.setDetPlaneBound     ( RichTraceMode::loose  );
  m_traceMode.setForcedSide        ( false                 );
  m_traceMode.setOutMirrorBoundary ( false                 );
  m_traceMode.setMirrorSegBoundary ( false                 );

  return sc;
}

StatusCode RichMCTrackInfoTool::finalize()
{
  // Execute base class method
  return RichToolBase::finalize();
}

const bool
RichMCTrackInfoTool::panelIntersectGlobal( const LHCb::MCRichSegment * segment,
                                           Gaudi::XYZPoint & hitPoint ) const
{
  return ( 0 != segment &&
           m_rayTrace->traceToDetectorWithoutEff( segment->rich(),
                                                  segment->bestPoint(0.5),
                                                  segment->bestMomentum(0.5),
                                                  hitPoint,
                                                  m_traceMode ) );
}

const bool
RichMCTrackInfoTool::panelIntersectLocal( const LHCb::MCRichSegment * segment,
                                          Gaudi::XYZPoint & hitPoint ) const
{

  // find global point
  Gaudi::XYZPoint globalPoint;
  if ( !panelIntersectGlobal( segment, globalPoint ) ) return false;

  // convert global to local position
  hitPoint = m_smartIDTool->globalToPDPanel(globalPoint);

  return true; // all OK
}
