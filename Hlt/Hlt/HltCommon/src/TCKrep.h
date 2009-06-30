#ifndef TCKREP_H
#define TCKREP_H
#include "boost/operators.hpp"
#include "boost/format.hpp"

   //TODO: move in dedicated class in HltBase...
class TCKrep : public boost::equality_comparable<TCKrep> {
    public:
      TCKrep() : m_unsigned(0) {}
      explicit TCKrep(unsigned int i) : m_unsigned(i) { set(i); }
      explicit TCKrep(const std::string& s) : m_unsigned(0) { set(s); }
      bool operator<(const TCKrep& rhs) const { return m_unsigned  < rhs.m_unsigned; } 
      bool operator==(const TCKrep& rhs) const { return m_unsigned == rhs.m_unsigned; } 
      bool operator==(unsigned int rhs) const { return m_unsigned == rhs; } 
      bool operator!=(unsigned int rhs) const { return !operator==(rhs); }
      TCKrep& operator&=(unsigned int rhs) { return set( uint() & rhs ); }
      TCKrep& operator++() { return set( ++m_unsigned ); }
      const std::string&  str() const { return m_stringRep; }
      unsigned int uint() const { return m_unsigned;  }
      TCKrep& set(const std::string& s);
      TCKrep& set(unsigned i)           { 
              m_unsigned = i;
              m_stringRep = boost::str( boost::format("0x%08x")%i ) ;
              return *this;
      }
      bool valid() const { return m_unsigned != 0 ; }
    private:
      std::string m_stringRep;
      unsigned int m_unsigned;
};
std::ostream& operator<<(std::ostream& os, const TCKrep& tck);
#endif
