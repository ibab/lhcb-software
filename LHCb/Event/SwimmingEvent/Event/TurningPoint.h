// $Id: $
#ifndef EVENT_TURNINGPOINT_H 
#define EVENT_TURNINGPOINT_H 1

// Include files
#include <map>
#include <string>

// Gaudi
#include <GaudiKernel/GaudiException.h>
#include <GaudiKernel/DataObject.h>

// LHCb
#include <Event/Particle.h>

#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
#include "GaudiKernel/boost_allocator.h"

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
                const std::map<std::string, std::map<int, bool> >& info)
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
                const std::map<std::string, std::map<int, bool> >& info,
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
      for (std::map<std::string, double>::const_iterator it = m_extra.begin(),
              end = m_extra.end(); it != end; ++it) {
         names.push_back(it->first);
      }
      return names;
   }

   double extra(const std::string& name) const
   {
      std::map<std::string, double>::const_iterator it = m_extra.find(name);
      if (it != m_extra.end()) {
         return it->second;
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
      for (std::map<std::string,bool>::const_iterator it = m_decisions.begin(),
              end = m_decisions.end(); it != end; ++it) {
         d.push_back(it->first);
      }
      return d;
   }

   bool decision(const std::string& name) const
   {
      std::map<std::string, bool>::const_iterator it = m_decisions.find(name);
      if (it != m_decisions.end()) {
         return it->second;
      } else {
         std::string msg = "Decision with name ";
         msg += name;
         msg += " does not exist.";
         throw GaudiException(msg, "TurningPoint", StatusCode::FAILURE);
      }

   }

   std::vector<std::string> infoNames() const
   {
      std::vector<std::string> names;
      for (std::map<std::string, std::map<int, bool> >::const_iterator it = m_info.begin(),
              end = m_info.end(); it != end; ++it) {
         names.push_back(it->first);
      }
      return names;
   }

   const std::map<int, bool>& info(const std::string& name) const
   {
      std::map<std::string, std::map<int, bool> >::const_iterator it = m_info.find(name);
      if (it != m_info.end()) {
         return it->second;
      } else {
         std::string msg = "DaughterInfo with name ";
         msg += name;
         msg += " does not exist.";
         throw GaudiException(msg, "TurningPoint", StatusCode::FAILURE);
      }
   }

   bool participated(const std::string& name, const LHCb::Particle& daughter) const
   {
      const std::map<int, bool>& i = info(name);
      std::map<int, bool>::const_iterator it = i.find(daughter.key());
      if (it != i.end()) {
         return it->second;
      } else {
         std::string msg = "No entry present for daughter with key ";
         msg += daughter.key();
         msg += " in info with with name ";
         msg += name;
         msg += ".";
         throw GaudiException(msg, "TurningPoint", StatusCode::FAILURE);
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
   std::map<std::string, std::map<int, bool> > m_info;
   std::map<std::string, double> m_extra;

};
} // namespace LHCb
#endif // EVENT_TURNINGPOINT_H
