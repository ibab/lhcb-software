#include <iostream>
#include <algorithm>

#include "Archive.h"
#include "DbRootHist.h"
#include "PresenterMainFrame.h"

#include <TFile.h>
#include <TH1.h>
#include <TList.h>

#include <TString.h>
#include <TPRegexp.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TSystem.h>
#include <TGMsgBox.h>

using namespace pres;
using namespace std;
using namespace boost::filesystem;
using namespace boost::posix_time;

Archive::Archive(PresenterMainFrame* gui,
                 const std::string & archiveRoot,
                 const std::string & savesetPath,
                 const std::string & referencePath) :
  m_mainFrame(gui),
  m_archiveRoot(path(archiveRoot)),
  m_savesetPath(path(savesetPath)),
  m_referencePath(path(referencePath)),
  m_verbosity(Silent),
  m_analysisLib(NULL),
  m_msgBoxReturnCode(0)
{
}
Archive::~Archive()
{
}
void Archive::setArchiveRoot(const std::string & archiveRoot)
{
  m_archiveRoot = path(archiveRoot);
}
void Archive::setSavesetPath(const std::string & savesetPath)
{
  m_savesetPath = path(savesetPath);
}
void Archive::setReferencePath(const std::string & referencePath)
{
  m_referencePath = path(referencePath);
}
void Archive::refreshDirectory(const DirectoryType & directoryType)
{
  switch (directoryType) {
    case Savesets:
      m_foundSavesets = listDirectory(m_savesetPath);
      break;
    case References:
      m_foundReferences = listDirectory(m_referencePath);
      break;
    default:
      break;
  }
}
std::string Archive::referenceFilePath(DbRootHist* histogram)
{
  return string("");
}
void Archive::fillHistogram(DbRootHist* histogram,
                            const std::string & timePoint,
                            const std::string & pastDuration)
{
  if (m_verbosity >= Debug) {
    std::cout << "Histogram to seek: " << histogram->identifier() << std::endl;
  }
  // listFiles()
  if ( ! (histogram->isAnaHist()) ) {
    std::vector<path> foundRootFiles = findSavesets(histogram->taskName(),
                                                    timePoint,
                                                    pastDuration);
    if (!foundRootFiles.empty()) {
      if (m_verbosity >= Verbose) {
        std::cout << "Merging " << foundRootFiles.size() <<
                     " file(s)" << std::endl;
      }
      std::vector<path>::const_iterator foundRootFilesIt;
      foundRootFilesIt = foundRootFiles.begin();

      TFile rootFile((*foundRootFilesIt).file_string().c_str());
      if (rootFile.IsZombie()) {
        cout << "Error opening Root file" << endl;
      } else {
        if (histogram->rootHistogram) {
          delete (histogram->rootHistogram);
          histogram->rootHistogram = NULL;
        }
        TH1* archiveHisto;
        rootFile.GetObject((histogram->onlineHistogram()->hname()).c_str(),
                           archiveHisto);
        if (archiveHisto) {
          histogram->rootHistogram = archiveHisto;
        }
        ++foundRootFilesIt;
      }
      TList* list = new TList;
      while (foundRootFilesIt != foundRootFiles.end()) {
        TFile rootFile((*foundRootFilesIt).file_string().c_str());
        if (rootFile.IsZombie()) {
          cout << "Error opening Root file: " <<
            (*foundRootFilesIt).file_string() << endl;
        } else {
          TH1* archiveHisto;
          rootFile.GetObject((histogram->onlineHistogram()->hname()).c_str(),
                             archiveHisto);
          if (archiveHisto) {
            list->Add(archiveHisto);
          }
        }
        ++foundRootFilesIt;
      }
      if (histogram->rootHistogram) {
        (histogram->rootHistogram)->Merge(list);
      }

      TH1* histo;
      TIter next(list);
      while (histo = (TH1 *) next()) {
        list->Remove(histo);
        delete histo;
      }
      delete list;
    } else {
      histogram->beEmptyHisto();
    }
  } else { //analysis histogram
    if(m_analysisLib) {
      unsigned int ns= (histogram->anaSources())->size();
      std::vector<TH1*> sources(ns);
      bool sourcesok = true;
      for (unsigned int i=0; i< ns ; ++i) {
        fillHistogram((histogram->anaSources())->at(i),
                      timePoint,
                      pastDuration);
        sources[i]= ((histogram->anaSources())->at(i))->rootHistogram;
        if ( ((histogram->anaSources())->at(i))->isEmptyHisto() ) {
          sourcesok = false;
        }
      }
      OMAHcreatorAlg* creator = dynamic_cast<OMAHcreatorAlg*>
                     (m_analysisLib->getAlg(histogram->creationAlgorithm()));
      if (creator && sourcesok) {
        histogram->rootHistogram = creator->exec(&sources,
                                      histogram->anaParameters(),
                                      histogram->identifier(),
                                      histogram->onlineHistogram()->htitle(),
                                      histogram->rootHistogram);
      }
    }
    if (! (histogram->rootHistogram) ) { histogram->beEmptyHisto(); }
  }
  histogram->setTH1FromDB();
}
std::vector<path> Archive::listDirectory(const path & dirPath)
{
  std::vector<path> foundRootFiles;
  if (exists(dirPath)) {
    directory_iterator end_itr;
    for (directory_iterator itr(dirPath); itr != end_itr; ++itr) {
      if (is_regular(itr->path()) &&
          extension(itr->path()) == s_rootFileExtension ) {
        foundRootFiles.push_back(itr->path());
      }
    }
  }
  sort(foundRootFiles.begin(), foundRootFiles.end());
  if (m_verbosity >= Debug &&
      !foundRootFiles.empty()) {
    m_foundFilesIt = foundRootFiles.begin();
    while (m_foundFilesIt != foundRootFiles.end()) {
      cout << "available file: " << *m_foundFilesIt << endl;
      ++m_foundFilesIt;
    }
  }
  return foundRootFiles;
}
path Archive::findFile(const path & dirPath, const string & fileName)
{
  path foundFile("");
  if (exists(dirPath)) {
    // default construction yields past-the-end
    directory_iterator end_itr;
    for (directory_iterator itr(dirPath); itr != end_itr; ++itr) {
      if (is_directory(itr->status())) {
        foundFile = findFile(itr->path(), fileName);
      } else if (itr->path().leaf() == fileName) {
        foundFile = itr->path();
      }
      if (is_regular(foundFile)) { return foundFile; }
    }
  }
  return foundFile;
}
std::vector<path> Archive::findSavesets(const std::string & taskname,
                                        const std::string & endTimeIsoString,
                                        const std::string & durationTimeString)
{
  std::vector<path> foundRootFiles;
  ptime endTime;
  if ("Now" == endTimeIsoString) {
    endTime = ptime(second_clock::local_time());
  } else {
    endTime = ptime(from_iso_string(endTimeIsoString));
  }
  time_duration timeDuration(duration_from_string(durationTimeString));
  ptime startTime = endTime - timeDuration;

// The proper way to do this:
// 1. read savesetdir recursively, into boost::multi_index_container
// 2. index on Taskname and sort on date
// 3. this phase: jump to proper key, iterate next through timestamp until endTime
// 4. inotify tells if the dirlisting needs to be refreshed.

  string taskNameFound;
  string fileTimeFound;
  ptime fileTime;
  TObjArray* fileDateMatchGroup;

  if (!m_foundSavesets.empty()) {
    m_foundSavesetsIt = m_foundSavesets.end(); --m_foundSavesetsIt;
    while (m_foundSavesetsIt >= m_foundSavesets.begin()) {
      fileDateMatchGroup = s_fileDateRegexp.MatchS((*m_foundSavesetsIt).leaf());
      if (!fileDateMatchGroup->IsEmpty()) {
        taskNameFound = (((TObjString *)fileDateMatchGroup->At(1))->GetString()).Data();
        fileTimeFound = (((TObjString *)fileDateMatchGroup->At(2))->GetString()).Data();
        fileTime = (from_iso_string(fileTimeFound));
        if (taskNameFound == taskname && !fileTime.is_not_a_date_time()) {
          if (fileTime <= endTime &&
              fileTime >= startTime) {
            if (m_verbosity >= Verbose) {
              cout << "using file: " << (*m_foundSavesetsIt).leaf() << endl;
            }
            foundRootFiles.push_back(*m_foundSavesetsIt);
          } else if (fileTime < startTime) {
            break;
          }
        }
      }
      fileDateMatchGroup->Delete();
      delete fileDateMatchGroup;
      --m_foundSavesetsIt;
    }
  }
  return foundRootFiles;
}
TH1* Archive::referenceHistogram(const string & referenceDbEntry)
{
  return 0;
}
void Archive::saveAsReferenceHistogram(DbRootHist* histogram)
{
  path referenceFilePath(m_referencePath/histogram->taskName());
    bool out;
    try {
      create_directories(referenceFilePath);
    } catch (const filesystem_error & error) {
      std::cout << "exception: "
                << lookup_error_code(error.system_error())
                << std::endl;
    if (m_verbosity >= Verbose) {}
    }

    std::stringstream refFile;
    refFile << referenceFilePath.file_string() << s_slash
            << histogram->dataType() << s_savesetToken
            << histogram->startRun() << s_rootFileExtension;

    TH1* m_reference = (TH1*) (histogram->rootHistogram)->Clone(histogram->onlineHistogram()->hname().c_str());
    TFile* f = new TFile(refFile.str().c_str(),"UPDATE");
    if (f) {
      if (false == f->IsZombie() ) {
        // keeps old versions.. use Write("",TObject::kOverwrite) to
        // overwrite them
        m_reference->Write();
        f->Write();
        f->Close();
        out = true;
//        histogram->onlineHistogram()->getTask()->setReference(refFile.str());
      }
      delete f;
      f = NULL;
    }
    m_reference->SetDirectory(0);

    if (!out) {
      std::cout << "ERROR in DbRootHist::setReference : could not save reference into file "
                << refFile.str() << std::endl;
    }
}
//void deleteReferenceHistogram(TH1* reference, int revision){}
void Archive::closeRootFiles()
{
//  std::vector<path>::const_iterator foundRootFilesIt;
//  foundRootFilesIt = foundRootFiles.begin();
//  while (foundRootFilesIt != foundRootFiles.end()) {
//    if (rootFile.IsOpen()) {
//      rootFile.Close();
//    }
//    ++foundRootFilesIt;
//  }
}
