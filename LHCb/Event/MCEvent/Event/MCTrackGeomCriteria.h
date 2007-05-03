#ifndef MCEVENT_MCTRACKGEOMCRITERIA_H
#define MCEVENT_MCTRACKGEOMCRITERIA_H 1
 
// Event
#include "Event/MCTrackInfo.h" 

// boost
#include <boost/function.hpp>

// stl
#include <string>
#include <map>
#include <vector>

namespace LHCb
{
  class MCParticle;
}

namespace LHCb{
  namespace MC{

    class MCTrackGeomCriteria {

   public:

    typedef boost::function<bool(MCTrackInfo&, 
                                 const LHCb::MCParticle*)> Criteria;

    /** Constructor from vector of Criteria */
    MCTrackGeomCriteria(std::vector<Criteria>& criteria);   

    /** Constructor from vector of strings */ 
    MCTrackGeomCriteria(std::vector<std::string>& criteria);   

    /** destructor */
    ~MCTrackGeomCriteria();
  
    /** fufulls Criteria or not */
    bool accepted(MCTrackInfo& info, const LHCb::MCParticle* particle) const;

    class FunctionMap{

      public:

       /// map to enum
       Criteria toType(std::string& aName) const;

       /// map to type
       //std::string toString(const Criteria& aType) const;

       /// destructer
       ~FunctionMap() {}
 
       friend class MCTrackGeomCriteria;
       
       private:

       /// constructer
       FunctionMap();
       FunctionMap(const FunctionMap& rhs){
        m_mapping = rhs.m_mapping;
       }

       typedef std::map<std::string, const Criteria> funMap; 
       mutable funMap m_mapping;
    };

    FunctionMap& theMap(){
    static FunctionMap sMap;
    return sMap;
    }

    private:

    std::vector<Criteria> m_criteria;

   };
  }
}

inline LHCb::MC::MCTrackGeomCriteria::MCTrackGeomCriteria(std::vector<MCTrackGeomCriteria::Criteria>& criteria):
m_criteria(criteria){
  // contructor
}

inline LHCb::MC::MCTrackGeomCriteria::~MCTrackGeomCriteria()
{
  // destructor
}

inline LHCb::MC::MCTrackGeomCriteria::Criteria 
       LHCb::MC::MCTrackGeomCriteria::FunctionMap::toType(std::string& aName) const{

  //  map to enum
  funMap::iterator iter = m_mapping.find(aName);

  return (iter != m_mapping.end() ? iter->second : Criteria());
}

/*
inline std::string 
MCTrackGeomCriteria::FunctionMap::toString(const MCTrackGeomCriteria::Criteria& fun) const{
  // map to string
  funMap::iterator iter = m_mapping.begin();
  while (iter != m_mapping.end() && iter->second != fun ) {
    ++iter;
  } // iter 

  return (iter != m_mapping.end() ? iter->first : "Unknown");
}
*/

#endif
