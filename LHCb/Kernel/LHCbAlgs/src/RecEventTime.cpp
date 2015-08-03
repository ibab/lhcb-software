#include "RecEventTime.h"

#include "Event/RecHeader.h"

RecEventTime::RecEventTime(const std::string & type, const std::string & name,
                           const IInterface *parent) :
  base_class(type, name, parent)
{
  declareProperty("RecHeaderLocation",
                  m_recHeaderLoc = LHCb::RecHeaderLocation::Default,
                  "Alternative location for RecHeader.");
}

RecEventTime::~RecEventTime()
{
}

Gaudi::Time RecEventTime::getTime() const
{
  // If the rec header is available, return its event time
  LHCb::RecHeader *h = getIfExists<LHCb::RecHeader>(evtSvc(), m_recHeaderLoc);
  if ( NULL != h ) {
    return h->gpsTime() * 1000;
  }
  // Otherwise return '0'
  return Gaudi::Time::epoch();
}

DECLARE_TOOL_FACTORY(RecEventTime)
