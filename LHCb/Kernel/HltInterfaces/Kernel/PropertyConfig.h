#ifndef PROPERTYCONFIG_H
#define PROPERTYCONFIG_H 1
#include <string>
#include <vector>
#include <iostream>
#include "boost/operators.hpp"
#include "GaudiKernel/System.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/IProperty.h"
#include "LHCbMath/MD5.h"

class PropertyConfig final : public boost::equality_comparable<PropertyConfig> {
public:
    typedef Gaudi::Math::MD5 digest_type;

    typedef std::pair<std::string,std::string> Prop;
    typedef std::vector<Prop> Properties;

    PropertyConfig() = default;

    PropertyConfig(std::string name, const IProperty& obj, std::string kind)
      : m_type(System::typeinfoName(typeid(obj)))
      , m_name(std::move(name))
      , m_kind(std::move(kind))
    { initProperties( obj ) ; }

    PropertyConfig(std::string name, std::string type, std::string kind)
      : m_type(std::move(type))
      , m_name(std::move(name))
      , m_kind(std::move(kind))
    {  }

    PropertyConfig(std::string name, std::string type, std::string kind, Properties props)
      : m_properties(std::move(props))
      , m_type(std::move(type))
      , m_name(std::move(name))
      , m_kind(std::move(kind))
    { }

    PropertyConfig(const PropertyConfig& orig, Properties properties)
        : m_properties( std::move(properties) )
        , m_type( orig.m_type )
        , m_name( orig.m_name )
        , m_kind( orig.m_kind )
    { }

    bool operator==(const PropertyConfig& rhs) const { 
        return digest() != digest_type::createInvalid() ? digest() == rhs.digest()
                             : ( m_type == rhs.m_type 
                              && m_name == rhs.m_name 
                              && m_kind == rhs.m_kind 
                              && m_properties == rhs.m_properties );
    }

    const std::string& name() const    { return m_name;}
    std::string  fullyQualifiedName() const    { return type() + "/" + name();}
    const std::string& type() const    { return m_type;}
    const std::string& kind() const    { return m_kind;}
    const Properties& properties() const { return m_properties;}

    PropertyConfig copyAndModify(const std::string& key, const std::string& value ) const;
    PropertyConfig copyAndModify(const std::string& keyAndValue) const;

    template <typename T>
    PropertyConfig copyAndModify(const std::pair<T,T>& keyAndValue) const { return copyAndModify(keyAndValue.first,keyAndValue.second);}
    
    template <typename T>
    PropertyConfig copyAndModify(T begin, T end) const { PropertyConfig ret(*this); while (begin!=end) ret=ret.copyAndModify(*begin++); return ret;}

    std::ostream& print(std::ostream& os) const;
    std::istream& read(std::istream& is);

    digest_type digest() const { if (!m_digest.valid()) updateCache(); return m_digest; }

private:
    friend class ConfigArchiveAccessSvc; // provide access to 'str' to allow backwards compatible writes...
    std::string str() const;

    Properties   m_properties;
    std::string  m_type,m_name,m_kind;
    mutable digest_type  m_digest = digest_type::createInvalid();
    void updateCache() const;
    void initProperties( const IProperty& obj );
};

inline std::ostream& operator<<(std::ostream& os, const PropertyConfig& x) { return x.print(os);}
inline std::istream& operator>>(std::istream& is, PropertyConfig& x) { return x.read(is); }
#endif
