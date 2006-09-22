// $Id: HltContainers.h,v 1.1 2006-09-22 11:36:07 hernando Exp $
#ifndef HLTCOMMON_HLTCONTAINERS_H 
#define HLTCOMMON_HLTCONTAINERS_H 1

// #include "HltCommon/EContainer.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"

namespace Hlt 
{

  typedef std::vector< LHCb::Track* >       TrackContainer;
  typedef std::vector< LHCb::RecVertex* >   VertexContainer;

};
#endif // HLTCOMMON_HLTCONTAINERS_H
