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

void PropertyConfig::initProperties(const IProperty& obj) {
    typedef vector<Property*> PropertyList;
    const PropertyList& items = obj.getProperties();
    for (PropertyList::const_iterator i = items.begin(); i!=items.end();++i) {
        // FIXME: check for duplicates!!!
        // FIXME: remove the erasing of all newlines when switching to json or xml...
        m_properties.push_back(make_pair((*i)->name(),boost::algorithm::erase_all_copy((*i)->toString(), "\n")));
        // verify that toString / fromString are each others inverse...
        // WARNING: this does not guarantee that we don't loose information -- toString may be lossy!!!!
        std::auto_ptr<Property> clone( (*i)->clone() );
        if ( clone->fromString(m_properties.back().second).isFailure() ) {
            std::cerr << "Property::fromString fails to parse Property::toString" << std::endl;
            std::cerr << "component: " << kind() << " / " << name() << ", property type: " << (*i)->type()<< " : " << (*i)->name() << " -> " << (*i)->toString() << std::endl;
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
    if (!type().empty() && !name().empty() && !kind().empty() )  {
        std::ostringstream str; this->print(str);
        m_digest = digest_type::compute(str.str());
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
    using namespace boost::lambda;
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
    if (c == string::npos ) return PropertyConfig();
    return copyAndModify(keyAndValue.substr(0,c),keyAndValue.substr(c+1,string::npos));
}

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

using boost::property_tree::ptree;
namespace {
void read_custom(istream& is, ptree& top) {
    bool parsing_properties = false;
    static boost::regex propstart("^Properties: \\[$"),
                        property("^ ?'([^']+)':(.*)$"),
                        propend("^\\]$"),
                        topitem("^(Name|Type|Kind): (.*)$");
    boost::smatch what;
    string s;
    ptree& props = top.put_child("Properties",ptree());
    while (!is.eof()) {
        getline(is,s);
        if (s.empty()) break;
        if (parsing_properties)  {
            if (boost::regex_match(s,what,property) ) {
                // props.put(what[1].str(),what[2].str());
                // use add instead of put as Gaudi allows multiple properties with 
                // the same name ... surely an oversight...
                props.add(what[1].str(),what[2].str());
            } else if (boost::regex_match(s,what,propend) ) {
                parsing_properties = false;
            } else {
                cerr << "parsing error while looking for properties!!! : [" << s << "]" << endl;
            }
        } else {
            if (boost::regex_match(s,what,topitem) ) { 
                top.put(what[1].str(),what[2].str()); 
            } else if (boost::regex_match(s,what,propstart) ) { 
                assert(props.empty());
                parsing_properties = true;
            } else { cerr << "parsing error!!! : [" << s << "]" << endl; }
        }
    }
}
}

istream& PropertyConfig::read(istream& is) {
    m_digest = digest_type::createInvalid(); // reset our state;
    ptree top;
    int fmt = is.peek();
    if (fmt=='{')      { read_json(is,top);   }
    else if (fmt=='<') { read_xml(is,top);    }
    else               { read_custom(is,top); }
    m_name = top.get<std::string>("Name");
    m_kind = top.get<std::string>("Kind");
    m_type = top.get<std::string>("Type");
    const ptree& props = top.get_child("Properties");
    m_properties.clear();
    for (ptree::const_iterator i=props.begin(); i!=props.end(); ++i ) m_properties.push_back(make_pair(i->first,i->second.data()));
    return is;
};


ostream& PropertyConfig::print(ostream& os) const {
    // this is the 'original' canonical representation that MUST be used to compute the hash....
    // (unless we 'convert' the already written data to a new format)
    os << "Name: " << name() << '\n'
       << "Kind: " << kind() << '\n'
       << "Type: " << type() << '\n'
       << "Properties: [\n";
    for (Properties::const_iterator i=properties().begin();i!=properties().end();++i )
        os << " '"<< i->first << "':"<< boost::algorithm::erase_all_copy(i->second, "\n") <<'\n';
    return os << "]" << endl;
}

// for now, do not make this the default....
ostream& PropertyConfig::print_json(ostream& os) const {
    // note: advantage of json (or xml): in case of hash collision, can add an optional extra field...
    ptree top;
    top.put("Name",name());
    top.put("Kind",kind());
    top.put("Type",type());
    ptree& props = top.put_child("Properties",ptree());
    for (Properties::const_iterator i=m_properties.begin();i!=m_properties.end();++i ) props.put(i->first,i->second);
    write_json(os,top);
    return os;
}

std::ostream& operator<<(std::ostream& os, const PropertyConfig& x) { return x.print(os);}
std::istream& operator>>(std::istream& is, PropertyConfig& x) { return x.read(is); }
