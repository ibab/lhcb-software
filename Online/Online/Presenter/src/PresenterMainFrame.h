#ifndef PRESENTERMAINFRAME_H_
#define PRESENTERMAINFRAME_H_

#include <TGFrame.h>
#include <TGListTree.h>
#include <TColor.h>
#include <vector>

#include <iostream>


#include "LoginDialog.h"
#include "presenter.h"

class TObject;
class TGDockableFrame;
class TString;
class TGButton;
class TGPictureButton;
class TGSplitButton;
class TGFrame;
class TGPopupMenu;
class TGMenuBar;
class TGToolBar;
//struct ToolBarData_t;
class TGStatusBar;
class TGTextView;
class TGPicture;
class TGTab;
class TGComboBox;
class TCanvas;
class TRootEmbeddedCanvas;
class TPad;
class TH1;
class TList;
class TGListTree;
class TGListTreeItem;
class TGVSplitter;
class TObjArray; // for ROOT 5.17

class DbRootHist;
class TDirectory;

class DimBrowser;
class OnlineHistDB;
class OnlineHistoOnPage;
class OMAlib;
class Archive;
class IntervalPicker;
class TDatime;

class stringstream;

class TBenchmark;

class PresenterMainFrame : public TGMainFrame
{
  public:
    PresenterMainFrame(const char* name,
                       const std::string & savesetPath,
                       const std::string & referencePath,
                       const int & x,
                       const int & y,
                       const int & width,
                       const int & height);
    virtual ~PresenterMainFrame();

    // Add button, menu etc. commands to this list (do not define elsewhere:
    // typesafety...)
    enum Command
    {
      X_ENIGMA_COMMAND,
      EXIT_COMMAND,
      SET_DIM_DNS_COMMAND,
      LOGOUT_COMMAND,
      LOGIN_COMMAND,
      CLEAR_PAGE_COMMAND,
      SAVE_PAGE_TO_DB_COMMAND,
      REMOVE_HISTO_FROM_CANVAS_COMMAND,
      CLEAR_HISTOS_COMMAND,
      EDIT_HISTO_COMMAND,
      EDIT_PAGE_PROPERTIES_COMMAND,
      INSPECT_HISTO_COMMAND,
      HISTOGRAM_DESCRIPTION_COMMAND,
      INSPECT_PAGE_COMMAND,
      START_COMMAND,
      STOP_COMMAND,
      AUTO_LAYOUT_COMMAND,
      HISTORY_PLOTS_COMMAND,
      OVERLAY_REFERENCE_HISTO_COMMAND,
      FAST_HITMAP_DRAW_COMMAND,
      PICK_REFERENCE_HISTO_COMMAND,
      SAVE_AS_REFERENCE_HISTO_COMMAND,
      HELP_CONTENTS_COMMAND,
      HELP_ABOUT_COMMAND,
      UNDOCK_PAGE_COMMAND,
      DOCK_ALL_COMMAND,
      ONLINE_MODE_COMMAND,
      OFFLINE_MODE_COMMAND,
      PAGE_EDITOR_ONLINE_MODE_COMMAND,
      PAGE_EDITOR_OFFLINE_MODE_COMMAND,
      M_AddHistoToDB_COMMAND,
      M_AddHistoToPage_COMMAND,
      M_AddDBHistoToPageAsOne_COMMAND,
      M_AddDBHistoToPage_COMMAND,
      M_SetDimSourceFromTree,
      M_SetDimSourceFromQuickButton,
      M_DimCollapseAllChildren_COMMAND,
      M_DeleteDBHisto_COMMAND,
      M_DBHistoCollapseAllChildren_COMMAND,
      M_SetHistoPropertiesInDB_COMMAND,
      M_RefreshHistoDBListTree_COMMAND,
      M_RefreshHistoSvcListTree,
      M_RefreshDBPagesListTree_COMMAND,
      M_RefreshAlarmDBListTree_COMMAND,
      M_LoadPage_COMMAND,
      M_DeletePage_COMMAND,
      M_DeleteFolder_COMMAND,
      M_LAST_1_HOURS,
      M_LAST_8_HOURS,
      M_Last_File,
      M_File_Picker,
      M_Last_Interval,
      M_IntervalPicker,
      M_UtgidPicker,
      M_RefreshKnownPartitions,
      M_Previous_Interval,
      M_Next_Interval
    };

