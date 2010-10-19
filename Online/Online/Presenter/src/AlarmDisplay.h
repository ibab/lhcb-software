// $Id: AlarmDisplay.h,v 1.4 2010-10-19 13:25:42 ggiacomo Exp $
#ifndef ALARMDISPLAY_H 
#define ALARMDISPLAY_H 1
/** @class AlarmDisplay AlarmDisplay.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2010-05-24
 */
#include <vector>
#include <dim/dic.hxx>
#include "presenter.h"

class TGListTree;
class TGListTreeItem;
class PresenterMainFrame;
class PresenterInformation;

class AlarmDisplay : public DimInfo {
 public: 
  /// Constructor 
  AlarmDisplay(PresenterMainFrame* mainFrame,
               PresenterInformation* presInfo,
               TGListTree* listView) : 
    DimInfo((const char*) "OMA/DBDrivenAnalysisTask/LHCb/MESSAGES", 
	    (char*) ""),
    m_mainFrame(mainFrame),
    m_presenterInfo(presInfo),
    m_enabled(false),
    m_listView(listView),
    m_filterCriteria(AllHistograms),
    m_nAlarms(0), m_nWarning(0), m_nArchived(0) { ; } ;
    
    
    virtual ~AlarmDisplay( ) {} ///< Destructor 
    inline void enable(bool state) {m_enabled=state;}
    inline void setFilterCriteria(FilterCriteria filterCriteria) { m_filterCriteria = filterCriteria;}
    
    void listAlarmsFromHistogramDB();
    void loadSelectedAlarmFromDB(int msgId);
    
    virtual void infoHandler(); 
    
    
 private:
    PresenterMainFrame* m_mainFrame;
    PresenterInformation* m_presenterInfo ;
    bool m_enabled;
    TGListTree* m_listView;
    FilterCriteria m_filterCriteria;
    std::vector<int> m_alarmMessageIDs;
    int m_msgBoxReturnCode;
    int m_nAlarms;
    int m_nWarning;
    int m_nArchived;
};
#endif // ALARMDISPLAY_H
