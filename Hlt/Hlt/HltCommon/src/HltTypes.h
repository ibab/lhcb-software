// $Id: HltTypes.h,v 1.3 2009-10-12 18:13:52 graven Exp $
#ifndef HLTBASE_HLTTYPES_H 
#define HLTBASE_HLTTYPES_H 1

#include "HltBase/IFunctions.h"
#include "HltBase/IFunctionFactory.h"
#include "HltBase/IBiFunctionFactory.h"

namespace LHCb {
  class Track;
  class RecVertex;
}

namespace Hlt {
  
  
  typedef zen::filter<double> Filter;
  
  typedef zen::function<LHCb::Track>     TrackFunction;  
  typedef zen::function<LHCb::RecVertex> VertexFunction;
  typedef zen::filter<LHCb::Track>       TrackFilter;
  typedef zen::filter<LHCb::RecVertex>   VertexFilter;
  
  typedef zen::bifunction<LHCb::Track, LHCb::Track>         TrackBiFunction;  
  typedef zen::bifunction<LHCb::RecVertex, LHCb::RecVertex> VertexBiFunction;
  typedef zen::bifunction<LHCb::Track, LHCb::RecVertex>     TrackVertexBiFunction;

  typedef zen::bifilter<LHCb::Track, LHCb::Track>         TrackBiFilter;  
  typedef zen::bifilter<LHCb::RecVertex, LHCb::RecVertex> VertexBiFilter;
  typedef zen::bifilter<LHCb::Track, LHCb::RecVertex>     TrackVertexBiFilter;

};

typedef IFunctionFactory<LHCb::Track> ITrackFunctionFactory;
typedef IFunctionFactory<LHCb::RecVertex> IVertexFunctionFactory;
typedef IBiFunctionFactory<LHCb::Track,LHCb::Track> ITrackBiFunctionFactory;

#endif
