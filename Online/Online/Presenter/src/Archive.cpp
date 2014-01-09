// This Class
#include "Archive.h"

// STL
#include <iostream>
#include <sstream>
#include <algorithm>

// Boost
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/predicate.hpp>

// ROOT
#include <TFile.h>
#include <TH1.h>
#include <TList.h>
#include <TString.h>
#include <TPRegexp.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TSystem.h>
#include <TGMsgBox.h>

// Online
#include "OnlineHistDB/OnlineHistogram.h"
#include "OMAlib/OMAlib.h"
//#include "Gaucho/MonObject.h"

// Local
#include "PresenterInformation.h"
#include "DisplayHistogram.h"

//=============================================================================
// compare date of two files
//=============================================================================
namespace ArchiveSpace {
  bool datecomp( boost::filesystem::path pfirst ,
                 boost::filesystem::path psecond) {
    TObjArray* fileDateMatchGroup = 0;
    fileDateMatchGroup = pres::s_fileDateRegexp.MatchS(pfirst.filename().string());
    boost::posix_time::ptime firstTime =
      boost::posix_time::from_iso_string
      ((((TObjString *)fileDateMatchGroup->At(3))->
        GetString()).Data());
    fileDateMatchGroup = pres::s_fileDateRegexp.MatchS(psecond.filename().string());
    boost::posix_time::ptime secondTime =
      boost::posix_time::from_iso_string
      ((((TObjString *)fileDateMatchGroup->At(3))->
        GetString()).Data());
    return (firstTime < secondTime);
  }
  const int maxRunInterval = 1000;
}

//=============================================================================
// Constructor
//=============================================================================
Archive::Archive( PresenterInformation * presInfo,
                  const std::string & savesetPath,
                  const std::string & referencePath) :
  m_presenterInfo( presInfo ),
  m_savesetPath  ( boost::filesystem::path( savesetPath ) ),
  m_referencePath( boost::filesystem::path( referencePath ) ),
  m_verbosity( pres::Silent),
  m_analysisLib(NULL) {
  if (m_verbosity >= pres::Verbose)
    std::cout << "m_referencePath " << m_referencePath
              << "m_savesetPath " << m_savesetPath << std::endl ;
}


//=============================================================================
// Destructor
//=============================================================================
Archive::~Archive() { ; }

//=============================================================================
// refresh the list of available files in the selected directory
//=============================================================================
void Archive::refreshDirectory(const DirectoryType & directoryType ,
                               const boost::gregorian::date_period &datePeriod,
                               const std::string & taskName)
{
  switch (directoryType) {
  case Savesets:
    m_foundSavesets = listAvailableRootFiles( m_savesetPath , datePeriod ,
                                              taskName ) ;
    break;
  case References:
    m_foundReferences = listAvailableRootFiles( m_referencePath , datePeriod ,
                                                taskName ) ;
    break;
  default:
    break;
  }
}

//=============================================================================
// List available ROOT files
//=============================================================================
std::vector< boost::filesystem::path >
Archive::listAvailableRootFiles(const boost::filesystem::path & dirPath,
                                const boost::gregorian::date_period & datePeriod,
                                const std::string & taskName) {
  std::vector< boost::filesystem::path > foundRootFiles;
  std::string partition( m_presenterInfo -> currentPartition() ) ;
  for ( boost::gregorian::day_iterator dateIterator(datePeriod.begin());
        dateIterator <= datePeriod.end(); ++dateIterator) {
    //if (m_verbosity >= pres::Verbose)
      std::cout << "Date: "
                << boost::gregorian::to_simple_string(*dateIterator)
                << std::endl;

    tm d_tm = boost::gregorian::to_tm(*dateIterator);
    Int_t year, month, day;
    year  = d_tm.tm_year + 1900;
    month = d_tm.tm_mon + 1;
    day   = d_tm.tm_mday;

    std::stringstream  dayLocation;
    dayLocation << dirPath.string() << pres::s_slash
                << std::setfill('0') << std::setw(4) << year << pres::s_slash
                << partition << pres::s_slash << taskName << pres::s_slash
                << std::setfill('0') << std::setw(2) << month << pres::s_slash
                << std::setfill('0') << std::setw(2) << day   << pres::s_slash;

    boost::filesystem::path pathOfTheDay(dayLocation.str());
    //if (m_verbosity >= pres::Verbose)
      std::cout << "Seeking in: " << pathOfTheDay << std::endl;

    boost::filesystem::directory_iterator end_itr;
    if (exists(pathOfTheDay)) {
      for ( boost::filesystem::directory_iterator itr(pathOfTheDay);
            itr != end_itr; ++itr) {
        if (is_regular(itr->path()) &&
            pres::s_rootFileExtension == extension(itr->path()) ){
          foundRootFiles.push_back(itr->path());
        }
      }
    } else {
      std::cout << " --- non existent area !"<< std::endl;

    }
  }

  std::sort(foundRootFiles.begin(), foundRootFiles.end());
  if ( m_verbosity >= pres::Debug && !foundRootFiles.empty()) {
    std::vector<boost::filesystem::path>::const_iterator foundFilesIt ;
    for ( foundFilesIt = foundRootFiles.begin() ;
          foundFilesIt != foundRootFiles.end() ; ++foundFilesIt )
      std::cout << "available file: " << *foundFilesIt << std::endl;
  }
  std::cout << " *** Found " << foundRootFiles.size() << " files" << std::endl;
  return foundRootFiles;
}

