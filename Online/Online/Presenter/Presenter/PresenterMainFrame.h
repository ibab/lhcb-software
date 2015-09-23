#ifndef PRESENTERMAINFRAME_H_
#define PRESENTERMAINFRAME_H_

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TGFrame.h>
#include <TGListTree.h>
#include <TColor.h>
#ifdef WIN32
#pragma warning( pop )
#endif

#include <vector>
#include <map>

#include "Presenter/PresenterInformation.h"
#include "Presenter/presenter.h"

#include "Presenter/PresenterPage.h"

class TObject;
class TGDockableFrame;
class TString;
class TGButton;
class TGPictureButton;
class TGTextButton;
class TGFrame;
class TGString;
class TGPopupMenu;
class TGMenuBar;
class TGToolBar;
class TGStatusBar;
class PageDescriptionTextView;
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
class TGHSplitter;
class TObjArray;
class TGTextView ;

class TDirectory;

class DimBrowser;
class OnlineHistDB;
class OnlineHistoOnPage;
class OMAlib;
class AlarmDisplay;
class Archive;
class IntervalPicker;
class TGLabel ;

class stringstream;

class OnlineHistPage ;
class RunDB ;
class IntervalPickerData ;

class PresenterMainFrame : public TGMainFrame {
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
      LOAD_NEXT_PAGE_COMMAND,
      LOAD_PREVIOUS_PAGE_COMMAND,
      CLEAR_PAGE_COMMAND,
      SAVE_PAGE_TO_FILE_COMMAND,
      SAVE_PAGE_TO_DB_COMMAND,
      REPORT_COMMAND,
      REMOVE_HISTO_FROM_CANVAS_COMMAND,
      CLEAR_HISTOS_COMMAND,
      EDIT_HISTO_COMMAND,
      EDIT_PAGE_PROPERTIES_COMMAND,
      EDIT_ADD_TRENDINGHISTO_COMMAND ,
      INSPECT_HISTO_COMMAND,
      HISTOGRAM_DESCRIPTION_COMMAND,
      INSPECT_PAGE_COMMAND,
      START_COMMAND,
      STOP_COMMAND,
      HISTORY_PLOTS_COMMAND,
      OVERLAY_REFERENCE_HISTO_COMMAND,
      FAST_HITMAP_DRAW_COMMAND,
      SHOW_ALARM_LIST_COMMAND,
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
      M_Move_COMMAND,
      M_DeletePage_COMMAND,
      M_CreateFolder_COMMAND,
      M_RecentHistory,
      M_File_Picker,
      M_IntervalPicker,
      M_IntervalPickerRun ,
      M_UtgidPicker,
      M_PartitionList,
      M_Previous_Interval,
      M_Next_Interval,
      SET_REFERENCE_COMMAND,
      M_TrendLastTenMinutes,
      M_TrendLastTwoHours,
      M_TrendLastDay,
      M_TrendLastWeek,
      M_TrendLastMonth,
      M_TrendLastYear,
      M_TrendAll
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

  void about();
  void buildGUI();
  void dataDropped(TGListTreeItem* item, TDNDData* data);
  virtual void CloseWindow();

  /// Set the presenter Mode
  void setPresenterMode(const pres::PresenterMode & presenterMode);

  void setDisplayModeVariable(const pres::DisplayMode & displayMode) { m_displayMode = displayMode;}

  void displaySimpleHistos ( );

  const pres::DisplayMode & displayMode() {return m_displayMode; }
  void setDatabaseMode(const pres::DatabaseMode & databaseMode);

  pres::DatabaseMode databaseMode() { return m_databaseMode; }

  void setVerbosity(const pres::MsgLevel & verbosity); ///< Set verbosity level

  /// Set logbook configuration
  void setLogbookConfig(const std::string & logBookConfig) { m_logBookConfig = logBookConfig ; }
  std::string logbookConfig() { return m_logBookConfig; }

  /// Set the problem database configuration
  void setPbdbConfig(const std::string & pbdbConfig) { m_pbdbConfig = pbdbConfig ; } ;

  /// Set name of run database web host
  void setRundbConfig(const std::string & rundbConfig) {
    m_rundbConfig = rundbConfig ; } ;

