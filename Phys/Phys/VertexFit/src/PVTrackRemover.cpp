// $Id: $
// Include files 

//
#include "TrackInterfaces/IPVOfflineTool.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "LoKi/PhysExtract.h"
// local
#include "PVTrackRemover.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PVTrackRemover
//
// 2010-12-07 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PVTrackRemover )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVTrackRemover::PVTrackRemover( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_pvToolType("PVOfflineTool"),
    m_pvTool(0)
{

  declareInterface<IPVReFitter>(this);

  declareProperty("IPVOffline", m_pvToolType);
  
}
//=============================================================================
StatusCode PVTrackRemover::initialize() 
{
  m_pvTool = tool<IPVOfflineTool>(m_pvToolType);
  warning() << "PVTrackRemover is no longer maintained and thus deprecated." 
            << endmsg;

  if (0==m_pvTool) {
    return Error("Could not load IPVOfflineTool " + m_pvToolType);
  }

  return StatusCode::SUCCESS;
  
}
//=============================================================================
StatusCode PVTrackRemover::reFit(LHCb::VertexBase*) const
{
  return Error("PVTrackRemover::reFit makes no sense!");
}
//=============================================================================
StatusCode PVTrackRemover::remove(const LHCb::Particle* particle,
                                  LHCb::VertexBase* PV) const 
{
 
  if (0==PV) return Error("NULL input PV");

  LHCb::RecVertex* outPV = dynamic_cast<LHCb::RecVertex*>(PV);
  
  if (0==outPV) return Error("Could now cast VertexBase to RecVertex");

  const LHCb::RecVertex referencePV(*outPV);
  
  std::vector<const LHCb::Track*> tracks;

  LoKi::Extract::tracks(particle, std::back_inserter(tracks));

  m_pvTool->removeTracksAndRecalculatePV(&referencePV,
                                         tracks,
                                         *outPV);
  
  return StatusCode::SUCCESS;

}
//=============================================================================
// Destructor
//=============================================================================
PVTrackRemover::~PVTrackRemover() {} 

//=============================================================================
