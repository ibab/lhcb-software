// $Id: HltContainers.h,v 1.2 2007-02-14 17:23:51 cattanem Exp $
#ifndef HLTBASE_HLTCONTAINERS_H 
#define HLTBASE_HLTCONTAINERS_H 1

#include <vector>

//forward declarations
namespace LHCb {
  class Track;
  class RecVertex;
};

namespace Hlt 
{
  typedef std::vector< LHCb::Track* >       TrackContainer;
  typedef std::vector< LHCb::RecVertex* >   VertexContainer;
};
#endif // HLTBASE_HLTCONTAINERS_H
