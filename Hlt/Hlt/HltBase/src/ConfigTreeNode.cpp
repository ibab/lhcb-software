#include "HltBase/ConfigTreeNode.h"
#include "boost/regex.hpp"

using namespace std;
using Gaudi::Math::MD5;


istream& ConfigTreeNode::read(istream& is) {
    bool parsing_nodes = false;
    boost::regex leaf("^Leaf: ([a-fA-F0-9]{32})$"),
                 nodestart("^Nodes: \\\[$"),
                 nodeend("^\\\]$"),
                 node("^ ?([a-fA-F0-9]{32})$"),
                 label("^Label: (.*)$"),
                 ignore("^$");
    while (!is.eof()) {
        string s; getline(is,s);
        boost::smatch what;
        if (parsing_nodes)  {
            if (boost::regex_match(s,what,nodeend) ) { 
                parsing_nodes = false;
            } else if (boost::regex_match(s,what,node) ) { 
                m_nodes.push_back( MD5::convertString2Digest(what[1]));
            } else {
                cout << "parsing error while looking for nodes!!! : [" << s << "]" << endl;
            }
        } else {
            if (boost::regex_match(s,what,leaf) ) { assert(m_leaf.invalid());
                m_leaf = MD5::convertString2Digest( what[1] );
            } else if (boost::regex_match(s,what,nodestart) ) { assert(m_nodes.empty());
                parsing_nodes = true;
            } else if (boost::regex_match(s,what,label) ) { assert(m_label.empty());
                m_label = what[1];
            } else if (boost::regex_match(s,what,ignore)) { 
                // do nothing
            } else {
                cout << "parsing error!!! : [" << s << "]" << endl;
            }
        }
    }
    return is;
}

ostream& ConfigTreeNode::print(ostream& os) const {
    os << "Label: " << m_label << '\n'
       << "Leaf: " << m_leaf << '\n'
       << "Nodes: [\n";
    for (NodeRefs::const_iterator i=m_nodes.begin();i!=m_nodes.end();++i ) 
        os << " "<< *i <<'\n';
    return os << "]" << endl;
}

ConfigTreeNode::digest_type ConfigTreeNode::digest() const { return MD5::computeDigest(*this); }

std::ostream& operator<<(std::ostream& os, const ConfigTreeNode& x) { return x.print(os);}
std::istream& operator>>(std::istream& is, ConfigTreeNode& x)       { return x.read(is);}
