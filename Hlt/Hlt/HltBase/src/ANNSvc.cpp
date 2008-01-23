#include "HltBase/ANNSvc.h"
#include <vector>
#include <map>
#include <utility>
#include "GaudiKernel/Property.h"


using std::map;
using std::vector;
using std::string;
using boost::optional;

namespace ANNSvcUtilities { 

template <typename KEY, typename VALUE>
class bimap_t {
    public:
      typedef KEY key_type;
      typedef VALUE value_type;
      typedef GaudiUtils::VectorMap< key_type, value_type > k2v_type;
      typedef GaudiUtils::VectorMap< value_type, key_type > v2k_type;
      typedef SimpleProperty<std::map<key_type,value_type> > property_type;
      typedef boost::optional<value_type> value_result_type; 
      typedef boost::optional<key_type>   key_result_type; 

      bimap_t();

      property_type& property() { return m_property; } 

      value_result_type value(const key_type& key) const {
             typename k2v_type::const_iterator i = m_map.find(key);
             return i == m_map.end() ? value_result_type() 
                                     : value_result_type(i->second) ;
      }

      key_result_type   key(const value_type& value) const {
            typename v2k_type::const_iterator i = m_invmap.find(value);
            return i == m_invmap.end() ? key_result_type() 
                                       : key_result_type(i->second) ;
      }

      const k2v_type& mapping() const { return m_map; }
      
    private:
      bimap_t(const bimap_t& rhs);

      property_type   m_property;
      k2v_type        m_map;
      v2k_type        m_invmap;
      void updateHandler(Property& prop);
};


template<typename KEY, typename VALUE>
bimap_t<KEY,VALUE>::bimap_t() {
    m_property.declareUpdateHandler( &bimap_t<KEY,VALUE>::updateHandler, this);
}

template <typename KEY, typename VALUE>
bimap_t<KEY,VALUE>::bimap_t(const bimap_t& rhs) {
    m_map = rhs.m_map;
    m_invmap = rhs.m_invmap;
    m_property = rhs.m_property;
    m_property.declareUpdateHandler(&bimap_t<KEY,VALUE>::updateHandler, this);
}

template <typename KEY, typename VALUE>
void
bimap_t<KEY,VALUE>::updateHandler(Property& /*prop*/) {
    m_invmap.clear();
    m_map.clear();
    for (typename std::map<KEY,VALUE>::const_iterator i  = m_property.value().begin();
                                                 i != m_property.value().end(); ++i ) {
        m_map.insert( i->first, i->second );
        m_invmap.insert( i->second, i->first );
    }
}

};




//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ANNSvc::ANNSvc( const string& name, ISvcLocator* pSvcLocator, const vector<string>& majors )
  : Service( name , pSvcLocator )
{
  for (vector<string>::const_iterator i = majors.begin(); i!=majors.end(); ++i)  {
    maps_type::result_type r = m_maps.insert(*i, new bimap_type()); 
    assert(r.second);
    declareProperty(r.first->first, 
                    const_cast<maps_type::mapped_type&>(r.first->second)->property());
  }
}
//=============================================================================
// Destructor
//=============================================================================
ANNSvc::~ANNSvc() {
} 


//=============================================================================
// Reinitialize
//=============================================================================
StatusCode ANNSvc::reinitialize() {
  StatusCode sc = reinitialize();
  if (!sc.isSuccess()) return sc;
  return setProperties();
}


//=============================================================================
// queryInterface
//=============================================================================
StatusCode ANNSvc::queryInterface(const InterfaceID& riid,
                                     void** ppvUnknown) {
  if ( IANNSvc::interfaceID().versionMatch(riid) )   {
    *ppvUnknown = (IANNSvc*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}


bool ANNSvc::hasMajor(const string& major) const {
    return m_maps.find(major)!=m_maps.end();
}

optional<int>  ANNSvc::asInt(const string& major, const string& minor) const {
    maps_type::const_iterator i = m_maps.find(major);
    return i==m_maps.end() ?  optional<int>() : 
                              i->second->value(minor);
}

optional<string>   ANNSvc::asString(const string& major, int minor) const {
    maps_type::const_iterator i = m_maps.find(major);
    return i==m_maps.end() ? optional<string>() 
                           : i->second->key(minor);
}


std::vector< std::pair<std::string,int> > 
ANNSvc::items(const std::string& major) const {
 std::vector< std::pair<std::string,int> >  r;
 maps_type::const_iterator i = m_maps.find(major);
 if (i!=m_maps.end())  {
    r.insert( r.end(), i->second->mapping().begin(), 
                       i->second->mapping().end() );
 }
 return r;
}

std::vector< std::string > 
ANNSvc::majors() const {
    std::vector< std::string > r;
    for (maps_type::const_iterator i = m_maps.begin(); i!=m_maps.end(); ++i) {
            r.push_back( i->first );
    }
    return r;
}
