#include "HltBase/PropertyConfig.h"
#include "boost/regex.hpp"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IProperty.h"
#include "md5.h"

using namespace std;

namespace {
    boost::uint8_t unhex(char C) {
            char c=tolower(C);
            boost::uint8_t x = ( c >= '0' && c <= '9' ? c-'0' :
                               ( c >= 'a' && c <='f'  ? 10+(c-'a') 
                                                      : 255 ) );
            if (x==255) {  /* whoah  */ }
            return x;
    };
};

std::string 
PropertyConfig::digest_type::hexit(const uint8_t *begin,const uint8_t *end) {
    string s;
    static const char *digits="0123456789abcdef";
    while(begin!=end) {
        s+=digits[(*begin>>4)&0x0F];
        s+=digits[(*begin++ )&0x0F];
    }
    return s;
}

void 
PropertyConfig::digest_type::unhexit(const std::string& s,value_type& t) {
    assert(s.size()==32 || s.empty());
    if (s.empty()) {
      for (int i=0;i<16;++i) t[i]=0;
    } else {
      for (int i=0;i<16;++i) t[i]=(unhex(s[2*i])<<4|unhex(s[2*i+1]));
    }
};

void PropertyConfig::initProperties( const IProperty& obj) {
    typedef vector<Property*> PropertyList;
    PropertyList items = obj.getProperties();
    for (PropertyList::const_iterator i = items.begin(); i!=items.end();++i) {
        m_properties.push_back(make_pair((*i)->name(),(*i)->toString()));
    }
}

istream& PropertyConfig::read(istream& is) {
    bool parsing_properties = false;
    bool parsing_deps = false;
    boost::regex name("^Name: (.*)$"),
                 type("^Type: (.*)$"),
                 propstart("^Properties: \\\[$"),
                 propend("^\\\]$"),
                 property("^ ?'([^']+)':(.*)$"),
                 depstart("^Dependencies: \\\[$"),
                 depend("^\\\]$"),
                 dep("^ ?'([^']+)':(.*)$"),
                 ignore("^$");
    while (!is.eof()) {
        string s; getline(is,s);
        boost::smatch what;
        if (parsing_properties)  {
            if (boost::regex_match(s,what,property) ) { 
                m_properties.push_back(make_pair(what[1],what[2]));
            } else if (boost::regex_match(s,what,propend) ) { 
                parsing_properties = false;
            } else {
                cout << "parsing error while looking for properties!!! : [" << s << "]" << endl;
            }
        } else if (parsing_deps) {
            if (boost::regex_match(s,what,dep) ) { 
                m_depRefs.push_back(make_pair(what[1],digest_type(what[2])));
            } else if (boost::regex_match(s,what,depend) ) { 
                parsing_deps = false;
            } else {
                cout << "parsing error while looking for dependencies!!! : [" << s << "]" << endl;
            }
        } else {
            if (boost::regex_match(s,what,name) ) { assert(m_name.empty());
                m_name = what[1];
            } else if (boost::regex_match(s,what,type) ) { assert(m_type.empty());
                m_type = what[1];
            } else if (boost::regex_match(s,what,propstart) ) { assert(m_properties.empty());
                parsing_properties = true;
            } else if (boost::regex_match(s,what,depstart) ) { assert(m_depRefs.empty());
                parsing_deps = true;
            } else if (boost::regex_match(s,what,ignore)) { 
                // do nothing
            } else {
                cout << "parsing error!!! : [" << s << "]" << endl;
            }
        }
    }
    return is;
}

ostream& PropertyConfig::print(ostream& os) const {
    os << "Name: " << m_name << '\n'
       << "Type: " << m_type << '\n'
       << "Properties: [\n";
    for (Properties::const_iterator i=m_properties.begin();i!=m_properties.end();++i ) 
        os << " '"<< i->first << "':"<<i->second<<'\n';
    os << "]\n" ;
    os << "Dependencies: [\n";
    for (Dependencies::const_iterator i = m_depRefs.begin(); i!= m_depRefs.end();++i)
        os << " '"<< i->first << "':"<<i->second<<'\n';
    os << "]\n";
    return os << endl;
}

PropertyConfig::digest_type 
PropertyConfig::digest() const {
   std::ostringstream s; s << *this;
   return createDigest( std::string( md5(s.str().c_str()).digest().hex_str_value(),32) );
}

std::ostream& operator<<(std::ostream& os, const PropertyConfig& x) { return x.print(os);}
std::istream& operator>>(std::istream& is, PropertyConfig& x)       { return x.read(is);}
std::ostream& operator<<(std::ostream& os, const PropertyConfig::digest_type& x) { return os << x.str() ; }
