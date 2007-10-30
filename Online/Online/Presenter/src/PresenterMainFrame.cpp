#include <iostream>
#include <vector>
#include <algorithm>

#include <TApplication.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TG3DLine.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGMenu.h>
#include <TGPicture.h>
#include <TGTab.h>
#include <TGToolBar.h>
#include <TRootEmbeddedCanvas.h>
#include <TTimer.h>
#include <TGMsgBox.h>
#include <TGStatusBar.h>
#include <TObjArray.h>
#include <TH1.h>
#include <TH2.h>
#include <TPRegexp.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TPad.h>

#include "dim/dic.hxx"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAlib.h"

#include "EditorFrame.h"
#include "LoginDialog.h"
#include "PageSaveDialog.h"
#include "PresenterMainFrame.h"
#include "HistogramIdentifier.h"

using namespace pres;

PresenterMainFrame* gPresenter = 0;

ClassImp(PresenterMainFrame)

PresenterMainFrame::PresenterMainFrame(const char* name,
                                       UInt_t x, UInt_t y, UInt_t w, UInt_t h,
                                       MsgLevel v):
  TGMainFrame(gClient->GetRoot(), w, h),
  m_verbosity(v),
  m_refreshTimer(NULL),
  m_refreshing(false),
  m_clearedHistos(false),
  m_histogramDB(NULL),
  m_analib(NULL),
  m_connectedToHistogramDatabase(false),
  m_msgBoxReturnCode(0),
  m_mainCanvasWidth(w),
  m_mainCanvasHeight(h),
  m_treeNode(NULL),
  m_taskNode(NULL),
  m_algorithmNode(NULL),
  m_histogramSetNode(NULL),
  m_histogramNode(NULL),
  m_pageNode(NULL),
  m_folderItems(NULL),
  m_folderItemsIt(NULL),
  m_folderItem(NULL)
{
#ifndef WIN32
  // RICH colour palette -- broken on w32 - see an eventual patch in v5.18?
  const int colNum = 1024;  // Number of colors in the palette
  int startIndex = 1000;    // starting index of allocated colors
  int palette[colNum];
  float val;
  int iCount = 0;
  const int iStep = 1;

  m_RootColourVector.reserve(colNum);

  // blue(0,0,255) -> cyan(0,255,255)
  for (int i = 0; i < 256; i += iStep) {
    val = i/(float)256;
    TColor* color = new TColor(startIndex + iCount, 0, val, 1);
    m_RootColourVector.push_back(color);
    palette[iCount] = startIndex + iCount;
    iCount++;
  }

  // cyan (0,255,255) -> green (0,255,0)
  for (int i = 0; i < 256; i += iStep){
    val = i/(float)256;
    TColor* color = new TColor(startIndex + iCount, 0, 1, 1 - val);
    m_RootColourVector.push_back(color);
    palette[iCount] = startIndex + iCount;
    iCount++;
  }

  // green (0,255,0) -> yellow (255,255,0)
  for (int i=0; i < 256; i += iStep){
    val = i/(float)256;
    TColor* color = new TColor(startIndex + iCount, val, 1, 0);
    m_RootColourVector.push_back(color);
    palette[iCount] = startIndex + iCount;
    iCount++;
  }

  // yellow (255,255,0) -> red (255,0,0)
  for (int i = 0; i < 256; i += iStep){
    val = i/(float)256;
    TColor* color = new TColor(startIndex+iCount, 1, 1 - val, 0); 
    m_RootColourVector.push_back(color);
    palette[iCount] = startIndex + iCount;
    iCount++;
  }

  gStyle->SetPalette(colNum, palette);
#endif

  gStyle->SetOptStat("emr"); // nemr
  
  SetCleanup(kDeepCleanup);
  
  // allocate vectors:  
  m_histogramTypes.reserve(EstimatedDbHistoCount);
  m_localDatabaseIDS.reserve(EstimatedDbHistoCount);
  m_localDatabasePages.reserve(EstimatedDbPageCount);
  m_localDatabaseFolders.reserve(EstimatedDbFolderCount);
  
  // only one session viewer allowed
  if (gPresenter) { return; }
  if (!m_refreshTimer) m_refreshTimer = new TTimer(TimeoutOfMainTimer);
  m_refreshTimer->TurnOff();
  m_refreshTimer->Connect("Timeout()", "PresenterMainFrame", this,
                         "refreshPage()");
  SetWindowName(name);
  Move(x, y);
//  Resize(w, h);
  gPresenter = this;
  m_dimBrowser = new DimBrowser();
  buildGUI();
  loginOnlineHistDB();
}
PresenterMainFrame::~PresenterMainFrame()
{
  //Clean up used widgets: frames, buttons, layouthints
  //  if (zoomCanvas) delete zoomCanvas;
  
#ifndef WIN32
  std::vector<TColor*>::const_iterator colourIter;
  std::vector<TColor*>::const_iterator colourIterBegin =
      m_RootColourVector.begin();
  std::vector<TColor*>::const_iterator colourIterEnd   =
      m_RootColourVector.end();
  for (colourIter = colourIterBegin; colourIter != colourIterEnd;
         colourIter++) {
    delete *colourIter;     // m_RootColourVector.~X();
  }
#endif  
  
  clearPage();
  Cleanup();
  if (m_histogramDB) { delete m_histogramDB; m_histogramDB = NULL;}
  if (m_analib) {delete m_analib; m_analib = NULL;}
  if (m_dimBrowser) {delete m_dimBrowser; m_dimBrowser = NULL;}
  if (gPresenter == this) { gPresenter = 0; }
}
void PresenterMainFrame::buildGUI()
{
  SetCleanup(kDeepCleanup);
  // set minimun size
  // SetWMSizeHints(400 + 200, 370+50, 2000, 1000, 1, 1);
  m_fileText = new TGHotString("&File");
  m_fileNew = new TGHotString("&New");
  m_fileLoginText = new TGHotString("&Login...");
  m_fileLogoutText = new TGHotString("Log&out...");
  m_fileQuitText = new TGHotString("&Quit");

  // list tree
  m_openedFolderIcon = gClient->GetPicture("ofolder_t.xpm");
  m_closedFolderIcon = gClient->GetPicture("folder_t.xpm");

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


  // File menu
  m_fileMenu = new TGPopupMenu(fClient->GetRoot());
  m_fileMenu->AddEntry(m_fileNew, CLEAR_PAGE_COMMAND);
  m_fileMenu->AddSeparator();
  m_fileMenu->AddEntry(m_fileLoginText, LOGIN_COMMAND);
  m_fileMenu->AddEntry(m_fileLogoutText, LOGOUT_COMMAND);
  m_fileMenu->AddSeparator();
  //   m_fileMenu->AddEntry("&Close Viewer", M_FILE_CLOSE);
  //   m_fileMenu->AddSeparator();
  m_fileMenu->AddEntry(m_fileQuitText, EXIT_COMMAND);

  m_fileMenu->Connect("Activated(Int_t)", "PresenterMainFrame", this,
                      "handleCommand(Command)");

  m_menuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  m_menuBar->AddPopup(m_fileText, m_fileMenu,
                      new TGLayoutHints(kLHintsTop | kLHintsLeft,
                                        0, 4, 0, 0));
  AddFrame(new TGHorizontal3DLine(this),
           new TGLayoutHints(kLHintsTop | kLHintsExpandX));
  AddFrame(m_menuBar,
           new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                             0, 0, 1, 1));
  AddFrame(new TGHorizontal3DLine(this),
           new TGLayoutHints(kLHintsTop | kLHintsExpandX));

  // Toolbar
  m_toolBar = new TGToolBar(this, 60, 20, kHorizontalFrame);
  m_toolbarElement = new ToolBarData_t();

  // New page
  m_toolbarElement->fPixmap = "ed_new.png";
  m_toolbarElement->fTipText = "New page";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = CLEAR_PAGE_COMMAND;
  m_newPageButton = m_toolBar->AddButton(this, m_toolbarElement,
    0);
  m_newPageButton->Connect("Clicked()", "PresenterMainFrame", this,
                           "clearPage()");

  // Save page
  m_toolbarElement->fPixmap = "ed_save.png";
  m_toolbarElement->fTipText = "Save page";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = SAVE_PAGE_COMMAND;
  m_newPageButton = m_toolBar->AddButton(this, m_toolbarElement,
    8);
  m_newPageButton->SetState(kButtonDisabled);
  m_newPageButton->Connect("Clicked()", "PresenterMainFrame", this,
                           "savePage()");

  // Auto n x n layout
  m_toolbarElement->fPixmap = "ed_compile.png";
  m_toolbarElement->fTipText = "Auto n x n histogram tiling layout";
  m_toolbarElement->fStayDown = false;
  m_toolbarElement->fId = AUTO_LAYOUT_COMMAND;
  m_autoCanvasLayoutButton = m_toolBar->AddButton(this,
    m_toolbarElement, 0);
  m_autoCanvasLayoutButton->Connect("Clicked()", "PresenterMainFrame", this,
                                    "autoCanvasLayout()");

  // Login
  m_toolbarElement->fPixmap = "connect.xpm";
  m_toolbarElement->fTipText = "Login";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = LOGIN_COMMAND;
  m_loginButton = m_toolBar->AddButton(this, m_toolbarElement, 8);
  m_loginButton->Connect("Clicked()", "PresenterMainFrame", this,
                         "loginOnlineHistDB()");
  // Logout
  m_toolbarElement->fPixmap = "disconnect.xpm";
  m_toolbarElement->fTipText = "Logout";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = LOGOUT_COMMAND;
  m_logoutButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
  m_logoutButton->SetState(kButtonDisabled);
  m_logoutButton->Connect("Clicked()", "PresenterMainFrame", this,
                          "logoutOnlineHistDB()");
  // Start
  m_toolbarElement->fPixmap = "ed_execute.png";
  m_toolbarElement->fTipText = "Start";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = START_COMMAND;
  m_startRefreshButton = m_toolBar->AddButton(this, m_toolbarElement, 8);
  m_startRefreshButton->Connect("Clicked()", "PresenterMainFrame", this,
                                "startPageRefresh()");
  // Stop
  m_toolbarElement->fPixmap = "ed_interrupt.png";
  m_toolbarElement->fTipText = "Stop";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = STOP_COMMAND;
  m_stopRefreshButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
  m_stopRefreshButton->SetState(kButtonDisabled);
  m_stopRefreshButton->Connect("Clicked()", "PresenterMainFrame", this,
                               "stopPageRefresh()");

  // Clear Histos
  m_toolbarElement->fPixmap = "refresh.xpm";
  m_toolbarElement->fTipText = "Clear histograms";
  m_toolbarElement->fStayDown =  true;
  m_toolbarElement->fId = CLEAR_HISTOS_COMMAND;
  m_clearHistoButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
  m_clearHistoButton->SetState(kButtonDisabled);
  m_clearHistoButton->Connect("Clicked()", "PresenterMainFrame", this,
                              "clearHistos()");

  // Clone Histo
  m_toolbarElement->fPixmap = "tb_find.xpm";
  m_toolbarElement->fTipText = "Zoom in histogram";
  m_toolbarElement->fStayDown = false;
  m_toolbarElement->fId = ZOOM_HISTO_COMMAND;
  m_zoomHistoButton = m_toolBar->AddButton(this, m_toolbarElement, 8);
  m_zoomHistoButton->Connect("Clicked()", "PresenterMainFrame", this,
                             "zoomHistogram()");

  // Edit Histogram properties
  m_toolbarElement->fPixmap = "bld_edit.png";
  m_toolbarElement->fTipText = "Edit histogram options";
  m_toolbarElement->fStayDown = false;
  m_toolbarElement->fId = EDIT_HISTO_COMMAND;
  m_editHistoButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
  m_editHistoButton->Connect("Clicked()", "PresenterMainFrame", this,
                             "editHistogramProperties()");

  // Delete histo from Canvas
  m_toolbarElement->fPixmap = "ed_delete.png";
  m_toolbarElement->fTipText = "Delete selected histogram from Canvas";
  m_toolbarElement->fStayDown = false;
  m_toolbarElement->fId = DELETE_HISTO_FROM_CANVAS_COMMAND;
  m_deleteHistoFromCanvasButton = m_toolBar->AddButton(this,
                                                       m_toolbarElement, 8);
  m_deleteHistoFromCanvasButton->Connect("Clicked()", "PresenterMainFrame",
                                         this, "deleteHistoFromCanvas()");

  AddFrame(m_toolBar,
           new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 0, 0));
  AddFrame(new TGHorizontal3DLine(this),
           new TGLayoutHints(kLHintsTop | kLHintsExpandX));
  //m_toolBar->GetButton(CLEAR_PAGE_COMMAND)->SetState(kButtonDisabled);

  Connect("CloseWindow()", "PresenterMainFrame", this, "CloseWindow()");

  Int_t parts[] = {35, 25, 40};
  m_statusBar = new TGStatusBar(this, 50, 10, kVerticalFrame);
  m_statusBar->SetParts(parts, 3);
  m_statusBar->Draw3DCorner(false);
  AddFrame(m_statusBar, new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0));

  m_statusBar->SetText("Launching Presenter...");

  if (m_dimBrowser->getServers() == 0) {
    m_message = "Sorry, no DIM server found.";
    if (m_verbosity >= Verbose) {
      std::cout << m_message << std::endl;
    }
    m_statusBar->SetText(m_message.c_str());
    new TGMsgBox(fClient->GetRoot(), this, "DIM Error", m_message.c_str(),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  } else {
    m_statusBar->SetText("Ok.");
  }

  // Load EditorFrame for a start...
  m_ef = new EditorFrame(this, m_mainCanvasWidth, m_mainCanvasHeight,
                         *m_dimBrowser, Silent);
  AddFrame(m_ef,
           new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
                             2, 0, 1, 2));
  editorFrame = dynamic_cast<EditorFrame*>(m_ef);
  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputPrimaryApplicationModal);
  MapSubwindows();
  Resize();
  MapWindow();
}
void PresenterMainFrame::CloseWindow()
{
  if (m_refreshing && m_refreshTimer) {
    m_refreshTimer->Stop();
  }
  gApplication->Terminate();
}
void PresenterMainFrame::doSomething()
{
}
void PresenterMainFrame::handleCommand(Command cmd)
{
  // ROOT GUI workaround: slot mechanism differs on different signals from
  // different widgets... void* 4 messages :-(
  if (X_ENIGMA_COMMAND == cmd) {
    TGButton* btn = reinterpret_cast<TGButton*>(gTQSender);
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
    case CLEAR_PAGE_COMMAND:
      clearPage();
      break;
    case LOGOUT_COMMAND:
      logoutOnlineHistDB();
      break;
    case LOGIN_COMMAND:
      loginOnlineHistDB();
      break;
    default:
      if (m_verbosity >= Debug) {
        std::cout << "zut. TGButton WidgetId from gTQSender is corrupt"
          << std::endl;
      }
      break;
  }
}
void PresenterMainFrame::enableAutoCanvasLayoutBtn()
{
  m_autoCanvasLayoutButton->SetState(kButtonEngaged);
  m_autoCanvasLayoutButton->SetState(kButtonUp);
}
void PresenterMainFrame::disableAutoCanvasLayoutBtn()
{
  m_autoCanvasLayoutButton->SetState(kButtonDisabled);
}
void PresenterMainFrame::autoCanvasLayout()
{
  editorFrame->autoCanvasLayout();
}
void PresenterMainFrame::deleteHistoFromCanvas()
{
  editorFrame->deleteSelectedHistoFromCanvas();
}
void PresenterMainFrame::clearPage()
{
//  new TGMsgBox(fClient->GetRoot(), this, "Clear Page",
//        "Do you really want to clear the current page?",
//        kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
//    switch (m_msgBoxReturnCode) {
//      case kMBNo:
//        return;
//  }
  stopPageRefresh();
  editorFrame->editorCanvas->SetEditable(true);
  editorFrame->clearHistosOnPage();

  if (m_verbosity >= Verbose) {
    std::cout << "clearCanvas." << std::endl;
  }
}
void PresenterMainFrame::savePage()
{
  if (m_connectedToHistogramDatabase) {
    fClient->WaitFor(new PageSaveDialog(this, 493, 339, m_verbosity));
//  m_newPageButton->SetState(kButtonDisabled);

    editorFrame->refreshPagesDBListTree();
    editorFrame->refreshHistoDBListTree();
//    statusBar()
  }
}
bool PresenterMainFrame::connectToDatabase(std::string dbPassword,
  std::string dbUsername, std::string dbName)
{
  try {
    if (m_histogramDB && m_connectedToHistogramDatabase) {
      delete m_histogramDB;
      m_histogramDB = NULL;
      
    }
    m_histogramDB = new OnlineHistDB(dbPassword, dbUsername,
      dbName);
    m_analib = new OMAlib(m_histogramDB);

    m_dbName = dbName;
    m_statusBar->SetText(dbUsername.c_str(), 2);

    m_message = "Successfully connected to OnlineHistDB.";
    m_statusBar->SetText(m_message.c_str());

    if (m_verbosity >= Verbose) {
      std::cout << m_message << std::endl;
    }

    m_logoutButton->SetState(kButtonUp);
    m_loginButton->SetState(kButtonDisabled);
    m_newPageButton->SetState(kButtonEngaged);
    m_newPageButton->SetState(kButtonUp);

    m_fileMenu->DisableEntry(LOGIN_COMMAND);
    m_fileMenu->EnableEntry(LOGOUT_COMMAND);

//    showHistogramDatabaseTools();
//    statusBar()->message(tr("Successfully connected to OnlineHistDB."));
    m_connectedToHistogramDatabase = true;

    m_histogramDB->setExcLevel(AllExceptions);
    m_histogramDB->setDebug(m_verbosity);
  } catch (SQLException sqlException) {
    // TODO: add error logging backend - MsgStream?
    m_message = sqlException.getMessage();
    m_statusBar->SetText(m_message.c_str());
    if (m_verbosity >= Verbose) {
      std::cout << m_message << std::endl;
    }

    new TGMsgBox(fClient->GetRoot(), this, "Login Error",
                 Form("Could not connect to the OnlineHistDB server:\n\n%s\n"
                      "This program cannot function correctly",
                      m_message.c_str()),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);

    //  TODO: disable/hideHistogramDatabaseTools();
    m_connectedToHistogramDatabase = false;
  }
  return m_connectedToHistogramDatabase;
}
void PresenterMainFrame::loginOnlineHistDB()
{
  //Modal dialog
  fClient->WaitFor(new LoginDialog(this, 350, 310));

  //  editorFrame->histoDBFilterComboBox->HideFrame();
  if (m_connectedToHistogramDatabase) {
    editorFrame->showDBTools();
  } else {
    editorFrame->hideDBTools();
  }
  //  TODO: refactor refresh* methods to use only 1 DB readout...
  editorFrame->refreshPagesDBListTree();
  editorFrame->refreshHistoDBListTree();

  clearPage();
  editorFrame->enableLoadPage();
}
void PresenterMainFrame::logoutOnlineHistDB()
{
  if (m_histogramDB && m_connectedToHistogramDatabase) {
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
  if (m_analib) {delete m_analib; m_analib = NULL;}  
  m_connectedToHistogramDatabase = false;
  clearPage();
  m_message = "Disconnected from histogramDB";
  m_statusBar->SetText(m_message.c_str());
  if (m_verbosity >= Verbose) {
    std::cout << m_message << std::endl;
  }

  m_loginButton->SetState(kButtonUp);
  m_logoutButton->SetState(kButtonDisabled);
  m_newPageButton->SetState(kButtonDisabled);

  m_fileMenu->EnableEntry(LOGIN_COMMAND);
  m_fileMenu->DisableEntry(LOGOUT_COMMAND);

  //  editorFrame->histoDBFilterComboBox->ShowFrame();
  editorFrame->hideDBTools();

  //  TODO: refactor refresh* methods to use only 1 DB readout...
  editorFrame->refreshPagesDBListTree();
  editorFrame->refreshHistoDBListTree();

  clearPage();
}
void PresenterMainFrame::startPageRefresh()
{
  m_message = "Refresh started.";
  m_statusBar->SetText(m_message.c_str());
  if (m_verbosity >= Verbose) {
    std::cout << m_message << std::endl;
  }

  m_stopRefreshButton->SetState(kButtonUp);
  m_startRefreshButton->SetState(kButtonDisabled);
  m_clearHistoButton->SetState(kButtonUp);

  //  m_refreshTimer->Start(50);
  editorFrame->enablePageRefresh();
  m_refreshTimer->TurnOn();
  m_refreshing = true;
}
void PresenterMainFrame::stopPageRefresh()
{
  editorFrame->disablePageRefresh();
  if (m_refreshTimer) {
    m_message = "Refresh stopped.";
    m_statusBar->SetText(m_message.c_str());
    if (m_verbosity >= Verbose) {
      std::cout << m_message << std::endl;
    }

    m_startRefreshButton->SetState(kButtonUp);
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_clearHistoButton->SetState(kButtonDisabled);

    //    m_refreshTimer->Stop();
    if (m_clearedHistos) { clearHistos();}
    m_refreshTimer->TurnOff();
    m_refreshing = false;
  }
}
void PresenterMainFrame::clearHistos()
{
  if (m_clearedHistos) {
    m_message = "Reverted to integral.";
    m_statusBar->SetText(m_message.c_str());
    if (m_verbosity >= Verbose) {
      std::cout << m_message << std::endl;
    }
    m_clearHistoButton->SetState(kButtonUp);
    m_clearedHistos = false;

    editorFrame->disableClearHistos();

  } else {
    m_message = "Histograms cleared.";
    m_statusBar->SetText(m_message.c_str());
    m_clearHistoButton->SetState(kButtonDown);
    m_clearedHistos = true;

    editorFrame->enableClearHistos();
  }
}
void PresenterMainFrame::refreshPage()
{
  if (m_verbosity >= Verbose) {
    std::cout << "refreshing." << std::endl;
  }
  editorFrame->refreshPage();
  //  gSystem->ProcessEvents();  // handle GUI events
}
OMAlib* PresenterMainFrame::getAnalib()
{
  return m_analib;
}
OnlineHistDB* PresenterMainFrame::getDatabase()
{
  if (!m_connectedToHistogramDatabase) {
    loginOnlineHistDB();
  }
  return m_histogramDB;
}
bool PresenterMainFrame::isConnectedToDatabase()
{
  return m_connectedToHistogramDatabase;
}
void PresenterMainFrame::readFromHistoDatabase(TGListTree* listView,
  FilterCriteria filterCriteria, bool histograms)
{
  if (m_connectedToHistogramDatabase && listView) {
    try {
      m_treeNode = listView->GetFirstItem();
      listView->RenameItem(m_treeNode, TString(m_dbName));
      m_localDatabaseFolders.clear();
      m_localDatabaseIDS.clear();
      m_histogramTypes.clear();
      deleteTreeChildrenItemsUserData(m_treeNode);
      listView->DeleteChildren(m_treeNode);
      switch (filterCriteria) {
        case ByFolderAndPage:
          m_histogramDB->getPageFolderNames(m_localDatabaseFolders,
            "_ALL_");
          break;
        case BySubsystem:
          m_histogramDB->getSubsystems(m_localDatabaseFolders);
          break;
        case ByTask:
          m_histogramDB->getTasks(m_localDatabaseFolders);
          break;
        case ByHistogramsWithAnalysis:
          m_histogramDB->getHistogramsWithAnalysis(NULL,
            &m_localDatabaseIDS, &m_histogramTypes);
          break;
        case ByAnalysisHistograms:
          m_histogramDB->getAnalysisHistograms(NULL,
            &m_localDatabaseIDS, &m_histogramTypes);
          break;
        case AllHistos:
          m_histogramDB->getAllHistograms(NULL,
            &m_localDatabaseIDS, &m_histogramTypes);
          break;
        default:
          break;
      }
      std::sort(m_localDatabaseFolders.begin(),
                m_localDatabaseFolders.end());

      if (filterCriteria == ByFolderAndPage ||
          filterCriteria == BySubsystem ||
          filterCriteria == ByTask) {
        for (m_folderIt = m_localDatabaseFolders.begin();
             m_folderIt != m_localDatabaseFolders.end(); ++m_folderIt) {
          m_folderItems = TString(*m_folderIt).Tokenize("/");
          m_folderItemsIt = m_folderItems->MakeIterator();
          m_treeNode = listView->GetFirstItem();
          while (m_folderItem = m_folderItemsIt->Next()) {
            if (listView->FindChildByName(m_treeNode,
              m_folderItem->GetName())) {
                m_treeNode = listView->FindChildByName(m_treeNode,
                  m_folderItem->GetName());
            } else {
              m_treeNode = listView->AddItem(m_treeNode,
                m_folderItem->GetName());
            }
            m_treeNode->SetUserData(0);
            if (m_folderItem == m_folderItems->Last() &&
                filterCriteria == ByFolderAndPage) {
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
                m_pageNode->SetUserData(new TString(*m_pageIt));
                listView->CheckItem(m_pageNode, false);
                setTreeNodeIcon(m_pageNode, PAGE);
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
                       filterCriteria != ByFolderAndPage) {
              if (histograms) {
                m_localDatabaseIDS.clear();
                m_histogramTypes.clear();
                if (filterCriteria == ByTask) {
                  m_histogramDB->getHistogramsByTask(*m_folderIt, NULL,
                                                     &m_localDatabaseIDS,
                                                     &m_histogramTypes);
                } else if (filterCriteria == BySubsystem) {
                  m_histogramDB->getHistogramsBySubsystem(*m_folderIt,
                    NULL, &m_localDatabaseIDS, &m_histogramTypes);
                }
                fillTreeNodeWithHistograms(listView, m_treeNode,
                  &m_localDatabaseIDS, &m_histogramTypes);
              }
            }
          }
          m_folderItems->Delete();
          delete m_folderItems;
          m_folderItems = NULL;
          delete m_folderItemsIt;
          m_folderItemsIt = NULL;
        }
      } else if (filterCriteria == ByHistogramsWithAnalysis ||
                 filterCriteria == ByAnalysisHistograms ||
                 filterCriteria == AllHistos) {
          fillTreeNodeWithHistograms(listView, listView->GetFirstItem(),
                                     &m_localDatabaseIDS, &m_histogramTypes);
      }
      fClient->NeedRedraw(listView);
      m_message = "Histograms read from Database.";
    } catch (SQLException sqlException) {
        // TODO: add error logging backend
        m_message = sqlException.getMessage();
        m_statusBar->SetText(m_message.c_str());
        if (m_verbosity >= Verbose) {
          std::cout << m_message << std::endl;
        }

        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
            Form("OnlineHistDB server:\n\n%s\n",
                 m_message.c_str()),
            kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
  } else if (!m_connectedToHistogramDatabase) {
    m_message = "Histograms not read from Database.";
  }
  m_statusBar->SetText(m_message.c_str());
  if (m_verbosity >= Verbose) {
    std::cout << m_message << std::endl;
  }
}
void PresenterMainFrame::fillTreeNodeWithHistograms(TGListTree* listView,
  TGListTreeItem* node, std::vector<std::string>*  histogramList,
  std::vector<std::string>*  histogramTypes)
{
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
      setTreeNodeIcon(m_taskNode, TASK);
    }
    m_taskNode->SetUserData(0);
    // Algorithmname
    m_algorithmNode = m_taskNode;
    if (listView->FindChildByName(m_algorithmNode,
      histogramIdentifier.algorithmName().c_str())) {
        m_algorithmNode = listView->FindChildByName(
          m_algorithmNode, histogramIdentifier.algorithmName().c_str());
    } else {
      m_algorithmNode = listView->AddItem(m_algorithmNode,
        histogramIdentifier.algorithmName().c_str());
      setTreeNodeIcon(m_algorithmNode, ALGORITHM);
    }
    m_algorithmNode->SetUserData(0);
    // histogramsetname
    m_histogramSetNode = m_algorithmNode;
    if (histogramIdentifier.isFromHistogramSet()) {
      if (listView->FindChildByName(m_histogramSetNode,
        histogramIdentifier.histogramSetName().c_str())) {
          m_histogramSetNode = listView->FindChildByName(
            m_histogramSetNode,
            histogramIdentifier.histogramSetName().c_str());
      } else {
        m_histogramSetNode = listView->AddItem(m_histogramSetNode,
          histogramIdentifier.histogramSetName().c_str());
        setTreeNodeIcon(m_histogramSetNode, SET);
      }
      m_histogramSetNode->SetUserData(0);
    }
    // Histogramame
    m_histogramNode = m_histogramSetNode;
    m_histogramIdItems = TString(histogramIdentifier.histogramName()).
                                 Tokenize("/");
    m_histogramIdItemsIt = m_histogramIdItems->MakeIterator();

    while (m_histogramIdItem = m_histogramIdItemsIt->Next()) {
      if (m_histogramIdItem != m_histogramIdItems->Last()) {
        if (listView->FindChildByName(m_histogramNode,
          m_histogramIdItem->GetName())) {
            m_histogramNode = listView->FindChildByName(m_histogramNode,
              m_histogramIdItem->GetName());
        } else {
          m_histogramNode = listView->AddItem(m_histogramNode,
            m_histogramIdItem->GetName());
        }
        m_histogramNode->SetUserData(0);
        setTreeNodeIcon(m_histogramNode, LEVEL);
      }
      if (m_histogramIdItem == m_histogramIdItems->Last()) {
        m_histogramNode = listView->AddItem(m_histogramNode,
          m_histogramIdItem->GetName());
        listView->SetCheckBox(m_histogramNode, kTRUE);
        listView->ToggleItem(m_histogramNode);
        setTreeNodeIcon(m_histogramNode, *m_histogramType);
        m_histogramNode->SetUserData(new TString(*m_histogramIt));
      }
    }
    m_histogramIdItems->Delete();
    delete m_histogramIdItems;
    m_histogramIdItems = NULL;
    delete m_histogramIdItemsIt;
    m_histogramIdItemsIt = NULL;
  }
  sortTreeChildrenItems(listView, node);
}
std::string PresenterMainFrame::getDBName()
{
  return m_dbName;
}
// TODO: move all below to an eventual static helper class
void PresenterMainFrame::setTreeNodeIcon(TGListTreeItem* node,
  std::string type)
{
  const TGPicture*  m_icon;
  if (H1D == type) {
    m_icon = m_iconH1D;
  } else if (H2D == type) {
    m_icon = m_iconH2D;
  } else if (P1D == type ||
             HPD == type ||
             P2D == type ) {
    m_icon = m_iconProfile;
  } else if (CNT == type) {
    m_icon = m_iconCounter;
  } else if (PAGE == type) {
    m_icon = m_iconPage;
  } else if (TASK == type) {
    m_icon = m_iconTask;
  } else if (ALGORITHM == type) {
    m_icon = m_iconAlgorithm;
  } else if (SET == type) {
    m_icon = m_iconSet;
  } else if (LEVEL == type) {
    m_icon = m_iconLevel;
  } else {
    m_icon = m_iconQuestion;
  }
  if (node) { node->SetPictures(m_icon, m_icon); }
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
  while(node) {
    if (node->GetFirstChild()) {
      checkedTreeItemsChildren(node->GetFirstChild(), selected);
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::checkedTreeItemsChildren(TGListTreeItem *node,
  TGListTree* selected)
{
  while (node) {
    if (node->IsChecked() && node->GetUserData()) {
      selected->AddItem(0, 0, node->GetUserData());
    }
    if (node->GetFirstChild()) {
      checkedTreeItemsChildren(node->GetFirstChild(), selected);
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::checkTreeChildrenItems(TGListTreeItem* node,
  bool check)
{
  if (node && NULL == node->GetFirstChild() &&
      node->GetUserData()) {
    node->CheckItem(check);
  }
  if (node && node->GetFirstChild()) {
    checkTreeChildrenItemsChildren(node->GetFirstChild(), check);
  }
}
void PresenterMainFrame::checkTreeChildrenItemsChildren(TGListTreeItem* node,
  bool check)
{
  while (node) {
    if (NULL == node->GetFirstChild() && node->GetUserData()) {
      node->CheckItem(check);
    }
    if (node->GetFirstChild()) {
      checkTreeChildrenItemsChildren(node->GetFirstChild(), check);
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::collapseTreeChildrenItems(TGListTree* treeList,
  TGListTreeItem* node)
{
  if (node && treeList) {
    treeList->CloseItem(node);
  }
  while (node) {
    if (node->GetFirstChild()) {
      collapseTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::collapseTreeChildrenItemsChildren(
  TGListTree* treeList, TGListTreeItem* node)
{
  while (node) {
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
  if (node && treeList) {
    treeList->SortChildren(node);
  }
  while (node) {
    if (node->GetFirstChild()) {
      sortTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::sortTreeChildrenItemsChildren(
  TGListTree* treeList, TGListTreeItem* node)
{
  while (node) {
    if (node && treeList) {
      treeList->SortChildren(node);
    }
    if (node->GetFirstChild()) {
      sortTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::deleteTreeChildrenItemsUserData(TGListTreeItem* node)
{
  if (node && node->GetUserData()) {
    delete (TString*)(node->GetUserData());
    node->SetUserData(NULL);  
  }
  if (node && node->GetFirstChild()) {
    deleteTreeChildrenItemsUserDataChildren(node->GetFirstChild());
  }
}
void PresenterMainFrame::deleteTreeChildrenItemsUserDataChildren(TGListTreeItem* node)
{
  while (node) {
    if (node->GetUserData()) {
      delete (TString*)(node->GetUserData());
      node->SetUserData(NULL);
    }
    if (node->GetFirstChild()) {
      deleteTreeChildrenItemsUserDataChildren(node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::setStatusBarText(const char* text, int slice)
{
  m_statusBar->SetText(text, slice);
}
TH1* PresenterMainFrame::getSelectedHisto()
{
  if (gPad) {
    TIter nextPadElem(gPad->GetListOfPrimitives());
    TObject *histoCandidate;
    while (histoCandidate = nextPadElem()) {
      if (histoCandidate->InheritsFrom(TH1::Class())) {
// TODO: When refhistos will come... editorFrame->
// 0 == std::string((*m_DbHistosOnPageIt)->rootHistogram->GetName()).compare(histoCandidate->GetName())) {
        return dynamic_cast<TH1*>(histoCandidate);
      }
    } return 0;
  } else {
    return 0;
  }
}
void PresenterMainFrame::editHistogramProperties()
{
  bool stopped = false;
  if (m_refreshing) {
    stopPageRefresh();
    stopped = true;
  }
  if (TH1* selectedHisto = getSelectedHisto()) {
    selectedHisto->DrawPanel();
  }
  if (stopped) {
//    startPageRefresh();
  }
}
void PresenterMainFrame::zoomHistogram()
{
  // TODO: stopped needs to be killed off...
  bool stopped = false;
  if (m_refreshing) {
    stopPageRefresh();
    stopped = true;
  }
  if (TH1* selectedHisto = getSelectedHisto()) {
    TCanvas* zoomCanvas =  new TCanvas();
//    zoomCanvas->SetBit(kCanDelete);
    zoomCanvas->cd();
    zoomCanvas->SetTitle(selectedHisto->GetTitle());
    selectedHisto->DrawCopy();
    zoomCanvas->ToggleEventStatus();
    zoomCanvas->ToggleAutoExec();
    zoomCanvas->ToggleToolBar();
    zoomCanvas->ToggleEditor();
  }
  if (stopped) {
    startPageRefresh();
  }
}
void PresenterMainFrame::fitPanel()
{
  bool stopped = false;
  if (m_refreshing) {
    stopPageRefresh();
    stopped = true;
  }
  if (TH1* selectedHisto = getSelectedHisto()) {
    selectedHisto->FitPanel();
  }
  if (stopped) {
//  startPageRefresh();
  }
}
