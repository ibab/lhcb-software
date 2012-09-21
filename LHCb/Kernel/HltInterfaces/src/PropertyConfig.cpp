#include <cstdlib>
#include <memory>
#include "Kernel/PropertyConfig.h"
#include "boost/regex.hpp"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IProperty.h"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include <boost/algorithm/string/erase.hpp>

using namespace std;
using namespace boost;
using namespace boost::lambda;

void PropertyConfig::initProperties(const IProperty& obj) {
    typedef vector<Property*> PropertyList;
    const PropertyList& items = obj.getProperties();
    for (PropertyList::const_iterator i = items.begin(); i!=items.end();++i) {
        m_properties.push_back(make_pair((*i)->name(),boost::algorithm::erase_all_copy((*i)->toString(), "\n")));
        // verify that toString / fromString are each others inverse...
        std::auto_ptr<Property> clone( (*i)->clone() );
        if ( clone->fromString(m_properties.back().second).isFailure() ) {
            std::cerr << "Property::fromString fails to parse Property::toString" << std::endl;
            std::cerr << "component: " << m_kind << " / " << m_name << ", property type: " << (*i)->type()<< " : " << (*i)->name() << " -> " << (*i)->toString() << std::endl;
            ::abort(); // this is REALLY bad, and we should die, die, die!!!
                       // as we won't realize this until we read back later, so we
                       // really have to make sure this is never written in the first place
        }
    }
    m_digest = digest_type::createInvalid();
}

void
PropertyConfig::updateCache() const
{
    // type, name and kind MUST be filled for a valid object...
    if (!m_type.empty() && !m_name.empty() && !m_kind.empty() )  {
        m_digest = digest_type::compute(*this);
    }
}

PropertyConfig PropertyConfig::copyAndModify(const std::string& key, const std::string& value ) const
{
    Properties update = properties();
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
    Properties::iterator i = find_if( update.begin(),
                                      update.end(),
                                      [&](Prop& p) { return p.first == key; } );
#else
    Properties::iterator i = find_if( update.begin(),
                                      update.end(),
                                      bind(&Prop::first,_1) == key );
#endif
    if (i==update.end()) {
          cerr << "trying to update a non-existing property: " << key << endl;
          return PropertyConfig();
    }

    i->second = value;
    return PropertyConfig(*this, update);
}

PropertyConfig PropertyConfig::copyAndModify(const std::string& keyAndValue) const
{
    string::size_type c = keyAndValue.find(':');
    if (c == string::npos ) {
        return PropertyConfig();
    }
    return copyAndModify(keyAndValue.substr(0,c),keyAndValue.substr(c+1,string::npos));
}

istream& PropertyConfig::read(istream& is) {
    //TODO: switch to XML/JSON
    bool parsing_properties = false;
    static boost::regex name("^Name: (.*)$"),
                        type("^Type: (.*)$"),
                        kind("^Kind: (.*)$"),
                        propstart("^Properties: \\[$"),
                        propend("^\\]$"),
                        property("^ ?'([^']+)':(.*)$"),
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
        } else {
            if (boost::regex_match(s,what,name) ) { assert(m_name.empty());
                m_name = what[1];
            } else if (boost::regex_match(s,what,type) ) { assert(m_type.empty());
                m_type = what[1];
            } else if (boost::regex_match(s,what,kind) ) { assert(m_kind.empty());
                m_kind = what[1];
            } else if (boost::regex_match(s,what,propstart) ) { assert(m_properties.empty());
                parsing_properties = true;
            } else if (boost::regex_match(s,what,ignore)) {
                // do nothing
            } else {
                cout << "parsing error!!! : [" << s << "]" << endl;
            }
        }
    }
    m_digest = digest_type::createInvalid();
    return is;
}

ostream& PropertyConfig::print(ostream& os) const {
    //TODO: switch to XML/JSON

    os << "Name: " << m_name << '\n'
       << "Kind: " << m_kind << '\n'
       << "Type: " << m_type << '\n'
       << "Properties: [\n";
    for (Properties::const_iterator i=m_properties.begin();i!=m_properties.end();++i )
        os << " '"<< i->first << "':"<< boost::algorithm::erase_all_copy(i->second, "\n") <<'\n';
    return os << "]" << endl;
}

std::ostream& operator<<(std::ostream& os, const PropertyConfig& x) { return x.print(os);}
std::istream& operator>>(std::istream& is, PropertyConfig& x) { return x.read(is); }
