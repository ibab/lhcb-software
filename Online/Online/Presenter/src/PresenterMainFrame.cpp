// $Id: PresenterMainFrame.cpp,v 1.341 2010-10-01 12:54:11 frankb Exp $
// This class
#include "PresenterMainFrame.h"

// STL
#include <algorithm>
#include <iostream>
#include <vector>

// ROOT include
#include <TApplication.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGResourcePool.h>
#include <TG3DLine.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGDockableFrame.h>
#include <TGLabel.h>
#include <TGListTree.h>
#include <TGMenu.h>
#include <TGMsgBox.h>
#include <TGInputDialog.h>
#include <TGPicture.h>
#include <TGSplitter.h>
#include <TGStatusBar.h>
#include <TGTextEdit.h>
#include <TGToolBar.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TError.h>
#include <TObjArray.h>
#include <TObject.h>
#include <TKey.h>
#include <TPad.h>
#include <TRootEmbeddedCanvas.h>
#include <TString.h>
#include <TObjString.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TTimer.h>
#include <TGFileDialog.h>
#include <TRootHelpDialog.h>
#include <TBenchmark.h> 
#include <TGraph.h>

// BOOST
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// Online
#include "dim/dic.hxx"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAlib.h"
#include "Gaucho/MonObject.h"

// Local
#include "DbRootHist.h"
#include "HistogramIdentifier.h"
#include "LoginDialog.h"
#include "DatabasePagePathDialog.h"
#include "HistoPropDialog.h"
#include "Archive.h"
#include "ParallelWait.h"
#include "ReferencePicker.h"
#include "IntervalPicker.h"
#include "../icons/presenter32.xpm"
#include "icons.h"
#include "Elog.h"
#include "ElogDialog.h"
#include "ShiftDB.h"
#include "KnownProblemList.h"
#include "ProblemDB.h"
#include "RunDB.h"
#include "PageDescriptionTextView.h"
#include "AlarmDisplay.h"
#include "CreateTrendingHistogramDialog.h"
#include "TrendingHistogram.h"

PresenterMainFrame* gPresenter = 0;

ClassImp(PresenterMainFrame);

//==============================================================================
// Constructor
//==============================================================================
PresenterMainFrame::PresenterMainFrame(const char* name,
                                       const std::string & savesetPath,
                                       const std::string & referencePath,
                                       const int & x,
                                       const int & y,
                                       const int & width,
                                       const int & height) :
  TGMainFrame(gClient->GetRoot(), width, height, kLHintsExpandX | 
              kLHintsExpandY),
  m_initWidth(width),
  m_initHeight(height),
  m_verbosity(pres::Silent),
  m_logBookConfig(""),
  m_pbdbConfig(""),
  m_rundbConfig( "" ) ,
  m_resumePageRefreshAfterLoading(false),
  m_loadingPage(false),
  m_currentPartition(pres::s_lhcbPartitionName.Data()),
  m_currentPageName(""),
  m_referencePath(referencePath),
  m_savesetPath(savesetPath),
  m_imagePath(gSystem->TempDirectory()),
  m_savesetFileName(""),
  m_archive(NULL),
  m_prevPresenterMode(pres::History),
  m_databaseMode(pres::LoggedOut),
  m_displayMode(pres::None),
  m_pageRefreshTimer(NULL),
  m_clockTimer(NULL),
  m_clearedHistos(false),
  m_referencesOverlayed(false),
  m_fastHitMapDraw(false),
  m_refreshingPage(false),
  m_histogramDB(NULL),
  m_analysisLib(NULL),
  m_alarmDisplayEnabled(true),
  m_alarmDisplay(NULL),
  m_msgBoxReturnCode(0),
  m_menuDock(NULL),
  m_toolBarDock(NULL),
  m_databaseHistogramsDock(NULL),
  m_pageDock(NULL),
  m_mainCanvasInfoDock(NULL),
  m_drawPattern(NULL),
  m_dimBrowserDock(NULL),
  m_databasePagesDock(NULL),
  m_histoSvcBrowserGroupFrame(NULL),
  m_treeNode(NULL),
  m_taskNode(NULL),
  m_algorithmNode(NULL),
  m_histogramSetNode(NULL),
  m_histogramNode(NULL),
  m_pageNode(NULL),
  m_folderItems(NULL),
  m_folderItemsIt(NULL),
  m_folderItem(NULL),
  m_benchmark(NULL),
  m_deadTasksOnPage(0),
  m_runDb( 0 ) ,
  m_intervalPickerData(NULL) ,
  m_trendingHisto( 0 ) {

  m_benchmark = new TBenchmark();
  SetCleanup(kDeepCleanup);

  // Thermo palette for hitmaps
  gStyle->SetPalette(1);

  gStyle->SetOptStat( pres::s_defStatOptions ) ; 

  gStyle->SetFrameFillColor(10);
  gStyle->SetStatStyle(0);
  
  //== Added 17/03/2010: Make Calo plot nicer.
  gStyle->SetPaintTextFormat("3.0f");

  // only one presenter session allowed
  if (gPresenter) { return; }

  if (!m_pageRefreshTimer) m_pageRefreshTimer = 
    new TTimer( pres::s_pageRefreshRate ) ;

  m_pageRefreshTimer->TurnOff();
  m_pageRefreshTimer->Connect("Timeout()", "PresenterMainFrame", 
                              this, "refreshPage()");

  if (!m_clockTimer) { m_clockTimer = new TTimer(1000); }
  m_clockTimer->TurnOn();
  m_clockTimer->Connect("Timeout()", "PresenterMainFrame", this, 
                        "refreshClock()");

  SetWindowName(name);
  Move(x, y);
  gPresenter = this;

  m_knownHistogramServices.reserve( pres::s_estimatedDimServiceCount ) ;
  m_candidateDimServices.reserve( pres::s_estimatedDimServiceCount ) ;
  m_histogramTypes.reserve( pres::s_estimatedDimServiceCount ) ;
  dbHistosOnPage.reserve( pres::s_estimatedHistosOnPage ) ;

  m_knownOnlinePartitionList = new TList();
  m_knownHistoryPartitionList = new TList();

  if (m_verbosity >= pres::Verbose) {
    std::cout << "refRoot: "  << m_referencePath << std::endl
              << "savesetRoot: "  << m_savesetPath << std::endl;
  }

  m_dimBrowser = new DimBrowser();
  char *dimFormat;
  char *dimService;
  m_dimBrowser->getServices(s_Dim_Dns_Version_Number.c_str());
  if (0 == m_dimBrowser->getNextService(dimService, dimFormat)) {
    m_message = "Sorry, no DIM server found.";
    if (m_verbosity >= pres::Verbose) {
      std::cout << m_message << std::endl;
    }
  } else refreshHistogramSvcList( pres::s_withoutTree ) ;

  m_archive = new Archive( &m_presenterInfo , m_savesetPath , 
                           m_referencePath ) ;
  if (0 != m_archive) { m_archive->setVerbosity(m_verbosity); }
}

//==============================================================================
//  Destructor.
//==============================================================================
PresenterMainFrame::~PresenterMainFrame() {
  TGListTreeItem* node = m_histoSvcListTree->GetFirstItem();
  deleteTreeChildrenItemsUserData(node);
  m_histoSvcListTree->DeleteChildren(node);

  node = m_databaseHistogramTreeList->GetFirstItem();
  deleteTreeChildrenItemsUserData(node);
  m_databaseHistogramTreeList->DeleteChildren(node);

  node = m_pagesFromHistoDBListTree->GetFirstItem();
  deleteTreeChildrenItemsUserData(node);
  m_pagesFromHistoDBListTree->DeleteChildren(node);

  removeHistogramsFromPage();
  Cleanup();

  if (0 != m_alarmDisplay) { delete m_alarmDisplay; m_alarmDisplay = NULL; }
  cleanHistogramDB();

  if (0 != m_dimBrowser) { delete m_dimBrowser; m_dimBrowser = NULL; }
  if (0 != m_archive) { delete m_archive; m_archive = NULL; }


  std::vector<std::string*>::iterator dbString;
  for (dbString = m_knownDatabases.begin();dbString != m_knownDatabases.end(); 
       ++dbString) delete *dbString;

  std::map<std::string*, std::string*>::iterator dbCreds;
  for (dbCreds = m_knownDbCredentials.begin();
       dbCreds != m_knownDbCredentials.end(); ++dbCreds) {
    delete (*dbCreds).first;
    delete (*dbCreds).second;
  }

  if (0 != m_intervalPickerData) {
    delete m_intervalPickerData ; m_intervalPickerData = NULL;
  }
  if (0 != m_clockTimer) { delete m_clockTimer; m_clockTimer = NULL; }
  if (0 != m_pageRefreshTimer) {
    delete m_pageRefreshTimer;
    m_pageRefreshTimer = NULL;
  }
  if (0 != m_knownOnlinePartitionList) {
    m_knownOnlinePartitionList->Delete();
    delete m_knownOnlinePartitionList;
    m_knownOnlinePartitionList = NULL;
  }
  if (0 != m_knownHistoryPartitionList) {
    m_knownHistoryPartitionList->Delete();
    delete m_knownHistoryPartitionList;
    m_knownHistoryPartitionList = NULL;
  }
  if (0 != m_benchmark) { delete m_benchmark; m_benchmark = NULL; }

  if (gPresenter == this) { gPresenter = 0; }
}

//==============================================================================
// Clean the histogram DB
//==============================================================================
void PresenterMainFrame::cleanHistogramDB() {
  if (0 != m_analysisLib) { delete m_analysisLib; m_analysisLib = NULL; }
  if (0 != m_histogramDB) { delete m_histogramDB; m_histogramDB = NULL; }
  if (0 != m_alarmDisplay) { m_alarmDisplay->enable(false); }
}

