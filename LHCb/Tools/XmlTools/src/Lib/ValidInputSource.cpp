#include "XmlTools/ValidInputSource.h"

/// Constructor
ValidInputSource::ValidInputSource ( const XMLByte *const           srcDocBytes,
                                     const unsigned int             byteCount,
                                     const XMLCh *const             bufId,
                                     const bool                     adoptBuffer,
                                     xercesc::MemoryManager *const  manager ):
  xercesc::MemBufInputSource(srcDocBytes,byteCount,bufId,adoptBuffer,manager),
  m_since(Gaudi::Time::epoch()),
  m_until(Gaudi::Time::max())
{}

ValidInputSource::~ValidInputSource() {}

bool ValidInputSource::isValid() const
{
  return validSince() <= validTill();
}

bool ValidInputSource::isValid( const Gaudi::Time& time ) const
{
  return validSince() <= time &&  time < validTill();
}

const Gaudi::Time& ValidInputSource::validSince() const
{
  return m_since;
}

const Gaudi::Time& ValidInputSource::validTill() const
{
  return m_until;
}

void ValidInputSource::setValidity( const Gaudi::Time& since, const Gaudi::Time& until)
{
  m_since = since;
  m_until = until;
}

void ValidInputSource::setValiditySince( const Gaudi::Time& since)
{
  m_since = since;
} 

void ValidInputSource::setValidityTill( const Gaudi::Time& until)
{
  m_until = until;
}
