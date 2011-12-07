// $Id: Archive.h,v 1.29 2010-09-20 18:41:27 robbep Exp $
#ifndef PRESENTER_ARCHIVE_H
#define PRESENTER_ARCHIVE_H 1

// include files
#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "presenter.h"

// forward declaration
class OnlineHistDB;
class OMAlib;
class OnlineHistogram;
class TH1;
class PresenterInformation;
class DisplayHistogram;

/** @class Archive Archive.h
 *
 *  Class to fill and handle histograms from history files
 *
 */
class Archive {
public:
  /// Directory type
  enum DirectoryType {
    Savesets, ///< directory of saveset files
    References ///< directory of reference files
  };

  /// Constructor
  Archive( PresenterInformation * presenterInfo ,
           const std::string & savesetPath ,
           const std::string & referencePath ) ;

  virtual ~Archive() ; ///< Destructor

  /// list partitions
  std::vector<std::string> listPartitions() ;

  /// set the saveset Path
  void setSavesetPath(const std::string & savesetPath){
    m_savesetPath = boost::filesystem::path(savesetPath) ; } ;

  /// set the reference path
  void setReferencePath(const std::string & referencePath) {
    m_referencePath = boost::filesystem::path(referencePath) ; } ;

  /// return the reference path as string
  std::string referencePath() const { return m_referencePath.native(); }

  /// Set verbosity level
  void setVerbosity(const pres::MsgLevel & verbosity) {
    m_verbosity = verbosity ; }

  /// set pointer to analysis library object
  void setAnalysisLib(OMAlib* analib) { m_analysisLib = analib; }

  /// Get task name from a file
  std::string taskNameFromFile(const std::string & fileName) const ;

  /// Set the file list for the specified task
  void setFiles(  const std::string& task,
                  const std::string& timePoint,
                  const std::string& pastDuration  );

  /// Fill histograms from the files previously found.
  void fillHistogramsFromFiles ( std::vector<DisplayHistogram>& histos );

  /// Utility function to create ISO time string
  static std::string createIsoTimeString(const int& year, const int& month,
                                         const int& day, const int& hour,
                                         const int& min, const int& sec);

  /// Utility function to create ISO time string
  static std::string createIsoTimeString(const tm &pt_tm);

  /// Utility function to substract 2 times
  static std::string timeDiff(const std::string & startTimeIsoString,
                              const std::string & endTimeIsoString);

  /// Utility function to add 2 times
  static std::string addIsoTimeDate(const std::string & startTimeIsoString,
                             const std::string & deltaTimeString);

  /// Utility function to substract 2 times
  static std::string substractIsoTimeDate(const std::string &
                                          startTimeIsoString,
                                          const std::string & deltaTimeString);

  void closeFiles();

private:
  /// List available ROOT files in the given directory
  std::vector<boost::filesystem::path>
  listAvailableRootFiles(const boost::filesystem::path & dirPath,
                         const boost::gregorian::date_period & datePeriod,
                         const std::string & taskName) ;

  /// refresh directory content
  void refreshDirectory(const DirectoryType & directoryType,
                        const boost::gregorian::date_period & datePeriod,
                        const std::string & taskName);

  /// Find a file
  boost::filesystem::path findFile(const boost::filesystem::path & dirPath,
                                   const std::string & fileName);

  /// find save sets
  std::vector<boost::filesystem::path>
  findSavesets(const std::string & taskname,
               const std::string & endTimeIsoString,
               const std::string & durationTimeString);


  /// find the savesets by run
  std::vector<boost::filesystem::path>
  findSavesetsByRun(const std::string & taskname,
                    const std::string & endTimeIsoString,
                    const std::string & durationTimeString);

  /// set history labels
  void setHistoryLabels(TH1* h,
                        std::vector<boost::filesystem::path>& foundRootFiles);

  PresenterInformation *  m_presenterInfo ; ///< Presenter information
  boost::filesystem::path m_savesetPath   ; ///< saveset file path
  boost::filesystem::path m_referencePath ; ///< reference file path
  pres::MsgLevel          m_verbosity     ; ///< Verbosity level
  OMAlib *                m_analysisLib   ; ///< Pointer to analysis lib

  std::vector<boost::filesystem::path> m_foundSavesets; ///< List of save sets
  /// List of references
  std::vector<boost::filesystem::path> m_foundReferences;
  std::vector< boost::filesystem::path > m_rootFiles ;
  std::vector<TFile*> m_openFiles;
  std::vector<int> m_runNumbers;
};
#endif /*PRESENTER_ARCHIVE_H*/