//=============================================================================
// List partitions
//=============================================================================
std::vector<std::string> Archive::listPartitions() {
  std::vector<std::string> foundPartitionDirectories;
  if (exists(m_savesetPath)) {
    boost::filesystem::directory_iterator year_end_itr;
    for ( boost::filesystem::directory_iterator year_itr(m_savesetPath);
          year_itr != year_end_itr; ++year_itr) {
      if (is_directory(year_itr->path())) {
        boost::filesystem::directory_iterator partition_end_itr;
        for ( boost::filesystem::directory_iterator
                partition_itr(year_itr->path());
              partition_itr != partition_end_itr; ++partition_itr) {
          if (is_directory(partition_itr->path()) &&
              std::string::npos ==
              partition_itr->path().string().find("ByRun"))
            foundPartitionDirectories.push_back(partition_itr->path().filename().string());
        }
      }
    }
  }
  std::sort(foundPartitionDirectories.begin(),
            foundPartitionDirectories.end());

  return foundPartitionDirectories;
}

//=============================================================================
// Find a file
//=============================================================================
boost::filesystem::path Archive::findFile(const boost::filesystem::path & dirPath,
                                          const std::string & fileName) {
  boost::filesystem::path foundFile( "" ) ;
  if (exists(dirPath)) {
    // default construction yields past-the-end
    boost::filesystem::directory_iterator end_itr;
    for ( boost::filesystem::directory_iterator itr(dirPath) ;
          itr != end_itr; ++itr) {
      if (is_directory(itr->status()))
        foundFile = findFile(itr->path(), fileName);
      else if (itr->path().leaf() == fileName)
        foundFile = itr->path() ;
      if (is_regular(foundFile)) return foundFile ;
    }
  }
  return foundFile;
}

//=============================================================================
// Create string in ISO format for time
//=============================================================================
std::string Archive::createIsoTimeString (const tm &pt_tm ) {
  Int_t year, month, day, hour, min, sec;
  year = pt_tm.tm_year + 1900 ;
  month = pt_tm.tm_mon + 1 ;
  day = pt_tm.tm_mday;
  hour = pt_tm.tm_hour;
  min = pt_tm.tm_min;
  sec = pt_tm.tm_sec;
  return createIsoTimeString(year, month, day, hour, min, sec);
}

//=============================================================================
// Compute difference between two times (in string format)
//=============================================================================
std::string Archive::timeDiff( const std::string & startTimeIsoString ,
                               const std::string & endTimeIsoString ) {
  boost::posix_time::ptime
    startTime( boost::posix_time::from_iso_string(startTimeIsoString));
  boost::posix_time::ptime
    endTime( boost::posix_time::from_iso_string(endTimeIsoString));
  boost::posix_time::time_duration diff= endTime - startTime;
  return boost::posix_time::to_simple_string(diff);
}

