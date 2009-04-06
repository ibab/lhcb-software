// $Id: WriteSTStatusConditions.h,v 1.1 2009-04-06 07:50:48 mneedham Exp $
#ifndef WriteSTStatusConditions_H
#define WriteSTStatusConditions_H 1

#include "Kernel/STAlgBase.h"

/** @class WriteSTAlignmentConditions WriteSTAlignmentConditions.h
 *
 *  Class for writing ST status conditions
 *
 *  @author M.Needham
 *  @date   2/4/2009
 */

#include <fstream>
#include <iomanip>

class WriteSTStatusConditions : public ST::AlgBase {

public:
 
  /// constructer
  WriteSTStatusConditions(const std::string& name, ISvcLocator *svcloc );

  /// destructer
  virtual ~WriteSTStatusConditions();

  /// initialize
  virtual StatusCode initialize();

  /// execute
  StatusCode execute();

  // finalize
  StatusCode finalize();

private:

  std::string m_footer;
  std::string m_startTag;

  std::string footer() const;
  std::string header(const std::string& conString) const;
  std::string strip(const std::string& conString) const;
  void replaceChars(std::string& conString) const;
  void replace(std::string& conString,std::string in, std::string out ) const;

  std::string m_outputFileName;
  std::ofstream m_outputFile;
  bool m_removeCondb;
  unsigned int m_precision;
  unsigned int m_depth;

};

#endif // WriteAlignment conditions