//==============================================================================
//  Build the graphical interface. A lot of ROOT calls !!!
//==============================================================================
void PresenterMainFrame::buildGUI() {
  if ( ( pres::EditorOnline == presenterMode() ) ||
       ( pres::EditorOffline == presenterMode() ) ||
       ( pres::Online == presenterMode() ) ||
       ( pres::History == presenterMode() ) ) {
    SetCleanup(kDeepCleanup);
    TGPicturePool* picpool = gClient->GetResourcePool()->GetPicturePool();
    SetIconPixmap((char**)presenter32);
    // set minimum size
    // SetWMSizeHints(400 + 200, 370+50, 2000, 1000, 1, 1);
    m_fileText = new TGHotString("&File");
    m_filePrint = new TGHotString("Save/&Print Page to File...");
    m_fileNew = new TGHotString("&Create Page");
    m_fileSaveText = new TGHotString("&Save Page to Database...");
    m_fileLoginText = new TGHotString("&Login to Database...");
    m_fileLogoutText = new TGHotString("Log&out...");
    m_fileQuitText = new TGHotString("&Quit");

    m_editText = new TGHotString("&Edit");
    m_editAutoLayoutText = new TGHotString("Automatic Histogram &Layout");
    m_editHistogramPropertiesText = new TGHotString("&Histogram Properties...");
    m_editRemoveHistoText = new TGHotString("&Delete Histogram from Page");
    m_editPickReferenceHistogramText = 
      new TGHotString("Set &Reference Histogram...");
    m_editSaveSelectedHistogramAsReferenceText = 
      new TGHotString("&Save as Reference Histogram");
    m_editPagePropertiesText = new TGHotString("&Page Properties...");
    m_editAddTrendingHistoText = 
      new TGHotString("&Create New Trending Histogram...");

    m_viewText = new TGHotString("&View");
    m_viewStartRefreshText = new TGHotString("Start page &refresh");
    m_viewStopRefreshText = new TGHotString("&Stop page refresh");
    m_viewToggleHistoryPlotsText = new TGHotString("&History plots");
    m_viewToggleReferenceOverlayText = new TGHotString("&Overlay reference");
    m_viewToggleFastHitMapDrawText = new TGHotString("&Fast hitmap draw");
    m_viewAlarmListText = new TGHotString("Show &alarm list");
    m_viewKnownProblemListText = new TGHotString( "Show &known problems" );
    m_viewInspectHistoText = new TGHotString("&Inspect histogram");
    m_viewHistogramDescriptionText = new TGHotString("Histogram &description");
    m_viewInspectPageText  = new TGHotString("Inspect &page");
    m_viewClearHistosText = new TGHotString("&Clear histograms");
    m_viewUnDockPageText = new TGHotString("&Undock Page");
    m_viewDockAllText = new TGHotString("&Dock All");

    m_toolText = new TGHotString("&Tools");
    m_toolMode = new TGHotString("&Mode");
    m_toolPageEditorOnline = new TGHotString("Page Editor Online");
    m_toolPageEditorOffline  = new TGHotString("Page Editor Offline");
    m_toolOnline = new TGHotString("&Online");
    m_toolOffline = new TGHotString("&History mode");
    //    m_toolSetUtgidTaskText = new TGHotString("Set &UTGID for Task...");
    m_toolSetDimDnsText = new TGHotString("Set &DIM DNS...");
    m_toolFullBenchmarkText = new TGHotString("Benchmark all pages");

    m_helpText = new TGHotString("&Help");
    m_helpContentsText = new TGHotString("&Contents");
    m_helpAboutText = new TGHotString("&About");

    // list tree
    m_openedFolderIcon = 
      picpool->GetPicture("folderOpen16",(char**)folderOpen16);
    m_closedFolderIcon = picpool->GetPicture("folder16",(char**)folder16);

    // Histo alarms
    m_iconAlarm = picpool->GetPicture("mb_stop_s",(char**)mb_stop_s);
    m_iconWarning = picpool->GetPicture("mb_exclamation_s",
                                        (char**)mb_exclamation_s);
    m_iconInfo = picpool->GetPicture("mb_asterisk_s",(char**)mb_asterisk_s);
    m_iconAnalysisAlarm = gClient->GetPicture("ps_t.xpm");

    m_databaseSourceIcon =  picpool->GetPicture("databaseSource16",
                                                (char**)databaseSource16);
    m_dimOnline16 =  picpool->GetPicture("dimOnline16",(char**)dimOnline16);
    m_iconROOT = gClient->GetPicture("root_t.xpm");

    m_iconH1D = gClient->GetPicture("h1_t.xpm");
    m_iconH2D = gClient->GetPicture("h2_t.xpm");
    m_iconProfile = gClient->GetPicture("profile_t.xpm");
    m_iconCounter = gClient->GetPicture("c_src_t.xpm");
    m_iconPage = gClient->GetPicture("bld_paste_into.png");
    m_iconQuestion = gClient->GetPicture("mdi_help.xpm");

    m_iconTask = gClient->GetPicture("tmacro_t.xpm");
    m_iconAlgorithm = gClient->GetPicture("expression_t.xpm");
    //m_iconSet = gClient->GetPicture("pack-empty.xpm");
    m_iconSet = gClient->GetPicture("pack-empty_t.xpm");
    m_iconLevel = gClient->GetPicture("bld_vbox.png");

    m_stockNewFormula =  picpool->GetPicture("stock_new_formula_xpm", 
					     (char**)stock_new_formula_xpm);

    // File menu
    m_fileMenu = new TGPopupMenu(fClient->GetRoot());
    m_fileMenu->AddEntry(m_fileNew, CLEAR_PAGE_COMMAND);
    m_fileMenu->AddSeparator();
    m_fileMenu->AddEntry(m_fileSaveText, SAVE_PAGE_TO_DB_COMMAND);
    m_fileMenu->AddEntry(m_filePrint, SAVE_PAGE_TO_FILE_COMMAND);
    m_fileMenu->AddSeparator();
    m_fileMenu->AddEntry(m_fileLoginText, LOGIN_COMMAND);
    m_fileMenu->AddEntry(m_fileLogoutText, LOGOUT_COMMAND);
    m_fileMenu->AddSeparator();
    m_fileMenu->AddEntry(m_fileQuitText, EXIT_COMMAND);
    m_fileMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                        this, "handleCommand(Command)");

    // Edit menu
    m_editMenu = new TGPopupMenu(fClient->GetRoot());
    m_editMenu->AddEntry(m_editAutoLayoutText, AUTO_LAYOUT_COMMAND);
    m_editMenu->AddEntry(m_editHistogramPropertiesText, EDIT_HISTO_COMMAND);
    m_editMenu->AddEntry(m_editRemoveHistoText,
                         REMOVE_HISTO_FROM_CANVAS_COMMAND);
    m_editMenu->AddEntry(m_editPickReferenceHistogramText,
                         PICK_REFERENCE_HISTO_COMMAND);
    m_editMenu->AddEntry(m_editSaveSelectedHistogramAsReferenceText,
                         SAVE_AS_REFERENCE_HISTO_COMMAND);
    m_editMenu->AddEntry(m_editPagePropertiesText,
                         EDIT_PAGE_PROPERTIES_COMMAND);
    m_editMenu -> AddEntry( m_editAddTrendingHistoText , 
			    EDIT_ADD_TRENDINGHISTO_COMMAND ) ;
    m_editMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                        this, "handleCommand(Command)");
    m_editMenu->DisableEntry(PICK_REFERENCE_HISTO_COMMAND);
    m_editMenu->DisableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);
    m_editMenu->DisableEntry(EDIT_PAGE_PROPERTIES_COMMAND);

    // View menu
    m_viewMenu = new TGPopupMenu(fClient->GetRoot());
    m_viewMenu->AddEntry(m_viewInspectHistoText, INSPECT_HISTO_COMMAND);
    m_viewMenu->AddEntry(m_viewHistogramDescriptionText, 
                         HISTOGRAM_DESCRIPTION_COMMAND);
    m_viewMenu->AddEntry(m_viewInspectPageText, INSPECT_PAGE_COMMAND);
    m_viewMenu->AddEntry(m_viewStartRefreshText, START_COMMAND);
    m_viewMenu->AddEntry(m_viewStopRefreshText, STOP_COMMAND);
    m_viewMenu->DisableEntry(STOP_COMMAND);
    m_viewMenu->AddEntry(m_viewToggleHistoryPlotsText,
                         HISTORY_PLOTS_COMMAND);
    m_viewMenu->UnCheckEntry(HISTORY_PLOTS_COMMAND);
    m_viewMenu->AddEntry(m_viewToggleReferenceOverlayText,
                         OVERLAY_REFERENCE_HISTO_COMMAND);
    m_viewMenu->AddEntry(m_viewToggleFastHitMapDrawText,
                         FAST_HITMAP_DRAW_COMMAND);

    m_viewMenu->UnCheckEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
    m_viewMenu->UnCheckEntry(FAST_HITMAP_DRAW_COMMAND);
    m_viewMenu->AddEntry(m_viewClearHistosText, CLEAR_HISTOS_COMMAND);
    m_viewMenu->UnCheckEntry(CLEAR_HISTOS_COMMAND);
    m_viewMenu->AddSeparator();
    m_viewMenu->AddEntry(m_viewAlarmListText, SHOW_ALARM_LIST_COMMAND);
    m_viewMenu->CheckEntry(SHOW_ALARM_LIST_COMMAND);
    m_viewMenu->AddEntry(m_viewKnownProblemListText, 
                         SHOW_KNOWNPROBLEM_LIST_COMMAND);
    m_viewMenu->CheckEntry(SHOW_KNOWNPROBLEM_LIST_COMMAND);
    m_viewMenu->AddEntry(m_viewUnDockPageText, UNDOCK_PAGE_COMMAND);
    m_viewMenu->AddEntry(m_viewDockAllText, DOCK_ALL_COMMAND);
    m_viewMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                        this, "handleCommand(Command)");
    m_viewMenu->DisableEntry(HISTORY_PLOTS_COMMAND);
    m_viewMenu->DisableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);

    // Mode menu
    m_toolMenu = new TGPopupMenu(fClient->GetRoot());
    m_toolMenu->AddEntry(m_toolPageEditorOnline, 
                         PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->CheckEntry(PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->AddEntry(m_toolPageEditorOffline, 
                         PAGE_EDITOR_OFFLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_OFFLINE_MODE_COMMAND);
    m_toolMenu->AddEntry(m_toolOnline, ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(ONLINE_MODE_COMMAND);
    m_toolMenu->AddEntry(m_toolOffline, OFFLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(OFFLINE_MODE_COMMAND);
    m_toolMenu->AddSeparator();
    m_toolMenu->AddEntry(m_toolFullBenchmarkText, FULL_BENCHMARK_COMMAND);
    m_toolMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                        this, "handleCommand(Command)");

    // Help menu
    m_helpMenu = new TGPopupMenu(fClient->GetRoot());
    m_helpMenu->AddEntry(m_helpAboutText, HELP_ABOUT_COMMAND);
    m_helpMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                        this, "handleCommand(Command)");

    m_menuDock = new TGDockableFrame(this);
    m_menuDock->SetWindowName("LHCb Presenter Menu");

    m_toolBarDock = new TGDockableFrame(this);
    m_toolBarDock->SetWindowName("LHCb Presenter Toolbar");

    TGLayoutHints* menuBarLayout = new TGLayoutHints(kLHintsTop |
                                                     kLHintsExpandX);
    TGLayoutHints* menuBarItemLayout = new TGLayoutHints(kLHintsTop |
                                                         kLHintsLeft,
                                                         0, 4, 0, 0);
    TGLayoutHints* menuBarHelpLayout = new TGLayoutHints(kLHintsTop |
                                                         kLHintsRight);
    m_menuBar = new TGMenuBar(m_menuDock, 1, 1, kHorizontalFrame);
    m_menuBar->AddPopup(m_fileText, m_fileMenu, menuBarItemLayout);
    m_menuBar->AddPopup(m_editText, m_editMenu, menuBarItemLayout);
    m_menuBar->AddPopup(m_viewText, m_viewMenu, menuBarItemLayout);
    m_menuBar->AddPopup(m_toolText, m_toolMenu, menuBarItemLayout);
    m_menuBar->AddPopup(m_helpText, m_helpMenu, menuBarHelpLayout);

    m_menuDock->EnableUndock(false);
    m_menuDock->EnableHide(true);

    m_toolBarDock->EnableUndock(true);
    m_toolBarDock->EnableHide(true);

    m_menuDock->AddFrame(m_menuBar, menuBarLayout);
    m_menuDock->AddFrame(new TGHorizontal3DLine(this), menuBarLayout);

    // Toolbar
    m_toolBar = new TGToolBar(m_toolBarDock, 60, 20, kHorizontalFrame);

    const TGPicture* previousPage = picpool->GetPicture("tb_back.xpm");
    m_previousPageSaveButton = new TGPictureButton(m_toolBar, previousPage,
                                                   LOAD_PREVIOUS_PAGE_COMMAND);
    m_previousPageSaveButton->SetToolTipText("Load previous page");
    m_previousPageSaveButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_previousPageSaveButton, 0);
    m_previousPageSaveButton->Connect("Clicked()", "PresenterMainFrame", this,
                                      "loadPreviousPage()");

    const TGPicture* nextPage = picpool->GetPicture("tb_forw.xpm");
    m_nextPageButton = new TGPictureButton(m_toolBar, nextPage,
                                           LOAD_NEXT_PAGE_COMMAND);
    m_nextPageButton->SetToolTipText("Load next page");
    m_nextPageButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_nextPageButton, 0);
    m_nextPageButton->Connect("Clicked()", "PresenterMainFrame", this,
                              "loadNextPage()");


    const TGPicture* printpic = picpool->GetPicture("ed_print.png");
    m_printSaveButton = new TGPictureButton(m_toolBar, printpic,
                                            SAVE_PAGE_TO_FILE_COMMAND);
    m_printSaveButton->SetToolTipText("Print/Save page");
    m_printSaveButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_printSaveButton, 8);
    m_printSaveButton->Connect("Clicked()", "PresenterMainFrame", this,
                               "savePageToFile()");
    // Log
    const TGPicture* reportButton16pic =  picpool->GetPicture("tree_t.xpm");
    m_reportButton = new TGPictureButton(m_toolBar,
                                         reportButton16pic,
                                         REPORT_COMMAND);
    m_reportButton->SetToolTipText("Report to Log");
    m_reportButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_reportButton, 0);
    m_reportButton->Connect("Clicked()", "PresenterMainFrame",
                            this, "reportToLog()");


    // New page
    const TGPicture* newPage16pic = picpool->GetPicture("newPage16",
                                                        (char**)newPage16);
    m_newPageButton = new TGPictureButton(m_toolBar, newPage16pic,
                                          CLEAR_PAGE_COMMAND);
    m_newPageButton->SetToolTipText("New page");
    m_newPageButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_newPageButton, 8);
    m_newPageButton->Connect("Clicked()", "PresenterMainFrame", this,
                             "removeHistogramsFromPage()");

    // Save page
    const TGPicture* savePageToDb16pic = 
      picpool->GetPicture("savePageToDb16", (char**)savePageToDb16);
    m_savePageToDatabaseButton = new TGPictureButton(m_toolBar,
                                                     savePageToDb16pic,
                                                     SAVE_PAGE_TO_DB_COMMAND);
    m_savePageToDatabaseButton->SetToolTipText("Save page to Database");
    m_savePageToDatabaseButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_savePageToDatabaseButton, 0);
    m_savePageToDatabaseButton->SetState(kButtonDisabled);
    m_savePageToDatabaseButton->Connect("Clicked()", "PresenterMainFrame",
                                        this, "savePageToHistogramDB()");

    // Auto n x n layout
    const TGPicture* automaticLayout16pic = 
      picpool->GetPicture("automaticLayout16",(char**)automaticLayout16);
    m_autoCanvasLayoutButton = 
      new TGPictureButton(m_toolBar, automaticLayout16pic,
                          AUTO_LAYOUT_COMMAND);
    m_autoCanvasLayoutButton->
      SetToolTipText("Auto n x n histogram tiling layout");
    m_autoCanvasLayoutButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_autoCanvasLayoutButton, 8);
    m_autoCanvasLayoutButton->SetState(kButtonDisabled);
    m_autoCanvasLayoutButton->Connect("Clicked()", "PresenterMainFrame",
                                      this, "autoCanvasLayout()");

    // Login
    const TGPicture* connectpic = picpool->GetPicture("connect.xpm");
    m_loginButton = new TGPictureButton(m_toolBar, connectpic, LOGIN_COMMAND);
    m_loginButton->SetToolTipText("Login");
    m_loginButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_loginButton, 8);
    m_loginButton->Connect("Clicked()", "PresenterMainFrame", this,
                           "loginToHistogramDB()");
    // Logout
    const TGPicture* disconnectpic = picpool->GetPicture("disconnect.xpm");
    m_logoutButton = new TGPictureButton(m_toolBar, disconnectpic,
                                         LOGOUT_COMMAND);
    m_logoutButton->SetToolTipText("Logout");
    m_logoutButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_logoutButton, 0);
    m_logoutButton->SetState(kButtonDisabled);
    m_logoutButton->Connect("Clicked()", "PresenterMainFrame", this,
                            "logoutFromHistogramDB()");
    // Start
    const TGPicture* ed_executepic = picpool->GetPicture("ed_execute.png");
    m_startRefreshButton = new TGPictureButton(m_toolBar, ed_executepic,
                                               START_COMMAND);
    m_startRefreshButton->SetToolTipText("Start");
    m_startRefreshButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_startRefreshButton, 8);
    m_startRefreshButton->Connect("Clicked()", "PresenterMainFrame", this,
                                  "startPageRefresh()");
    // Stop
    const TGPicture* ed_interruptpic = picpool->GetPicture("ed_interrupt.png");
    m_stopRefreshButton = new TGPictureButton(m_toolBar, ed_interruptpic,
                                              START_COMMAND);
    m_stopRefreshButton->SetToolTipText("Stop");
    m_stopRefreshButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_stopRefreshButton, 0);
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_stopRefreshButton->Connect("Clicked()", "PresenterMainFrame", this,
                                 "stopPageRefresh()");

    // Clear Histos
    const TGPicture* clear16pic = 
      picpool->GetPicture("clear16", (char**)clear16);
    m_clearHistoButton = new TGPictureButton(m_toolBar, clear16pic,
                                             CLEAR_HISTOS_COMMAND);
    m_clearHistoButton->SetToolTipText("Clear histograms");
    m_clearHistoButton->AllowStayDown(true);
    m_toolBar->AddButton(this, m_clearHistoButton, 0);
    m_clearHistoButton->Connect("Clicked()", "PresenterMainFrame", this,
                                "clearHistos()");

    //Partition selector
    m_partitionSelectorComboBox = 
      new TGComboBox(m_toolBar, -1,kHorizontalFrame | 
                     kSunkenFrame | kDoubleBorder | kOwnBackground);

    m_toolBar->AddFrame(new TGLabel(m_toolBar,"Partition: ") ,
                        new TGLayoutHints(kLHintsCenterY, 2,2,2,2));
    m_toolBar->AddFrame(m_partitionSelectorComboBox , 
                        new TGLayoutHints(kLHintsCenterY,2,2,2,2));
    m_partitionSelectorComboBox -> AddEntry("refresh list", 1);
    m_partitionSelectorComboBox -> Select(1, kTRUE ) ;
    m_partitionSelectorComboBox -> Resize(122,22);
    m_partitionSelectorComboBox -> 
      Connect("Selected(int)", "PresenterMainFrame",
              this, "partitionSelectorComboBoxHandler(int)");

    //History mode
    m_toolBarLabel = new TGLabel( m_toolBar , "Time: " ) ;

    m_toolBar->AddFrame( m_toolBarLabel ,
			 new TGLayoutHints(kLHintsCenterY, 2,2,2,2));

    const TGPicture* arrow_left = picpool->GetPicture("arrow_left.xpm" , 
						      (UInt_t) 16 , (UInt_t) 16 ) ;
    m_previousIntervalButton = new TGPictureButton(m_toolBar, arrow_left,
                                                   M_Previous_Interval);
    m_previousIntervalButton->SetToolTipText("Previous interval");
    m_previousIntervalButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_previousIntervalButton, 8);
    m_previousIntervalButton->Connect("Clicked()", "PresenterMainFrame", this,
                                      "previousInterval()");

    m_textNavigation = new TGTextView( m_toolBar , 100 , 25 ) ;
    m_toolBar -> AddFrame( m_textNavigation ) ;

    const TGPicture* arrow_right = picpool->GetPicture("arrow_right.xpm" , 
						       (UInt_t) 16 , (UInt_t) 16 );
    m_nextIntervalButton = new TGPictureButton(m_toolBar, arrow_right,
                                               M_Next_Interval);
    m_nextIntervalButton->SetToolTipText("Next interval");
    m_nextIntervalButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_nextIntervalButton, 0);
    m_nextIntervalButton->Connect("Clicked()", "PresenterMainFrame", this,
                                  "nextInterval()");

    // History plots
    m_historyPlotsButton= new TGPictureButton(m_toolBar, m_stockNewFormula,
                                              HISTORY_PLOTS_COMMAND);
    m_historyPlotsButton->
      SetToolTipText("Toggle history plot merge or trend mode");
    m_historyPlotsButton->AllowStayDown(true);
    m_toolBar->AddButton(this, m_historyPlotsButton, 8);
    m_historyPlotsButton->Connect("Clicked()", "PresenterMainFrame",
                                  this, "toggleHistoryPlots()");
    m_historyPlotsButton->SetState(kButtonDisabled);

    m_historyIntervalComboBox = new TGComboBox(m_toolBar, -1,
					       kHorizontalFrame | kSunkenFrame | kDoubleBorder | 
					       kOwnBackground);
    m_toolBar->AddFrame(new TGLabel(m_toolBar,"History using: ") ,
                        new TGLayoutHints(kLHintsCenterY, 2,2,2,2));
    m_toolBar->AddFrame(m_historyIntervalComboBox , 
                        new TGLayoutHints(kLHintsCenterY,2,2,2,2));
    m_historyIntervalComboBox->SetEnabled(false);
    // Select history by run
    m_historyIntervalComboBox -> AddEntry ("set run number" , 
					   M_IntervalPickerRun ) ;
    m_historyIntervalComboBox->AddEntry("preset file", M_Last_File);
    m_historyIntervalComboBox->AddEntry("set file", M_File_Picker);
    m_historyIntervalComboBox->AddEntry("preset interval", M_Last_Interval);
    m_historyIntervalComboBox->AddEntry("last 1 hour", M_LAST_1_HOURS);
    m_historyIntervalComboBox->Select(M_LAST_1_HOURS, false);
    m_historyIntervalComboBox->AddEntry("last 8 hours", M_LAST_8_HOURS);
    m_historyIntervalComboBox->AddEntry("set interval", M_IntervalPicker);
    m_historyIntervalComboBox->Resize(112,22);
    m_historyIntervalComboBox->Connect("Selected(Int_t)", "PresenterMainFrame",
                                       this, "handleCommand(Command)");

    // Reference
    const TGPicture* f2_tpic = picpool->GetPicture("f2_t.xpm");
    m_overlayReferenceHistoButton = 
      new TGPictureButton(m_toolBar, f2_tpic, OVERLAY_REFERENCE_HISTO_COMMAND);
    m_overlayReferenceHistoButton->SetToolTipText("Overlay with reference");
    m_overlayReferenceHistoButton->AllowStayDown(true);
    m_toolBar->AddButton(this, m_overlayReferenceHistoButton, 8);
    m_overlayReferenceHistoButton->Connect("Clicked()", "PresenterMainFrame",
                                           this, "toggleReferenceOverlay()");
    m_overlayReferenceHistoButton->SetState(kButtonDisabled);

    // Set reference
    const TGPicture* f1_tpic = picpool->GetPicture("f1_t.xpm");
    m_pickReferenceHistoButton = 
      new TGPictureButton( m_toolBar , f1_tpic , 
			   PICK_REFERENCE_HISTO_COMMAND ) ;
    m_pickReferenceHistoButton->SetToolTipText("Pick reference histogram");
    m_pickReferenceHistoButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_pickReferenceHistoButton, 0);
    m_pickReferenceHistoButton->SetState(kButtonDisabled);
    m_pickReferenceHistoButton->Connect("Clicked()", "PresenterMainFrame",
                                        this, "pickReferenceHistogram()");

    // Clone Histo
    const TGPicture* inspectHistogram16pic = 
      picpool->GetPicture("inspectHistogram16", (char**)inspectHistogram16);
    m_inspectHistoButton = new TGPictureButton(m_toolBar, inspectHistogram16pic,
                                               INSPECT_HISTO_COMMAND);
    m_inspectHistoButton->SetToolTipText("Inspect histogram");
    m_inspectHistoButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_inspectHistoButton, 8);
    m_inspectHistoButton->Connect("Clicked()", "PresenterMainFrame",
                                  this, "inspectHistogram()");

    // Histo Description
    const TGPicture* histogramDescription16pic = 
      picpool->GetPicture("about.xpm");
    m_histogramDescriptionButton = 
      new TGPictureButton(m_toolBar, histogramDescription16pic,
                          HISTOGRAM_DESCRIPTION_COMMAND);
    m_histogramDescriptionButton->SetToolTipText("Histogram description");
    m_histogramDescriptionButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_histogramDescriptionButton, 0);
    m_histogramDescriptionButton->Connect("Clicked()", "PresenterMainFrame",
                                          this, "histogramDescription()");


    // Edit Histogram properties
    const TGPicture* editProperties16pic = 
      picpool->GetPicture("editProperties16", (char**)editProperties16);
    m_editHistoButton = new TGPictureButton(m_toolBar, editProperties16pic,
                                            EDIT_HISTO_COMMAND);
    m_editHistoButton->SetToolTipText("Edit histogram properties");
    m_editHistoButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_editHistoButton, 0);
    m_editHistoButton->Connect("Clicked()", "PresenterMainFrame",
                               this, "editHistogramProperties()");

    // Delete histo from Canvas
    const TGPicture* editDelete16pic = 
      picpool->GetPicture("editDelete16", (char**)editDelete16);
    m_deleteHistoFromCanvasButton = 
      new TGPictureButton(m_toolBar, editDelete16pic,
                          REMOVE_HISTO_FROM_CANVAS_COMMAND);
    m_deleteHistoFromCanvasButton->
      SetToolTipText("Remove selected histogram from page");
    m_deleteHistoFromCanvasButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_deleteHistoFromCanvasButton, 8);
    m_deleteHistoFromCanvasButton->
      Connect("Clicked()", "PresenterMainFrame",
              this, "deleteSelectedHistoFromCanvas()");

    m_toolBarDock->AddFrame(m_toolBar,
                            new TGLayoutHints(kLHintsTop |
                                              kLHintsExpandX,
                                              0, 0, 0, 0));

    m_mainHorizontalFrame = new TGHorizontalFrame(this, 544, 498,
                                                  kHorizontalFrame |
                                                  kLHintsExpandX |
                                                  kLHintsExpandY);

    m_leftMiscFrame = new TGVerticalFrame(m_mainHorizontalFrame, 200, 494,
                                          kVerticalFrame | kFixedWidth |
                                          kLHintsExpandX);

    TGVSplitter* leftVerticalSplitter = 
      new TGVSplitter(m_mainHorizontalFrame, 4, 4);
    leftVerticalSplitter->SetFrame(m_leftMiscFrame, true);

    m_databasePagesDock = new TGDockableFrame(m_leftMiscFrame);
    m_databasePagesDock->SetWindowName("Database Page Browser");
    m_databasePagesDock->EnableUndock(true);
    m_databasePagesDock->EnableHide(true);
    m_databasePagesDock->Resize(220, 494);
    m_leftMiscFrame->AddFrame(m_databasePagesDock,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop |
                                                kLHintsExpandX |
                                                kLHintsExpandY,
                                                0, 0, 0, 0));

    TGGroupFrame* databasePagesGroupFrame = 
      new TGGroupFrame(m_databasePagesDock, TGString("Database Page Browser"),
                       kVerticalFrame | kFixedHeight);
    m_databasePagesDock->AddFrame(databasePagesGroupFrame,
                                  new TGLayoutHints(kLHintsTop |
                                                    kLHintsExpandX |
                                                    kLHintsExpandY,
                                                    0, 0, 0, 0));

    TGCanvas* fCanvas652 = new TGCanvas(databasePagesGroupFrame, 220, 243);
    m_pagesFromHistoDBViewPort = fCanvas652->GetViewPort();

    // list tree
    m_pagesFromHistoDBListTree = new TGListTree(fCanvas652, kHorizontalFrame);
    m_pagesFromHistoDBListTree->AddRoot("Pages");
    m_pagesFromHistoDBListTree->SetCheckMode(TGListTree::kRecursive);
    (m_pagesFromHistoDBListTree->GetFirstItem())->
      SetPictures(m_databaseSourceIcon, m_databaseSourceIcon);
    (m_pagesFromHistoDBListTree->GetFirstItem())->SetOpen(1);
    m_pagesFromHistoDBListTree->
      Connect("Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)",
              "PresenterMainFrame", this,
              "clickedPageTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");

    m_pagesFromHistoDBListTree->
      Connect("DataDropped(TGListTreeItem*, TDNDData*)",
              "PresenterMainFrame", this,
              "dataDropped(TGListTreeItem*, TDNDData*)");

    m_pagesFromHistoDBViewPort->AddFrame(m_pagesFromHistoDBListTree);
    m_pagesFromHistoDBListTree->
      SetLayoutManager( new TGHorizontalLayout(m_pagesFromHistoDBListTree));
    m_pagesFromHistoDBListTree->MapSubwindows();
    fCanvas652->SetContainer(m_pagesFromHistoDBListTree);
    fCanvas652->MapSubwindows();
    databasePagesGroupFrame->AddFrame(fCanvas652,
                                      new TGLayoutHints(kLHintsRight |
                                                        kLHintsBottom |
                                                        kLHintsExpandX |
                                                        kLHintsExpandY,
                                                        0, 0, 0, 0));

    m_pagesContextMenu = new TGPopupMenu(fClient->GetRoot());
    m_pagesContextMenu->AddEntry("Load Page", M_LoadPage_COMMAND);
    m_pagesContextMenu->AddSeparator();
    m_pagesContextMenu->AddEntry("Move/Rename", M_Move_COMMAND);
    m_pagesContextMenu->AddSeparator();
    m_pagesContextMenu->AddEntry("Delete", M_DeletePage_COMMAND);
    m_pagesContextMenu->AddEntry("Create Folder", M_CreateFolder_COMMAND);
    m_pagesContextMenu->AddSeparator();
    m_pagesContextMenu->AddEntry("Refresh", M_RefreshDBPagesListTree_COMMAND);
    m_pagesContextMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                                this, "handleCommand(Command)");

    // -------------------------

    m_databaseAlarmsDock = new TGDockableFrame(m_leftMiscFrame);
    m_databaseAlarmsDock->SetWindowName("Histogram Alarms");
    m_databaseAlarmsDock->EnableUndock(true);
    m_databaseAlarmsDock->EnableHide(true);
    m_databaseAlarmsDock->Resize(220, 494);
    m_leftMiscFrame->AddFrame(m_databaseAlarmsDock,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop |
                                                kLHintsExpandX |
                                                kLHintsExpandY,
                                                0, 0, 0, 0));

    m_leftDatabaseAlarmFrame = new TGVerticalFrame(m_databaseAlarmsDock,
                                                   220, 100,
                                                   kVerticalFrame |
                                                   kFixedWidth);

    TGGroupFrame* databaseAlarmGroupFrame = 
      new TGGroupFrame(m_leftDatabaseAlarmFrame,
                       TGString("Alarms from Automatic Analysis"), 
                       kVerticalFrame | kFixedWidth);

    m_leftDatabaseAlarmFrame->AddFrame(databaseAlarmGroupFrame,
                                       new TGLayoutHints(kLHintsTop |
                                                         kLHintsExpandX |
                                                         kLHintsExpandY,
                                                         0, 0, 0, 0));

    m_databaseAlarmsDock->AddFrame(m_leftDatabaseAlarmFrame,
                                   new TGLayoutHints(kLHintsTop |
                                                     kLHintsExpandX |
                                                     kLHintsExpandY,
                                                     0, 0, 0, 0));

    // canvas widget
    m_alarmDBCanvas = new TGCanvas(databaseAlarmGroupFrame, 124, 460);

    // canvas viewport
    m_alarmDBCanvasViewPort = m_alarmDBCanvas->GetViewPort();

    // list tree
    m_alarmHistogramTreeList = new TGListTree(m_alarmDBCanvas,
                                              kHorizontalFrame);
    m_alarmHistogramTreeList->AddRoot("Alarms");
    (m_alarmHistogramTreeList->GetFirstItem())->SetPictures(m_iconAnalysisAlarm, 
							    m_iconAnalysisAlarm);
    m_alarmHistogramTreeList->SetCheckMode(TGListTree::kRecursive);
    m_alarmHistogramTreeList->
      Connect( "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", 
               "PresenterMainFrame", this , 
               "clickedAlarmTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");

    m_alarmDBContextMenu = new TGPopupMenu(fClient->GetRoot());
    m_alarmDBContextMenu->AddEntry("Refresh", M_RefreshAlarmDBListTree_COMMAND);
    m_alarmDBContextMenu->Connect("Activated(Int_t)","PresenterMainFrame",
                                  this, "handleCommand(Command)");

    m_alarmDBCanvasViewPort->AddFrame(m_alarmHistogramTreeList);
    m_alarmHistogramTreeList->
      SetLayoutManager( new TGHorizontalLayout(m_alarmHistogramTreeList));
    m_alarmHistogramTreeList->MapSubwindows();
    m_alarmDBCanvas->SetContainer(m_alarmHistogramTreeList);
    m_alarmDBCanvas->MapSubwindows();
    databaseAlarmGroupFrame->AddFrame(m_alarmDBCanvas,
                                      new TGLayoutHints(kLHintsLeft |
                                                        kLHintsTop |
                                                        kLHintsExpandX |
                                                        kLHintsExpandY,
                                                        0, 0, 0, 0));
    if(m_alarmDisplayEnabled) {
      m_alarmDisplay = new AlarmDisplay( this , m_alarmHistogramTreeList ) ;
    }
    //=========================================================================
    // List of known problems
    
    m_knownProblemDock = new TGDockableFrame( m_leftMiscFrame ) ;
    m_knownProblemDock -> SetWindowName( "List of known problems" ) ;
    m_knownProblemDock -> EnableUndock( true ) ;
    m_knownProblemDock -> EnableHide( true ) ;
    m_knownProblemDock -> Resize( 220 , 494 ) ;
    m_leftMiscFrame -> AddFrame( m_knownProblemDock ,
				 new TGLayoutHints(kLHintsLeft | kLHintsTop |
						   kLHintsExpandX | kLHintsExpandY,
						   0, 0, 0, 0 ) ) ;
    m_globalKnownProblemList = new KnownProblemList( m_knownProblemDock ,  
						     m_pbdbConfig ) ;
    m_knownProblemDock -> AddFrame( m_globalKnownProblemList , 
				    new TGLayoutHints( kLHintsLeft | kLHintsBottom |
						   kLHintsExpandX | kLHintsExpandY,
						   0, 0, 0, 0 ) ) ;
    // -------------------------
    m_pageDock = new TGDockableFrame(m_mainHorizontalFrame, -1, 
                                     kVerticalFrame | kFixedWidth );
    m_pageDock->SetWindowName("LHCb Presenter Page");
    m_pageDock->EnableUndock(true);
    m_pageDock->EnableHide(false);
    m_pageDock->Resize(600, 494);
    m_pageDock->Connect("Docked()", "PresenterMainFrame", this, 
                        "enablePageUndocking()");
    m_pageDock->Connect("Undocked()", "PresenterMainFrame", this, 
                        "disablePageUndocking()");

    // centralFrame
    TGVerticalFrame* centralPageFrame = 
      new TGVerticalFrame( m_pageDock, 600 , 494 , 
                           kVerticalFrame | kFixedHeight );
    m_pageDock->AddFrame(centralPageFrame,
                         new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                           kLHintsExpandX | kLHintsExpandY,
                                           0, 0, 0, 0));
    int partsTop[] = {25, 50, 25};
    m_statusBarTop = new TGStatusBar(centralPageFrame, 50, 10, kVerticalFrame);
    m_statusBarTop->SetParts(partsTop, 3);
    m_statusBarTop->Draw3DCorner(false);
    centralPageFrame->AddFrame(m_statusBarTop, 
                               new TGLayoutHints(kLHintsTop | kLHintsExpandX ,
                                                 0, 0, 0, 0));

    // embedded canvas
    editorEmbCanvas = new TRootEmbeddedCanvas(0, centralPageFrame, 600, 494);
    int wm_editorEmbCanvas = editorEmbCanvas->GetCanvasWindowId();
    editorEmbCanvas->SetBit(kNoContextMenu);
    editorCanvas = new TCanvas("editor canvas", 600, 494, wm_editorEmbCanvas);
    editorCanvas->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)",
                          "PresenterMainFrame", this,
                          "EventInfo(Int_t, Int_t, Int_t, TObject*)");
    editorCanvas->SetBit(kNoContextMenu);

    editorEmbCanvas->AdoptCanvas(editorCanvas);
    centralPageFrame->AddFrame(editorEmbCanvas, 
                               new TGLayoutHints(kLHintsRight | kLHintsTop |
                                                 kLHintsExpandX |
                                                 kLHintsExpandY , 0, 0, 0, 0));

    // Add popup menu for right-click on histo:
    m_histomenu = new TGPopupMenu( editorEmbCanvas ) ;
    m_histomenu -> AddEntry( "NAME" , 10 ) ;
    m_histomenu -> HideEntry( 10 ) ;
    m_histomenu -> AddSeparator( ) ;
    m_histomenu -> AddEntry( "--- no documentation available ---" , 2 ) ;

    //== Page comments window 
    int commentSize = 130;
    TGGroupFrame* mainCanvasInfoGroupFrame = 
      new TGGroupFrame( centralPageFrame, TGString("Page Comments"),
                        kHorizontalFrame | kFixedWidth | kFixedHeight );
    mainCanvasInfoGroupFrame->Resize(600, commentSize+10);
    centralPageFrame->AddFrame( mainCanvasInfoGroupFrame,
                                new TGLayoutHints( kLHintsBottom |
                                                   kLHintsExpandX ,
                                                   0, 0, 0, 0) );

    //== Page comments view
    m_pageDescriptionView = 
      new PageDescriptionTextView( mainCanvasInfoGroupFrame,
                                   600, commentSize , m_pbdbConfig );
    mainCanvasInfoGroupFrame->
      AddFrame(m_pageDescriptionView,
               new TGLayoutHints( kLHintsLeft | kLHintsTop |
                                  kLHintsExpandX | kLHintsExpandY ,
                                  2,2,2,2));
 
    //== splitter, to adjust the size of the page comment
    TGHSplitter * horizontalSplitter = 
      new TGHSplitter( centralPageFrame,  4, 4 );
    horizontalSplitter->SetFrame( mainCanvasInfoGroupFrame , false ) ;
    centralPageFrame->
      AddFrame( horizontalSplitter,
                new TGLayoutHints( kLHintsBottom | kLHintsExpandX ));

    
    //====================

    m_rightMiscFrame = new TGVerticalFrame(m_mainHorizontalFrame, 220, 494,
                                           kVerticalFrame | kFixedWidth |
                                           kLHintsExpandX);

    m_rightVerticalSplitter = new TGVSplitter(m_mainHorizontalFrame, 4, 4);
    m_rightVerticalSplitter->SetFrame(m_rightMiscFrame, false);

    m_dimBrowserDock = new TGDockableFrame(m_rightMiscFrame);
    m_dimBrowserDock->SetWindowName("DIM Histogram Browser");
    m_dimBrowserDock->EnableUndock(true);
    m_dimBrowserDock->EnableHide(false);
    m_dimBrowserDock->Resize(220, 494);
    m_rightMiscFrame->AddFrame(m_dimBrowserDock,
                               new TGLayoutHints(kLHintsRight |
                                                 kLHintsTop |
                                                 kLHintsExpandX |
                                                 kLHintsExpandY,
                                                 0, 0, 0, 0));

    m_histoSvcBrowserGroupFrame = 
      new TGGroupFrame(m_dimBrowserDock,
                       TGString("DIM Histogram Browser"),
                       kVerticalFrame | kFixedHeight);
    m_dimBrowserDock->AddFrame(m_histoSvcBrowserGroupFrame,
                               new TGLayoutHints(kLHintsTop |
                                                 kLHintsExpandX |
                                                 kLHintsExpandY,
                                                 0, 0, 0, 0));

    m_dimSvcListTreeContainterCanvas = new TGCanvas(m_histoSvcBrowserGroupFrame,
                                                    220, 100);
    TGViewPort* fViewPort664 = m_dimSvcListTreeContainterCanvas->GetViewPort();
    m_histoSvcListTree = new TGListTree(m_dimSvcListTreeContainterCanvas,
                                        kHorizontalFrame);
    m_histoSvcListTree->AddRoot("DIM");
    (m_histoSvcListTree->GetFirstItem())->SetPictures(m_dimOnline16,
                                                      m_dimOnline16);
    m_histoSvcListTree->SetCheckMode(TGListTree::kRecursive);
    m_histoSvcListTree->
      Connect( "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", 
               "PresenterMainFrame", this, 
               "clickedHistoSvcTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");

    fViewPort664->AddFrame(m_histoSvcListTree);
    m_histoSvcListTree->
      SetLayoutManager(new TGHorizontalLayout( m_histoSvcListTree));
    m_histoSvcListTree->MapSubwindows();
    m_dimSvcListTreeContainterCanvas->SetContainer(m_histoSvcListTree);
    m_dimSvcListTreeContainterCanvas->MapSubwindows();
    m_histoSvcBrowserGroupFrame->
      AddFrame(m_dimSvcListTreeContainterCanvas,
               new TGLayoutHints(kLHintsRight | kLHintsTop |
                                 kLHintsExpandX | kLHintsExpandY,
                                 0, 0, 0, 0));

    m_histoSvcTreeContextMenu = new TGPopupMenu(fClient->GetRoot());
    m_histoSvcTreeContextMenu->AddEntry("Add checked to Database",
                                        M_AddHistoToDB_COMMAND);
    m_histoSvcTreeContextMenu->AddEntry("Add checked to Page",
                                        M_AddHistoToPage_COMMAND);
    m_histoSvcTreeContextMenu->AddSeparator();
    m_histoSvcTreeContextMenu->AddEntry("Collsapse all children",
                                        M_DimCollapseAllChildren_COMMAND);
    m_histoSvcTreeContextMenu->AddSeparator();
    m_histoSvcTreeContextMenu->AddEntry("Refresh", M_RefreshHistoSvcListTree);
    m_histoSvcTreeContextMenu->Connect("Activated(Int_t)","PresenterMainFrame", 
                                       this, "handleCommand(Command)");

    m_databaseHistogramsDock = new TGDockableFrame(m_rightMiscFrame);
    m_databaseHistogramsDock->SetWindowName("Histograms in Database");
    m_databaseHistogramsDock->EnableUndock(true);
    m_databaseHistogramsDock->EnableHide(false);
    m_databaseHistogramsDock->Resize(220, 100);
    m_rightMiscFrame->AddFrame(m_databaseHistogramsDock,
                               new TGLayoutHints(kLHintsRight |
                                                 kLHintsBottom |
                                                 kLHintsExpandX |
                                                 kLHintsExpandY |
                                                 kFixedWidth,
                                                 0, 0, 0, 0));
    // databaseHistogramFrame
    TGVerticalFrame* leftDatabaseHistogramFrame = 
      new TGVerticalFrame(m_databaseHistogramsDock, 220, 100,
                          kVerticalFrame | kFixedWidth);
    TGGroupFrame* databaseHistogramGroupFrame = 
      new TGGroupFrame(leftDatabaseHistogramFrame,
                       TGString("Histograms in Database"),
                       kVerticalFrame | kFixedWidth);

    leftDatabaseHistogramFrame->AddFrame(databaseHistogramGroupFrame,
                                         new TGLayoutHints(kLHintsTop |
                                                           kLHintsExpandX |
                                                           kLHintsExpandY,
                                                           0, 0, 0, 0));

    m_databaseHistogramsDock->AddFrame(leftDatabaseHistogramFrame,
                                       new TGLayoutHints(kLHintsTop |
                                                         kLHintsExpandX |
                                                         kLHintsExpandY,
                                                         0, 0, 0, 0));
    // Database histogram filter box
    m_histoDBFilterComboBox = new TGComboBox(databaseHistogramGroupFrame, -1,
                                             kHorizontalFrame | kDoubleBorder |
                                             kSunkenFrame | kOwnBackground);
    m_histoDBFilterComboBox->AddEntry("Folder/Page", FoldersAndPages); // #0
    m_histoDBFilterComboBox->AddEntry("Tasks/Algorithm", Tasks); // #1
    m_histoDBFilterComboBox->AddEntry("Subsystems", Subsystems); // #2
    m_histoDBFilterComboBox->AddEntry("With auto analysis",
                                      HistogramsWithAnalysis); //#3
    m_histoDBFilterComboBox->AddEntry("Produced by analysis",
                                      AnalysisHistograms); //#4
    m_histoDBFilterComboBox->AddEntry("Full list", AllHistograms); // #5
    m_histoDBFilterComboBox->Resize(149, 22);
    m_histoDBFilterComboBox->Select(Tasks);
    m_histoDBFilterComboBox->Connect("Selected(Int_t)", "PresenterMainFrame",
                                     this, "refreshHistoDBListTree()");
    databaseHistogramGroupFrame->AddFrame(m_histoDBFilterComboBox,
                                          new TGLayoutHints(kLHintsLeft |
                                                            kLHintsTop |
                                                            kLHintsExpandX,
                                                            0, 0, 0, 0));
    // canvas widget
    m_histoDBCanvas = new TGCanvas(databaseHistogramGroupFrame, 124, 460);

    // canvas viewport
    m_histoDBCanvasViewPort = m_histoDBCanvas->GetViewPort();

    // list tree
    m_databaseHistogramTreeList = new TGListTree(m_histoDBCanvas,
                                                 kHorizontalFrame);
    m_databaseHistogramTreeList->AddRoot("Histograms");
    m_databaseHistogramTreeList->SetCheckMode(TGListTree::kRecursive);
    m_databaseHistogramTreeList->
      Connect( "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", 
               "PresenterMainFrame", this, 
               "clickedHistoDBTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");

    m_histoDBContextMenu = new TGPopupMenu(fClient->GetRoot());
    m_histoDBContextMenu->AddEntry("Add checked histogram(s) to Page", 
                                   M_AddDBHistoToPage_COMMAND);
    m_histoDBContextMenu->AddEntry("Add checked to Page in overlap", 
				   M_AddDBHistoToPageAsOne_COMMAND);
    m_histoDBContextMenu->AddEntry("Set properties of checked histogram(s)", 
				   M_SetHistoPropertiesInDB_COMMAND);
    m_histoDBContextMenu->AddEntry("Delete checked histogram(s)", 
                                   M_DeleteDBHisto_COMMAND);
    m_histoDBContextMenu->AddSeparator();
    m_histoDBContextMenu->AddEntry("Collsapse all children", 
                                   M_DBHistoCollapseAllChildren_COMMAND);
    m_histoDBContextMenu->AddSeparator();
    m_histoDBContextMenu->AddEntry("Refresh", M_RefreshHistoDBListTree_COMMAND);
    m_histoDBContextMenu->Connect( "Activated(Int_t)","PresenterMainFrame",
                                   this, "handleCommand(Command)");

    m_histoDBCanvasViewPort->AddFrame(m_databaseHistogramTreeList);
    m_databaseHistogramTreeList->
      SetLayoutManager( new TGHorizontalLayout(m_databaseHistogramTreeList));
    m_databaseHistogramTreeList->MapSubwindows();
    m_histoDBCanvas->SetContainer(m_databaseHistogramTreeList);
    m_histoDBCanvas->MapSubwindows();
    databaseHistogramGroupFrame->
      AddFrame(m_histoDBCanvas,
               new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                 kLHintsExpandX | kLHintsExpandY,
                                 0, 0, 0, 0));


    m_mainHorizontalFrame->AddFrame(m_leftMiscFrame,
                                    new TGLayoutHints(kLHintsLeft |
                                                      kLHintsExpandY,
                                                      0, 0, 0, 0));

    m_mainHorizontalFrame->AddFrame(leftVerticalSplitter,
                                    new TGLayoutHints(kLHintsLeft |
                                                      kLHintsExpandY));
    m_mainHorizontalFrame->AddFrame(m_pageDock,
                                    new TGLayoutHints(kLHintsLeft |
                                                      kLHintsExpandX |
                                                      kLHintsExpandY,
                                                      0, 0, 0, 0));

    m_mainHorizontalFrame->AddFrame(m_rightMiscFrame,
                                    new TGLayoutHints(kLHintsRight |
                                                      kLHintsExpandY,
                                                      0, 0, 0, 0));

    m_mainHorizontalFrame->AddFrame(m_rightVerticalSplitter,
                                    new TGLayoutHints(kLHintsRight |
                                                      kLHintsExpandY));

    int parts[] = {35, 25, 40};
    m_mainStatusBar = new TGStatusBar(this, 50, 10, kVerticalFrame);
    m_mainStatusBar->SetParts(parts, 3);
    m_mainStatusBar->SetText("Launching Presenter...", 2);

    AddFrame(m_menuDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
    AddFrame(m_toolBarDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
    AddFrame(m_mainHorizontalFrame, new TGLayoutHints(kLHintsExpandX |
                                                      kLHintsExpandY));
    AddFrame(m_mainStatusBar, new TGLayoutHints(kLHintsBottom | kLHintsExpandX,
                                                0, 0, 0, 0));
    m_menuDock->MapWindow();
    m_toolBarDock->MapWindow();
    m_databaseHistogramsDock->MapWindow();
    m_pageDock->MapWindow();
    m_dimBrowserDock->MapWindow();
    m_databasePagesDock->MapWindow();

    m_runDb = new RunDB( m_rundbConfig ) ;
    m_intervalPickerData = new IntervalPickerData( ) ;
    partitionSelectorComboBoxHandler(m_partitionSelectorComboBox->
                                     GetSelected());
    m_prevPresenterMode = presenterMode() ;
    m_presenterInfo.setPresenterMode( pres::Online ) ;

    SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputPrimaryApplicationModal);
    MapSubwindows();
    MapWindow();
    m_rightMiscFrame->UnmapWindow();
    m_rightVerticalSplitter->UnmapWindow();
    this->Resize(m_initWidth, m_initHeight);
    DoRedraw();
  }
}

