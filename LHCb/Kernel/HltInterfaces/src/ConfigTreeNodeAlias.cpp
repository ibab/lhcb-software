#include "Kernel/ConfigTreeNodeAlias.h"
#include "GaudiKernel/GaudiException.h"
#include "boost/regex.hpp"
#include "boost/assign/list_of.hpp"

#include <cassert>

using namespace std;
using Gaudi::Math::MD5;

unsigned char unhex(unsigned char C) {
    unsigned char c=tolower(C);
    boost::uint8_t x = ( c >= '0' && c <= '9' ? c-'0' :
                       ( c >= 'a' && c <='f'  ? 10+(c-'a') : 255 ) );
    if ( x&0xF0 ) {  /* whoah: C is not in [0-9a-fA-F] */ }
    return x;
}

unsigned int unhex(const std::string& val) {
    assert( val.substr(0,2)=="0x" );
    assert( val.size()==10 );
    unsigned int i = 0;
    const char *x = val.c_str()+2;
    while (*x) i = ( i<<4 | unhex(*x++) );
    return i;
}

void
ConfigTreeNodeAlias::invalidate(const std::string& reason) {
    if (!reason.empty()) cerr << reason << endl;
    m_ref = digest_type::createInvalid(); 
    m_alias = std::string();
}

ConfigTreeNodeAlias::ConfigTreeNodeAlias(const digest_type& ref, const alias_type& alias)
      : m_ref(ref), m_alias(alias) { 
    //Verify validity rules: 
    if (alias.major()=="TCK")  {
        //  If TCK, it must of format TCK/0xabcd1234
        static boost::regex e("^TCK/(0x[0-9a-fA-F]{8})$");
        boost::smatch what;
        if(!boost::regex_match(alias.str(), what, e)) {
            invalidate("invalid TCK format");
            return;
        }
        //  and if most significant bit is set, lower 16 must be zero and vice versa
        unsigned int tck = unhex(what[1]);
        if (  (tck & 0x80000000) != 0 ? ((tck&0xFFFF)!=0) : ((tck&0xFFFF)==0) ){
           invalidate("the requested TCK does not match the rules..");
           return;
        }
    } else if (alias.major()=="TOPLEVEL") {
        // must have TOPLEVEL/release/runtype/digest
        static boost::regex e("^TOPLEVEL/[^/]+/[^/]+/([0-9a-fA-F]{32})$");
        boost::smatch what;
        if(!boost::regex_match(alias.str(), what, e)) {
            invalidate("invalid TOPLEVEL format");
            return;
        }
    } else if (alias.major()=="TAG") {
        // No rules just yet...
    } else {
        invalidate("unknown major");
    }
}

std::string ConfigTreeNodeAlias::alias_type::major() const {
    string::size_type i = m_alias.find('/');
    assert(i!=string::npos);
    return m_alias.substr(0,i);
}

bool ConfigTreeNodeAlias::alias_type::valid() const { 
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
    static boost::regex ref("^Ref: ([a-fA-F0-9]{32})$"),
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
