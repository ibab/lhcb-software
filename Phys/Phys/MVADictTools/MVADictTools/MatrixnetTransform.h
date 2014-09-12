#ifndef LIB_MATRIXNETTRANSFORM_H 
#define LIB_MATRIXNETTRANSFORM_H 1


#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include "Kernel/IParticleDictTool.h"
#include <string>
#include <vector>
#include <map>

#include "Options.h"

// typedef std::map<std::string, float> DICT;
typedef IParticleDictTool::DICT DICT;
/** @class MatrixnetTransform
 *  Policy class to be used by the DictTransform template
 *  Implementing the Matrixnet Reader backend
 *
 *  @author Andrey Ustyuzhanin, Egor Khairullin
 *  @date   2014-01-31
 */

class MatrixnetTransform {
public:
  typedef std::map<std::string, std::string> optmap;

private:

  bool m_setup_success;
  bool m_keep_all_vars;
  std::string m_matrixnet_file;
  std::string m_name;
  std::string m_default_path;

  // Matrixnet bits
  std::string m_formula;

  bool m_debug;

private:

  // Helper Functions
  void readWeightsFile(std::ostream&);
  bool parseOpts(const optmap&, std::ostream&);

public:

  std::vector<std::string> m_variables; /// variables needed by the classifier
  MatrixnetTransform();
  ~MatrixnetTransform();

  bool checkWeightsFile(std::ostream& info);

  bool Init(const optmap& options, std::ostream& info, const bool debug = false );

  bool operator()(const DICT& in, DICT& out) const;
};

#endif // LIB_MATRIXNETTRANSFORM_H