    struct obsolete_BulkHistoOptions {
      TString m_description;
      TString m_genericRootDrawOption;
      TString m_1DRootDrawOption;
      TString m_2DRootDrawOption;
      TString m_xLabel;
      TString m_yLabel;
      TString m_drawPatternTextEntry;
      int m_fillColour;
      int m_lineColour;
      int m_statsOption;
      int m_fillStyle;
      int m_lineWidth;
      int m_lineStyle;
    };
    
    //slots
    
    void startBenchmark(const std::string &timer);
    void stopBenchmark(const std::string &timer);
    void printBenchmark(const std::string &timer);

    void about();
    void buildGUI();
    void CloseWindow();

    void setPresenterMode(const pres::PresenterMode & presenterMode);
    void setDatabaseMode(const pres::DatabaseMode & databaseMode);
    void setHistoryMode(bool mode) { m_historyMode = mode; }
    pres::DatabaseMode databaseMode() { return m_databaseMode; }
    std::string currentPartition() { return m_currentPartition; }
    void setVerbosity(const pres::MsgLevel & verbosity);
    void setTitleFontSize(int fontSize);
    pres::MsgLevel verbosity() const { return m_verbosity; }
    Archive* archive() const { return m_archive; }
    IntervalPicker* intervalPicker() const { return m_intervalPicker; }
    
    void setReferencePath(const std::string & referencePath);
    void setSavesetPath(const std::string & savesetPath);

    void setStartupHistograms(const std::vector<std::string> & histogramList);

    void dockAllFrames();
    void handleCommand(Command id = X_ENIGMA_COMMAND);
    void savePageToHistogramDB();
    void autoCanvasLayout();
    void loginToHistogramDB();
    bool connectToHistogramDB(const std::string & dbPassword,
                              const std::string & dbUsername,
                              const std::string & dbName);
    OMAlib* analysisLib();
    OnlineHistDB* histogramDB();
    void logoutFromHistogramDB();
    void startPageRefresh();
    void stopPageRefresh();
    void clearHistos();
    void nextInterval();
    void previousInterval();
    void refreshPage();
    void refreshClock();
    void listAlarmsFromHistogramDB(TGListTree* listView,
                                   const FilterCriteria & filterCriteria);   
    void listHistogramsFromHistogramDB(TGListTree* listView,
                             const FilterCriteria & filterCriteria,
                             bool histograms);

    void fillTreeNodeWithHistograms(TGListTree* listView,
                                    TGListTreeItem* node,
                                    std::vector<std::string>* localDatabaseIDS,
                                    std::vector<std::string>* histogramTypes);
    std::string histogramDBName();

    void setTreeNodeIcon(TGListTreeItem* node, const std::string & type);
    void setStatusBarText(const char* text, int slice);
    void editHistogramProperties();
    void inspectHistogram();
    void histogramDescription();
    void inspectPage();

    void checkedTreeItems(TGListTree* selected, TGListTree* treeList);
    void checkedTreeItemsChildren(TGListTreeItem* node,
                                  TGListTree* selected);

    void collapseTreeChildrenItems(TGListTree* treeList,
                                   TGListTreeItem* node);
    void collapseTreeChildrenItemsChildren(TGListTree* treeList,
                                           TGListTreeItem* node);

    void sortTreeChildrenItems(TGListTree* treeList,
                               TGListTreeItem* node);
    void sortTreeChildrenItemsChildren(TGListTree* treeList,
                                       TGListTreeItem* node);

    void deleteTreeChildrenItemsUserData(TGListTreeItem* node);
    void deleteTreeChildrenItemsUserDataChildren(TGListTreeItem* node);


    void enableAutoCanvasLayoutBtn();
    void disableAutoCanvasLayoutBtn();

    TRootEmbeddedCanvas* mainEmbCanvas;
    TCanvas*             mainCanvas;

    obsolete_BulkHistoOptions   bulkHistoOptions;

    void refreshHistoDBListTree();
    void refreshPagesDBListTree();
    void refreshHistogramSvcList(bool withTree);
    void listRootHistogramsFrom(TDirectory* rootFile,
                                std::vector<std::string> & histogramList,
                                std::vector<std::string> & histogramTypes,
                                std::string& taskName,
                                pres::SavesetType savesetType);
    void refreshPartitionSelectorPopupMenu();
    void hideDBTools();
    void showDBTools(pres::DatabaseMode databasePermissions);
    void reconfigureGUI();

