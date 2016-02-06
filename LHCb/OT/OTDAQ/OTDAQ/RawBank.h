#ifndef OTDAQ_OTRAWBANK_H 
#define OTDAQ_OTRAWBANK_H 1

#include "OTDAQ/Gol.h"
#include "OTDAQ/OTSpecificHeader.h"

namespace OTDAQ
{
  class RawBank
  {
  public:
    typedef std::vector<Gol> GolContainer ;

    RawBank( int bankversion, size_t banksize, const OTSpecificHeader& otbankheader ) :
      m_version(bankversion), m_size(banksize), m_header(otbankheader) {}
    
    size_t size() const { return m_size ; }
    int version() const { return m_version ; }
    const OTSpecificHeader& header() const { return m_header ; }
    const GolContainer& gols() const { return m_gols ; }
    
  public:
    GolContainer& gols() { return m_gols ; }
    
  private:
    int m_version ;
    size_t m_size ;
    OTSpecificHeader m_header ;
    GolContainer m_gols ;
  } ;
}

#endif
