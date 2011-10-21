#ifndef HltEvent_SwimmingReport_H
#define HltEvent_SwimmingReport_H 1

// Include files
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/VectorMap.h"
#include "GaudiKernel/NamedRange.h"
#include "GaudiKernel/SharedObjectsContainer.h"
#include "GaudiKernel/KeyedObject.h"
#include "GaudiKernel/KeyedContainer.h"

// Event
#include <Event/Particle.h>

#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "GaudiKernel/boost_allocator.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

// STD 
#include <ostream>
#include <string>

// STL
#include <utility>
#include <vector>
#include <map>

// local
#include "Event/TurningPoint.h"

namespace LHCb 
{
   static const CLID CLID_SwimmingReport = 900;

  // Namespace for locations in TDS
  namespace SwimmingReportsLocation {
    static const std::string Default = "Swimming/Reports";
  }

  /**Class ID definition
   * 
   *  @class SwimmingReport SwimmingReport.h
   * 
   * Class to store the results of the swimming.
   * 
   * @author Roel Aaij
   * created Fri Sep 16 10:30:49 2011
   */

class SwimmingReport: public KeyedObject<int>
{
   
public:

   /// Definition of Keyed Container for SwimmingReport
   typedef KeyedContainer<SwimmingReport, Containers::HashMap> Container;
   typedef std::map<std::string, std::vector<LHCb::TurningPoint> > TurningPoints;

   /// Default constructor
   SwimmingReport()
      : KeyedObject<int>()
   {

   }

   /// Constructor from a particle
   SwimmingReport(const LHCb::Particle* particle)
      : KeyedObject<int>(particle->key())
   {

   }

   /// Constructor from key
   SwimmingReport(const int key)
      : KeyedObject<int>(key)
   {

   }

   /// Copy constructor
   SwimmingReport(const SwimmingReport& other)
      : KeyedObject<int>(),
        m_turningPoints(other.m_turningPoints)
   {
   
   }
   
   /// Default Destructor
   virtual ~SwimmingReport()
   {
   
   }
   
   // Retrieve pointer to class definition structure
   virtual const CLID& clID() const;
   static const CLID& classID();
   
   /// Fill the ASCII output stream
   virtual std::ostream& fillStream(std::ostream& s) const;
   
   /// Clone report
   SwimmingReport* clone() const;
   
   /// Assignment operator, note that the original vertex and protoSwimmingReport are kept
   SwimmingReport& operator=(const SwimmingReport& rhs);

   /// Retrieve the stage of this report (Trigger, Stripping, Offline, etc..)
   std::vector<std::string> stages() const
   {
      std::vector<std::string> s;
      for(TurningPoints::const_iterator it = m_turningPoints.begin(),
             end = m_turningPoints.end(); it != end; ++it) {
         s.push_back(it->first);
      }
      return s;
   }

   /// Check if a stage is present
   bool hasStage(const std::string& stage) const
   {
      return m_turningPoints.count(stage) != 0;
   }
   
   /// Get the turningpoints for a stage
   const std::vector<LHCb::TurningPoint>& turningPoints(const std::string& stage) const
   {
      TurningPoints::const_iterator it = m_turningPoints.find(stage);
      if (it != m_turningPoints.end()) {
         return it->second;
      } else {
         std::string msg = "Turning points for stage ";
         msg += stage;
         msg += " do not exist.";
         throw GaudiException(msg, "SwimmingReport", StatusCode::FAILURE);
      }
   }

   void clearTurningPoints()
   {
      m_turningPoints.clear();
   }
   
   void addTurningPoint(const std::string& stage, const LHCb::TurningPoint& tp)
   {
      TurningPoints::iterator it = m_turningPoints.find(stage);
      if ( it == m_turningPoints.end() ) {
         std::vector<LHCb::TurningPoint> tps(1, tp);
         m_turningPoints.insert(make_pair(stage, tps));
      } else {
         it->second.push_back(tp);
      }
   }

#ifndef GOD_NOALLOC
   /// operator new
   static void* operator new ( size_t size )
   {
      return ( sizeof(SwimmingReport) == size ? 
               boost::singleton_pool<SwimmingReport, sizeof(SwimmingReport)>::malloc() :
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
      boost::singleton_pool<SwimmingReport, sizeof(SwimmingReport)>::is_from(p) ?
         boost::singleton_pool<SwimmingReport, sizeof(SwimmingReport)>::free(p) :
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

   TurningPoints m_turningPoints;
   
}; // class SwimmingReport

inline std::ostream& operator<< (std::ostream& str, const SwimmingReport& obj)
{
   return obj.fillStream(str);
}

} // namespace LHCb;

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

inline const CLID& LHCb::SwimmingReport::clID() const
{
   return LHCb::SwimmingReport::classID();
}

inline const CLID& LHCb::SwimmingReport::classID()
{
   return CLID_SwimmingReport;
}

inline std::ostream& LHCb::SwimmingReport::fillStream(std::ostream& s) const
{
   s << "TODO" << std::endl;
   return s;
}


inline LHCb::SwimmingReport* LHCb::SwimmingReport::clone() const 
{
   return new SwimmingReport(*this);
}

inline LHCb::SwimmingReport& LHCb::SwimmingReport::operator=(const SwimmingReport& rhs) 
{
   if ( this != &rhs ) {
      m_turningPoints = rhs.m_turningPoints;
   }
   return *this;
}



#endif ///HltEvent_SwimmingReport_H
