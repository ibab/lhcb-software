#include "HltBase/MD5.h"
#include "md5.h"

using namespace std;

MD5::Digest
MD5::computeDigest(const string& s) {
   md5 x(s.c_str());
   return MD5::Digest( string( x.digest().hex_str_value(),32) );
}

namespace {
    boost::uint8_t unhex(char C) {
            char c=tolower(C);
            boost::uint8_t x = ( c >= '0' && c <= '9' ? c-'0' :
                               ( c >= 'a' && c <='f'  ? 10+(c-'a') 
                                                      : 255 ) );
            if (x==255) {  /* whoah  */ }
            return x;
    };
};

string 
MD5::Digest::hexit(const uint8_t *begin,const uint8_t *end) {
    string s;
    static const char *digits="0123456789abcdef";
    while(begin!=end) {
        s+=digits[(*begin>>4)&0x0F];
        s+=digits[(*begin++ )&0x0F];
    }
    return s;
}

void 
MD5::Digest::unhexit(const string& s,value_type& t) {
    assert(s.size()==32 || s.empty());
    if (s.empty()) {
      for (int i=0;i<16;++i) t[i]=0;
    } else {
      for (int i=0;i<16;++i) t[i]=(unhex(s[2*i])<<4|unhex(s[2*i+1]));
    }
};


ostream& operator<<(ostream& os, const MD5::Digest& x) {
   return os << x.str(); 
}
