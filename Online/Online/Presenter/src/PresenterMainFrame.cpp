#include <algorithm>
#include <iostream>
#include <vector>
#include <TApplication.h>
#include <TError.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TGIcon.h>
#include <TGResourcePool.h>
#include <TG3DLine.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGDockableFrame.h>
#include <TGDoubleSlider.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGListTree.h>
#include <TGListView.h>
#include <TGMenu.h>
#include <TGMsgBox.h>
#include <TGPicture.h>
#include <TGNumberEntry.h>
#include <TGSplitter.h>
#include <TGStatusBar.h>
#include <TGTab.h>
#include <TGTextEdit.h>
#include <TGTextView.h>
//#include <TGText.h>
#include <TGToolBar.h>
#include <TDatime.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TImage.h>
//#include <TList.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TObject.h>
#include <TBox.h>
#include <TLine.h>
#include <TText.h>
#include <TKey.h>
#include <TPad.h>
#include <TPRegexp.h>
#include <TRootEmbeddedCanvas.h>
#include <TString.h>
#include <TObjString.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TThread.h>
#include <TTimer.h>
#include <TGFileDialog.h>
#include <TRootHelpDialog.h>
#include <TBenchmark.h>


#include "dim/dic.hxx"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAlib.h"

#include "Gaucho/MonObject.h"

#include "DbRootHist.h"
#include "PresenterMainFrame.h"
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

using namespace pres;
using namespace std;
PresenterMainFrame* gPresenter = 0;

ClassImp(PresenterMainFrame)

PresenterMainFrame::PresenterMainFrame(const char* name,
                                       const std::string & savesetPath,
                                       const std::string & referencePath,
                                       const int & x,
                                       const int & y,
                                       const int & width,
                                       const int & height) :
  TGMainFrame(gClient->GetRoot(), width, height, kLHintsExpandX | kLHintsExpandY),
  currentTime(NULL),
  global_timePoint("20081126T160921"),
  global_pastDuration("00:05:00"),
  global_stepSize("00:15:00"),
  m_initWidth(width),
  m_initHeight(height),
  m_verbosity(Silent),
  m_logBookConfig(""),  
  m_historyMode(s_timeInterval),
  m_resumePageRefreshAfterLoading(false),
  m_loadingPage(false),
  m_currentPartition(s_lhcbPartionName.Data()),
  m_currentPageName(""),
//  m_currentMonitoringNode(""),
//  m_currentMonitoringInstance(""),
  m_referencePath(referencePath),
  m_savesetPath(savesetPath),
  m_imagePath(gSystem->TempDirectory()),
  m_savesetFileName(""),
  m_archive(NULL),
  m_intervalPicker(NULL),
  m_presenterMode(History),
  m_prevPresenterMode(History),  
  m_databaseMode(LoggedOut),
  m_pageRefreshTimer(NULL),
  m_clockTimer(NULL),
  m_clearedHistos(false),
  m_historyTrendPlots(false),
  m_referencesOverlayed(false),
  m_fastHitMapDraw(false),
  m_refreshingPage(false),
  m_histogramDB(NULL),
  m_analysisLib(NULL),
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
  m_currentTCK("")
{
  m_benchmark = new TBenchmark();
  SetCleanup(kDeepCleanup);
  
  // Thermo palette for hitmaps
  gStyle->SetPalette(1);
  
//std::cout << "PresenterMainFrame() gStyle->GetOptStat: " << gStyle->GetOptStat;
  gStyle->SetOptStat("emrou"); // nemr emrou

  gStyle->SetFrameFillColor(10);


  // only one presenter session allowed
  if (gPresenter) { return; }

  if (!currentTime) { currentTime = new TDatime(); }
  
  if (!m_pageRefreshTimer) { 
    m_pageRefreshTimer = new TTimer(s_pageRefreshRate);
  }
  m_pageRefreshTimer->TurnOff();
  m_pageRefreshTimer->Connect("Timeout()", "PresenterMainFrame",
                              this, "refreshPage()");

  if (!m_clockTimer) { m_clockTimer = new TTimer(1000); }
  m_clockTimer->TurnOn();
  m_clockTimer->Connect("Timeout()", "PresenterMainFrame",
                        this, "refreshClock()");

  SetWindowName(name);
  Move(x, y);
  gPresenter = this;

  m_knownHistogramServices.reserve(s_estimatedDimServiceCount);
  m_candidateDimServices.reserve(s_estimatedDimServiceCount);
  m_histogramTypes.reserve(s_estimatedDimServiceCount);
  dbHistosOnPage.reserve(s_estimatedHistosOnPage);

  m_knownOnlinePartitionList = new TList();
  m_knownHistoryPartitionList = new TList();

  if (m_verbosity >= Verbose) {
    std::cout << "refRoot: "  << m_referencePath << std::endl
              << "savesetRoot: "  << m_savesetPath << std::endl;
  }

  m_dimBrowser = new DimBrowser();
  char *dimFormat;
  char *dimService;
  m_dimBrowser->getServices(s_Dim_Dns_Version_Number.c_str());
  if (0 == m_dimBrowser->getNextService(dimService, dimFormat)) {
    m_message = "Sorry, no DIM server found.";
    if (m_verbosity >= Verbose) {
      std::cout << m_message << std::endl;
    }
//    m_mainStatusBar->SetText(m_message.c_str(), 2);
//  if (Batch != m_presenterMode) {
//      new TGMsgBox(fClient->GetRoot(), this, "DIM Error", m_message.c_str(),
//                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
//    }
  } else {
      refreshHistogramSvcList(s_withoutTree);
//    if (Batch != m_presenterMode) {      
//      m_mainStatusBar->SetText("Ok.");
//    }
  }

  m_archive = new Archive(this, m_savesetPath, m_referencePath);
  if (0 != m_archive) { m_archive->setVerbosity(m_verbosity); }  

}