//=============================================================================
// Add date to time in iso format
//=============================================================================
std::string Archive::addIsoTimeDate(const std::string & startTimeIsoString,
                                    const std::string & deltaTimeString) {
  boost::posix_time::ptime
    startTime(boost::posix_time::from_iso_string((startTimeIsoString)));
  boost::posix_time::time_duration
    diffTime(boost::posix_time::duration_from_string((deltaTimeString)));

  boost::posix_time::ptime newPTime = startTime + diffTime;

  return createIsoTimeString(boost::posix_time::to_tm(newPTime)) ;
}

//=============================================================================
// Substract date to time in iso format
//=============================================================================
std::string Archive::substractIsoTimeDate(const std::string &
                                          startTimeIsoString,
                                          const std::string & deltaTimeString)
{
  boost::posix_time::ptime
    startTime(boost::posix_time::from_iso_string((startTimeIsoString)));
  boost::posix_time::time_duration
    diffTime(boost::posix_time::duration_from_string((deltaTimeString)));

  boost::posix_time::ptime newPTime = startTime - diffTime;

  return createIsoTimeString(boost::posix_time::to_tm(newPTime));
}

//=============================================================================
// Find task name from file
//=============================================================================
std::string Archive::taskNameFromFile(const std::string & fileName) const {
  TObjArray* fileNameMatchGroup = 0;
  std::string taskNameFound("");
  std::string fn = boost::filesystem::path(fileName).filename().string();
  fileNameMatchGroup =
    pres::s_fileDateRegexp.MatchS(fn);
  if (!fileNameMatchGroup->IsEmpty())
    taskNameFound =
      (((TObjString *)fileNameMatchGroup->At(1))->GetString()).Data();
  else {
    if (fileNameMatchGroup) {
      fileNameMatchGroup->Delete();
      delete fileNameMatchGroup;
      fileNameMatchGroup = 0;
    }
    // try with run-aggregated saveset
    fileNameMatchGroup =
      pres::s_fileRunRegexp.MatchS(fn);
    if (!fileNameMatchGroup->IsEmpty())
      taskNameFound =
        (((TObjString *)fileNameMatchGroup->At(1))->GetString()).Data();
    else {
      // try with offline DQ saveset
      if (fileNameMatchGroup) {
        fileNameMatchGroup->Delete();
        delete fileNameMatchGroup;
        fileNameMatchGroup = 0;
      }
      fileNameMatchGroup =
        pres::s_offlineJobRegexp.MatchS(fn);
      if (!fileNameMatchGroup->IsEmpty())
        taskNameFound =
          (((TObjString *)fileNameMatchGroup->At(1))->GetString()).Data();
    }
  }
  if (fileNameMatchGroup) {
    fileNameMatchGroup->Delete();
    delete fileNameMatchGroup;
    fileNameMatchGroup = 0;
  }
  return taskNameFound;
}

