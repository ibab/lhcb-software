#include "Kernel/ConfigTreeNodeAlias.h"
#include "GaudiKernel/GaudiException.h"
#include "boost/regex.hpp"

#include <cassert>

using Gaudi::Math::MD5;

void ConfigTreeNodeAlias::invalidate(const std::string& reason) {
    if (!reason.empty()) std::cerr << reason << std::endl;
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
        unsigned int tck = std::stoul(what[1],nullptr,0);
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
    std::string::size_type i = m_alias.find('/');
    assert(i!=std::string::npos);
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

std::istream& ConfigTreeNodeAlias::read(std::istream& is) {
    static boost::regex ref("^Ref: ([a-fA-F0-9]{32})$"),
                        alias("^Alias: (.*)$");
    while (std::istream::traits_type::not_eof( is.peek()) ) {
        std::string s; getline(is,s);
        boost::smatch what;
        if (boost::regex_match(s,what,ref))   m_ref   = digest_type::createFromStringRep(what[1]);
        if (boost::regex_match(s,what,alias)) m_alias = what[1];
    }
    assert(!m_ref.invalid());
    assert(!m_alias.str().empty());
    return is;
}

std::ostream& ConfigTreeNodeAlias::print(std::ostream& os) const {
    return os << "Ref: "   << ref() << "\n"
              << "Alias: " << alias().str()  << std::endl;
}