    void enableHistogramClearing();
    void disableHistogramClearing();
    
    void enableReferenceOverlay();
    void disableReferenceOverlay();
    
    void enablePageRefresh();
    void disablePageRefresh();

    void enablePageUndocking();
    void disablePageUndocking();

    void enablePageLoading();

    void deleteSelectedHistoFromCanvas();
    DbRootHist* selectedDbRootHistogram();

    void clickedHistoSvcTreeItem(TGListTreeItem* node,
                                 EMouseButton btn,
                                 int x, int y);
                                                     
    void clickedHistoDBTreeItem(TGListTreeItem* node,
                                EMouseButton btn,
                                int x, int y);
    void clickedPageTreeItem(TGListTreeItem* node,
                             EMouseButton btn,
                             int x, int y);
    void clickedAlarmTreeItem(TGListTreeItem* node,
                             EMouseButton btn,
                             int x, int y);                             

//    TGPopupMenu* histoSvcTreeContextMenu() const { return m_histoSvcTreeContextMenu; }

    void addHistoToHistoDB();
    void addDimHistosToPage();
    void addDbHistoToPage(bool overlapMode);
    void addHistoToPage(const std::string& histogramUrl, bool overlapMode);
    void setHistogramDimSource(bool tree);
    void dimCollapseAllChildren();

    void setHistogramPropertiesInDB();
    void dbHistoCollapseAllChildren();

    void deleteSelectedHistoFromDB();
    std::string selectedPageFromDbTree();
    int selectedAlarmFromDbTree();
    void loadSelectedAlarmFromDB(int msgId);
    void loadSelectedPageFromDB(const std::string & pageName = "",
                                const std::string & timePoint = "",
                                const std::string & pastDuration = "");
    void deleteSelectedPageFromDB();
    void deleteSelectedFolderFromDB();
    void pickReferenceHistogram();
    void saveSelectedHistogramAsReference();
    void toggleReferenceOverlay();
    void toggleFastHitMapDraw();
    void toggleHistoryPlots();
    void paintHist(DbRootHist* histogram, TPad* overlayOnPad);


    void removeHistogramsFromPage();
    void EventInfo(int event, int px, int py, TObject *selected);

    bool isConnectedToHistogramDB();
    bool canWriteToHistogramDB();
    bool isHistoryTrendPlotMode() { return m_historyTrendPlots; }
    TCanvas* editorCanvas;
    TRootEmbeddedCanvas* editorEmbCanvas;
    
    TDatime* currentTime;
    std::string global_timePoint;
    std::string global_pastDuration;
    std::string global_stepSize;

    std::vector<DbRootHist*>  dbHistosOnPage;
//    std::vector<DbRootHist*>::iterator  dbHistosOnPageIt;

  private:

    pres::MsgLevel    m_verbosity;
    bool              m_historyMode;
    std::string       m_currentPartition;
    std::string       m_currentPageName;
    std::string       m_referencePath;
    std::string       m_savesetPath;
    std::string       m_savesetFileName;
    DimBrowser*       m_dimBrowser;
    Archive*          m_archive;
    IntervalPicker*   m_intervalPicker;
    pres::PresenterMode m_presenterMode;
    pres::PresenterMode m_prevPresenterMode;
    pres::DatabaseMode  m_databaseMode;
    TTimer*           m_pageRefreshTimer;
    TTimer*           m_clockTimer;
    bool              m_clearedHistos;
    bool              m_historyTrendPlots;
    bool              m_referencesOverlayed;
    bool              m_fastHitMapDraw;
    bool              m_refreshingPage;
    OnlineHistDB*     m_histogramDB;
    OMAlib*           m_analysisLib;
    int               m_msgBoxReturnCode;
    std::string       m_dbName;
    std::string       m_message;

    TGDockableFrame*  m_menuDock;
    TGDockableFrame*  m_toolBarDock;
    TGDockableFrame*  m_databaseHistogramsDock;
    TGDockableFrame*  m_databaseAlarmsDock;
    TGDockableFrame*  m_pageDock;
    TGDockableFrame*  m_mainCanvasInfoDock;
    TPad*             m_drawPattern;
    TGDockableFrame*  m_dimBrowserDock;
    TGDockableFrame*  m_databasePagesDock;

    TGGroupFrame*     m_histoSvcBrowserGroupFrame;

