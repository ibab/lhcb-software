#ifndef LIB_BBDECTREETRANSFORM_H
#define LIB_BBDECTREETRANSFORM_H 1


//#include "GaudiKernel/MsgStream.h"
#include <strstream>
#include "Kernel/IParticleDictTool.h"
#include "TMVA/Reader.h"

#include "MVADictTools/Options.h"

typedef IParticleDictTool::DICT DICT;

/** @class BBDecTreeTransform
 *  Policy class to be used by the DictTransform template
 *  Implementing the BBDT reader backed
 *  Moved from Phys/BBDecTreeTool
 *
 *  @author Egor Khairullin
 *  @date   2015-03-11
 */

class BBDecTreeTransform
{

public:

  typedef std::map<std::string, std::string> optmap;

private:

  bool m_setup_success;
  bool m_keep_all_vars;
  std::string m_bbdt_file;
  std::string m_name;
  std::string m_default_path;

  // BBDT bits
  int m_ntrees; ///< number of trees used in training - it's just multiplier
  std::vector<std::vector<double> > m_splits; ///< variable split points
  std::vector<unsigned short int> m_values; ///< response values

  bool m_debug;

private:

  // Helper Functions
  void readWeightsFile(std::ostream&);
  void setupReader(std::ostream&);
  bool parseOpts(const optmap&, std::ostream&);

  /// utility method to obtain index to m_values
  int getIndex(const DICT&) const;

  /// utility method to obtain split index for single variable
  int getVarIndex(int varIndex, double value) const;

public:

  std::vector<std::string> m_variables; /// variables needed by the classifier

public:

  BBDecTreeTransform();
  ~BBDecTreeTransform();

  // the policy methods neede for collaboration with DictTransform
  bool Init(const optmap& options, std::ostream& info, const bool debug = false );
  bool operator()(const DICT& in, DICT& out) const;
  bool checkWeightsFile(std::ostream& info);

};

#endif // LIB_BBDECTREETRANSFORM_H
