// $Id: $
// ============================================================================
#include "BBDecTreeTool.h"
#include "GaudiKernel/System.h"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem.hpp"
// ============================================================================
std::string SubstituteEnvVarInPath(const std::string& in) {
  /* cp'd directly from $L0MUONROOT/src/component/SubstituteEnvVarInPath.cpp */
  boost::filesystem::path path(in);
  boost::filesystem::path::iterator i = path.begin();
  boost::filesystem::path out;
  while (i != path.end()) {
    if ( (*i)[0]=='$' ) {
      std::string x = System::getEnv( i->c_str()+1 );
      out /= x.empty() ? *i : x ;
    } else {
      out /= *i;
    }
    ++i;
  }
  return out.string();
}
// ============================================================================
BBDecTreeTool::BBDecTreeTool(const std::string& type, const std::string& name, 
			     const IInterface* parent) 
  : base_class(type,name,parent), m_threshold(-1.0), m_ntrees(-1), 
    m_dist(0), m_dva(0), m_vars(){
  // declare configurable properties
  declareProperty("Threshold", m_threshold, "response threshold (cut)");
  declareProperty("ParamFile", m_paramFile, "parameter file (full path)");
}
// ============================================================================
StatusCode BBDecTreeTool::readError(const std::string &msg) const {
  Error("Problem with file " + m_paramFile + ": " + msg + ".");
  return StatusCode::FAILURE;
}
// ===========================================================================
StatusCode BBDecTreeTool::initialize() {

  // initialize the base class  (the first action)
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc; 

  // get tools and algs
  m_dist = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
  m_dva = Gaudi::Utils::getDVAlgorithm(contextSvc());
  if (0 == m_dva) 
    return Error("Couldn't get parent DVAlgorithm", StatusCode::FAILURE);  
 
  // read in parameters
  if(!m_paramFile.empty() && m_paramFile[0] != '/' && m_paramFile[0]!='$')  
    m_paramFile = "$PARAMFILESROOT/data/" + m_paramFile;
  std::string fnam = SubstituteEnvVarInPath(m_paramFile);
  std::ifstream inFile(fnam.c_str());
  if(!inFile.is_open()) return this->readError("failed to open file");
  unsigned int nvar,index, value;
  double dvalue;
  std::string var_name;
  // number of variables 
  inFile >> nvar;
  if(nvar != m_vars.numVars()) 
    return this->readError("unsupported number of variables");
  m_splits.resize(nvar);
  // variable names (currently not used)
  for(unsigned int v = 0; v < nvar; v++){
    if(!(inFile >> var_name))
      return this->readError("error reading in variable names");
  }
  // number of splits for each variable
  unsigned int numSplits = 1;
  for(unsigned int v = 0; v < nvar; v++) {
    if(!(inFile >> value)) 
      return this->readError("error reading no. of splits");
    m_splits[v].resize(value);
    numSplits *= value;
  }
  // split values for each variable
  for(unsigned int v = 0; v < nvar; v++) {
    unsigned int size = m_splits[v].size();
    for(unsigned int s = 0; s < size; s++){
      if(!(inFile >> dvalue)) return this->readError("error reading splits");
      m_splits[v][s] = dvalue;
    }
  }
  // number of trees
  if(!(inFile >> m_ntrees)) 
    return this->readError("error reading no. of trees");
  // actual values  
  m_values.resize(numSplits);
  while(inFile >> index >> value){
    if(index > numSplits) return this->readError("error reading values");
    m_values[index] = (unsigned short int)value;
  }
  inFile.close();

  // print info
  info() << "Initialized w/ Threshold = "  << m_threshold << ", ParamFile = " 
	 << m_paramFile << " -> " << fnam  << " (" << m_ntrees << " trees," 
	 << nvar << " vars," << numSplits << " splits)." << endmsg;

  return StatusCode::SUCCESS ;
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
  if(size != m_vars.numVars()) return -1;
  int ind_mult = 1, index = 0;
  for(int v = size-1; v >= 0; v--){
    if(v < (int)size-1) ind_mult *= m_splits[v+1].size();
    index += this->getVarIndex(v,m_vars[v])*ind_mult;
  }
  return index;
}
// ============================================================================
bool BBDecTreeTool::operator()(const LHCb::Particle* p) const {

  if(0 == p) {
    Error("LHCb::Particle* points to NULL, return false");
    return false ;
  }
  if(!m_vars.set(p,m_dva,m_dist)) return false;

  // get response
  int index = this->getIndex();
  if(index < 0 || index >= (int)m_values.size()){
    Error("BBDecTreeTool tool is misconfigured!");
    return false;
  }
  return m_values[index]/(double)m_ntrees > m_threshold;
}
// ============================================================================
/// declare & implement the factory 
DECLARE_TOOL_FACTORY(BBDecTreeTool);
// ============================================================================
