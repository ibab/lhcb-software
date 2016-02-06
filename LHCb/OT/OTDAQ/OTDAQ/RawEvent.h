#ifndef OTDAQ_RAWEVENT_H
#define OTDAQ_RAWEVENT_H

#include <vector>
#include "OTDAQ/RawBank.h"

namespace OTDAQ
{

  class RawEvent
  {
  public:
    typedef std::vector<RawBank> RawBankContainer ;
    RawEvent() {}
    void clear() { m_banks.clear() ; }
    const RawBankContainer& banks() const { return m_banks ; }
    RawBankContainer& banks() { return m_banks ; }
    size_t numRawHits() const ;
    size_t numGols() const ;
  private:
    RawBankContainer m_banks ;
  } ;

  inline size_t RawEvent::numRawHits() const
  {
    size_t rc(0) ;
    for( RawBankContainer::const_iterator ibank = m_banks.begin() ;
	 ibank != m_banks.end(); ++ibank)
      for( RawBank::GolContainer::const_iterator igol = ibank->gols().begin() ;
	   igol != ibank->gols().end(); ++igol )
	rc += igol->hits().size() ;
    return rc ;
  }

  inline size_t RawEvent::numGols() const
  {
    size_t rc(0) ;
    for( RawBankContainer::const_iterator ibank = m_banks.begin() ;
	 ibank != m_banks.end(); ++ibank)
      rc += ibank->gols().size() ;
    return rc ;
  }
  
}

#endif
