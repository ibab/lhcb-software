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
  if (exist<LHCb::RecHeader>(evtSvc(), m_recHeaderLoc)) {
    LHCb::RecHeader *h = get<LHCb::RecHeader>(evtSvc(), m_recHeaderLoc);
    return h->gpsTime() * 1000;
  }
  // Otherwise return '0'
  return Gaudi::Time::epoch();
}

#include "GaudiKernel/ToolFactory.h"
DECLARE_TOOL_FACTORY(RecEventTime)
