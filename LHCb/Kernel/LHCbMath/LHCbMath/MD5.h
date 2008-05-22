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
 *  @author Gerhad Raven Gerhard.Raven@cern.ch
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
        /** Digest
         * represents the result of an MD5 computation, i.e.
         * internally it is an 128 bit quantitity, and provides
         * a nicer interface to this quantity.
         **/
         
        class Digest {
            public:
               typedef boost::uint8_t value_type[16];

               std::string str() const; 
               bool invalid() const { value_type x; return memcmp(m_value,memset(x,0u,sizeof(x)),sizeof(m_value))==0;}
               bool valid() const { return !invalid(); }
               bool operator< (const Digest& rhs) const { return memcmp(m_value,rhs.m_value,sizeof(m_value))<0;}
               bool operator> (const Digest& rhs) const { return memcmp(m_value,rhs.m_value,sizeof(m_value))>0;}
               bool operator==(const Digest& rhs) const { return memcmp(m_value,rhs.m_value,sizeof(m_value))==0;}
               bool operator<=(const Digest& rhs) const { return !operator>(rhs);}
               bool operator>=(const Digest& rhs) const { return !operator<(rhs);}
               bool operator!=(const Digest& rhs) const { return !operator==(rhs);}
            private:
               friend class MD5;
               friend class md5_engine;
               explicit Digest(const std::string& val);
               explicit Digest(const value_type& val) { memcpy(m_value,val,sizeof(m_value)); }

               value_type m_value;
        };

        static Digest convertString2Digest(const std::string& s);
        static Digest createInvalidDigest() { MD5::Digest::value_type x; memset(x,0u,sizeof(x)); return Digest(x); }


        // the following two are the real 'guts' of this class...

        // Compute MD5 digest of the provided string
        static Digest computeDigest(const std::string& s);

        // given an object T which can be streamed into a string,
        // this function computes the MD5 digest of the string, and
        // returns the corresponding Digest object
        template <typename T>
        static Digest computeDigest(const T& t) { 
                std::ostringstream x; x << t; return computeDigest(x.str());
        }
    };
  } // end of namespace Math
} // end of namespace Gaudi

std::ostream& operator<<(std::ostream& os, const Gaudi::Math::MD5::Digest& x) ;
#endif
