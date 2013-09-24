#ifndef PROPERTYCONFIG_H
#define PROPERTYCONFIG_H 1
#include <string>
#include <vector>
#include <iostream>
#include "boost/optional.hpp"
#include "boost/operators.hpp"
#include "GaudiKernel/System.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/IProperty.h"
#include "LHCbMath/MD5.h"

class PropertyConfig; 
std::ostream& operator<<(std::ostream& os, const PropertyConfig& x);
std::istream& operator>>(std::istream& is, PropertyConfig& x);

class PropertyConfig : public boost::equality_comparable<PropertyConfig> {
public:
    typedef Gaudi::Math::MD5 digest_type;

    typedef std::pair<std::string,std::string> Prop;
    typedef std::vector<Prop> Properties;

    PropertyConfig() {} ;

    PropertyConfig(const std::string& name, const IProperty& obj, const std::string& kind)
      : m_type(System::typeinfoName(typeid(obj)))
      , m_name(name)
      , m_kind(kind)
      , m_digest(digest_type::createInvalid())
    { initProperties( obj ) ; }

    PropertyConfig(const std::string& name, const std::string& type, const std::string& kind)
      : m_type(type)
      , m_name(name)
      , m_kind(kind)
      , m_digest(digest_type::createInvalid())
    {  }

    PropertyConfig(const std::string& name, const std::string& type, const std::string& kind, const Properties& props)
      : m_properties(props)
      , m_type(type)
      , m_name(name)
      , m_kind(kind)
      , m_digest(digest_type::createInvalid())
    { }

    PropertyConfig(const PropertyConfig& orig, const Properties& properties)
        : m_properties( properties )
        , m_type( orig.m_type )
        , m_name( orig.m_name )
        , m_kind( orig.m_kind )
        , m_digest( digest_type::createInvalid())
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

    std::ostream& print_json(std::ostream& os) const;
    std::ostream& print(std::ostream& os) const;
    std::istream& read(std::istream& is);

    digest_type digest() const { if (!m_digest.valid()) updateCache(); return m_digest; }

private:
    Properties   m_properties;
    std::string  m_type,m_name,m_kind;
    mutable digest_type  m_digest;
    void updateCache() const;
    void initProperties( const IProperty& obj );
};
#endif
