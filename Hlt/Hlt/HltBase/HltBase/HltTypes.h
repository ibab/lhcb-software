// $Id: HltTypes.h,v 1.7 2008-06-28 08:16:18 graven Exp $
#ifndef HLTBASE_HLTTYPES_H 
#define HLTBASE_HLTTYPES_H 1

#include <vector>
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ContainedObject.h"
#include "HltBase/EFunctions.h"
#include "HltBase/EDictionary.h"
#include "HltBase/ERelations.h"
#include "HltBase/HltSelection.h"
#include "HltBase/IFunctionFactory.h"
#include "HltBase/IBiFunctionFactory.h"

namespace LHCb {
  class Track;
  class RecVertex;
  class L0CaloCandidate;
  class L0MuonCandidate;
}

namespace AIDA {
  class IHistogram1D;
}

namespace Hlt {
  template <class OBJ>
  class DObj : public OBJ, public DataObject {};
  
  typedef std::vector<Hlt::Selection*> Selections;
  typedef std::vector<Hlt::Selection*>::iterator SelectionIterator;

  typedef TrackSelection     TrackContainer;
  typedef VertexSelection    VertexContainer;

  typedef AIDA::IHistogram1D Histo;

  typedef DObj<zen::dictionary>  Configuration;
  typedef DObj<zen::IntRelation> IntRelation;
  
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
