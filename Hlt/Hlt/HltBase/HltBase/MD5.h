#ifndef MD5_H
#define MD5_H 1
#include "boost/cstdint.hpp"
#include <cstring>
#include <iostream>
#include <sstream>

class MD5 {
public:
    class Digest {
        public:
           typedef boost::uint8_t value_type[16];
           explicit Digest(const value_type& val) { memcpy(m_value,val,sizeof(m_value)); }

           std::string str() const; 
           bool invalid() const { value_type x; return memcmp(m_value,memset(x,0u,sizeof(x)),sizeof(m_value))==0;}
           bool operator< (const Digest& rhs) const { return memcmp(m_value,rhs.m_value,sizeof(m_value))<0;}
           bool operator> (const Digest& rhs) const { return memcmp(m_value,rhs.m_value,sizeof(m_value))>0;}
           bool operator==(const Digest& rhs) const { return memcmp(m_value,rhs.m_value,sizeof(m_value))==0;}
           bool operator<=(const Digest& rhs) const { return !operator>(rhs);}
           bool operator>=(const Digest& rhs) const { return !operator<(rhs);}
           bool operator!=(const Digest& rhs) const { return !operator==(rhs);}
        private:
           friend class MD5;
           explicit Digest(const std::string& val);

           value_type m_value;
    };

    static Digest convertString2Digest(const std::string& s);
    static Digest createInvalidDigest() { MD5::Digest::value_type x; memset(x,0u,sizeof(x)); return Digest(x); }


    // the following two are the real 'guts' of this class...
    static Digest computeDigest(const std::string& s);

    template <typename T>
    static Digest computeDigest(const T& t) { 
            std::ostringstream x; x << t; return computeDigest(x.str());
    }
};

std::ostream& operator<<(std::ostream& os, const MD5::Digest& x) ;
#endif