//=============================================================================
// Find savesets
//=============================================================================
std::vector< boost::filesystem::path> Archive::findSavesets(const std::string & taskname,
                                                            const std::string & endTimeIsoString,
                                                            const std::string & durationTimeString) {
  std::vector< boost::filesystem::path > foundRootFiles;
  boost::posix_time::ptime endTime;
  bool isEFF = (taskname == pres::s_efftask);

  if ("Now" == endTimeIsoString) {
    endTime = boost::posix_time::ptime(boost::posix_time::second_clock::local_time());
  } else {
    endTime = boost::posix_time::ptime(boost::posix_time::from_iso_string(endTimeIsoString));
  }

  //if (m_verbosity >= pres::Verbose)
  std::cout << "Archive::findSavesets: " << taskname
            << " timePoint " << to_iso_string(endTime)
            << " pastDuration " << durationTimeString << std::endl;

  boost::posix_time::time_duration timeDuration(boost::posix_time::duration_from_string(durationTimeString));
  boost::posix_time::ptime startTime = endTime - timeDuration;

  boost::gregorian::date startDate(startTime.date());
  boost::gregorian::date endDate(endTime.date());
  boost::gregorian::date_period datePeriod(startDate, endDate);

  refreshDirectory(Archive::Savesets, datePeriod, taskname);

  std::string taskNameFound;
  std::string fileTimeFound;
  boost::posix_time::ptime fileTime;

  if ( !m_foundSavesets.empty() ) {
    std::vector<boost::filesystem::path>::const_iterator foundSavesetsIt;
    foundSavesetsIt = m_foundSavesets.end();
    --foundSavesetsIt;
    while ( foundSavesetsIt >= m_foundSavesets.begin()) {
      TObjArray* fileDateMatchGroup = 0;
      fileDateMatchGroup = pres::s_fileDateRegexp.MatchS((*foundSavesetsIt).filename().string());
      if (fileDateMatchGroup->GetEntriesFast() > 4 ) {
        // the array contains the matched strings +2
        taskNameFound = (((TObjString *)fileDateMatchGroup->At(1))->GetString()).Data();
        fileTimeFound = (((TObjString *)fileDateMatchGroup->At(3))->GetString()).Data();
        fileTime = boost::posix_time::from_iso_string(fileTimeFound);

        bool acceptSvs = (taskNameFound == taskname && (!fileTime.is_not_a_date_time()));
        // for EFF tasks, require also the end of run flag
        // (histograms not reset during run)
        if (isEFF) {
          if (fileDateMatchGroup->GetEntriesFast() >5 )
            acceptSvs &= ( (((TObjString *)fileDateMatchGroup->At(4))->GetString()) == pres::s_eor );
          else acceptSvs = false;
        }

        if (acceptSvs) {
          if ( ( fileTime <= endTime ) && ( fileTime >= startTime ) ) {
            //std::cout << "using file: " << (*foundSavesetsIt).leaf() << std::endl ;
            foundRootFiles.push_back(*foundSavesetsIt);
          } else if ( fileTime < startTime) {
            std::string run = (((TObjString *)fileDateMatchGroup->At(2))->GetString()).Data();
            std::cout << "Ignore file " << (*foundSavesetsIt).leaf() << " run string '" << run
                      << "' time " << fileTimeFound << std::endl;
            if ( "" != run ) {
              if (fileDateMatchGroup) {
                fileDateMatchGroup->Delete();
                delete fileDateMatchGroup;
                fileDateMatchGroup = 0;
              }
              break;
            }
          }
        }
      }
      if (fileDateMatchGroup) {
        fileDateMatchGroup->Delete();
        delete fileDateMatchGroup;
        fileDateMatchGroup = 0;
      }
      --foundSavesetsIt;
    }
  }
  return foundRootFiles;
}

