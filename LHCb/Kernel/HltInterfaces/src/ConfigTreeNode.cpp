#include "Kernel/ConfigTreeNode.h"
#include "boost/regex.hpp"

#include "GaudiKernel/GaudiException.h"

using Gaudi::Math::MD5;

ConfigTreeNode::ConfigTreeNode(const LeafRef& leaf, const NodeRefs& nodes, std::string label)
      : m_nodes(nodes)
      , m_leaf(leaf)
      , m_label( std::move(label))
      , m_digest( digest_type::createInvalid() )
{ 
 if (this->label().find(':')!=std::string::npos)
    throw GaudiException("':' is not allowed in label",label,StatusCode::FAILURE);
}

void ConfigTreeNode::updateCache() const { 
    m_digest = digest_type::compute(str());
}

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/stream_translator.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
using boost::property_tree::ptree;
namespace {
void read_custom(std::istream& is, ptree& top) {
    bool parsing_nodes = false;
    static boost::regex leaf("^Leaf: ([a-fA-F0-9]{32})$"),
                        label("^Label: (.*)$"),
                        nodestart("^Nodes: \\[$"),
                        node("^ ?([a-fA-F0-9]{32})$"),
                        nodeend("^\\]$");
    std::string s;
    boost::smatch what;
    auto& nodes = top.put_child(ptree::path_type("Nodes"),ptree{});
    while (std::istream::traits_type::not_eof( is.peek()) ) {
        getline(is,s);
        if (s.empty()) continue;
        if (parsing_nodes)  {
            if (boost::regex_match(s,what,nodeend) ) { 
                parsing_nodes = false;
            } else if (boost::regex_match(s,what,node) ) { 
                nodes.push_back(std::make_pair(std::string{},ptree{what[1].str()}));
            } else {
                std::cerr << "ConfigTreeNode: read_custom: parsing error while looking for nodes!!! : [" << s << "]" << std::endl;
                top = ptree{} ; return;
            }
        } else {
            if (boost::regex_match(s,what,leaf) ) {
                top.put("Leaf", what[1].str() );
            } else if (boost::regex_match(s,what,nodestart) ) {
                parsing_nodes = true;
            } else if (boost::regex_match(s,what,label) ) {
                top.put("Label",what[1].str());
            } else {
                std::cerr << "ConfigTreeNode: read_custom: parsing error!!! : [" << s << "]" << std::endl;
                top = ptree{} ; return;
            }
        }
    }
}

struct MD5Translator {
    typedef std::string                  internal_type;
    typedef ConfigTreeNode::digest_type  external_type;
    boost::optional<external_type> get_value( internal_type const & v) 
    { return external_type::createFromStringRep(v); }
    boost::optional<internal_type> put_value( external_type const & v) 
    { return v.str(); }
};

}

namespace boost { namespace property_tree {
    template <> 
    struct translator_between<std::string,ConfigTreeNode::digest_type> {
        typedef MD5Translator type;
    };
} }


std::istream& ConfigTreeNode::read(std::istream& is) {
    m_digest = digest_type::createInvalid();
    ptree top;
    int fmt = is.peek();
    if (fmt=='{')      { read_json(is,top);   }
    else if (fmt=='<') { read_xml(is,top);    }
    else               { read_custom(is,top); }
    if (top.empty()) {
        // flag as invalid 'forever' -- needs to interact with updateCache
        m_label.clear();
        m_nodes.clear();
        m_leaf = digest_type::createInvalid();
        m_digest = digest_type::createInvalid();
    } else {
        m_label = top.get<std::string>("Label",std::string()); // add default: empty string (most nodes have no label)
        m_leaf  = top.get<digest_type>("Leaf",digest_type::createInvalid());  // add default: digest_type::createInvalid() (top level has no leaf)
        const auto& nodes = top.get_child("Nodes", ptree{});
        m_nodes.clear(); m_nodes.reserve( nodes.size() );
        std::transform( std::begin(nodes), std::end(nodes), 
                        std::back_inserter(m_nodes),
                        []( const ptree::value_type& i ) { return i.second.get_value<ConfigTreeNode::digest_type>() ; } );
    }
    return is;
}

std::string ConfigTreeNode::str() const {
    std::string out; out.reserve( 58+label().size()+34*nodes().size() );
    out +=  "Label: "; out += label();       out+= '\n';
    out +=  "Leaf: ";  out += leaf().str();  out+= '\n';
    out +=  "Nodes: [\n";
    std::for_each( std::begin(nodes()), std::end(nodes()), [&out]( const NodeRef& i ) {
         out += ' '; out +=  i.str() ; out += '\n';
    } );
    out +=  "]\n";
    return out;
}

std::ostream& ConfigTreeNode::print(std::ostream& os) const {
    // note: advantage of json (or xml): in case of hash collision, can add an optional extra field...
    // ...but that only works if the json representation is used to compute the digest!!!
    ptree top;
    if (!label().empty()) top.put("Label",label());
    if (leaf().valid())   top.put("Leaf",leaf().str());
    if (!nodes().empty()) {
        std::transform( std::begin(nodes()), std::end(nodes()),
                        std::back_inserter(top.put_child("Nodes",ptree{})),
                        []( const NodeRef& i ) { return std::make_pair(std::string(),ptree{ i.str() }); }  );
    }
    write_json(os,top,false);
    return os;
}
