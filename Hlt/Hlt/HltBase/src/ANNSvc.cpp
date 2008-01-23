#include "HltBase/ANNSvc.h"
#include <vector>
#include <map>
#include <utility>
#include "GaudiKernel/Property.h"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"


using std::map;
using std::vector;
using std::string;
using boost::optional;
namespace bl = boost::lambda;

namespace ANNSvcUtilities { 

template <typename KEY, typename VALUE>
class bimap_t {
public:
    typedef KEY    key_type;
    typedef VALUE  value_type;
    typedef std::pair<KEY,VALUE>                 mapped_type;
    typedef GaudiUtils::VectorMap<KEY,VALUE>     k2v_type;
    typedef GaudiUtils::VectorMap<VALUE,KEY>     v2k_type;
    typedef SimpleProperty<std::map<KEY,VALUE> > property_type;
    typedef optional<value_type>                 result_value_type; 
    typedef optional<key_type>                   result_key_type; 

    bimap_t();

    property_type& property() { return m_property; } 

    result_value_type value(const key_type& key) const {
        typename k2v_type::const_iterator i = m_map.find(key);
        return i == m_map.end() ? result_value_type() 
                                : result_value_type(i->second) ;
    }

    result_key_type   key(const value_type& value) const {
        typename v2k_type::const_iterator i = m_invmap.find(value);
        return i == m_invmap.end() ? result_key_type() 
                                   : result_key_type(i->second) ;
    }

    const k2v_type& mapping() const { return m_map; }
      
private:
    bimap_t(const bimap_t& rhs);
    void updateHandler(Property& prop);

    property_type   m_property;
    k2v_type        m_map;
    v2k_type        m_invmap;
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
ANNSvc::ANNSvc( const string& name, ISvcLocator* pSvcLocator, 
                const vector<IANNSvc::major_key_type>& majors )
  : Service( name , pSvcLocator )
{
    for (vector<string>::const_iterator i  = majors.begin();
                                        i != majors.end(); ++i)  {
        maps_type::result_type r = m_maps.insert(*i, new bimap_type()); 
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


bool ANNSvc::hasMajor(const IANNSvc::major_key_type& major) const {
    return m_maps.find(major)!=m_maps.end();
}

optional<int>  ANNSvc::asInt(const IANNSvc::major_key_type& major, const string& minor) const {
    maps_type::const_iterator i = m_maps.find(major);
    return i==m_maps.end() ?  optional<int>() 
                           :  i->second->value(minor);
}

optional<string> ANNSvc::asString(const IANNSvc::major_key_type& major, int minor) const {
    maps_type::const_iterator i = m_maps.find(major);
    return i==m_maps.end() ? optional<string>() 
                           : i->second->key(minor);
}


std::vector<IANNSvc::minor_value_type> ANNSvc::items(const IANNSvc::major_key_type& major) const {
    std::vector<IANNSvc::minor_value_type>  r;
    maps_type::const_iterator i = m_maps.find(major);
    if (i!=m_maps.end())  {
        r.insert( r.end(), i->second->mapping().begin(), 
                           i->second->mapping().end() );
    }
    return r;
}

std::vector<IANNSvc::major_key_type> ANNSvc::majors() const {
    std::vector< major_key_type > r;
    std::transform( m_maps.begin(), 
                    m_maps.end(),
                    std::back_inserter(r), 
                    bl::bind(&maps_type::value_type::first,bl::_1) );
    return r;
}
