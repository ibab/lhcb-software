// $Id: $
#ifndef EVENT_TURNINGPOINT_H 
#define EVENT_TURNINGPOINT_H 1

// Include files
#include <map>
#include <string>
#include <sstream>

// Gaudi
#include <GaudiKernel/GaudiException.h>
#include <GaudiKernel/DataObject.h>
#include <GaudiKernel/Hash.h>

// LHCb Event
#include <Event/ProtoParticle.h>
#include <Event/Particle.h>
#include <Event/Track.h>

#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
#include "GaudiKernel/boost_allocator.h"

namespace {
   GaudiUtils::Hash<std::vector<unsigned int> > _hash;

   void decayTree(const LHCb::Particle& p, LHCb::Particle::ConstVector& particles) {
      particles.push_back(&p);
      if (!p.isBasicParticle()) {
         typedef SmartRefVector<LHCb::Particle> Daughters;
         const Daughters& daughters = p.daughters();
         for (Daughters::const_iterator it = daughters.begin(), end = daughters.end();
              it != end; ++it) {
            decayTree(**it, particles);
         }
      }
   }
}

namespace GaudiUtils {
   
/// Particle Hash function
template <> struct Hash<const LHCb::Particle&>
{ 
   size_t operator() (const LHCb::Particle& p) const
   {
      LHCb::Particle::ConstVector particles;
      decayTree(p, particles);
      std::vector<unsigned int> ids;
      ids.reserve(80);
      
      for (LHCb::Particle::ConstVector::const_iterator it = particles.begin(), end = particles.end();
           it != end; ++it) {
         const LHCb::Particle* particle = *it;

         // Add the pid so that both different decay trees and Particles from the same ProtoParticle
         // have different hashes.
         ids.push_back(particle->particleID().pid());
         if (!particle->isBasicParticle()) continue;

         const LHCb::ProtoParticle* pp = particle->proto();
         if (!pp) {
            throw GaudiException("Need access to proto particle to compute hash",
                                 "Hash<const LHCb::Particle&>", StatusCode::FAILURE);
         }
         const LHCb::Track* track = pp->track();
         if (!track) {
            throw GaudiException("Need access to track to compute hash",
                                 "Hash<const LHCb::Particle&>", StatusCode::FAILURE);
         }
         const std::vector<LHCb::LHCbID>& lhcbIDs = track->lhcbIDs();
         // Add lhcbIDs
         std::transform(lhcbIDs.begin(), lhcbIDs.end(), std::back_inserter(ids),
                        std::mem_fun_ref(&LHCb::LHCbID::lhcbID));
      }
      return _hash(ids);
   }
};

}

namespace {
   GaudiUtils::Hash<const LHCb::Particle&> _hashParticle;
}

namespace Swimming {
   inline size_t hashParticle(const LHCb::Particle& p) {
      return _hashParticle(p);
   }
}

namespace LHCb {

   static const CLID CLID_TurningPoint = 901;

/** @class TurningPoint
 *  
 *
 *  @author 
 *  @date   2011-10-06
 */
class TurningPoint : public DataObject {
public: 

   /// Standard constructor
   TurningPoint()
      : m_raw(0.),
        m_tau(0.),
        m_ip(0.),
        m_dec(0.)
   {

   }

   TurningPoint(const double raw, const double tau,
                const double ip, const double dec)
      : m_raw(raw),
        m_tau(tau),
        m_ip(ip),
        m_dec(dec)
   {

   }

   TurningPoint(const double raw, const double tau,
                const double ip, const double dec,
                const std::map<std::string, bool>& decisions)
      : m_raw(raw),
        m_tau(tau),
        m_ip(ip),
        m_dec(dec),
        m_decisions(decisions)
   {
   
   }

   TurningPoint(const double raw, const double tau,
                const double ip, const double dec,
                const std::map<std::string, bool>& decisions,
                const std::map<std::string, std::map<size_t, bool> >& info)
      : m_raw(raw),
        m_tau(tau),
        m_ip(ip),
        m_dec(dec),
        m_decisions(decisions),
        m_info(info)
   {
   
   }

   TurningPoint(const double raw, const double tau,
                const double ip, const double dec,
                const std::map<std::string, bool>& decisions,
                const std::map<std::string, double>& extra)
      : m_raw(raw),
        m_tau(tau),
        m_ip(ip),
        m_dec(dec),
        m_decisions(decisions),
        m_extra(extra)
   {

   }

   TurningPoint(const double raw, const double tau,
                const double ip, const double dec,
                const std::map<std::string, bool>& decisions,
                const std::map<std::string, std::map<size_t, bool> >& info,
                const std::map<std::string, double>& extra)
      : m_raw(raw),
        m_tau(tau),
        m_ip(ip),
        m_dec(dec),
        m_decisions(decisions),
        m_info(info),
        m_extra(extra)
   {

   }

