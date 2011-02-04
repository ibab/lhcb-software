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
  : base_class(type,name,parent), 
    m_nbody(-1), m_threshold(-1.0), m_ntrees(-1),
    m_SUMPT(LoKi::Cuts::SUMTREE(LoKi::Cuts::PT,"K+" == LoKi::Cuts::ABSID,0.0)),
    m_MINPT(LoKi::Cuts::MINTREE("K+" == LoKi::Cuts::ABSID,LoKi::Cuts::PT)),
    m_BPVIPCHI2(""), m_LoKiDistCalc(0), m_dva(0) {
  // declare configurable properties
  declareProperty("NBody", m_nbody, "2, 3 or 4-body?");
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
  m_LoKiDistCalc = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
  m_dva = Gaudi::Utils::getDVAlgorithm(contextSvc());
  if (0 == m_dva) 
    return Error("Couldn't get parent DVAlgorithm", StatusCode::FAILURE);  
 
  // read in parameters
  std::ifstream inFile(SubstituteEnvVarInPath(m_paramFile).c_str());
  if(!inFile.is_open()) return this->readError("failed to open file");
  unsigned int nvar,index, value;
  double dvalue;
  std::string var_name;
  // number of variables (must be 7 for now)
  inFile >> nvar;
  if(nvar != 7) return this->readError("unsupported number of variables");
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
  info() << "Initialized w/ NBody = " << m_nbody << ", Threshold = " 
	 << m_threshold << ", ParamFile = " << m_paramFile << " (" << m_ntrees 
	 << " trees," << nvar << " vars," << numSplits << " splits)." <<endmsg;

  return StatusCode::SUCCESS ;
}
// ============================================================================
bool BBDecTreeTool::keepRegion(const std::vector<double> &vars) const {

  double ptSum = vars[0],m = vars[1],doca = vars[2],mCor = vars[4],
    ptMin = vars[5],fdChi2 = vars[6];

  if(m > 7000 || doca > 0.2 || fdChi2 < 100 || ptSum < 3000) return false;  
  if(mCor < 4000 || mCor > 7000) return false;

  if(m_nbody == 2){
    if(ptMin > 3000) return true;
    if(ptSum > 10000) return true;
    if(m < 2000) return false;
    if(fdChi2 > 1000){
      if(ptSum > 5000 && ptMin > 750) return true;
    }
    else if(fdChi2 > 300){
      if(ptSum > 5500 && ptMin > 1000) return true;
    }
    return false;
  }
  else if(m_nbody == 3){
    if(ptSum < 4000 || doca > 0.1) return false;
    if(ptMin > 3000) return true;
    if(ptSum > 15000) return true;
    if(m < 3000) return false;
    if(mCor < 4500 || mCor > 6500) return false;
    if(fdChi2 > 300){
      if(ptMin > 2000) return true;
      if(ptSum > 6000 && ptMin > 1000) return true;
      if(ptSum > 7500 && ptMin > 750) return true;
    }
    if(fdChi2 > 1000){    
      if(ptMin > 1500) return true;
      if(ptSum > 7000 && ptMin > 600) return true;
    }
    return false;
  }
  else {
    if(ptSum < 4000 || doca > 0.1) return false;
    if(ptMin > 2000) return true;
    if(ptSum > 15000) return true;
    if(m < 3500) return false;
    if(fdChi2 > 300){
      if(ptMin > 1750) return true;
      if(ptSum > 8000 && ptMin > 600) return true;
    }
    if(fdChi2 > 1000){    
      if(ptMin > 1250) return true;
      if(ptSum > 7500 && ptMin > 550) return true;
    }
    return false;
  }
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
int BBDecTreeTool::getIndex(const std::vector<double> &vars) const {
  unsigned int size = m_splits.size();
  if(size != vars.size()) return -1;
  int ind_mult = 1, index = 0;
  for(int v = size-1; v >= 0; v--){
    if(v < (int)size-1) ind_mult *= m_splits[v+1].size();
    index += this->getVarIndex(v,vars[v])*ind_mult;
  }
  return index;
}
// ============================================================================
bool BBDecTreeTool::operator()(const LHCb::Particle* p) const {

  if(0 == p) {
    Error("LHCb::Particle* points to NULL, return false");
    return false ;
  }

  if(m_nbody < 2 || m_nbody > 4){
    Error("Not configured for 2, 3 or 4-body, return false");
    return false;
  }

  // get variables we need
  std::vector<double> vars(m_splits.size());  
  const LHCb::VertexBase* bpv = m_dva->bestPV(p);
  // SUMTREE(PT,'K+'==ABSID,0.0)
  vars[0] = m_SUMPT(p)/Gaudi::Units::MeV; 
  // M
  vars[1] = p->measuredMass()/Gaudi::Units::MeV; 
  // AMAXDOCA('LoKi::DistanceCalculator')
  const LoKi::ATypes::AFun AMAXDOCA = LoKi::Cuts::AMAXDOCA(m_LoKiDistCalc);
  vars[2] = AMAXDOCA(p->daughtersVector())/Gaudi::Units::mm; 
  // BPVIPCHI2()
  vars[3] = m_BPVIPCHI2(p); 
  // BPVCORRM
  const LoKi::Types::Fun BPVCORRM = LoKi::Cuts::CORRM(bpv);
  vars[4] = BPVCORRM(p)/Gaudi::Units::MeV; 
  // MINTREE('K+'==ABSID,PT)
  vars[5] = m_MINPT(p)/Gaudi::Units::MeV; 
  // BPVVDCHI2
  const LoKi::Types::Fun BPVVDCHI2 = LoKi::Cuts::VDCHI2(bpv);
  vars[6] = BPVVDCHI2(p); 

  // is candidate in factored-out keep region?
  if(this->keepRegion(vars)) return true;

  // get response
  int index = this->getIndex(vars);
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
