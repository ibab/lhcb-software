// $Id: WriteAlignmentConditions.h,v 1.7 2009-08-31 12:23:45 mneedham Exp $
#ifndef WriteAlignmentConditions_H
#define WriteAlignmentConditions_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

/** @class WriteAlignmentConditions WriteAlignmentConditions.h
 *
 *  Class for printing out the full geometry tree
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   21/12/2006
 */

#include <fstream>
#include <iomanip>


class WriteAlignmentConditions : public GaudiAlgorithm {

public:
 
  /// constructer
  WriteAlignmentConditions(const std::string& name, ISvcLocator *svcloc );

  /// destructer
  virtual ~WriteAlignmentConditions();

  /// initialize
  virtual StatusCode initialize();

  /// execute
  StatusCode execute();

  // finalize
  StatusCode finalize();

  // write to file with name name
  StatusCode write(const std::string& filename) ;

private:

  std::string m_topElement;
  std::string m_footer;
  std::string m_startTag;

  /// Recursively print out the tree of the child DetectorElements
  void children(DetectorElement* parent, std::ofstream& out, unsigned int depth);

  std::string footer() const;
  std::string header(const std::string& conString) const;
  std::string strip(const std::string& conString) const;
  void replaceChars(std::string& conString) const;
  void replace(std::string& conString,std::string in, std::string out ) const;
  void createDirectory( const std::string& dir ) const ;

  std::string m_outputFileName;
  std::ofstream m_outputFile;
  unsigned int m_precision;
  std::vector<unsigned int> m_depths;

  std::string m_author;
  std::string m_desc;
  std::string m_tag;

};


#endif // WriteAlignment conditions
