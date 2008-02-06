#ifndef PROPERTYCONFIG_H
#define PROPERTYCONFIG_H 1
#include <string>
#include <vector>
#include "GaudiKernel/System.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/IProperty.h"
#include "boost/cstdint.hpp"
#include <iostream>

class PropertyConfig {
public:
    class digest_type {
    public:
       typedef boost::uint8_t value_type[16];
       std::string str() const { return hexit(m_value,m_value+16); }
       bool invalid() const { bool iszero(true); for (size_t i=0;i<16 && iszero ;++i) {iszero = (m_value[i]==0);}; return iszero;}
       bool operator< (const digest_type& rhs) const { return memcmp(m_value,rhs.m_value,16)<0;}
       bool operator> (const digest_type& rhs) const { return memcmp(m_value,rhs.m_value,16)>0;}
       bool operator==(const digest_type& rhs) const { return memcmp(m_value,rhs.m_value,16)==0;}
       bool operator<=(const digest_type& rhs) const { return !operator>(rhs);}
       bool operator>=(const digest_type& rhs) const { return !operator<(rhs);}
       bool operator!=(const digest_type& rhs) const { return !operator==(rhs);}
    private:
       friend class PropertyConfig;
       explicit digest_type(const std::string& val) { unhexit(val,m_value); }
       explicit digest_type(const value_type val) { memcpy(m_value,val,16); }

       static std::string hexit(const boost::uint8_t *begin, const boost::uint8_t *end);
       static void unhexit(const std::string& s,value_type& t);
       value_type m_value;
    };
    typedef std::pair<std::string,digest_type> Dep;
    typedef std::vector<Dep> Dependencies;
    typedef std::pair<std::string,std::string> Prop;
    typedef std::vector<Prop> Properties;


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


  // note: we add digest functionality explicitly here, because if we were to
  // use a tool instead, we would get into a chicken and egg problem: the 
  // configuration svc would then have to use the toolsvc _before_
  // initialize is called, which should not be relied on to work properly...
  // as a side effect, this way we protect the digest_type from inappropriate 
  // c'tor calls, since 'we' are the only ones creating it...
    digest_type digest() const;

    static digest_type createDigest(const std::string& s) {
        return digest_type(s);
    }
    static digest_type createNullDigest() {
       return digest_type(std::string());
    }
private:
    Properties   m_properties;
    Dependencies m_depRefs;
    std::string  m_type,m_name;
    void initProperties( const IProperty& obj );
};

std::ostream& operator<<(std::ostream& os, const PropertyConfig& x);
std::istream& operator>>(std::istream& is, PropertyConfig& x);

std::ostream& operator<<(std::ostream& os, const PropertyConfig::digest_type& x) ;
#endif
