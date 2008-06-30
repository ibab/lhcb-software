#include "HltBase/ConfigTreeNodeAlias.h"
#include "GaudiKernel/GaudiException.h"
#include "boost/regex.hpp"
#include "boost/assign/list_of.hpp"

#include <cassert>

using namespace std;
using Gaudi::Math::MD5;

std::string ConfigTreeNodeAlias::alias_type::major() const {
    string::size_type i = m_alias.find('/');
    assert(i!=string::npos);
    return m_alias.substr(0,i);
}

bool ConfigTreeNodeAlias::alias_type::valid() const { 
//@TODO: require we start with 'TCK/','TOPLEVEL/', or 'TAG/'
    return !m_alias.empty(); 
}

std::ostream& ConfigTreeNodeAlias::alias_type::print(std::ostream& os) const {
    return os << m_alias;
}

std::istream& ConfigTreeNodeAlias::alias_type::read(std::istream& os) {
//@TODO: validate input!!!
    return os >> m_alias;
}

istream& ConfigTreeNodeAlias::read(istream& is) {
    boost::regex ref("^Ref: ([a-fA-F0-9]{32})$"),
                 alias("^Alias: (.*)$");
    while (!is.eof()) {
        string s; getline(is,s);
        boost::smatch what;
        if (boost::regex_match(s,what,ref))   m_ref   = digest_type::createFromStringRep(what[1]);
        if (boost::regex_match(s,what,alias)) m_alias = what[1];
    }
    assert(!m_ref.invalid());
    assert(!m_alias.str().empty());
    return is;
}

ostream& ConfigTreeNodeAlias::print(ostream& os) const {
    return os << "Ref: "   << ref() << "\n"
              << "Alias: " << alias().str()  << endl;
}

std::ostream& operator<<(std::ostream& os, const ConfigTreeNodeAlias& x) { return x.print(os); }
std::istream& operator>>(std::istream& is, ConfigTreeNodeAlias& x) { return x.read(is); }

std::ostream& operator<<(std::ostream& os, const ConfigTreeNodeAlias::alias_type& x) { return x.print(os); }
std::istream& operator>>(std::istream& is, ConfigTreeNodeAlias::alias_type& x) { return x.read(is); }