    TGStatusBar*      m_mainStatusBar;
    TGStatusBar*      m_statusBarTop;
    TGHorizontalFrame* m_intervalPickerFrame;

    std::string  m_currentPageHistogramHelp;
    TGTextView*  m_pageDescriptionView;
    std::string  m_pageDescription;

    // File menu
    TGToolBar*    m_toolBar;
    TGMenuBar*    m_menuBar;
    TGPopupMenu*  m_fileMenu;
      TGHotString*  m_fileText;
//      TGHotString*  m_filePrint;
      TGHotString*  m_fileNew;
      TGHotString*  m_fileSaveText;
      TGHotString*  m_fileLoginText;
      TGHotString*  m_fileLogoutText;
      TGHotString*  m_fileQuitText;

    TGPopupMenu*  m_editMenu;
    TGHotString*  m_editText;
      TGHotString*  m_editHistogramPropertiesText;
      TGHotString*  m_editPickReferenceHistogramText;
      TGHotString*  m_editSaveSelectedHistogramAsReferenceText;
      TGHotString*  m_editAutoLayoutText;
      TGHotString*  m_editPagePropertiesText;
      TGHotString*  m_editRemoveHistoText;

    TGPopupMenu*  m_viewMenu;
    TGHotString*  m_viewText;
      TGHotString*  m_viewStartRefreshText;
      TGHotString*  m_viewStopRefreshText;
      TGHotString*  m_viewToggleHistoryPlotsText;
      TGHotString*  m_viewToggleReferenceOverlayText;
      TGHotString*  m_viewToggleFastHitMapDrawText;
      TGHotString*  m_viewInspectHistoText;
      TGHotString*  m_viewHistogramDescriptionText;
      TGHotString*  m_viewInspectPageText;
      TGHotString*  m_viewClearHistosText;
      TGHotString*  m_viewUnDockPageText;
      TGHotString*  m_viewDockAllText;

    TGPopupMenu*  m_toolMenu;
    TGHotString*  m_toolText;
      TGHotString*  m_toolMode;
      TGHotString*  m_toolPageEditorOnline;
      TGHotString*  m_toolPageEditorOffline;
      TGHotString*  m_toolOnline;
      TGHotString*  m_toolOffline;
      TGHotString*  m_toolSetUtgidTaskText;
      TGHotString*  m_toolSetDimDnsText;

    TGPopupMenu*  m_helpMenu;
    TGHotString*  m_helpText;
      TGHotString*  m_helpContentsText;
      TGHotString*  m_helpAboutText;

    // Toolbar
//    ToolBarData_t*      m_toolbarElement;
    TGPictureButton*  m_loginButton;
    TGPictureButton*  m_logoutButton;
    TGPictureButton*  m_savePageToDatabaseButton;
    TGPictureButton*  m_newPageButton;
    TGPictureButton*  m_startRefreshButton;
    TGPictureButton*  m_stopRefreshButton;
    TGPictureButton*  m_clearHistoButton;
    TGPictureButton*  m_previousIntervalButton;
    TGPictureButton*  m_nextIntervalButton;
    TGPictureButton*  m_inspectHistoButton;
    TGPictureButton*  m_histogramDescriptionButton;
    TGPictureButton*  m_editHistoButton;
    TGPictureButton*  m_autoCanvasLayoutButton;
    TGPictureButton*  m_deleteHistoFromCanvasButton;
    TGPictureButton*  m_overlayReferenceHistoButton;
    TGPictureButton*  m_historyPlotsButton;
    TGPictureButton*  m_pickReferenceHistoButton;
    TGSplitButton*    m_historyIntervalQuickButton;
    TGPopupMenu*      m_presetTimePopupMenu;
    TGSplitButton*    m_partitionSelectorQuickButton;
    TGPopupMenu*      m_partitionSelectorPopupMenu;


    //  TGButton*         m_quitButton;
    //  TGPicturePool*    m_picturePool;
    const TGPicture*  m_openedFolderIcon;
    const TGPicture*  m_closedFolderIcon;
    const TGPicture*  m_databaseSourceIcon;
    const TGPicture*  m_dimOnline16;
    const TGPicture*  m_iconROOT;
    const TGPicture*  m_iconH1D;
    const TGPicture*  m_iconH2D;
    const TGPicture*  m_iconProfile;
    const TGPicture*  m_iconCounter;
    const TGPicture*  m_iconPage;
    const TGPicture*  m_fitIcon;
    const TGPicture*  m_iconQuestion;
    const TGPicture*  m_iconTask;
    const TGPicture*  m_iconAlgorithm;
    const TGPicture*  m_iconSet;
    const TGPicture*  m_iconLevel;
    const TGPicture*  m_stockNewFormula;
    const TGPicture*  m_iconAlarm;
    const TGPicture*  m_iconWarning;
    const TGPicture*  m_iconInfo;
    const TGPicture*  m_iconAnalysisAlarm;

