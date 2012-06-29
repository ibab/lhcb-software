// $Id: $
// ============================================================================
#include "BBDTSimpleTool.h"
#include "LoKi/IHybridFactory.h"
#include "Kernel/GetIDVAlgorithm.h"
// ============================================================================
BBDTSimpleTool::BBDTSimpleTool(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : base_class(type,name,parent), m_vars(0)
{
  // declare configurable properties
  declareProperty("Cuts", m_cuts, "Simple tree of cuts");
}
// ===========================================================================
StatusCode BBDTSimpleTool::initialize()
{
  // initialize the base class  (the first action)
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // Get DV Alg
  const IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm(contextSvc(),this);
  if ( !dva )
  {
    return Error("Couldn't get parent DVAlgorithm", StatusCode::FAILURE);
  }

  // get tools
  const IDistanceCalculator * dist = dva->distanceCalculator();

  m_vars = new BBDTVarHandler(dva, dist);

  // configure the BBDT var handler to use only K+
  LoKi::PhysTypes::Cut cut(LoKi::Cuts::ABSID == "K+");
  m_vars->setPIDs(cut);
  std::vector<bool> use(7,true); // use 1st 7 vars in the handler
  if(!m_vars->initialize(use))
    return Error("Couldn't init BBDTVarHandler", StatusCode::FAILURE);

  // display cuts
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
  return sc;
}
// ===========================================================================
StatusCode BBDTSimpleTool::finalize() {
  // declare configurable properties
  if (m_vars) {
    delete m_vars;
    m_vars = 0;
  }
  return GaudiTool::finalize();
}
// ============================================================================
bool BBDTSimpleTool::operator()(const LHCb::Particle* p) const {

  if(0 == p) {
    Error("LHCb::Particle* points to NULL, return false");
    return false ;
  }
  if(!m_vars->set(p)) return false;

  std::vector<std::map<std::string,std::pair<double,double> > >
    ::const_iterator iter = m_cuts.begin();
  while(iter != m_cuts.end()){
    if(m_vars->cut(*iter)) return true;
    iter++;
  }
  return false;
}
// ============================================================================
/// declare & implement the factory
DECLARE_TOOL_FACTORY(BBDTSimpleTool);
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
