#include "HltBase/MD5.h"
#include "md5.h"

using namespace std;

MD5::Digest
MD5::computeDigest(const string& s) {
   md5 x(s.c_str());
   return MD5::Digest( string( x.digest().hex_str_value(),2*sizeof(MD5::Digest::value_type)) );
}

string 
MD5::Digest::str() const {
    string s; s.reserve(2*sizeof(value_type));
    for (const boost::uint8_t* i = m_value; i!=m_value+sizeof(value_type); ++i) {
        static const char *digits="0123456789abcdef";
        s+=digits[(*i>>4)&0x0F]; s+=digits[(*i)&0x0F];
    }
    return s;
}

namespace {
    boost::uint8_t unhex(unsigned char C) {
            unsigned char c=tolower(C);
            boost::uint8_t x = ( c >= '0' && c <= '9' ? c-'0' :
                               ( c >= 'a' && c <='f'  ? 10+(c-'a') 
                                                      : 255 ) );
            if ( x&0xF0 ) {  /* whoah: C is not in [0-9a-fA-F] */ }
            return x;
    };
};

MD5::Digest::Digest(const std::string& val) { 
    assert(val.size()==2*sizeof(value_type) || val.empty());
    if (val.empty()) {
      memset(m_value,0u,sizeof(value_type));
    } else {
      for (size_t i=0;i<sizeof(value_type);++i) m_value[i]=(unhex(val[2*i])<<4|unhex(val[2*i+1]));
    }
};


ostream& operator<<(ostream& os, const MD5::Digest& x) {
   return os << x.str(); 
}