    TGVerticalFrame* m_rightMiscFrame;
    TGVerticalFrame* m_leftMiscFrame;
    TGHorizontalFrame* m_mainHorizontalFrame;
    TGVSplitter* m_rightVerticalSplitter;

    // vector of colours defined for own colour palette
    std::vector<TColor*>          m_RootColourVector;

    std::vector<std::string>      m_localDatabaseFolders;
    std::vector<std::string>::const_iterator m_folderIt;

    std::vector<std::string>      m_localDatabasePages;
    std::vector<std::string>::const_iterator m_pageIt;

    std::vector<std::string>      m_localDatabaseIDS;
    std::vector<std::string>::const_iterator m_histogramIt;

    std::vector<std::string>      m_histogramTypes;
    std::vector<std::string>::const_iterator m_histogramType;

    std::vector<std::string>      m_tasksNotRunning;
    std::vector<std::string>::const_iterator m_tasksNotRunningIt;

    TGListTreeItem* m_treeNode;
    TGListTreeItem* m_taskNode;
    TGListTreeItem* m_algorithmNode;
    TGListTreeItem* m_histogramSetNode;

    TGListTreeItem* m_histogramNode;
    TGListTreeItem* m_pageNode;

    TObjArray* m_folderItems;
    TIterator* m_folderItemsIt;
    TObject*   m_folderItem;

    TObjArray* m_histogramIdItems;
    TIterator* m_histogramIdItemsIt;
    TObject*   m_histogramIdItem;

    TBenchmark* m_benchmark;

    std::string convDimToHistoID(const std::string & dimSvcName);


    TGCompositeFrame* m_editorCanvasMainFrame;

    int m_histoPadOffset;

    TString m_drawOption; // bulk DO
    TString m_1ddrawOption; // bulk DO
    TString m_2ddrawOption; // bulk DO

    TGListTree*          m_databaseHistogramTreeList;
    TGListTree*          m_alarmHistogramTreeList;
    TGListTree*          m_pagesFromHistoDBListTree;
    TGListTree*          m_histoSvcListTree;
    TGPopupMenu*         m_histoSvcTreeContextMenu;
    TGPopupMenu*         m_histoDBContextMenu;
    TGPopupMenu*         m_alarmDBContextMenu;
    TGPopupMenu*         m_pagesContextMenu;
    TGViewPort*          m_histoDBCanvasViewPort;
    TGViewPort*          m_alarmDBCanvasViewPort;
    TGViewPort*          m_pagesFromHistoDBViewPort;
    TGCanvas*            m_dimSvcListTreeContainterCanvas;
    TGCanvas*            m_histoDBCanvas;
    TGCanvas*            m_alarmDBCanvas;
    TGVerticalFrame*     m_histoDBFilterFrame;
    TGVerticalFrame*     m_alarmDBFilterFrame;
    TGComboBox*          m_histoDBFilterComboBox;
    TGComboBox*          m_alarmDBFilterComboBox;

    std::vector<int> m_alarmMessageIDs;

    std::vector<std::string>      m_knownHistogramServices;
//    std::vector<std::string>::const_iterator m_knownDimServicesIt;
    
    std::vector<std::string>      m_candidateDimServices;
    std::vector<std::string>::const_iterator m_candidateDimServicesIt;

    std::vector<OnlineHistoOnPage*>      m_onlineHistosOnPage;
    std::vector<OnlineHistoOnPage*>::const_iterator m_onlineHistosOnPageIt;

    TList*  m_knownOnlinePartitionList;
    TList*  m_knownHistoryPartitionList;
    TList*  m_knownMonitoringNodeList;
    TList*  m_knownInstanceNumberList;

    ClassDef(PresenterMainFrame, 0) // main editor window
};

R__EXTERN PresenterMainFrame* gPresenter;

#endif /*PRESENTERMAINFRAME_H_*/
