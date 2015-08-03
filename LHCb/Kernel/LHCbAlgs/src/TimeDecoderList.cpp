#include "TimeDecoderList.h"

// Constructor
TimeDecoderList::TimeDecoderList(const std::string & type, const std::string & name, const IInterface *parent):
  base_class(type, name, parent)
{
  declareProperty("Decoders", m_decoderNames,
                  "List of IEventTimeDecoder tools to use");
}

TimeDecoderList::~TimeDecoderList()
{
}

StatusCode TimeDecoderList::initialize()
{
  StatusCode sc = base_class::initialize();
  if (sc.isFailure()) return sc;

  m_decoders.clear();
  for (DecoderNamesType::const_iterator d = m_decoderNames.begin(); d != m_decoderNames.end(); ++d) {
    IEventTimeDecoder *p = tool<IEventTimeDecoder>(*d, this);
    if (p) m_decoders.push_back(p);
    else {
      error() << "Cannot retrieve " << *d << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

Gaudi::Time TimeDecoderList::getTime() const
{
  Gaudi::Time t;
  for (DecodersType::const_iterator d = m_decoders.begin(); d != m_decoders.end(); ++d) {
    if ( (t = (*d)->getTime()).ns() )
      return t;
  }
  return Gaudi::Time::epoch();
}

DECLARE_TOOL_FACTORY(TimeDecoderList)
