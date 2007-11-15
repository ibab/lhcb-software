// $Id: HltTypes.h,v 1.3 2007-11-15 14:34:29 graven Exp $
#ifndef HLTBASE_HLTTYPES_H 
#define HLTBASE_HLTTYPES_H 1

#include <vector>
#include "HltBase/EFunctions.h"
#include "HltBase/EDictionary.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ContainedObject.h"
// #include "Event/Track.h"
// #include "Event/RecVertex.h"
// #include "Event/L0CaloCandidate.h"

namespace LHCb 
{
  class Track;
  class RecVertex;
  class L0CaloCandidate;
  class L0MuonCandidate;
}

namespace Hlt 
{

  class ISizeHolder : public ContainedObject {
  public:
    virtual size_t size() const = 0;
  };
 
  template <class CONT> 
  class DataHolder : public DataObject , public ContainedObject {
  public:
    DataHolder(CONT& cont):m_cont(&cont) {};
    virtual ~DataHolder() {};
    CONT& object() {return *m_cont;}
    const CONT& object() const {return *m_cont;}
  protected:
    CONT* m_cont;    
  };

  template <class CONT> 
  class DataSizeHolder : public DataObject, public ISizeHolder {
  public:
    DataSizeHolder(CONT& cont):m_cont(&cont) {};
    virtual ~DataSizeHolder() {};
    CONT& object() {return *m_cont;}
    const CONT& object() const {return *m_cont;}
    size_t size() const {return m_cont->size();}
  protected:
    CONT* m_cont;    
  };
  
}


namespace Hlt 
{
  typedef std::vector< LHCb::Track* >       TrackContainer;
  typedef std::vector< LHCb::RecVertex* >   VertexContainer;
  // typedef std::vector< LHCb::L0CaloCandidate* >   L0CaloCandidateContainer;
  // typedef std::vector< LHCb::L0MuonCandidate* >   L0MuonCandidateContainer;

  typedef Estd::dictionary Configuration;
  
  typedef Estd::filter<double> Filter;

  typedef Estd::function<LHCb::Track>     TrackFunction;  
  typedef Estd::function<LHCb::RecVertex> VertexFunction;
  typedef Estd::filter<LHCb::Track>       TrackFilter;
  typedef Estd::filter<LHCb::RecVertex>   VertexFilter;
  
  typedef Estd::bifunction<LHCb::Track, LHCb::Track>         TrackBiFunction;  
  typedef Estd::bifunction<LHCb::RecVertex, LHCb::RecVertex> VertexBiFunction;
  typedef Estd::bifunction<LHCb::Track, LHCb::RecVertex>     TrackVertexBiFunction;

  typedef Hlt::DataHolder< Hlt::Configuration >  HolderConfiguration;
  typedef Hlt::DataSizeHolder< TrackContainer >  HolderTrackContainer;
  typedef Hlt::DataSizeHolder< VertexContainer > HolderVertexContainer;

};
#endif // HLTBASE_HLTCONTAINERS_H