//=============================================================================
// Find savesets by run number
//=============================================================================
std::vector< boost::filesystem::path> Archive::findSavesetsByRun(const std::string & taskname,
                                                                 const std::string & string_endRun,
                                                                 const std::string & string_runDuration) {
  std::vector< boost::filesystem::path > foundRootFiles;
  std::istringstream iendRun(string_endRun);
  std::istringstream irunDuration(string_runDuration);
  int startRun,endRun;
  std::string partition(m_presenterInfo->currentPartition());
  m_runNumbers.clear();

  iendRun >> endRun;
  irunDuration >> startRun;
  startRun=endRun-startRun;
  if (endRun < startRun) {
    std::cout << "Invalid run interval "<<startRun << " - "<<endRun
              << "  converted to "<<endRun << " - "<<endRun<< std::endl;
    startRun = endRun;
  }
  if ((endRun - startRun) > ArchiveSpace::maxRunInterval) {
    std::cout << "Too large interval "<<startRun << " - "<<endRun
              << "  converted to "<<endRun-ArchiveSpace::maxRunInterval
              << " - "<<endRun<< std::endl;
    startRun = endRun-ArchiveSpace::maxRunInterval;
  }

  std::cout << "Archive::findSavesetsByRun: task " << taskname
            << " startRun " << startRun
            << " endRun " << endRun << std::endl;

  if ( m_presenterInfo->offlineContext() ) {
    std::string runType = m_presenterInfo->runType();
    std::string magnet  = m_presenterInfo->magnetState();
    std::string velo    = m_presenterInfo->veloPosition();
    std::string energy  = m_presenterInfo->beamEnergy();
    std::string rootDir = m_savesetPath.string() + "/" + runType + "/Beam" + energy + "GeV-Velo" + velo +  "-Mag" + magnet + "/";
    for ( int kRun = startRun; endRun >= kRun; ++kRun ) {
      std::stringstream  dirLocation;
      dirLocation << rootDir << kRun << "/" << m_presenterInfo->processing() << "/";
      if ( m_presenterInfo->eventType() == "EXPRESS" ) {
        dirLocation << "91000000";
      } else {
        dirLocation << "90000000";
      }
      boost::filesystem::path runPath(dirLocation.str());
      boost::filesystem::directory_iterator end_itr;
      std::cout << "inspecting folder: " << dirLocation.str() << std::endl;
      if ( exists( runPath ) ) {
        boost::filesystem::path lastPath( "" );
        for ( boost::filesystem::directory_iterator itr(runPath); itr != end_itr; ++itr) {
          std::cout << "Found file " << itr->path() << std::endl;
          lastPath = itr->path();
        }
        if ( "" != lastPath ) {
          foundRootFiles.push_back( lastPath );
          m_runNumbers.push_back( kRun );
        }
      }
    }
    return foundRootFiles;
  }

  std::string aggrSvsPath = m_savesetPath.string() + pres::s_slash + pres::s_byRunDir + pres::s_slash + taskname;

  int th = startRun / 1000 * 1000;

  while (th <= endRun) {
    int dth = th / 10000 * 10000;
    std::stringstream  dirLocation;
    dirLocation << aggrSvsPath << pres::s_slash << dth << pres::s_slash << th;

    boost::filesystem::path runPath(dirLocation.str());
    boost::filesystem::directory_iterator end_itr;
    std::cout << "inspecting folder: " << dirLocation.str() << std::endl;
    if (exists(runPath)) {
      for ( boost::filesystem::directory_iterator itr(runPath);
            itr != end_itr; ++itr) {
        if ( is_regular(itr->path()) &&
             pres::s_rootFileExtension == extension(itr->path()) ) {
          TObjArray* fileRunMatchGroup = 0;
          fileRunMatchGroup = pres::s_fileRunRegexp.MatchS((itr->path()).filename().string());
          if (fileRunMatchGroup->GetSize() > 2) {
            std::istringstream icurRun((((TObjString *)(fileRunMatchGroup->At(2)))->GetString()).Data());
            int curRun;
            icurRun >> curRun;
            if (curRun >=  startRun && curRun <= endRun) {
              foundRootFiles.push_back(itr->path());
              std::cout << "using file: " << itr->path() << std::endl;
              m_runNumbers.push_back( curRun );
            }
          }
        }
      }
    } // end loop on directory containing up to 1000 runs, step by 1000 runs
    th += 1000;
  }
  return foundRootFiles;
}

//=============================================================================
// Set history by labels
//=============================================================================
void Archive::setHistoryLabels(TH1* h, std::vector<boost::filesystem::path>& rootFiles) {
  if (!h) return;
  unsigned int n=h->GetNbinsX();
  if (n != rootFiles.size()) {
    std::cout << " Error in setHistoryLabels: uncompatible file list"
              << std::endl;
    return;
  }
  int bin=0,lastbin=-100;
  int maxnbinUnlabeled = n/4;
  if ( m_presenterInfo->globalHistoryByRun( ) ) {
    int lastrun=0,run;
    for ( std::vector<boost::filesystem::path>::const_iterator rootFilesIt = rootFiles.begin();
          rootFilesIt != rootFiles.end(); ++rootFilesIt ) {
      bin++;
      if ( m_presenterInfo->offlineContext() ) {
        run = m_runNumbers[rootFilesIt-rootFiles.begin()];
      } else {
        TObjArray* fileRunMatchGroup = pres::s_fileRunRegexp.MatchS((*rootFilesIt).filename().string());
        std::istringstream irun((((TObjString *)fileRunMatchGroup->At(2))->GetString()).Data());
        irun >> run;
      }
      char runString[40];
      sprintf(runString, "%d", run );
      if ( (run-lastrun) > (bin-lastbin) ||
           (bin-lastbin) > maxnbinUnlabeled ) {  // add a label
        h->GetXaxis()->SetBinLabel(bin, runString);
        lastrun = run;
        lastbin = bin;
      }
    }
  }
  else {
    std::string ts("20020131T235959");
    boost::posix_time::ptime lastTime(boost::posix_time::from_iso_string(ts));
    boost::posix_time::time_duration maxdelta = boost::posix_time::hours(1);
    for (std::vector<boost::filesystem::path>::const_iterator rootFilesIt =
           rootFiles.begin(); rootFilesIt != rootFiles.end(); rootFilesIt++ ) {
      bin++;
      TObjArray* fileDateMatchGroup = 0;
      fileDateMatchGroup = pres::s_fileDateRegexp.MatchS((*rootFilesIt).filename().string());
      boost::posix_time::ptime fileTime =
        boost::posix_time::from_iso_string
        ((((TObjString *)fileDateMatchGroup->At(3))->
          GetString()).Data());
      boost::posix_time::time_duration delta = (fileTime-lastTime);
      if ( delta > maxdelta ||(bin-lastbin) > maxnbinUnlabeled ) {
        // add a label
        h->GetXaxis()->SetBinLabel(bin,
                                   Form("%d/%d %02d:%02d",
                                        (int) (fileTime.date().day()),
                                        (int) (fileTime.date().month()),
                                        (int) (fileTime.time_of_day().hours()),
                                        (int) (fileTime.time_of_day().minutes())
                                        ) );
        lastTime = fileTime;
        lastbin = bin;
      }
    }
  }
}

