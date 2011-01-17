#include "Kernel/TCK.h"
#include "boost/regex.hpp"
#include "boost/algorithm/string/case_conv.hpp"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GaudiException.h"


namespace {
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
}

TCK& 
TCK::set(const std::string& s) {
        static boost::regex e("^(0x[0-9a-fA-F]{8})$");
        boost::smatch what;
        if(!boost::regex_match(s, what, e)) {
            throw GaudiException("Invalid TCK format",s,StatusCode::FAILURE);
            return *this;
        }
        m_unsigned = unhex(what[1]);
        m_stringRep = s;
        // canonical rep is lower case...
        boost::algorithm::to_lower(m_stringRep);
        return *this;
}


std::ostream& operator<<(std::ostream& os, const TCK& tck) { return os << tck.str(); }

