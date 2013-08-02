#ifndef TMVACLASSIFIERTOOL_H
#define TMVACLASSIFIERTOOL_H


//#include "GaudiKernel/MsgStream.h"
#include <strstream>
#include "Kernel/IParticleDictTool.h"
#include "TMVA/Reader.h"



#include "Options.h"

/** @class TMVATransform 
 *  Policy class to be used by the DictTransform template
 *  Implementing the TMVA Reader backend
 *
 *  @author Sam Hall
 *  @date   2013-07-29
 */

class TMVATransform {
public:
  typedef std::map<std::string, std::string> optmap;

private:
  bool m_setup_success;
  bool m_keep_all_vars;
  std::string m_reader_opts;
  std::string m_weightfile;
  std::string m_name;
  std::string m_branchname;
  std::vector<std::string> m_spectator;
  //std::vector<double> m_values; // because of const this is defined within operator()
 
  //std::map<std::string, OptionBase*> m_options;

  // TMVA bits
  TMVA::Reader* m_reader;

  // Helper Functions
  void readWeightsFile(std::ostream&);
  void setupReader(std::ostream&);
  bool parseOpts(optmap&, std::ostream&);


public:
  std::vector<std::string> m_variables; /// variables needed by the classifier
  TMVATransform();
  ~TMVATransform();

  // the policy methods neede for collaboration with DictTransform
  bool Init(optmap options, std::ostream& info);
  bool operator()(const IParticleDictTool::DICT& in, IParticleDictTool::DICT& out) const;

};

#endif // TMVACLASSIFIERTOOL_H