//=============================================================================
// Create string in ISO format for time
//=============================================================================
std::string Archive::createIsoTimeString(const int& year, const int& month,
                                         const int& day, const int& hour,
                                         const int& min, const int& sec) {
  std::stringstream isoTimeStringStream;
  isoTimeStringStream << std::setfill('0') << std::setw(4) << year
                      << std::setfill('0') << std::setw(2) << month
                      << std::setfill('0') << std::setw(2) << day << "T"
                      << std::setfill('0') << std::setw(2) << hour
                      << std::setfill('0') << std::setw(2) << min
                      << std::setfill('0') << std::setw(2) << sec;
  return isoTimeStringStream.str();
}

//=========================================================================
//
//=========================================================================
void Archive::setFiles ( const std::string& task,
                         const std::string& timePoint,
                         const std::string& pastDuration  ) {
  std::cout << "globalHistoryByRun flag " << m_presenterInfo -> globalHistoryByRun() << std::endl;
  m_rootFiles.clear();
  if ( m_presenterInfo -> globalHistoryByRun() ) {
    m_rootFiles = findSavesetsByRun( task, timePoint , pastDuration ) ;
  } else if ( pres::s_startupFile == timePoint) {
    boost::filesystem::path filePath(pastDuration) ;

    if (pres::s_rootFileExtension == extension(filePath) ) {
      if (exists(filePath)) {
        m_rootFiles.push_back(filePath);
      } else if ( exists( m_savesetPath/filePath ) ) {
        m_rootFiles.push_back( m_savesetPath/filePath );
      } else if ( boost::algorithm::starts_with( filePath.native() , "castor:/") ) {
        m_rootFiles.push_back( filePath ) ;
      }
      std::cout << "Archive::setFiles: File is " << m_rootFiles.back() << std::endl;
    }
  } else {
    m_rootFiles = findSavesets( task, timePoint , pastDuration ) ;
  }

  if ( !m_rootFiles.empty() ) {
    if ( m_presenterInfo -> globalHistoryByRun() ) {
      std::sort( m_rootFiles.begin(), m_rootFiles.end());
    } else {
      std::sort( m_rootFiles.begin(), m_rootFiles.end(), ArchiveSpace::datecomp);
    }

    std::cout << "Merging " << m_rootFiles.size() << " file(s)" << std::endl;
  }
}

