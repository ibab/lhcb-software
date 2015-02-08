#include <cstdlib>
#include <memory>
#include "Kernel/PropertyConfig.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IProperty.h"

void PropertyConfig::initProperties(const IProperty& obj) {
    const auto& items = obj.getProperties();
    std::for_each( std::begin(items), std::end(items), [&]( const Property *i ) {
        // FIXME: check for duplicates!!!
        m_properties.emplace_back(i->name(),i->toString()); 
        // verify that toString / fromString are each others inverse...
        // WARNING: this does not guarantee that we don't loose information -- toString may be lossy!!!!
        std::unique_ptr<Property> clone( i->clone() );
        if ( clone->fromString(m_properties.back().second).isFailure() ) {
            std::cerr << "Property::fromString fails to parse Property::toString" << std::endl;
            std::cerr << "component: " << kind() << " / " << name() << ", property type: " << i->type()<< " : " << i->name() << " -> " << i->toString() << std::endl;
            ::abort(); // this is REALLY bad, and we should die, die, die!!!
                       // as we won't realize this until we read back later, so we
                       // really have to make sure this is never written in the first place
        }
        if ( clone->toString() != m_properties.back().second ) {
            std::cerr << " Property::fromString, followed by Property::toString does not give the same result:\n"
                      << "\'" <<  clone->toString() << "\' vs. \n\'" << m_properties.back().second << "\'"
                      << std::endl;
            // this is still no guarantee everything is OK, as it checks that checks that toString is a projection operator. 
            // The initial toString may be lossy, and this 'just' checks that the subsequent toString doesn't loose anything 
            // more than the initial one...
            ::abort();
        }
    } );
    m_digest = digest_type::createInvalid();
}

void
PropertyConfig::updateCache() const
{
    // type, name and kind MUST be filled for a valid object...
    if (!type().empty() && !name().empty() && !kind().empty() )  {
        m_digest = digest_type::compute(str());
    }
}

PropertyConfig PropertyConfig::copyAndModify(const std::string& key, const std::string& value ) const
{
    auto  update = properties();
    auto i = std::find_if( std::begin(update), std::end(update),
                           [&key](const Prop& p) { return p.first == key; } );
    if (i==update.end()) {
          std::cerr << "trying to update a non-existing property: " << key << std::endl;
          return PropertyConfig();
    }
    i->second = value;
    return PropertyConfig(*this, update);
}

PropertyConfig PropertyConfig::copyAndModify(const std::string& keyAndValue) const
{
    auto c = keyAndValue.find(':');
    if (c == std::string::npos ) return PropertyConfig();
    return copyAndModify(keyAndValue.substr(0,c),keyAndValue.substr(c+1,std::string::npos));
}

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/regex.hpp"

using boost::property_tree::ptree;
namespace {
void read_custom(std::istream& is, ptree& top) {
    bool parsing_properties = false;
    static boost::regex propstart("^Properties: \\[$"),
                        property("^ ?'([^']+)':(.*)$"),
                        propend("^\\]$"),
                        topitem("^(Name|Type|Kind): (.*)$");
    boost::smatch what;
    std::string s;
    ptree& props = top.put_child("Properties",ptree{});
    while (std::istream::traits_type::not_eof( is.peek()) ) {
        getline(is,s);
        if (s.empty()) continue;
        if (parsing_properties) {
            if (boost::regex_match(s,what,property) ) {
                // props.put(what[1].str(),what[2].str());
                // use add instead of put as Gaudi allows multiple properties with 
                // the same name ... surely an oversight...
                props.add(what[1].str(),what[2].str());
            } else if (boost::regex_match(s,what,propend) ) {
                parsing_properties = false;
            } else {
                std::cerr << "parsing error while looking for properties!!! : [" << s << "]" << std::endl;
            }
        } else {
            if (boost::regex_match(s,what,topitem) ) { 
                top.put(what[1].str(),what[2].str()); 
            } else if (boost::regex_match(s,what,propstart) ) { 
                assert(props.empty());
                parsing_properties = true;
            } else { 
                std::cerr << "parsing error!!! : [" << s << "]" << std::endl; 
            }
        }
    }
}
}

std::istream& PropertyConfig::read(std::istream& is) {
    m_digest = digest_type::createInvalid(); // reset our state;
    ptree top;
    int fmt = is.peek();
    if (fmt=='{')      { read_json(is,top);   }
    else if (fmt=='<') { read_xml(is,top);    }
    else               { read_custom(is,top); }
    if (top.empty()) {
        // flag as invalid 'forever' -- needs to interact with updateCache
    } else {
        m_name = top.get<std::string>("Name");
        m_kind = top.get<std::string>("Kind");
        m_type = top.get<std::string>("Type");
        const ptree& props = top.get_child("Properties");
        m_properties.clear(); m_properties.reserve( props.size() );
        std::transform( std::begin(props), std::end(props), 
                        std::back_inserter(m_properties),
                        []( const ptree::value_type& i ) { return std::make_pair( i.first, i.second.data() ); } );
    }
    return is;
}

std::string PropertyConfig::str() const {
    // this is the 'original' canonical representation that MUST be used to compute the hash....
    // (unless we 'convert' & 're-index' the already written data to any new format)
    // as a result, do NOT change the result of this function!!!
    std::string out; 
    out.reserve(std::accumulate( std::begin(properties()), std::end(properties()),
                                 name().size()+kind().size()+type().size()+37,
                                 [](int s, const Prop& i) { 
                        return s+5+i.first.size()+i.second.size(); 
    } ) ) ;
    out +=  "Name: "; out += name(); out += '\n';
    out +=  "Kind: "; out += kind(); out += '\n';
    out +=  "Type: "; out += type(); out += '\n';
    out +=  "Properties: [\n";
    std::for_each( std::begin(properties()), std::end(properties()), [&out]( const Prop& i ) {
        out +=  " '"; out+= i.first ; out += "':"; 
        // remove all newlines... as we use newline as a record seperator (and yes, this
        // should have been escaped instead... didn't think of that when I originally wrote 
        // it...)
        std::copy_if( std::begin(i.second), std::end(i.second), 
                      std::back_inserter(out), 
                      [](const char x) { return x!='\n'; } );
        out += '\n';
    } );
    out += "]\n" ;
    return out;
}

std::ostream& PropertyConfig::print(std::ostream& os) const {
    // note: advantage of json (or xml): in case of hash collision, can add an optional extra field...
    // but that only works if the json representation is used to compute the digest -- which isn't the
    // case!!! (not impossible, but requires a very carefull, non-trivial re-persisting of the already 
    // persistent config data...)
    ptree top;
    top.put("Name",name());
    top.put("Kind",kind());
    top.put("Type",type());
    std::transform( std::begin(properties()), std::end(properties()),
                    std::back_inserter(top.put_child("Properties",ptree{})),
                    []( const Prop& i ) { return std::make_pair(i.first,ptree{i.second}); } );
    write_json(os,top,false);
    return os;
}
