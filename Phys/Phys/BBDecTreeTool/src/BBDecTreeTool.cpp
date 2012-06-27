#include "BBDecTreeTool.h"
#include "GaudiKernel/System.h"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem.hpp"
#include "LoKi/IHybridFactory.h"
#include <Kernel/GetIDVAlgorithm.h>
// ============================================================================
std::string SubstituteEnvVarInPath(const std::string& in)
{
  /* cp'd directly from $L0MUONROOT/src/component/SubstituteEnvVarInPath.cpp */
  boost::filesystem::path path(in);
  boost::filesystem::path::iterator i = path.begin();
  boost::filesystem::path out;
  while (i != path.end()) 
  {
    if ( *(i->c_str())=='$' ) 
    {
      std::string x = System::getEnv( i->c_str()+1 );
      out /= x.empty() ? *i : x ;
    }
    else
    {
      out /= *i;
    }
    ++i;
  }
  return out.string();
}
// ============================================================================
BBDecTreeTool::BBDecTreeTool(const std::string& type, const std::string& name,
                             const IInterface* parent)
  : base_class(type,name,parent), 
    m_threshold(-1.0), 
    m_key(-1), 
    m_ntrees(-1),
    m_vars(0)
{
  // declare configurable properties
  declareProperty("Threshold", m_threshold, "response threshold (cut)");
  declareProperty("ParamFile", m_paramFile, "parameter file (full path)");
  declareProperty("ANNSvcKey", m_key, "extrainfo key");
  declareProperty("PIDs",m_pids, "PID names for daughter PT variables");
}
// ============================================================================
StatusCode BBDecTreeTool::readError(const std::string &msg) const 
{
  return Error("Problem with file " + m_paramFile + " : '" + msg + "'");
}
// ===========================================================================
StatusCode BBDecTreeTool::initialize() 
{
  // initialize the base class  (the first action)
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  IDistanceCalculator * dist
    = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);

  // Get DV Alg. *must* be private to do this
  const IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm(contextSvc(),this);
  if ( !dva ) 
  {
    return Error("Couldn't get parent DVAlgorithm", StatusCode::FAILURE);
  }
  m_vars = new BBDTVarHandler(dva, dist);

  // configure the BBDT var handler to use specified PIDs
  const unsigned int size = m_pids.size();
  if ( size > 0 )
  {
    LoKi::PhysTypes::Cut cut(LoKi::Cuts::ABSID == m_pids[0]);
    for ( unsigned int i = 1; i < size; ++i )
    {
      cut = (cut || (LoKi::Cuts::ABSID == m_pids[i]));
    }
    m_vars->setPIDs(cut);
  }

  // read in parameters
  if(!m_paramFile.empty() && m_paramFile[0] != '/' && m_paramFile[0]!='$')
    m_paramFile = "$PARAMFILESROOT/data/" + m_paramFile;
  std::string fnam = SubstituteEnvVarInPath(m_paramFile);
  std::ifstream inFile(fnam.c_str());
  if(!inFile.is_open()) return this->readError("failed to open file");
  unsigned int nvar,index, value;
  double dvalue;
  std::vector<std::string> var_names;
  // number of variables
  inFile >> nvar;
  var_names.resize(nvar);
  m_splits.resize(nvar);

  // variable names
  for ( unsigned int v = 0; v < nvar; ++v )
  {
    if(!(inFile >> var_names[v]))
      return this->readError("error reading in variable names");
  }
  if(!m_vars->initialize(var_names))
    return Error("Couldn't init BBDTVarHandler", StatusCode::FAILURE);

  // number of splits for each variable
  unsigned int numSplits = 1;
  for ( unsigned int v = 0; v < nvar; ++v )
  {
    if(!(inFile >> value))
      return this->readError("error reading no. of splits");
    m_splits[v].resize(value);
    numSplits *= value;
  }
  // split values for each variable
  for ( unsigned int v = 0; v < nvar; ++v ) 
  {
    const unsigned int size = m_splits[v].size();
    for ( unsigned int s = 0; s < size; ++s )
    {
      if(!(inFile >> dvalue)) return this->readError("error reading splits");
      m_splits[v][s] = dvalue;
    }
  }
  // number of trees
  if(!(inFile >> m_ntrees))
    return this->readError("error reading no. of trees");
  // actual values
  m_values.resize(numSplits);
  while(inFile >> index >> value)
  {
    if(index > numSplits) return this->readError("error reading values");
    m_values[index] = (unsigned short int)value;
  }
  inFile.close();

  // print info
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Initialized w/ Threshold = "  << m_threshold << ", ParamFile = "
            << m_paramFile << " -> " << fnam  << " (" << m_ntrees << " trees,"
            << nvar << " vars," << numSplits << " splits)." << endmsg;

  return sc;
}
// ===========================================================================
StatusCode BBDecTreeTool::finalize() {
  if (m_vars) {
    delete m_vars;
    m_vars = 0;
  }
  return GaudiTool::finalize();
}
// ============================================================================
int BBDecTreeTool::getVarIndex(int varIndex, double value) const {
  if(value < m_splits[varIndex][0]) return 0;
  unsigned int size = m_splits[varIndex].size();
  for(unsigned int s = 1; s < size; s++){
    if(value < m_splits[varIndex][s]) return s-1;
  }
  return size-1;
}
// ============================================================================
int BBDecTreeTool::getIndex() const {
  unsigned int size = m_splits.size();
  if(size != m_vars->numVars()) return -1;
  int ind_mult = 1, index = 0;
  for(int v = size-1; v >= 0; v--){
    if(v < (int)size-1) ind_mult *= m_splits[v+1].size();
    index += this->getVarIndex(v,(*m_vars)[v])*ind_mult;
  }
  return index;
}
// ============================================================================
bool BBDecTreeTool::operator()(const LHCb::Particle* p) const {

  if(0 == p) {
    Error("LHCb::Particle* points to NULL, return false");
    return false ;
  }
  if(!m_vars->set(p)) return false;

  // get response
  int index = this->getIndex();
  if(index < 0 || index >= (int)m_values.size()){
    Error("BBDecTreeTool tool is misconfigured!");
    return false;
  }
  double response = m_values[index]/(double)m_ntrees;
  if(m_key >= 0){
    if(p->hasInfo(m_key)){
      /* don't write msg b/c it's OK for STD, mu, e lines to write to same
         place since they report the same response */
    }
    else const_cast<LHCb::Particle*>(p)->addInfo(m_key, response);
  }
  return response > m_threshold;
}
// ============================================================================
/// declare & implement the factory
DECLARE_TOOL_FACTORY(BBDecTreeTool);
// ============================================================================
