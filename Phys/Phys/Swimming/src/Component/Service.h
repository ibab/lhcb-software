// $Id: $
#ifndef SWIMMINGSERVICE_H 
#define SWIMMINGSERVICE_H 1

// Include files
#include <GaudiKernel/Service.h>
#include <GaudiKernel/Vector3DTypes.h>
#include <GaudiKernel/IIncidentListener.h>

// Event
#include <Event/RecVertex.h>

// boost
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/unordered_map.hpp>

// Interface
#include <Swimming/Interfaces.h>

class IPVRefitter;
class IDataProviderSvc;

namespace Swimming {

// namespace Vertex2Vertex {
//    typedef LHCb::Relation1D<LHCb::RecVertex, LHCb::RecVertex> Table;
//    typedef Table::IBase::TypeTraits::Entry   Relation;
//    typedef Table::IBase::TypeTraits::Entries Relations;
// }

class GAUDI_API Service: public extends2< ::Service, Interfaces::IService, IIncidentListener> {
public: 

   Service(const std::string& name, ISvcLocator* svcloc); 

   virtual ~Service(); 

   virtual StatusCode initialize();
  
   virtual StatusCode finalize();

   virtual void handle(const Incident&);

   virtual Gaudi::XYZVector offset() const
   {
      return m_offset;
   }

   virtual void setOffset(const Gaudi::XYZVector& o);

   virtual LHCb::RecVertex::ConstVector getOfflinePVs(const LHCb::Particle* particle,
                                                      const IPVReFitter* refitter = 0);

   virtual void setSignalTracks(const LHCb::Particle* candidate = 0);
   virtual const std::vector<const LHCb::Track*>& getSignalTracks() const;

private:

   const LHCb::RecVertex* originalPV(const LHCb::RecVertex* pv) const;
   bool loadPVs();

   void finalStateTracks(const LHCb::Particle* particle,
                         LHCb::Track::ConstVector& fst);
   
   class VertexEntry {
   public:
      VertexEntry(LHCb::RecVertex* pv, const LHCb::RecVertex* const original,
                  const LHCb::Particle* const particle, const std::string& refitter);
      virtual ~VertexEntry() {};

      LHCb::RecVertex* pv() const
      { 
         return m_pv;
      }

      const LHCb::RecVertex* original() const
      {
         return m_original;
      }

      const LHCb::Particle* particle() const
      {
         return m_particle;
      }

      size_t combinedHash() const
      {
         return m_hash;
      }

      size_t particleHash() const
      {
         return m_particleHash;
      }

      const std::string& refitter() const
      {
         return m_refitter;
      }

   private:

      const std::string m_refitter;
      LHCb::RecVertex* m_pv;
      const LHCb::RecVertex* const m_original;
      const LHCb::Particle* const m_particle;

      size_t m_hash;
      size_t m_particleHash;
   };
   
   // Index tags
   struct OfflinePV {};
   struct OriginalPV {};
   struct Refitter {};

   // Container to store original vertices
   typedef boost::unordered_map<int, const LHCb::RecVertex*> Originals;

   // Container to store refitted vertices
   typedef boost::multi_index_container<
      VertexEntry,
      boost::multi_index::indexed_by<
         boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<Refitter>,
            boost::multi_index::composite_key<
               VertexEntry,
               boost::multi_index::const_mem_fun< VertexEntry, size_t,
                                                  &VertexEntry::particleHash>,
               boost::multi_index::const_mem_fun< VertexEntry, const std::string&,
                                                  &VertexEntry::refitter>
               >
            >
         >
      > VertexContainer;

   typedef VertexContainer::index<Refitter>::type Vertices;
   typedef boost::iterator_range<Vertices::const_iterator> VertexRange;

   // properties
   std::string m_offlineLocation;
   bool m_refitPVs;

   // services
   IIncidentSvc* m_incidentSvc;
   IDataProviderSvc* m_evtSvc;

   // data 
   Gaudi::XYZVector m_offset;
   VertexContainer m_pvs;
   mutable Originals m_originals;

   std::vector<const LHCb::Track*> m_signalTracks;

   LHCb::RecVertex::Container* m_offlinePVs;
};
}
#endif // SWIMMINGSERVICE_H
