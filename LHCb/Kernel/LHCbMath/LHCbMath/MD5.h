#ifndef MD5_H
#define MD5_H 1
#include "boost/cstdint.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
// ============================================================================
/** @file MD5.h
 *
 *
 *  @author Gerhard Raven Gerhard.Raven@cern.ch
 *  @date 2008-05-22 
 */
// ============================================================================

namespace Gaudi 
{
  namespace Math 
  {

    class MD5 {
        private:
           class md5_engine;
        public:
           typedef boost::uint8_t value_type[16];

           /// create an (recognizable!) invalid digest...
           static MD5 createInvalid() 
           { value_type x; memset(x,0u,sizeof(x)); return MD5(x); }

           /// compute MD5 digest for the specified string
           static MD5 compute(const std::string& s);

           /// compute MD5 digest for the provided object by first streaming it to a string
           template <typename T> static MD5 compute(const T& t) 
           { std::ostringstream x; x << t; return compute(x.str()); }

           /// provide a hex string representation of the (internal) 128 bit value of the computed digest
           std::string str() const; 

           /// convert the hex string representation back to a full-blown instance
           static MD5 createFromStringRep(const std::string& s);

           /// check whether the digest is the (valid) result of an MD5 computation
           bool invalid() const 
           { value_type x; 
             return memcmp(m_value,memset(x,0u,sizeof(x)),sizeof(m_value))==0; 
           }
           bool valid() const { return !invalid(); }

           /// binary comparisons
           bool operator< (const MD5& rhs) const 
           { return memcmp(m_value,rhs.m_value,sizeof(m_value))<0; }
           bool operator> (const MD5& rhs) const 
           { return memcmp(m_value,rhs.m_value,sizeof(m_value))>0; }
           bool operator==(const MD5& rhs) const 
           { return memcmp(m_value,rhs.m_value,sizeof(m_value))==0; }

           bool operator<=(const MD5& rhs) const { return !operator>(rhs); }
           bool operator>=(const MD5& rhs) const { return !operator<(rhs); }
           bool operator!=(const MD5& rhs) const { return !operator==(rhs); }

        private:
           explicit MD5(const value_type& val) { memcpy(m_value,val,sizeof(m_value)); }

           value_type m_value;
    };
  } // end of namespace Math
} // end of namespace Gaudi

std::ostream& operator<<(std::ostream& os, const Gaudi::Math::MD5& x) ;
#endif
