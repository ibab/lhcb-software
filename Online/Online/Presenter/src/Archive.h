#ifndef ARCHIVE_H_
#define ARCHIVE_H_

#ifdef WIN32
  #undef boolean
  #undef BOOLEAN
  #include <Windows4Root.h>
#endif

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "presenter.h"

class DbRootHist;
class OnlineHistDB;
class OMAlib;
class OnlineHistogram;
class TH1;
class PresenterMainFrame;

class Archive
{
public:
  enum DirectoryType
  {
    Savesets,
    References
  };
  Archive(PresenterMainFrame* gui,
          const std::string & savesetPath,
          const std::string & referencePath);
  virtual ~Archive();
  std::vector<boost::filesystem::path> listAvailableRootFiles(const boost::filesystem::path & dirPath,
                                                              const boost::gregorian::date_period & datePeriod,
                                                              const std::string & taskName);
  std::vector<std::string> listPartitions();
  void setSavesetPath(const std::string & savesetPath);  
  std::string savesetPath() { return m_savesetPath.file_string(); }    
  void setReferencePath(const std::string & referencePath);
  std::string referencePath() { return m_referencePath.file_string(); }
  
  void setVerbosity(const pres::MsgLevel & verbosity) { m_verbosity = verbosity; }
  void setAnalysisLib(OMAlib* analib) { m_analysisLib = analib; }
  void refreshDirectory(const DirectoryType & directoryType,
                        const boost::gregorian::date_period & datePeriod,
                        const std::string & taskName);
  void closeRootFiles();  
  void fillHistogram(DbRootHist* histogram,
                     const std::string & timePoint,
                     const std::string & pastDuration);
//  TH1* referenceHistogram(const std::string & referenceDbEntry);
  void saveAsReferenceHistogram(DbRootHist* histogram);
  std::string createIsoTimeString(int& year, int& month, int& day,
                                  int& hour, int& min, int& sec);
  std::string createIsoTimeString(const tm &pt_tm);                                  
  std::string timeDiff(const std::string & startTimeIsoString,
                       const std::string & endTimeIsoString);
  std::string addIsoTimeDate(const std::string & startTimeIsoString,
                             const std::string & deltaTimeString);
  std::string substractIsoTimeDate(const std::string & startTimeIsoString,
                                   const std::string & deltaTimeString);                      
  std::string taskNameFromFile(const std::string & fileName);

//  void deleteReferenceHistogram(TH1* reference); 
private:
  PresenterMainFrame* m_mainFrame;
  boost::filesystem::path findFile(const boost::filesystem::path & dirPath,
                                   const std::string & fileName);
  std::vector<boost::filesystem::path> findSavesets(const std::string & taskname,
                                                    const std::string & endTimeIsoString,
                                                    const std::string & durationTimeString);
  void setHistoryLabels(TH1* h, 
                        std::vector<boost::filesystem::path>& foundRootFiles);

  boost::filesystem::path m_savesetPath;
  boost::filesystem::path m_referencePath;
  pres::MsgLevel m_verbosity;
  OMAlib* m_analysisLib;
  int m_msgBoxReturnCode;
  std::vector<boost::filesystem::path>::const_iterator m_foundFilesIt;
  std::vector<std::string>::const_iterator  m_foundPartitionsIt;
  std::vector<boost::filesystem::path> m_foundSavesets;
  std::vector<boost::filesystem::path>::const_iterator m_foundSavesetsIt;
  std::vector<boost::filesystem::path> m_foundReferences;
  std::vector<boost::filesystem::path>::const_iterator m_foundReferencesIt;
};
#endif /*ARCHIVE_H_*/
