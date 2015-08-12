// $Id: $
// Include files 

// Gaudi
#include <GaudiKernel/MsgStream.h>
#include <GaudiKernel/Incident.h>
#include <GaudiKernel/IIncidentSvc.h>
#include <GaudiKernel/SmartDataPtr.h>
#include <GaudiKernel/IDataProviderSvc.h>

// DaVinci
#include <Kernel/IPVReFitter.h>

// Event
#include <Event/TurningPoint.h>
#include <Event/RecVertex.h>

// boost
#include <boost/range/iterator_range.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>

// stdlib
#include <string>

// local
#include "Service.h"

namespace IncidentType {
   // Out own incident type
   const std::string BeginSwimming = "BeginSwimming";
}

namespace Swimming {
   DECLARE_NAMESPACE_SERVICE_FACTORY(Swimming, Service)
}
//-----------------------------------------------------------------------------
// Implementation file for class : Service::VertexEntry
//
// 2012-01-17 : Roel Aaij
//-----------------------------------------------------------------------------
//=============================================================================
Swimming::Service::VertexEntry::VertexEntry
(LHCb::RecVertex* pv, const LHCb::RecVertex* const original,
 const LHCb::Particle* const particle, const std::string& refitter)
   : m_refitter(refitter), m_pv(pv), m_original(original), m_particle(particle)
{
   m_particleHash = Swimming::Helpers::hashParticle(*particle);
   size_t seed = m_particleHash;
   boost::hash_combine(seed, refitter);
   m_hash = seed;
}

//-----------------------------------------------------------------------------
// Implementation file for class : Service
//
// 2011-11-24 : Roel Aaij
//-----------------------------------------------------------------------------
//=============================================================================
Swimming::Service::Service(const std::string& name, ISvcLocator* svcloc)
   : base_class(name, svcloc), m_incidentSvc(0), m_evtSvc(0),
     m_offset(0., 0., 0.), m_offlinePVs(0)
{
   declareProperty("OfflinePVLocation", m_offlineLocation = "/Event/Rec/Vertex/Primary");
   declareProperty("RefitPVs", m_refitPVs = false);
}

//=============================================================================
Swimming::Service::~Service()
{

}

//=========================================================================
StatusCode Swimming::Service::initialize() {

   // base class initialization
   StatusCode sc = ::Service::initialize();
   if (!sc.isSuccess()) return sc;

   // local initialization
   MsgStream log(msgSvc(),name());
   if( log.level() <= MSG::DEBUG ) {
      log << MSG::DEBUG << "--- initialize ---" << endmsg;
      log << MSG::DEBUG << m_offset.x() << " " << m_offset.y() << " " << m_offset.z()
          << endmsg;
   }

   // grab IncidentSvc
   if (!service( "IncidentSvc", m_incidentSvc).isSuccess()) {
      fatal() << "Failed to get the IncidentSvc." << endmsg;
      return StatusCode::FAILURE;
   }
   m_incidentSvc->addListener(this, IncidentType::BeginSwimming);
   m_incidentSvc->addListener(this, IncidentType::EndEvent);

   // grab TES
   if (!service("EventDataSvc", m_evtSvc).isSuccess()) {
      fatal() << "Failed to get the EventDataSvc." << endmsg;
      return StatusCode::FAILURE;
   }
   return sc;
}

//=========================================================================
StatusCode Swimming::Service::finalize ( ) {

   // local finalization
   MsgStream log(msgSvc(),name());
   if( log.level() <= MSG::DEBUG )
      log << MSG::DEBUG << "--- finalize ---" << endmsg;

   return ::Service::finalize();
}

//=============================================================================
// Handle Incidents
//=============================================================================
void Swimming::Service::handle(const Incident& incident)
{
   if (incident.type() == IncidentType::BeginSwimming) {
      // Delete old PV clones
      BOOST_FOREACH(const VertexEntry& entry, m_pvs.get<Refitter>()) {
         if (entry.pv()) delete entry.pv();
      }
      m_pvs.clear();
      // Delete old original PVs
      BOOST_FOREACH(const Originals::value_type& entry, m_originals) {
         delete entry.second;
      }
      m_originals.clear();
      // Reset offlinePV container pointer
      m_offlinePVs = 0;
      // Load new PVs
      loadPVs();
      // Insert PVs as originals
      BOOST_FOREACH(const LHCb::RecVertex* pv, *m_offlinePVs) {
         m_originals.insert(std::make_pair(pv->key(), pv->clone()));
      }
   } else if (incident.type() == IncidentType::EndEvent) {
      // Reset offline PV pointer
      m_offlinePVs = 0;
   }
}