   TurningPoint(const TurningPoint& other)
      : DataObject(),
        m_raw(other.m_raw),
        m_tau(other.m_tau),
        m_ip(other.m_ip),
        m_dec(other.m_dec),
        m_decisions(other.m_decisions),
        m_info(other.m_info),
        m_extra(other.m_extra)
   {

   }

   virtual ~TurningPoint( )
   {

   }

   // ClassID
   virtual const CLID& clID() const
   {
      return classID();
   }
   static const CLID& classID()
   {
      return CLID_TurningPoint;
   }

   double raw() const
   {
      return m_raw;
   }

   double tau() const
   {
      return m_tau;
   }

   double ip() const
   {
      return m_ip;
   }

   double dec() const
   {
      return m_dec;
   }

   std::vector<std::string> extraNames() const
   {
      std::vector<std::string> names;
#ifndef WIN32
      for (std::map<std::string, double>::const_iterator it = m_extra.begin(),
              end = m_extra.end(); it != end; ++it) {
         names.push_back(it->first);
      }
#endif
      return names;
   }

   double extra(const std::string& name) const
   {
      std::map<std::string, double>::const_iterator it = m_extra.find(name);
      if (it != m_extra.end()) {
#ifndef WIN32
         return it->second;
#else
         return 0.;
#endif
      } else {
         std::string msg = "Extra turningpoint with name ";
         msg += name;
         msg += " does not exist.";
         throw GaudiException(msg, "TurningPoint", StatusCode::FAILURE);
      }
   }

   std::vector<std::string> decisions() const
   {
      std::vector<std::string> d;
#ifndef WIN32
      typedef std::map<std::string,bool> map_t;
      for (map_t::const_iterator it = m_decisions.begin(), end = m_decisions.end();
           it != end; ++it) {
         d.push_back(it->first);
      }
#endif
      return d;
   }

   bool decision(const std::string& name) const
   {
      std::map<std::string, bool>::const_iterator it = m_decisions.find(name);
      if (it != m_decisions.end()) {
#ifndef WIN32
         return it->second;
#else
         return true;
#endif
      } else {
         std::stringstream msg;
         msg << "Decision with name " << name << " does not exist.";
         throw GaudiException(msg.str(), "TurningPoint", StatusCode::FAILURE);
      }

   }

   std::vector<std::string> infoNames() const
   {
      std::vector<std::string> names;
#ifndef WIN32
      for (std::map<std::string, std::map<size_t, bool> >::const_iterator it = m_info.begin(),
              end = m_info.end(); it != end; ++it) {
         names.push_back(it->first);
      }
#endif
      return names;
   }

   const std::map<size_t, bool>& info(const std::string& name) const
   {
      std::map<std::string, std::map<size_t, bool> >::const_iterator it = m_info.find(name);
      if (it != m_info.end()) {
         return it->second;
      } else {
         std::stringstream msg;
         msg << "DaughterInfo with name " << name << " does not exist.";
         throw GaudiException(msg.str(), "TurningPoint", StatusCode::FAILURE);
      }
   }

   bool participated(const std::string& name, const LHCb::Particle& daughter) const
   {
      size_t h = Swimming::hashParticle(daughter);
      typedef std::map<size_t, bool> map_t;
      const map_t& i = info(name);
      map_t::const_iterator it = i.find(h);
      if (it != i.end()) {
#ifndef WIN32
         return it->second;
#else
         return true;
#endif
      } else {
         std::stringstream msg;
         msg << "No entry present for daughter with hash " << h 
             << " in info with with name " << name << ".";
         throw GaudiException(msg.str(), "TurningPoint", StatusCode::FAILURE);
      }
   }

#ifndef GOD_NOALLOC
   /// operator new
   static void* operator new ( size_t size )
   {
      return ( sizeof(TurningPoint) == size ? 
               boost::singleton_pool<TurningPoint, sizeof(TurningPoint)>::malloc() :
               ::operator new(size) );
   }

   /// placement operator new
   /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
   /// it is not needed in libstdc++ >= 3.4
   static void* operator new ( size_t size, void* pObj )
   {
      return ::operator new (size,pObj);
   }

   /// operator delete
   static void operator delete ( void* p )
   {
      boost::singleton_pool<TurningPoint, sizeof(TurningPoint)>::is_from(p) ?
         boost::singleton_pool<TurningPoint, sizeof(TurningPoint)>::free(p) :
         ::operator delete(p);
   }

   /// placement operator delete
   /// not sure if really needed, but it does not harm
   static void operator delete ( void* p, void* pObj )
   {
      ::operator delete (p, pObj);
   }
#endif

private:

   double m_raw;
   double m_tau;
   double m_ip;
   double m_dec;

   std::map<std::string, bool> m_decisions;
   std::map<std::string, std::map<size_t, bool> > m_info;
   std::map<std::string, double> m_extra;

};
} // namespace LHCb
#endif // EVENT_TURNINGPOINT_H
