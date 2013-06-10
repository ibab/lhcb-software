// $Id: $
#ifndef DUMPTRENDINGFILE_H 
#define DUMPTRENDINGFILE_H 1

// Include files
#include "Trending/ITrendingTool.h"

/** @class DumpTrendingFile DumpTrendingFile.h
 *  Debug a trend file
 *
 *  @author Olivier Callot
 *  @date   2011-05-31
 */
class DumpTrendingFile {
public: 
  /// Standard constructor
  DumpTrendingFile( ITrendingTool* trend ); 

  virtual ~DumpTrendingFile( ); ///< Destructor

  void dump( std::string file, bool verbose );

protected:

private:
  ITrendingTool*   m_trend;
  FILE*            m_file;
  Header           m_tagHeader;
  DirectoryRecord  m_dir;
  DataRecord       m_data;
  long             m_firstDirAddress;
  long             m_tagAddressInFile;
  long             m_dirAddressInFile;
  long             m_dataAddressInFile;
  //int              m_ptDir; // unused!
  int              m_ptData;
  int              m_nbTag;
  int              m_nbMask;
  std::vector<std::string> m_tags;
  std::vector<float>       m_lastData;
};
#endif // DUMPTRENDINGFILE_H
