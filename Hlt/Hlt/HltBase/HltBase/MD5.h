#ifndef MD5_H
#define MD5_H 1
#include "boost/cstdint.hpp"
#include <string.h>
#include <iostream>
#include <sstream>

class MD5 {
public:
    class Digest {
        public:
           typedef boost::uint8_t value_type[16];
           std::string str() const { return hexit(m_value,m_value+sizeof(value_type)); }
           bool invalid() const { value_type x; return memcmp(m_value,memset(x,0u,sizeof(value_type)),sizeof(value_type))==0;}
           bool operator< (const Digest& rhs) const { return memcmp(m_value,rhs.m_value,sizeof(value_type))<0;}
           bool operator> (const Digest& rhs) const { return memcmp(m_value,rhs.m_value,sizeof(value_type))>0;}
           bool operator==(const Digest& rhs) const { return memcmp(m_value,rhs.m_value,sizeof(value_type))==0;}
           bool operator<=(const Digest& rhs) const { return !operator>(rhs);}
           bool operator>=(const Digest& rhs) const { return !operator<(rhs);}
           bool operator!=(const Digest& rhs) const { return !operator==(rhs);}
        private:
           friend class MD5;
           explicit Digest(const std::string& val) { unhexit(val,m_value); }
           explicit Digest(const value_type val) { memcpy(m_value,val,sizeof(value_type)); }

           static std::string hexit(const boost::uint8_t *begin, const boost::uint8_t *end);
           static void unhexit(const std::string& s,value_type& t);
           value_type m_value;
    };

    static Digest convertString2Digest(const std::string& s) {
        return Digest(s);
    }
    static Digest createInvalidDigest() {
       return Digest(std::string());
    }


    // the following two are the real 'guts' of this class...
    static Digest computeDigest(const std::string& s);

    template <typename T>
    static Digest computeDigest(const T& t) { 
            std::ostringstream x; x << t;
            return computeDigest(x.str());
    }
};

std::ostream& operator<<(std::ostream& os, const MD5::Digest& x) ;
#endif