PresenterMainFrame::~PresenterMainFrame()
{
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
  if (0 != m_histogramDB) { delete m_histogramDB; m_histogramDB = NULL; }
  if (0 != m_analysisLib) { delete m_analysisLib; m_analysisLib = NULL; }
  if (0 != m_dimBrowser) { delete m_dimBrowser; m_dimBrowser = NULL; }
  if (0 != m_archive) { delete m_archive; m_archive = NULL; }
  
  
  std::vector<std::string*>::iterator pageString;
  for (pageString = m_loadedPagesHistory.begin();pageString != m_loadedPagesHistory.end(); ++pageString) 
    delete *pageString; 
    
  std::vector<std::string*>::iterator dbString;
  for (dbString = m_knownDatabases.begin();dbString != m_knownDatabases.end(); ++dbString) 
    delete *dbString;

  std::map<std::string*, std::string*>::iterator dbCreds;
  for (dbCreds = m_knownDbCredentials.begin();dbCreds != m_knownDbCredentials.end(); ++dbCreds) {
    delete (*dbCreds).first;
    delete (*dbCreds).second;
  }
  
  if (0 != m_intervalPicker) {
    m_intervalPicker->UnmapWindow();
    m_intervalPicker->CloseWindow();
    delete m_intervalPicker; m_intervalPicker = NULL;
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
void PresenterMainFrame::buildGUI()
{
//  if (Batch == m_presenterMode &&
//      0 == editorCanvas) {
//    editorCanvas = new TCanvas("c", "c", GetWidth(), GetHeight());
//    editorCanvas->cd();
//    gStyle->SetOptStat("ne");
//  } else
  if ((EditorOnline == m_presenterMode) ||
         (EditorOffline == m_presenterMode) ||
         (Online == m_presenterMode) ||
         (History == m_presenterMode) ) {
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
      m_editPickReferenceHistogramText = new TGHotString("Set &Reference Histogram...");
      m_editSaveSelectedHistogramAsReferenceText = new TGHotString("&Save as Reference Histogram");
      m_editPagePropertiesText = new TGHotString("&Page Properties...");
  
    m_viewText = new TGHotString("&View");
      m_viewStartRefreshText = new TGHotString("Start page &refresh");
      m_viewStopRefreshText = new TGHotString("&Stop page refresh");
      m_viewToggleHistoryPlotsText = new TGHotString("&History plots");
      m_viewToggleReferenceOverlayText = new TGHotString("&Overlay reference");
      m_viewToggleFastHitMapDrawText = new TGHotString("&Fast hitmap draw");
      m_viewAlarmListText = new TGHotString("Show &alarm list");
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
    m_openedFolderIcon = picpool->GetPicture("folderOpen16",(char**)folderOpen16);
    m_closedFolderIcon = picpool->GetPicture("folder16",(char**)folder16);
  
    //gVirtualX CreatePictureFromData(Drawable_t id, char** data, Pixmap_t& pict, Pixmap_t& pict_mask, PictureAttributes_t& attr)
  
    // Histo alarms
    m_iconAlarm = picpool->GetPicture("mb_stop_s",(char**)mb_stop_s);
    m_iconWarning = picpool->GetPicture("mb_exclamation_s",(char**)mb_exclamation_s);
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
    
    m_stockNewFormula =  picpool->GetPicture("stock_new_formula_xpm", (char**)stock_new_formula_xpm);
  
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
  //m_fileMenu->DisableEntry(SAVE_PAGE_TO_FILE_COMMAND);
  
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
    m_editMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                        this, "handleCommand(Command)");
  m_editMenu->DisableEntry(PICK_REFERENCE_HISTO_COMMAND);
  m_editMenu->DisableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);
  m_editMenu->DisableEntry(EDIT_PAGE_PROPERTIES_COMMAND);
  
    // View menu
    m_viewMenu = new TGPopupMenu(fClient->GetRoot());
    m_viewMenu->AddEntry(m_viewInspectHistoText, INSPECT_HISTO_COMMAND);
    m_viewMenu->AddEntry(m_viewHistogramDescriptionText, HISTOGRAM_DESCRIPTION_COMMAND);
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
    m_viewMenu->AddEntry(m_viewUnDockPageText, UNDOCK_PAGE_COMMAND);
    m_viewMenu->AddEntry(m_viewDockAllText, DOCK_ALL_COMMAND);
    m_viewMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                        this, "handleCommand(Command)");
  m_viewMenu->DisableEntry(HISTORY_PLOTS_COMMAND);
  m_viewMenu->DisableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
  
    // Mode menu
    m_toolMenu = new TGPopupMenu(fClient->GetRoot());
    m_toolMenu->AddEntry(m_toolPageEditorOnline, PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->CheckEntry(PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->AddEntry(m_toolPageEditorOffline, PAGE_EDITOR_OFFLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_OFFLINE_MODE_COMMAND);  
    m_toolMenu->AddEntry(m_toolOnline, ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(ONLINE_MODE_COMMAND);
    m_toolMenu->AddEntry(m_toolOffline, OFFLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(OFFLINE_MODE_COMMAND);
    m_toolMenu->AddSeparator();
    m_toolMenu->AddEntry(m_toolFullBenchmarkText, FULL_BENCHMARK_COMMAND);
  //  m_toolMenu->AddSeparator();
  //  m_toolMenu->AddEntry(m_toolSetDimDnsText, SET_DIM_DNS_COMMAND);
    m_toolMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                        this, "handleCommand(Command)");
  //m_toolMenu->DisableEntry(ONLINE_MODE_COMMAND);
  //m_toolMenu->DisableEntry(OFFLINE_MODE_COMMAND);
  //m_toolMenu->DisableEntry(SET_UTGID_TASK_COMMAND);
  //m_toolMenu->DisableEntry(SET_DIM_DNS_COMMAND);
  
    // Help menu
    m_helpMenu = new TGPopupMenu(fClient->GetRoot());
  //  m_helpMenu->AddEntry(m_helpContentsText, HELP_CONTENTS_COMMAND);
    m_helpMenu->AddEntry(m_helpAboutText, HELP_ABOUT_COMMAND);
    m_helpMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                        this, "handleCommand(Command)");
  // m_helpMenu->DisableEntry(HELP_CONTENTS_COMMAND);
  
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
  //  m_toolbarElement = new ToolBarData_t();
  
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
    const TGPicture* savePageToDb16pic = picpool->GetPicture("savePageToDb16",
                                                             (char**)savePageToDb16);
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
    const TGPicture* automaticLayout16pic = picpool->GetPicture("automaticLayout16",
                                                                (char**)automaticLayout16);
    m_autoCanvasLayoutButton = new TGPictureButton(m_toolBar, automaticLayout16pic,
                                                   AUTO_LAYOUT_COMMAND);
    m_autoCanvasLayoutButton->SetToolTipText("Auto n x n histogram tiling layout");
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
    const TGPicture* clear16pic = picpool->GetPicture("clear16", (char**)clear16);
    m_clearHistoButton = new TGPictureButton(m_toolBar, clear16pic,
                                             CLEAR_HISTOS_COMMAND);
    m_clearHistoButton->SetToolTipText("Clear histograms");
    m_clearHistoButton->AllowStayDown(true);
    m_toolBar->AddButton(this, m_clearHistoButton, 0);
    m_clearHistoButton->Connect("Clicked()", "PresenterMainFrame", this,
                                "clearHistos()");
    // SetToggleButton(Bool_t)                              
  
    //UTGID selector (partition)
  //  m_partitionSelectorPopupMenu = new TGPopupMenu(gClient->GetRoot());
  //  m_partitionSelectorPopupMenu->AddEntry("refresh list", M_RefreshKnownPartitions);
    // Create a split button, the menu is adopted.
    m_partitionSelectorComboBox = new TGComboBox(m_toolBar, -1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
     
  //  m_partitionSelectorQuickButton = new TGSplitButton(m_toolBar,
  //                                                new TGHotString("lhcb"),
  //                                                m_partitionSelectorPopupMenu);
    m_toolBar->AddFrame(new TGLabel(m_toolBar,"Partition: ") , 
                        new TGLayoutHints(kLHintsCenterY, 2,2,2,2));
    m_toolBar->AddFrame(m_partitionSelectorComboBox , new TGLayoutHints(kLHintsCenterY,2,2,2,2));
    m_partitionSelectorComboBox->AddEntry("refresh list", 0);
    m_partitionSelectorComboBox->Select(0, false);
    m_partitionSelectorComboBox->Resize(122,22);
  // (m_partitionSelectorComboBox->GetTextEntry())->SetToolTipText("Partition selector", 300);
    
  //  m_toolBar->AddButton(this, (TGPictureButton*)m_partitionSelectorQuickButton, 8);
    m_partitionSelectorComboBox->Connect("Selected(int)", "PresenterMainFrame",
                                         this, "partitionSelectorComboBoxHandler(int)");
  
    //History mode
  
  //  m_partitionSelectorQuickButton = new TGSplitButton(m_toolBar,
  //                                                new TGHotString("lhcb"),
  //                                                m_partitionSelectorPopupMenu);
  
    
      
  //  m_presetTimePopupMenu = new TGPopupMenu(gClient->GetRoot());
    
    m_toolBar->AddFrame(new TGLabel(m_toolBar,"Time: ") , 
                        new TGLayoutHints(kLHintsCenterY, 2,2,2,2));  
    
    const TGPicture* arrow_left = picpool->GetPicture("arrow_left.xpm");
    m_previousIntervalButton = new TGPictureButton(m_toolBar, arrow_left,
                                               M_Previous_Interval);
    m_previousIntervalButton->SetToolTipText("Previous interval");
    m_previousIntervalButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_previousIntervalButton, 8);
    m_previousIntervalButton->Connect("Clicked()", "PresenterMainFrame", this,
                                  "previousInterval()");
  
    const TGPicture* arrow_right = picpool->GetPicture("arrow_right.xpm");
    m_nextIntervalButton = new TGPictureButton(m_toolBar, arrow_right,
                                               M_Next_Interval);
    m_nextIntervalButton->SetToolTipText("Next interval");
    m_nextIntervalButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_nextIntervalButton, 0);
    m_nextIntervalButton->Connect("Clicked()", "PresenterMainFrame", this,
                                  "nextInterval()");                                
  
    // Create a split button, the menu is adopted.
  //  m_historyIntervalQuickButton = new TGSplitButton(m_toolBar,
  //                                                   new TGHotString("Last &Options"),
  //                                                   m_presetTimePopupMenu);
  //  m_toolBar->AddButton(this, (TGPictureButton*)m_historyIntervalQuickButton, 8);
  //  m_historyIntervalQuickButton->Connect("ItemClicked(Int_t)", "PresenterMainFrame",
  //                                        this, "handleCommand(Command)");
  //  m_historyIntervalQuickButton->SetState(kButtonDisabled);
  
    // History plots
    m_historyPlotsButton= new TGPictureButton(m_toolBar, m_stockNewFormula,
                                               HISTORY_PLOTS_COMMAND);
    m_historyPlotsButton->SetToolTipText("Toggle history plot merge or trend mode");
    m_historyPlotsButton->AllowStayDown(true);
    m_toolBar->AddButton(this, m_historyPlotsButton, 8);
    m_historyPlotsButton->Connect("Clicked()", "PresenterMainFrame",
                                  this, "toggleHistoryPlots()");
    m_historyPlotsButton->SetState(kButtonDisabled);
    // SetToggleButton(Bool_t)
  
    m_historyIntervalComboBox = new TGComboBox(m_toolBar, -1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
    m_toolBar->AddFrame(new TGLabel(m_toolBar,"History using: ") ,
                        new TGLayoutHints(kLHintsCenterY, 2,2,2,2));  
    m_toolBar->AddFrame(m_historyIntervalComboBox , new TGLayoutHints(kLHintsCenterY,2,2,2,2));  
    m_historyIntervalComboBox->SetEnabled(false);
  //  (m_historyIntervalComboBox->GetTextEntry())->SetToolTipText("History time interval", 300);    
    m_historyIntervalComboBox->AddEntry("preset file", M_Last_File);
    m_historyIntervalComboBox->AddEntry("set file", M_File_Picker);
  //  m_historyIntervalComboBox->AddSeparator();  
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
    m_overlayReferenceHistoButton = new TGPictureButton(m_toolBar, f2_tpic,
                                               OVERLAY_REFERENCE_HISTO_COMMAND);
    m_overlayReferenceHistoButton->SetToolTipText("Overlay with reference");
    m_overlayReferenceHistoButton->AllowStayDown(true);
    m_toolBar->AddButton(this, m_overlayReferenceHistoButton, 8);
    m_overlayReferenceHistoButton->Connect("Clicked()", "PresenterMainFrame",
                                           this, "toggleReferenceOverlay()");
    m_overlayReferenceHistoButton->SetState(kButtonDisabled);
    // SetToggleButton(Bool_t) 
  
    // Set reference
    const TGPicture* f1_tpic = picpool->GetPicture("f1_t.xpm");
    m_pickReferenceHistoButton = new TGPictureButton(m_toolBar, f1_tpic,
                                                    PICK_REFERENCE_HISTO_COMMAND);
    m_pickReferenceHistoButton->SetToolTipText("Pick reference histogram");
    m_pickReferenceHistoButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_pickReferenceHistoButton, 0);
    m_pickReferenceHistoButton->SetState(kButtonDisabled);
    m_pickReferenceHistoButton->Connect("Clicked()", "PresenterMainFrame",
                                        this, "pickReferenceHistogram()");
  //  m_pickReferenceHistoButton->SetState(kButtonDisabled);
  
    // Clone Histo
    const TGPicture* inspectHistogram16pic = picpool->GetPicture("inspectHistogram16",
                                                                 (char**)inspectHistogram16);
    m_inspectHistoButton = new TGPictureButton(m_toolBar, inspectHistogram16pic,
                                               INSPECT_HISTO_COMMAND);
    m_inspectHistoButton->SetToolTipText("Inspect histogram");
    m_inspectHistoButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_inspectHistoButton, 8);
    m_inspectHistoButton->Connect("Clicked()", "PresenterMainFrame",
                                  this, "inspectHistogram()");
  
    // Histo Description
    const TGPicture* histogramDescription16pic = picpool->GetPicture("about.xpm");
    m_histogramDescriptionButton = new TGPictureButton(m_toolBar, histogramDescription16pic,
                                               HISTOGRAM_DESCRIPTION_COMMAND);
    m_histogramDescriptionButton->SetToolTipText("Histogram description");
    m_histogramDescriptionButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_histogramDescriptionButton, 0);
    m_histogramDescriptionButton->Connect("Clicked()", "PresenterMainFrame",
                                  this, "histogramDescription()");
                                                                  
  
    // Edit Histogram properties
    const TGPicture* editProperties16pic = picpool->GetPicture("editProperties16",
                                                               (char**)editProperties16);
    m_editHistoButton = new TGPictureButton(m_toolBar, editProperties16pic,
                                            EDIT_HISTO_COMMAND);
    m_editHistoButton->SetToolTipText("Edit histogram properties");
    m_editHistoButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_editHistoButton, 0);
    m_editHistoButton->Connect("Clicked()", "PresenterMainFrame",
                               this, "editHistogramProperties()");
  
    // Delete histo from Canvas
    const TGPicture* editDelete16pic = picpool->GetPicture("editDelete16",
                                                           (char**)editDelete16);
    m_deleteHistoFromCanvasButton = new TGPictureButton(m_toolBar,
                                                        editDelete16pic,
                                                        REMOVE_HISTO_FROM_CANVAS_COMMAND);
    m_deleteHistoFromCanvasButton->SetToolTipText("Remove selected histogram from page");
    m_deleteHistoFromCanvasButton->AllowStayDown(false);
    m_toolBar->AddButton(this, m_deleteHistoFromCanvasButton, 8);
    m_deleteHistoFromCanvasButton->Connect("Clicked()", "PresenterMainFrame",
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
                                             
      TGVSplitter* leftVerticalSplitter = new TGVSplitter(m_mainHorizontalFrame,2, 2);
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
  
      TGGroupFrame* databasePagesGroupFrame = new TGGroupFrame(m_databasePagesDock,
                                                               TGString("Database Page Browser"),
                                                               kVerticalFrame |
                                                               kFixedHeight);
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
      (m_pagesFromHistoDBListTree->GetFirstItem())->SetPictures(m_databaseSourceIcon,
                                                                m_databaseSourceIcon);
      m_pagesFromHistoDBListTree->Connect("Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)",
                                          "PresenterMainFrame", this,
                                          "clickedPageTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");

      m_pagesFromHistoDBListTree->Connect("DataDropped(TGListTreeItem*, TDNDData*)",
                                          "PresenterMainFrame", this,
                                          "dataDropped(TGListTreeItem*, TDNDData*)");
  
      m_pagesFromHistoDBViewPort->AddFrame(m_pagesFromHistoDBListTree);
      m_pagesFromHistoDBListTree->SetLayoutManager(
        new TGHorizontalLayout(m_pagesFromHistoDBListTree));
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
//      m_pagesContextMenu->AddSeparator();
//      m_pagesContextMenu->AddEntry("Move/Rename", M_Move_COMMAND);
      m_pagesContextMenu->AddSeparator();
      m_pagesContextMenu->AddEntry("Delete Page", M_DeletePage_COMMAND);
      m_pagesContextMenu->AddEntry("Delete Folder", M_DeleteFolder_COMMAND);
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
  
    TGGroupFrame* databaseAlarmGroupFrame = new TGGroupFrame(m_leftDatabaseAlarmFrame,
                                                                 TGString("Histograms in Alarm"),
                                                                 kVerticalFrame |
                                                                 kFixedWidth);
  
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
          // Alarm sort box
          m_alarmDBFilterComboBox = new TGComboBox(databaseAlarmGroupFrame, -1,
                                                   kHorizontalFrame | kDoubleBorder |
                                                   kSunkenFrame | kOwnBackground);
          m_alarmDBFilterComboBox->AddEntry("Severity",
                                            HistogramsWithAnalysis); //#3 //Alarm, Warning, Info
          m_alarmDBFilterComboBox->AddEntry("Analysis Task", Tasks); // #1
  //        m_alarmDBFilterComboBox->AddEntry("Time", FoldersAndPages); // #0 // Time
          m_alarmDBFilterComboBox->AddEntry("Full list", AllHistograms); // #5
          // @enum FilterCriteria
          m_alarmDBFilterComboBox->Resize(149, 22);
          m_alarmDBFilterComboBox->Select(Tasks);
  //        m_alarmDBFilterComboBox->Connect("Selected(Int_t)", "PresenterMainFrame",
  //                                         this, "refreshHistoDBListTree()");
          databaseAlarmGroupFrame->AddFrame(m_alarmDBFilterComboBox,
                                                new TGLayoutHints(kLHintsLeft |
                                                                  kLHintsTop |
                                                                  kLHintsExpandX,
                                                                  0, 0, 0, 0));
          // canvas widget
          m_alarmDBCanvas = new TGCanvas(databaseAlarmGroupFrame, 124, 460);
  
          // canvas viewport
          m_alarmDBCanvasViewPort = m_alarmDBCanvas->GetViewPort();
  
          // list tree
          m_alarmHistogramTreeList = new TGListTree(m_alarmDBCanvas,
                                                       kHorizontalFrame);
          m_alarmHistogramTreeList->AddRoot("Alarms");
          (m_alarmHistogramTreeList->GetFirstItem())->SetPictures(m_iconAnalysisAlarm, m_iconAnalysisAlarm);
          m_alarmHistogramTreeList->SetCheckMode(TGListTree::kRecursive);
          m_alarmHistogramTreeList->Connect(
            "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", "PresenterMainFrame",
            this, "clickedAlarmTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");
  
          m_alarmDBContextMenu = new TGPopupMenu(fClient->GetRoot());
  //        m_alarmDBContextMenu->AddEntry("Add checked histogram(s) to Page",
  //                                       M_AddDBHistoToPage_COMMAND);
  //        m_alarmDBContextMenu->AddEntry("Add checked to Page as One",
  //                                       M_AddDBHistoToPageAsOne_COMMAND);                                          
  //        m_alarmDBContextMenu->AddEntry("Set properties of checked histogram(s)",
  //                                       M_SetHistoPropertiesInDB_COMMAND);
  //        m_alarmDBContextMenu->AddEntry("Delete checked histogram(s)",
  //                                       M_DeleteDBHisto_COMMAND);
  //        m_alarmDBContextMenu->AddSeparator();
  //        m_alarmDBContextMenu->AddEntry("Collsapse all children",
  //                                       M_DBHistoCollapseAllChildren_COMMAND);
  //        m_alarmDBContextMenu->AddSeparator();
          m_alarmDBContextMenu->AddEntry("Refresh", M_RefreshAlarmDBListTree_COMMAND);
          m_alarmDBContextMenu->Connect("Activated(Int_t)","PresenterMainFrame",
                                        this, "handleCommand(Command)");
  //m_alarmDBContextMenu->DisableEntry(M_SetHistoPropertiesInDB_COMMAND);
  
          m_alarmDBCanvasViewPort->AddFrame(m_alarmHistogramTreeList);
          m_alarmHistogramTreeList->SetLayoutManager(
            new TGHorizontalLayout(m_alarmHistogramTreeList));
          m_alarmHistogramTreeList->MapSubwindows();
          m_alarmDBCanvas->SetContainer(m_alarmHistogramTreeList);
          m_alarmDBCanvas->MapSubwindows();
          databaseAlarmGroupFrame->AddFrame(m_alarmDBCanvas,
                                                new TGLayoutHints(kLHintsLeft |
                                                                  kLHintsTop |
                                                                  kLHintsExpandX |
                                                                  kLHintsExpandY,
                                                                  0, 0, 0, 0));
  // -------------------------
  
    m_pageDock = new TGDockableFrame(m_mainHorizontalFrame);
    m_pageDock->SetWindowName("LHCb Presenter Page");
    m_pageDock->EnableUndock(true);
    m_pageDock->EnableHide(false);
    m_pageDock->Resize(600, 494);
    m_pageDock->Connect("Docked()", "PresenterMainFrame",
                        this, "enablePageUndocking()");
    m_pageDock->Connect("Undocked()", "PresenterMainFrame",
                        this, "disablePageUndocking()");
    // centralFrame
    TGVerticalFrame* centralPageFrame = new TGVerticalFrame(m_pageDock,
                                                            600, 494,
                                                            kVerticalFrame);
    m_pageDock->AddFrame(centralPageFrame,
                         new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                           kLHintsExpandX | kLHintsExpandY,
                                           0, 0, 0, 0));
    int partsTop[] = {75, 25};
    m_statusBarTop = new TGStatusBar(centralPageFrame, 50, 10, kVerticalFrame);
    m_statusBarTop->SetParts(partsTop, 2);
    m_statusBarTop->Draw3DCorner(false);
    centralPageFrame->AddFrame(m_statusBarTop, new TGLayoutHints(kLHintsTop |
                                                                 kLHintsExpandX,
                                                                 0, 0, 0, 0));
  
    // embedded canvas
    editorEmbCanvas = new TRootEmbeddedCanvas(0, centralPageFrame, 600, 494);
    int wm_editorEmbCanvas = editorEmbCanvas->GetCanvasWindowId();
    editorEmbCanvas->SetBit(kNoContextMenu);
    editorCanvas = new TCanvas("editor canvas", 600, 494, wm_editorEmbCanvas);
    editorCanvas->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)",
                          "PresenterMainFrame", this,
                          "EventInfo(Int_t, Int_t, Int_t, TObject*)");
  //  editorCanvas->SetGrid();
    editorCanvas->SetBit(kNoContextMenu);
  
  //  editorCanvas->SetFixedAspectRatio(true);
    editorEmbCanvas->AdoptCanvas(editorCanvas);
    centralPageFrame->AddFrame(editorEmbCanvas, new TGLayoutHints(kLHintsRight |
                                                                  kLHintsTop |
                                                                  kLHintsExpandX |
                                                                  kLHintsExpandY,
                                                                  0, 0, 0, 0));
  // Info dock
    m_mainCanvasInfoDock = new TGDockableFrame(centralPageFrame);
  //  m_pageDock->SetWindowName("LHCb Presenter Page");
    m_mainCanvasInfoDock->SetWindowName("LHCb Presenter Information");
  
    m_mainCanvasInfoDock->EnableUndock(true);
    m_mainCanvasInfoDock->EnableHide(true);
    m_mainCanvasInfoDock->Resize(600, 90);
  
  
    TGGroupFrame* mainCanvasInfoGroupFrame = new TGGroupFrame(m_mainCanvasInfoDock,
                                                             TGString("Page Comments"),
                                                             kVerticalFrame |
                                                             kFixedHeight);
    m_mainCanvasInfoDock->AddFrame(mainCanvasInfoGroupFrame,
                               new TGLayoutHints(kLHintsTop |
                                                 kLHintsExpandX|
                                                 kLHintsExpandY,
                                                 0, 0, 0, 0));
  
  //  TGCanvas* fCanvas652 = new TGCanvas(mainCanvasInfoGroupFrame, 220, 243);
    
  
    centralPageFrame->AddFrame(m_mainCanvasInfoDock,
                               new TGLayoutHints(kLHintsRight |
                                                 kLHintsTop |
                                                 kLHintsExpandX,
                                                 0, 0, 0, 0));
  
  
  
  
  
  //    TGTab* fTab515 = new TGTab(m_mainCanvasInfoDock, 600, 90);
      // container of "Page Description"
  //    TGCompositeFrame* fCompositeFrame518;
  //    fCompositeFrame518 = fTab515->AddTab("Page Description");
  //    fCompositeFrame518->SetLayoutManager(new TGVerticalLayout(fCompositeFrame518));
  //    fCompositeFrame518->SetLayoutBroken(true);
  //    TGTextEdit* fTextEdit523 = new TGTextEdit(fCompositeFrame518, 600, 80);
  
  //   TGTextEdit *fTextEdit515 = new TGTextEdit(fMainFrame648,486,62);
  //   fTextEdit515->LoadFile("TxtEdit515");
  //   fMainFrame648->AddFrame(fTextEdit515, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));
  //   fTextEdit515->MoveResize(2,2,486,62);
  
  
      m_pageDescriptionView = new TGTextView(mainCanvasInfoGroupFrame, 600, 80);
      //fTextEdit523->LoadFile("TxtEdit523");
      mainCanvasInfoGroupFrame->AddFrame(m_pageDescriptionView, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));
  //    fTextEdit523->Layout();
                                                                   
  //    fTextEdit523->MoveResize(2, 2, 600, 80);
      // container of "Tab2"
  //    TGCompositeFrame* fCompositeFrame520;
  //    fCompositeFrame520 = fTab515->AddTab("Message Log");
  //    //fCompositeFrame520->SetLayoutManager(new TGVerticalLayout(fCompositeFrame520));
  //    fCompositeFrame520->SetLayoutBroken(true);
  //    TGTextEdit* fTextEdit542 = new TGTextEdit(fCompositeFrame520, 400, 95);
  //    //fTextEdit542->LoadFile("TxtEdit542");
  //    fCompositeFrame520->AddFrame(fTextEdit542,
  //                                 new TGLayoutHints(kLHintsLeft |
  //                                                   kLHintsBottom |
  //                                                   kLHintsExpandX,
  //                                                   2, 2, 2, 2));
  //    //fTextEdit542->MoveResize(0, 0, 290, 77);
  //    fTab515->SetTab(1);
  //    fTab515->Resize(fTab515->GetDefaultSize());
  //    m_mainCanvasInfoDock->AddFrame(fTab515, new TGLayoutHints(kLHintsRight |
  //                                                              kLHintsTop |
  //                                                              kLHintsExpandX,
  //                                                              2, 2, 2, 2));
      //fTab515->MoveResize(64, 40, 296, 104);
  
    m_rightMiscFrame = new TGVerticalFrame(m_mainHorizontalFrame, 220, 494,
                                           kVerticalFrame | kFixedWidth |
                                           kLHintsExpandX);
  //                                         | kLHintsExpandX); // kFixedWidth
  
      m_rightVerticalSplitter = new TGVSplitter(m_mainHorizontalFrame, 2, 2);
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
  
      m_histoSvcBrowserGroupFrame = new TGGroupFrame(m_dimBrowserDock,
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
    //  m_histoSvcListTree->SetAutoCheckBoxPic(false); ROOT v5.16(?)...
      m_histoSvcListTree->AddRoot("DIM");
      (m_histoSvcListTree->GetFirstItem())->SetPictures(m_dimOnline16,
                                                      m_dimOnline16);
      m_histoSvcListTree->SetCheckMode(TGListTree::kRecursive);                                                    
      m_histoSvcListTree->Connect(
        "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", "PresenterMainFrame",
        this, "clickedHistoSvcTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");
  
      fViewPort664->AddFrame(m_histoSvcListTree);
      m_histoSvcListTree->SetLayoutManager(new TGHorizontalLayout(
                                           m_histoSvcListTree));
      m_histoSvcListTree->MapSubwindows();
      m_dimSvcListTreeContainterCanvas->SetContainer(m_histoSvcListTree);
      m_dimSvcListTreeContainterCanvas->MapSubwindows();
      m_histoSvcBrowserGroupFrame->AddFrame(m_dimSvcListTreeContainterCanvas,
        new TGLayoutHints(kLHintsRight | kLHintsTop |
                          kLHintsExpandX | kLHintsExpandY,
                          0, 0, 0, 0));
  
      m_histoSvcTreeContextMenu = new TGPopupMenu(fClient->GetRoot());
      m_histoSvcTreeContextMenu->AddEntry("Add checked to Database",
                                 M_AddHistoToDB_COMMAND);
      m_histoSvcTreeContextMenu->AddEntry("Add checked to Page",
                                 M_AddHistoToPage_COMMAND);                            
      m_histoSvcTreeContextMenu->AddSeparator();
  //    m_histoSvcTreeContextMenu->AddEntry("Resubscribe to checked",
  //                               M_SetDimSourceFromTree);
      m_histoSvcTreeContextMenu->AddSeparator();
      m_histoSvcTreeContextMenu->AddEntry("Collsapse all children",
                                 M_DimCollapseAllChildren_COMMAND);
      m_histoSvcTreeContextMenu->AddSeparator();
      m_histoSvcTreeContextMenu->AddEntry("Refresh", M_RefreshHistoSvcListTree);
      m_histoSvcTreeContextMenu->Connect("Activated(Int_t)","PresenterMainFrame", this,
                                "handleCommand(Command)");
  
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
    TGVerticalFrame* leftDatabaseHistogramFrame = new TGVerticalFrame(m_databaseHistogramsDock,
                                                                      220, 100,
                                                                      kVerticalFrame |
                                                                      kFixedWidth);
  //TGGroupFrame* leftDatabaseHistogramFrame = new TGGroupFrame(m_databaseHistogramsDock, kVerticalFrame | kFixedWidth);
    TGGroupFrame* databaseHistogramGroupFrame = new TGGroupFrame(leftDatabaseHistogramFrame,
                                                                 TGString("Histograms in Database"),
                                                                 kVerticalFrame |
                                                                 kFixedWidth);
  //TGGroupFrame(m_databaseHistogramsDock, TGString("Histogram Database"), kVerticalFrame | kFixedWidth);
  
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
          // @enum FilterCriteria
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
          m_databaseHistogramTreeList->Connect(
            "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", "PresenterMainFrame",
            this, "clickedHistoDBTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");
  
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
          m_histoDBContextMenu->AddEntry("Refresh",
            M_RefreshHistoDBListTree_COMMAND);
          m_histoDBContextMenu->Connect("Activated(Int_t)","PresenterMainFrame",
                                        this, "handleCommand(Command)");
  //m_histoDBContextMenu->DisableEntry(M_SetHistoPropertiesInDB_COMMAND);
  
          m_histoDBCanvasViewPort->AddFrame(m_databaseHistogramTreeList);
          m_databaseHistogramTreeList->SetLayoutManager(
            new TGHorizontalLayout(m_databaseHistogramTreeList));
          m_databaseHistogramTreeList->MapSubwindows();
          m_histoDBCanvas->SetContainer(m_databaseHistogramTreeList);
          m_histoDBCanvas->MapSubwindows();
          databaseHistogramGroupFrame->AddFrame(m_histoDBCanvas,
                                                new TGLayoutHints(kLHintsLeft |
                                                                  kLHintsTop |
                                                                  kLHintsExpandX |
                                                                  kLHintsExpandY,
                                                                  0, 0, 0, 0));
  
                                                    
    m_mainHorizontalFrame->AddFrame(m_leftMiscFrame,
                                    new TGLayoutHints(kLHintsLeft |
                                                      kLHintsExpandY,
                                                      0, 0, 0, 0));                                                    
  
  //  m_mainHorizontalFrame->AddFrame(m_databasePagesDock,
  //                                  new TGLayoutHints(kLHintsLeft |
  //                                                    kLHintsExpandY,
  //                                                    0, 0, 0, 0));
  
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
  //  m_mainCanvasInfoDock->MapWindow();
  

  m_intervalPicker = new IntervalPicker(this);
  partitionSelectorComboBoxHandler(m_partitionSelectorComboBox->GetSelected());
  m_prevPresenterMode = m_presenterMode;
  m_presenterMode = Online;  
  
  
    SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputPrimaryApplicationModal);
    MapSubwindows();
    MapWindow();
    m_rightMiscFrame->UnmapWindow();
    m_rightVerticalSplitter->UnmapWindow();
  //  m_databaseAlarmsDock->UndockContainer();
    this->Resize(m_initWidth, m_initHeight);
    DoRedraw();
  }
}

