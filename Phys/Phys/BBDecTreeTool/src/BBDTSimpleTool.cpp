// $Id: $
// ============================================================================
#include "BBDTSimpleTool.h"
#include <algorithm>
// ============================================================================
BBDTSimpleTool::BBDTSimpleTool(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : base_class(type,name,parent), 
    m_dictool_name( "LoKi::Hybrid::DictOfFunctors" ),
    m_hybrid_dicttool(0)
{
  // declare configurable properties
  declareProperty("Cuts", m_cuts, "Simple tree of cuts");
  declareProperty("ParticleDictTool" , m_dictool_name,
                  "Type/Name for C++/Python hybrid dictionary of functors tool");
}
// ===========================================================================
StatusCode BBDTSimpleTool::initialize()
{
  // initialize the base class  (the first action)
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  // Get the LoKi::Hybrid::DictOfFunctors tool that manages the interactions
  // with the hybrid functors.
  m_hybrid_dicttool = tool<IParticleDictTool>(m_dictool_name, this);

  // display cuts
  if ( msgLevel(MSG::DEBUG) )
  {
    std::vector<std::map<std::string,std::pair<double,double> > >
        ::const_iterator iter = m_cuts.begin();
    debug() << "Initialized w/ Cuts = [";
    while(iter != m_cuts.end()){
      debug() << "(";
      std::map<std::string,std::pair<double,double> >::const_iterator it
        = iter->begin();
      while(it != iter->end()){
        double min_cut = it->second.first;
        double max_cut = it->second.second;
        if(min_cut > 0)
          debug() << "(" << it->first << " > " << min_cut << ")";
        if(max_cut > 0){
          if(min_cut > 0) debug() << "&";
            debug() << "(" << it->first << " < " << max_cut << ")";
        }
        it++;
        if(it != iter->end()) debug() << "&";
      }
      debug() << ")";
      iter++;
      if(iter != m_cuts.end()) debug() << "|";
    }
    debug() << "]" << endmsg;
  }

  return sc;
}


// ===========================================================================
StatusCode BBDTSimpleTool::finalize() {
  // declare configurable properties
  return GaudiTool::finalize();
}


// ============================================================================
bool BBDTSimpleTool::operator()(const LHCb::Particle* p) const {

  if(0 == p) {
    Error("LHCb::Particle* points to NULL, return false");
    return false ;
  }

  // DICT is a typedef of GaudiUtils::VectorMap< std::string, double > 
  IParticleDictTool::DICT vals;   // Map of functor values keyed by var name

  // Evaluate the functors
  StatusCode sc = m_hybrid_dicttool->fill(p, vals);
  if( !sc.isSuccess() )
  {
    Error("Unable to fill map of functor values, return false");
    return false ;
  }

  bool passAny = false;
  std::vector<std::map<std::string,std::pair<double,double> > >
    ::const_iterator iter = m_cuts.begin();
  while(!passAny && (iter != m_cuts.end()))
  {
    bool passSet = true;
    std::map<std::string,std::pair<double,double> >::const_iterator it
        = iter->begin();
    while(passSet && (it != iter->end()))
    {
      // Consider a harder failure state.
      IParticleDictTool::DICT::iterator pval = vals.find( it->first );
      if( pval == vals.end() )
      {
        Error("Unable to find value for " + it->first + " in dictionary of functor values, returning false.");
        passSet = false;
        break;
      }

      double min_cut = it->second.first;
      double max_cut = it->second.second;

      double value = pval->second;

      // The use of negative values to indicate no cut can no longer work
      //   for a generalized variable handler.
      // As a (hopefully) temporary generalization to remain compatible with
      //  the original function, do not apply an upper limit cut if the upper
      //  limit is less than the lower limit.
      // Always apply a lower limit.  Since the original variables are all
      //   positive definite, a negative lower limit is equivalent to no cut.
      if(value < min_cut) passSet = false;
      if(max_cut > min_cut && value > max_cut) passSet = false;
      it++;
    }
    passAny = passSet;

    iter++;
  }
  return passAny;
}

// ============================================================================
/// declare & implement the factory
DECLARE_TOOL_FACTORY(BBDTSimpleTool)
// ============================================================================

#include "GaudiKernel/ParsersFactory.h"

namespace Gaudi {
  namespace Parsers {
    StatusCode parse(std::vector<std::map<std::string,
                     std::pair<double,double> > >& result,
                     const std::string& input)
    {
      return parse_(result, input);
    }
  }
}
// ============================================================================