//==============================================================================
// Call back when data is dropped
//==============================================================================
void PresenterMainFrame::dataDropped(TGListTreeItem* folder, TDNDData* data) {
  if ( ( canWriteToHistogramDB() ) && ( 0 != folder ) && 
       ( folder->IsDNDTarget() ) && ( 0 != data ) && ( data->fData ) && 
       ( folder->GetUserData() ) ) {
    TString fromName = static_cast<char*>(data->fData);
    if (fromName.BeginsWith( pres::s_FILE_URI ) )
      fromName.Remove( 0 , pres::s_FILE_URI.length() ) ; 
    std::string pageName(fromName);
    
    TString newPageName = 
      (*static_cast<TObjString*>(folder->GetUserData())).GetString();
    if (newPageName.BeginsWith( pres::s_FILE_URI ) ) 
      newPageName.Remove( 0 , pres::s_FILE_URI.length() ) ; 
    
    // Tree does not handle deep recursion correctly ...target, 
    // source and 42 broken.
    TGListTreeItem* page = 0;
    char path[ pres::s_maxPageNameLength ] ;
    strncpy( path, "", sizeof(path)-1);
    m_pagesFromHistoDBListTree->
      GetPathnameFromItem( m_pagesFromHistoDBListTree -> GetSelected() , 
			   path ) ;
    
    if ( 0 != strlen(path) ) {
      std::string rootPath(path);
      page = m_pagesFromHistoDBListTree->FindItemByPathname(rootPath.c_str());
      if ( ( 0 != page ) && ( kFALSE == page->IsDNDTarget() ) &&
	   (kFALSE == folder->IsDNDSource())) {
        fromName.Remove(0, fromName.Last('/')+1);
        if (0 == strcmp(fromName, page->GetText())) {
          newPageName.Append( pres::s_slash ).Append(fromName);
          try {
            if (m_verbosity >= pres::Verbose)
              std::cout << "mv \"" << pageName << "\" \"" << newPageName 
                        << "\""<< std::endl;
	    
            OnlineHistPage* dbPage = m_histogramDB->getPage(pageName);
            dbPage->rename(std::string(newPageName.Data()));
            if (m_histogramDB->commit()) {
              std::string userPageData( pres::s_FILE_URI ) ;
              userPageData.append(newPageName);
              TObjString *objectString = new TObjString(userPageData.c_str());
              page->SetUserData(objectString);
              m_pagesFromHistoDBListTree->Reparent(page, folder);
              m_pagesFromHistoDBListTree->Sort(page);
            }
          } catch (std::string sqlException) {
            setStatusBarText(sqlException.c_str(), 2);
            if (m_verbosity >= pres::Verbose) 
              std::cout << sqlException << std::endl; 
            if ( ! isBatch() ) 
              new 
                TGMsgBox(fClient->GetRoot(), this, "Database Error",
                         Form("Could not move the page in OnlineHistDB:\n%s\n",
                              sqlException.c_str()),
                         kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
          }
        } 
      }
    }
  }
}

//==============================================================================
// Close presenter window
//==============================================================================
void PresenterMainFrame::CloseWindow() {
  if (0 != m_pageRefreshTimer) { m_pageRefreshTimer->Stop(); }
  if (0 != m_clockTimer) { m_clockTimer->Stop(); }

  removeHistogramsFromPage();
  if (0 != m_analysisLib) { delete m_analysisLib; m_analysisLib = NULL; }
  
  // delete RunDB interface object
  if ( m_runDb ) delete m_runDb ;

  gApplication->Terminate( 0 );
}

//==============================================================================
// dock all frames
//==============================================================================
void PresenterMainFrame::dockAllFrames() {
  m_pageDock->DockContainer();
  m_toolBarDock->DockContainer();
  m_menuDock->DockContainer();
  m_databaseHistogramsDock->DockContainer();
  m_dimBrowserDock->DockContainer();
  m_databasePagesDock->DockContainer();
  m_databaseAlarmsDock->DockContainer();
  m_knownProblemDock -> DockContainer () ;
}

//==============================================================================
// Handle the command
//==============================================================================
void PresenterMainFrame::handleCommand(Command cmd) {
  if (X_ENIGMA_COMMAND == cmd) {
    TGButton* btn = static_cast<TGButton*>(gTQSender);
    cmd = static_cast<Command>(btn->WidgetId());
  }

  switch (cmd) {
  case EXIT_COMMAND:
    CloseWindow();
    break;
  case START_COMMAND:
    startPageRefresh();
    break;
  case STOP_COMMAND:
    stopPageRefresh();
    break;
  case CLEAR_HISTOS_COMMAND:
    clearHistos();
    break;
  case SHOW_ALARM_LIST_COMMAND:
    toggleShowAlarmList();
    break;
  case SHOW_KNOWNPROBLEM_LIST_COMMAND:
    toggleShowKnownProblemList() ;
    break ;
  case CLEAR_PAGE_COMMAND:
    removeHistogramsFromPage();
    break;
  case SAVE_PAGE_TO_DB_COMMAND:
    savePageToHistogramDB();
    break;
  case SAVE_PAGE_TO_FILE_COMMAND:
    savePageToFile();
    break;
  case REPORT_COMMAND:
    reportToLog();
    break;
  case M_Previous_Interval:
    previousInterval();
    break;
  case M_Next_Interval:
    nextInterval();
    break;
  case LOGOUT_COMMAND:
    logoutFromHistogramDB();
    break;
  case LOGIN_COMMAND:
    loginToHistogramDB();
    break;
  case INSPECT_HISTO_COMMAND:
    inspectHistogram();
    break;
  case HISTOGRAM_DESCRIPTION_COMMAND:
    histogramDescription();
    break;
  case INSPECT_PAGE_COMMAND:
    inspectPage();
    break;
  case AUTO_LAYOUT_COMMAND:
    autoCanvasLayout();
    break;
  case EDIT_HISTO_COMMAND:
    editHistogramProperties();
    break;
  case REMOVE_HISTO_FROM_CANVAS_COMMAND:
    deleteSelectedHistoFromCanvas();
    break;
  case EDIT_ADD_TRENDINGHISTO_COMMAND:
    addTrendingHisto() ;
    break ;
  case UNDOCK_PAGE_COMMAND:
    m_pageDock->UndockContainer();
    break;
  case OFFLINE_MODE_COMMAND:
    setPresenterMode( pres::History ) ;
    break;
  case FULL_BENCHMARK_COMMAND:
    loadAllPages();
    break;
  case ONLINE_MODE_COMMAND:
    setPresenterMode( pres::Online ) ;
    break;
  case PAGE_EDITOR_ONLINE_MODE_COMMAND:
    setPresenterMode(pres::EditorOnline);
    break;
  case PAGE_EDITOR_OFFLINE_MODE_COMMAND:
    setPresenterMode( pres::EditorOffline ) ;
    break;
  case DOCK_ALL_COMMAND:
    dockAllFrames();
    break;
  case M_AddHistoToDB_COMMAND:
    addHistoToHistoDB();
    break;
  case M_AddHistoToPage_COMMAND:
    addDimHistosToPage();
    break;
  case M_DimCollapseAllChildren_COMMAND:
    dimCollapseAllChildren();
    break;
  case M_RefreshHistoSvcListTree:
    refreshHistogramSvcList(pres::s_withTree);
    partitionSelectorComboBoxHandler( 1 );
    break;
  case M_PartitionList:
    partitionSelectorComboBoxHandler(m_partitionSelectorComboBox->
                                     GetSelected());
    break;
  case M_AddDBHistoToPage_COMMAND:
    addDbHistoToPage( pres::separate ) ;
    break;
  case M_AddDBHistoToPageAsOne_COMMAND:
    addDbHistoToPage( pres::overlap ) ;
    break;
  case M_DBHistoCollapseAllChildren_COMMAND:
    dbHistoCollapseAllChildren();
    break;
  case M_DeleteDBHisto_COMMAND:
    deleteSelectedHistoFromDB();
    break;
  case M_SetHistoPropertiesInDB_COMMAND:
    setHistogramPropertiesInDB();
    break;
  case M_RefreshHistoDBListTree_COMMAND:
    refreshHistoDBListTree();
    m_histogramDB->refresh();
    break;
  case M_RefreshAlarmDBListTree_COMMAND:
    if(m_alarmDisplay) { m_alarmDisplay->listAlarmsFromHistogramDB();}
    break;
  case M_RefreshDBPagesListTree_COMMAND:
    refreshPagesDBListTree();
    m_histogramDB->refresh();
    break;
  case M_IntervalPicker:
    m_intervalPickerData -> setMode( IntervalPickerData::TimeInterval ) ; 
    fClient -> 
      WaitFor(dynamic_cast< TGWindow * >( new IntervalPicker( &m_presenterInfo ,
							      this ,
							      m_archive , 
							      m_verbosity ,
							      m_runDb , 
							      m_intervalPickerData ) ) );
    
    m_previousIntervalButton->SetState(kButtonEngaged);
    m_previousIntervalButton->SetState(kButtonUp);
    m_nextIntervalButton->SetState(kButtonEngaged);
    m_nextIntervalButton->SetState(kButtonUp);
    switchToRunNavigation( false ) ;
    if ( ! m_currentPageName.empty( ) ) 
      loadSelectedPageFromDB( m_currentPageName , 
                              m_presenterInfo.globalTimePoint() , 
                              m_presenterInfo.globalPastDuration() ) ;
    break;
  case M_IntervalPickerRun:
    m_intervalPickerData -> setMode( IntervalPickerData::SingleRun ) ;
    fClient->
      WaitFor(dynamic_cast<TGWindow*>( new IntervalPicker( &m_presenterInfo ,
							   this , 
							   m_archive , 
							   m_verbosity , 
							   m_runDb , 
							   m_intervalPickerData ) ) ) ;
    m_previousIntervalButton->SetState(kButtonEngaged);
    m_previousIntervalButton->SetState(kButtonUp);
    m_nextIntervalButton->SetState(kButtonEngaged);
    m_nextIntervalButton->SetState(kButtonUp);
    switchToRunNavigation( true ) ;
    if ( ! m_currentPageName.empty() ) 
      loadSelectedPageFromDB( m_currentPageName , 
                              m_presenterInfo.globalTimePoint() , 
                              m_presenterInfo.globalPastDuration() ) ;
    break ;
  case M_Last_Interval:
    m_previousIntervalButton->SetState(kButtonEngaged);
    m_previousIntervalButton->SetState(kButtonUp);
    m_nextIntervalButton->SetState(kButtonEngaged);
    m_nextIntervalButton->SetState(kButtonUp);
    //      m_currentPageName = selectedPageFromDbTree();
    if (!m_currentPageName.empty() && ( ! m_loadingPage)) {
      loadSelectedPageFromDB(m_currentPageName, 
                             m_presenterInfo.globalTimePoint(), 
                             m_presenterInfo.globalPastDuration() );
    }
    break;
  case LOAD_NEXT_PAGE_COMMAND:
    loadNextPage();
    break;
  case LOAD_PREVIOUS_PAGE_COMMAND:
    loadPreviousPage();
    break;
  case M_LoadPage_COMMAND:
  case M_LAST_1_HOURS:
  case M_LAST_8_HOURS:
  case M_Last_File: 
    m_previousIntervalButton->SetState(kButtonDisabled);
    m_nextIntervalButton->SetState(kButtonDisabled);
    m_currentPageName = selectedPageFromDbTree();
    if (!m_currentPageName.empty() && ( ! m_loadingPage ) ) 
      loadSelectedPageFromDB( m_currentPageName , pres::s_startupFile , 
                              m_savesetFileName ) ;
    break;
  case M_File_Picker: {
    TGFileInfo fileInfo;
    const char* fileTypes[] = {"ROOT files", "*.root",
                               "All files", "*",
                               0 , 0};
    fileInfo.fFileTypes = fileTypes;
    fileInfo.fIniDir = StrDup(m_savesetPath.c_str());
    fileInfo.SetMultipleSelection(false);

    new TGFileDialog(fClient->GetRoot(),this,kFDOpen,&fileInfo);
    if (0 != fileInfo.fFilename) {
      m_savesetFileName = std::string(fileInfo.fFilename);
      std::size_t pos = m_savesetFileName.find( "castor:/" ) ;
      if ( std::string::npos != pos ) 
        // file is in Castor, change path to reflect this
        m_savesetFileName.erase( 0 , pos ) ;

      if(false == m_savesetFileName.empty()) {
        if (pres::EditorOffline == presenterMode() )
          refreshHistogramSvcList(pres::s_withTree);
        
        m_previousIntervalButton->SetState(kButtonDisabled);
        m_nextIntervalButton->SetState(kButtonDisabled);
        m_currentPageName = selectedPageFromDbTree();
        
        if (!m_currentPageName.empty() && (false == m_loadingPage)) 
          loadSelectedPageFromDB(m_currentPageName, pres::s_startupFile, 
                                 m_savesetFileName) ;
      }
    }
  }
    break;
  case M_Move_COMMAND:
    moveSelectedInDB();
    break;
  case M_DeletePage_COMMAND:
    deleteSelectedPageFromDB();
    break;
  case M_CreateFolder_COMMAND:
    createFolderInDB();
    break;
  case HELP_ABOUT_COMMAND:
    about();
    break;
  case PICK_REFERENCE_HISTO_COMMAND:
    pickReferenceHistogram();
    break;
  case HISTORY_PLOTS_COMMAND:
    toggleHistoryPlots();
    break;
  case OVERLAY_REFERENCE_HISTO_COMMAND:
    toggleReferenceOverlay();
    break;
  case FAST_HITMAP_DRAW_COMMAND:
    toggleFastHitMapDraw();
    break;
  case SAVE_AS_REFERENCE_HISTO_COMMAND:
    saveSelectedHistogramAsReference();
    break;
  default:
    if (m_verbosity >= pres::Debug)
      std::cout << "zut. TGButton WidgetId from gTQSender is corrupt"
                << std::endl;
    break;
  }
}

//==============================================================================
// Set verbosity of presenter
//==============================================================================
void PresenterMainFrame::setVerbosity(const pres::MsgLevel & verbosity) {
  m_verbosity = verbosity;
  if ( pres::Silent == m_verbosity) gErrorIgnoreLevel = 1001;
  if (0 != m_archive) { m_archive->setVerbosity(m_verbosity); }
}

//==============================================================================
// Set the database mode 
//==============================================================================
void PresenterMainFrame::setDatabaseMode(const pres::DatabaseMode & 
                                         databaseMode) {
  switch (databaseMode) {
  case pres::LoggedOut:
    logoutFromHistogramDB();
    break;
  case pres::UserSelected:
    logoutFromHistogramDB();
    loginToHistogramDB();
    break;
  case pres::ReadOnly:
    logoutFromHistogramDB();
    if (0 < m_knownDbCredentials.size()) {
      std::map<std::string*, std::string*>::iterator dbCred;
      dbCred = m_knownDbCredentials.begin();
      connectToHistogramDB(*(*dbCred).second,
                           *(*dbCred).first,
                           *m_knownDatabases[0]);
    }
    break;
  case pres::ReadWrite:
    logoutFromHistogramDB();
    loginToHistogramDB();
    break;
  default:
    if (m_verbosity >= pres::Debug) 
      std::cout << "something went wrong when setting database mode."
                << std::endl;
    break;
  }
}

//=============================================================================
// Set reference path
//=============================================================================
void PresenterMainFrame::setReferencePath(const std::string & referencePath) {
  m_referencePath = referencePath;
  if (m_analysisLib) m_analysisLib->setRefRoot(m_referencePath);
  if (!m_referencePath.empty()) {
    if (0 != m_archive) {
      m_archive->setReferencePath(m_referencePath);
    }
  }
}

//=============================================================================
// set saveset path
//=============================================================================
void PresenterMainFrame::setSavesetPath(const std::string & savesetPath) {
  m_savesetPath = savesetPath;
  if (!m_savesetPath.empty()) {
    if (0 != m_archive) {
      m_archive->setSavesetPath(m_savesetPath);
    }
  }
}

//=============================================================================
// Change the mode of the presenter
//=============================================================================
void PresenterMainFrame::setPresenterMode(const pres::PresenterMode & pMode) {
  switch ( pMode ) {
  case pres::Init:
    buildGUI();
    break;
  case pres::Batch:
    gROOT->SetBatch() ;
    m_presenterInfo.setPresenterMode( pres::Batch ) ;
    editorCanvas = new TCanvas("c", "c", 300, 300);
    editorCanvas->cd();
    gStyle->SetOptStat(0);
    break;
  case pres::Online:
    m_savesetFileName = "" ;
    m_prevPresenterMode = presenterMode();
    m_presenterInfo.setPresenterMode( pres::Online ) ;
    break;
  case pres::History:
    m_prevPresenterMode = presenterMode();
    m_presenterInfo.setPresenterMode( pres::History ) ;
    break;
  case pres::EditorOnline:
    m_prevPresenterMode = presenterMode();
    m_presenterInfo.setPresenterMode( pres::EditorOnline ) ;
    if (pres::ReadWrite != m_databaseMode) setDatabaseMode(pres::ReadWrite);
    break;
  case pres::EditorOffline:
    m_prevPresenterMode = presenterMode();
    m_presenterInfo.setPresenterMode( pres::EditorOffline ) ;
    if (pres::ReadWrite != m_databaseMode) setDatabaseMode(pres::ReadWrite);
    break;
  default:
    if (m_verbosity >= pres::Debug) 
      std::cout << "something went wrong when setting presenter mode."
                << std::endl;
    break;
  }
  if ( ! isBatch() ) reconfigureGUI();
}

//=============================================================================
// Set the known databases
//=============================================================================
void PresenterMainFrame::setKnownDatabases(const std::string & databasesCfg,
                                           const std::string & dbcredentials) {
  TObjArray* 
    databaseItems(TString(databasesCfg.c_str()).Tokenize(s_configToken));
  TIterator* databaseItemsIt = databaseItems->MakeIterator();
  TObject*   databaseItem;
  while ((databaseItem = databaseItemsIt->Next())) {
    std::string* databaseID = new std::string(databaseItem->GetName());
    m_knownDatabases.push_back(databaseID);
  }
  databaseItems->Delete();
  delete databaseItemsIt;
  databaseItemsIt = NULL;
  delete databaseItems;
  databaseItems = NULL;

  TObjArray* 
    dbLoginPairItems(TString(dbcredentials.c_str()).Tokenize(s_configToken));
  TIterator* dbLoginPairItemsIt = dbLoginPairItems->MakeIterator();
  TObject*   dbLoginItem;
  while ((dbLoginItem = dbLoginPairItemsIt->Next())) {
    TObjArray* loginElementItems(TString(dbLoginItem->GetName()).Tokenize(":"));
    TIterator* loginElementIt = loginElementItems->MakeIterator();
    TObject*   dbLoginElement = NULL;
    std::string* login = NULL;
    std::string* password = NULL;
    if ((dbLoginElement = loginElementIt->Next())) {
      login = new std::string(dbLoginElement->GetName());
    }

    if ((dbLoginElement = loginElementIt->Next())) {
      password = new std::string(dbLoginElement->GetName());
    }
    if (login && password) {
      m_knownDbCredentials[login] = password;
    }

    loginElementItems->Delete();
    delete loginElementIt;
    loginElementIt = NULL;
    delete loginElementItems;
    loginElementItems = NULL;
  }
  dbLoginPairItems->Delete();
  delete dbLoginPairItemsIt;
  dbLoginPairItemsIt = NULL;
  delete dbLoginPairItems;
  dbLoginPairItemsIt = NULL;
}

//==============================================================================
// Enable auto canvas layout
//==============================================================================
void PresenterMainFrame::enableAutoCanvasLayoutBtn() {
  m_autoCanvasLayoutButton->SetState(kButtonUp);
}

//==============================================================================
// Disable auto canvas layout
//==============================================================================
void PresenterMainFrame::disableAutoCanvasLayoutBtn() {
  m_autoCanvasLayoutButton->SetState(kButtonDisabled);
}

//==============================================================================
// check if connected to histogram db
//==============================================================================
bool PresenterMainFrame::isConnectedToHistogramDB() {
  if ( ( pres::LoggedOut == m_databaseMode ) && ( 0 == m_histogramDB ) ) 
    return false;
  if ( ( pres::ReadOnly == m_databaseMode ) && ( 0 != m_histogramDB ) ) 
    return true;
  if ( ( pres::ReadWrite == m_databaseMode ) && ( 0 != m_histogramDB ) ) 
    return true;
  return false;
}

//==============================================================================
// check if can write to histogram db
//==============================================================================
bool PresenterMainFrame::canWriteToHistogramDB() {
  if ( ( pres::ReadWrite == m_databaseMode ) && ( 0 != m_histogramDB ) ) 
    return true;
  return false;
}

//==============================================================================
// Save page to file
//==============================================================================
void PresenterMainFrame::savePageToFile() {
  if (m_refreshingPage) {
    stopPageRefresh();
    m_resumePageRefreshAfterLoading = true;
  }

  static const char *gSaveAsTypes[] = {"Encapsulated PostScript", "*.eps",
                                       "PDF",          "*.pdf",
                                       "SVG",          "*.svg",
                                       "GIF",          "*.gif",
                                       "ROOT macros",  "*.C",
                                       "ROOT files",   "*.root",
                                       "XML",          "*.xml",
                                       "PNG",          "*.png",
                                       "XPM",          "*.xpm",
                                       "JPEG",         "*.jpg",
                                       "TIFF",         "*.tiff",
                                       "XCF",          "*.xcf",
                                       "PostScript",   "*.ps",
                                       0,              0 };


  static TString dir(m_imagePath); //HomeDirectory(const char* userName = 0)
  static Int_t typeidx = 0;
  static Bool_t overwr = kFALSE;
  TGFileInfo fi;
  fi.fFileTypes   = gSaveAsTypes;
  fi.fIniDir      = StrDup(dir);
  fi.fFileTypeIdx = typeidx;
  fi.fOverwrite = overwr;
  new TGFileDialog(fClient->GetDefaultRoot(), this, kFDSave, &fi);
  TString fn = fi.fFilename;
  TString ft = fi.fFileTypes[fi.fFileTypeIdx+1];
  fn += ft(ft.Index("."), ft.Length());
  if (false == fn.IsNull()) {
    if (fn.EndsWith(".root") ||
        fn.EndsWith(".ps")   ||
        fn.EndsWith(".eps")  ||
        fn.EndsWith(".pdf")  ||
        fn.EndsWith(".svg")  ||
        fn.EndsWith(".gif")  ||
        fn.EndsWith(".xml")  ||
        fn.EndsWith(".xpm")  ||
        fn.EndsWith(".jpg")  ||
        fn.EndsWith(".png")  ||
        fn.EndsWith(".xcf")  ||
        fn.EndsWith(".tiff")) {
      editorCanvas->SaveAs(fn);
    } else if (fn.EndsWith(".C")) {
      editorCanvas->SaveSource(fn);
    }
  }
  if (m_resumePageRefreshAfterLoading) { startPageRefresh(); }
}

void PresenterMainFrame::savePageToHistogramDB() {
  if (pres::ReadWrite == m_databaseMode) {
    fClient->WaitFor(new DatabasePagePathDialog(this, 493, 339, m_verbosity));
    refreshPagesDBListTree();
  }
}
void PresenterMainFrame::reportToLog() {
  if (m_refreshingPage) {
    stopPageRefresh();
    m_resumePageRefreshAfterLoading = true;
  }

  if (false == m_logBookConfig.empty()) {

    ElogDialog* elogDialog = new ElogDialog(this, 646, 435 );
    // Default values.
    std::string logbook  = "Shift";
    std::string username = "Data Manager";
    std::string system   = currentPartition() ;
    std::string subject  = "";
    std::string message  = "See attached plot.";
    std::string title    = "";
    int         isOK     = 0;

    //== In LHCb partition, get the Data Manager name, extract the system 
    // from the SHIFTS page and remove subject
    if ( "LHCb" == currentPartition() ) {
      //== get name of Data Manager on shift
      ShiftDB shiftdb ;
      username =  shiftdb.getCurrentDataManager().c_str();
      if ( "/Shift/" == m_currentPageName.substr(0, 7) ) {
        system   = m_currentPageName.substr( 7 );
        system   = system.substr( 0, system.find( ':' ));
      } else if ( "/OfflineDataQuality/" == m_currentPageName.substr(0, 20) ) {
        username =  shiftdb.getDQPiquet().c_str();
        system   = m_currentPageName.substr( 20 );
        system   = system.substr( 0, system.find( ':' ));
      }
      subject = "-none-";
      if ( !m_pbdbConfig.empty() )  elogDialog->setProblemDatabase( title );
    }

    elogDialog->setParameters( logbook, username, system, subject, message, 
                               isOK );

    TString pageName;
    gSystem->TempFileName(pageName);
    pageName.Append(".png");
    editorCanvas->SaveAs(pageName.Data());

    fClient->WaitFor(dynamic_cast<TGWindow*>( elogDialog ));
    if ( 1 == isOK ) {
      Elog myElog( m_logBookConfig, 8080 );
      myElog.setCredential( "common", "Common!" );
      myElog.setLogbook( logbook );
      myElog.addAttachment( pageName.Data() );
      myElog.addAttribute( "Author", username );
      myElog.addAttribute( "System", system );
      if ( "Shift" != logbook ) myElog.addAttribute( "Subject", subject );
      std::cout << "send to Elog " << std::endl;
      int number = myElog.submit( message );
      std::cout << "=== produced entry " << number << std::endl;
      char statusMessage[100];
      char linkText[100];
      std::string message = "Created ELOG entry ";
      if ( 0 != number ) {
        sprintf( linkText, "http://lblogbook.cern.ch/%s/%d", logbook.c_str(), 
                 number );
        sprintf( statusMessage, "Entry %d created in logbook %s", number, 
                 logbook.c_str() );
        message = message + std::string( linkText );
      } else {
        sprintf( statusMessage, "Failed to create entry, see log file" );
        sprintf( linkText,  "Failed to create entry, see log file" );
        message = "*** Failed to create Elog entry, see log file ***";
        title = "";
      }

      m_mainStatusBar->SetText( statusMessage, 2);

      if ( "" != title ) {
        ProblemDB myProblem( m_pbdbConfig );
        std::string link( linkText );
        int stat = myProblem.post( system, username, title, message, link );
        std::cout << "Status = " << stat << " entry " << 
          myProblem.getReference() << std::endl;
        if ( 1 == stat ) {
          message = message + "\r\nCreated Problem Database entry " + 
            myProblem.getReference();
        } else {
          message = message + 
            "\r\n FAILED to create the problem database entry, see log file";
        }
      }

      new TGMsgBox( fClient->GetRoot(), this, "Elog and ProblemDatabase result",
                    message.c_str(), kMBIconExclamation, kMBOk, 
                    &m_msgBoxReturnCode );
    } else {
      std::string statusMessage = "No logbook entry created";
      m_mainStatusBar->SetText( statusMessage.c_str(), 2);
    }
    remove( pageName.Data() ); // delete temporary file.
  }
  if ( m_resumePageRefreshAfterLoading ) startPageRefresh() ;
}

//==============================================================================
// below is either HISTO DIM sevices, or a ROOT filename
// Open ROOT file at startup
//==============================================================================
void PresenterMainFrame::setStartupHistograms(const std::vector< std::string > & 
                                              histogramList) {
  std::vector< std::string >::const_iterator histogramListIt ;
  
  for ( histogramListIt = histogramList.begin() ; 
	histogramListIt != histogramList.end() ; ++histogramListIt ) {
    TString inputParamTS = TString( *histogramListIt ) ;
    if ( inputParamTS.EndsWith( pres::s_rootFileExtension.c_str() ) ) {
      m_savesetFileName = *histogramListIt ;
      setStatusBarText( m_savesetFileName.c_str() , 2 ) ;
      m_historyIntervalComboBox -> Select( M_Last_File , false ) ;
    } else {
      HistogramIdentifier histogram = HistogramIdentifier( *histogramListIt ) ;
      if (histogram.isDimFormat()) 
        addHistoToPage(*histogramListIt, pres::separate);
    }
  }

  if ( pres::Batch != presenterMode() ) autoCanvasLayout();
}

//==============================================================================
// Connect to the histogram database
//==============================================================================
bool PresenterMainFrame::connectToHistogramDB(const std::string & dbPassword,
                                              const std::string & dbUsername,
                                              const std::string & dbName) {
  if ( ( "" != dbPassword ) && ( "" != dbUsername ) && ( "" != dbUsername ) ) {
    removeHistogramsFromPage();
    try {
      if ( ( 0 != m_histogramDB ) && isConnectedToHistogramDB()) 
        cleanHistogramDB();
      
      m_histogramDB = new OnlineHistDB(dbPassword, dbUsername, dbName);

      // allocate vectors:
      m_histogramTypes.reserve(m_histogramDB->nHistograms());
      m_localDatabaseIDS.reserve(m_histogramDB->nHistograms());
      m_localDatabasePages.reserve(m_histogramDB->nPages());
      m_localDatabaseFolders.reserve(m_histogramDB->nPageFolders());

      m_histogramDB->setRefRoot(m_referencePath);
      m_histogramDB->setSavesetsRoot(m_savesetPath);
      if (0 != m_analysisLib) { delete m_analysisLib; m_analysisLib = NULL; }
      m_analysisLib = new OMAlib(m_histogramDB);
      m_analysisLib->setRefRoot(m_referencePath);
      if (0 != m_archive) { m_archive->setAnalysisLib(m_analysisLib); }

      m_dbName = dbName;
      m_message = "Successfully connected to OnlineHistDB";

      m_histogramDB->setExcLevel( pres::AllExceptions ) ;
      m_histogramDB->setDebug(m_verbosity);

      if (!m_savesetPath.empty()) {
        if (0 != m_archive) { m_archive->setSavesetPath(m_savesetPath); }
      }
      if (!m_referencePath.empty()) {
        if (0 != m_archive) { m_archive->setReferencePath(m_referencePath); }
      }

      if (m_histogramDB->canwrite()) {
        m_databaseMode = pres::ReadWrite;
      } else {
        m_databaseMode = pres::ReadOnly;
      }
      if ( pres::Batch != presenterMode()) {
        m_mainStatusBar->SetText(dbUsername.c_str(), 2);
        m_mainStatusBar->SetText(m_message.c_str(), 2);
      }

      if (m_verbosity >= pres::Verbose)
	std::cout << m_message  << " in "
		  << (m_databaseMode == pres::ReadWrite ? "read-write" : "read-only") 
		  << " mode" << std::endl ;

      if(m_alarmDisplay) { m_alarmDisplay->enable(true);} 

    } catch (std::string sqlException) {
      if (m_verbosity >= pres::Verbose) std::cout << sqlException << std::endl;
      if ( pres::Batch != presenterMode()) {
        m_mainStatusBar->SetText(sqlException.c_str(), 2);
        new TGMsgBox(fClient->GetRoot(), this, "Login Error",
                     Form("Could not connect to the OnlineHistDB server:\\n%s\n"
                          "This program cannot function correctly",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }

      if (0 != m_histogramDB) { cleanHistogramDB(); }
      m_databaseMode = pres::LoggedOut;
    }
  } else {
    if ( pres::Batch != presenterMode()) {
      new TGMsgBox(fClient->GetRoot(), this, "Login Error",
                   Form("Could not connect to the OnlineHistDB server:\n"
                        "Please enter valid credentials."),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
  }
  
  reconfigureGUI();
  return isConnectedToHistogramDB();
}

//==============================================================================
// Login to the histogram database
//==============================================================================
void PresenterMainFrame::loginToHistogramDB() {
  if ( pres::Batch != presenterMode()) {
    fClient->WaitFor(new LoginDialog(this, 350, 310, m_databaseMode,
                                     m_knownDatabases, m_knownDbCredentials));

    reconfigureGUI();
    removeHistogramsFromPage();
  }
}

//==============================================================================
// logout from histogram database
//==============================================================================
void PresenterMainFrame::logoutFromHistogramDB() {
  if ( ( 0 != m_histogramDB ) && isConnectedToHistogramDB() &&
       ( pres::Batch != presenterMode() ) ) {
    new TGMsgBox(fClient->GetRoot(), this, "Logout from Database",
                 "Do you really want to logout from Database?",
                 kMBIconQuestion, kMBYes | kMBNo, &m_msgBoxReturnCode);
    switch (m_msgBoxReturnCode) {
    case kMBNo:
      return;
    }
  }
  cleanHistogramDB();

  m_databaseMode = pres::LoggedOut;

  m_message = "Disconnected from histogramDB";
  if ( pres::Batch != presenterMode()) {
    m_mainStatusBar->SetText(m_message.c_str(), 2);
    reconfigureGUI();
    removeHistogramsFromPage();
  }
  if (m_verbosity >= pres::Verbose) std::cout << m_message << std::endl;
}

//==============================================================================
// Start the page refresh
//==============================================================================
void PresenterMainFrame::startPageRefresh() {
  if (false == m_loadingPage) {
    if ( pres::Batch != presenterMode()) {
      m_message = "Refresh started.";
      m_mainStatusBar->SetText(m_message.c_str(), 2);
      if (m_verbosity >= pres::Verbose) std::cout << m_message << std::endl;

      m_stopRefreshButton->SetState(kButtonUp);
      m_viewMenu->EnableEntry(STOP_COMMAND);
      m_startRefreshButton->SetState(kButtonDisabled);
      m_viewMenu->DisableEntry(START_COMMAND);
    }
    enablePageRefresh();
    m_pageRefreshTimer->TurnOn();
    m_refreshingPage = true;
  }
}

//==============================================================================
// Stop the page refresh
//==============================================================================
void PresenterMainFrame::stopPageRefresh() {
  disablePageRefresh();
  if (0 != m_pageRefreshTimer) {
    m_message = "Refresh stopped.";
    if (m_verbosity >= pres::Verbose) std::cout << m_message << std::endl;
    if ( pres::History != presenterMode()) {
      m_mainStatusBar->SetText(m_message.c_str(), 2);
      m_startRefreshButton->SetState(kButtonUp);
      m_viewMenu->EnableEntry(START_COMMAND);
    }
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(STOP_COMMAND);

    m_pageRefreshTimer->TurnOff();
    m_refreshingPage = false;
  }
}

//==============================================================================
// clear the histograms
//==============================================================================
void PresenterMainFrame::clearHistos() {
  if (m_clearedHistos) {
    m_message = "Reverted to integral.";
    m_clearHistoButton->SetState(kButtonUp);
    m_viewMenu->UnCheckEntry(CLEAR_HISTOS_COMMAND);
    m_clearedHistos = false;
    disableHistogramClearing();
  } else {
    m_message = "Histograms cleared.";
    m_clearHistoButton->SetState(kButtonDown);
    m_clearedHistos = true;
    m_viewMenu->CheckEntry(CLEAR_HISTOS_COMMAND);
    enableHistogramClearing();
  }
  if ( pres::History != presenterMode())  
    m_mainStatusBar->SetText(m_message.c_str(), 2);
  
  if (m_verbosity >= pres::Verbose) std::cout << m_message << std::endl;
}

//==============================================================================
// Pointer to histogram db (connect if needed)
//==============================================================================
OnlineHistDB* PresenterMainFrame::histogramDB() {
  if (!isConnectedToHistogramDB()) loginToHistogramDB();
  return m_histogramDB;
}

//==============================================================================
// list histograms from histogram db
//==============================================================================
void PresenterMainFrame::listHistogramsFromHistogramDB(TGListTree* listView,
                                                       const FilterCriteria & 
                                                       filterCriteria, 
                                                       bool histograms) {
  if (isConnectedToHistogramDB() && 0 != listView) {
    try {
      listView->UnmapWindow();
      gVirtualX->SetCursor(GetId(), 
                           gClient->GetResourcePool()->GetWaitCursor());

      m_treeNode = listView->GetFirstItem();
      deleteTreeChildrenItemsUserData(m_treeNode);
      listView->DeleteChildren(m_treeNode);
      listView->RenameItem(m_treeNode, TString(m_dbName));
      m_localDatabaseFolders.clear();
      m_localDatabaseIDS.clear();
      m_histogramTypes.clear();
      switch (filterCriteria) {
      case FoldersAndPages:
        m_histogramDB->getPageFolderNames(m_localDatabaseFolders, "_ALL_");
        break;
      case Subsystems:
        m_histogramDB->getSubsystems(m_localDatabaseFolders);
        break;
      case Tasks:
        m_histogramDB->getTasks(m_localDatabaseFolders);
        break;
      case HistogramsWithAnalysis:
        m_histogramDB->getHistogramsWithAnalysis(NULL, &m_localDatabaseIDS, 
                                                 &m_histogramTypes);
        break;
      case AnalysisHistograms:
        m_histogramDB->getAnalysisHistograms(NULL, &m_localDatabaseIDS, 
                                             &m_histogramTypes);
        break;
      case AllHistograms:
        m_histogramDB->getAllHistograms(NULL, &m_localDatabaseIDS, 
                                        &m_histogramTypes);
        break;
      default:
        break;
      }
      std::sort(m_localDatabaseFolders.begin(),
                m_localDatabaseFolders.end());

      if (filterCriteria == FoldersAndPages ||
          filterCriteria == Subsystems ||
          filterCriteria == Tasks) {
        for (m_folderIt = m_localDatabaseFolders.begin();
             m_folderIt != m_localDatabaseFolders.end(); ++m_folderIt) {
          m_folderItems = TString(*m_folderIt).Tokenize( pres::s_slash ) ;
          m_folderItemsIt = m_folderItems->MakeIterator();
          m_treeNode = listView->GetFirstItem();
          while ((m_folderItem = m_folderItemsIt->Next())) {
            if ((listView->FindChildByName(m_treeNode,
                                           m_folderItem->GetName()))) {
              m_treeNode = listView->FindChildByName(m_treeNode,
                                                     m_folderItem->GetName());
            } else {
              if (filterCriteria != Tasks) {
                std::string userFolderData( pres::s_FILE_URI ) ;
                m_treeNode = 
                  listView->AddItem(m_treeNode, m_folderItem->GetName());
                char path[ pres::s_maxPageNameLength ] ;
                strncpy(path, "", sizeof(path)-1);
                m_pagesFromHistoDBListTree->GetPathnameFromItem(m_treeNode, 
                                                                path);
                std::string folderPath(path);
                // Drop DB url
                folderPath = 
                  folderPath.erase(0, 
                                   strlen(m_pagesFromHistoDBListTree->
                                          GetFirstItem()->GetText())+1);
                userFolderData.append(folderPath);
                TObjString *objectString = 
                  new TObjString(userFolderData.c_str());
                m_treeNode->SetUserData(objectString);
                m_treeNode->SetDNDSource(kFALSE);
                m_treeNode->SetDNDTarget(kTRUE);
              }
            }
            if (m_folderItem == m_folderItems->Last() &&
                filterCriteria == FoldersAndPages) {
              m_localDatabasePages.clear();
              m_histogramDB->getPageNamesByFolder(*m_folderIt,
                                                  m_localDatabasePages);
              std::sort(m_localDatabasePages.begin(),
                        m_localDatabasePages.end());
              for (m_pageIt = m_localDatabasePages.begin();
                   m_pageIt != m_localDatabasePages.end(); ++m_pageIt) {
                std::string pageName = 
                  std::string(*m_pageIt).erase(0, (*m_folderIt).length()+1);
                m_pageNode = listView->AddItem(m_treeNode, pageName.c_str());

                std::string userPageData( pres::s_FILE_URI ) ;
                char path[ pres::s_maxPageNameLength ] ;
                strncpy(path, "", sizeof(path)-1);
                m_pagesFromHistoDBListTree->
                  GetPathnameFromItem(m_pageNode, path);
                std::string pagePath(path);
                // Drop DB url
                pagePath = pagePath.erase(0, 
                                          strlen(m_pagesFromHistoDBListTree->
                                                 GetFirstItem()->GetText())+1);
                userPageData.append(pagePath);
                TObjString *objectString = new TObjString(userPageData.c_str());
                m_pageNode->SetUserData(objectString);
                m_pageNode->SetDNDSource(kTRUE);
                m_pageNode->SetDNDTarget(kFALSE);
                listView->CheckItem(m_pageNode, false);
                setTreeNodeType(m_pageNode, pres::s_PAGE);
                if (histograms) {
                  m_localDatabaseIDS.clear();
                  m_histogramTypes.clear();
                  m_histogramDB->getHistogramsByPage(*m_pageIt, NULL,
                                                     &m_localDatabaseIDS,
                                                     &m_histogramTypes);
                  fillTreeNodeWithHistograms(listView, m_pageNode,
                                             &m_localDatabaseIDS,
                                             &m_histogramTypes);
                }
              }
            } else if (m_folderItem == m_folderItems->Last() &&
                       filterCriteria != FoldersAndPages) {
              if (histograms) {
                m_localDatabaseIDS.clear();
                m_histogramTypes.clear();
                if (filterCriteria == Tasks) {
                  m_histogramDB->getHistogramsByTask(*m_folderIt, NULL,
                                                     &m_localDatabaseIDS,
                                                     &m_histogramTypes);
                } else if (filterCriteria == Subsystems) {
                  m_histogramDB->getHistogramsBySubsystem(*m_folderIt, NULL,
                                                          &m_localDatabaseIDS,
                                                          &m_histogramTypes);
                }
                fillTreeNodeWithHistograms(listView, m_treeNode,
                                           &m_localDatabaseIDS,
                                           &m_histogramTypes);
              }
            }
          }
          m_folderItems->Delete();
          delete m_folderItems;
          m_folderItems = NULL;
          delete m_folderItemsIt;
          m_folderItemsIt = NULL;
        }
      } else if (filterCriteria == HistogramsWithAnalysis ||
                 filterCriteria == AnalysisHistograms ||
                 filterCriteria == AllHistograms) {
        fillTreeNodeWithHistograms(listView, listView->GetFirstItem(),
                                   &m_localDatabaseIDS, &m_histogramTypes);
      }
      fClient->NeedRedraw(listView);
      //      (listView->GetCanvas())->Update();
      Resize(); DoRedraw();
      m_message = "Histograms read from Database.";
      //      listView->MapSubwindows();
      listView->MapWindow();
      gVirtualX->SetCursor(GetId(), 
                           gClient->GetResourcePool()->GetDefaultCursor());

    } catch (std::string sqlException) {
      if (m_verbosity >= pres::Verbose) std::cout << sqlException << std::endl;
      if ( pres::Batch != presenterMode()) {
        m_mainStatusBar->SetText(sqlException.c_str());
        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("OnlineHistDB server:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }
    }
  } else if (!isConnectedToHistogramDB()) {
    m_message = "Histograms not read from Database.";
  }
  if ( pres::Batch != presenterMode()) {
    if ( pres::History != presenterMode())
      m_mainStatusBar->SetText(m_message.c_str(), 2);
    
    sortTreeChildrenItems(listView, listView->GetFirstItem());
    fClient->NeedRedraw(listView);
  }
  if (m_verbosity >= pres::Verbose) std::cout << m_message << std::endl; 

}

//==============================================================================
// fill tree with the histograms
//==============================================================================
void PresenterMainFrame::fillTreeNodeWithHistograms(TGListTree* listView,
                                                    TGListTreeItem* node, 
                                                    std::vector<std::string>*  
                                                    histogramList,
                                                    std::vector<std::string>*  
                                                    histogramTypes){
  listView->UnmapWindow();
  
  m_histogramType = histogramTypes->begin();
  for (m_histogramIt = histogramList->begin(); 
       m_histogramIt != histogramList->end(); 
       ++m_histogramIt, ++m_histogramType) {
    HistogramIdentifier histogramIdentifier = 
      HistogramIdentifier(*m_histogramIt);
    m_taskNode = node;
    // Taskname
    if (listView->
        FindChildByName(m_taskNode,
                        histogramIdentifier.histogramUTGID().c_str())) {
      m_taskNode = 
        listView->FindChildByName(m_taskNode,
                                  histogramIdentifier.histogramUTGID().c_str());
    } else {
      m_taskNode = 
        listView->AddItem(m_taskNode,
                          histogramIdentifier.histogramUTGID().c_str());
      setTreeNodeType(m_taskNode, pres::s_TASK);
    }
    listView->SetCheckBox(m_taskNode, true);
    listView->CheckItem(m_taskNode, false);
    m_taskNode->SetUserData(0);

    // Algorithm name
    m_algorithmNode = m_taskNode;
    if (listView->
        FindChildByName(m_algorithmNode,
                        histogramIdentifier.algorithmName().c_str())) {
      m_algorithmNode = 
        listView->FindChildByName(m_algorithmNode,
                                  histogramIdentifier.algorithmName().c_str());
    } else {
      m_algorithmNode = 
        listView->AddItem(m_algorithmNode,
                          histogramIdentifier.algorithmName().c_str());
      setTreeNodeType(m_algorithmNode, pres::s_ALGORITHM);
    }
    listView->SetCheckBox(m_algorithmNode, true);
    listView->CheckItem(m_algorithmNode, false);
    m_algorithmNode->SetUserData(0);
    // Histogramset name
    m_histogramSetNode = m_algorithmNode;
    if (histogramIdentifier.isFromHistogramSet()) {
      if (listView->
          FindChildByName(m_histogramSetNode,
                          histogramIdentifier.histogramSetName().c_str())) {
        m_histogramSetNode = 
          listView->
          FindChildByName(m_histogramSetNode,
                          histogramIdentifier.histogramSetName().c_str());
      } else {
        m_histogramSetNode = 
          listView->AddItem(m_histogramSetNode,
                            histogramIdentifier.histogramSetName().c_str());
        setTreeNodeType(m_histogramSetNode, pres::s_SET);
      }
      listView->SetCheckBox(m_histogramSetNode, true);
      listView->CheckItem(m_histogramSetNode, false);
      m_histogramSetNode->SetUserData(0);
    }
    // Histogram name
    m_histogramNode = m_histogramSetNode;
    m_histogramIdItems = TString(histogramIdentifier.histogramName()).
      Tokenize(pres::s_slash);
    m_histogramIdItemsIt = m_histogramIdItems->MakeIterator();

    while ((m_histogramIdItem = m_histogramIdItemsIt->Next())) {
      if (m_histogramIdItem != m_histogramIdItems->Last()) {
        if ((listView->FindChildByName(m_histogramNode,
                                       m_histogramIdItem->GetName()))) {
          m_histogramNode = 
            listView->FindChildByName(m_histogramNode,
                                      m_histogramIdItem->GetName());
        } else 
          m_histogramNode = listView->AddItem(m_histogramNode,
                                              m_histogramIdItem->GetName());

        listView->SetCheckBox(m_histogramNode, true);
        listView->CheckItem(m_histogramNode, false);
        m_histogramNode->SetUserData(0);
        setTreeNodeType(m_histogramNode, pres::s_LEVEL);
      }
      if (m_histogramIdItem == m_histogramIdItems->Last()) {
        m_histogramNode = listView->AddItem(m_histogramNode,
                                            m_histogramIdItem->GetName());
        listView->SetCheckBox(m_histogramNode, true);
        listView->CheckItem(m_histogramNode, false);
        setTreeNodeType(m_histogramNode, *m_histogramType);
        m_histogramNode->SetUserData(new TObjString((*m_histogramIt).c_str()));
      }
    }
    m_histogramIdItems->Delete();
    delete m_histogramIdItems;
    m_histogramIdItems = NULL;
    delete m_histogramIdItemsIt;
    m_histogramIdItemsIt = NULL;
  }
  listView->MapWindow();
}

//==============================================================================
// Start benchmark
//==============================================================================
void PresenterMainFrame::startBenchmark(const std::string &timer) { 
  m_benchmark->Start(timer.c_str()) ; 
}

//==============================================================================
// Stop benchmark
//==============================================================================
void PresenterMainFrame::stopBenchmark(const std::string &timer) { 
  m_benchmark->Stop(timer.c_str());
}

//==============================================================================
// print benchmark
//==============================================================================
void PresenterMainFrame::printBenchmark(const std::string &timer) {
  std::cout << m_deadTasksOnPage << ","
            << timer << ","
            << (m_benchmark->GetRealTime(timer.c_str())) << ","
            << int(dbHistosOnPage.size()) << ",";
  std::vector<DbRootHist*>::iterator timer_dbHistosOnPageIt = 
    dbHistosOnPage.begin();
  while( timer_dbHistosOnPageIt !=  dbHistosOnPage.end() ) {
    std::cout << (*timer_dbHistosOnPageIt)->identifier() << ","
              << int(((*timer_dbHistosOnPageIt)->getRootHistogram())->
                     GetEntries()) << ","
              << m_benchmark->
      GetRealTime((*timer_dbHistosOnPageIt)->identifier().c_str()) << ",";
    timer_dbHistosOnPageIt++;
  }
  std::cout << std::endl ;
  m_benchmark->Reset();
}

//==============================================================================
// List histograms from a ROOT file
//==============================================================================
void PresenterMainFrame::listRootHistogramsFrom(TDirectory* rootFile,
                                                std::vector<std::string> & 
                                                histogramList,
                                                std::vector<std::string> & 
                                                histogramTypes,
                                                std::string& taskName,
                                                pres::SavesetType savesetType) {

  std::string::size_type fileNameLength = 0;
  std::string task = taskName;
  if (pres::OfflineFile == savesetType)
    fileNameLength = std::string((rootFile->GetFile())->GetName()).size() ;

  TDirectory* tmpdir = gDirectory;
  TIter next(rootFile->GetListOfKeys());
  TKey* key;
  std::string type("");
  std::string hid("");

  std::stringstream histogramIDStream;

  while ((key = (TKey*)next())) {

    if (key->IsFolder()) {
      rootFile->cd(key->GetName());
      TDirectory *subdirectory = gDirectory;
      std::string pathName = taskName + pres::s_slash + key->GetName();
      listRootHistogramsFrom(subdirectory, histogramList, histogramTypes,
                             pathName, savesetType);
      tmpdir->cd();
      continue;
    }
    if ((std::string(key->GetClassName()) == "TH1D")||
	(std::string(key->GetClassName()) == "TH1F")) {
      type = s_pfixMonH1D;
      if (pres::OfflineFile == savesetType) {
	if ( std::string(key->GetClassName()) == "TH1D" ) 
	  histogramIDStream << ((TH1D*)key->ReadObj())->GetName();
	else 
	  histogramIDStream << ((TH1F*)key->ReadObj())->GetName();
      } else {
        histogramIDStream << s_pfixMonH1D << pres::s_slash << taskName 
			  << pres::s_slash << key->GetName();
      }
    } else if ((std::string(key->GetClassName()) == "TH2D")||
	       (std::string(key->GetClassName()) == "TH2F")) {
      type = s_pfixMonH2D;
      if (pres::OfflineFile == savesetType) {
	if ( std::string(key->GetClassName()) == "TH2D" )
	  histogramIDStream << ((TH2D*)key->ReadObj())->GetName();
	else
	  histogramIDStream << ((TH2F*)key->ReadObj())->GetName();
      } else {
        histogramIDStream << s_pfixMonH1D << pres::s_slash << taskName 
			  << pres::s_slash << key->GetName();
      }
    } else if (std::string(key->GetClassName()) == "TProfile") {
      type = s_pfixMonProfile;
      if (pres::OfflineFile == savesetType) {
        histogramIDStream << ((TProfile*)key->ReadObj())->GetName();
      } else {
        histogramIDStream << s_pfixMonH1D << pres::s_slash << taskName 
			  << pres::s_slash << key->GetName();
      }

    }
    std::string histogramID = histogramIDStream.str();
    if (pres::OfflineFile == savesetType) {
      if (histogramID.size() > fileNameLength) {
        histogramID = histogramID.erase(0, fileNameLength+1);
      }
    }
    if (std::string(key->GetClassName()) == "TH1D" ||
	std::string(key->GetClassName()) == "TH1F" ||
        std::string(key->GetClassName()) == "TH2D" ||
	std::string(key->GetClassName()) == "TH2F" ||
        std::string(key->GetClassName()) == "TProfile") {
      if (pres::OfflineFile == savesetType) {
        histogramID = 
          type + pres::s_slash + taskName + pres::s_slash + histogramID;
      }
      histogramTypes.push_back(type);
      histogramList.push_back(histogramID);
      if (m_verbosity >= pres::Verbose) 
        std::cout << "Found: " << histogramID << std::endl;
    }
    histogramIDStream.str("");
  }
}

//==============================================================================
// Set tree node type
//==============================================================================
void PresenterMainFrame::setTreeNodeType(TGListTreeItem* node,
                                         const std::string & type) {
  if ( 0 == node ) return ;
  const TGPicture*  m_icon;
  
  if ( ( pres::s_H1D == type ) || ( s_pfixMonH1D == type ) ) 
    m_icon = m_iconH1D ;
  else if ( ( pres::s_H2D == type ) || ( s_pfixMonH2D == type ) ) 
    m_icon = m_iconH2D ; 
  else if ( ( pres::s_P1D == type ) || ( s_pfixMonProfile == type ) || 
	    ( pres::s_HPD == type ) || ( pres::s_P2D == type ) ) 
    m_icon = m_iconProfile ;
  else if ( pres::s_CNT == type )  m_icon = m_iconCounter;
  else if ( pres::s_PAGE == type )  m_icon = m_iconPage; 
  else if ( pres::s_TASK == type) m_icon = m_iconTask ; 
  else if ( pres::s_ALGORITHM == type )  m_icon = m_iconAlgorithm ;
  else if ( pres::s_SET == type ) m_icon = m_iconSet; 
  else if ( pres::s_LEVEL == type ) m_icon = m_iconLevel; 
  else if ( pres::s_ALARM == type ) m_icon = m_iconAlarm; 
  else if ( pres::s_WARNING == type ) m_icon = m_iconWarning; 
  else if ( pres::s_INFO == type ) m_icon = m_iconInfo; 
  else m_icon = m_iconQuestion; 

  node->SetPictures(m_icon, m_icon);
}

//==============================================================================
// Checked tree items
//==============================================================================
void PresenterMainFrame::checkedTreeItems(TGListTree* selected,
                                          TGListTree* treeList) {
  if (!selected) return;
  TGListTreeItem* node;
  node = treeList->GetFirstItem();
  if (node->IsChecked() && node->GetUserData()) {
    selected->AddItem(0, 0, node->GetUserData());
  }
  while (0 != node) {
    if (node->GetFirstChild()) {
      checkedTreeItemsChildren(node->GetFirstChild(), selected);
    }
    node = node->GetNextSibling();
  }
}

//==============================================================================
// Checked tree item childern
//==============================================================================
void PresenterMainFrame::checkedTreeItemsChildren(TGListTreeItem* node,
                                                  TGListTree* selected) {
  while (0 != node) {
    if (node->IsChecked() && node->GetUserData()) {
      selected->AddItem(0, 0, node->GetUserData());
    }
    if (node->GetFirstChild()) {
      checkedTreeItemsChildren(node->GetFirstChild(), selected);
    }
    node = node->GetNextSibling();
  }
}

//==============================================================================
// Collapse tree children
//==============================================================================
void PresenterMainFrame::collapseTreeChildrenItems(TGListTree* treeList,
                                                   TGListTreeItem* node) {
  if (0 != node && 0 !=  treeList) {
    treeList->CloseItem(node);
  }
  while (0 != node) {
    if (node->GetFirstChild()) {
      collapseTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}

//==============================================================================
// Collapse tress children items
//==============================================================================
void PresenterMainFrame::collapseTreeChildrenItemsChildren(
                                                           TGListTree* treeList, 
                                                           TGListTreeItem* 
                                                           node) {
  while (0 != node) {
    if (node && treeList) {
      treeList->CloseItem(node);
    }
    if (node->GetFirstChild()) {
      collapseTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}

//==============================================================================
// Sort tree children items
//==============================================================================
void PresenterMainFrame::sortTreeChildrenItems(TGListTree* treeList,
                                               TGListTreeItem* node) {
  if (0 != node && 0 != treeList) {
    treeList->SortChildren(node);
  }
  while (0 != node) {
    if (0 != (node->GetFirstChild())) {
      sortTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}

//==============================================================================
// Sort tree children items
//==============================================================================
void PresenterMainFrame::sortTreeChildrenItemsChildren(TGListTree* treeList,
                                                       TGListTreeItem* node) {
  while (0 != node) {
    if (0 != node && 0 != treeList) {
      treeList->SortChildren(node);
    }
    if (0 != (node->GetFirstChild())) {
      sortTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}

//==============================================================================
// Delete the node and all its children
//==============================================================================
void PresenterMainFrame::deleteTreeChildrenItemsUserData(TGListTreeItem* node) {
  if (0 != node && 0 != (node->GetUserData())) {
    delete (TObjString*)(node->GetUserData());
    node->SetUserData(NULL);
  }
  if (0 != node && 0 != (node->GetFirstChild())) {
    deleteTreeChildrenItemsUserDataChildren(node->GetFirstChild());
  }
}

//==============================================================================
// Delete recursively all children of this node
//==============================================================================
void PresenterMainFrame::deleteTreeChildrenItemsUserDataChildren(TGListTreeItem*
                                                                 node) {
  while (0 != node) {
    if (0 != (node->GetUserData())) {
      delete (TObjString*)(node->GetUserData());
      node->SetUserData(NULL);
    }
    if (0 != (node->GetFirstChild())) {
      deleteTreeChildrenItemsUserDataChildren(node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}

//==============================================================================
// Get the text of the status bar
//==============================================================================
const char*	PresenterMainFrame::getStatusBarText(int slice) {
  if ( pres:: Batch != presenterMode()) return m_mainStatusBar->GetText(slice);
  return (const char*) "";
}

//==============================================================================
// set the text of the status bar
//==============================================================================
void PresenterMainFrame::setStatusBarText(const char* text, int slice) {
  if ( pres::Batch != presenterMode() ) 
    m_mainStatusBar -> SetText( text , slice);
}

//==============================================================================
// set the text of the top status bar
//==============================================================================
void PresenterMainFrame::setTopStatusBarText(const char* text, int slice) {
  if ( pres::Batch != presenterMode() ) m_statusBarTop->SetText(text, slice);
}

//==============================================================================
// Edit histogram properties
//==============================================================================
void PresenterMainFrame::editHistogramProperties() {
  bool referenceOverlay(false);
  if (m_referencesOverlayed) {
    disableReferenceOverlay();
    referenceOverlay = true;
  }
  DbRootHist* selectedDbHisto = selectedDbRootHistogram();
  if (0 != selectedDbHisto) {
    if (0 != selectedDbHisto) {
      TH1* selectedHisto = selectedDbHisto->getRootHistogram();
      if (0 != selectedHisto) {
        selectedHisto->DrawPanel();
      }
    }
  }
  if (referenceOverlay) { enableReferenceOverlay(); }
}

//==============================================================================
// Inspect histogram
//==============================================================================
void PresenterMainFrame::inspectHistogram() {
  DbRootHist* selectedDbHisto = selectedDbRootHistogram();
  if ( 0 == selectedDbHisto ) return ;
  
  bool referenceOverlay(false);
  if ( m_referencesOverlayed ) {
    disableReferenceOverlay() ;
    referenceOverlay = true ;
  }

  TH1* selectedHisto = selectedDbHisto->getRootHistogram();
  if (0 == selectedHisto) return ;

  TCanvas* zoomCanvas =  new TCanvas();
  zoomCanvas->cd();
  selectedDbHisto->getRootHistogram()->DrawCopy();
  zoomCanvas->ToggleEventStatus();
  zoomCanvas->ToggleAutoExec();
  zoomCanvas->ToggleToolBar();
  zoomCanvas->ToggleEditor();

  if (referenceOverlay) { enableReferenceOverlay(); }
}

//==============================================================================
// Histogram description button
//==============================================================================
void PresenterMainFrame::histogramDescription() {
  if (isConnectedToHistogramDB()) {

    m_currentPageHistogramHelp.clear();
    std::stringstream currentPageHistogramHelp;

    DbRootHist* histogram = selectedDbRootHistogram();
    if (0 != histogram) {

      currentPageHistogramHelp 
        << "Title:\t\t"  << (histogram->onlineHistogram())->htitle() 
        << std::endl << std::endl
        << "Name:\t\t" <<  (histogram->onlineHistogram())->hname() << std::endl
        << "Type:\t\t" <<  (histogram->onlineHistogram())->hstype() << std::endl
        << "Task:\t\t" <<  (histogram->onlineHistogram())->task() << std::endl
        << "Algorithm:\t" <<  (histogram->onlineHistogram())->algorithm() 
        << std::endl
        << "ROOT name:\t" <<  (histogram->onlineHistogram())->rootName()  
        << std::endl
        << "DIM in DB:\t" <<  (histogram->onlineHistogram())->dimServiceName() 
        << std::endl << std::endl
        << "Description:" << std::endl 
        << (histogram->onlineHistogram())->descr() << std::endl << std::endl;

      m_currentPageHistogramHelp = currentPageHistogramHelp.str().c_str();

      TRootHelpDialog *hd = 
        new TRootHelpDialog(this, "Histogram Description", 600, 400);
      hd->SetText(Form("%s", m_currentPageHistogramHelp.c_str()));
      hd->Popup();
    }
  } else {
    if ( ! isBatch() ) 
      new TGMsgBox(fClient->GetRoot(), this, "Description Unavailable",
                   "Description Unavailable, not logged in!",
                   kMBIconExclamation, kMBClose, &m_msgBoxReturnCode);
  }
}

//==============================================================================
// Inspect the page
//==============================================================================
void PresenterMainFrame::inspectPage() {
  TCanvas* inspectCanvas =  new TCanvas();
  inspectCanvas->cd();
  inspectCanvas->SetTitle("Inspect Page");
  editorCanvas->DrawClonePad();
  inspectCanvas->ToggleEventStatus();
  inspectCanvas->ToggleAutoExec();
  inspectCanvas->ToggleToolBar();
  inspectCanvas->ToggleEditor();
}

//==============================================================================
// Reconfigure the GUI (when mode has changed for example)
//==============================================================================
void PresenterMainFrame::reconfigureGUI() {
  removeHistogramsFromPage();
  UInt_t current_width  = 0;
  UInt_t current_height = 0;
  current_width = this->GetWidth();
  current_height = this->GetHeight();
  if ( isConnectedToHistogramDB( ) ) {
    refreshPagesDBListTree();
    if(m_alarmDisplay) {
      m_alarmDisplay->enable(true);
      m_alarmDisplay->listAlarmsFromHistogramDB();
    }
    enableAutoCanvasLayoutBtn();
    if ( ( canWriteToHistogramDB() ) &&
	 ( ( pres::EditorOnline == presenterMode() ) ||
	   ( pres::EditorOffline == presenterMode() ) || 
	   ( pres::History == presenterMode () ) ) ) showDBTools(pres::ReadWrite);
    else showDBTools(pres::ReadOnly);
  } else hideDBTools();

  if ( pres::Online == presenterMode()) {
    if ( m_presenterInfo.isHistoryTrendPlotMode() ) toggleHistoryPlots() ; 

    m_toolMenu->CheckEntry(ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_OFFLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(OFFLINE_MODE_COMMAND);
    m_rightMiscFrame->UnmapWindow();
    m_rightVerticalSplitter->UnmapWindow();

    m_startRefreshButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(START_COMMAND);
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(STOP_COMMAND);
    m_clearHistoButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(CLEAR_HISTOS_COMMAND);

    m_previousIntervalButton->SetState(kButtonDisabled);
    m_nextIntervalButton->SetState(kButtonDisabled);

    m_historyPlotsButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(HISTORY_PLOTS_COMMAND);

    m_overlayReferenceHistoButton->SetState(kButtonEngaged);
    m_overlayReferenceHistoButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);

    m_pickReferenceHistoButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(PICK_REFERENCE_HISTO_COMMAND);

    m_viewMenu->DisableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);

    m_historyIntervalComboBox->SetEnabled(false);
  } else if ( pres::History == presenterMode()) {
    disablePageRefresh();
    stopPageRefresh();

    m_toolMenu->CheckEntry(OFFLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_OFFLINE_MODE_COMMAND);
    m_rightMiscFrame->UnmapWindow();
    m_rightVerticalSplitter->UnmapWindow();

    m_startRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(START_COMMAND);
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(STOP_COMMAND);
    m_clearHistoButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(CLEAR_HISTOS_COMMAND);

    m_previousIntervalButton->SetState(kButtonEngaged);
    m_previousIntervalButton->SetState(kButtonUp);
    m_nextIntervalButton->SetState(kButtonEngaged);
    m_nextIntervalButton->SetState(kButtonUp);

    m_historyIntervalComboBox->SetEnabled(true);
    m_historyPlotsButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(HISTORY_PLOTS_COMMAND);

    m_overlayReferenceHistoButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);

    m_pickReferenceHistoButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(PICK_REFERENCE_HISTO_COMMAND);

    m_viewMenu->DisableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);

    // hide refreshHistoDBListTree
    if (( pres::Online == m_prevPresenterMode) &&
        ( presenterMode() != m_prevPresenterMode)) {
      partitionSelectorComboBoxHandler( 1 ) ;
    }
  } else if (pres::EditorOnline == presenterMode()) {
    stopPageRefresh();
    unclearHistosIfNeeded();
    if (m_referencesOverlayed) { toggleReferenceOverlay(); }
    if ( m_presenterInfo.isHistoryTrendPlotMode() ) toggleHistoryPlots(); 
    // enable play/stop/reset!
    m_toolMenu->CheckEntry(PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_OFFLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(OFFLINE_MODE_COMMAND);

    m_previousIntervalButton->SetState(kButtonDisabled);
    m_nextIntervalButton->SetState(kButtonDisabled);
    m_rightMiscFrame->MapWindow();
    m_rightVerticalSplitter->MapWindow();

    if (( pres::Online == m_prevPresenterMode || 
          pres::History == m_prevPresenterMode) &&
        presenterMode() != m_prevPresenterMode ) 
      refreshHistogramSvcList(pres::s_withTree);

    if (isConnectedToHistogramDB()) refreshHistoDBListTree() ;

    m_startRefreshButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(START_COMMAND);
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(STOP_COMMAND);
    m_clearHistoButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(CLEAR_HISTOS_COMMAND);

    m_historyIntervalComboBox->SetEnabled(true);

    m_historyPlotsButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(HISTORY_PLOTS_COMMAND);

    m_overlayReferenceHistoButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);

    // show refreshHistoDBListTree
  } else if (pres::EditorOffline == presenterMode()) {
    stopPageRefresh();
    unclearHistosIfNeeded();
    if (m_referencesOverlayed) { toggleReferenceOverlay(); }
    if ( m_presenterInfo.isHistoryTrendPlotMode() ) toggleHistoryPlots() ; 
    // enable play/stop/reset!
    m_toolMenu->CheckEntry(PAGE_EDITOR_OFFLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(OFFLINE_MODE_COMMAND);

    if (isConnectedToHistogramDB()) { refreshHistoDBListTree(); }

    m_previousIntervalButton->SetState(kButtonDisabled);
    m_nextIntervalButton->SetState(kButtonDisabled);
    m_rightMiscFrame->MapWindow();
    m_rightVerticalSplitter->MapWindow();
    m_startRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(START_COMMAND);
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(STOP_COMMAND);
    m_clearHistoButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(CLEAR_HISTOS_COMMAND);

    m_historyIntervalComboBox->SetEnabled(true);

    m_historyPlotsButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(HISTORY_PLOTS_COMMAND);

    m_overlayReferenceHistoButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);

    if (( pres::Online == m_prevPresenterMode || 
          pres::History == m_prevPresenterMode) &&
        presenterMode() != m_prevPresenterMode ) {
      refreshHistogramSvcList(pres::s_withTree);
      partitionSelectorComboBoxHandler( 1 );
    }
  }

  fClient->NeedRedraw(this);
  this->Resize(current_width,current_height);
  DoRedraw();
}

//==============================================================================
// Hide DB tools
//==============================================================================
void PresenterMainFrame::hideDBTools() {
  m_histoDBFilterComboBox->SetEnabled(false);

  m_histoDBCanvasViewPort->HideFrame(m_databaseHistogramTreeList);
  m_pagesFromHistoDBViewPort->HideFrame(m_pagesFromHistoDBListTree);
  m_histoSvcTreeContextMenu->DisableEntry(M_AddHistoToDB_COMMAND);
  m_histoSvcTreeContextMenu->EnableEntry(M_AddHistoToPage_COMMAND);
  m_loginButton->SetState(kButtonUp);
  m_logoutButton->SetState(kButtonDisabled);
  m_savePageToDatabaseButton->SetState(kButtonDisabled);
  m_pickReferenceHistoButton->SetState(kButtonDisabled);

  m_fileMenu->EnableEntry(LOGIN_COMMAND);
  m_fileMenu->DisableEntry(LOGOUT_COMMAND);
  m_fileMenu->DisableEntry(SAVE_PAGE_TO_DB_COMMAND);
}

//==============================================================================
// Show DB tools
//==============================================================================
void PresenterMainFrame::showDBTools(pres::DatabaseMode databasePermissions) {
  m_logoutButton->SetState(kButtonUp);
  m_loginButton->SetState(kButtonDisabled);

  m_fileMenu->DisableEntry(LOGIN_COMMAND);
  m_fileMenu->EnableEntry(LOGOUT_COMMAND);

  m_histoDBFilterComboBox->SetEnabled(true);
  m_databaseHistogramTreeList->SetBackgroundColor(GetWhitePixel());
  m_histoDBCanvasViewPort->ShowFrame(m_databaseHistogramTreeList);
  m_pagesFromHistoDBViewPort->ShowFrame(m_pagesFromHistoDBListTree);

  if (pres::ReadWrite == databasePermissions) {
    m_histoSvcTreeContextMenu->EnableEntry(M_AddHistoToDB_COMMAND);
    m_savePageToDatabaseButton->SetState(kButtonUp);
    m_pickReferenceHistoButton->SetState(kButtonUp);
    m_fileMenu->EnableEntry(SAVE_PAGE_TO_DB_COMMAND);
    m_pagesContextMenu->EnableEntry(M_Move_COMMAND);
    m_pagesContextMenu->EnableEntry(M_DeletePage_COMMAND);
    m_pagesContextMenu->EnableEntry(M_CreateFolder_COMMAND);
    m_histoDBContextMenu->EnableEntry(M_SetHistoPropertiesInDB_COMMAND);
    m_histoDBContextMenu->EnableEntry(M_DeleteDBHisto_COMMAND);

    m_pickReferenceHistoButton->SetState(kButtonUp);
    m_editMenu->EnableEntry(PICK_REFERENCE_HISTO_COMMAND);
    m_editMenu->EnableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);

  } else if ( pres::ReadOnly == databasePermissions) {
    m_savePageToDatabaseButton->SetState(kButtonDisabled);
    m_fileMenu->DisableEntry(SAVE_PAGE_TO_DB_COMMAND);
    m_histoSvcTreeContextMenu->DisableEntry(M_AddHistoToDB_COMMAND);
    m_pagesContextMenu->DisableEntry(M_Move_COMMAND);
    m_pagesContextMenu->DisableEntry(M_DeletePage_COMMAND);
    m_pagesContextMenu->DisableEntry(M_CreateFolder_COMMAND);
    m_histoDBContextMenu->DisableEntry(M_SetHistoPropertiesInDB_COMMAND);
    m_histoDBContextMenu->DisableEntry(M_DeleteDBHisto_COMMAND);

    m_pickReferenceHistoButton->SetState(kButtonDisabled);
    m_editMenu->DisableEntry(PICK_REFERENCE_HISTO_COMMAND);
    m_editMenu->DisableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);
  }
  m_histoSvcTreeContextMenu->DisableEntry(M_AddHistoToPage_COMMAND);

  enablePageLoading();
}

//==============================================================================
// refresh histo list from DB
//==============================================================================
void PresenterMainFrame::refreshHistoDBListTree() {
  if (m_verbosity >= pres::Verbose) std::cout << m_message << std::endl;
  listHistogramsFromHistogramDB( m_databaseHistogramTreeList,
                                 static_cast<FilterCriteria>
                                 ( m_histoDBFilterComboBox->GetSelected() ),
                                 pres::s_withHistograms ) ;
}

//==============================================================================
// refresh pages list from DB
//==============================================================================
void PresenterMainFrame::refreshPagesDBListTree() {
  listHistogramsFromHistogramDB(m_pagesFromHistoDBListTree, FoldersAndPages,
                                pres::s_withoutHistograms);
  openHistogramTreeAt( "" ) ;
}

//==============================================================================
// refresh histogram service list
//==============================================================================
void PresenterMainFrame::refreshHistogramSvcList(bool tree) {
  if ( ! isBatch() ) {
    gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetWaitCursor());
    if (m_refreshingPage) {
      stopPageRefresh();
      m_resumePageRefreshAfterLoading = true;
    }

    m_knownHistogramServices.clear();
    m_histogramTypes.clear();

    if ( pres::Online == presenterMode() || 
         pres::EditorOnline == presenterMode()) {

      m_dimBrowserDock->SetWindowName("DIM Histogram Browser");
      m_histoSvcBrowserGroupFrame->SetTitle("DIM Histogram Browser");

      m_candidateDimServices.clear();
      m_knownOnlinePartitionList->Delete();

      char *dimService;
      char *dimFormat;
      int dimType;

      std::string serviceType = pres::s_Mon;
      serviceType.append( pres::s_DimWildcard ) ;
      m_dimBrowser->getServices(serviceType.c_str());
      while((dimType = m_dimBrowser->getNextService(dimService, dimFormat))) {
        TString dimMon(dimService);
        if (dimMon.BeginsWith(s_pfixMonH1D.c_str()) ||
            dimMon.BeginsWith(s_pfixMonH2D.c_str()) ||
            dimMon.BeginsWith(s_pfixMonProfile.c_str())) {
          m_candidateDimServices.push_back(std::string(dimService));
        }
      }

      serviceType = 
        pres::s_DimWildcard + pres::s_eff_monRate + pres::s_DimWildcard;
      m_dimBrowser->getServices(serviceType.c_str());
      while((dimType = m_dimBrowser->getNextService(dimService, dimFormat))) {
        TString dimMon(dimService);
        TObjArray* dimHltCntMatchGroup = 0;
        dimHltCntMatchGroup = pres::s_DimHltCntRegexp.MatchS(dimFormat);
        if ((true == dimMon.Contains( pres::s_adder.c_str())) &&
            (true == dimMon.Contains( pres::s_eff_monRate.c_str())) &&
            (false == dimHltCntMatchGroup->IsEmpty())) {
          if (false == dimMon.BeginsWith( pres::s_CNT.c_str())) {
            dimMon.Prepend( pres::s_slash.c_str());
            dimMon.Prepend( pres::s_CNT.c_str());
          }
          HistogramIdentifier histogramIdentifier = 
            HistogramIdentifier(dimMon.Data());
          if (histogramIdentifier.isPlausible()) {
            m_candidateDimServices.push_back(dimMon.Data());
          }
        }
        if (dimHltCntMatchGroup) {
          dimHltCntMatchGroup->Delete();
          delete dimHltCntMatchGroup;
          dimHltCntMatchGroup = 0;
        }
      }
      if (0 != m_histoSvcListTree && 0 != m_dimBrowser) {
        const int nDimServers = m_dimBrowser->getServers();

        if (m_verbosity >= pres::Verbose) 
          std::cout << "nDimServers:\t" << nDimServers << std::endl;

        TString dimServerName;
        TString dimServerNodeName;
        TString dimServiceName;
        TString statusMessage;

        // If some servers found, discover what services are available
        if (nDimServers > 0) {
          const char* dimDnsServerNode = DimClient::getDnsNode();

          if (m_verbosity >= pres::Verbose) 
            std::cout << std::endl << "DNS: " << dimDnsServerNode << std::endl;

          setStatusBarText(dimDnsServerNode, 2);
          if (pres::s_withTree == tree) {
            TGListTreeItem* node = m_histoSvcListTree->GetFirstItem();
            m_histoSvcListTree->RenameItem(node, dimDnsServerNode);

            (m_histoSvcListTree->GetFirstItem())->SetPictures(m_dimOnline16,
                                                              m_dimOnline16);


            deleteTreeChildrenItemsUserData(node);
            m_histoSvcListTree->DeleteChildren(node);
          }

          m_candidateDimServicesIt = m_candidateDimServices.begin();
          while (m_candidateDimServicesIt != m_candidateDimServices.end()) {
            TString dimServiceTS = TString(*m_candidateDimServicesIt);
            HistogramIdentifier histogramService = 
              HistogramIdentifier(*m_candidateDimServicesIt);
            if (histogramService.isPlausible()) {
              std::string candidateDimService(*m_candidateDimServicesIt);
              if (pres::s_withTree == tree) {
                if ( pres::s_CNT == histogramService.histogramType()) {
                  candidateDimService = 
		    candidateDimService.erase(0, pres::s_CNT.length()).erase(0, 
									     pres::s_slash.length());
                }

                m_knownHistogramServices.push_back(candidateDimService);
                m_histogramTypes.push_back(histogramService.histogramType());
              }
              if (!m_knownOnlinePartitionList->
                  FindObject(histogramService.partitionName().c_str())) {
                TObjString* partitionName = 
                  new TObjString(histogramService.partitionName().c_str());
                m_knownOnlinePartitionList->Add(partitionName);
              }
              if (m_verbosity >= pres::Verbose) 
                std::cout << "\t\t\t|_ " << candidateDimService << std::endl;
            } else {
              if ( ! isBatch() ) 
                new TGMsgBox(fClient->GetRoot(), this, "DIM Service name error",
                             Form("The DIM servicename\n%s\n does not appear "\
                                  "to follow the convention\nPlease use the "\
                                  "following format:\n"\
                                  "TYP/UTGID/Algorithmname/Histogramname\n "\
                                  "where the UTGID "\
                                  "normally has the following format:\n"\
                                  "partition_node_taskname_instance", 
                                  dimService),
                             kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
            }
            ++m_candidateDimServicesIt;
          }
        }
      } else {
        if (pres::s_withTree == tree) {
          m_histoSvcListTree->RenameItem(m_histoSvcListTree->GetFirstItem(), 
                                         "No DIM");
        }
        if (m_verbosity >= pres::Verbose) 
          std::cout << "Sorry, no DIM server found." << std::endl;
        
        if ( ! isBatch() ) 
          new TGMsgBox(fClient->GetRoot(), this, "DIM Error",
                       "Sorry, no DIM server found", kMBIconExclamation,
                       kMBOk, &m_msgBoxReturnCode);
      }
    } else if (pres::EditorOffline == presenterMode()) {
      if (pres::s_withTree == tree && !m_savesetFileName.empty()) {

        m_dimBrowserDock->SetWindowName("ROOT Histogram Browser");
        m_histoSvcBrowserGroupFrame->SetTitle("ROOT Histogram Browser");

        TGListTreeItem* node = m_histoSvcListTree->GetFirstItem();
        m_histoSvcListTree->RenameItem(node, m_savesetFileName.c_str());

        (m_histoSvcListTree->GetFirstItem())->SetPictures(m_iconROOT, 
                                                          m_iconROOT);

        deleteTreeChildrenItemsUserData(node);
        m_histoSvcListTree->DeleteChildren(node);
        
        TFile rootFile(m_savesetFileName.c_str());

        if (rootFile.IsZombie() && ( ! isBatch() ) ) 
          new TGMsgBox(fClient->GetRoot(), this, "File Error",
                       "Sorry, file specified not found", kMBIconExclamation,
                       kMBOk, &m_msgBoxReturnCode);
        else {
          std::string 
            taskName(m_archive->
                     taskNameFromFile(std::string(rootFile.GetName())));
          if (taskName.empty()) {
            m_message = 
              "ERROR: could not get guess the taskname from your saveset name";
            std::cout << m_message << ". Offline Editor cannot work"<<std::endl;
            m_mainStatusBar->SetText(m_message.c_str(), 2);
          }
          else {
            pres::SavesetType savesetType = pres::OfflineFile;
            TObjArray* fileNameMatchGroup = 0;

            fileNameMatchGroup = 
              pres::s_fileDateRegexp.MatchS(rootFile.GetName());
            if ( ! fileNameMatchGroup->IsEmpty()) {
              savesetType = pres::OnlineFile;
              if (fileNameMatchGroup) {
                fileNameMatchGroup->Delete();
                delete fileNameMatchGroup;
                fileNameMatchGroup = 0;
              }
            } else {
              if (fileNameMatchGroup) {
                fileNameMatchGroup->Delete();
                delete fileNameMatchGroup;
                fileNameMatchGroup = 0;
              }
              fileNameMatchGroup = 
                pres::s_offlineJobRegexp.MatchS(rootFile.GetName());
              if ( ! fileNameMatchGroup->IsEmpty()) {
                savesetType = pres::OfflineFile;
                if (fileNameMatchGroup) {
                  fileNameMatchGroup->Delete();
                  delete fileNameMatchGroup;
                  fileNameMatchGroup = 0;
                }
              } else {
                if (fileNameMatchGroup) {
                  fileNameMatchGroup->Delete();
                  delete fileNameMatchGroup;
                  fileNameMatchGroup = 0;
                }
                fileNameMatchGroup = 
                  pres::s_fileRunRegexp.MatchS(rootFile.GetName());
                if (false == fileNameMatchGroup->IsEmpty()) {
                  savesetType = pres::OnlineFile;
                }
                if (fileNameMatchGroup) {
                  fileNameMatchGroup->Delete();
                  delete fileNameMatchGroup;
                  fileNameMatchGroup = 0;
                }
              }
            }
            listRootHistogramsFrom(dynamic_cast<TDirectory*>(&rootFile),
                                   m_knownHistogramServices, m_histogramTypes,
                                   taskName, savesetType);
          }
        }
        rootFile.Close();
      }
    }
    if (pres::s_withTree == tree) {
      fillTreeNodeWithHistograms(m_histoSvcListTree,
                                 m_histoSvcListTree->GetFirstItem(),
                                 &m_knownHistogramServices,
                                 &m_histogramTypes);
      sortTreeChildrenItems(m_histoSvcListTree, 
                            m_histoSvcListTree->GetFirstItem());
      fClient->NeedRedraw(m_histoSvcListTree);
    }
    Resize();DoRedraw();
    if (m_resumePageRefreshAfterLoading) { startPageRefresh(); }
    gVirtualX->SetCursor(GetId(), 
                         gClient->GetResourcePool()->GetDefaultCursor());
  }
}

//==============================================================================
// Selection partition name
//==============================================================================
void PresenterMainFrame::partitionSelectorComboBoxHandler(int partitionNumber) {
  if ( 1 == partitionNumber ) {
    m_partitionSelectorComboBox -> 
      RemoveEntries( 2 , m_partitionSelectorComboBox -> 
                     GetNumberOfEntries() + 1 ) ;
    bool updateList( false ) ;
    int foundLHCbPartitionId( 0 ) ;
    int id = 2 ;
    TIterator * partitionIt = 0;

    if ( ( ( pres::History == presenterMode()) || 
           ( pres::EditorOffline == presenterMode() ) )
         && ( 0 != m_archive ) ) {
      updateList = true;
      std::vector< std::string > m_historyPartitionList ;
      std::vector< std::string >::const_iterator m_historyPartitionListIt ;
      m_knownHistoryPartitionList -> Delete() ;

      m_historyPartitionList = m_archive -> listPartitions() ;

      for ( m_historyPartitionListIt = m_historyPartitionList.begin() ;
	    m_historyPartitionListIt != m_historyPartitionList.end() ; 
	    ++m_historyPartitionListIt ) {
        if (!m_knownHistoryPartitionList->
            FindObject((*m_historyPartitionListIt).c_str())) {
          TObjString* partitionName = 
            new TObjString((*m_historyPartitionListIt).c_str());
          m_knownHistoryPartitionList -> Add( partitionName ) ;
        }
      }
      partitionIt = m_knownHistoryPartitionList->MakeIterator();

    } else if ( ( pres::Online == presenterMode() ) || 
		( pres::EditorOnline == presenterMode() ) ) {
      updateList = true ;
      refreshHistogramSvcList( pres::s_withoutTree ) ;
      partitionIt = m_knownOnlinePartitionList -> MakeIterator() ;
    }

    if (updateList) {
 
      int idLimit( 550 ) ;   // directories to avoid deep nesting troubles
      TObject *obj = 0;
      while ( ( obj = partitionIt->Next() ) ) {
	m_partitionSelectorComboBox -> AddEntry( obj->GetName() , id ) ;
	if ( id < idLimit ) {
	  if ( 0 == pres::s_lhcbPartitionName.CompareTo( obj->GetName() ,
							 TString::kIgnoreCase ) )
	    foundLHCbPartitionId = id ;
	  id++;
	}
      }
    }
    
    m_partitionSelectorComboBox->SortByName();
    if (partitionIt) { delete partitionIt; partitionIt = 0; }
    if ( 0 != foundLHCbPartitionId ) {
      m_partitionSelectorComboBox -> Select( foundLHCbPartitionId , kTRUE ) ;
      if (m_verbosity >= pres::Verbose) 
        std::cout << "Found default LHCb partition: " 
		  << currentPartition() << std::endl;
    } else if (1 < id) {
      m_partitionSelectorComboBox -> Select( id , true ) ;
      if ( m_verbosity >= pres::Verbose )
        std::cout << "Defaulting to partition: " << currentPartition() 
                  << std::endl;
    }
  } else {
    std::string 
      partition_entry(dynamic_cast<TGTextLBEntry*>(m_partitionSelectorComboBox->
                                                   GetSelectedEntry())
                      ->GetText()->GetString() );
    setPartition( partition_entry ) ;
  }
  if (isConnectedToHistogramDB() && (false == m_currentPageName.empty()) && 
      (false == m_loadingPage)) 
    loadSelectedPageFromDB(m_currentPageName, pres::s_startupFile, 
                           m_savesetFileName);
}

//==============================================================================
// call back function for click on histo service tree item
//==============================================================================
void PresenterMainFrame::clickedHistoSvcTreeItem(TGListTreeItem* node, 
                                                 EMouseButton btn, int x, 
                                                 int y) {
  if (0 != node && btn == kButton3) {
    m_histoSvcTreeContextMenu->PlaceMenu(x, y, 1, 1);
  }
}

//==============================================================================
// call back function for click on histo db tree item
//==============================================================================
void PresenterMainFrame::clickedHistoDBTreeItem(TGListTreeItem* node,
                                                EMouseButton btn, int x, 
                                                int y) {
  if (0 != node && btn == kButton3) {
    m_histoDBContextMenu->PlaceMenu(x, y, 1, 1);
  }
}

//==============================================================================
// call back function when page tree clicked
//==============================================================================
void PresenterMainFrame::clickedPageTreeItem( TGListTreeItem* node,
                                              EMouseButton btn , int x , 
                                              int y ) {
  if ( ( 0 != node ) && ( kButton3 == btn ) ) 
    m_pagesContextMenu -> PlaceMenu( x , y , 1 , 1 ) ;
  else if ( ( kButton1 == btn ) && ( 0 != node ) && 
	    ( 0 == node->GetFirstChild()) &&
	    ( (pres::EditorOnline != presenterMode() ) || 
	      (pres::EditorOffline != presenterMode() ) ) ) {
    m_currentPageName = selectedPageFromDbTree();
    if ( !m_currentPageName.empty() && ( ! m_loadingPage ) ) {
      fillGroupPages( node ) ;
      loadSelectedPageFromDB( m_currentPageName, pres::s_startupFile , 
			      m_savesetFileName ) ;
      
      //== Store the siblings to have group navigation by prev/next pages
      m_alarmPages.clear();
    }
  }
}

//==============================================================================
// Fill vector of group pages for page navigation
//==============================================================================
void PresenterMainFrame::fillGroupPages( TGListTreeItem * node ) {
  m_groupPages.clear();
  TGListTreeItem* temp = node;
  while ( 0 != temp -> GetPrevSibling() ) temp = temp -> GetPrevSibling() ;
  while ( 0 != temp ) {
    //== pages without children only!
    if ( 0 == temp->GetFirstChild() ) {  
      TString newPageName = 
	(*static_cast< TObjString* >( temp->GetUserData() )).GetString();
      if (newPageName.BeginsWith( pres::s_FILE_URI ) )
	newPageName.Remove( 0 , pres::s_FILE_URI.length( ) ) ;
      std::string name( newPageName );
      m_groupPages.push_back( name );
    }
    temp = temp->GetNextSibling();
  }

  //== get iterator to point to the current page
  for ( m_groupPagesIt = m_groupPages.begin() ; 
	m_groupPagesIt != m_groupPages.end() ; ++m_groupPagesIt )
    if ( *m_groupPagesIt == m_currentPageName ) break ;
}

//==============================================================================
// call back function when alarm tree clicked
//==============================================================================
void PresenterMainFrame::clickedAlarmTreeItem(TGListTreeItem* node,
                                              EMouseButton btn, int x , 
                                              int y ) {

  if (0 != node && btn == kButton3) {
    m_alarmDBContextMenu->PlaceMenu(x, y, 1, 1);
  } else if (0 != node &&
             NULL == node->GetFirstChild() &&
             isConnectedToHistogramDB()) {
    int id;
    
    if (0 != node && node->GetUserData() &&
        m_alarmHistogramTreeList->GetFirstItem() != node) {
      id= *(int*)(node->GetUserData());
    } else {
      id= -1;
    }

    if (id !=-1) {
      m_alarmDisplay->loadSelectedAlarmFromDB(id);
      m_groupPages.clear(); //== No navigation there...
      m_alarmPages.clear();

      TGListTreeItem* temp = node;
      while ( 0 != temp->GetPrevSibling() ) temp = temp->GetPrevSibling();
      while ( 0 != temp ) {
        int id = *(int*)(node->GetUserData());
        m_alarmPages.push_back( id );
        temp = temp->GetNextSibling();
      }
      //== get iterator to point to the current page
      m_alarmPagesIt = m_alarmPages.begin();
      while ( m_alarmPagesIt != m_alarmPages.end() &&
              *m_alarmPagesIt != id ) {
        m_alarmPagesIt++;
      }
    }
  }
}

void PresenterMainFrame::addHistoToHistoDB() {
  disableAutoCanvasLayoutBtn();
  disableHistogramClearing();

  TGListTree* list = new TGListTree();
  checkedTreeItems(list, m_histoSvcListTree);

  TGListTreeItem* currentNode;
  currentNode = list->GetFirstItem();
  while (0 != currentNode) {
    try {
      if (0 != m_histogramDB) {
        TString histoName = 
          (*static_cast<TObjString*>(currentNode->GetUserData())).GetString();
        if (histoName.BeginsWith( pres::s_FILE_URI ) ) 
	  histoName.Remove(0, pres::s_FILE_URI.length( ) ) ; 
        if (m_verbosity >= pres::Verbose)
          std::cout<<"presenter is declaring "<<std::string(histoName)
                   <<std::endl;
        
        HistogramIdentifier histogramService = 
          HistogramIdentifier(std::string(histoName));

        if(histogramService.isPlausible()) {
          if (m_verbosity >= pres::Verbose) 
            std::cout << "declaring histogram to DB: " << std::endl 
                      << "   taskName= " << histogramService.taskName() 
                      <<std::endl 
                      << "   algorithmName= "<< histogramService.algorithmName() 
                      <<std::endl
                      << "   histogramName= " 
                      << histogramService.histogramFullName() << std::endl;
	  
          m_histogramDB->declareHistogram(histogramService.taskName(),
                                          histogramService.algorithmName(),
                                          histogramService.histogramFullName(),
                                          (OnlineHistDBEnv::HistType) 
                                          histogramService.dbHistogramType());
        }
        else {
          std::cout<< "ERROR: input histogram identifier is not correct: " 
                   << " taskName= "<< histogramService.taskName() 
                   << " algorithmName= "<< histogramService.algorithmName() 
                   << " histogramName= " << histogramService.histogramFullName() 
                   << std::endl;
        }
      }
    } catch (std::string sqlException) {
      if (m_verbosity >= pres::Verbose) std::cout << sqlException;
      if ( ! isBatch() ) 
        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("OnlineHistDB server:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
    currentNode = currentNode->GetNextSibling();
  }
  try {
    if (0 != m_histogramDB) {

      m_histogramDB->commit();
      refreshHistoDBListTree();
    }
  } catch (std::string sqlException) {
    if (m_verbosity >= pres::Verbose) { std::cout << sqlException; }
    if ( ! isBatch() ) 
      new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                   Form("OnlineHistDB server:\n\n%s\n",
                        sqlException.c_str()),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }
  list->Delete();
  delete list;
  list = NULL;

  m_histoSvcListTree->CheckAllChildren(m_histoSvcListTree->GetFirstItem(),
                                       pres::s_uncheckTreeItems);
  enableAutoCanvasLayoutBtn();
}

void PresenterMainFrame::addHistoToPage( const std::string& histogramUrl,  
                                         pres::ServicePlotMode overlapMode){
  HistogramIdentifier histogramID = HistogramIdentifier(histogramUrl);
  int newHistoInstance = 0;
  // see if the histogram object exists already
  std::vector<DbRootHist*>::iterator inst_dbHistosOnPageIt;
  for (inst_dbHistosOnPageIt = dbHistosOnPage.begin();
       inst_dbHistosOnPageIt != dbHistosOnPage.end();
       inst_dbHistosOnPageIt++) {
    int existingHistoInstance = (*inst_dbHistosOnPageIt)->instance();

    if (0 == (histogramID.histogramIdentifier()).
        compare((*inst_dbHistosOnPageIt)->identifier()) &&
        (newHistoInstance < existingHistoInstance) ) {
      newHistoInstance = existingHistoInstance;
      break;
    }
  }
  if (newHistoInstance > 998) { newHistoInstance = 0; }
  newHistoInstance++;
  OnlineHistDB*  histogramDB = NULL;
  OnlineHistogram* onlineHistogram = NULL;
  DimBrowser* dimBrowser = NULL;
  std::string theCurrentPartition( currentPartition() ) ;
  if (isConnectedToHistogramDB() &&
      ( pres::invisible != overlapMode) ){
    histogramDB = m_histogramDB;
    TString dimMon(histogramUrl);
    if (false == (dimMon.BeginsWith( pres::s_adder.c_str()) &&
                  dimMon.EndsWith(pres::s_eff_TCK.c_str()))) {
      onlineHistogram = m_histogramDB->getHistogram(dimMon.Data());
    }
  } else {
    histogramDB = NULL;
    onlineHistogram = NULL;
  }
  if (( pres::History == presenterMode()) || 
      (pres::EditorOffline == presenterMode())) 
    dimBrowser = NULL;
  else if ( ( pres::Online == presenterMode()) ||
	    ( pres::EditorOnline == presenterMode()) || ( isBatch() ) ) {
    dimBrowser = m_dimBrowser;
    if ( ! isConnectedToHistogramDB() ) theCurrentPartition = histogramUrl; 
  }
  DbRootHist* dbRootHist = Presenter::getPageHistogram( &m_presenterInfo ,
						        histogramUrl,
						        theCurrentPartition,
						        2, newHistoInstance,
						        histogramDB,
						        analysisLib(),
						        onlineHistogram,
						        m_verbosity,
						        dimBrowser);
  dbRootHist->setOverlapMode(overlapMode);
  if ( (0 != m_archive ) && ( ! m_savesetFileName.empty() ) &&
       ( ( pres::History == presenterMode()) || 
	 ( pres::EditorOffline == presenterMode() ) ) ) 
    m_archive->fillHistogram(dbRootHist, pres::s_startupFile, 
                             m_savesetFileName);
  
  if ( ( ! isConnectedToHistogramDB( ) ) && 
       ( pres::invisible != overlapMode) ) {
    // Set Properties
    TString paintDrawXLabel = bulkHistoOptions.m_xLabel;
    TString paintDrawYLabel = bulkHistoOptions.m_yLabel;
    int paintLineWidth = bulkHistoOptions.m_lineWidth;
    int paintFillColour = bulkHistoOptions.m_fillColour;
    int paintFillStyle = bulkHistoOptions.m_fillStyle;
    int paintLineStyle = bulkHistoOptions.m_lineStyle;
    int paintLineColour = bulkHistoOptions.m_lineColour;
    int paintStats = bulkHistoOptions.m_statsOption;
    if (dbRootHist && dbRootHist->hasRootHistogram()) {
      dbRootHist->getRootHistogram()->SetXTitle(paintDrawXLabel.Data());
      dbRootHist->getRootHistogram()->SetYTitle(paintDrawYLabel.Data());
      dbRootHist->getRootHistogram()->SetLineWidth(paintLineWidth);
      dbRootHist->getRootHistogram()->SetFillColor(paintFillColour);
      dbRootHist->getRootHistogram()->SetFillStyle(paintFillStyle);
      dbRootHist->getRootHistogram()->SetLineStyle(paintLineStyle);
      dbRootHist->getRootHistogram()->SetLineColor(paintLineColour);
      dbRootHist->getRootHistogram()->SetStats(paintStats);

      TString paintDrawOption;
      if ( pres::s_H1D == dbRootHist->histogramType()) {
        m_drawOption = bulkHistoOptions.m_1DRootDrawOption;
        paintDrawOption = 
          TString(m_drawOption + TString(" ") +
                  bulkHistoOptions.m_genericRootDrawOption).Data();
      } else if ( ( pres::s_P1D == dbRootHist->histogramType()) ||
                  ( pres::s_HPD == dbRootHist->histogramType()) ) {
        m_drawOption = bulkHistoOptions.m_1DRootDrawOption;
        paintDrawOption = TString(m_drawOption + TString(" ") +
                                  bulkHistoOptions.m_genericRootDrawOption + 
                                  TString(" E ")).Data();
      } else if ( pres::s_H2D == dbRootHist->histogramType()) {
        m_drawOption = bulkHistoOptions.m_2DRootDrawOption;
        paintDrawOption = 
          TString(m_drawOption + TString(" ") +
                  bulkHistoOptions.m_genericRootDrawOption).Data();
      }
      dbRootHist->getRootHistogram()->SetOption(paintDrawOption.Data());
    }
  }
  TPad* targetPad = NULL ;
  DbRootHist* prevDbRootHist = NULL;
  if ( ( pres::overlap == overlapMode ) && ( ! dbHistosOnPage.empty()) &&
       ( isConnectedToHistogramDB()) ) {
    std::vector<DbRootHist*>::iterator pad_dbHistosOnPageIt;
    pad_dbHistosOnPageIt = dbHistosOnPage.end();
    pad_dbHistosOnPageIt--;
    prevDbRootHist = *pad_dbHistosOnPageIt;
    if ( (0 != prevDbRootHist) && (0 != prevDbRootHist->getHostingPad()) ) { 
      targetPad = prevDbRootHist->getHostingPad();
      dbRootHist->setHostingPad( targetPad ) ;
    }
  }
  dbHistosOnPage.push_back(dbRootHist);
  if ( ( ! isBatch() ) && ( pres::invisible != overlapMode) ) 
    paintHist(dbRootHist, targetPad);
  else if ( ( pres::invisible != overlapMode) && ( isBatch() ) ) {
    dbRootHist->getRootHistogram()->SetLineStyle(1);
    dbRootHist->getRootHistogram()->SetLineWidth(1);
  }
}

//==============================================================================
// Add dim histogram to page
//==============================================================================
void PresenterMainFrame::addDimHistosToPage() {
  disableAutoCanvasLayoutBtn();
  disableHistogramClearing();

  stopPageRefresh();
  unclearHistosIfNeeded();
  if (m_referencesOverlayed) { toggleReferenceOverlay(); }

  fClient->
    WaitFor(dynamic_cast<TGWindow*>( new HistoPropDialog(this, 646, 435, 
                                                         m_verbosity)));

  TGListTree* list = new TGListTree();
  checkedTreeItems(list, m_histoSvcListTree);

  TGListTreeItem* currentNode;
  currentNode = list->GetFirstItem();

  while (0 != currentNode) {
    TString 
      histoID((*static_cast<TObjString*>
               (currentNode->GetUserData())).GetString());
    if (histoID.BeginsWith( pres::s_FILE_URI ) ) 
      histoID.Remove(0, pres::s_FILE_URI.length( ) ) ;
    addHistoToPage(std::string(histoID), pres::separate);
    currentNode = currentNode->GetNextSibling();
  }
  list->Delete();
  delete list;
  list = NULL;

  m_histoSvcListTree->CheckAllChildren(m_histoSvcListTree->GetFirstItem(),
                                       pres::s_uncheckTreeItems);
  editorCanvas->Update();
  enableAutoCanvasLayoutBtn();
}

//==============================================================================
// Add db histogram to the page
//==============================================================================
void PresenterMainFrame::addDbHistoToPage(pres::ServicePlotMode overlapMode) {
  disableAutoCanvasLayoutBtn();
  stopPageRefresh();
  try {
    if (0 != m_histogramDB) {
      TGListTree* list = new TGListTree();
      checkedTreeItems(list, m_databaseHistogramTreeList);
      TGListTreeItem* currentNode = list->GetFirstItem();
      if (0 != currentNode) {
        addHistoToPage(std::string((*static_cast<TObjString*>
                                    (currentNode->
                                     GetUserData())).GetString()), 
                       pres::separate);
        currentNode = currentNode->GetNextSibling();
        while (0 != currentNode) {
          addHistoToPage(std::string((*static_cast<TObjString*>
                                      (currentNode->
                                       GetUserData())).GetString()), 
                         overlapMode);
          currentNode = currentNode->GetNextSibling();
        }
      }
      list->Delete();
      delete list;
      list = NULL;
    }
  } catch (std::string sqlException) {
    if (m_verbosity >= pres::Verbose) std::cout << sqlException <<std::endl ;
    if ( ! isBatch() ) 
      new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                   Form("OnlineHistDB server:\n\n%s\n",
                        sqlException.c_str()),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }

  m_databaseHistogramTreeList->CheckAllChildren(m_databaseHistogramTreeList->
                                                GetFirstItem(),
                                                pres::s_uncheckTreeItems);
  editorCanvas->Update();
  enableAutoCanvasLayoutBtn();
}

//==============================================================================
// Collapse all children of dim tree
//==============================================================================
void PresenterMainFrame::dimCollapseAllChildren() {
  if ( ( 0 != m_histoSvcListTree ) && ( m_histoSvcListTree->GetSelected() ) ) {
    collapseTreeChildrenItems(m_histoSvcListTree,
                              m_histoSvcListTree->GetSelected());
    fClient->NeedRedraw(m_histoSvcListTree);
  }
}

//==============================================================================
// Collapse all children of histogram tree
//==============================================================================
void PresenterMainFrame::dbHistoCollapseAllChildren() {
  if ( ( 0 != m_databaseHistogramTreeList ) &&
       ( m_databaseHistogramTreeList -> GetSelected() ) ) {
    collapseTreeChildrenItems(m_databaseHistogramTreeList,
                              m_databaseHistogramTreeList->GetSelected());
    fClient->NeedRedraw(m_databaseHistogramTreeList);
  }
}

//==============================================================================
// convert dim to histo id
//==============================================================================
std::string PresenterMainFrame::convDimToHistoID(const std::string & 
                                                 dimSvcName) {
  HistogramIdentifier histogram = HistogramIdentifier(dimSvcName);
  if (histogram.isDimFormat()) return histogram.histogramIdentifier();
  
  if ( ! isBatch() ) 
    new TGMsgBox(fClient->GetRoot(), this, "DIM Service name error",
		 Form("The DIM servicename\n%s\n does not appear to follow the" \
		      "convention\nPlease use the following format:\n"	\
		      "HnD/UTGID/Algorithmname/Histogramname\n where the UTGID " \
		      "normally has the following format:\n node00101_taskname_1 " \
		      "or simply taskname, without \"_\"", dimSvcName.c_str()),
		 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  return "0" ;
}

//==============================================================================
// Toggle history plot
//==============================================================================
void PresenterMainFrame::toggleHistoryPlots() {
  if ( m_presenterInfo.isHistoryTrendPlotMode() ) {
    m_historyPlotsButton->SetState(kButtonUp);
    m_viewMenu->UnCheckEntry(HISTORY_PLOTS_COMMAND);
    m_presenterInfo.setHistoryTrendPlotMode( false ) ;
  } else {
    m_historyPlotsButton->SetState(kButtonDown);
    m_presenterInfo.setHistoryTrendPlotMode( true ) ;
    m_viewMenu->CheckEntry(HISTORY_PLOTS_COMMAND);
  }
}

//==============================================================================
// Toggle reference overlay
//==============================================================================
void PresenterMainFrame::toggleReferenceOverlay() {
  if (m_referencesOverlayed) {
    m_overlayReferenceHistoButton->SetState(kButtonUp);
    m_viewMenu->UnCheckEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
    m_referencesOverlayed = false;
    disableReferenceOverlay();
  } else {
    m_overlayReferenceHistoButton->SetState(kButtonDown);
    m_referencesOverlayed = true;
    m_viewMenu->CheckEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
    enableReferenceOverlay();
  }
}

//==============================================================================
// Toggle fast hit map draw mode
//==============================================================================
void PresenterMainFrame::toggleFastHitMapDraw() {
  if (m_fastHitMapDraw) {
    m_viewMenu->UnCheckEntry(FAST_HITMAP_DRAW_COMMAND);
    m_fastHitMapDraw = false;
  } else {
    m_fastHitMapDraw = true;
    m_viewMenu->CheckEntry(FAST_HITMAP_DRAW_COMMAND);
  }
}

//==============================================================================
// Enable alarm display
//==============================================================================
void PresenterMainFrame::enableAlarmDisplay(bool mode) {
  m_alarmDisplayEnabled = mode;
  if ( ! ( m_alarmDisplayEnabled ) && 
       ( m_viewMenu->IsEntryChecked(SHOW_ALARM_LIST_COMMAND) ) ) {
    m_leftMiscFrame->HideFrame(m_databaseAlarmsDock);
    m_viewMenu->UnCheckEntry(SHOW_ALARM_LIST_COMMAND);
    m_viewMenu->DisableEntry(SHOW_ALARM_LIST_COMMAND);
    delete m_alarmDisplay; m_alarmDisplay=NULL;
  }
}

//==============================================================================
// Toggle show alarm list
//==============================================================================
void PresenterMainFrame::toggleShowAlarmList() {
  if (m_viewMenu->IsEntryChecked(SHOW_ALARM_LIST_COMMAND)) {
    m_viewMenu->UnCheckEntry(SHOW_ALARM_LIST_COMMAND);
    m_leftMiscFrame->HideFrame(m_databaseAlarmsDock);
  } else {
    m_viewMenu->CheckEntry(SHOW_ALARM_LIST_COMMAND);
    m_leftMiscFrame->ShowFrame(m_databaseAlarmsDock);
  }
}

//==============================================================================
// Display window listing already known problems
//==============================================================================
void PresenterMainFrame::toggleShowKnownProblemList() {
  if (m_viewMenu->IsEntryChecked(SHOW_KNOWNPROBLEM_LIST_COMMAND)) {
    m_viewMenu->UnCheckEntry(SHOW_KNOWNPROBLEM_LIST_COMMAND);
    m_leftMiscFrame->HideFrame(m_knownProblemDock);
  } else {
    m_viewMenu->CheckEntry(SHOW_KNOWNPROBLEM_LIST_COMMAND);
    m_leftMiscFrame->ShowFrame(m_knownProblemDock);
    m_globalKnownProblemList -> retrieveListOfProblems( "" ) ;
  }
}

//==============================================================================
// Pick reference histogram
//==============================================================================
void PresenterMainFrame::pickReferenceHistogram() {
  DbRootHist* histogram = selectedDbRootHistogram();

  if (0 != histogram) {
    TVirtualPad * padsav = gPad;
    fClient->WaitFor(new ReferencePicker(this, histogram));
    padsav->cd();
  }
}

//==============================================================================
// Save selected histogram as reference
//==============================================================================
void PresenterMainFrame::saveSelectedHistogramAsReference() {
  DbRootHist* histogram = selectedDbRootHistogram();

  if ( ( 0 != histogram ) && ( 0 != m_archive ) && ( ! isBatch() ) ) {
    new TGMsgBox(fClient->GetRoot(), this, "Save as Reference Histogram",
                 "Are you sure to save selected histogram as a reference?",
                 kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
    switch (m_msgBoxReturnCode) {
    case kMBYes:
      m_archive -> saveAsReferenceHistogram( histogram ) ;
      break;
    default:
      break;
    }
  }
}

//==============================================================================
// set the histogram properties in the database
//==============================================================================
void PresenterMainFrame::setHistogramPropertiesInDB() {
  try {
    if (0 != m_histogramDB) {
      fClient -> WaitFor( dynamic_cast<TGWindow*>( new HistoPropDialog(this, 
								       646, 435, 
								       m_verbosity)));

      TGListTree* list = new TGListTree();
      checkedTreeItems(list, m_databaseHistogramTreeList);

      TGListTreeItem* currentNode;
      currentNode = list->GetFirstItem();
      while (0 != currentNode) {
        OnlineHistogram* onlineHistogramToSet = m_histogramDB->
          getHistogram(std::string((*static_cast<TObjString*>
                                    (currentNode->GetUserData())).GetString()));

        std::string descriptionField = bulkHistoOptions.m_description.Data();

        std::string paintDrawXLabel = bulkHistoOptions.
          m_xLabel.Data();
        std::string paintDrawYLabel = bulkHistoOptions.
          m_yLabel.Data();
        std::string drawPatternTextEntry = bulkHistoOptions.
          m_drawPatternTextEntry.Data();

        int paintLineWidth = bulkHistoOptions.m_lineWidth;
        int paintFillColour = bulkHistoOptions.m_fillColour;
        int paintFillStyle = bulkHistoOptions.m_fillStyle;
        int paintLineStyle = bulkHistoOptions.m_lineStyle;
        int paintLineColour = bulkHistoOptions.m_lineColour;
        int paintStats = bulkHistoOptions.m_statsOption;

        if ( pres::s_H1D == onlineHistogramToSet->hstype() ||
             pres::s_P1D == onlineHistogramToSet->hstype() ||
             s_pfixMonH1D == onlineHistogramToSet->hstype() ||
             s_pfixMonProfile == onlineHistogramToSet->hstype() ||
             pres::s_HPD == onlineHistogramToSet->hstype()) {
          m_drawOption = bulkHistoOptions.m_1DRootDrawOption;
        } else if ( pres::s_H2D == onlineHistogramToSet->hstype() ||
		    s_pfixMonH2F == onlineHistogramToSet->hstype() ||
		    s_pfixMonH2D == onlineHistogramToSet->hstype() ) {
          m_drawOption = bulkHistoOptions.m_2DRootDrawOption;
        }
        std::string paintDrawOption = 
          TString( m_drawOption + TString(" ") +
                   bulkHistoOptions.m_genericRootDrawOption).Data();

        onlineHistogramToSet->setDescr(descriptionField);

        onlineHistogramToSet->setDisplayOption("LABEL_X",
                                               &paintDrawXLabel);
        onlineHistogramToSet->setDisplayOption("LABEL_Y",
                                               &paintDrawYLabel);
        onlineHistogramToSet->setDisplayOption("FILLSTYLE",
                                               &paintFillStyle);
        onlineHistogramToSet->setDisplayOption("FILLCOLOR",
                                               &paintFillColour);
        onlineHistogramToSet->setDisplayOption("LINESTYLE",
                                               &paintLineStyle);
        onlineHistogramToSet->setDisplayOption("LINECOLOR",
                                               &paintLineColour);
        onlineHistogramToSet->setDisplayOption("LINEWIDTH",
                                               &paintLineWidth);
        onlineHistogramToSet->setDisplayOption("STATS",
                                               &paintStats);
        onlineHistogramToSet->setDisplayOption("DRAWOPTS",
                                               &paintDrawOption);

        onlineHistogramToSet->setDisplayOption("DRAWPATTERN",
                                               &drawPatternTextEntry);

        onlineHistogramToSet->saveDisplayOptions();

        currentNode = currentNode->GetNextSibling();
      }
      list->Delete();
      delete list;
      list = NULL;
      m_histogramDB->commit();
    }
  } catch (std::string sqlException) {
    if (m_verbosity >= pres::Verbose) { std::cout << sqlException; }
    if ( ! isBatch() ) 
      new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                   Form("OnlineHistDB server:\n\n%s\n",
                        sqlException.c_str()),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }
}

//==============================================================================
// Delete the selected histogram from database
//==============================================================================
void PresenterMainFrame::deleteSelectedHistoFromDB() {
  if ( ! isBatch() ) {
    new TGMsgBox(fClient->GetRoot(), this, "Delete histogram",
                 "Are you sure to delete selected histogram from the database?",
                 kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
    switch (m_msgBoxReturnCode) {
    case kMBNo:
      return;
    }
  }
  try {
    if (0 != m_histogramDB) {
      TGListTree* list = new TGListTree();
      checkedTreeItems(list, m_databaseHistogramTreeList);

      TGListTreeItem* currentNode;
      currentNode = list->GetFirstItem();
      while (0 != currentNode) {
        std::string 
          hist_id((*static_cast<TObjString*>(currentNode->
                                             GetUserData())).GetString());
        OnlineHistogram* histoToDelete = m_histogramDB->getHistogram(hist_id);
        if (histoToDelete) {
          // remove this histogram from current page if present
          std::vector<DbRootHist*>::iterator delHist_dbHistosOnPageIt;
          delHist_dbHistosOnPageIt = dbHistosOnPage.begin();
          while (delHist_dbHistosOnPageIt != dbHistosOnPage.end() ) {
            if ( (*delHist_dbHistosOnPageIt)->identifier() == hist_id ) {
              delete *delHist_dbHistosOnPageIt;
              *delHist_dbHistosOnPageIt = NULL;
              delHist_dbHistosOnPageIt = 
                dbHistosOnPage.erase(delHist_dbHistosOnPageIt);
            }
            else {
              delHist_dbHistosOnPageIt++;
            }
          }
          editorCanvas->Update();
          // remove from DB
          m_histogramDB->removeHistogram(histoToDelete, false);
        }
        currentNode = currentNode->GetNextSibling();
      }
      list->Delete();
      delete list;
      list = NULL;
      m_histogramDB->commit();
    }
  } catch (std::string sqlException) {
    if (m_verbosity >= pres::Verbose) { std::cout << sqlException; }
    if ( ! isBatch() ) 
      new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                   Form("OnlineHistDB server:\n\n%s\n",
                        sqlException.c_str()),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }
  refreshHistoDBListTree();
}

//==============================================================================
// Selected page from DB
//==============================================================================
std::string PresenterMainFrame::selectedPageFromDbTree(){
  TGListTreeItem* node = m_pagesFromHistoDBListTree->GetSelected();
  if (0 != node && node->GetUserData() &&
      m_pagesFromHistoDBListTree->GetFirstItem() != node) {
    TString 
      pageName = (*static_cast<TObjString*>(node->GetUserData())).GetString();
    if (pageName.BeginsWith( pres::s_FILE_URI ) ) 
      pageName.Remove(0, pres::s_FILE_URI.length( ) ) ;
    if ( ! pageName.IsNull()) return std::string(pageName);
    return std::string("");
  } 
  return std::string("");
}

//==============================================================================
// Load all pages
//==============================================================================
void PresenterMainFrame::loadAllPages() {
  if (isConnectedToHistogramDB()) {
    m_localDatabaseFolders.clear();
    m_histogramDB->getPageNames(m_localDatabaseFolders);

    for (m_folderIt = m_localDatabaseFolders.begin();
         m_folderIt != m_localDatabaseFolders.end(); ++m_folderIt) 
      loadSelectedPageFromDB(*m_folderIt, pres::s_startupFile, 
                             m_savesetFileName);
  }
}

//==============================================================================
// Load previous page -- handle click on "previous page" button
//==============================================================================
void PresenterMainFrame::loadPreviousPage() {
  if ((false == m_loadingPage) ) {
    if ( (false == m_groupPages.empty()) ) {

      if ( m_groupPagesIt == m_groupPages.begin() )  
        m_groupPagesIt = m_groupPages.end();
      m_groupPagesIt--;
      if ( false == (*m_groupPagesIt).empty()) {
        m_currentPageName = *m_groupPagesIt;
        openHistogramTreeAt( m_currentPageName ) ;
        loadSelectedPageFromDB(m_currentPageName, pres::s_startupFile, 
                               m_savesetFileName);
      }
    } else if ( (false == m_alarmPages.empty()) ) {
      if ( m_alarmPagesIt == m_alarmPages.begin() )  
        m_alarmPagesIt = m_alarmPages.end();
      m_alarmPagesIt--;
      if(m_alarmDisplay) {
        m_alarmDisplay->loadSelectedAlarmFromDB( *m_alarmPagesIt );
      }
    }
  }
}


//==============================================================================
// Load next page -- handle click on "next page" button
//==============================================================================
void PresenterMainFrame::loadNextPage() {
  if ( m_loadingPage ) return ;

  if ( ! m_groupPages.empty() ) {
    m_groupPagesIt++;
    if ( m_groupPagesIt == m_groupPages.end() ) 
      m_groupPagesIt = m_groupPages.begin();
    if ( ! (*m_groupPagesIt).empty() ) {
      m_currentPageName = *m_groupPagesIt;
      openHistogramTreeAt( m_currentPageName ) ;
      loadSelectedPageFromDB( m_currentPageName , pres::s_startupFile , 
                              m_savesetFileName ) ;
    }
  } else if ( ! m_alarmPages.empty() ) {
    m_alarmPagesIt++;
    if ( m_alarmPagesIt == m_alarmPages.end() )  
      m_alarmPagesIt = m_alarmPages.begin();
    if ( m_alarmDisplay ) 
      m_alarmDisplay->loadSelectedAlarmFromDB( *m_alarmPagesIt );
  }
}

//==============================================================================
// Load the page from the DB
//==============================================================================
void PresenterMainFrame::loadSelectedPageFromDB(const std::string & pageName,
                                                const std::string & timePoint,
                                                const std::string & 
                                                pastDuration ) {
  if ( isConnectedToHistogramDB() && ! m_loadingPage ) {
    TGListTreeItem * node = openHistogramTreeAt( pageName ) ;
    if ( m_groupPages.empty() ) fillGroupPages( node ) ;
    m_loadingPage = true;
    gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetWaitCursor());
    if (m_refreshingPage) {
      stopPageRefresh();
      m_resumePageRefreshAfterLoading = true;
    }
    unclearHistosIfNeeded();

    std::string currentTCK_service = "";

    disableAutoCanvasLayoutBtn();
    removeHistogramsFromPage();

    m_presenterInfo.setRwTimePoint( timePoint ) ;
    m_presenterInfo.setRwPastDuration( pastDuration ) ;

    std::string 
      history_entry( dynamic_cast<TGTextLBEntry*>(m_historyIntervalComboBox->
                                                  GetSelectedEntry())->
                     GetText()->GetString() );

    if ( ( 0 != m_archive ) &&
         ( ( pres::History == m_presenterInfo.presenterMode()) || 
           ( pres::EditorOffline == m_presenterInfo.presenterMode()))) {
      if ( "last 8 hours" == history_entry ) {
        m_presenterInfo.setRwTimePoint( pres::s_Now ) ;
        m_presenterInfo.setRwPastDuration( std::string("08:00:00") ) ;
        m_message = "History for last 8 hours";
      } else if ( "last 1 hour" == history_entry ) {
        m_presenterInfo.setRwTimePoint( pres::s_Now ) ;
        m_presenterInfo.setRwPastDuration( std::string("01:00:00") ) ;
        m_message = "History for last hour";
      } else if ( "preset file" == history_entry ) {
        m_presenterInfo.setRwTimePoint( pres::s_startupFile ) ;
        m_presenterInfo.setRwPastDuration( pastDuration ) ;
        m_message = m_savesetFileName;
      } else if ( "set file" == history_entry ) {
        m_presenterInfo.setRwTimePoint( pres::s_startupFile ) ;
        m_presenterInfo.setRwPastDuration( pastDuration ) ;
        m_message = m_savesetFileName;
      } else {
        m_presenterInfo.setRwTimePoint( m_presenterInfo.globalTimePoint() ) ;
        m_presenterInfo.setRwPastDuration(m_presenterInfo.globalPastDuration());  
        if ( m_presenterInfo.globalHistoryByRun() ) {
          if ( m_presenterInfo.globalPastDuration() == "0" ) 
            m_message = 
              Form( "History from run %d, start time: %s, duration: %s" ,
                    m_intervalPickerData->startRun() ,
                    m_runDb -> getCurrentStartTime().c_str() , 
                    m_runDb -> getCurrentRunDuration().c_str() ) ;
          else 
            m_message = Form("History from run %d to %d",
                             m_intervalPickerData->startRun(),
                             m_intervalPickerData->endRun());
        }
        else 
          m_message = Form("History from %s to %s",
                           m_intervalPickerData->getStartTimeString(),
                           m_intervalPickerData->getEndTimeString());
      }
      if (m_verbosity >= pres::Verbose)
        std::cout << m_message << std::endl 
		  << "Navigation step size " 
		  << m_presenterInfo.globalStepSize() << std::endl;
      m_mainStatusBar->SetText(m_message.c_str(), 2);
    }

    try {
      removeHistogramsFromPage();
      m_onlineHistosOnPage.clear();
      OnlineHistPage* page = m_histogramDB -> getPage( pageName ) ;
      if (m_verbosity >= pres::Verbose) 
        std::cout << "Loading page: "  << m_currentPageName << std::endl;

      if (m_verbosity >= pres::Debug) page->dump();

      if (0 != page) {
	// Display status bar and comments
	displayStatusAndComments( pageName , page ) ;

	// now load histograms
        page -> getHistogramList( &m_onlineHistosOnPage ) ;
        ParallelWait parallelWait ;
        parallelWait.loadHistograms( &m_onlineHistosOnPage , &dbHistosOnPage ,
				     &m_presenterInfo , dimBrowser() , 
				     histogramDB() , analysisLib() , 
				     verbosity() , canWriteToHistogramDB() , 
				     archive() );

        std::vector<DbRootHist*>::iterator drawHist_dbHistosOnPageIt;

        for (drawHist_dbHistosOnPageIt = dbHistosOnPage.begin();
             drawHist_dbHistosOnPageIt != dbHistosOnPage.end();
             ++drawHist_dbHistosOnPageIt) {

          if ( (m_verbosity >= pres::Verbose) &&
               (*drawHist_dbHistosOnPageIt) &&
               (*drawHist_dbHistosOnPageIt)->onlineHistogram() &&
               ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage() &&
               ( pres::TCKinfo != 
                 (*drawHist_dbHistosOnPageIt)->effServiceType()) ) 
            std::cout << "db identifier "
                      << ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->
              onpage()-> histo->identifier()
                      << std::endl << "\tdb task "
                      << ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->
              onpage()-> histo->task() << std::endl
                      << "\tdb dimServiceName "
                      << ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->
              onpage()->histo->dimServiceName()
                      << std::endl;
	  
          if ( (m_verbosity >= pres::Debug) &&
               ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage() &&
               (pres::TCKinfo!=(*drawHist_dbHistosOnPageIt)->effServiceType())) 
            ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->histo->
              dump();
          
          TPad* overlayOnPad = NULL;

          double xlow(0.0);
          double ylow(0.0);
          double xup(0.0);
          double yup(0.0);

          if ( (*drawHist_dbHistosOnPageIt) &&
               ((*drawHist_dbHistosOnPageIt)->onlineHistogram()) &&
               ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage() &&
               (pres::TCKinfo!=(*drawHist_dbHistosOnPageIt)->effServiceType())){
            xlow = 
              ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->xmin;
            ylow = 
              ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->ymin;
            xup  = 
              ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->xmax;
            yup  = 
              ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->ymax;

            // histogram must be overdrawn on previous hist: 
            // look for the corresponding TPad
            if (((*drawHist_dbHistosOnPageIt)->onlineHistogram())->
                onpage()->isOverlap()) {
              OnlineHistoOnPage 
                *mother = ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->
                onpage()->getOverlap();
              std::vector<DbRootHist*>::iterator prevh;
              DbRootHist* dbRootHist = NULL;
              for (prevh=dbHistosOnPage.begin() ; (*prevh) != dbRootHist ; 
                   ++prevh) {
                if ( (*prevh)->onlineHistogram() == mother->histo) {
                  overlayOnPad = (*prevh)->getHostingPad();
                  break;
                }
              }
            }
          }
          if (m_verbosity >= pres::Debug) {
            if (overlayOnPad) {
              std::cout << "drawing overlap on pad " << overlayOnPad 
                        <<std::endl;
            } else {
              std::cout << "drawing pad X "<< xlow << " to "<< xup <<
                "  Y " << ylow << " to " << yup <<std::endl;
            }
          }
          if ( (*drawHist_dbHistosOnPageIt) &&
               (pres::TCKinfo != (*drawHist_dbHistosOnPageIt)->
                effServiceType())) {
            (*drawHist_dbHistosOnPageIt)->draw(editorCanvas, xlow, ylow, xup, 
                                               yup, m_fastHitMapDraw, 
                                               overlayOnPad);
          }
        }

        // workaround attempt for ROOT painter objects:
        // reliable random crashes when below invoked... 
        // via TH painter (timing dependent?)
        editorCanvas->Update();
        std::vector<DbRootHist*>::iterator drawOpt_dbHistosOnPageIt;

        for (drawOpt_dbHistosOnPageIt = dbHistosOnPage.begin();
             drawOpt_dbHistosOnPageIt != dbHistosOnPage.end();
             ++drawOpt_dbHistosOnPageIt) {
          if ( pres::TCKinfo != (*drawOpt_dbHistosOnPageIt)->effServiceType()) 
            (*drawOpt_dbHistosOnPageIt)->
              setDrawOptionsFromDB((*drawOpt_dbHistosOnPageIt)
                                   ->getHostingPad());

          if ( ( pres::s_CNT == (*drawOpt_dbHistosOnPageIt)->histogramType()) &&
               ( currentTCK_service.empty()) ) {
            HistogramIdentifier histogramIdentifier = 
              HistogramIdentifier((*drawOpt_dbHistosOnPageIt)->identifier());
            if (histogramIdentifier.isPlausible()) {
              currentTCK_service = 
                pres::s_adder + histogramIdentifier.taskName() + 
                pres::s_slash + histogramIdentifier.algorithmName() + 
                pres::s_slash + pres::s_eff_MonitorSvc + pres::s_slash + 
                pres::s_eff_monRate + pres::s_slash + pres::s_eff_TCK;
            }
          }
        }

        if (false == currentTCK_service.empty()) 
	  addHistoToPage(currentTCK_service, pres::invisible);
        
        editorCanvas->Update();
      }

    } catch (std::string sqlException) {
      setStatusBarText(sqlException.c_str(), 2);
      if (m_verbosity >= pres::Verbose) std::cout << sqlException << std::endl;
      if ( ! isBatch() ) 
        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("Could not save the page to OnlineHistDB:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }

    m_loadingPage = false;
    if ( m_resumePageRefreshAfterLoading &&
	 ( pres::Online == m_presenterInfo.presenterMode())) startPageRefresh() ;
    
    gVirtualX->SetCursor(GetId(), 
                         gClient->GetResourcePool()->GetDefaultCursor());
  }
  if (m_referencesOverlayed) enableReferenceOverlay(); 
  else disableReferenceOverlay();
  
  if ((pres::EditorOnline == m_presenterInfo.presenterMode() ) || 
      (pres::EditorOffline == m_presenterInfo.presenterMode() ))
    enableAutoCanvasLayoutBtn();

  m_displayMode = pres::Page;
}

//==============================================================================
// Move in DB
//==============================================================================
void PresenterMainFrame::moveSelectedInDB() {
  TGListTreeItem *page = m_pagesFromHistoDBListTree->GetSelected();
  if (canWriteToHistogramDB()) {
    std::string selectedPage = selectedPageFromDbTree();
    char path[256]; 
    strncpy(path, selectedPage.c_str(), sizeof(path)-1);
    new TGInputDialog(gClient->GetRoot(), GetMainFrame(),
                      "Enter new name:",
                      path, path);

    while ( strcmp(path, selectedPage.c_str()) == 0 ) {
      new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error",
                   "Please enter a valid new name.",
                   kMBIconStop, kMBOk);
      new TGInputDialog(gClient->GetRoot(), GetMainFrame(),
                        "Enter new name:",
                        path, path);
    }

    if ( strcmp(path, "") == 0 ) { return; }
    try {
      std::string newPageName(path);
      if (m_verbosity >= pres::Verbose) 
        std::cout << "mv \"" << selectedPage << "\" \"" << newPageName << "\""
                  << std::endl;

      if (0 == page->GetFirstChild()) {
        OnlineHistPage* dbPage = m_histogramDB->getPage(selectedPage);
        dbPage->rename(newPageName);
        if (m_histogramDB->commit()) {
          refreshPagesDBListTree();
        }
      } else {
        if (pres::Batch != m_presenterInfo.presenterMode() ) {
          new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                       "Please select a leaf.",
                       kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
        }
      }
    } catch (std::string sqlException) {
      setStatusBarText(sqlException.c_str(), 2);
      if (m_verbosity >= pres::Verbose) std::cout << sqlException << std::endl;
      if (pres::Batch != m_presenterInfo.presenterMode() ) {
        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("Could not move item to OnlineHistDB:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }
    }
  }
}

//==============================================================================
// Delete the page from the DB
//==============================================================================
void PresenterMainFrame::deleteSelectedPageFromDB() {
  if (pres::Batch != m_presenterInfo.presenterMode() ) {
    new TGMsgBox(fClient->GetRoot(), this, "Delete",
                 "Are you sure to delete selected item from the database?",
                 kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
    switch (m_msgBoxReturnCode) {
    case kMBNo:
      return;
    }
  }

  if (canWriteToHistogramDB()) {
    std::string selectedItem = selectedPageFromDbTree();

    if (m_verbosity >= pres::Verbose) {
      std::cout << "Item to delete: " << selectedItem << std::endl;
    }

    try {
      TGListTreeItem *item = m_pagesFromHistoDBListTree->GetSelected();
      if (m_histogramDB->removePageFolder(selectedItem) &&
          m_histogramDB->commit()){
        m_pagesFromHistoDBListTree->DeleteItem(item);
      } else {
        OnlineHistPage* page = m_histogramDB->getPage(selectedItem);
        if (0 != page) {
          if (m_histogramDB->removePage(page) &&
              m_histogramDB->commit()){
            m_pagesFromHistoDBListTree->DeleteItem(item);
          }
        }
      }
    } catch (std::string sqlException) {
      setStatusBarText(sqlException.c_str(), 2);
      if (m_verbosity >= pres::Verbose) std::cout << sqlException << std::endl; 
      if (pres::Batch != m_presenterInfo.presenterMode() ) {
        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("Could not delete item to OnlineHistDB:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }
    }
  }
}

//==============================================================================
// Create the folder in DB
//==============================================================================
void PresenterMainFrame::createFolderInDB() {
  if (canWriteToHistogramDB()) {
    std::string selectedPage = selectedPageFromDbTree();
    selectedPage.append( pres::s_slash ) ;
    char path[256]; 
    strncpy(path, selectedPage.c_str(), sizeof(path)-1);
    new TGInputDialog(gClient->GetRoot(), GetMainFrame(),
                      "Enter new name:",
                      path, path);

    while ( 0 == strcmp(path, selectedPage.c_str()) ) {
      new TGMsgBox(gClient->GetRoot(), GetMainFrame(), "Error",
                   "Please enter a valid new name.",
                   kMBIconStop, kMBOk);
      new TGInputDialog(gClient->GetRoot(), GetMainFrame(),
                        "Enter new name:",
                        path, path);
    }
    if ( 0 == strcmp(path, "") ) { return; }

    try {
      std::string newPageName(path);
      if (m_verbosity >= pres::Verbose) {
        std::cout << "mkdir -p \"" << newPageName << "\""<< std::endl;
      }
      std::string staticRandom("/GP9m3CYZhqNL");
      newPageName.append(staticRandom); //just a tmp name
      OnlineHistPage* tmpPage = m_histogramDB->getPage(newPageName);
      tmpPage->save();
      m_histogramDB->removePage(tmpPage);
      if (m_histogramDB->commit()) {
        refreshPagesDBListTree();
      }
    } catch (std::string sqlException) {
      setStatusBarText(sqlException.c_str(), 2);
      if (m_verbosity >= pres::Verbose) std::cout << sqlException << std::endl; 
      if (pres::Batch != m_presenterInfo.presenterMode() ) {
        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("Could not create item in OnlineHistDB:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }
    }
  }
}

//==============================================================================
// Paint the histogram
//==============================================================================
void PresenterMainFrame::paintHist(DbRootHist* histogram, TPad* overlayOnPad) {
  //  xlow [0, 1] is the position of the bottom left point of the pad
  //             expressed in the mother pad reference system
  //  ylow [0, 1] is the Y position of this point.
  //  xup  [0, 1] is the x position of the top right point of the pad
  //             expressed in the mother pad reference system
  //  yup  [0, 1] is the Y position of this point.
  double xlow = 0.1 + 0.01*m_histoPadOffset;
  double ylow = xlow;
  double xup = 0.5 + 0.01*m_histoPadOffset;
  double yup = xup;

  if (NULL == overlayOnPad) {
    if (0.5 == xlow) { m_histoPadOffset = 0; }
    m_histoPadOffset++;
  }
  histogram->draw(editorCanvas, xlow, ylow, xup, yup, m_fastHitMapDraw, 
                  overlayOnPad);
}

//==============================================================================
// Auto canvas layout
//==============================================================================
void PresenterMainFrame::autoCanvasLayout() {

  int dbHistosOnPageSize(0);
  std::vector<DbRootHist*>::iterator layout_dbHistosOnPageIt;
  std::vector<DbRootHist*>::iterator layout_dbHistosOnPageIt2;
  layout_dbHistosOnPageIt = dbHistosOnPage.begin();
  while (layout_dbHistosOnPageIt != dbHistosOnPage.end()) {
    if (NULL != (*layout_dbHistosOnPageIt) &&
        false == (*layout_dbHistosOnPageIt)->isOverlap()) {
      dbHistosOnPageSize++;
    }
    layout_dbHistosOnPageIt++;
  }

  double xmargin = 0.01;
  double ymargin = 0.01;
  int nx = (int) ceil(sqrt((double)dbHistosOnPageSize));
  int ny = nx;

  if (nx <= 0) { nx = 1; }
  if (ny <= 0) { ny = 1; }
  int ix, iy;
  double x1, y1, x2, y2;
  double dx, dy;

  layout_dbHistosOnPageIt2 = dbHistosOnPage.begin();

  if (xmargin > 0 && ymargin > 0) {
    dy = 1/double(ny);
    dx = 1/double(nx);
    for (iy = 0; iy < ny; ++iy) {

      if (NULL != (*layout_dbHistosOnPageIt2) &&
          false == (*layout_dbHistosOnPageIt2)->isOverlap()) {
        if (dbHistosOnPage.end() == layout_dbHistosOnPageIt2)  { break; }
        y2 = 1 - iy*dy - ymargin;
        y1 = y2 - dy + 2*ymargin;
        if (y1 < 0) { y1 = 0; }
        if (y1 > y2) { continue; }
        for (ix = 0; ix < nx; ++ix) {
          if (dbHistosOnPage.end() == layout_dbHistosOnPageIt2)  { break; }
          x1 = ix*dx + xmargin;
          x2 = x1 + dx - 2*xmargin;
          if (x1 > x2) { continue; }
          if (0 != (*layout_dbHistosOnPageIt2) &&
              0 != ((*layout_dbHistosOnPageIt2)->getHostingPad())) {
            ((*layout_dbHistosOnPageIt2)->getHostingPad())->SetPad(x1, y1, x2, 
                                                                   y2);
            ((*layout_dbHistosOnPageIt2)->getHostingPad())->Modified();
          }
          layout_dbHistosOnPageIt2++;
        }
      }
    }
  }
  editorCanvas->Update();
}

//==============================================================================
// Remove the selected histogram from the page
//==============================================================================
void PresenterMainFrame::deleteSelectedHistoFromCanvas() {
  if (m_refreshingPage) {
    stopPageRefresh();
    m_resumePageRefreshAfterLoading = true;
  }
  bool referenceOverlay(false);
  if (m_referencesOverlayed) {
    disableReferenceOverlay();
    referenceOverlay = true;
  }
  unclearHistosIfNeeded();

  DbRootHist* histogram = selectedDbRootHistogram();
  DbRootHist * histoToDelete( 0 ) ;
  if (0 != histogram) {
    std::vector<DbRootHist*>::iterator del_dbHistosOnPageIt ;
    for ( del_dbHistosOnPageIt = dbHistosOnPage.begin() ;
	  del_dbHistosOnPageIt != dbHistosOnPage.end()  ;
	  ++del_dbHistosOnPageIt ) {
      if (0 == ( (*del_dbHistosOnPageIt) -> getName() ).
          compare( histogram -> getName() ) ) {
        histoToDelete = ( *del_dbHistosOnPageIt ) ;
        break;
      }
    }
    if ( ( 0 != histoToDelete ) && 
	 ( pres::Batch != m_presenterInfo.presenterMode() ) ) {
      new TGMsgBox( fClient->GetRoot() , this , "Remove Histogram" ,
		    "Are you sure to remove selected histogram from the page?",
		    kMBIconQuestion, kMBYes|kMBNo , &m_msgBoxReturnCode ) ;
      switch (m_msgBoxReturnCode) {
      case kMBYes:
        dbHistosOnPage.erase( del_dbHistosOnPageIt ) ;
	delete histoToDelete ;
        editorCanvas->Update();
        break;
      default:
        break;
      }
    }
  }
  if ( m_resumePageRefreshAfterLoading ) startPageRefresh() ; 
  if ( referenceOverlay ) enableReferenceOverlay() ; 
}

//==============================================================================
// Retrieve the histogram selected on the page
//==============================================================================
DbRootHist* PresenterMainFrame::selectedDbRootHistogram() {
  DbRootHist* dbRootHist = 0;

  bool referenceOverlay(false);
  if ( m_referencesOverlayed ) {
    disableReferenceOverlay() ;
    referenceOverlay = true;
  }

  TPad * thePad = dynamic_cast< TPad * >( editorCanvas -> Pad( ) ) ;
  if ( 0 != thePad ) {
    TIter nextPadElem( thePad -> GetListOfPrimitives() ) ;
    TObject * histoCandidate ( 0 ) ;
    TH1 * theHisto( 0 ) ;
    while ( ( histoCandidate = nextPadElem() ) ) {
      if ( histoCandidate -> InheritsFrom( TH1::Class() ) ) {
	theHisto = dynamic_cast< TH1 * >( histoCandidate ) ;
	break ;
      }
    }

    if ( 0 != theHisto ) {
      std::vector<DbRootHist*>::iterator selected_dbHistosOnPageIt;
      for ( selected_dbHistosOnPageIt = dbHistosOnPage.begin() ;
	    selected_dbHistosOnPageIt != dbHistosOnPage.end()  ;
	    ++selected_dbHistosOnPageIt) {
	if ( 0 == std::string( (*selected_dbHistosOnPageIt) -> 
			       getName() ).compare( theHisto -> GetName() ) ) {
	  dbRootHist = *selected_dbHistosOnPageIt;
	  break;
	}
      }
    }
  }
  
  if ( ( 0 == dbRootHist ) && 
       ( pres::Batch != m_presenterInfo.presenterMode() ) ) {
    new TGMsgBox(fClient->GetRoot(), this, "Histogram Selection",
		 "Please select a histogram with the middle mouse button",
		 kMBIconStop, kMBOk, &m_msgBoxReturnCode);
  }
  if (referenceOverlay) { enableReferenceOverlay(); }
  return dbRootHist;
}

//==============================================================================
// Refresh clock
//==============================================================================
void PresenterMainFrame::refreshClock() {
  if ( 0 != m_presenterInfo.currentTime() ) {
    m_presenterInfo.currentTime() -> Set() ;
    if ( pres::Batch != m_presenterInfo.presenterMode() ) {
      m_statusBarTop->SetText( m_presenterInfo.currentTime() -> AsSQLString(), 
                               2);
    }
  }
}

//==============================================================================
// Go to previous interval 
//==============================================================================
void PresenterMainFrame::previousInterval() {
  std::string 
    history_entry(dynamic_cast<TGTextLBEntry*>(m_historyIntervalComboBox->
                                               GetSelectedEntry())
                  ->GetText()->GetString());
  if ( ( pres::History == m_presenterInfo.presenterMode() ) && 
       ( pres::s_timeInterval == m_presenterInfo.historyMode() ) &&
       ( ("preset interval" == history_entry) || 
         ("set interval" == history_entry ) ) ) {
    m_presenterInfo.
      setGlobalTimePoint
      (m_archive->
       substractIsoTimeDate(m_presenterInfo.globalTimePoint(),
                            m_presenterInfo.globalStepSize()));

    boost::posix_time::ptime startTime = 
      boost::posix_time::from_iso_string( m_presenterInfo.globalTimePoint() ) ;
    boost::posix_time::time_duration duration = 
      boost::posix_time::duration_from_string(m_presenterInfo.globalStepSize());
    boost::posix_time::ptime endTime = startTime + duration ;

    m_intervalPickerData -> setStartTime( startTime.date().year(), 
                                          startTime.date().month() , 
                                          startTime.date().day() , 
                                          startTime.time_of_day().hours() , 
                                          startTime.time_of_day().minutes() , 
                                          startTime.time_of_day().seconds() ) ;

    m_intervalPickerData -> setEndTime( endTime.date().year(), 
                                        endTime.date().month() , 
                                        endTime.date().day() , 
                                        endTime.time_of_day().hours() , 
                                        endTime.time_of_day().minutes() , 
                                        endTime.time_of_day().seconds() ) ;
    
    if (!m_currentPageName.empty()) 
      loadSelectedPageFromDB(m_currentPageName, 
                             m_presenterInfo.globalTimePoint() , 
                             m_presenterInfo.globalPastDuration() ) ;
    
    if (m_verbosity >= pres::Verbose) 
      std::cout << "after previousInterval global_timePoint " 
		<< m_presenterInfo.globalTimePoint() << std::endl;

  } else if ( IntervalPickerData::SingleRun == 
              m_intervalPickerData -> getMode() ) {
    int previousRun = m_runDb -> getPreviousRun( ) ;
    if ( 0 == previousRun ) 
      new TGMsgBox( fClient -> GetRoot() , this , "Previous run" , 
		    "Already at first run" , kMBIconExclamation ) ;  
    else {
      std::ostringstream is ;
      is << previousRun ;
      m_presenterInfo.setGlobalTimePoint( is.str() ) ;
      m_presenterInfo.setGlobalPastDuration( "0" ) ;
      m_intervalPickerData -> setStartRun( previousRun ) ;
      m_intervalPickerData -> setEndRun  ( previousRun ) ;
      m_textNavigation     -> LoadBuffer ( Form( "%d" ,
						 m_runDb -> getCurrentRunNumber() ) ) ;
      if ( ! m_currentPageName.empty() ) 
	loadSelectedPageFromDB( m_currentPageName , m_presenterInfo.globalTimePoint() , 
				m_presenterInfo.globalPastDuration() ) ;
    }
  }
}

//==============================================================================
// go to next interval 
//==============================================================================
void PresenterMainFrame::nextInterval() {
  if ( pres::History != m_presenterInfo.presenterMode() ) return ;
  std::string 
    history_entry( dynamic_cast< TGTextLBEntry * >(m_historyIntervalComboBox->
						   GetSelectedEntry())->
		   GetText()->GetString());

  if( ( pres::s_timeInterval == m_presenterInfo.historyMode() ) && 
      ( ( "preset interval" == history_entry ) || 
        ( "set interval" == history_entry ) ) ) {
    m_presenterInfo.
      setGlobalTimePoint( m_archive -> 
                          addIsoTimeDate( m_presenterInfo.globalTimePoint() ,
                                          m_presenterInfo.globalStepSize() ) ) ;

    boost::posix_time::ptime startTime = 
      boost::posix_time::from_iso_string( m_presenterInfo.globalTimePoint() ) ;
    boost::posix_time::time_duration duration = 
      boost::posix_time::duration_from_string(m_presenterInfo.globalStepSize());
    boost::posix_time::ptime endTime = startTime + duration ;
    
    m_intervalPickerData -> setStartTime( startTime.date().year(), 
                                          startTime.date().month() , 
                                          startTime.date().day() , 
                                          startTime.time_of_day().hours() , 
                                          startTime.time_of_day().minutes() , 
                                          startTime.time_of_day().seconds() ) ;
    
    m_intervalPickerData -> setEndTime( endTime.date().year(), 
                                        endTime.date().month() , 
                                        endTime.date().day() , 
                                        endTime.time_of_day().hours() , 
                                        endTime.time_of_day().minutes() , 
                                        endTime.time_of_day().seconds() ) ;

    if ( ! m_currentPageName.empty() ) 
      loadSelectedPageFromDB( m_currentPageName , 
                              m_presenterInfo.globalTimePoint() , 
                              m_presenterInfo.globalPastDuration() ) ;
    
    if (m_verbosity >= pres::Verbose) 
      std::cout << "after nextInterval global_timePoint " 
                << m_presenterInfo.globalTimePoint() << std::endl;
  } else if ( IntervalPickerData::SingleRun == 
              m_intervalPickerData -> getMode() ) {
    int nextRun = m_runDb -> getNextRun( ) ;
    if ( 0 == nextRun ) 
      new TGMsgBox( fClient -> GetRoot() , this , "Next run" , 
		    "Already at last run" , kMBIconExclamation ) ;  
    else {
      std::ostringstream is ;
      is << nextRun ;
      m_presenterInfo.setGlobalTimePoint( is.str() ) ;
      m_presenterInfo.setGlobalPastDuration( "0" ) ;
      m_intervalPickerData -> setStartRun( nextRun ) ;
      m_intervalPickerData -> setEndRun  ( nextRun ) ;
      m_textNavigation     -> LoadBuffer ( Form( "%d" ,
						 m_runDb -> getCurrentRunNumber() ) ) ;
      if ( ! m_currentPageName.empty() ) 
	loadSelectedPageFromDB( m_currentPageName , m_presenterInfo.globalTimePoint() , 
				m_presenterInfo.globalPastDuration() ) ;
    }
  }
}

//==============================================================================
// Check if the page is thread safe
//==============================================================================
bool PresenterMainFrame::threadSafePage() {
  bool out=true;
  std::vector<DbRootHist*>::iterator pageHistoIt ;
  for ( pageHistoIt = dbHistosOnPage.begin() ; 
        pageHistoIt != dbHistosOnPage.end() ; ++pageHistoIt ) {
    if ( 0 == (*pageHistoIt) -> onlineHistogram() ) {
      out = false ;
      break ;
    }
    if ( (*pageHistoIt)->onlineHistogram()->hasFitFunction() ||
         (*pageHistoIt)->onlineHistogram()->isAnaHist() ) {
      out=false;
      break;
    }
  }
  return out;
}

//==============================================================================
// Refresh pages in refresh mode
//==============================================================================
void PresenterMainFrame::refreshPage() {
  bool parallelRefresh = threadSafePage();

  if (m_verbosity >= pres::Verbose) std::cout << "refreshing..." << std::endl;

  if ( ! isBatch() ) {
    if( parallelRefresh ) {
      if (m_verbosity >= pres::Debug) 
        std::cout << "refreshing histograms in parallel..." << std::endl;
      
      ParallelWait parallelWait ;
      parallelWait.refreshHistograms( &dbHistosOnPage , &m_presenterInfo ,
				      m_mainStatusBar ) ;

    }
    else {
      if (m_verbosity >= pres::Debug) 
        std::cout << "refreshing histograms one by one..." << std::endl;
      std::vector<DbRootHist*>::iterator dbHistosOnPageIt;
      for (dbHistosOnPageIt = dbHistosOnPage.begin() ; 
	   dbHistosOnPageIt != dbHistosOnPage.end() ; ++dbHistosOnPageIt ) {
        (*dbHistosOnPageIt)->fillHistogram();
        (*dbHistosOnPageIt)->normalizeReference();
      }
    }
    if (! gROOT->IsInterrupted()) editorCanvas->Update();
  }
  else {
    std::vector<DbRootHist*>::iterator dump_dbHistosOnPageIt;
    dump_dbHistosOnPageIt = dbHistosOnPage.begin();
    editorCanvas->cd();
    while (dump_dbHistosOnPageIt != dbHistosOnPage.end()) {
      if (false == (*dump_dbHistosOnPageIt)->isEmptyHisto()) {
        (*dump_dbHistosOnPageIt)->fillHistogram();
        (*dump_dbHistosOnPageIt)->normalizeReference();
      }
      
      std::string plotName( currentPartition() ) ;
      plotName.append(" ").append(m_presenterInfo.currentTime()->AsSQLString());
      (*dump_dbHistosOnPageIt)->getRootHistogram()->SetTitle(plotName.c_str());

      (*dump_dbHistosOnPageIt)->getRootHistogram()->Draw();
      (*dump_dbHistosOnPageIt)->fit();

      gPad->Modified();
      editorCanvas->Modified();
      editorCanvas->Update();
      std::string dumpFile = m_imagePath + pres::s_slash +
        (*dump_dbHistosOnPageIt)->fileName() + "." + m_dumpFormat;
      if (! gROOT->IsInterrupted()) editorCanvas->SaveAs(dumpFile.c_str());
      
      dump_dbHistosOnPageIt++;
    }
  }
}

//==============================================================================
// Enable histogram clearing
//==============================================================================
void PresenterMainFrame::enableHistogramClearing() {
  std::vector<DbRootHist*>::iterator clear_dbHistosOnPageIt;
  for ( clear_dbHistosOnPageIt = dbHistosOnPage.begin() ;
	clear_dbHistosOnPageIt != dbHistosOnPage.end() ; 
	++clear_dbHistosOnPageIt ) 
    (*clear_dbHistosOnPageIt)->enableClear();
}

//==============================================================================
// Disable histogram clearing
//==============================================================================
void PresenterMainFrame::disableHistogramClearing() {
  std::vector<DbRootHist*>::iterator clearDisable_dbHistosOnPageIt;
  for ( clearDisable_dbHistosOnPageIt = dbHistosOnPage.begin() ; 
	clearDisable_dbHistosOnPageIt != dbHistosOnPage.end() ;
	++clearDisable_dbHistosOnPageIt ) 
    (*clearDisable_dbHistosOnPageIt)->disableClear();
}

//==============================================================================
// Enable reference overlay
//==============================================================================
void PresenterMainFrame::enableReferenceOverlay() {
  bool stopped(false);
  if (m_refreshingPage) {
    stopPageRefresh();
    stopped = true;
  }
  std::vector<DbRootHist*>::iterator enableRef_dbHistosOnPageIt;
  for (enableRef_dbHistosOnPageIt = dbHistosOnPage.begin() ; 
       enableRef_dbHistosOnPageIt != dbHistosOnPage.end() ; 
       ++enableRef_dbHistosOnPageIt )
    (*enableRef_dbHistosOnPageIt)->referenceHistogram( DbRootHist::Show );
  
  editorCanvas->Update();
  if (stopped) { startPageRefresh(); }
}

//==============================================================================
// Disable reference overlay
//==============================================================================
void PresenterMainFrame::disableReferenceOverlay() {
  std::vector<DbRootHist*>::iterator disableRef_dbHistosOnPageIt;
  for ( disableRef_dbHistosOnPageIt = dbHistosOnPage.begin() ;
        disableRef_dbHistosOnPageIt != dbHistosOnPage.end() ;
        ++disableRef_dbHistosOnPageIt )
    (*disableRef_dbHistosOnPageIt)->referenceHistogram( DbRootHist::Hide );
  editorCanvas->Update();
}

//==============================================================================
// Enable the page refresh
//==============================================================================
void PresenterMainFrame::enablePageRefresh() {
  if (false == m_loadingPage) {
    if ( pres::Batch != m_presenterInfo.presenterMode() ) {
      editorCanvas->SetEditable(false);
    }
    std::vector<DbRootHist*>::iterator enableRefresh_dbHistosOnPageIt;
    for ( enableRefresh_dbHistosOnPageIt = dbHistosOnPage.begin() ;
	  enableRefresh_dbHistosOnPageIt != dbHistosOnPage.end() ;
	  ++enableRefresh_dbHistosOnPageIt ) 
      (*enableRefresh_dbHistosOnPageIt)->disableEdit();
  }
}

//==============================================================================
// Disable the page refresh
//==============================================================================
void PresenterMainFrame::disablePageRefresh() {
  m_refreshingPage = false;
  if ( ( ! isBatch() ) && ( 0 != editorCanvas ) ) 
    editorCanvas->SetEditable(true);
  
  std::vector<DbRootHist*>::iterator disableRefresh_dbHistosOnPageIt;
  for ( disableRefresh_dbHistosOnPageIt = dbHistosOnPage.begin() ;
	disableRefresh_dbHistosOnPageIt != dbHistosOnPage.end() ; 
	++disableRefresh_dbHistosOnPageIt )
    (*disableRefresh_dbHistosOnPageIt)->enableEdit();
}

//==============================================================================
// Remove the histograms from the page
//==============================================================================
void PresenterMainFrame::removeHistogramsFromPage() {

  disablePageRefresh();
  stopPageRefresh();
  editorCanvas->SetEditable(true);

  if (m_verbosity >= pres::Verbose) std::cout << "clearCanvas." << std::endl;
  std::vector<DbRootHist*>::iterator removeHistosFromPage_dbHistosOnPageIt;
  removeHistosFromPage_dbHistosOnPageIt = dbHistosOnPage.begin();
  while (removeHistosFromPage_dbHistosOnPageIt != dbHistosOnPage.end()) {
    if (*removeHistosFromPage_dbHistosOnPageIt) {
      delete *removeHistosFromPage_dbHistosOnPageIt;
      *removeHistosFromPage_dbHistosOnPageIt = NULL;
    }
    removeHistosFromPage_dbHistosOnPageIt++;
  }
  dbHistosOnPage.clear();
  m_histoPadOffset = 0;

  editorCanvas->cd();
  editorCanvas->Clear();
  editorCanvas->Update();
  m_displayMode = pres::None;
}

//==============================================================================
// Mouse actions on pads in the presenter
//==============================================================================
void PresenterMainFrame::EventInfo(int event, int px, int py, 
                                   TObject* selected) {
  switch (event) {

    // Left click goes to linked presenter page

  case kButton1Double:
    if ( 0 != selected ) { 
      TPad * thePad = 
	dynamic_cast< TPad *>( editorCanvas -> GetClickSelectedPad() ) ;
      if ( 0 != thePad ) {
	TIter next( thePad -> GetListOfPrimitives() ) ;
	TObject * obj ; TH1 * theHisto( 0 ) ; 
	while ( ( obj = next( ) ) ) {
	  if ( obj -> InheritsFrom( TH1::Class() ) ) {
	    theHisto = dynamic_cast< TH1* >( obj ) ;
	    break ;
	  }
	}
	
	if ( 0 != theHisto ) {
	  std::vector<DbRootHist*>::iterator eventInfo_dbHistosOnPageIt;
	  for (eventInfo_dbHistosOnPageIt = dbHistosOnPage.begin();
	       eventInfo_dbHistosOnPageIt != dbHistosOnPage.end(); 
	       ++eventInfo_dbHistosOnPageIt) {
	    if (0 == ( *eventInfo_dbHistosOnPageIt ) -> 
          getName().compare( theHisto -> GetName() ) ) {
	      if ( NULL != (*eventInfo_dbHistosOnPageIt)->onlineHistogram() &&
		   false == (*eventInfo_dbHistosOnPageIt)->onlineHistogram() -> 
		   page2display().empty( ) ) {
		if (m_verbosity >= pres::Verbose) {
		  std::cout << "loadPage: " 
                << ((*eventInfo_dbHistosOnPageIt)->
                    onlineHistogram())->page2display() << std::endl;
		}
		if ( ! ((*eventInfo_dbHistosOnPageIt) -> onlineHistogram()) 
		     -> page2display( ).empty() )  {
		  m_currentPageName = ((*eventInfo_dbHistosOnPageIt) -> onlineHistogram()) 
		    -> page2display( ) ;
		  openHistogramTreeAt( m_currentPageName ) ;
		  loadSelectedPageFromDB( m_currentPageName , pres::s_startupFile, 
					  m_savesetFileName ) ;
		}
		break;
	      }
	    }
	  }
	}
      }
    }
    break;

    // Right click opens a menu where we can select to open the
    // histogram documentation in a web browser

  case kButton3Down:
    if ( 0 != selected ) { 
      TPad * thePad = 
        dynamic_cast< TPad *>( editorCanvas -> GetClickSelectedPad() ) ;
      if ( 0 != thePad ) {
	TIter next( thePad -> GetListOfPrimitives() ) ;
	TObject * obj ; TH1 * theHisto( 0 ) ; TGraph * theGraph( 0 ) ;
	while ( ( obj = next( ) ) ) {
	  if ( obj -> InheritsFrom( TH1::Class() ) ) {
	    theHisto = dynamic_cast< TH1* >( obj ) ;
	    break ;
	  } else if ( obj -> InheritsFrom( TGraph::Class() ) ) {
	    theGraph = dynamic_cast< TGraph * >( obj ) ;
	    break ;
	  }
	}
	
	if ( 0 != theHisto ) {
	  std::vector<DbRootHist*>::iterator eventInfo_dbHistosOnPageIt;
	  for (eventInfo_dbHistosOnPageIt = dbHistosOnPage.begin() ;
	       eventInfo_dbHistosOnPageIt != dbHistosOnPage.end() ; 
         ++eventInfo_dbHistosOnPageIt) {
	    if (0 == ( *eventInfo_dbHistosOnPageIt ) -> 
          getName().compare( theHisto -> GetName() ) ) {
	      // Display a pop-up menu 
	      m_histomenu -> DeleteEntry( 1 ) ;
	      m_histomenu -> AddEntry( theHisto -> GetName() , 1 , 0 , 0 , 
				       m_histomenu -> GetEntry( 10 ) ) ;
	      
	      m_histomenu -> PlaceMenu( px , py , true , true ) ;
	      
	      m_weblink = (*eventInfo_dbHistosOnPageIt)->onlineHistogram()->doc() ;
	      if ( ! m_weblink.empty() ) {
		m_histomenu -> DeleteEntry( 2 ) ;
		m_histomenu -> DeleteEntry( 3 ) ;
		m_histomenu -> DeleteEntry( 4 ) ;
		m_histomenu -> DeleteEntry( 5 ) ;
		m_histomenu -> DeleteEntry( 6 ) ;
		m_histomenu -> DeleteEntry( 7 ) ;
		m_histomenu -> DeleteEntry( 8 ) ;
		m_histomenu -> DeleteEntry( 9 ) ;
		m_histomenu -> AddEntry( "Click for documentation" , 2 ) ;
		m_histomenu -> Connect( m_histomenu , "Activated(Int_t)",
					"PresenterMainFrame" , this, 
					"loadWebPage(Int_t)" );
	      } else {
		m_histomenu -> DeleteEntry( 2 ) ;
		m_histomenu -> DeleteEntry( 3 ) ;
		m_histomenu -> DeleteEntry( 4 ) ;
		m_histomenu -> DeleteEntry( 5 ) ;
		m_histomenu -> DeleteEntry( 6 ) ;
		m_histomenu -> DeleteEntry( 7 ) ;
		m_histomenu -> DeleteEntry( 8 ) ;
		m_histomenu -> DeleteEntry( 9 ) ;
		m_histomenu -> AddEntry( "-- no documentation available --" , 2 ) ;
	      }
	      break;
	    }
	  }
	} else if ( 0 != theGraph ) {
	  std::vector<DbRootHist*>::iterator eventInfo_dbHistosOnPageIt;
	  for (eventInfo_dbHistosOnPageIt = dbHistosOnPage.begin();
	       eventInfo_dbHistosOnPageIt != dbHistosOnPage.end(); 
	       ++eventInfo_dbHistosOnPageIt) {
	    if (0 == ( *eventInfo_dbHistosOnPageIt ) -> 
          getName().compare( theGraph -> GetName() ) ) {
	      m_trendingHisto = 
          dynamic_cast< TrendingHistogram * >( (*eventInfo_dbHistosOnPageIt) ) ;
	      // Display a pop-up menu 
	      m_histomenu -> DeleteEntry( 1 ) ;
	      m_histomenu -> AddEntry( theGraph -> GetName() , 1 , 0 , 0 , 
				       m_histomenu -> GetEntry( 10 ) ) ;
	      
	      m_histomenu -> PlaceMenu( px , py , true , true ) ;
	      
	      m_weblink = (*eventInfo_dbHistosOnPageIt)->onlineHistogram()->doc() ;
	      if ( ! m_weblink.empty() ) {
		m_histomenu -> DeleteEntry( 2 ) ;
		m_histomenu -> DeleteEntry( 3 ) ;
		m_histomenu -> DeleteEntry( 4 ) ;
		m_histomenu -> DeleteEntry( 5 ) ;
		m_histomenu -> DeleteEntry( 6 ) ;
		m_histomenu -> DeleteEntry( 7 ) ;
		m_histomenu -> DeleteEntry( 8 ) ;
		m_histomenu -> DeleteEntry( 9 ) ;		
		m_histomenu -> AddEntry( "Click for documentation" , 2 ) ;
	      } else {
		m_histomenu -> DeleteEntry( 2 ) ;
		m_histomenu -> DeleteEntry( 3 ) ;
		m_histomenu -> DeleteEntry( 4 ) ;
		m_histomenu -> DeleteEntry( 5 ) ;
		m_histomenu -> DeleteEntry( 6 ) ;
		m_histomenu -> DeleteEntry( 7 ) ;
		m_histomenu -> DeleteEntry( 8 ) ;
		m_histomenu -> DeleteEntry( 9 ) ;		
		m_histomenu -> AddEntry( "-- no documentation available --" , 2 ) ;
	      }
	      m_histomenu -> AddEntry( "Last 5 minutes" , 3 ) ;
	      m_histomenu -> AddEntry( "Last 30 minutes" , 4 ) ;
	      m_histomenu -> AddEntry( "Last 2 hours" , 5 ) ;
	      m_histomenu -> AddEntry( "Last day" , 6 ) ;
	      m_histomenu -> AddEntry( "Last week" , 7 ) ;
	      m_histomenu -> AddEntry( "Last year" , 8 ) ;
	      m_histomenu -> AddEntry( "Last 10 years" , 9 ) ;
	      m_histomenu -> Connect( m_histomenu , "Activated(Int_t)",
				      "PresenterMainFrame" , this, 
				      "loadWebPage(Int_t)" );
	      break;
	    }
	  }	  
	}
      }
    }
    break ;

    // Display coordinates 

  case kMouseMotion:
  case kMouseEnter:
  case kMouseLeave:
    if (NULL != selected) {
      const char* text0;
      const char* text1;
      const char* text3;
      text0 = selected->GetTitle();
      setStatusBarText(text0, 0);
      text1 = selected->GetName();
      setStatusBarText(text1, 1);
      text3 = selected->GetObjectInfo(px, py);
      m_statusBarTop->SetText(text3, 0);
    }
    break;
  }
}

//==============================================================================
// Print info about presenter
//==============================================================================
void PresenterMainFrame::about() {
  std::stringstream config;
  config << "Version: " << s_presenterVersion << std::endl << std::endl 
         << "Histogram Database version: " << OnlineHistDBEnv_constants::version 
         << std::endl 
         << "Histogram Database schema: " << OnlineHistDBEnv_constants::DBschema 
         << std::endl 
         << "Analysis Library version: " << OMAconstants::version << std::endl 
         << "Connected to Histogram Database: "
         << (isConnectedToHistogramDB() ? "yes" : "no") << std::endl;
  
  if (0 != m_histogramDB) {
    config << "Write access to Histogram Database: "
           << (canWriteToHistogramDB()? "yes" : "no") << std::endl 
           << "Reference histograms directory: " << m_referencePath << std::endl 
           << "Savesets directory: " << m_savesetPath << std::endl 
           << "DB Reference histograms directory: " 
           << dynamic_cast<OnlineHistDBEnv*>(m_histogramDB)->refRoot() 
           << std::endl 
           << "DB Savesets directory: "
           << dynamic_cast<OnlineHistDBEnv*>(m_histogramDB)->savesetsRoot() 
           << std::endl;
  }
  if ( ! isBatch() )
    new TGMsgBox(fClient->GetRoot(), this, "LHCb Presenter",
                 Form("%s", config.str().c_str()),
                 kMBIconAsterisk, kMBOk, &m_msgBoxReturnCode);
}

//==============================================================================
// Display status and comment of the loaded page
//==============================================================================
void PresenterMainFrame::displayStatusAndComments( const std::string & 
                                                   pageName , 
                                                   OnlineHistPage * page ) {
  if ( ! isBatch() )
    // UPdate the page name in the status bar.
    m_statusBarTop -> SetText( pageName.c_str(), 1 ) ;

  // Reload page in db in case of automatic changes (in the comments for 
  // example)
  page -> load() ;
  m_pageDescriptionView->Clear();
  
  // If this is a Shift page, prepend to the page description
  // the related list of problems from the ProblemDB
  if ( ( "/Shift" == page -> folder() ) || 
       ( "/OfflineDataQuality" == page -> folder() ) )
    m_pageDescriptionView -> retrieveListOfProblems( page -> name() ,
						     m_savesetFileName ) ;
  
  m_pageDescriptionView->LoadBuffer( page->doc().c_str());
  m_pageDescriptionView->DataChanged() ;
}

//==============================================================================
// Switch between interval navigation and by-run navigation for history mode
//==============================================================================
void PresenterMainFrame::switchToRunNavigation( bool ok ) {
  if ( ok ) {
    m_previousIntervalButton -> SetToolTipText( "Previous run" ) ;
    m_nextIntervalButton     -> SetToolTipText( "Next run" ) ;
    m_toolBarLabel           -> SetText       ( "Run: " ) ;
    m_textNavigation         -> Clear         ( ) ;
    m_textNavigation         -> LoadBuffer    ( Form( "%d" ,
						      m_runDb -> getCurrentRunNumber() ) ) ;
  } else {
    m_previousIntervalButton -> SetToolTipText( "Previous interval" ) ;
    m_nextIntervalButton     -> SetToolTipText( "Next interval" ) ;
    m_toolBarLabel           -> SetText       ( "Time: " ) ;
    m_textNavigation         -> Clear         ( ) ;
    m_textNavigation         -> LoadBuffer    ( "" ) ;
  }
}

//==============================================================================
// Open this page in histogram tree
// Format of the page is for example : /L0DU/BCId
// Name in the tree would be: /HISTDB/L0DU/BCId
// if pageName is empty, only open first level "HISTDB"
//==============================================================================
TGListTreeItem * PresenterMainFrame::openHistogramTreeAt( const std::string & 
                                                          pageName ) {
  TGListTreeItem * node = 0 ;

  std::string path = "/HISTDB" ;
  node = m_pagesFromHistoDBListTree -> FindItemByPathname( path.c_str() ) ;
  m_pagesFromHistoDBListTree -> OpenItem( node ) ;

  if ( "" != pageName ) {
    
    std::vector< std::string > splitVec;
    boost::algorithm::split( splitVec , pageName , 
			     boost::algorithm::is_any_of("/") ) ;
    
    std::vector< std::string >::iterator it ;
    for ( it = splitVec.begin() ; it != splitVec.end() ; ++it ) {
      if ( (*it) == "" ) continue ;
      path += "/" + (*it) ;
      if ( path == pageName ) continue ;
      node = m_pagesFromHistoDBListTree -> FindItemByPathname( path.c_str() ) ;
      m_pagesFromHistoDBListTree -> OpenItem( node ) ;
    }
   
    m_pagesFromHistoDBListTree -> ClearHighlighted() ;
 
    node = m_pagesFromHistoDBListTree -> 
      FindItemByPathname( ("/HISTDB" + pageName).c_str() ) ;
    m_pagesFromHistoDBListTree -> HighlightItem( node ) ;
  }  

  m_pagesFromHistoDBListTree -> Resize() ;
  DoRedraw() ;

  return node ;
}

//==============================================================================
// Open a web browser (firefox) at the given link
//==============================================================================
void PresenterMainFrame::loadWebPage( Int_t item ) {
  TrendingHistogram * theHisto = m_trendingHisto ;
  m_trendingHisto = 0 ;
  if ( 1 == item ) return ;
  if ( 2 == item ) { 
    if ( m_weblink.empty() ) return ;
#ifndef WIN32
    // check if an instance of firefox is already running
    bool running = false ;
    FILE * fp ;
    fp = popen( "pgrep -u `whoami` firefox" , "r" ) ;
    if ( fp == NULL ) 
      std::cerr << "Could not launch firefox" << std::endl ;
    else {
      char line[256] ;
      while ( fgets( line , sizeof(line) , fp ) ) { 
	running = true ;
      }
    }
    pclose( fp ) ;
    
    if ( ! running ) {
      if ( 0 == fork() ) {
	execlp( "firefox" , "firefox" , m_weblink.c_str() , NULL ) ;
      }
    } else
      if ( 0 == fork() ) {
	std::string foxargs( m_weblink ) ;
	foxargs = "openURL(" + foxargs + ")" ;
	execlp( "firefox" , "firefox" , "-remote" , foxargs.c_str()  
		, NULL ) ;
      }
  } else { 
    switch ( item ) {
    case 3:
      theHisto -> setMode( TrendingHistogram::Last5Minutes ) ;
      break ;
    case 4:
      theHisto -> setMode( TrendingHistogram::Last30Minutes ) ;
      break ;
    case 5:
      theHisto -> setMode( TrendingHistogram::Last2Hours ) ;
      break ;
    case 6:
      theHisto -> setMode( TrendingHistogram::LastDay ) ;
      break ;
    case 7:
      theHisto -> setMode( TrendingHistogram::LastWeek ) ;
      break ;
    case 8:
      theHisto -> setMode( TrendingHistogram::LastYear ) ;
      break ;
    case 9:
      theHisto -> setMode( TrendingHistogram::Last10Years ) ;
      break ;
    default: 
      break ;
    }
    theHisto -> refresh() ;
    if (! gROOT->IsInterrupted()) editorCanvas->Update();
#endif  // end: not _WIN32
  }
}

//==============================================================================
// Display a dialog box to add a trending histo to the database
//==============================================================================
void PresenterMainFrame::addTrendingHisto() {
  new CreateTrendingHistogramDialog( gClient->GetRoot() , GetMainFrame() ,
				     m_histogramDB ) ;
}

//==============================================================================
// Enable page undocking
//==============================================================================
void PresenterMainFrame::enablePageUndocking() { 
  m_viewMenu->EnableEntry(UNDOCK_PAGE_COMMAND) ; 
} 

//==============================================================================
// Disable page undocking
//==============================================================================
void PresenterMainFrame::disablePageUndocking() { 
  m_viewMenu->DisableEntry(UNDOCK_PAGE_COMMAND) ; 
} 

//==============================================================================
// Enable page loading
//==============================================================================
void PresenterMainFrame::enablePageLoading() { 
  m_pagesContextMenu->EnableEntry(M_LoadPage_COMMAND) ; 
} 


