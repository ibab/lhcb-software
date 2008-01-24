#ifndef PROPERTYCONFIG_H
#define PROPERTYCONFIG_H 1
#include <string>
#include <vector>
#include "GaudiKernel/System.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/IProperty.h"
#include <iostream>

class PropertyConfig {
public:
    typedef std::pair<std::string,std::string> KeyValue;
    typedef std::vector< KeyValue > Dependencies;
    typedef std::vector< KeyValue > Properties;

    PropertyConfig() {} ;

    PropertyConfig(const std::string& name, const IProperty& obj, const Dependencies& depRefs) 
      : m_depRefs(depRefs)
      , m_type(System::typeinfoName(typeid(obj)))
      , m_name(name)
   { initProperties( obj ) ; }

    PropertyConfig(const std::string& name, const std::string& type, const Dependencies& depRefs) 
      : m_depRefs(depRefs)
      , m_type(type)
      , m_name(name)
   {  }

    bool operator==(const PropertyConfig& rhs) const { 
        return m_name == rhs.m_name 
            && m_type == rhs.m_type 
            && m_properties == rhs.m_properties
            && m_depRefs == rhs.m_depRefs;
    }

    bool operator!=(const PropertyConfig& rhs) const { return !operator==(rhs); }


    const std::string & name() const    { return m_name;}
    const std::string & type() const    { return m_type;}
    const Properties& properties() const { return m_properties;}
    const Dependencies& dependants() const { return m_depRefs; }
    std::ostream& print(std::ostream& os) const;
    std::istream& read(std::istream& is);
private:
    Properties   m_properties;
    Dependencies m_depRefs;
    std::string  m_type,m_name;
    void initProperties( const IProperty& obj );
};

std::ostream& operator<<(std::ostream& os, const PropertyConfig& x);
std::istream& operator>>(std::istream& is, PropertyConfig& x);
#endif