  /// Presenter verbosity
  pres::MsgLevel verbosity() const { return m_verbosity; }

  /// Link to archive object
  Archive* archive() const { return m_archive; }

  /// Link to interval picker object
  IntervalPicker* intervalPicker() const { return m_intervalPicker; }

  /// return the saveset file name
  std::string& savesetFileName() {return m_savesetFileName;}

  /// set the saveset file name
  void setSavesetFileName(const std::string & svsFilename) { m_savesetFileName = svsFilename;}

  void setKnownDatabases(const std::string & databasesCfg,
                         const std::string & dbCredentials);


  void setStartupHistograms(const std::vector<std::string> & histogramList);

  void dockAllFrames();
  void handleCommand(Int_t id = X_ENIGMA_COMMAND);
  void savePageToFile();
  void savePageToHistogramDB();
  void reportToLog();
  void loginToHistogramDB( bool write = false );
  bool connectToHistogramDB(const std::string & dbPassword,
                            const std::string & dbUsername,
                            const std::string & dbName);

  /// Pointer to analysis lib
  OMAlib* analysisLib() const { return m_analysisLib ; } ;

  /// Pointer to histogram db
  OnlineHistDB* histogramDB();

  /// Pointer to dim browser
  DimBrowser* dimBrowser() const { return m_dimBrowser ; } ;

  void logoutFromHistogramDB();
  void startPageRefresh();
  void stopPageRefresh();
  void clearHistos();
  void unclearHistosIfNeeded() {if (m_clearedHistos) clearHistos();}
  void clearAlarmPages() { m_alarmPages.clear();}
  void clearAlarm( Int_t item );
  void loadNextPage();
  void loadPreviousPage();
  void nextInterval();
  void previousInterval();
  void refreshPageForced();
  void refreshPage();
  void refreshClock();
  void listHistogramsFromHistogramDB(TGListTree* listView,
                                     const FilterCriteria & filterCriteria,
                                     bool histograms);
  void fillTreeNodeWithHistograms(TGListTree* listView,
                                  TGListTreeItem* node,
                                  std::vector<std::string>* localDatabaseIDS,
                                  std::vector<std::string>* histogramTypes);

  /// Return the histogram database name
  std::string histogramDBName() const { return m_dbName ; } ;

  void setTreeNodeType(TGListTreeItem* node, const std::string & type);
  const char* getStatusBarText(int slice=0);
  void setStatusBarText(const char* text, int slice=0);
  void setTopStatusBarText(const char* text, int slice=0);
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

  void setReference();

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
  void partitionSelectorComboBoxHandler(int partitionNumber = 0);
  void hideDBTools();
  void showDBTools(pres::DatabaseMode databasePermissions);
  void reconfigureGUI();

  void enableReferenceOverlay();
  void disableReferenceOverlay();

  /// Enable page undocking
  void enablePageUndocking()  ;

  /// Disable page undocking
  void disablePageUndocking()  ;

  /// Enable page loading
  void enablePageLoading()  ;

  void deleteSelectedHistoFromCanvas();
  void addTrendingHisto() ;

  DisplayHistogram* selectedDisplayHistogram();

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


  void addHistoToHistoDB();
  void addDimHistosToPage();
  void addDbHistoToPage();
  void addHistoToPage( const std::string& histogramUrl );

  void dimCollapseAllChildren();

  void setHistogramPropertiesInDB();
  void dbHistoCollapseAllChildren();

  void deleteSelectedHistoFromDB();
  std::string selectedPageFromDbTree();
  void loadAllPages();
  void loadSelectedPageFromDB(const std::string & pageName ,
                              const std::string & timePoint ,
                              const std::string & pastDuration );
  PageDescriptionTextView*  pageDescriptionView() { return m_pageDescriptionView;}
  void moveSelectedInDB();
  void deleteSelectedPageFromDB();
  void createFolderInDB();
  void toggleReferenceOverlay();
  void enableAlarmDisplay(bool mode);
  void enableEditing( bool mode ) { m_editingAllowed = mode; }
  void toggleShowAlarmList();

  void toggleFastHitMapDraw();
  void toggleHistoryPlots();

  void removeHistogramsFromPage();
  void EventInfo(int event, int px, int py, TObject *selected);