void PresenterMainFrame::dataDropped(TGListTreeItem* folder, TDNDData* data)
{
  if (isConnectedToHistogramDB() &&
      (ReadWrite == m_databaseMode) &&
      folder && folder->IsDNDTarget() &&
      data && data->fData &&
      folder->GetUserData()) {
    TString fromName = static_cast<char*>(data->fData);
    if (fromName.BeginsWith(s_FILE_URI)) {fromName.Remove(0, s_FILE_URI.length()); }
    std::string pageName(fromName);

    TString newPageName = (*static_cast<TObjString*>(folder->GetUserData())).GetString();
    if (newPageName.BeginsWith(s_FILE_URI)) { newPageName.Remove(0, s_FILE_URI.length()); }

    // Tree does not handle deep recursion correctly ...target, source and 42 broken.
    TGListTreeItem* page = 0;
    char path[1024];
    m_pagesFromHistoDBListTree->GetPathnameFromItem(m_pagesFromHistoDBListTree->GetSelected(), path);

    if (strlen(path)) { // avoid being @root
      std::string rootPath(path);
      page = m_pagesFromHistoDBListTree->FindItemByPathname(rootPath.c_str());
      if (page && (kFALSE == page->IsDNDTarget()) &&
          (kFALSE == folder->IsDNDSource())) {
        fromName.Remove(0, fromName.Last('/')+1);
        // when TDNDData/TGListTreeItem seems to be uninit., 2nd try works...
        if (0 == strcmp(fromName, page->GetText())) {
          newPageName.Append(s_slash).Append(fromName);
          try {
            if (m_verbosity >= Verbose) {
              std::cout << "mv \"" << pageName << "\" \"" << newPageName << "\""<< std::endl;
            }
            OnlineHistPage* dbPage = m_histogramDB->getPage(pageName);
            dbPage->rename(std::string(newPageName.Data()));
            if (m_histogramDB->commit()) {
              std::string userPageData(s_FILE_URI);
              userPageData.append(newPageName);
              TObjString *objectString = new TObjString(userPageData.c_str());
              page->SetUserData(objectString);
              m_pagesFromHistoDBListTree->Reparent(page, folder);
            }
          } catch (std::string sqlException) {
            setStatusBarText(sqlException.c_str(), 2);
            if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }
            if (Batch != m_presenterMode) {
              new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                  Form("Could move the page to OnlineHistDB:\n\n%s\n",
                      sqlException.c_str()),
                      kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
            }
          }
        } else { //zut.
        }
      }
    }
  }
}

