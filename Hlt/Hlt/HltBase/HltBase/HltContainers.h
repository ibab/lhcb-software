// $Id: HltContainers.h,v 1.1.1.1 2006-09-26 12:57:36 cattanem Exp $
#ifndef HLTBASE_HLTCONTAINERS_H 
#define HLTBASE_HLTCONTAINERS_H 1

#include "Event/Track.h"
#include "Event/RecVertex.h"

namespace Hlt 
{

  typedef std::vector< LHCb::Track* >       TrackContainer;
  typedef std::vector< LHCb::RecVertex* >   VertexContainer;

};
#endif // HLTBASE_HLTCONTAINERS_H