  bool isConnectedToHistogramDB();
  bool canWriteToHistogramDB();

  bool referencesOverlayed() { return m_referencesOverlayed;}

  TCanvas* editorCanvas;
  TRootEmbeddedCanvas* editorEmbCanvas;

  /// Load a web page
  void loadWebPage( Int_t item ) ;

  /// set partition
  void setPartition( const std::string & partition ) {
    m_presenterInfo.setPartition( partition ) ; } ;

  /// get presenter mode
  const pres::PresenterMode & presenterMode( ) const {
    return m_presenterInfo.presenterMode() ; } ;

  /// get current partition
  std::string currentPartition( ) const {
    return m_presenterInfo.currentPartition( ) ; } ;

  /// set the presenter mode
  void setPresenterModeVariable( const pres::PresenterMode & mode ) {
    m_presenterInfo.setPresenterMode( mode ) ; };

  /// set the current page name
  void setCurrentPageName( const std::string & pageName ) {
    m_currentPageName = pageName ; } ;

  /// Set the path for images
  void setImagePath(const std::string & imagePath) {
    m_imagePath = imagePath; };

  /// Set format for image saving
  void setDumpFormat(const std::string & dumpFormat) {
    m_dumpFormat = dumpFormat; } ;

  PresenterPage& myPage() { return m_presenterPage; }

  void reAccessPage();  // After an alarm, if refresh, reload from database.

  void setOfflineContext( bool ctxt, std::string processing, std::string eventType )  {
    m_presenterInfo.setOfflineContext( ctxt );
    m_presenterInfo.setProcessing( processing );
    m_presenterInfo.setEventType( eventType );
  }

  void getDatabaseWriter( std::string& pass, std::string& user, std::string& name );

  void setShiftCrew( bool crew ) { m_shiftCrew = crew; }

 private:
  UInt_t            m_initWidth;
  UInt_t            m_initHeight;
  pres::MsgLevel    m_verbosity;
  std::string       m_logBookConfig;
  std::string       m_pbdbConfig;
  std::string       m_rundbConfig ;
  bool              m_resumePageRefreshAfterLoading;
  bool              m_loadingPage;
  std::string       m_currentPageName;
  std::string       m_referencePath;
  std::string       m_savesetPath;
  std::string       m_imagePath;
  std::string       m_dumpFormat;
  std::string       m_savesetFileName;
  DimBrowser*       m_dimBrowser;
  Archive*          m_archive;
  IntervalPicker*   m_intervalPicker;
  pres::PresenterMode m_prevPresenterMode;
  pres::DatabaseMode  m_databaseMode;
  pres::DisplayMode m_displayMode;
  TTimer*           m_pageRefreshTimer;
  TTimer*           m_clockTimer;
  bool              m_clearedHistos;
  bool              m_referencesOverlayed;
  bool              m_fastHitMapDraw;
  bool              m_refreshingPage;
  OnlineHistDB*     m_histogramDB;
  OMAlib*           m_analysisLib;
  bool              m_alarmDisplayEnabled;
  AlarmDisplay*     m_alarmDisplay;
  int               m_msgBoxReturnCode;
  std::string       m_dbName;
  std::string       m_message;

  bool m_editingAllowed;
  bool m_shiftCrew;

  void cleanHistogramDB();

  std::vector<std::string*> m_knownDatabases;
  std::map<std::string*, std::string*> m_knownDbCredentials;

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

  TGVerticalFrame* m_leftDatabaseAlarmFrame;

  std::string  m_currentPageHistogramHelp;
  PageDescriptionTextView*  m_pageDescriptionView;

  // File menu
  TGToolBar*    m_toolBar;
  TGLabel  *    m_toolBarLabel ;
  TGTextView *  m_textNavigation ;
  TGMenuBar*    m_menuBar;
  TGPopupMenu*  m_fileMenu;
  TGHotString*  m_fileText;
  TGHotString*  m_filePrint;
  TGHotString*  m_fileNew;
  TGHotString*  m_fileSaveText;
  TGHotString*  m_fileLoginText;
  TGHotString*  m_fileLogoutText;
  TGHotString*  m_fileQuitText;

