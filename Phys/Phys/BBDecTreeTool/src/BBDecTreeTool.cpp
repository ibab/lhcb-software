#include <sstream>
#include "BBDecTreeTool.h"
#include "GaudiKernel/System.h"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem.hpp"
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
BBDecTreeTool::BBDecTreeTool( const std::string& type, 
                              const std::string& name,
                              const IInterface* parent )
  : base_class(type,name,parent), 
    m_threshold(-1.0), 
    m_key(-1), 
    m_dictool_name( "LoKi::Hybrid::DictOfFunctors" ),
    m_ntrees(-1),
    m_var_names(),
    m_hybrid_dicttool(0)
{
  // declare configurable properties
  declareProperty("Threshold", m_threshold, "response threshold (cut)");
  declareProperty("ParamFile", m_paramFile, "parameter file (full path)");
  declareProperty("ANNSvcKey", m_key, "extrainfo key");
  declareProperty("ParticleDictTool" , m_dictool_name,
                        "Type/Name for C++/Python hybrid dictionary of functors tool");
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

  // Get the LoKi::Hybrid::DictOfFunctors tool that manages the interactions
  // with the hybrid functors.
  m_hybrid_dicttool = tool<IParticleDictTool>(m_dictool_name, this);

  // read in parameters
  if( !m_paramFile.empty() && m_paramFile[0] != '/'
                                && m_paramFile[0] != '.' && m_paramFile[0]!='$')
    m_paramFile = "$PARAMFILESROOT/data/" + m_paramFile;
  std::string fnam = SubstituteEnvVarInPath(m_paramFile);
  std::ifstream inFile(fnam.c_str());
  if(!inFile.is_open()) return this->readError("failed to open file");

  unsigned int nvar(0),index(0),value(0);
  double dvalue(0);
  inFile >> nvar;               // number of variables
  m_var_names.resize(nvar);
  m_splits.resize(nvar);

  // variable names
  for ( unsigned int v = 0; v < nvar; ++v )
  {
    if(!(inFile >> m_var_names[v]))
      return this->readError("error reading in variable names");
  }

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
int BBDecTreeTool::getIndex( const IParticleDictTool::DICT &vars ) const {
  unsigned int size = m_splits.size();

  int ind_mult = 1, index = 0;
  for(int v = size-1; v >= 0; v--){
    if(v < (int)size-1) ind_mult *= m_splits[v+1].size();

    IParticleDictTool::DICT::iterator pval = vars.find( m_var_names[v] );
    if( pval == vars.end() )
    {
      Error("Unable to find value for " + m_var_names[v] + " in dictionary of functor values, returning invalid index.");
      index = -1;
      break;
    }

    index += this->getVarIndex(v, pval->second)*ind_mult;
  }
  return index;
}


// ============================================================================
bool BBDecTreeTool::operator()(const LHCb::Particle* p) const {

  // TODO:  consider whether some of these error conditions should be fatal.
  if(0 == p) {
    Error("LHCb::Particle* points to NULL, return false");
    return false ;
  }

  // DICT is a typedef of GaudiUtils::VectorMap< std::string, double > 
  IParticleDictTool::DICT vals;   // Map of functor values keyed by var_name

  // Evaluate the functors
  StatusCode sc = m_hybrid_dicttool->fill(p, vals);
  if( !sc.isSuccess() )
  {
    Error("Unable to fill map of functor values, return false");
    return false ;
  }

  // get response
  int index = this->getIndex( vals );
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

  // Debugging.  Consider putting this in an optionally compiled block.
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Input variable values:\n";
    unsigned int size = m_splits.size();
    for(int v = size-1; v >= 0; v--)
      debug() << "        " << m_var_names[v] << " = " << vals[m_var_names[v]] << "\n";

    debug() << endmsg;
    debug() << "Index = " << index << ";   Response/ ntrees = "
            << m_values[index] << " / " << m_ntrees << " = " << response
            << ";  Threshold:  " << m_threshold << endmsg;
  }

  return response > m_threshold;
}
// ============================================================================
/// declare & implement the factory
DECLARE_TOOL_FACTORY(BBDecTreeTool)
// ============================================================================
