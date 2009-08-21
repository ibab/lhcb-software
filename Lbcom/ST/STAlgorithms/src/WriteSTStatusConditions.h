// $Id: WriteSTStatusConditions.h,v 1.3 2009-08-21 17:15:14 mneedham Exp $
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

class WriteSTStatusConditions : public ST::AlgBase {

public:
 
  /// constructer
  WriteSTStatusConditions(const std::string& name, ISvcLocator *svcloc );

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

  std::string m_outputFileName;
  std::ofstream m_outputFile;
  std::string m_author;
  std::string m_tag;
  std::string m_desc;
  bool m_removeCondb;
  unsigned int m_precision;
  unsigned int m_depth;

};

#endif // WriteAlignment conditions
