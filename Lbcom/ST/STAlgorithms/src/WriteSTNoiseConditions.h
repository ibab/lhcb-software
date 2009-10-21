// $Id: WriteSTNoiseConditions.h,v 1.1 2009-10-21 13:03:17 mneedham Exp $
#ifndef WriteSTNoiseConditions_H
#define WriteSTNoiseConditions_H 1

#include "Kernel/STAlgBase.h"

/** @class WriteSTAlignmentConditions WriteSTAlignmentConditions.h
 *
 *  Class for writing ST noise conditions
 *
 *  @author M.Needham
 *  @date   21/10/2009
 */

#include <fstream>

class WriteSTNoiseConditions : public ST::AlgBase {

public:
 
  /// constructer
  WriteSTNoiseConditions(const std::string& name, ISvcLocator *svcloc );

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

#endif // WriteSTNoiseConditions conditions