void PresenterMainFrame::CloseWindow()
{
  if (0 != m_pageRefreshTimer) { m_pageRefreshTimer->Stop(); }
  if (0 != m_clockTimer) { m_clockTimer->Stop(); }
  gApplication->Terminate();
}
void PresenterMainFrame::dockAllFrames()
{
  m_pageDock->DockContainer();
//  m_mainCanvasInfoDock->DockContainer();
  m_toolBarDock->DockContainer();
  m_menuDock->DockContainer();
  m_databaseHistogramsDock->DockContainer();
  m_dimBrowserDock->DockContainer();
  m_databasePagesDock->DockContainer();
  m_databaseAlarmsDock->DockContainer();
}
void PresenterMainFrame::handleCommand(Command cmd)
{
  // ROOT GUI workaround: slot mechanism differs on different signals from
  // different widgets... void* 4 messages :-(
  if (X_ENIGMA_COMMAND == cmd) {
    TGButton* btn = static_cast<TGButton*>(gTQSender);
    cmd = static_cast<Command>(btn->WidgetId());
  }  
//  if ((500 <= cmd) < 550) {
//    setHistogramDimSource(s_withoutTree);
//  }
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
    case UNDOCK_PAGE_COMMAND:
      m_pageDock->UndockContainer();
      break;
    case OFFLINE_MODE_COMMAND:
      setPresenterMode(History);
      break;
    case FULL_BENCHMARK_COMMAND:
      loadAllPages();
      break;
    case ONLINE_MODE_COMMAND:
      setPresenterMode(Online);
      break;
    case PAGE_EDITOR_ONLINE_MODE_COMMAND:
      setPresenterMode(EditorOnline);
      break;
    case PAGE_EDITOR_OFFLINE_MODE_COMMAND:
      setPresenterMode(EditorOffline);
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
//    case M_SetDimSourceFromTree:
//      setHistogramDimSource(s_withTree);
//      break;
//    case M_SetDimSourceFromQuickButton:
//      setHistogramDimSource(s_withoutTree);
//      break;      
    case M_DimCollapseAllChildren_COMMAND:
      dimCollapseAllChildren();
      break;
    case M_RefreshHistoSvcListTree:
      refreshHistogramSvcList(s_withTree);
      partitionSelectorComboBoxHandler(0);
      break;
    case M_PartitionList:
      partitionSelectorComboBoxHandler(m_partitionSelectorComboBox->GetSelected());
      break;
    case M_AddDBHistoToPage_COMMAND:
        addDbHistoToPage(separate);
      break;
    case M_AddDBHistoToPageAsOne_COMMAND:
        addDbHistoToPage(overlap);
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
      listAlarmsFromHistogramDB(m_alarmHistogramTreeList, AllHistograms);
      break;          
    case M_RefreshDBPagesListTree_COMMAND:
      refreshPagesDBListTree();
      m_histogramDB->refresh();
      break;
    case M_IntervalPicker:
      m_intervalPicker->MapWindow();
      m_previousIntervalButton->SetState(kButtonEngaged);
      m_previousIntervalButton->SetState(kButtonUp);
      m_nextIntervalButton->SetState(kButtonEngaged);
      m_nextIntervalButton->SetState(kButtonUp);
      break;
    case M_Last_Interval:
      m_previousIntervalButton->SetState(kButtonEngaged);
      m_previousIntervalButton->SetState(kButtonUp);
      m_nextIntervalButton->SetState(kButtonEngaged);
      m_nextIntervalButton->SetState(kButtonUp);
//      m_currentPageName = selectedPageFromDbTree();
      if (!m_currentPageName.empty() && (false == m_loadingPage)) {
        loadSelectedPageFromDB(m_currentPageName, global_timePoint, global_pastDuration, false);
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
    case M_Last_File: {
      m_previousIntervalButton->SetState(kButtonDisabled);
      m_nextIntervalButton->SetState(kButtonDisabled);
      m_currentPageName = selectedPageFromDbTree();
      if (!m_currentPageName.empty() && (false == m_loadingPage)) {
        loadSelectedPageFromDB(m_currentPageName, s_startupFile, m_savesetFileName, s_previousPageToHistory);
      }
    }
      break;
    case M_File_Picker: {
        TGFileInfo fileInfo;
        const char* fileTypes[] = {"ROOT files", "*.root",
                                   "All files", "*",
                                   0 , 0};
        fileInfo.fFileTypes = fileTypes;
        fileInfo.fIniDir = StrDup(m_savesetPath.c_str());
        fileInfo.SetMultipleSelection(false);
// if (true == fileInfo.fMultipleSelection) { TList 1st element};
        new TGFileDialog(fClient->GetRoot(),this,kFDOpen,&fileInfo);
        if (0 != fileInfo.fFilename) {
          m_savesetFileName = std::string(fileInfo.fFilename);
          if(false == m_savesetFileName.empty()) {        
            if (EditorOffline == m_presenterMode) {
              refreshHistogramSvcList(s_withTree);
            }
            m_previousIntervalButton->SetState(kButtonDisabled);
            m_nextIntervalButton->SetState(kButtonDisabled);
  // std::cout << "selected m_savesetFileName: " << m_savesetFileName << std::endl;
            m_currentPageName = selectedPageFromDbTree();                  
  
            if (!m_currentPageName.empty() && (false == m_loadingPage)) {
              loadSelectedPageFromDB(m_currentPageName, s_startupFile, m_savesetFileName, s_previousPageToHistory);
            }          
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
    case M_DeleteFolder_COMMAND:
      deleteSelectedFolderFromDB();
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
      if (m_verbosity >= Debug) {
        std::cout << "zut. TGButton WidgetId from gTQSender is corrupt"
          << std::endl;
      }
      break;
  }
}
void PresenterMainFrame::setVerbosity(const pres::MsgLevel & verbosity)
{
  m_verbosity = verbosity;
  if (Silent == m_verbosity) {
    gErrorIgnoreLevel = 1001;
  }
  if (0 != m_archive) { m_archive->setVerbosity(m_verbosity); }
}

void PresenterMainFrame::setLogbookConfig(const std::string & logBookConfig)
{
  m_logBookConfig = logBookConfig;
}

void PresenterMainFrame::setDatabaseMode(const DatabaseMode & databaseMode) {
  switch (databaseMode) {
    case LoggedOut:
      logoutFromHistogramDB();
      break;
    case UserSelected:
      logoutFromHistogramDB();
      loginToHistogramDB();
      break;
    case ReadOnly:
      logoutFromHistogramDB();
      if (0 < m_knownDbCredentials.size()) {
        std::map<std::string*, std::string*>::iterator dbCred;
        dbCred = m_knownDbCredentials.begin();      
        connectToHistogramDB(*(*dbCred).second,
                             *(*dbCred).first,
                             *m_knownDatabases[0]);
      }
      break;
    case ReadWrite:
      logoutFromHistogramDB();
      loginToHistogramDB();
      break;
    default:
      if (m_verbosity >= Debug) {
        std::cout << "something went wrong when setting database mode."
                  << std::endl;
      }
      break;
  }
}
void PresenterMainFrame::setReferencePath(const std::string & referencePath) {
  m_referencePath = referencePath;
  if (m_analysisLib) m_analysisLib->setRefRoot(m_referencePath);
  if (!m_referencePath.empty()) {
    if (0 != m_archive) {
      m_archive->setReferencePath(m_referencePath);
    }    
  }
}
void PresenterMainFrame::setSavesetPath(const std::string & savesetPath) {
  m_savesetPath = savesetPath;
  if (!m_savesetPath.empty()) {
    if (0 != m_archive) {
      m_archive->setSavesetPath(m_savesetPath);
    }
  }
}
void PresenterMainFrame::setImagePath(const std::string & imagePath) {
  m_imagePath = imagePath;
}
void PresenterMainFrame::setDumpFormat(const std::string & dumpFormat) {
  m_dumpFormat = dumpFormat;
}
void PresenterMainFrame::setPartition(const std::string & partition) {
  m_currentPartition = partition;
}

void PresenterMainFrame::setPresenterMode(const PresenterMode & presenterMode)
{
  switch (presenterMode) {
    case Init:
      buildGUI();
      break;
    case Batch:
      gROOT->SetBatch();
      m_presenterMode = Batch;
      editorCanvas = new TCanvas("c", "c", 300, 300);
      editorCanvas->cd();
//      gStyle->SetOptStat("m");
      gStyle->SetOptStat(0);
      break;              
    case Online:
      m_prevPresenterMode = m_presenterMode;
      m_presenterMode = Online;
      break;
    case History:
      m_prevPresenterMode = m_presenterMode;
      m_presenterMode = History;
      break;
    case EditorOnline:
      m_prevPresenterMode = m_presenterMode;
      m_presenterMode = EditorOnline;
      if (ReadWrite != m_databaseMode) { setDatabaseMode(ReadWrite); }
      break;
    case EditorOffline:
      m_prevPresenterMode = m_presenterMode;
      m_presenterMode = EditorOffline;
      if (ReadWrite != m_databaseMode) { setDatabaseMode(ReadWrite); }
      break;      
    default:    
      if (m_verbosity >= Debug) {
        std::cout << "something went wrong when setting presenter mode."
          << std::endl;
      }
      break;
  }
  if (Batch != m_presenterMode) {
    reconfigureGUI();
  }
}

void PresenterMainFrame::setKnownDatabases(const std::string & databasesCfg,
                                           const std::string & dbcredentials)
{
    TObjArray* databaseItems(TString(databasesCfg.c_str()).Tokenize(s_configToken));
    TIterator* databaseItemsIt = databaseItems->MakeIterator();
    TObject*   databaseItem;
    while ((databaseItem = databaseItemsIt->Next())) {
      std::string* databaseID = new std::string(databaseItem->GetName());
      m_knownDatabases.push_back(databaseID);
    }
    databaseItems->Delete();
    delete databaseItems;
    databaseItems = NULL;
    delete databaseItemsIt;
    databaseItemsIt = NULL;
    
    TObjArray* dbLoginPairItems(TString(dbcredentials.c_str()).Tokenize(s_configToken));
    TIterator* dbLoginPairItemsIt = dbLoginPairItems->MakeIterator(); // memleak?
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
      delete loginElementItems;
      loginElementItems = NULL;
      delete loginElementIt;
      loginElementIt = NULL;
    }
    dbLoginPairItems->Delete();
    delete dbLoginPairItems;
    dbLoginPairItemsIt = NULL;
    delete dbLoginPairItemsIt;
    dbLoginPairItemsIt = NULL;    
}

void PresenterMainFrame::enableAutoCanvasLayoutBtn()
{
//  m_autoCanvasLayoutButton->SetState(kButtonEngaged);
  m_autoCanvasLayoutButton->SetState(kButtonUp);
}
void PresenterMainFrame::disableAutoCanvasLayoutBtn()
{
  m_autoCanvasLayoutButton->SetState(kButtonDisabled);
}
bool PresenterMainFrame::isConnectedToHistogramDB()
{
  if (LoggedOut == m_databaseMode && 0 == m_histogramDB) {
    return false;
  } else if (ReadOnly == m_databaseMode && 0 != m_histogramDB) {
    return true;
  } else if (ReadWrite == m_databaseMode && 0 != m_histogramDB) {
    return true;
  } else {
    return false;
  }
}
bool PresenterMainFrame::canWriteToHistogramDB()
{
  if (ReadWrite == m_databaseMode && 0 != m_histogramDB) {
    return true;
  } else {
    return false;
  }
}
void PresenterMainFrame::savePageToFile()
{
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

void PresenterMainFrame::savePageToHistogramDB()
{
  if (ReadWrite == m_databaseMode) {
    fClient->WaitFor(new DatabasePagePathDialog(this, 493, 339, m_verbosity));
//  m_savePageToDatabaseButton->SetState(kButtonDisabled);
    refreshPagesDBListTree();
//    refreshHistoDBListTree();
//    statusBar()
  }
}
void PresenterMainFrame::reportToLog()
{
  if (m_refreshingPage) {
    stopPageRefresh();
    m_resumePageRefreshAfterLoading = true;
  }
  
  if (false == m_logBookConfig.empty()) {  
    TString pageName;
    gSystem->TempFileName(pageName);
    pageName.Append(".png");
    editorCanvas->SaveAs(pageName.Data());      
    std::string shellcmd(m_logBookConfig + " -f " + pageName.Data() + " " + std::string("LHCb ST TED run pages removed from logbook")); //  " -m "  
    gSystem->Exec(shellcmd.c_str());
    gSystem->Unlink(pageName.Data());
  }
  if (m_resumePageRefreshAfterLoading) { startPageRefresh(); }
  
}
// below is either HISTO DIM sevices, or a ROOT filename
void PresenterMainFrame::setStartupHistograms(const std::vector<std::string> & histogramList)
{
  std::vector<std::string>::const_iterator histogramListIt;
  histogramListIt = histogramList.begin();
  while (histogramListIt != histogramList.end()) {
    TString inputParamTS = TString(*histogramListIt);
    if (inputParamTS.EndsWith(s_rootFileExtension.c_str())) {
      m_savesetFileName = *histogramListIt;
//      m_rightMiscFrame->HideFrame(m_dimBrowserDock);
//      m_rightMiscFrame-> ShowFrame();
      
    } else {
      HistogramIdentifier histogram = HistogramIdentifier(*histogramListIt);
      if (histogram.isDimFormat()) {      
        addHistoToPage(*histogramListIt, separate);
      }
    }
    histogramListIt++;
  }

  if (Batch != m_presenterMode) {
    autoCanvasLayout();
  }
}
bool PresenterMainFrame::connectToHistogramDB(const std::string & dbPassword,
                                              const std::string & dbUsername,
                                              const std::string & dbName)
{
//std::cout << dbPassword << " " <<   dbUsername << " " << dbName <<  std::endl;
  if ("" != dbPassword &&
      "" != dbUsername &&
      "" != dbUsername) {
    removeHistogramsFromPage();        
    try {
      if (0 != m_histogramDB && isConnectedToHistogramDB()) {
        delete m_histogramDB;
        m_histogramDB = NULL;
      }

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
      m_message = "Successfully connected to OnlineHistDB.";
      
      if (Batch != m_presenterMode) {
        m_mainStatusBar->SetText(dbUsername.c_str(), 2);
        m_mainStatusBar->SetText(m_message.c_str(), 2);
      }

      if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }

//    statusBar()->message(tr("Successfully connected to OnlineHistDB."));
      m_histogramDB->setExcLevel(AllExceptions);
      m_histogramDB->setDebug(m_verbosity);

      if (!m_savesetPath.empty()) {
        if (0 != m_archive) { m_archive->setSavesetPath(m_savesetPath); }
      }
      if (!m_referencePath.empty()) {
        if (0 != m_archive) { m_archive->setReferencePath(m_referencePath); }
      }

      if (m_histogramDB->canwrite()) {
        m_databaseMode = ReadWrite;
      } else {
        m_databaseMode = ReadOnly;
      }
    } catch (std::string sqlException) {
      if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }
      if (Batch != m_presenterMode) {
        m_mainStatusBar->SetText(sqlException.c_str(), 2);
        new TGMsgBox(fClient->GetRoot(), this, "Login Error",
                     Form("Could not connect to the OnlineHistDB server:\n\n%s\n"
                          "This program cannot function correctly",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }

      if (0 != m_histogramDB) { delete m_histogramDB; m_histogramDB = NULL; }
      m_databaseMode = LoggedOut;
    }
  } else {
    if (Batch != m_presenterMode) {
      new TGMsgBox(fClient->GetRoot(), this, "Login Error",
                     Form("Could not connect to the OnlineHistDB server:\n"
                          "Please enter valid credentials."),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
  }
  return isConnectedToHistogramDB();
}
void PresenterMainFrame::loginToHistogramDB()
{

  if (Batch != m_presenterMode) {
    fClient->WaitFor(new LoginDialog(this, 350, 310, m_databaseMode,
                                     m_knownDatabases, m_knownDbCredentials));
  
    //  histoDBFilterComboBox->HideFrame();
  //  reconfigureGUI();
  //  refreshPagesDBListTree();
  //  refreshHistoDBListTree();
  
    reconfigureGUI();
    removeHistogramsFromPage();
  }
}
void PresenterMainFrame::logoutFromHistogramDB()
{
  if (0 != m_histogramDB && isConnectedToHistogramDB() &&
      Batch != m_presenterMode) {
    new TGMsgBox(fClient->GetRoot(), this, "Logout from Database",
                 "Do you really want to logout from Database?",
                 kMBIconQuestion, kMBYes | kMBNo, &m_msgBoxReturnCode);
    switch (m_msgBoxReturnCode) {
      case kMBNo:
        return;
    }
  }
  delete m_histogramDB;
  m_histogramDB = NULL;
  if (0 != m_analysisLib) { delete m_analysisLib; m_analysisLib = NULL; }

  m_databaseMode = LoggedOut;

//  removeHistogramsFromPage();
  m_message = "Disconnected from histogramDB";
  if (Batch != m_presenterMode) {  
      m_mainStatusBar->SetText(m_message.c_str(), 2);
    reconfigureGUI();
    removeHistogramsFromPage();
  }
  if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }
}
void PresenterMainFrame::startPageRefresh()
{
  if (false == m_loadingPage) {
    if (Batch != m_presenterMode) {
      m_message = "Refresh started.";
      m_mainStatusBar->SetText(m_message.c_str(), 2);
      if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }
    
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
void PresenterMainFrame::stopPageRefresh()
{
  disablePageRefresh();
  if (0 != m_pageRefreshTimer) {
    m_message = "Refresh stopped.";
    m_mainStatusBar->SetText(m_message.c_str(), 2);
    if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }
    if (History != m_presenterMode) {
      m_startRefreshButton->SetState(kButtonUp);
      m_viewMenu->EnableEntry(START_COMMAND);
    }
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(STOP_COMMAND);

    m_pageRefreshTimer->TurnOff();
    m_refreshingPage = false;
  }
}
void PresenterMainFrame::clearHistos()
{
  if (m_clearedHistos) {
    m_message = "Reverted to integral.";
    m_clearHistoButton->SetState(kButtonUp);
    m_viewMenu->UnCheckEntry(CLEAR_HISTOS_COMMAND);
    m_clearedHistos = false;
    disableHistogramClearing();
  } else {
    m_message = "Histograms cleared.";
    m_mainStatusBar->SetText(m_message.c_str(), 2);
    m_clearHistoButton->SetState(kButtonDown);
    m_clearedHistos = true;
    m_viewMenu->CheckEntry(CLEAR_HISTOS_COMMAND);
    enableHistogramClearing();
  }
  m_mainStatusBar->SetText(m_message.c_str(), 2);
  if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }
}
OMAlib* PresenterMainFrame::analysisLib()
{
  return m_analysisLib;
}
OnlineHistDB* PresenterMainFrame::histogramDB()
{
  if (!isConnectedToHistogramDB()) { loginToHistogramDB(); }
  return m_histogramDB;
}
DimBrowser* PresenterMainFrame::dimBrowser()
{
  return m_dimBrowser;
}

void PresenterMainFrame::listAlarmsFromHistogramDB(TGListTree* listView,
                                      const FilterCriteria & /*filterCriteria*/)
{
  if (isConnectedToHistogramDB() && 0 != listView) {

    listView->UnmapWindow();

      TGListTreeItem* treeRoot = listView->GetFirstItem();
//      deleteTreeChildrenItemsUserData(treeRoot);
      listView->DeleteChildren(treeRoot);
      listView->RenameItem(treeRoot, TString(m_dbName));
        
    try {
      m_histogramDB->getMessages(m_alarmMessageIDs);
      std::vector<int>::const_iterator m_alarmMessageIDsIt;
      for (m_alarmMessageIDsIt = m_alarmMessageIDs.begin();
           m_alarmMessageIDsIt != m_alarmMessageIDs.end();
           m_alarmMessageIDsIt++) {
        OMAMessage* message = new OMAMessage(*m_alarmMessageIDsIt, *m_histogramDB);
        if (message) {
          std::string nodeName (message->humanTime());
          nodeName.erase(nodeName.length()-1); // remove \n
          nodeName = nodeName + ": ";
          nodeName = nodeName + message->ananame();
          TGListTreeItem* treeNode = listView->AddItem(treeRoot, nodeName.c_str());
          setTreeNodeType(treeNode, message->levelString());        
          listView->SetCheckBox(treeNode, false);
          treeNode->SetUserData((void*)&(*m_alarmMessageIDsIt));
          delete message;
        }
      }
    } catch (std::string sqlException) {
        if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }
        if (Batch != m_presenterMode) {
          m_mainStatusBar->SetText(sqlException.c_str());
          new TGMsgBox(fClient->GetRoot(), this, "Database Error",
              Form("OnlineHistDB server:\n\n%s\n",
                   sqlException.c_str()),
              kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
        }
    }
//    treeRoot->SetOpen(!treeRoot->IsOpen());
    listView->MapWindow();
  } else if (!isConnectedToHistogramDB()) {
    m_message = "Alarms not read from Database.";
  }
  if (Batch != m_presenterMode) {  
    m_mainStatusBar->SetText(m_message.c_str(), 2);
    fClient->NeedRedraw(listView);
  }
  if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }
//  sortTreeChildrenItems(listView, listView->GetFirstItem());
};

void PresenterMainFrame::listHistogramsFromHistogramDB(TGListTree* listView,
  const FilterCriteria & filterCriteria, bool histograms)
{
  if (isConnectedToHistogramDB() && 0 != listView) {
    try {
      listView->UnmapWindow();
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetWaitCursor());

      m_treeNode = listView->GetFirstItem();
//      m_treeNode->SetPictures(m_databaseSourceIcon, m_databaseSourceIcon);
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
          m_histogramDB->getHistogramsWithAnalysis(NULL,
            &m_localDatabaseIDS, &m_histogramTypes);
          break;
        case AnalysisHistograms:
          m_histogramDB->getAnalysisHistograms(NULL,
            &m_localDatabaseIDS, &m_histogramTypes);
          break;
        case AllHistograms:
          m_histogramDB->getAllHistograms(NULL,
            &m_localDatabaseIDS, &m_histogramTypes);
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
          m_folderItems = TString(*m_folderIt).Tokenize(s_slash);
          m_folderItemsIt = m_folderItems->MakeIterator();
          m_treeNode = listView->GetFirstItem();
          while ((m_folderItem = m_folderItemsIt->Next())) {
            if ((listView->FindChildByName(m_treeNode,
                                          m_folderItem->GetName())) &&
                (NULL == (m_treeNode->GetUserData()))) {
              m_treeNode = listView->FindChildByName(m_treeNode,
                                                     m_folderItem->GetName());
            } else {
              if (filterCriteria != Tasks) {
                std::string userFolderData(s_FILE_URI);
                m_treeNode = listView->AddItem(m_treeNode, m_folderItem->GetName());
                char path[1024];
                m_pagesFromHistoDBListTree->GetPathnameFromItem(m_treeNode, path);
                std::string folderPath(path);
                // Drop DB url
                folderPath = folderPath.erase(0, strlen(m_pagesFromHistoDBListTree->GetFirstItem()->GetText())+1);
                userFolderData.append(folderPath);
                TObjString *objectString = new TObjString(userFolderData.c_str());
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
                std::string pageName = std::string(*m_pageIt).erase(0,
                                                   (*m_folderIt).length()+1);
                m_pageNode = listView->AddItem(m_treeNode, pageName.c_str());

                std::string userPageData(s_FILE_URI);
                char path[1024];
                m_pagesFromHistoDBListTree->GetPathnameFromItem(m_pageNode, path);
                std::string pagePath(path);
                // Drop DB url
                pagePath = pagePath.erase(0, strlen(m_pagesFromHistoDBListTree->GetFirstItem()->GetText())+1);
                userPageData.append(pagePath);
                TObjString *objectString = new TObjString(userPageData.c_str());
                m_pageNode->SetUserData(objectString);
                m_pageNode->SetDNDSource(kTRUE);
                m_pageNode->SetDNDTarget(kFALSE);
                listView->CheckItem(m_pageNode, false);
                setTreeNodeType(m_pageNode, s_PAGE);
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
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetDefaultCursor());

    } catch (std::string sqlException) {
        if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }
        if (Batch != m_presenterMode) {
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
  if (Batch != m_presenterMode) {
    m_mainStatusBar->SetText(m_message.c_str(), 2);
    sortTreeChildrenItems(listView, listView->GetFirstItem());
    fClient->NeedRedraw(listView);
  }
  if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }

}
void PresenterMainFrame::fillTreeNodeWithHistograms(TGListTree* listView,
  TGListTreeItem* node, std::vector<std::string>*  histogramList,
  std::vector<std::string>*  histogramTypes)
{
  listView->UnmapWindow();
  
  m_histogramType = histogramTypes->begin();
  for (m_histogramIt = histogramList->begin();
       m_histogramIt != histogramList->end();
       ++m_histogramIt, ++m_histogramType) {
    HistogramIdentifier histogramIdentifier =
      HistogramIdentifier(*m_histogramIt);
    m_taskNode = node;
    // Taskname
    if (listView->FindChildByName(m_taskNode,
        histogramIdentifier.histogramUTGID().c_str())) {
      m_taskNode = listView->FindChildByName(m_taskNode,
        histogramIdentifier.histogramUTGID().c_str());
    } else {
      m_taskNode = listView->AddItem(m_taskNode,
        histogramIdentifier.histogramUTGID().c_str());
      setTreeNodeType(m_taskNode, s_TASK);
    }
    listView->SetCheckBox(m_taskNode, true);
    listView->CheckItem(m_taskNode, false);
    m_taskNode->SetUserData(0);
    // Algorithm name
    m_algorithmNode = m_taskNode;
    if (listView->FindChildByName(m_algorithmNode,
                              histogramIdentifier.algorithmName().c_str())) {
      m_algorithmNode = listView->FindChildByName(m_algorithmNode,
                                histogramIdentifier.algorithmName().c_str());
    } else {
      m_algorithmNode = listView->AddItem(m_algorithmNode,
                                histogramIdentifier.algorithmName().c_str());
      setTreeNodeType(m_algorithmNode, s_ALGORITHM);
    }
    listView->SetCheckBox(m_algorithmNode, true);
    listView->CheckItem(m_algorithmNode, false);    
    m_algorithmNode->SetUserData(0);
    // Histogramset name
    m_histogramSetNode = m_algorithmNode;
    if (histogramIdentifier.isFromHistogramSet()) {
      if (listView->FindChildByName(m_histogramSetNode,
        histogramIdentifier.histogramSetName().c_str())) {
          m_histogramSetNode = listView->FindChildByName(m_histogramSetNode,
                            histogramIdentifier.histogramSetName().c_str());
      } else {
        m_histogramSetNode = listView->AddItem(m_histogramSetNode,
          histogramIdentifier.histogramSetName().c_str());
        setTreeNodeType(m_histogramSetNode, s_SET);
      }
      listView->SetCheckBox(m_histogramSetNode, true);
      listView->CheckItem(m_histogramSetNode, false);  
      m_histogramSetNode->SetUserData(0);
    }
    // Histogram name
    m_histogramNode = m_histogramSetNode;
    m_histogramIdItems = TString(histogramIdentifier.histogramName()).
                                 Tokenize(s_slash);
    m_histogramIdItemsIt = m_histogramIdItems->MakeIterator();

    while ((m_histogramIdItem = m_histogramIdItemsIt->Next())) {
      if (m_histogramIdItem != m_histogramIdItems->Last()) {
        if ((listView->FindChildByName(m_histogramNode,
                                      m_histogramIdItem->GetName()))) {
            m_histogramNode = listView->FindChildByName(m_histogramNode,
                                              m_histogramIdItem->GetName());
        } else {
          
          
          
          m_histogramNode = listView->AddItem(m_histogramNode,
                                              m_histogramIdItem->GetName());
        }
        listView->SetCheckBox(m_histogramNode, true);
        listView->CheckItem(m_histogramNode, false);
        m_histogramNode->SetUserData(0);
        setTreeNodeType(m_histogramNode, s_LEVEL);
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
//  sortTreeChildrenItems(listView, node);
  listView->MapWindow();
}
std::string PresenterMainFrame::histogramDBName()
{
  return m_dbName;
}

void PresenterMainFrame::startBenchmark(const std::string &timer)
  { m_benchmark->Start(timer.c_str()) ; }
 
void PresenterMainFrame::stopBenchmark(const std::string &timer)
  { m_benchmark->Stop(timer.c_str());}
 
void PresenterMainFrame::printBenchmark(const std::string &timer)
{
  std::cout << m_deadTasksOnPage << "," 
            << timer << ","
            << (m_benchmark->GetRealTime(timer.c_str())) << ","
            << int(dbHistosOnPage.size()) << ",";
  std::vector<DbRootHist*>::iterator timer_dbHistosOnPageIt = dbHistosOnPage.begin();
  while( timer_dbHistosOnPageIt !=  dbHistosOnPage.end() ) {
    std::cout << (*timer_dbHistosOnPageIt)->identifier() << "," 
              << int(((*timer_dbHistosOnPageIt)->rootHistogram)->GetEntries()) << ","
              << m_benchmark->GetRealTime((*timer_dbHistosOnPageIt)->identifier().c_str()) << ",";
    timer_dbHistosOnPageIt++;
  }
  std::cout << std::endl ; 
  m_benchmark->Reset(); 
}

void PresenterMainFrame::listRootHistogramsFrom(TDirectory* rootFile,
                                                std::vector<std::string> & histogramList,
                                                std::vector<std::string> & histogramTypes,
                                                std::string& taskName,
                                                pres::SavesetType savesetType) {
  
  string::size_type fileNameLength = 0;
  std::string task = taskName;
   if (pres::OfflineFile == savesetType) { 
    fileNameLength = std::string((rootFile->GetFile())->GetName()).size();
   }

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
//         std::string pathName = taskName + s_slash + key->GetName();
         std::string pathName = taskName + s_slash + key->GetName();
         listRootHistogramsFrom(subdirectory, histogramList, histogramTypes,
                                pathName, savesetType);
         tmpdir->cd();
         continue;
      }    
      if (std::string(key->GetClassName()) == "TH1D") {
        type = s_pfixMonH1D;
        if (pres::OfflineFile == savesetType) {
          histogramIDStream << ((TH1D*)key->ReadObj())->GetName();
        } else {
          histogramIDStream << s_pfixMonH1D << s_slash << taskName << s_slash << key->GetName();
        }        
      } else if (std::string(key->GetClassName()) == "TH2D") {
        type = s_pfixMonH2D;
        if (pres::OfflineFile == savesetType) {        
          histogramIDStream << ((TH2D*)key->ReadObj())->GetName();
        } else {
          histogramIDStream << s_pfixMonH1D << s_slash << taskName << s_slash << key->GetName();
        }
      } else if (std::string(key->GetClassName()) == "TProfile") {
        type = s_pfixMonProfile;
        if (pres::OfflineFile == savesetType) {        
          histogramIDStream << ((TProfile*)key->ReadObj())->GetName();
        } else {
          histogramIDStream << s_pfixMonH1D << s_slash << taskName << s_slash << key->GetName();
        }        
        
      }
      std::string histogramID = histogramIDStream.str();
      if (pres::OfflineFile == savesetType) {
        if (histogramID.size() > fileNameLength) {
          histogramID = histogramID.erase(0, fileNameLength+1);
        }
      }
      if (std::string(key->GetClassName()) == "TH1D" ||
          std::string(key->GetClassName()) == "TH2D" ||
          std::string(key->GetClassName()) == "TProfile") {
      if (pres::OfflineFile == savesetType) {        
          histogramID = type + s_slash + taskName + s_slash + histogramID;
      }
      histogramTypes.push_back(type);
      histogramList.push_back(histogramID);
      if (m_verbosity >= Verbose) {
        std::cout << "Found: " << histogramID << std::endl;
      }
    
   }
   histogramIDStream.str("");
   }
}

void PresenterMainFrame::setTreeNodeType(TGListTreeItem* node,
                                         const std::string & type)
{
  const TGPicture*  m_icon;
  if (s_H1D == type || 
      s_pfixMonH1D == type) {
    m_icon = m_iconH1D;
  } else if (s_H2D == type ||
      s_pfixMonH2D == type) {
    m_icon = m_iconH2D;
  } else if (s_P1D == type ||
             s_pfixMonProfile == type ||
             s_HPD == type ||
             s_P2D == type ) {
    m_icon = m_iconProfile;
  } else if (s_CNT == type) {
    m_icon = m_iconCounter;
  } else if (s_PAGE == type) {
    m_icon = m_iconPage;
  } else if (s_TASK == type) {
    m_icon = m_iconTask;
  } else if (s_ALGORITHM == type) {
    m_icon = m_iconAlgorithm;
  } else if (s_SET == type) {
    m_icon = m_iconSet;
  } else if (s_LEVEL == type) {
    m_icon = m_iconLevel;
  } else if (s_ALARM == type) {
    m_icon = m_iconAlarm;
  } else if (s_WARNING == type) {
    m_icon = m_iconWarning;
  } else if (s_INFO == type) {
    m_icon = m_iconInfo;                
  } else {
    m_icon = m_iconQuestion;
  }
  if (0 != node) { node->SetPictures(m_icon, m_icon); }
}
void PresenterMainFrame::checkedTreeItems(TGListTree* selected,
                                          TGListTree* treeList)
{
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
void PresenterMainFrame::checkedTreeItemsChildren(TGListTreeItem* node,
                                                  TGListTree* selected)
{
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
void PresenterMainFrame::collapseTreeChildrenItems(TGListTree* treeList,
                                                   TGListTreeItem* node)
{
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
void PresenterMainFrame::collapseTreeChildrenItemsChildren(
  TGListTree* treeList, TGListTreeItem* node)
{
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
void PresenterMainFrame::sortTreeChildrenItems(TGListTree* treeList,
                                               TGListTreeItem* node)
{
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
void PresenterMainFrame::sortTreeChildrenItemsChildren(TGListTree* treeList,
                                                       TGListTreeItem* node)
{
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
void PresenterMainFrame::deleteTreeChildrenItemsUserData(TGListTreeItem* node)
{
  if (0 != node && 0 != (node->GetUserData())) {
    delete (TObjString*)(node->GetUserData());
    node->SetUserData(NULL);
  }
  if (0 != node && 0 != (node->GetFirstChild())) {
    deleteTreeChildrenItemsUserDataChildren(node->GetFirstChild());
  }
}
void PresenterMainFrame::deleteTreeChildrenItemsUserDataChildren(TGListTreeItem* node)
{
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
void PresenterMainFrame::setStatusBarText(const char* text, int slice)
{
  if (Batch != m_presenterMode) {
    m_mainStatusBar->SetText(text, slice);
  }
}
void PresenterMainFrame::editHistogramProperties()
{
  bool referenceOverlay(false);
  if (m_referencesOverlayed) {
    disableReferenceOverlay();
    referenceOverlay = true;
  }
  DbRootHist* selectedDbHisto = selectedDbRootHistogram();
  if (0 != selectedDbHisto) {
    if (0 != selectedDbHisto) {
      TH1* selectedHisto = selectedDbHisto->rootHistogram;
      if (0 != selectedHisto) {
        selectedHisto->DrawPanel();
      }
    }
  }
  if (referenceOverlay) { enableReferenceOverlay(); }  
}
void PresenterMainFrame::inspectHistogram()
{
  bool referenceOverlay(false);
  if (m_referencesOverlayed) {
    disableReferenceOverlay();
    referenceOverlay = true;
  }

  DbRootHist* selectedDbHisto = selectedDbRootHistogram();
  if (0 != selectedDbHisto) {
    if (0 != selectedDbHisto) {
      TH1* selectedHisto = selectedDbHisto->rootHistogram;
      if (0 != selectedHisto) {
        TCanvas* zoomCanvas =  new TCanvas();
        if (zoomCanvas) {
          zoomCanvas->cd();
      //    zoomCanvas->SetTitle(selectedHisto->GetTitle());
          selectedDbHisto->rootHistogram->DrawCopy();
          zoomCanvas->ToggleEventStatus();
          zoomCanvas->ToggleAutoExec();
          zoomCanvas->ToggleToolBar();
          zoomCanvas->ToggleEditor();
        }
      }
    }
  }
  if (referenceOverlay) { enableReferenceOverlay(); } 
}
void PresenterMainFrame::histogramDescription()
{
  if (isConnectedToHistogramDB()) {
    
    m_currentPageHistogramHelp.clear();
    std::stringstream currentPageHistogramHelp;
    
    DbRootHist* histogram = selectedDbRootHistogram();
    if (0 != histogram) {
      
      currentPageHistogramHelp << "Title:\t\t" << (histogram->onlineHistogram())->htitle() << std::endl << std::endl
                               << "Name:\t\t" <<  (histogram->onlineHistogram())->hname() << std::endl
                               << "Type:\t\t" <<  (histogram->onlineHistogram())->hstype() << std::endl
                               << "Task:\t\t" <<  (histogram->onlineHistogram())->task() << std::endl
                               << "Algorithm:\t" <<  (histogram->onlineHistogram())->algorithm() << std::endl
                               << "ROOT name:\t" <<  (histogram->onlineHistogram())->rootName()  << std::endl
                               << "DIM in DB:\t" <<  (histogram->onlineHistogram())->dimServiceName() << std::endl << std::endl
                               << "Description:" << std::endl << (histogram->onlineHistogram())->descr() << std::endl
                               << std::endl;

      m_currentPageHistogramHelp = currentPageHistogramHelp.str().c_str();
            
      TRootHelpDialog *hd = new TRootHelpDialog(this, "Histogram Description", 600, 400);
      hd->SetText(Form("%s", m_currentPageHistogramHelp.c_str()));
      hd->Popup();
    }
  } else {
    if (Batch != m_presenterMode) {
      new TGMsgBox(fClient->GetRoot(), this, "Description Unavailable",
                   "Description Unavailable, not logged in!",
                   kMBIconExclamation, kMBClose, &m_msgBoxReturnCode);
    }
  } 
}
void PresenterMainFrame::inspectPage()
{
  TCanvas* inspectCanvas =  new TCanvas();
  inspectCanvas->cd();
  inspectCanvas->SetTitle("Inspect Page");
  editorCanvas->DrawClonePad();
  inspectCanvas->ToggleEventStatus();
  inspectCanvas->ToggleAutoExec();
  inspectCanvas->ToggleToolBar();
  inspectCanvas->ToggleEditor();
}
void PresenterMainFrame::reconfigureGUI()
{
  removeHistogramsFromPage();
  UInt_t current_width  = 0;
  UInt_t current_height = 0;
  current_width = this->GetWidth();
  current_height = this->GetHeight();
  if (isConnectedToHistogramDB()) {
    refreshPagesDBListTree();
    listAlarmsFromHistogramDB(m_alarmHistogramTreeList, AllHistograms);
    enableAutoCanvasLayoutBtn();
    if (canWriteToHistogramDB() && 
        ((EditorOnline == m_presenterMode) ||
         (EditorOffline == m_presenterMode))) {
      showDBTools(ReadWrite);
    } else {
      showDBTools(ReadOnly);
    }
  } else {
    hideDBTools();
  }

  if (Online == m_presenterMode) {
    if (m_historyTrendPlots) { toggleHistoryPlots(); }
    
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
//    m_historyIntervalQuickButton->SetState(kButtonDisabled);
//    m_partitionSelectorQuickButton->SetState(kButtonEngaged);    
//    m_partitionSelectorQuickButton->SetState(kButtonUp);

    // hide refreshHistoDBListTree
//    if (!isConnectedToHistogramDB()) { refreshHistogramSvcList(); }
//    refreshHistogramSvcList();
  } else if (History == m_presenterMode) {
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
//    m_historyIntervalComboBox->SetState(kButtonUp);
//    m_historyIntervalQuickButton->SetState(kButtonEngaged);
//    m_historyIntervalQuickButton->SetState(kButtonUp);
//    m_partitionSelectorQuickButton->SetState(kButtonDisabled);

//    m_historyPlotsButton->SetState(kButtonEngaged);
    m_historyPlotsButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(HISTORY_PLOTS_COMMAND);

//    m_overlayReferenceHistoButton->SetState(kButtonEngaged);
    m_overlayReferenceHistoButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
    
    m_pickReferenceHistoButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(PICK_REFERENCE_HISTO_COMMAND);
    
    m_viewMenu->DisableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);

    // hide refreshHistoDBListTree
    if ((Online == m_prevPresenterMode) &&
        (m_presenterMode != m_prevPresenterMode)) {
      partitionSelectorComboBoxHandler(0);
    }        
  } else if (EditorOnline == m_presenterMode) {
    stopPageRefresh();
    if (m_clearedHistos) { clearHistos(); }
    if (m_referencesOverlayed) { toggleReferenceOverlay(); }
    if (m_historyTrendPlots) { toggleHistoryPlots(); }
    // enable play/stop/reset!
    m_toolMenu->CheckEntry(PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_OFFLINE_MODE_COMMAND);    
    m_toolMenu->UnCheckEntry(ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(OFFLINE_MODE_COMMAND);
    
    m_previousIntervalButton->SetState(kButtonDisabled);
    m_nextIntervalButton->SetState(kButtonDisabled);
    m_rightMiscFrame->MapWindow();
    m_rightVerticalSplitter->MapWindow();
    
    if ((Online == m_prevPresenterMode ||
        History == m_prevPresenterMode) &&
        m_presenterMode != m_prevPresenterMode ) {
      refreshHistogramSvcList(s_withTree);
    }
    
    if (isConnectedToHistogramDB()) { refreshHistoDBListTree(); }
//    m_clearHistoButton->SetState(kButtonUp);
//    m_viewMenu->EnableEntry(CLEAR_HISTOS_COMMAND);

//    m_startRefreshButton->SetState(kButtonDisabled);
//    m_viewMenu->DisableEntry(START_COMMAND);
//    m_stopRefreshButton->SetState(kButtonDisabled);
//    m_viewMenu->DisableEntry(STOP_COMMAND);
//    m_clearHistoButton->SetState(kButtonDisabled);
//    m_viewMenu->DisableEntry(CLEAR_HISTOS_COMMAND);

      m_startRefreshButton->SetState(kButtonUp);
      m_viewMenu->EnableEntry(START_COMMAND);
      m_stopRefreshButton->SetState(kButtonDisabled);
      m_viewMenu->DisableEntry(STOP_COMMAND);
      m_clearHistoButton->SetState(kButtonUp);
      m_viewMenu->EnableEntry(CLEAR_HISTOS_COMMAND);

//      m_histoSvcTreeContextMenu->EnableEntry(M_SetDimSourceFromTree);

      m_historyIntervalComboBox->SetEnabled(true);
//    m_historyIntervalQuickButton->SetState(kButtonEngaged);
//    m_historyIntervalQuickButton->SetState(kButtonUp);
//    m_partitionSelectorQuickButton->SetState(kButtonEngaged);
//    m_partitionSelectorQuickButton->SetState(kButtonUp);

      m_historyPlotsButton->SetState(kButtonDisabled);
      m_viewMenu->DisableEntry(HISTORY_PLOTS_COMMAND);
    
//    m_overlayReferenceHistoButton->SetState(kButtonDisabled);
//    m_viewMenu->DisableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
//    m_overlayReferenceHistoButton->SetState(kButtonEngaged);
      m_overlayReferenceHistoButton->SetState(kButtonUp);
      m_viewMenu->EnableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
    
    // show refreshHistoDBListTree
  } else if (EditorOffline == m_presenterMode) {
    stopPageRefresh();
    if (m_clearedHistos) { clearHistos(); }
    if (m_referencesOverlayed) { toggleReferenceOverlay(); }
    if (m_historyTrendPlots) { toggleHistoryPlots(); }
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
    
//    m_histoSvcTreeContextMenu->DisableEntry(M_SetDimSourceFromTree);
    
    m_historyIntervalComboBox->SetEnabled(true);
//  m_historyIntervalQuickButton->SetState(kButtonEngaged);
//  m_historyIntervalQuickButton->SetState(kButtonUp);
//    m_partitionSelectorQuickButton->SetState(kButtonEngaged);
//    m_partitionSelectorQuickButton->SetState(kButtonUp);

    m_historyPlotsButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(HISTORY_PLOTS_COMMAND);    
    
    m_overlayReferenceHistoButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
    
    if ((Online == m_prevPresenterMode ||
        History == m_prevPresenterMode) &&
        m_presenterMode != m_prevPresenterMode ) {
      refreshHistogramSvcList(s_withTree);
      partitionSelectorComboBoxHandler(0);
    }
  }

//  partitionSelectorComboBoxHandler(0);
  fClient->NeedRedraw(this);
  this->Resize(current_width,current_height);
  DoRedraw();
//  DoRedraw();
}
void PresenterMainFrame::hideDBTools()
{
  m_histoDBFilterComboBox->SetEnabled(false);

  m_histoDBCanvasViewPort->HideFrame(m_databaseHistogramTreeList);
  m_pagesFromHistoDBViewPort->HideFrame(m_pagesFromHistoDBListTree);
//  m_databaseHistogramTreeList->SetToolTipText(
//      std::string("Please login to DB for Histogram list").c_str(),
//      1, 1, 250);
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
void PresenterMainFrame::showDBTools(DatabaseMode databasePermissions)
{
  m_logoutButton->SetState(kButtonUp);
  m_loginButton->SetState(kButtonDisabled);

  m_fileMenu->DisableEntry(LOGIN_COMMAND);
  m_fileMenu->EnableEntry(LOGOUT_COMMAND);

  m_histoDBFilterComboBox->SetEnabled(true);
//  m_databaseHistogramTreeList->SetToolTipText(
//    std::string("Please select Histogram from list").c_str(), 1, 1, 20);
  m_databaseHistogramTreeList->SetBackgroundColor(GetWhitePixel());
  m_histoDBCanvasViewPort->ShowFrame(m_databaseHistogramTreeList);
  m_pagesFromHistoDBViewPort->ShowFrame(m_pagesFromHistoDBListTree);

  if (ReadWrite == databasePermissions) {
    m_histoSvcTreeContextMenu->EnableEntry(M_AddHistoToDB_COMMAND);
//    m_savePageToDatabaseButton->SetState(kButtonEngaged);
    m_savePageToDatabaseButton->SetState(kButtonUp);
//    m_pickReferenceHistoButton->SetState(kButtonEngaged);
    m_pickReferenceHistoButton->SetState(kButtonUp);
    m_fileMenu->EnableEntry(SAVE_PAGE_TO_DB_COMMAND);
    m_pagesContextMenu->EnableEntry(M_Move_COMMAND);
    m_pagesContextMenu->EnableEntry(M_DeletePage_COMMAND);
    m_pagesContextMenu->EnableEntry(M_DeleteFolder_COMMAND);
    m_histoDBContextMenu->EnableEntry(M_SetHistoPropertiesInDB_COMMAND);
    m_histoDBContextMenu->EnableEntry(M_DeleteDBHisto_COMMAND);
    
//    m_pickReferenceHistoButton->SetState(kButtonEngaged);
    m_pickReferenceHistoButton->SetState(kButtonUp);
    m_editMenu->EnableEntry(PICK_REFERENCE_HISTO_COMMAND);
    m_editMenu->EnableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);

  } else if (ReadOnly == databasePermissions) {
    m_savePageToDatabaseButton->SetState(kButtonDisabled);
    m_fileMenu->DisableEntry(SAVE_PAGE_TO_DB_COMMAND);
    m_histoSvcTreeContextMenu->DisableEntry(M_AddHistoToDB_COMMAND);
    m_pagesContextMenu->DisableEntry(M_Move_COMMAND);
    m_pagesContextMenu->DisableEntry(M_DeletePage_COMMAND);
    m_pagesContextMenu->DisableEntry(M_DeleteFolder_COMMAND);
    m_histoDBContextMenu->DisableEntry(M_SetHistoPropertiesInDB_COMMAND);
    m_histoDBContextMenu->DisableEntry(M_DeleteDBHisto_COMMAND);

    m_pickReferenceHistoButton->SetState(kButtonDisabled);    
    m_editMenu->DisableEntry(PICK_REFERENCE_HISTO_COMMAND);    
    m_editMenu->DisableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);
  }
  m_histoSvcTreeContextMenu->DisableEntry(M_AddHistoToPage_COMMAND);

  enablePageLoading();
}
void PresenterMainFrame::refreshHistoDBListTree() {
  if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }
  listHistogramsFromHistogramDB(m_databaseHistogramTreeList,
    static_cast<FilterCriteria>(m_histoDBFilterComboBox->GetSelected()),
    s_withHistograms);
}
void PresenterMainFrame::refreshPagesDBListTree() {
  listHistogramsFromHistogramDB(m_pagesFromHistoDBListTree, FoldersAndPages,
                      s_withoutHistograms);
}
void PresenterMainFrame::refreshHistogramSvcList(bool tree) {
if (Batch != m_presenterMode) {
  gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetWaitCursor());
    if (m_refreshingPage) {
      stopPageRefresh();
      m_resumePageRefreshAfterLoading = true;
    }
  
    m_knownHistogramServices.clear();
    m_histogramTypes.clear();
  
    if (Online == m_presenterMode || EditorOnline == m_presenterMode) {
      
      m_dimBrowserDock->SetWindowName("DIM Histogram Browser");
      m_histoSvcBrowserGroupFrame->SetTitle("DIM Histogram Browser");    
      
      m_candidateDimServices.clear();
      m_knownOnlinePartitionList->Delete();
      
      char *dimService; 
      char *dimFormat;
      int dimType;
    
      std::string serviceType = s_Mon;
      serviceType.append(s_DimWildcard);
      m_dimBrowser->getServices(serviceType.c_str());
      while((dimType = m_dimBrowser->getNextService(dimService, dimFormat))) {    
        TString dimMon(dimService);        
        if (dimMon.BeginsWith(s_pfixMonH1D.c_str()) ||
            dimMon.BeginsWith(s_pfixMonH2D.c_str()) ||
            dimMon.BeginsWith(s_pfixMonProfile.c_str())) {
          m_candidateDimServices.push_back(std::string(dimService));
        }   
      }
      
      serviceType = s_DimWildcard + s_eff_monRate + s_DimWildcard;
      m_dimBrowser->getServices(serviceType.c_str());
      while((dimType = m_dimBrowser->getNextService(dimService, dimFormat))) {    
        TString dimMon(dimService);
        TObjArray* dimHltCntMatchGroup = 0;
        dimHltCntMatchGroup = s_DimHltCntRegexp.MatchS(dimFormat);
        if ((true == dimMon.Contains(s_adder.c_str())) &&
            (true == dimMon.Contains(s_eff_monRate.c_str())) &&
            (false == dimHltCntMatchGroup->IsEmpty())) {
          if (false == dimMon.BeginsWith(s_CNT.c_str())) {
            dimMon.Prepend(s_slash.c_str());
            dimMon.Prepend(s_CNT.c_str());
          }
          HistogramIdentifier histogramIdentifier = HistogramIdentifier(dimMon.Data());
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
//      m_dimBrowser->getServices("*/gauchocomment");
//      TObjArray* dimCNTMatchGroup = 0;
//      DimBrowser* dimBrowser = new DimBrowser();
//      while((dimType = m_dimBrowser->getNextService(dimService, dimFormat))) { 
//        TString dimCNT(dimService);
//        dimCNT.Remove(dimCNT.Length() - s_gauchocomment.length(),s_gauchocomment.length());
//  
//        if (false == dimCNT.BeginsWith(s_H1D.c_str()) &&
//            false == dimCNT.BeginsWith(s_H2D.c_str()) &&
//            false == dimCNT.BeginsWith(s_P1D.c_str()) &&
//            false == dimCNT.BeginsWith(s_HPD.c_str()) &&
//            false == dimCNT.BeginsWith(s_P2D.c_str()) ) {
//  
//      dimBrowser->getServices(dimCNT.Data());
//      if ((dimType = dimBrowser->getNextService(dimService, dimFormat))) {    
//        dimCNTMatchGroup = s_DimCNTRegexp.MatchS(dimFormat);
//        if (false == dimCNTMatchGroup->IsEmpty()) {
//          dimCNT.Prepend(s_slash.c_str());
//          dimCNT.Prepend(s_CNT.c_str());
//          m_candidateDimServices.push_back(std::string(dimCNT.Data()));
//        }
//      }       
//        }
//        if (dimCNTMatchGroup) {
//          dimCNTMatchGroup->Delete();
//          delete dimCNTMatchGroup;
//          dimCNTMatchGroup = 0;
//        }      
//      }
//      if (dimBrowser) {delete dimBrowser; dimBrowser = NULL;}
          
      if (0 != m_histoSvcListTree && 0 != m_dimBrowser) {
        const int nDimServers = m_dimBrowser->getServers();
        
        if (m_verbosity >= Verbose) {
          std::cout << "nDimServers:\t" << nDimServers << std::endl;
        }
    
        TString dimServerName;
        TString dimServerNodeName;
        TString dimServiceName;
        TString statusMessage;
    
        // If some servers found, discover what services are available
        if (nDimServers > 0) {
          const char* dimDnsServerNode = DimClient::getDnsNode();
    
          if (m_verbosity >= Verbose) {
            std::cout << std::endl << "DNS: " << dimDnsServerNode << std::endl;
          }
    
          setStatusBarText(dimDnsServerNode, 2);
          if (s_withTree == tree) {
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
            HistogramIdentifier histogramService = HistogramIdentifier(*m_candidateDimServicesIt);
            if (histogramService.isPlausible()) {
              std::string candidateDimService(*m_candidateDimServicesIt);
              if (s_withTree == tree) {          
                if (s_CNT == histogramService.histogramType()) {
                  candidateDimService = candidateDimService.erase(0, s_CNT.length()).erase(0, s_slash.length());
                }
                
                m_knownHistogramServices.push_back(candidateDimService);
                m_histogramTypes.push_back(histogramService.histogramType());
              }
              if (!m_knownOnlinePartitionList->FindObject(histogramService.partitionName().c_str())) {
                TObjString* partitionName = new TObjString(histogramService.partitionName().c_str());
                m_knownOnlinePartitionList->Add(partitionName);
              }
              if (m_verbosity >= Verbose) {
                std::cout << "\t\t\t|_ " << candidateDimService << std::endl;
              }
            } else {
              if (Batch != m_presenterMode) {
                new TGMsgBox(fClient->GetRoot(), this, "DIM Service name error",
                  Form("The DIM servicename\n%s\n does not appear to follow the" \
                    "convention\nPlease use the following format:\n" \
                    "TYP/UTGID/Algorithmname/Histogramname\n where the UTGID " \
                    "normally has the following format:\n" \
                    "partition_node_taskname_instance", dimService),
                  kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
              }
            }
            ++m_candidateDimServicesIt;
          }
  //        partitionSelectorComboBoxHandler(0);       
        }
      } else {
        if (s_withTree == tree) {
          m_histoSvcListTree->RenameItem(m_histoSvcListTree->GetFirstItem(), "No DIM");
        }
        if (m_verbosity >= Verbose) {
          std::cout << "Sorry, no DIM server found." << std::endl;
        }
        if (Batch != m_presenterMode) {
          new TGMsgBox(fClient->GetRoot(), this, "DIM Error",
                       "Sorry, no DIM server found", kMBIconExclamation,
                       kMBOk, &m_msgBoxReturnCode);
        }
      }
    } else if (EditorOffline == m_presenterMode) {
      if (s_withTree == tree && !m_savesetFileName.empty()) {
        
        m_dimBrowserDock->SetWindowName("ROOT Histogram Browser");
        m_histoSvcBrowserGroupFrame->SetTitle("ROOT Histogram Browser");
              
        TGListTreeItem* node = m_histoSvcListTree->GetFirstItem();
        m_histoSvcListTree->RenameItem(node, m_savesetFileName.c_str());
        
        (m_histoSvcListTree->GetFirstItem())->SetPictures(m_iconROOT, m_iconROOT);
        
        deleteTreeChildrenItemsUserData(node);
        m_histoSvcListTree->DeleteChildren(node);
        
        TFile rootFile(m_savesetFileName.c_str());
        
        if (rootFile.IsZombie() && (Batch != m_presenterMode)) {
          new TGMsgBox(fClient->GetRoot(), this, "File Error",
                       "Sorry, file specified not found", kMBIconExclamation,
                       kMBOk, &m_msgBoxReturnCode);
        } else {        
          std::string taskName(m_archive->taskNameFromFile(std::string(rootFile.GetName())));
          pres::SavesetType savesetType = pres::OfflineFile;                
          TObjArray* fileNameMatchGroup = 0;
          
          fileNameMatchGroup = s_fileDateRegexp.MatchS(rootFile.GetName());
          if (false == fileNameMatchGroup->IsEmpty()) {
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
            fileNameMatchGroup = s_offlineJobRegexp.MatchS(rootFile.GetName());        
            if (false == fileNameMatchGroup->IsEmpty()) {
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
              fileNameMatchGroup = s_fileRunRegexp.MatchS(rootFile.GetName());        
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
        rootFile.Close();       
      }
    }
    if (s_withTree == tree) {
      fillTreeNodeWithHistograms(m_histoSvcListTree,
                                 m_histoSvcListTree->GetFirstItem(),
                                 &m_knownHistogramServices,
                                 &m_histogramTypes);        
      sortTreeChildrenItems(m_histoSvcListTree, m_histoSvcListTree->GetFirstItem());
      fClient->NeedRedraw(m_histoSvcListTree);   
    }     
    Resize();DoRedraw();
    if (m_resumePageRefreshAfterLoading) { startPageRefresh(); }  
  gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetDefaultCursor());
  }
}
void PresenterMainFrame::partitionSelectorComboBoxHandler(int partitionNumber)
{
  if (0 == partitionNumber) {
    m_partitionSelectorComboBox->RemoveEntries(1,m_partitionSelectorComboBox->GetNumberOfEntries());
    bool updateList(false);
    int foundLHCbPartitionId(0);
    int id = 1;
    TIterator* partitionIt = 0;
  
    if (((History == m_presenterMode) ||
         (EditorOffline == m_presenterMode))
         && 0 != m_archive) {
      updateList = true;
      std::vector<std::string> m_historyPartitionList;
      std::vector<std::string>::const_iterator m_historyPartitionListIt;
      m_knownHistoryPartitionList->Delete();
      
      m_historyPartitionList = m_archive->listPartitions();

      m_historyPartitionListIt = m_historyPartitionList.begin();
      while (m_historyPartitionListIt != m_historyPartitionList.end()) {
        if (!m_knownHistoryPartitionList->FindObject((*m_historyPartitionListIt).c_str())) {
          TObjString* partitionName = new TObjString((*m_historyPartitionListIt).c_str());
          m_knownHistoryPartitionList->Add(partitionName);
        }
        m_historyPartitionListIt++;
      }
      partitionIt = m_knownHistoryPartitionList->MakeIterator();
      
    } else if (Online == m_presenterMode ||
               EditorOnline == m_presenterMode) {
      updateList = true;              
      refreshHistogramSvcList(s_withoutTree);
      partitionIt = m_knownOnlinePartitionList->MakeIterator();
    }
    if (updateList) {
      int idLimit(550);   // directories to avoid deep nesting troubles
      TObject *obj = 0;
      while ((obj = partitionIt->Next())) {
        if (obj) {
          m_partitionSelectorComboBox->AddEntry(obj->GetName(), id);
          if (id < idLimit) {
            if (0 == s_lhcbPartionName.CompareTo(obj->GetName(),
                                                 TString::kIgnoreCase)) {
              foundLHCbPartitionId = id;            
            }
            id++;          
          }
        }
      }
    }
    m_partitionSelectorComboBox->SortByName();
    if (partitionIt) { delete partitionIt; partitionIt = 0; }
    if (0 != foundLHCbPartitionId) {
      m_partitionSelectorComboBox->Select(id, true);
      if (m_verbosity >= Verbose) {
        std::cout << "Found default LHCb partition: " <<
                      m_currentPartition << std::endl;
      }
    } else if (0 < id) {
      m_partitionSelectorComboBox->Select(id, true);
      if (m_verbosity >= Verbose) {
         std::cout << "Defaulting to partition: " << m_currentPartition <<
                       std::endl;
      }
    }
  } else {
    std::string partition_entry(dynamic_cast<TGTextLBEntry*>(m_partitionSelectorComboBox->GetSelectedEntry())->GetText()->GetString());    
    m_currentPartition = partition_entry;  
  }
  if (isConnectedToHistogramDB() && (false == m_currentPageName.empty()) && (false == m_loadingPage)) {
    loadSelectedPageFromDB(m_currentPageName, s_startupFile, m_savesetFileName, s_noPageHistory);
  }
}
void PresenterMainFrame::clickedHistoSvcTreeItem(TGListTreeItem* node,
                                            EMouseButton btn,
                                            int x, int y) {
  if (0 != node && btn == kButton3) {
    m_histoSvcTreeContextMenu->PlaceMenu(x, y, 1, 1);
  }
}

void PresenterMainFrame::clickedHistoDBTreeItem(TGListTreeItem* node,
                                                EMouseButton btn,
                                                int x, int y) {
  if (0 != node && btn == kButton3) {
    m_histoDBContextMenu->PlaceMenu(x, y, 1, 1);
  }
}
void PresenterMainFrame::clickedPageTreeItem(TGListTreeItem* node,
                                             EMouseButton btn,
                                             int x, int y) {
  if ((0 != node) && (kButton3 == btn)) {
    m_pagesContextMenu->PlaceMenu(x, y, 1, 1);
  } else if (// (kButton1Double == btn) &&
             (0 != node) &&
             (NULL == node->GetFirstChild()) &&
             ((EditorOnline != m_presenterMode) || (EditorOffline != m_presenterMode))) {
    m_currentPageName = selectedPageFromDbTree();
    if (!m_currentPageName.empty() && (false == m_loadingPage)) {
      loadSelectedPageFromDB(m_currentPageName, s_startupFile, m_savesetFileName, s_previousPageToHistory);
    }     

  } else if (0 != node &&
             NULL == node->GetFirstChild() &&
             ((EditorOnline == m_presenterMode) || (EditorOffline == m_presenterMode))) {
    m_pagesContextMenu->PlaceMenu(x, y, 1, 1);
  }
}
void PresenterMainFrame::clickedAlarmTreeItem(TGListTreeItem* node,
                                             EMouseButton btn,
                                             int x, int y) {

  if (0 != node && btn == kButton3) {
    m_alarmDBContextMenu->PlaceMenu(x, y, 1, 1);
  } else if (0 != node &&
             NULL == node->GetFirstChild() &&
             isConnectedToHistogramDB()) {
    int id = selectedAlarmFromDbTree();
    if (id !=-1) {
      loadSelectedAlarmFromDB(id);
    }
  }
}
void PresenterMainFrame::addHistoToHistoDB()
{
  disableAutoCanvasLayoutBtn();
  disableHistogramClearing();

  TGListTree* list = new TGListTree();
  checkedTreeItems(list, m_histoSvcListTree);

//  m_histogramDB->declareSubSystem("HLT");
//  m_histogramDB->declareTask("Adder","HLT");
//  m_histogramDB->declareTask("GauchoJob","HLT");
//  m_histogramDB->declareTask("Moore","HLT");

  TGListTreeItem* currentNode;
  currentNode = list->GetFirstItem();
  while (0 != currentNode) {
    try {
      if (0 != m_histogramDB) {

        TString histoName = (*static_cast<TObjString*>(currentNode->GetUserData())).GetString();
        if (histoName.BeginsWith(s_FILE_URI)) {histoName.Remove(0, s_FILE_URI.length()); }
        HistogramIdentifier histogramService = HistogramIdentifier(std::string(histoName));

        m_histogramDB->declareHistogram(histogramService.taskName(),
                                        histogramService.algorithmName(),
                                        histogramService.histogramName(),
                                        ((OnlineHistDBEnv::HistType)histogramService.dbHistogramType()));
      }
    } catch (std::string sqlException) {
      if (m_verbosity >= Verbose) { std::cout << sqlException; }
      if (Batch != m_presenterMode) {
        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("OnlineHistDB server:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }
    }
    currentNode = currentNode->GetNextSibling();
  }
  try {
    if (0 != m_histogramDB) {

      m_histogramDB->commit();
      refreshHistoDBListTree();
    }
  } catch (std::string sqlException) {
    if (m_verbosity >= Verbose) { std::cout << sqlException; }
    if (Batch != m_presenterMode) {
      new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                   Form("OnlineHistDB server:\n\n%s\n",
                        sqlException.c_str()),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
  }
  list->Delete();
  delete list;
  list = NULL;

  m_histoSvcListTree->CheckAllChildren(m_histoSvcListTree->GetFirstItem(),
                                     s_uncheckTreeItems);
//  fClient->NeedRedraw(m_histoSvcListTree);
//  Resize();DoRedraw();
  enableAutoCanvasLayoutBtn();
}
void PresenterMainFrame::addHistoToPage(const std::string& histogramUrl,  pres::ServicePlotMode overlapMode)
{
  HistogramIdentifier histogramID = HistogramIdentifier(histogramUrl);
    int newHistoInstance = 0;
    // see if the histogram object exists already
    std::vector<DbRootHist*>::iterator inst_dbHistosOnPageIt;
    for (inst_dbHistosOnPageIt = dbHistosOnPage.begin();
        inst_dbHistosOnPageIt != dbHistosOnPage.end();
        inst_dbHistosOnPageIt++) {
      int existingHistoInstance = (*inst_dbHistosOnPageIt)->instance();

      if (0 == (histogramID.histogramIdentifier()).compare((*inst_dbHistosOnPageIt)->identifier()) &&
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
    std::string currentPartition(m_currentPartition);
    
    if (isConnectedToHistogramDB() &&
        (invisible != overlapMode) ){
      histogramDB = m_histogramDB;
      TString dimMon(histogramUrl);
      if (false == (dimMon.BeginsWith(s_adder.c_str()) &&
                    dimMon.EndsWith(s_eff_TCK.c_str()))) {
        onlineHistogram = m_histogramDB->getHistogram(dimMon.Data());
      }
    } else {
      histogramDB = NULL;
      onlineHistogram = NULL;
    }              
    if ((History == m_presenterMode) || (EditorOffline == m_presenterMode)) {
      dimBrowser = NULL;
    } else if ((Online == m_presenterMode) || 
               (EditorOnline == m_presenterMode) ||
               (Batch == m_presenterMode)) {
      dimBrowser = m_dimBrowser;
      if ( false == isConnectedToHistogramDB() ) {
        currentPartition = histogramUrl;
      }
    }

    DbRootHist* dbRootHist = getPageHistogram(this,
                                          histogramUrl,
                                          currentPartition,
                                          2, newHistoInstance,
                                          histogramDB,
                                          analysisLib(),
                                          onlineHistogram,
                                          m_verbosity,
                                          dimBrowser);

     dbRootHist->setOverlapMode(overlapMode);     
     if ((0 != m_archive) && isConnectedToHistogramDB() &&
          (false == m_savesetFileName.empty()) &&
          ((History == m_presenterMode) || (EditorOffline == m_presenterMode))) {
          m_archive->fillHistogram(dbRootHist,
                                 s_startupFile,
                                 m_savesetFileName);
      }
                                              
  if ( (false == isConnectedToHistogramDB())  &&
       (invisible != overlapMode) ) {
    // Set Properties
    TString paintDrawXLabel = bulkHistoOptions.m_xLabel;
    TString paintDrawYLabel = bulkHistoOptions.m_yLabel;
    int paintLineWidth = bulkHistoOptions.m_lineWidth;
    int paintFillColour = bulkHistoOptions.m_fillColour;
    int paintFillStyle = bulkHistoOptions.m_fillStyle;
    int paintLineStyle = bulkHistoOptions.m_lineStyle;
    int paintLineColour = bulkHistoOptions.m_lineColour;
    int paintStats = bulkHistoOptions.m_statsOption;
    if (dbRootHist && dbRootHist->rootHistogram) {
      dbRootHist->rootHistogram->SetXTitle(paintDrawXLabel.Data());
      dbRootHist->rootHistogram->SetYTitle(paintDrawYLabel.Data());
      dbRootHist->rootHistogram->SetLineWidth(paintLineWidth);
      dbRootHist->rootHistogram->SetFillColor(paintFillColour);
      dbRootHist->rootHistogram->SetFillStyle(paintFillStyle);
      dbRootHist->rootHistogram->SetLineStyle(paintLineStyle);
      dbRootHist->rootHistogram->SetLineColor(paintLineColour);
      dbRootHist->rootHistogram->SetStats(paintStats);

      TString paintDrawOption;
      if (s_H1D == dbRootHist->histogramType()) {
        m_drawOption = bulkHistoOptions.m_1DRootDrawOption;
        paintDrawOption = TString(m_drawOption + TString(" ") +
          bulkHistoOptions.m_genericRootDrawOption).Data();
      } else if ( (s_P1D == dbRootHist->histogramType()) ||
                  (s_HPD == dbRootHist->histogramType()) ) {
        m_drawOption = bulkHistoOptions.m_1DRootDrawOption;
        paintDrawOption = TString(m_drawOption + TString(" ") +
          bulkHistoOptions.m_genericRootDrawOption + TString(" E ")).Data();
      } else if (s_H2D == dbRootHist->histogramType()) {
        m_drawOption = bulkHistoOptions.m_2DRootDrawOption;
        paintDrawOption = TString(m_drawOption + TString(" ") +
          bulkHistoOptions.m_genericRootDrawOption).Data();
      }
      dbRootHist->rootHistogram->SetOption(paintDrawOption.Data());                                           
    }
  }
  TPad* targetPad = NULL;
  DbRootHist* prevDbRootHist = NULL;
  if ( (overlap == overlapMode) &&
       (false == dbHistosOnPage.empty()) &&
       (true == isConnectedToHistogramDB()) ) {
      std::vector<DbRootHist*>::iterator pad_dbHistosOnPageIt;
      pad_dbHistosOnPageIt = dbHistosOnPage.end();
      pad_dbHistosOnPageIt--;
      prevDbRootHist = *pad_dbHistosOnPageIt;
      if ( (NULL != prevDbRootHist) &&
           (NULL != prevDbRootHist->hostingPad) ) { // && (pres::MonRate != prevDbRootHist->effServiceType())
        targetPad = prevDbRootHist->hostingPad;
        dbRootHist->hostingPad = targetPad;        
      }
  }
  dbHistosOnPage.push_back(dbRootHist);  
  if ( (Batch != m_presenterMode) &&
       (invisible != overlapMode) ) {
    paintHist(dbRootHist, targetPad);
  } else if ( (invisible != overlapMode) &&
              (Batch == m_presenterMode) ){
     dbRootHist->rootHistogram->SetLineStyle(1);
     dbRootHist->rootHistogram->SetLineWidth(1);
  }
}
void PresenterMainFrame::addDimHistosToPage()
{
  disableAutoCanvasLayoutBtn();
  disableHistogramClearing();
  
  stopPageRefresh();
  if (m_clearedHistos) { clearHistos(); }
  if (m_referencesOverlayed) { toggleReferenceOverlay(); }

  fClient->WaitFor(dynamic_cast<TGWindow*>(
                   new HistoPropDialog(this, 646, 435, m_verbosity)));

  TGListTree* list = new TGListTree();
  checkedTreeItems(list, m_histoSvcListTree);

  TGListTreeItem* currentNode;
  currentNode = list->GetFirstItem();

  while (0 != currentNode) {
    TString histoID((*static_cast<TObjString*>(currentNode->GetUserData())).GetString());
    if (histoID.BeginsWith(s_FILE_URI)) { histoID.Remove(0, s_FILE_URI.length()); }
    addHistoToPage(std::string(histoID), separate);
    currentNode = currentNode->GetNextSibling();
  }
  list->Delete();
  delete list;
  list = NULL;
  m_histoSvcListTree->CheckAllChildren(m_histoSvcListTree->GetFirstItem(),
                                       s_uncheckTreeItems);
//  fClient->NeedRedraw(m_histoSvcListTree);
//  Resize();DoRedraw();

  editorCanvas->Update();
  enableAutoCanvasLayoutBtn();
}
void PresenterMainFrame::addDbHistoToPage(pres::ServicePlotMode overlapMode)
{
  disableAutoCanvasLayoutBtn();
  stopPageRefresh();
  try {
    if (0 != m_histogramDB) {
      TGListTree* list = new TGListTree();
      checkedTreeItems(list, m_databaseHistogramTreeList);
      TGListTreeItem* currentNode = list->GetFirstItem();      
      if (0 != currentNode) {
        addHistoToPage(std::string((*static_cast<TObjString*>(currentNode->GetUserData())).GetString()), separate);
        currentNode = currentNode->GetNextSibling();        
        while (0 != currentNode) {        
          addHistoToPage(std::string((*static_cast<TObjString*>(currentNode->GetUserData())).GetString()), overlapMode);
          currentNode = currentNode->GetNextSibling();
        }
      }
      list->Delete();
      delete list;
      list = NULL;
    }
  } catch (std::string sqlException) {
    if (m_verbosity >= Verbose) { std::cout << sqlException; }
    if (Batch != m_presenterMode) {
      new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                   Form("OnlineHistDB server:\n\n%s\n",
                         sqlException.c_str()),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
  }

  m_databaseHistogramTreeList->CheckAllChildren(m_databaseHistogramTreeList->GetFirstItem(),
                                                s_uncheckTreeItems);
//  fClient->NeedRedraw(m_databaseHistogramTreeList);
//  Resize();DoRedraw();
  editorCanvas->Update();
  enableAutoCanvasLayoutBtn();
}

void PresenterMainFrame::dimCollapseAllChildren()
{
  if (0 != m_histoSvcListTree &&
      m_histoSvcListTree->GetSelected()) {
    collapseTreeChildrenItems(m_histoSvcListTree,
                              m_histoSvcListTree->GetSelected());
    fClient->NeedRedraw(m_histoSvcListTree);
//    Resize();DoRedraw();
  }
}
void PresenterMainFrame::dbHistoCollapseAllChildren()
{
  if (0 != m_databaseHistogramTreeList &&
      m_databaseHistogramTreeList->GetSelected()) {
    collapseTreeChildrenItems(m_databaseHistogramTreeList,
                              m_databaseHistogramTreeList->GetSelected());
    fClient->NeedRedraw(m_databaseHistogramTreeList);
//    Resize();DoRedraw();
  }
}
std::string PresenterMainFrame::convDimToHistoID(const std::string & dimSvcName)
{
  HistogramIdentifier histogram = HistogramIdentifier(dimSvcName);
  if (histogram.isDimFormat()) {
    return histogram.histogramIdentifier();
  } else {
    if (Batch != m_presenterMode) {    
        new TGMsgBox(fClient->GetRoot(), this, "DIM Service name error",
                     Form("The DIM servicename\n%s\n does not appear to follow the" \
                       "convention\nPlease use the following format:\n" \
                       "HnD/UTGID/Algorithmname/Histogramname\n where the UTGID " \
                       "normally has the following format:\n node00101_taskname_1 " \
                       "or simply taskname, without \"_\"", dimSvcName.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
    return 0;
  }
}
void PresenterMainFrame::toggleHistoryPlots()
{
  if (m_historyTrendPlots) {
    m_historyPlotsButton->SetState(kButtonUp);
    m_viewMenu->UnCheckEntry(HISTORY_PLOTS_COMMAND);
    m_historyTrendPlots = false;
  } else {
    m_historyPlotsButton->SetState(kButtonDown);
    m_historyTrendPlots = true;
    m_viewMenu->CheckEntry(HISTORY_PLOTS_COMMAND);
  }
}
void PresenterMainFrame::toggleReferenceOverlay()
{
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
void PresenterMainFrame::toggleFastHitMapDraw()
{
  if (m_fastHitMapDraw) {
    m_viewMenu->UnCheckEntry(FAST_HITMAP_DRAW_COMMAND);
    m_fastHitMapDraw = false;
  } else {
    m_fastHitMapDraw = true;
    m_viewMenu->CheckEntry(FAST_HITMAP_DRAW_COMMAND);
  }
}
void PresenterMainFrame::toggleShowAlarmList()
{
  if (m_viewMenu->IsEntryChecked(SHOW_ALARM_LIST_COMMAND)) {
    m_viewMenu->UnCheckEntry(SHOW_ALARM_LIST_COMMAND);
//    m_databaseAlarmsDock->HideContainer();
    m_leftMiscFrame->HideFrame(m_databaseAlarmsDock);
  } else {
    m_viewMenu->CheckEntry(SHOW_ALARM_LIST_COMMAND);
    m_leftMiscFrame->ShowFrame(m_databaseAlarmsDock);
//    m_databaseAlarmsDock->ShowContainer();
  }
}
void PresenterMainFrame::pickReferenceHistogram()
{
  DbRootHist* histogram = selectedDbRootHistogram();
  
  if (0 != histogram) {
    TVirtualPad *padsav = gPad;
    fClient->WaitFor(new ReferencePicker(this, histogram));    
    padsav->cd();
  }
}
void PresenterMainFrame::saveSelectedHistogramAsReference()
{
  DbRootHist* histogram = selectedDbRootHistogram();
  
  if (0 != histogram && 0 != m_archive &&
      (Batch != m_presenterMode)) {
    new TGMsgBox(fClient->GetRoot(), this, "Save as Reference Histogram",
                 "Are you sure to save selected histogram as a reference?",
                 kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
    switch (m_msgBoxReturnCode) {
      case kMBYes:
        m_archive->saveAsReferenceHistogram(histogram);
        break;
      default:
        break;
    }
  }
}
void PresenterMainFrame::setHistogramPropertiesInDB()
{
  try {
    if (0 != m_histogramDB) {
      fClient->WaitFor(dynamic_cast<TGWindow*>(
                          new HistoPropDialog(this, 646, 435, m_verbosity)));

      TGListTree* list = new TGListTree();
      checkedTreeItems(list, m_databaseHistogramTreeList);

      TGListTreeItem* currentNode;
      currentNode = list->GetFirstItem();
      while (0 != currentNode) {
        OnlineHistogram* onlineHistogramToSet = m_histogramDB->
          getHistogram(std::string((*static_cast<TObjString*>(currentNode->GetUserData())).GetString()));

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

        if (s_H1D == onlineHistogramToSet->hstype() ||
            s_P1D == onlineHistogramToSet->hstype() ||
            s_pfixMonH1D == onlineHistogramToSet->hstype() ||
            s_pfixMonProfile == onlineHistogramToSet->hstype() ||
            s_HPD == onlineHistogramToSet->hstype()) {
          m_drawOption = bulkHistoOptions.m_1DRootDrawOption;
        } else if (s_H2D == onlineHistogramToSet->hstype() ||
                   s_pfixMonH2F == onlineHistogramToSet->hstype() ||
                   s_pfixMonH2D == onlineHistogramToSet->hstype() ) {
          m_drawOption = bulkHistoOptions.m_2DRootDrawOption;
        }
        std::string paintDrawOption = TString(
          m_drawOption + TString(" ") +
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
    if (m_verbosity >= Verbose) { std::cout << sqlException; }
    if (Batch != m_presenterMode) {
      new TGMsgBox(fClient->GetRoot(), this, "Database Error",
          Form("OnlineHistDB server:\n\n%s\n",
               sqlException.c_str()),
               kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
  }
//  refreshHistoDBListTree();
}
void PresenterMainFrame::deleteSelectedHistoFromDB() {
  if (Batch != m_presenterMode) {
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
        std::string hist_id((*static_cast<TObjString*>(currentNode->GetUserData())).GetString());
        OnlineHistogram* histoToDelete = m_histogramDB->getHistogram(hist_id);
        if (histoToDelete) {
          // remove this histogram from current page if present
          std::vector<DbRootHist*>::iterator delHist_dbHistosOnPageIt;
          delHist_dbHistosOnPageIt = dbHistosOnPage.begin();
          while (delHist_dbHistosOnPageIt != dbHistosOnPage.end() ) {
            if ( (*delHist_dbHistosOnPageIt)->identifier() == hist_id ) {
              delete *delHist_dbHistosOnPageIt;
              *delHist_dbHistosOnPageIt = NULL;
              delHist_dbHistosOnPageIt = dbHistosOnPage.erase(delHist_dbHistosOnPageIt);
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
    if (m_verbosity >= Verbose) { std::cout << sqlException; }
    if (Batch != m_presenterMode) {
      new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                   Form("OnlineHistDB server:\n\n%s\n",
                        sqlException.c_str()),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
  }
  refreshHistoDBListTree();
}
void PresenterMainFrame::enablePageLoading() {
  m_pagesContextMenu->EnableEntry(M_LoadPage_COMMAND);
}

std::string PresenterMainFrame::selectedPageFromDbTree(){
  TGListTreeItem* node = m_pagesFromHistoDBListTree->GetSelected();
  if (0 != node && node->GetUserData()) {

    TString pageName = (*static_cast<TObjString*>(node->GetUserData())).GetString();
    if (pageName.BeginsWith(s_FILE_URI)) {pageName.Remove(0, s_FILE_URI.length()); }
    return std::string(pageName);
  } else {
    return std::string("");
  }
}
int PresenterMainFrame::selectedAlarmFromDbTree(){
  TGListTreeItem* node = m_alarmHistogramTreeList->GetSelected();
  if (0 != node && node->GetUserData() &&
      m_alarmHistogramTreeList->GetFirstItem() != node) {
    return *(int*)(node->GetUserData());
  } else {
    return -1;
  }
}
void PresenterMainFrame::loadAllPages()
{
  if (isConnectedToHistogramDB()) {
     m_localDatabaseFolders.clear();
     //m_histogramDB->getPageFolderNames(m_localDatabaseFolders, "_ALL_");
     m_histogramDB->getPageNames(m_localDatabaseFolders);

     for (m_folderIt = m_localDatabaseFolders.begin();
          m_folderIt != m_localDatabaseFolders.end(); ++m_folderIt) {
      loadSelectedPageFromDB(*m_folderIt, s_startupFile, m_savesetFileName, false);
     }
  }
}
void PresenterMainFrame::loadSelectedAlarmFromDB(int msgId)
{
  if (isConnectedToHistogramDB()) {
    if (m_clearedHistos) { clearHistos(); }
    removeHistogramsFromPage();

    OMAMessage message(msgId, *m_histogramDB);
    std::string previousSaveset = m_savesetFileName;
    m_savesetFileName = message.saveSet();
    m_prevPresenterMode = m_presenterMode; 
    m_presenterMode = History;
    addHistoToPage(message.hIdentifier(), separate);
    autoCanvasLayout();
    
    m_presenterMode = m_prevPresenterMode;
    m_savesetFileName = previousSaveset;
    
    m_pageDescriptionView->Clear();
    m_pageDescriptionView->LoadBuffer(message.msgtext());
    m_pageDescriptionView->DataChanged();
    
    if (m_referencesOverlayed) {
      enableReferenceOverlay();
    } else {
      disableReferenceOverlay();
    }
  }
}
void PresenterMainFrame::loadPreviousPage()
{
  if ((false == m_loadedPagesHistory.empty()) &&
      (m_loadedPagesHistoryIt > m_loadedPagesHistory.begin()) &&
      (false == m_loadingPage)) {
    m_loadedPagesHistoryIt--;
    if (m_loadedPagesHistoryIt != m_loadedPagesHistory.end() &&
      (false == (**m_loadedPagesHistoryIt).empty())) {
    m_currentPageName = **m_loadedPagesHistoryIt;
    loadSelectedPageFromDB(m_currentPageName, s_startupFile, m_savesetFileName, s_noPageHistory);  
    } 
  }
}
void PresenterMainFrame::loadNextPage()
{
  if ((false == m_loadedPagesHistory.empty()) &&
      (m_loadedPagesHistoryIt < m_loadedPagesHistory.end()-1) &&
      (false == m_loadingPage)) {
    m_loadedPagesHistoryIt++;
    if ((m_loadedPagesHistoryIt != m_loadedPagesHistory.end()) &&
      (false == (**m_loadedPagesHistoryIt).empty())) {
      m_currentPageName = **m_loadedPagesHistoryIt;
      loadSelectedPageFromDB(m_currentPageName, s_startupFile, m_savesetFileName, s_noPageHistory);  
    }    
  }
}
void PresenterMainFrame::loadSelectedPageFromDB(const std::string & pageName,
                                                const std::string & timePoint,
                                                const std::string & pastDuration,
                                                bool pageHistoryMode)
{
  if (isConnectedToHistogramDB() && (false == m_loadingPage)) {
    m_loadingPage = true;
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetWaitCursor());
    if (m_refreshingPage) {
      stopPageRefresh();
      m_resumePageRefreshAfterLoading = true;
    }
    if (m_clearedHistos) { clearHistos(); }

    std::string currentTCK_service = "";

    disableAutoCanvasLayoutBtn();
    removeHistogramsFromPage();

    rw_timePoint = timePoint;
    rw_pastDuration = pastDuration;

    std::string history_entry(dynamic_cast<TGTextLBEntry*>(m_historyIntervalComboBox->GetSelectedEntry())->GetText()->GetString());

    if (0 != m_archive &&
        ((History == m_presenterMode) || (EditorOffline == m_presenterMode))) {
      if (("last 8 hours" == history_entry)) {
        rw_timePoint = s_Now;
        rw_pastDuration = std::string("08:00:00");
      } else if (("last 1 hour" == history_entry)) {
        rw_timePoint = s_Now;
        rw_pastDuration = std::string("01:00:00");
      } else if (("preset file" == history_entry)) {
        rw_timePoint = s_startupFile;
        rw_pastDuration = pastDuration;
      } else if (("set file" == history_entry)) {
        rw_timePoint = s_startupFile;
        rw_pastDuration = pastDuration;
      } else {
        rw_timePoint = global_timePoint;
        rw_pastDuration = global_pastDuration;        
      }
      if (m_verbosity >= Verbose) {
        std::cout << "Navigation step size" << global_stepSize << std::endl;
      }
    }
//    startBenchmark(pageName); 
      try {
        removeHistogramsFromPage();
        m_onlineHistosOnPage.clear();
        OnlineHistPage* page = m_histogramDB->getPage(pageName);
        if (m_verbosity >= Verbose) {
          std::cout << "Loading page: "  << m_currentPageName << std::endl;
        }
        if (m_verbosity >= Debug) {
          page->dump();
        }

        if (0 != page) {
//          if (false == page->patternFile().empty()) {
////            m_drawPattern;
//            TFile rootFile(page->patternFile().c_str()); 
//           if (rootFile.IsZombie()) {
//                std::cout << "Error opening page pattern file" << std::endl;
//            } else {
//               TIter next1(rootFile.GetListOfKeys());
//               TKey* key;    
//               while ((key = (TKey*)next1())) {
//                 if (key->ReadObj()->InheritsFrom(TCanvas::Class())) {
//               m_drawPattern = (TCanvas*)key->ReadObj();
//                 }
//               }        
//           TPad *padsav = (TPad*)gPad;
//        //   gPad =   editorCanvas->cd();
//           TObject *obj; //, *clone;
//        TPad* pad = editorCanvas;
//           dynamic_cast<TAttLine*>(m_drawPattern)->Copy((TAttLine&)*pad);
//           dynamic_cast<TAttFill*>(m_drawPattern)->Copy((TAttFill&)*pad);
//           dynamic_cast<TAttPad*>(m_drawPattern)->Copy((TAttPad&)*pad);
//        
//           TIter next(m_drawPattern->GetListOfPrimitives());
//           while ((obj=next())) {
//              pad->cd();
//              if (TBox::Class() == obj->IsA() ||
//                  TLine::Class() == obj->IsA() ||
//                  TText::Class() == obj->IsA()) {
//                  obj->Draw();
//        //      clone = obj->Clone();
//        //      pad->GetListOfPrimitives()->Add(clone,next.GetOption());
//              }
//           }
//           if (padsav) padsav->cd();
//          }
//          rootFile.Close();
//       } else {
//        editorCanvas->Clear();
//       }
////       if (m_verbosity >= Debug) {
//std::cout << "page pattern file: " << page->patternFile() << std::endl;
////       } 
          page->getHistogramList(&m_onlineHistosOnPage);
          ParallelWait parallelWait(this);
          parallelWait.loadHistograms(&m_onlineHistosOnPage, &dbHistosOnPage);

//          m_onlineHistosOnPageIt = m_onlineHistosOnPage.begin();
          std::vector<DbRootHist*>::iterator drawHist_dbHistosOnPageIt;
//     if (gSystem->ProcessEvents()) break; -> handle fast pageload clicks?
                   
          for (drawHist_dbHistosOnPageIt = dbHistosOnPage.begin();
               drawHist_dbHistosOnPageIt != dbHistosOnPage.end();
               drawHist_dbHistosOnPageIt++) {

// HLTA0101_Adder_1/GauchoJob/MonitorSvc/monRate/TCK
//           

            if ( (m_verbosity >= Verbose) &&
                 (*drawHist_dbHistosOnPageIt) &&
                 (*drawHist_dbHistosOnPageIt)->onlineHistogram() &&
                 ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage() &&
                 (TCKinfo != (*drawHist_dbHistosOnPageIt)->effServiceType()) ) {
              std::cout << "db identifier "
                        << ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->histo->identifier()
                        << std::endl << "\tdb task "
                        << ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->histo->task() << std::endl    
                        << "\tdb dimServiceName "
                        << ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->histo->dimServiceName()
                        << std::endl;
            }
            if ( (m_verbosity >= Debug) &&
                 ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage() &&
                 (TCKinfo != (*drawHist_dbHistosOnPageIt)->effServiceType())) {
              ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->histo->dump();
            }
            TPad* overlayOnPad = NULL;
  
            double xlow(0.0);
            double ylow(0.0);
            double xup(0.0);
            double yup(0.0);
                        
            if ( (*drawHist_dbHistosOnPageIt) &&
                 ((*drawHist_dbHistosOnPageIt)->onlineHistogram()) &&
                 ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage() &&
                 (TCKinfo != (*drawHist_dbHistosOnPageIt)->effServiceType()) ) {
              xlow = ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->xmin;
              ylow = ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->ymin;
              xup  = ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->xmax;
              yup  = ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->ymax;
  
             if(((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->isOverlap()) { // histogram must be overdrawn on previous hist: look for the corresponding TPad
               OnlineHistoOnPage *mother = ((*drawHist_dbHistosOnPageIt)->onlineHistogram())->onpage()->getOverlap();
               std::vector<DbRootHist*>::iterator prevh;
               DbRootHist* dbRootHist = NULL;
               for (prevh=dbHistosOnPage.begin() ; (*prevh) != dbRootHist ; prevh++) {
                 if ( (*prevh)->onlineHistogram() == mother->histo) {
                   overlayOnPad = (*prevh)->hostingPad;
                   break;
                 }
               }
             }
           }
           if (m_verbosity >= Debug) {
             if (overlayOnPad) {
               std::cout << "drawing overlap on pad " << overlayOnPad <<std::endl;
             } else {
               std::cout << "drawing pad X "<< xlow << " to "<< xup <<
                 "  Y " << ylow << " to " << yup <<std::endl;
             }
           }
          if ( (*drawHist_dbHistosOnPageIt) &&
               (TCKinfo != (*drawHist_dbHistosOnPageIt)->effServiceType())) {
            (*drawHist_dbHistosOnPageIt)->draw(editorCanvas, xlow, ylow, xup, yup, m_fastHitMapDraw, overlayOnPad);
          }
//          stopBenchmark((*m_onlineHistosOnPageIt)->histo->identifier());
//          m_onlineHistosOnPageIt++;
          }
          
          m_pageDescriptionView->Clear();
          m_pageDescription = page->doc();
          m_pageDescriptionView->LoadBuffer(m_pageDescription.c_str());
          m_pageDescriptionView->DataChanged();    
  // workaround attempt for ROOT painter objects:
  // reliable random crashes when below invoked... via TH painter (timing dependent?)
        editorCanvas->Update();
        std::vector<DbRootHist*>::iterator drawOpt_dbHistosOnPageIt;
        
        for (drawOpt_dbHistosOnPageIt = dbHistosOnPage.begin();
             drawOpt_dbHistosOnPageIt != dbHistosOnPage.end();
             drawOpt_dbHistosOnPageIt++) {
          if ( TCKinfo != (*drawOpt_dbHistosOnPageIt)->effServiceType() ) {
            (*drawOpt_dbHistosOnPageIt)->setDrawOptionsFromDB((*drawOpt_dbHistosOnPageIt)->hostingPad);

          }
           if ( (s_CNT == (*drawOpt_dbHistosOnPageIt)->histogramType()) &&
                (true == currentTCK_service.empty()) ) {
              HistogramIdentifier histogramIdentifier = HistogramIdentifier((*drawOpt_dbHistosOnPageIt)->identifier());                  
              if (histogramIdentifier.isPlausible()) {
                currentTCK_service = s_adder + histogramIdentifier.taskName() + s_slash + 
                               histogramIdentifier.algorithmName() + s_slash +
                               s_eff_MonitorSvc + s_slash + s_eff_monRate +
                               s_slash + s_eff_TCK;
              }
           }
        }

  if (false == currentTCK_service.empty()) {
    addHistoToPage(currentTCK_service, invisible);
  }          
        editorCanvas->Update();        

        }
        
        } catch (std::string sqlException) {
        setStatusBarText(sqlException.c_str(), 2);
        if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }
        if (Batch != m_presenterMode) {
          new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                       Form("Could not save the page to OnlineHistDB:\n\n%s\n",
                            sqlException.c_str()),
                       kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
        }
      }
      
//    stopBenchmark(pageName);
//    m_deadTasksOnPage = m_tasksNotRunning.size();
//    printBenchmark(pageName);      
    if (false == pageHistoryMode) {
      std::string* history = new std::string(pageName.c_str());
      m_loadedPagesHistory.push_back(history);
      m_loadedPagesHistoryIt = m_loadedPagesHistory.end()-1;      
    }
    
    m_loadingPage = false;
    if (m_resumePageRefreshAfterLoading &&
        (Online == m_presenterMode)) {
       startPageRefresh();
    }
    
    
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetDefaultCursor());
  }
  if (m_referencesOverlayed) {
    enableReferenceOverlay();
  } else {
    disableReferenceOverlay();
  }
//  editorCanvas->Update();
  if ((EditorOnline == m_presenterMode) || (EditorOffline == m_presenterMode)) {
    enableAutoCanvasLayoutBtn();
  }  
}

void PresenterMainFrame::moveSelectedInDB()
{
  if (ReadWrite == m_databaseMode) {
//    TGListTreeItem* TGListTreeItem* node = m_pagesFromHistoDBListTree->GetSelected();
//    selectedHistogramDatabaseItemName = PageName
//    selectedHistogramDatabaseItemType = FolderName

    DatabasePagePathDialog* databasePagePathDialog = new DatabasePagePathDialog(this, 493, 339, m_verbosity);
//    databasePagePathDialog->set
    fClient->WaitFor(databasePagePathDialog);
//    newName
    refreshPagesDBListTree();
  }
}

void PresenterMainFrame::deleteSelectedPageFromDB()
{
  if (Batch != m_presenterMode) {
    new TGMsgBox(fClient->GetRoot(), this, "Delete Page",
                 "Are you sure to delete selected page from the database?",
                 kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
    switch (m_msgBoxReturnCode) {
      case kMBNo:
        return;
    }
  }

  if (canWriteToHistogramDB()) {
    TGListTreeItem* node = m_pagesFromHistoDBListTree->GetSelected();

    if (node && node->GetUserData()) {
      std::string path = std::string((*static_cast<TObjString*>(node->GetUserData())).GetString());
      try {
        OnlineHistPage* page = m_histogramDB->getPage(path);
        if (0 != page) {
          m_histogramDB->removePage(page);
          m_histogramDB->commit();
          refreshPagesDBListTree();
        }
      } catch (std::string sqlException) {
        setStatusBarText(sqlException.c_str(), 2);
        if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }
        if (Batch != m_presenterMode) {
          new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                       Form("Could not delete the page to OnlineHistDB:\n\n%s\n",
                            sqlException.c_str()),
                       kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
        }
      }
    }
  }
}
void PresenterMainFrame::deleteSelectedFolderFromDB()
{
// new TGMsgBox(fClient->GetRoot(), this, "Delete Folder",
//              "Are you sure to delete selected folder from the Database?",
//              kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
//  switch (m_msgBoxReturnCode) {
//    case kMBNo:
//      return;
//  }
  if (canWriteToHistogramDB()) {

    TGListTreeItem* node = m_pagesFromHistoDBListTree->GetSelected();

    if (0 != node) {
      char path[1024];
      m_pagesFromHistoDBListTree->GetPathnameFromItem(node, path);
      std::string folder = std::string(path);
      // Drop DB url
      folder = folder.erase(0, strlen(m_pagesFromHistoDBListTree->GetFirstItem()->GetText())+1);
      if (m_verbosity >= Verbose) {
        std::cout << "Folder to delete: " << folder << std::endl;
      }

      try {
        m_histogramDB->removePageFolder(folder);
        m_histogramDB->commit();
        refreshPagesDBListTree();
      } catch (std::string sqlException) {
        setStatusBarText(sqlException.c_str(), 2);
        if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }
        if (Batch != m_presenterMode) {
          new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                       Form("Could delete the page to OnlineHistDB:\n\n%s\n",
                            sqlException.c_str()),
                       kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
        }
      }
    }
  }
}
void PresenterMainFrame::paintHist(DbRootHist* histogram, TPad* overlayOnPad)
{
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
  histogram->draw(editorCanvas, xlow, ylow, xup, yup, m_fastHitMapDraw, overlayOnPad);
}
void PresenterMainFrame::autoCanvasLayout()
{
  
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
              0 != ((*layout_dbHistosOnPageIt2)->hostingPad)) {
            ((*layout_dbHistosOnPageIt2)->hostingPad)->SetPad(x1, y1, x2, y2);
            ((*layout_dbHistosOnPageIt2)->hostingPad)->Modified();
          }
          layout_dbHistosOnPageIt2++;
        }
      }
    }
  }
  editorCanvas->Update();
}
void PresenterMainFrame::deleteSelectedHistoFromCanvas()
{
  if (m_refreshingPage) {
    stopPageRefresh();
    m_resumePageRefreshAfterLoading = true;
  }
  bool referenceOverlay(false);
  if (m_referencesOverlayed) {
    disableReferenceOverlay();
    referenceOverlay = true;
  }
  if (m_clearedHistos) { clearHistos(); }
    
  DbRootHist* histogram = selectedDbRootHistogram();
  bool foundSelectedHisto(false);
  if (0 != histogram) {
    std::vector<DbRootHist*>::iterator del_dbHistosOnPageIt;
    del_dbHistosOnPageIt = dbHistosOnPage.begin();
    while (del_dbHistosOnPageIt != dbHistosOnPage.end()) {
      if (0 == ((*del_dbHistosOnPageIt)->histoRootName()).
                compare(histogram->histoRootName())) {
        foundSelectedHisto = true;
        break;
      }
      if (false == foundSelectedHisto) { del_dbHistosOnPageIt++; }
    }
    if (foundSelectedHisto &&
        (Batch != m_presenterMode) ) {
      new TGMsgBox(fClient->GetRoot(), this, "Remove Histogram",
                   "Are you sure to remove selected histogram from the page?",
                   kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
      switch (m_msgBoxReturnCode) {
        case kMBYes:
          delete (*del_dbHistosOnPageIt);
          (*del_dbHistosOnPageIt) = NULL;
          dbHistosOnPage.erase(del_dbHistosOnPageIt);
// find all overlaps too!           hostingPad::Find(isA(TH))...
          editorCanvas->Update();
//          return;
          break;
        default:
          break;
      }
    }
  }
  if (m_resumePageRefreshAfterLoading) { startPageRefresh(); }
  if (referenceOverlay) { enableReferenceOverlay(); } 
}
DbRootHist* PresenterMainFrame::selectedDbRootHistogram()
{
 DbRootHist* dbRootHist = 0;

  bool referenceOverlay(false);
  if (m_referencesOverlayed) {
    disableReferenceOverlay();
    referenceOverlay = true;
  }

  if (0 != gPad) {
    TIter nextPadElem(gPad->GetListOfPrimitives());
    TObject* histoCandidate;
    while (0 != (histoCandidate = nextPadElem())) {
      if (histoCandidate->InheritsFrom(TH1::Class())) {
        std::vector<DbRootHist*>::iterator selected_dbHistosOnPageIt;
        for (selected_dbHistosOnPageIt = dbHistosOnPage.begin();
             selected_dbHistosOnPageIt != dbHistosOnPage.end();
             selected_dbHistosOnPageIt++) {
          if (0 == std::string((*selected_dbHistosOnPageIt)->rootHistogram->GetName()).compare(histoCandidate->GetName())) {
            dbRootHist = *selected_dbHistosOnPageIt;
            break;
          }
        }
        if (NULL != dbRootHist) {break;}
      }   
    }
    
    if (0 == dbRootHist && 
        (Batch != m_presenterMode)) {
    new TGMsgBox(fClient->GetRoot(), this, "Histogram Selection",
                 "Please select a histogram with the middle mouse button",
                 kMBIconStop, kMBOk, &m_msgBoxReturnCode);
    }
  }
  if (referenceOverlay) { enableReferenceOverlay(); }
  return dbRootHist;
}
void PresenterMainFrame::refreshClock()
{
  if (currentTime) {
    currentTime->Set();
    if (Batch != m_presenterMode) {
      m_statusBarTop->SetText(currentTime->AsSQLString(), 1);
    }
  }
}
void PresenterMainFrame::previousInterval()
{
  std::string history_entry(dynamic_cast<TGTextLBEntry*>(m_historyIntervalComboBox->GetSelectedEntry())->GetText()->GetString());
  if((History == m_presenterMode) &&
     (s_timeInterval == m_historyMode) &&
     (("preset interval" == history_entry) ||
     ("set interval" == history_entry))    
    ) {
    global_timePoint = m_archive->substractIsoTimeDate(global_timePoint,
                                                       global_stepSize);
//    m_currentPageName = selectedPageFromDbTree();
    if (!m_currentPageName.empty()) {
      loadSelectedPageFromDB(m_currentPageName, global_timePoint, global_pastDuration, false);
    }
    if (m_verbosity >= Verbose) {
      std::cout << "after previousInterval global_timePoint " << global_timePoint << std::endl;
    }
  }
}
void PresenterMainFrame::nextInterval()
{
  std::string history_entry(dynamic_cast<TGTextLBEntry*>(m_historyIntervalComboBox->GetSelectedEntry())->GetText()->GetString());
  if((History == m_presenterMode) &&
     (s_timeInterval == m_historyMode) &&
     (("preset interval" == history_entry) ||
     ("set interval" == history_entry))
    ) {
    global_timePoint = m_archive->addIsoTimeDate(global_timePoint,
                                                 global_stepSize);
//    m_currentPageName = selectedPageFromDbTree();                                                 
    if (!m_currentPageName.empty()) {
      loadSelectedPageFromDB(m_currentPageName, global_timePoint, global_pastDuration, false);
    }
    if (m_verbosity >= Verbose) {
      std::cout << "after nextInterval global_timePoint " << global_timePoint << std::endl;
    }
  }
}
void PresenterMainFrame::refreshPage()
{
  if (m_verbosity >= Verbose) {
//    std::cout << "refreshing." << std::endl;
  }
  if (Batch != m_presenterMode) {
      ParallelWait parallelWait(this);
      parallelWait.refreshHistograms(&dbHistosOnPage);
    if (! gROOT->IsInterrupted()) {      
      editorCanvas->Update();
    }
  } else if (Batch == m_presenterMode) {
    
//  <HEAD>
//  <META HTTP-EQUIV=REFRESH CONTENT=5> // CONTENT=5 means wait for 5 seconds to reload the page.
//  </HEAD>    
    
    std::vector<DbRootHist*>::iterator dump_dbHistosOnPageIt;
    dump_dbHistosOnPageIt = dbHistosOnPage.begin();
    editorCanvas->cd();
    while (dump_dbHistosOnPageIt != dbHistosOnPage.end()) {
    if (false == (*dump_dbHistosOnPageIt)->isEmptyHisto()) {
      (*dump_dbHistosOnPageIt)->fillHistogram();
      (*dump_dbHistosOnPageIt)->normalizeReference();
    }
//    if (true == (*dump_dbHistosOnPageIt)->rateInitialised()) {
      std::string plotName(m_currentPartition);
        plotName.append(" ").append(currentTime->AsSQLString());
        (*dump_dbHistosOnPageIt)->rootHistogram->SetTitle(plotName.c_str());
        
        (*dump_dbHistosOnPageIt)->rootHistogram->Draw();
        gPad->Modified(); 
        editorCanvas->Modified(); 
        editorCanvas->Update();
        std::string dumpFile = m_imagePath + s_slash +
                               (*dump_dbHistosOnPageIt)->fileName() +
                               "." + m_dumpFormat;
        if (! gROOT->IsInterrupted()) {
          editorCanvas->SaveAs(dumpFile.c_str());
        }
//      }  else {
//        if (m_verbosity >= Verbose) {
//          std::cout << (*dump_dbHistosOnPageIt)->fileName() <<
//                       " rate not initialised." << std::endl;
//        }      
//      }    
      dump_dbHistosOnPageIt++;              
    }
  }
}
void PresenterMainFrame::enableHistogramClearing()
{
  std::vector<DbRootHist*>::iterator clear_dbHistosOnPageIt;
  clear_dbHistosOnPageIt = dbHistosOnPage.begin();
  while (clear_dbHistosOnPageIt != dbHistosOnPage.end()) {
    (*clear_dbHistosOnPageIt)->enableClear();
    clear_dbHistosOnPageIt++;
  }
}
void PresenterMainFrame::disableHistogramClearing()
{
  std::vector<DbRootHist*>::iterator clearDisable_dbHistosOnPageIt;
  clearDisable_dbHistosOnPageIt = dbHistosOnPage.begin();
  while (clearDisable_dbHistosOnPageIt != dbHistosOnPage.end()) {
    (*clearDisable_dbHistosOnPageIt)->disableClear();
    clearDisable_dbHistosOnPageIt++;
  }
}
void PresenterMainFrame::enableReferenceOverlay()
{
  bool stopped(false);
  if (m_refreshingPage) {
    stopPageRefresh();
    stopped = true;
  }
  std::vector<DbRootHist*>::iterator enableRef_dbHistosOnPageIt;
  enableRef_dbHistosOnPageIt = dbHistosOnPage.begin();
  while (enableRef_dbHistosOnPageIt != dbHistosOnPage.end()) {
    (*enableRef_dbHistosOnPageIt)->referenceHistogram(Show);
    enableRef_dbHistosOnPageIt++;
  }
  editorCanvas->Update();
  if (stopped) { startPageRefresh(); }
}
void PresenterMainFrame::disableReferenceOverlay()
{
  std::vector<DbRootHist*>::iterator disableRef_dbHistosOnPageIt;
  disableRef_dbHistosOnPageIt = dbHistosOnPage.begin();
  while (disableRef_dbHistosOnPageIt != dbHistosOnPage.end()) {
    (*disableRef_dbHistosOnPageIt)->referenceHistogram(Hide);
    disableRef_dbHistosOnPageIt++;
  }
  editorCanvas->Update();
}
void PresenterMainFrame::enablePageRefresh()
{
  if (false == m_loadingPage) {
    if (Batch != m_presenterMode) {
      editorCanvas->SetEditable(false);
    }
    std::vector<DbRootHist*>::iterator enableRefresh_dbHistosOnPageIt;
    enableRefresh_dbHistosOnPageIt = dbHistosOnPage.begin();
    while (enableRefresh_dbHistosOnPageIt != dbHistosOnPage.end()) {
      (*enableRefresh_dbHistosOnPageIt)->disableEdit();
      enableRefresh_dbHistosOnPageIt++;
    }
  }
}
void PresenterMainFrame::disablePageRefresh()
{
  m_refreshingPage = false;
  if (Batch != m_presenterMode &&
      editorCanvas) {
    editorCanvas->SetEditable(true);
  }
  std::vector<DbRootHist*>::iterator disableRefresh_dbHistosOnPageIt;
  disableRefresh_dbHistosOnPageIt = dbHistosOnPage.begin();
  while (disableRefresh_dbHistosOnPageIt != dbHistosOnPage.end()) {
    (*disableRefresh_dbHistosOnPageIt)->enableEdit();
    disableRefresh_dbHistosOnPageIt++;
  }
}
void PresenterMainFrame::enablePageUndocking()
{
  m_viewMenu->EnableEntry(UNDOCK_PAGE_COMMAND);
}
void PresenterMainFrame::disablePageUndocking()
{
  m_viewMenu->DisableEntry(UNDOCK_PAGE_COMMAND);
}
void PresenterMainFrame::removeHistogramsFromPage()
{
//  new TGMsgBox(fClient->GetRoot(), this, "Clear Page",
//        "Do you really want to clear the current page?",
//        kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
//    switch (m_msgBoxReturnCode) {
//      case kMBNo:
//        return;
//  }
  disablePageRefresh();
  stopPageRefresh();
  editorCanvas->SetEditable(true);

  if (m_verbosity >= Verbose) { std::cout << "clearCanvas." << std::endl; }
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
}
void PresenterMainFrame::EventInfo(int event, int px, int py, TObject* selected)
{
 
  switch (event) {
  case kButton1Double:
    if (NULL != selected && selected->InheritsFrom(TH1::Class())) {
      std::vector<DbRootHist*>::iterator eventInfo_dbHistosOnPageIt;
      for (eventInfo_dbHistosOnPageIt = dbHistosOnPage.begin();
         eventInfo_dbHistosOnPageIt != dbHistosOnPage.end();
         eventInfo_dbHistosOnPageIt++) {
        if (0 == std::string((*eventInfo_dbHistosOnPageIt)->rootHistogram->GetName()).compare(selected->GetName())) {
          if (NULL != (*eventInfo_dbHistosOnPageIt)->onlineHistogram() &&
              false == (*eventInfo_dbHistosOnPageIt)->onlineHistogram()->page2display().empty()) {
            if (m_verbosity >= Verbose) {
               std::cout << "loadPage: " << ((*eventInfo_dbHistosOnPageIt)->onlineHistogram())->page2display() << std::endl;
            }
            m_currentPageName = ((*eventInfo_dbHistosOnPageIt)->onlineHistogram())->page2display();
            if (!m_currentPageName.empty()) {
              loadSelectedPageFromDB(m_currentPageName, s_startupFile, m_savesetFileName, s_previousPageToHistory);
            }
            break;
          }  
        }
      }
    }
    break;
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
void PresenterMainFrame::about()
{
  std::stringstream config;
  config << std::string("Version: ") << s_presenterVersion << std::endl << std::endl <<
            std::string("Histogram Database version: ") << OnlineHistDBEnv_constants::version << std::endl <<
            std::string("Histogram Database schema: ") << OnlineHistDBEnv_constants::DBschema << std::endl <<
            std::string("Analysis Library version: ")  << OMAconstants::version << std::endl <<
            std::string("Connected to Histogram Database: ") << (isConnectedToHistogramDB()? std::string("yes") : std::string("no")) << std::endl;
  if (0 != m_histogramDB) {
     config << std::string("Write access to Histogram Database: ") << (canWriteToHistogramDB()? std::string("yes") : std::string("no")) << std::endl <<
     
               std::string("Reference histograms directory: ") << m_referencePath << std::endl <<
               std::string("Savesets directory: ") << m_savesetPath << std::endl <<
               
               std::string("DB Reference histograms directory: ") << dynamic_cast<OnlineHistDBEnv*>(m_histogramDB)->refRoot() << std::endl <<
               std::string("DB Savesets directory: ") << dynamic_cast<OnlineHistDBEnv*>(m_histogramDB)->savesetsRoot() << std::endl;
  }
  if (Batch != m_presenterMode) {
    new TGMsgBox(fClient->GetRoot(), this, "LHCb Presenter",
                 Form("%s", config.str().c_str()),
                 kMBIconAsterisk, kMBOk, &m_msgBoxReturnCode);
  }
}
