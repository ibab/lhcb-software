#ifndef PROPERTYCONFIG_H
#define PROPERTYCONFIG_H 1
#include <string>
#include <vector>
#include <iostream>
#include "boost/optional.hpp"
#include "GaudiKernel/System.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/IProperty.h"
#include "LHCbMath/MD5.h"

class PropertyConfig {
public:
    typedef Gaudi::Math::MD5 digest_type;

    typedef std::pair<std::string,std::string> Prop;
    typedef std::vector<Prop> Properties;


    PropertyConfig() {} ;

    PropertyConfig(const std::string& name, const IProperty& obj, const std::string& kind)
      : m_type(System::typeinfoName(typeid(obj)))
      , m_name(name)
      , m_kind(kind)
   { initProperties( obj ) ; }

    PropertyConfig(const std::string& name, const std::string& type, const std::string& kind)
      : m_type(type)
      , m_name(name)
      , m_kind(kind)
   {  }

    PropertyConfig(const std::string& name, const std::string& type, const std::string& kind, const Properties& props)
      : m_properties(props)
      , m_type(type)
      , m_name(name)
      , m_kind(kind)
   {  }

    bool operator==(const PropertyConfig& rhs) const { 
        return m_type == rhs.m_type 
            && m_name == rhs.m_name 
            && m_kind == rhs.m_kind 
            && m_properties == rhs.m_properties;
    }

    bool operator!=(const PropertyConfig& rhs) const { return !operator==(rhs); }


    const std::string & name() const    { return m_name;}
    const std::string & type() const    { return m_type;}
    const std::string & kind() const    { return m_kind;}
    const Properties& properties() const { return m_properties;}

    PropertyConfig* clone( boost::optional<const Properties&> properties = boost::optional<const Properties&>() )  const {
        return new PropertyConfig(m_name,m_type,m_kind, 
                                  properties ? properties.get() : m_properties);
    }

    std::ostream& print(std::ostream& os) const;
    std::istream& read(std::istream& is);

    digest_type digest() const { return digest_type::compute(*this); }

private:
    Properties   m_properties;
    std::string  m_type,m_name,m_kind;
    void initProperties( const IProperty& obj );
};

std::ostream& operator<<(std::ostream& os, const PropertyConfig& x);
std::istream& operator>>(std::istream& is, PropertyConfig& x);

#endif
