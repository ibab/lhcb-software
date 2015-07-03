#include "MVADictTools/BBDecTreeTransform.h"
#include "Utils.h"

#include <fstream>
#include <alloca.h>
#include <math.h>


BBDecTreeTransform::BBDecTreeTransform()
  : m_setup_success(false)
  , m_keep_all_vars(true)
  , m_bbdt_file("")
  , m_name("")
  , m_ntrees(-1)
  , m_splits(0)
  , m_values(0)
  , m_debug(false)
  , m_variables(0)
{
  return;
}

BBDecTreeTransform::~BBDecTreeTransform() {}


bool
BBDecTreeTransform::Init(const optmap& options, std::ostream& info, const bool debug ){
  m_debug = debug;
  m_setup_success = parseOpts(options, info);
  if (!m_setup_success) { return false ; }

  readWeightsFile(info);
  if (!m_setup_success) { return false ; }

  return true;
}


bool
BBDecTreeTransform::operator()(const DICT& in, DICT& out) const {
  // Check initialization
  if (!m_setup_success) {
    return false;
  }
  int index = this->getIndex(in);
  if(index < 0 || index >= (int)m_values.size()){
    return false;
  }

  double response = m_values[index]/(double)m_ntrees;
  if (m_keep_all_vars) {
    out = in;
  } else {
    out.clear();
  }
  out.insert(out.end(), m_name, response);
  return true;
}


bool
BBDecTreeTransform::checkWeightsFile(std::ostream& info) {
  std::ifstream fin(resolveEnv(m_bbdt_file.c_str()));
  // Check existence of WeightFile: locally
  if (fin.good()) {
    return true;
  }
  fin.close();
  // else ERROR
  if (m_bbdt_file == "") {
    info << "ERROR  ";
    info << "WeightFile not given." << std::endl;
  } else {
    info << "WeightFile \"" << m_bbdt_file << "\" not found." << std::endl;
    info << "  BBDecTree will not be run.  The output will be 0 for each event." << std::endl;
  }
  return false;
}


void BBDecTreeTransform::readWeightsFile(std::ostream& info) {
  m_setup_success = false;
  if (!checkWeightsFile(info)) {
    return;
  }
  std::ifstream inFile(resolveEnv(m_bbdt_file.c_str()));
  unsigned int nvar = 0;
  unsigned int index = 0;
  unsigned int value = 0;
  double dvalue = 0;
  inFile >> nvar;               // number of variables
  m_variables.resize(nvar);
  m_splits.resize(nvar);

  // variable names
  for (unsigned int v = 0; v < nvar; ++v) {
    if (!(inFile >> m_variables[v])) {
      info << "error reading in variable names" << std::endl;
      return;
    }
  }

  // number of splits for each variable
  unsigned int numSplits = 1;
  for (unsigned int v = 0; v < nvar; ++v) {
    if (!(inFile >> value)) {
      info << "error reading no. of splits" << std::endl;
      return;
    }
    m_splits[v].resize(value);
    numSplits *= value;
  }

  // split values for each variable
  for (unsigned int v = 0; v < nvar; ++v) {
    const unsigned int size = m_splits[v].size();
    for (unsigned int s = 0; s < size; ++s) {
      if (!(inFile >> dvalue)) {
        info << "error reading splits" << std::endl;
        return;
      }
      m_splits[v][s] = dvalue;
    }
  }

  // number of trees
  if(!(inFile >> m_ntrees)) {
    info << "error reading no. of trees" << std::endl;
    return;
  }

  // actual values
  m_values.resize(numSplits);
  while(inFile >> index >> value) {
    if(index > numSplits) {
      info << "error reading values" << std::endl;
      return;
    }
    m_values[index] = (unsigned short int)value;
  }
  inFile.close();

  // print info
  if (m_debug) {
    info << "Initialized w/ , ParamFile = "
            << m_bbdt_file << " (" << m_ntrees << " trees,"
            << nvar << " vars," << numSplits << " splits)." << std::endl;
  }
  m_setup_success = true;
  return;
}


bool BBDecTreeTransform::parseOpts(const optmap& options, std::ostream& info) {
  bool pass = true;
  Options parse(options);
  parse.add<std::string>("Name", "Name of output branch (Required)",
                         m_name, info);
  parse.add<std::string>("BBDecTreeFile", "File with bbdt formula",
                         m_bbdt_file, info);
  parse.add<bool>("KeepVars", "Keep input variables, \"1\" or \"0\"",
                  m_keep_all_vars, info, false);
  pass = parse.check(info);
  return pass;
}


int BBDecTreeTransform::getVarIndex(int varIndex, double value) const {
  if(value < m_splits[varIndex][0]) return 0;
  unsigned int size = m_splits[varIndex].size();
  for(unsigned int s = 1; s < size; s++){
    if(value < m_splits[varIndex][s]) return s-1;
  }
  return size-1;
}


int BBDecTreeTransform::getIndex(const DICT &vars) const {
  unsigned int size = m_splits.size();

  int ind_mult = 1, index = 0;
  for(int v = size-1; v >= 0; v--){
    if(v < (int)size-1) ind_mult *= m_splits[v+1].size();

    DICT::iterator pval = vars.find( m_variables[v] );
    if( pval == vars.end() )
    {
      index = -1;
      break;
    }

    index += this->getVarIndex(v, pval->second)*ind_mult;
  }
  return index;
}
