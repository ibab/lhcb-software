#include "Kernel/ANNSvc.h"
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
    typedef KEY                                  key_type;
    typedef VALUE                                value_type;
    typedef std::pair<KEY,VALUE>                 mapped_type;

    typedef GaudiUtils::VectorMap<KEY,VALUE>     k2v_type;
    typedef GaudiUtils::VectorMap<VALUE,KEY>     v2k_type;
    typedef SimpleProperty<std::map<KEY,VALUE> > property_type;
    typedef optional<IANNSvc::minor_value_type>  result_type; 

    bimap_t(const ANNSvc* parent);

    property_type& property() { return m_property; } 

    result_type value(const key_type& key) const {
        typename k2v_type::const_iterator i = m_map.find(key);
        return i == m_map.end() ? result_type() 
                                : result_type(*i) ;
    }

    result_type   key(const value_type& value) const {
        typename v2k_type::const_iterator i = m_invmap.find(value);
        return i == m_invmap.end() ? result_type() 
                                   : result_type(std::make_pair(i->second,i->first)) ;
    }

    const k2v_type& mapping() const { return m_map; }

    void insert(const mapped_type& value);
      
private:
    bimap_t(const bimap_t& rhs);
    void updateHandler(Property& prop);

    property_type   m_property;
    k2v_type        m_map;
    v2k_type        m_invmap;
    const ANNSvc*   m_parent;
};


template<typename KEY, typename VALUE>
bimap_t<KEY,VALUE>::bimap_t(const ANNSvc* parent) : m_parent(parent) {
    m_property.declareUpdateHandler( &bimap_t<KEY,VALUE>::updateHandler, this);
}

template <typename KEY, typename VALUE>
bimap_t<KEY,VALUE>::bimap_t(const bimap_t& rhs) 
    : m_map(       rhs.m_map      )
    , m_invmap(    rhs.m_invmap   )
    , m_property(  rhs.m_property )
    , m_parent(    rhs.m_parent   )
{
    m_property.declareUpdateHandler(&bimap_t<KEY,VALUE>::updateHandler, this);
}

template <typename KEY, typename VALUE>
void
bimap_t<KEY,VALUE>::insert(const mapped_type& value) {
        // verify each entry to be unique
        if (m_map.find( value.first ) != m_map.end() ||
            m_invmap.find(value.second) != m_invmap.end() ) {
            if (m_parent!=0) {
                m_parent->log() << MSG::ERROR 
                                << " Entry \"" << value.first << "\" -> \""<< value.second<<"\" clashes with existing entry" << endmsg;
            }
            throw GaudiException( "non-unique entry in ANNSvc instance","",StatusCode::FAILURE);
        }
        m_map.   insert( value.first,  value.second );
        m_invmap.insert( value.second, value.first  );
}

template <typename KEY, typename VALUE>
void
bimap_t<KEY,VALUE>::updateHandler(Property& /*prop*/) {
    // grab the one we made ourselves and preserve them unless overruled???
    m_map.   clear();
    m_invmap.clear();
    for (typename std::map<KEY,VALUE>::const_iterator i  = m_property.value().begin();
                                                      i != m_property.value().end(); ++i ) {
        insert(*i);
    }
}


};




//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ANNSvc::ANNSvc( const string& name, ISvcLocator* pSvcLocator, 
                const vector<IANNSvc::major_key_type>& majors )
  : Service( name , pSvcLocator )
  , m_log(msgSvc(),name)
{
    for (vector<string>::const_iterator i  = majors.begin();
                                        i != majors.end(); ++i)  {
        maps_type::result_type r = m_maps.insert(*i, new bimap_type(this)); 
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
    if ( IANSvc::interfaceID().versionMatch(riid) )   {
        *ppvUnknown = (IANSvc*)this;
        addRef();
        return SUCCESS;
    }
    return Service::queryInterface(riid,ppvUnknown);
}

//=============================================================================
// finalize
//=============================================================================
StatusCode 
ANNSvc::finalize() {
    for( maps_type::const_iterator i = m_maps.begin(); i!=m_maps.end();++i) {
        const bimap_type::k2v_type&  map = i->second->mapping();
        const std::map<string,int>& prop = i->second->property().value();
        for (bimap_type::k2v_type::const_iterator j=map.begin();j!=map.end();++j) {
            if ( prop.find(j->first) != prop.end() ) continue;
            log() << MSG::INFO << " autogenerated: " << i->first << " / " << j->first << " : " << j->second << endmsg;
        }
    }
    return StatusCode::SUCCESS;
}


//=============================================================================
// IANNSvc interface implementation
//=============================================================================
bool 
ANNSvc::hasMajor(const IANNSvc::major_key_type& major) const {
    return m_maps.find(major)!=m_maps.end();
}

optional<IANNSvc::minor_value_type> 
ANNSvc::handleUndefined(const IANNSvc::major_key_type&, int ) const {
    return optional<IANNSvc::minor_value_type>();
}

optional<IANNSvc::minor_value_type> 
ANNSvc::handleUndefined(const IANNSvc::major_key_type&, const std::string& ) const {
    return optional<IANNSvc::minor_value_type>();
}

optional<IANNSvc::minor_value_type>  
ANNSvc::value(const IANNSvc::major_key_type& major, const string& minor) const {
    maps_type::const_iterator i = m_maps.find(major);
    if (i==m_maps.end()) return optional<IANNSvc::minor_value_type>();
    optional<IANNSvc::minor_value_type> x = i->second->value(minor);
    if (!x) {   
        x = handleUndefined(major,minor);
        if (x)  i->second->insert(*x);
    }
    return x;
}

optional<IANNSvc::minor_value_type> 
ANNSvc::value(const IANNSvc::major_key_type& major, int minor) const {
    maps_type::const_iterator i = m_maps.find(major);
    if (i==m_maps.end()) return optional<IANNSvc::minor_value_type>();
    optional<IANNSvc::minor_value_type> x = i->second->key(minor);
    if (!x) {   
        x = handleUndefined(major,minor);
        if (x)  i->second->insert(*x);
    }
    return x;
}

std::vector<IANNSvc::minor_key_type> 
ANNSvc::keys(const IANNSvc::major_key_type& major) const {
    std::vector<IANNSvc::minor_value_type> i = items(major);
    std::vector<IANNSvc::minor_key_type>  k; k.reserve(i.size());
    std::transform(i.begin(),
                   i.end(),
                   std::back_inserter(k),
                   bl::bind(&IANNSvc::minor_value_type::first,bl::_1));
    return k;
}

std::vector<IANNSvc::minor_value_type> 
ANNSvc::items(const IANNSvc::major_key_type& major) const {
    std::vector<IANNSvc::minor_value_type>  r;
    maps_type::const_iterator i = m_maps.find(major);
    if (i!=m_maps.end())  {
        r.reserve( i->second->mapping().size() );
        r.insert( r.end(), i->second->mapping().begin(), 
                           i->second->mapping().end() );
    }
    return r;
}

std::vector<IANNSvc::major_key_type> 
ANNSvc::majors() const {
    std::vector< major_key_type > r; r.reserve(m_maps.size());
    std::transform( m_maps.begin(), 
                    m_maps.end(),
                    std::back_inserter(r), 
                    bl::ret<const maps_type::value_type::first_type&>(bl::bind(&maps_type::value_type::first,bl::_1)) );
    return r;
}