//=========================================================================
//
//=========================================================================
void Archive::closeFiles( ) {
  std::cout << "Closing " << m_openFiles.size() << " files " << std::endl;
  for ( std::vector<TFile*>::iterator itF = m_openFiles.begin(); m_openFiles.end() != itF; ++itF ) {
    (*itF)->Close();
  }
  m_openFiles.clear();
}
//=========================================================================
//  Load histograms from (list of) files previously created
//=========================================================================
void Archive::fillHistogramsFromFiles ( std::vector<DisplayHistogram>& histos ) {

  std::vector<boost::filesystem::path> goodRootFiles;

  std::vector<TList*> list;
  std::vector<std::vector<double> > mean;
  std::vector<std::vector<double> > error;
  std::vector<double> dum;
  while ( histos.size() > list.size() ) {
    list.push_back( new TList );
    mean.push_back( dum );
    error.push_back( dum );
  }

  for ( std::vector< boost::filesystem::path >::const_iterator itF = m_rootFiles.begin();
        m_rootFiles.end() != itF; ++itF ) {
    std::string f = itF->native();
    std::cout << "++ Opening root file " << f.c_str() << std::endl;
    TFile* rootFile = TFile::Open( f.c_str() ) ;
    if ( 0 == rootFile ) {
      std::cout << "Error reading file " << f
                << std::endl
                << "History not available !"
                << std::endl ;
      for ( unsigned int kk = 0; histos.size() > kk; ++kk ) histos[kk].createDummyHisto();
      break ;
    }

    if ( rootFile -> IsZombie() ) {
      std::cout << "Error opening Root file: " << f << std::endl;
      continue;
    }
    m_openFiles.push_back( rootFile );

    std::cout << " ++ file " << f << " is open." << std::endl;

    //== Loop over histograms

    bool first = true;
    for ( unsigned int kk = 0; histos.size() > kk; ++kk ) {
      TH1* archiveHisto;
      std::string name = histos[kk].rootName();
      rootFile -> GetObject( name.c_str(), archiveHisto );
      if (!archiveHisto) {
        name = name.substr( name.find("/")+1);
        std::cout << "  .. try with " << name << std::endl;
        rootFile -> GetObject( name.c_str(), archiveHisto );
      }
      if (archiveHisto) {
        list[kk]->Add( archiveHisto );
        mean[kk].push_back ( archiveHisto->GetMean() );
        error[kk].push_back( archiveHisto->GetMeanError() );
        if ( first ) {
          goodRootFiles.push_back(*itF);
          first = false;
        }
        std::cout << "found histogram " << histos[kk].rootName().c_str() << std::endl;

        if ( !histos[kk].rootHist( ) ) {
          std::cout << "Create a clean root hist" << std::endl;
          histos[kk].setRootHist( (TH1*)( archiveHisto->Clone() ) ) ;
          histos[kk].rootHist()->Reset( ) ;
        }
      } else {
        std::cout << "Histo not found!" << std::endl;
      }
    }
  }
  //== If no file found, close properly.
  if ( goodRootFiles.size() == 0 ) {
    for ( unsigned int kk = 0; histos.size() > kk; ++kk ) histos[kk].createDummyHisto();
    return;
  }

  for ( unsigned int kk = 0; histos.size() > kk; ++kk ) {
    if ( NULL != histos[kk].rootHist() ) {  // at least one source is available
      if ( ( ! m_presenterInfo -> isHistoryTrendPlotMode() ) ||
           ( histos[kk].rootHist()->GetDimension() > 1  ) ||
           ( 2 > m_rootFiles.size() ) ) {
        std::cout << "Merge histos in " << histos[kk].rootHist() << std::endl;
        histos[kk].rootHist()->Merge(list[kk]);
      } else {
        // do a trend plot of mean and rms
        std::string histogramTitle("History plot for ");
        histogramTitle += histos[kk].identifier();
        TH1* newh = new TH1F( histogramTitle.c_str(), histogramTitle.c_str(),
                              list[kk]->GetSize(), 0.5,  list[kk]->GetSize() +0.5);
        newh->Sumw2();
        for (int i=0 ; i<list[kk]->GetSize(); i++) {
          newh->SetBinContent(i+1, mean[kk][i] );
          newh->SetBinError(i+1, error[kk][i] );
        }
        setHistoryLabels(newh, goodRootFiles);
        histos[kk].setHistoryTrendPlot(true);
        histos[kk].deleteRootHist( ) ;
        histos[kk].setRootHist( newh ) ;
      }
    } else {
      histos[kk].createDummyHisto();
    }
    //== Cleanup
    TIter next(list[kk]);
    TH1* histo;
    while ( (histo = (TH1 *) next())) {
      list[kk]->Remove(histo);
      delete histo;
    }
    delete list[kk];
  }
}
//=========================================================================
