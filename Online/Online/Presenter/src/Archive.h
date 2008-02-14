#ifndef ARCHIVE_H_
#define ARCHIVE_H_

#ifdef WIN32
  #undef boolean
  #undef BOOLEAN
  #include <Windows4Root.h>
#endif

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "presenter.h"

class DbRootHist;
class OnlineHistDB;
class OMAlib;
class OnlineHistogram;

class Archive
{
public:
  enum DirectoryType
  {
    Savesets,
    References
  };
  Archive(const std::string & archiveRoot,
  const std::string & savesetPath,
  const std::string & referencePath);
  virtual ~Archive();
  void setArchiveRoot(const std::string & archiveRoot);
  void setSavesetPath(const std::string & savesetPath);
  void setReferencePath(const std::string & referencePath);
  void setVerbosity(const pres::MsgLevel & verbosity) { m_verbosity = verbosity; }
  void setAnalysisLib(OMAlib* analib) { m_analysisLib = analib; }
  void refreshDirectory(const DirectoryType & directoryType);
  void closeRootFiles();
  void fillHistogram(DbRootHist* histogram,
                     const std::string & timePoint,
                     const std::string & pastDuration);
private:
  std::vector<boost::filesystem::path> listDirectory(const boost::filesystem::path & dirPath);
  boost::filesystem::path findFile(const boost::filesystem::path & dirPath,
                                   const std::string & fileName);
  std::vector<boost::filesystem::path> findSavesets(const std::string & taskname,
                                                    const std::string & endTimeIsoString,
                                                    const std::string & durationTimeString);


  boost::filesystem::path m_archiveRoot;
  boost::filesystem::path m_savesetPath;
  boost::filesystem::path m_referencePath;
  pres::MsgLevel m_verbosity;
  OMAlib* m_analysisLib;
  std::vector<boost::filesystem::path>::const_iterator m_foundFilesIt;
  std::vector<boost::filesystem::path> m_foundSavesets;
  std::vector<boost::filesystem::path>::const_iterator m_foundSavesetsIt;
  std::vector<boost::filesystem::path> m_foundReferences;
  std::vector<boost::filesystem::path>::const_iterator m_foundReferencesIt;
};
#endif /*ARCHIVE_H_*/
