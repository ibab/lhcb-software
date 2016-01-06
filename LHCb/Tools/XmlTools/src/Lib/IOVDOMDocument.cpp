#include "XmlTools/IOVDOMDocument.h"


IOVDOMDocument::IOVDOMDocument(xercesc::DOMDocument *dom):
  m_since(Gaudi::Time::epoch()),
  m_until(Gaudi::Time::max()),
  m_dom(dom)
{}

IOVDOMDocument::~IOVDOMDocument()
{
  if ( NULL != m_dom ) {
    m_dom->release();
  }
}

bool IOVDOMDocument::isValid() const
{
  return validSince() <= validTill();
}

bool IOVDOMDocument::isValid( const Gaudi::Time& time ) const
{
  return validSince() <= time &&  time < validTill();
}

const Gaudi::Time& IOVDOMDocument::validSince() const
{
  return m_since;
}

const Gaudi::Time& IOVDOMDocument::validTill() const
{
  return m_until;
}

void IOVDOMDocument::setValidity( const Gaudi::Time& since, const Gaudi::Time& until)
{
  m_since = since;
  m_until = until;
}

void IOVDOMDocument::setValiditySince( const Gaudi::Time& since)
{
  m_since = since;
} 

void IOVDOMDocument::setValidityTill( const Gaudi::Time& until)
{
  m_until = until;
}