  TGPopupMenu*  m_editMenu;
  TGHotString*  m_editText;
  TGHotString*  m_editHistogramPropertiesText;
  TGHotString*  m_editPagePropertiesText;
  TGHotString*  m_editAddTrendingHistoText ;
  TGHotString*  m_editRemoveHistoText;

  TGPopupMenu*  m_viewMenu;
  TGHotString*  m_viewText;
  TGHotString*  m_viewStartRefreshText;
  TGHotString*  m_viewStopRefreshText;
  TGHotString*  m_viewToggleHistoryPlotsText;
  TGHotString*  m_viewToggleReferenceOverlayText;
  TGHotString*  m_viewToggleFastHitMapDrawText;
  TGHotString*  m_viewAlarmListText;
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
  TGPictureButton*  m_reportButton;
  TGPictureButton*  m_printSaveButton;
  TGPictureButton*  m_previousPageSaveButton;
  TGPictureButton*  m_nextPageButton;
  TGPictureButton*  m_newPageButton;
  TGPictureButton*  m_startRefreshButton;
  TGPictureButton*  m_stopRefreshButton;
  TGPictureButton*  m_clearHistoButton;
  TGPictureButton*  m_previousIntervalButton;
  TGPictureButton*  m_nextIntervalButton;
  TGPictureButton*  m_inspectHistoButton;
  TGPictureButton*  m_histogramDescriptionButton;
  TGPictureButton*  m_editHistoButton;
  TGPictureButton*  m_deleteHistoFromCanvasButton;
  TGPictureButton*  m_overlayReferenceHistoButton;
  TGPictureButton*  m_historyPlotsButton;
  TGComboBox*       m_historyIntervalComboBox;
  TGComboBox*       m_trendDurationComboBox;
  TGComboBox*       m_partitionSelectorComboBox;
  TGPictureButton*  m_setReferenceButton;

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
  const TGPicture*  m_iconTrend;

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

  RunDB * m_runDb ; ///< Interface to run database

  std::string m_weblink ; ///< Name of web link to open

  std::vector<std::string> m_histoTimerName;

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

  std::vector<std::string>      m_knownHistogramServices;
  std::vector<std::string>      m_candidateDimServices;
  std::vector<std::string>::const_iterator m_candidateDimServicesIt;

  std::vector<std::string>      m_groupPages;
  std::vector<std::string>::const_iterator m_groupPagesIt;
  std::vector<int>              m_alarmPages;
  std::vector<int>::const_iterator m_alarmPagesIt;

  TList*  m_knownOnlinePartitionList;
  TList*  m_knownHistoryPartitionList;
  TList*  m_knownMonitoringNodeList;
  TList*  m_knownInstanceNumberList;

  /// Popup menu for right click on histo
  TGPopupMenu * m_histomenu ;

  IntervalPickerData * m_intervalPickerData ;

  /// Presenter Information to transfer to other objects
  PresenterInformation m_presenterInfo ;

  PresenterPage m_presenterPage;  ///< Contains the current page description and contents
  unsigned int m_trendDuration;
  unsigned int m_trendEnd;
  bool m_reAccess;
  bool m_idle;
  std::string m_oldProcessing;
  std::string m_oldEventType;

  /// Display page name in status bar and load comments
  void displayStatusAndComments( OnlineHistPage * page ) ;

  /// Change main window for run navigation
  void switchToRunNavigation( bool on ) ;

  /// Open and select the given page in the histogram tree
  TGListTreeItem * openHistogramTreeAt( const std::string & pageName ) ;

  /// Delete the node and all its children
  void deleteTreeChildrenItemsUserData(TGListTreeItem* node);

  /// Delete recursively the children of this node
  void deleteTreeChildrenItemsUserDataChildren(TGListTreeItem* node);

  /// Fill vector of page names for page navigation
  void fillGroupPages( TGListTreeItem * node ) ;

  /// is in batch mode
  bool isBatch( ) const { return ( pres::Batch == presenterMode() ) ; } ;

  void setDimDns();

  std::string timeStamp();

  ClassDef(PresenterMainFrame, 0) // main editor window
};

extern PresenterMainFrame* gPresenter;

#endif /*PRESENTERMAINFRAME_H_*/
