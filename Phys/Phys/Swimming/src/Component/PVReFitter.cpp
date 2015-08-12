// Include files 

// Event
#include <Event/VertexBase.h>

// Swimming
#include <Swimming/Interfaces.h>

// boost
#include <boost/foreach.hpp>

// local
#include "PVReFitter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SwimmingPVReFitter
//
// 2011-11-24 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_NAMESPACE_TOOL_FACTORY(Swimming, PVReFitter)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Swimming::PVReFitter::PVReFitter(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
: base_class(type, name , parent), m_refitter(0),
   m_service(0)
{
   declareInterface<IPVReFitter>(this);

   declareProperty("PVReFitter", m_refitterName = "AdaptivePVReFitter");
   declareProperty("Service", m_serviceName = "Swimming::Service");
}

//=============================================================================
// Destructor
//=============================================================================
Swimming::PVReFitter::~PVReFitter()
{

}

//=============================================================================
// Initialize
//=============================================================================
StatusCode Swimming::PVReFitter::initialize(){
   StatusCode sc = GaudiTool::initialize();
   if (!sc) return sc;

   m_refitter = tool<IPVReFitter>(m_refitterName, parent());
   m_service = svc<Swimming::Interfaces::IService>(m_serviceName);
   return sc;
}

//=============================================================================
// refit PV
//=============================================================================
StatusCode Swimming::PVReFitter::reFit(LHCb::VertexBase* PV) const {

   StatusCode sc = m_refitter->reFit(PV);
   if (sc.isSuccess()) {
      PV->setPosition(PV->position() + m_service->offset());
   }
   return sc;
}

//=============================================================================
// remove track used for a LHCb::Particle and refit PV
//=============================================================================
StatusCode Swimming::PVReFitter::remove(const LHCb::Particle* part,  
                                        LHCb::VertexBase* PV) const {
   // Do the refitting
   StatusCode sc = m_refitter->remove(part, PV);
   if (sc.isSuccess()) {
      PV->setPosition(PV->position() + m_service->offset());
   }
   return sc;
}
