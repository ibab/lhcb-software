#ifndef PRESENTERAPP_H_
#define PRESENTERAPP_H_

// TApplication presenterApp("Presenter", NULL, NULL);
// m_gui
// m_presenterMode
// m_verbosity
// m_archive
// m_histDB
// omalib
// m_dim
// boost locks
// timers
// main canvas?
// savesetpath, referencepath
// Current TCK : m_trackingTCK
// current Timepoints
// current partition
// about versions

class PresenterApp
{
public:
	PresenterApp();
	virtual ~PresenterApp();
};

#endif /*PRESENTERAPP_H_*/

// conversions:
// histTypes: H1D, MonH1D, 2 ->Gaucho: _pres & other.
// DIM format: CNT/, UTGID (<-> 1:1 mapping)
// gauchocomment
// savesetFormat, saveset folders: FS & .root

//  Archive::
//  SAVESET BROWSING/MERGING
//  (by the way, it would be nice if savesets could also be accessed by run, the aggregated savesets by run are available under /hist/Savesets/ByRun
//  independently on the ones by timestamp, you don't have to worry about the
//  time <-> run conversion)
//  runRange: regexp

//  Archive::setDataAggregation(merge|trend)
//  Archive::setPartition(std::string partition)
//  std::vector<std::string> Archive::listSavesets(std::string taskName, time_t from, time_t to)
//  std::vector<std::string> Archive::listSavesets(std::string taskName, std::string isoTimeFrom, std::string isoTimeTo)
//  std::vector<std::string> Archive::listSavesets(std::string taskName, int fromRun, int toRun)
//  std::string              Archive::lastSaveset(std::string taskName)
//  std::string              Archive::lastRunSaveset(std::string taskName)
//  TH1*                     Archive::findHistogram(std::string histogramId, time_t from, time_t to)
//  TH1*                     Archive::findHistogram(std::string histogramId, std::string isoTimwFrom, std::string isoTimeTo)
//  TH1*                     Archive::findHistogram(std::string histogramId, int fromRun, int toRun)
//  
//  Online:: //wait

//  PresenterDraw::
//  PresenterDraw(histogramDB, dimBrowser, archive):: -> 
//  PresenterDraw::setPartition(std::string partition) <- archive?
//  PresenterDraw::setCurrentTCK(std::string currentTCK) -> refresh()?
//  PresenterDraw::setHistoryMode(merge|trend)
//  PresenterDraw::setDataSource(timestamps|saveset|runRange|online)

//  TPad PresenterDraw::histogram(histogramID) -> histid -> dim* findDim() ? PresenterHistogram::
//  TCanvas PresenterDraw::page(std::string pageName) -> list of histos... PresenterPage::
//          PresenterDraw::releasePage(std::string pageName)
//          PresenterDraw::releaseHistogram(std::string histogramID) -> list of histos TName;n
//          PresenterDraw::cleanGarbage() -> gDirectory-> int ...na, ez alapból 0 TObject::GetUniqueID(), dbKey)
//                        TObject::SetUniqueID(UInt_t uid); UUID->GetInt

//          PresenterDraw::refresh(TCanvas*|TPad*) // ownership... std::map( int TObject::GetUniqueID(), dbKey)
//          PresenterDraw::references(show|hide) -> m_currentTCK
 