//=============================================================================
void Swimming::Service::setOffset(const Gaudi::XYZVector& o)
{
   m_offset.SetXYZ(o.x(), o.y(), o.z());
   const Vertices& pvs = m_pvs.get<Refitter>();
   BOOST_FOREACH(const VertexEntry& entry, pvs) {
      LHCb::RecVertex* pv = entry.pv();
      if (!pv) continue;
      pv->setPosition(entry.original()->position() + m_offset);
   }
   if (!loadPVs()) return;
   BOOST_FOREACH(LHCb::RecVertex* pv, *m_offlinePVs) {
      pv->setPosition(originalPV(pv)->position() + m_offset);
   }
}

//=============================================================================
LHCb::RecVertex::ConstVector Swimming::Service::getOfflinePVs
(const LHCb::Particle* particle, const IPVReFitter* refitter)
{
   size_t hash = Swimming::Helpers::hashParticle(*particle);
   std::string refitterName = refitter ? refitter->name() : "";

   // 
   VertexRange vertices = m_pvs.get<Refitter>().equal_range(boost::make_tuple(hash, refitterName));
   LHCb::RecVertex::ConstVector output;
   
   // Load offline PVs
   if (!loadPVs()) return output;

   if (vertices.empty()) {
      BOOST_FOREACH(const LHCb::RecVertex* pv, *m_offlinePVs) {
         LHCb::RecVertex* clone = pv->clone();
         const LHCb::RecVertex* original = originalPV(pv);
         if (refitter) {
            // Refit the clone and add it to the output.
            StatusCode sc = refitter->remove(particle, clone);
            if (sc.isSuccess()) {
               m_pvs.insert(VertexEntry(clone, original, particle, refitterName));
               clone->setPosition(original->position() + m_offset);
               output.push_back(clone);
            } else {
               m_pvs.insert(VertexEntry(0, original, particle, refitterName));
               delete clone;
            }
         } else {
            // No refitter passed, add the clone and move it.
            m_pvs.insert(VertexEntry(clone, original, particle, refitterName));
            clone->setPosition(original->position() + m_offset);
            output.push_back(clone);
         }
      }
   } else {
      BOOST_FOREACH(const VertexEntry& entry, vertices) {
         LHCb::RecVertex* pv = entry.pv();
         if (!entry.pv()) {
            continue;
         }
         pv->setPosition(entry.original()->position() + m_offset);
         output.push_back(pv);
      }
   }
   return output;
}

//=============================================================================
void Swimming::Service::setSignalTracks(const LHCb::Particle* candidate)
{
   m_signalTracks.clear();
   if (!candidate) return;

   LHCb::Track::ConstVector fst;
   finalStateTracks(candidate, fst);
   BOOST_FOREACH(const LHCb::Track* track, fst) {
      m_signalTracks.push_back(track);
   }
}

//=============================================================================
const std::vector<const LHCb::Track*>& Swimming::Service::getSignalTracks() const
{
   return m_signalTracks;
}

//=============================================================================
const LHCb::RecVertex* Swimming::Service::originalPV(const LHCb::RecVertex* pv) const
{
   Originals::const_iterator it = m_originals.find(pv->key());
   if (it == m_originals.end()) {
      std::stringstream msg;
      msg << "No original PV for key " << pv->key();
      throw GaudiException("Swimming::Service", msg.str(), StatusCode::FAILURE);
   } else {
      return it->second;
   }
}

//=============================================================================
bool Swimming::Service::loadPVs()
{
   if (m_offlinePVs) {
      return true;
   }
   SmartDataPtr<LHCb::RecVertex::Container> getter(m_evtSvc, m_offlineLocation);
   MsgStream log(msgSvc(),name());
   if (!getter) {
      log << MSG::WARNING << "Cannot get Offline PVs from " << m_offlineLocation 
          << "," << endmsg;
      return false;
   } else {
      m_offlinePVs = getter;
      return true;
   }
}

//=============================================================================
void Swimming::Service::finalStateTracks(const LHCb::Particle* particle,
                                         LHCb::Track::ConstVector& fst)
{
   if (particle->isBasicParticle()) {
      if (particle->proto() && particle->proto()->track()) {
         fst.push_back(particle->proto()->track());
      }
   } else {
      BOOST_FOREACH(const LHCb::Particle* daughter, particle->daughtersVector()) {
         finalStateTracks(daughter, fst);
      }
   }
}
