#include <iostream>
#include <vector>

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
#include <TCanvas.h>
#include <TPRegexp.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TPad.h>

#include "dim/dic.hxx"
#include "OnlineHistDB/OnlineHistDB.h"

#include "EditorFrame.h"
#include "LoginDialog.h"
#include "PageSaveDialog.h"
#include "PresenterMainFrame.h"

using namespace pres;

PresenterMainFrame* gPresenter = 0;

ClassImp(PresenterMainFrame)

PresenterMainFrame::PresenterMainFrame(const char* name, UInt_t x, UInt_t y,
                                       UInt_t w, UInt_t h, MsgLevel v) :
  TGMainFrame(gClient->GetRoot(), w, h),
  m_verbosity(v),
  m_refreshTimer(NULL),
  m_refreshing(false),
  m_clearedHistos(false),
  m_connectedToHistogramDatabase(false),
  m_histogramDB(NULL),
  m_msgBoxReturnCode(0),
  m_mainCanvasWidth(w),
  m_mainCanvasHeight(h)
{
  // RICH colour palette -- broken on w32?
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

  //green (0,255,0) -> yellow (255,255,0)
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
  
  SetCleanup(kDeepCleanup);
  // only one session viewer allowed
  if (gPresenter) {return;}
  
  if (!m_refreshTimer) m_refreshTimer = new TTimer(2000);
  m_refreshTimer->TurnOff();
  m_refreshTimer->Connect("Timeout()", "PresenterMainFrame",
                          this, "refreshPage()");
  SetWindowName(name);
  Move(x, y);
  Resize(w, h);
  gPresenter = this;
  
//  gStyle->SetOptStat("emrou");

  //TODO: DimExitHandler
  
  m_dimBrowser = new DimBrowser();
  buildGUI();
  loginOnlineHistDB();
}

PresenterMainFrame::~PresenterMainFrame()
{
//Clean up used widgets: frames, buttons, layouthints
//  if (zoomCanvas) delete zoomCanvas;

  std::vector<TColor*>::const_iterator colourIter;
  std::vector<TColor*>::const_iterator colourIterBegin = m_RootColourVector.begin();
  std::vector<TColor*>::const_iterator colourIterEnd   = m_RootColourVector.end();
  for (colourIter = colourIterBegin; colourIter != colourIterEnd; colourIter++) {
    delete *colourIter;    
  }

  clearPage();
  Cleanup();
  if (m_histogramDB && m_connectedToHistogramDatabase) { delete m_histogramDB; }
  if (m_dimBrowser) {delete m_dimBrowser; }
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

// load icons
//  m_picturePool = new TGPicturePool(fClient,"./");
//  m_fitIcon = m_picturePool->GetPicture("fitIcon.png");
  
  // list tree
  m_openedFolderIcon = gClient->GetPicture("ofolder_t.xpm");
  m_closedFolderIcon = gClient->GetPicture("folder_t.xpm");
  
  m_iconH1D = gClient->GetPicture("h1_t.xpm");
  m_iconH2D = gClient->GetPicture("h2_t.xpm");
  m_iconProfile = gClient->GetPicture("profile_t.xpm");
  m_iconCounter = gClient->GetPicture("c_src_t.xpm");
  m_iconPage = gClient->GetPicture("bld_paste_into.png");
  m_iconQuestion = gClient->GetPicture("mdi_help.xpm");

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
  AddFrame(m_menuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft |
                                        kLHintsExpandX, 0, 0, 1, 1));
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
    m_newPageButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
    m_newPageButton->Connect("Clicked()", "PresenterMainFrame",
                         this, "clearPage()");
                         
    // Save page
    m_toolbarElement->fPixmap = "ed_save.png";
    m_toolbarElement->fTipText = "Save page";
    m_toolbarElement->fStayDown =  false;
    m_toolbarElement->fId = SAVE_PAGE_COMMAND;
    m_newPageButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
    m_newPageButton->SetState(kButtonDisabled);
    m_newPageButton->Connect("Clicked()", "PresenterMainFrame",
                         this, "savePage()");

    // Auto n x n layout                             
    m_toolbarElement->fPixmap = "ed_compile.png";
    m_toolbarElement->fTipText = "n x n layout";
    m_toolbarElement->fStayDown = false;
    m_toolbarElement->fId = AUTO_LAYOUT_COMMAND;
    m_autoCanvasLayoutButton = m_toolBar->AddButton(this, m_toolbarElement, 8);
    m_autoCanvasLayoutButton->Connect("Clicked()", "PresenterMainFrame",
                               this, "autoCanvasLayout()");                                               
    // Login
    m_toolbarElement->fPixmap = "connect.xpm";
    m_toolbarElement->fTipText = "Login";
    m_toolbarElement->fStayDown =  false;
    m_toolbarElement->fId = LOGIN_COMMAND;
    m_loginButton = m_toolBar->AddButton(this, m_toolbarElement, 8);
    m_loginButton->Connect("Clicked()", "PresenterMainFrame",
                           this, "loginOnlineHistDB()");
    // Logout
    m_toolbarElement->fPixmap = "disconnect.xpm";
    m_toolbarElement->fTipText = "Logout";
    m_toolbarElement->fStayDown =  false;
    m_toolbarElement->fId = LOGOUT_COMMAND;
    m_logoutButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
    m_logoutButton->SetState(kButtonDisabled);
    m_logoutButton->Connect("Clicked()", "PresenterMainFrame",
                            this, "logoutOnlineHistDB()");          
    // Start
    m_toolbarElement->fPixmap = "ed_execute.png";
    m_toolbarElement->fTipText = "Start";
    m_toolbarElement->fStayDown =  false;
    m_toolbarElement->fId = START_COMMAND;
    m_startRefreshButton = m_toolBar->AddButton(this, m_toolbarElement, 8);
    m_startRefreshButton->Connect("Clicked()", "PresenterMainFrame",
                           this, "startPageRefresh()");          
    // Stop
    m_toolbarElement->fPixmap = "ed_interrupt.png";
    m_toolbarElement->fTipText = "Stop";
    m_toolbarElement->fStayDown =  false;
    m_toolbarElement->fId = STOP_COMMAND;
    m_stopRefreshButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_stopRefreshButton->Connect("Clicked()", "PresenterMainFrame",
                          this, "stopPageRefresh()");
    // Clear Histos
    m_toolbarElement->fPixmap = "refresh.xpm";
    m_toolbarElement->fTipText = "Clear histograms";
    m_toolbarElement->fStayDown =  true;
    m_toolbarElement->fId = CLEAR_HISTOS_COMMAND;
    m_clearHistoButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
    m_clearHistoButton->SetState(kButtonDisabled);
    m_clearHistoButton->Connect("Clicked()", "PresenterMainFrame",
                           this, "clearHistos()");
                           
    // Clone Histo
    m_toolbarElement->fPixmap = "tb_find.xpm";
    m_toolbarElement->fTipText = "Zoom in histogram";
    m_toolbarElement->fStayDown = false;
    m_toolbarElement->fId = ZOOM_HISTO_COMMAND;
    m_zoomHistoButton = m_toolBar->AddButton(this, m_toolbarElement, 8);
    m_zoomHistoButton->Connect("Clicked()", "PresenterMainFrame",
                               this, "zoomHistogram()");                

    // Edit Histogram properties
    m_toolbarElement->fPixmap = "bld_edit.png";
    m_toolbarElement->fTipText = "Edit histogram options";
    m_toolbarElement->fStayDown = false;
    m_toolbarElement->fId = EDIT_HISTO_COMMAND;
    m_editHistoButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
    m_editHistoButton->Connect("Clicked()", "PresenterMainFrame",
                               this, "editHistogramProperties()");

    // Fit Panel  - custom icon...
//    TString iconName(gProgPath);
//    #ifdef R__WIN32
//      iconName += "\\icons\\";
//    #else
//      iconName += "/icons/";
//    #endif
    //iconName += "fitIcon.png";
//  m_toolbarElement->fPixmap = "./fitIcon.png"; //iconName.Data();;
    m_toolbarElement->fPixmap = "package.xpm";
    m_toolbarElement->fTipText = "Fit panel";
    m_toolbarElement->fStayDown = false;
    m_toolbarElement->fId = FIT_PANEL_COMMAND;
    m_fitPanelButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
    m_fitPanelButton->Connect("Clicked()", "PresenterMainFrame",
                               this, "fitPanel()");
    m_fitPanelButton->SetState(kButtonDisabled);
                               
   // TODO: X del
                                                

  AddFrame(m_toolBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX,
           0, 0, 0, 0));
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
    if (m_verbosity >= VERBOSE) { std::cout << m_message << std::endl; }
      m_statusBar->SetText(m_message.c_str());
      new TGMsgBox(fClient->GetRoot(), this, "DIM Error",
               m_message.c_str(), kMBIconExclamation,
               kMBOk, &m_msgBoxReturnCode);
  } else {
    m_statusBar->SetText("Ok.");
  }
  
  // Load EditorFrame for a start...
  m_ef = new EditorFrame(this, m_mainCanvasWidth, m_mainCanvasHeight, *m_dimBrowser, SILENT);
  AddFrame(m_ef, new TGLayoutHints(kLHintsTop | kLHintsExpandX |
                                 kLHintsExpandY, 2, 0, 1, 2));
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
      if (m_verbosity >= DEBUG) {
        std::cout << "zut. TGButton WidgetId from gTQSender is corrupt" <<
                     std::endl;
      }
      break;
  }
}

void PresenterMainFrame::enableAutoCanvasLayoutBtn() {
  m_autoCanvasLayoutButton->SetState(kButtonEngaged);
  m_autoCanvasLayoutButton->SetState(kButtonUp);

}

void PresenterMainFrame::disableAutoCanvasLayoutBtn() {
  m_autoCanvasLayoutButton->SetState(kButtonDisabled);
}

void PresenterMainFrame::autoCanvasLayout()
{
  editorFrame->autoCanvasLayout();
}

void PresenterMainFrame::clearPage()
{
  
  stopPageRefresh();
  
  // TODO: share this stuff a bit prettier...
  editorFrame->editorCanvas->SetEditable(true);
  editorFrame->clearHistosOnPage();
//  editorFrame->editorEmbCanvas->GetCanvas()->Divide(1,1);
//  editorFrame->editorEmbCanvas->GetCanvas()->Clear();
//  editorFrame->editorEmbCanvas->GetCanvas()->Update();
  
  if (m_verbosity >= VERBOSE) { std::cout << "clearCanvas." << std::endl; }
}

void PresenterMainFrame::savePage()
{
  if (m_connectedToHistogramDatabase) {
    
  fClient->WaitFor(new PageSaveDialog(this, 493,339, m_verbosity));
  
//  m_newPageButton->SetState(kButtonDisabled);
  
  editorFrame->refreshPagesDBListTree();
  editorFrame->refreshHistoDBListTree(byTask);
  editorFrame->refreshDimSvcListTree();

  //      statusBar()
  }
}

bool PresenterMainFrame::connectToDatabase(std::string dbPassword, std::string dbUsername, std::string dbName)
{
 try {
  if (m_histogramDB && m_connectedToHistogramDatabase) { delete m_histogramDB; }
  m_histogramDB = new OnlineHistDB(dbPassword, dbUsername, dbName);
  m_dbName = dbName;
  m_statusBar->SetText(dbUsername.c_str(), 2);
  
  m_message = "Successfully connected to OnlineHistDB.";
  m_statusBar->SetText(m_message.c_str());
  
  if (m_verbosity >= VERBOSE) { std::cout << m_message << std::endl; }
    
  m_logoutButton->SetState(kButtonUp);
  m_loginButton->SetState(kButtonDisabled);
  m_newPageButton->SetState(kButtonEngaged);
  m_newPageButton->SetState(kButtonUp);

  m_fileMenu->DisableEntry(LOGIN_COMMAND);
  m_fileMenu->EnableEntry(LOGOUT_COMMAND);

//    showHistogramDatabaseTools();
//    statusBar()->message(tr("Successfully connected to OnlineHistDB."));
  m_connectedToHistogramDatabase = true;

  m_histogramDB->setExcLevel(ALL_EXCEPTIONS);
  m_histogramDB->setDebug(m_verbosity);

  } catch (SQLException sqlException) {
    // TODO: add error logging backend - MsgStream?
    m_message = sqlException.getMessage();
    m_statusBar->SetText(m_message.c_str());
    if (m_verbosity >= VERBOSE) { std::cout << m_message << std::endl; }
    
    new TGMsgBox(fClient->GetRoot(), this, "Login Error",
                 Form("Could not connect to the OnlineHistDB server:\n\n%s\n"
                 "This program cannot function correctly",               
                 m_message.c_str()), kMBIconExclamation,
                 kMBOk, &m_msgBoxReturnCode);

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
  editorFrame->refreshHistoDBListTree(byTask);
  editorFrame->refreshDimSvcListTree();
  
  clearPage();
  editorFrame->enableLoadPage();
  
}

void PresenterMainFrame::logoutOnlineHistDB()
{
  clearPage();
  if (m_histogramDB && m_connectedToHistogramDatabase) { delete m_histogramDB; }
  m_connectedToHistogramDatabase = false;
  m_message = "Disconnected from histogramDB";
  m_statusBar->SetText(m_message.c_str());  
  if (m_verbosity >= VERBOSE) { std::cout << m_message << std::endl; }
 
  m_loginButton->SetState(kButtonUp);
  m_logoutButton->SetState(kButtonDisabled);
  m_newPageButton->SetState(kButtonDisabled);
  
  m_fileMenu->EnableEntry(LOGIN_COMMAND);
  m_fileMenu->DisableEntry(LOGOUT_COMMAND);

//  editorFrame->histoDBFilterComboBox->ShowFrame();
  editorFrame->hideDBTools();

  //  TODO: refactor refresh* methods to use only 1 DB readout...  
  editorFrame->refreshPagesDBListTree();
  editorFrame->refreshHistoDBListTree(byTask);
  editorFrame->refreshDimSvcListTree();

  
  clearPage();
}

void PresenterMainFrame::startPageRefresh()
{
  m_message = "Refresh started.";
  m_statusBar->SetText(m_message.c_str());  
  if (m_verbosity >= VERBOSE) { std::cout << m_message << std::endl; }
  
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
    if (m_verbosity >= VERBOSE) { std::cout << m_message << std::endl; }
    
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
    if (m_verbosity >= VERBOSE) { std::cout << m_message << std::endl; }
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
  if (m_verbosity >= VERBOSE) {
//    std::cout << "refreshing." << std::endl;    
  }
  editorFrame->refreshPage();
//  gSystem->ProcessEvents();  // handle GUI events
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

void PresenterMainFrame::readFromHistoDatabase(TGListTree* listView, FilterCriteria filterCriteria, bool histograms)
{ 

// TODO: upgrade to real tree/path:  
//char path[1024];
//listView->GetPathnameFromItem(node, path);  
  std::vector<std::string>      localDatabaseFolders;
  std::vector<std::string>::const_iterator folderIt;
  
  std::vector<std::string>      localDatabasePages;
  std::vector<std::string>::const_iterator pageIt;
  
  std::vector<std::string>      localDatabaseIDS;
  std::vector<std::string>::const_iterator histogramIt;
  
  // Histogram type:
  std::vector<std::string>      histogramTypes;
  std::vector<std::string>::const_iterator histogramType;
  
  // CheckItem is a short-cut to mark Tree entries if they are histograms
  TGListTreeItem* node = listView->GetFirstItem();
//  listView->SetCheckBox(node, kTRUE);
//  listView->ToggleItem(node);
  TGListTreeItem* histoNode = 0;
  TGListTreeItem* pageNode = 0;
  
  TObjArray* folderItems = 0;
  TIterator* folderItemsIt = 0;
  TObject* folderItem = 0;
  
  try {
    if (m_connectedToHistogramDatabase && listView) {
      listView->RenameItem(node, TString(m_dbName));
//      // Make tree hierarchy from /folder1/folder2/... flat string      
//      
      localDatabaseFolders.clear();
      localDatabaseIDS.clear();
      histogramTypes.clear();      
      listView->DeleteChildren(node);
      
      switch (filterCriteria) {
        case byFolderAndPage:        
          m_histogramDB->getPageFolderNames(localDatabaseFolders, "_ALL_");
          break;
        case bySubsystem:
          m_histogramDB->getSubsystems(localDatabaseFolders);
          break;
        case byTask:
          m_histogramDB->getTasks(localDatabaseFolders);  
          break;          
        case byHistogramsWithAnalysis:
          m_histogramDB->getHistogramsWithAnalysis(NULL, &localDatabaseIDS,
                                                   &histogramTypes);  
          break;
        case byAnalysisHistograms:
          m_histogramDB->getAnalysisHistograms(NULL, &localDatabaseIDS,
                                               &histogramTypes);
          break;          
        case allHistos:
          m_histogramDB->getAllHistograms(NULL, &localDatabaseIDS,
                                          &histogramTypes);        
          break;
        default:
          break;          
      }
      if (filterCriteria == byFolderAndPage ||
          filterCriteria == bySubsystem ||
          filterCriteria == byTask) {
        for (folderIt = localDatabaseFolders.begin();
             folderIt != localDatabaseFolders.end(); ++folderIt) {                        
          folderItems = TString(*folderIt).Tokenize("/");
          folderItemsIt = folderItems->MakeIterator();
          node = listView->GetFirstItem(); 
          
          while (folderItem = folderItemsIt->Next()) {
            if (listView->FindChildByName(node, folderItem->GetName())) {
              node = listView->FindChildByName(node, folderItem->GetName());                
            } else {
              node = listView->AddItem(node, folderItem->GetName());              
//              listView->SetCheckBox(node, kTRUE);
//              listView->ToggleItem(node);                    
            }
            node->SetUserData(0);
            
            
//getPageNamesByFolder(std::string Folder,
//               std::vector<string>& list)            
            
            if (folderItem == folderItems->Last() &&
                filterCriteria == byFolderAndPage) {
              localDatabasePages.clear();
              m_histogramDB->getPageNamesByFolder(*folderIt,
                             localDatabasePages);
              for (pageIt = localDatabasePages.begin();
                   pageIt != localDatabasePages.end(); ++pageIt) {
                //std::string pageName = *folderIt + "/" + *pageIt;
                std::string pageName = std::string((*pageIt)).erase(0, (*folderIt).length()+1);
                pageNode = listView->AddItem(node, TString(pageName));                                                    
                pageNode->SetUserData(new TString(*pageIt));
                setTreeNodeIcon(pageNode, "Page");
                if (histograms) {
                  localDatabaseIDS.clear();
                  histogramTypes.clear();
                  m_histogramDB->getHistogramsByPage(*pageIt, NULL,
                                 &localDatabaseIDS, &histogramTypes);
                  histogramType = histogramTypes.begin();                              
                  for (histogramIt = localDatabaseIDS.begin();
                       histogramIt != localDatabaseIDS.end(); ++histogramIt) {
// TODO: H1D/OTMon/OTCommissioningMonitoring/TELL1Mult_$T3/L2/Q0/myTell
                        
                    histoNode = listView->AddItem(pageNode, TString(*histogramIt));
                    listView->SetCheckBox(histoNode, kTRUE);
                    listView->ToggleItem(histoNode);
                    setTreeNodeIcon(histoNode, *histogramType);
// TODO: new -> delete...                    
                    histoNode->SetUserData(new TString(*histogramIt));           
                    ++histogramType;    
                  }
                } else {
                  pageNode->CheckItem(true);
                }
              }
            } else if (folderItem == folderItems->Last() &&
                                     filterCriteria != byFolderAndPage) {
              if (histograms) {
                localDatabaseIDS.clear();
                histogramTypes.clear();
                if (filterCriteria == byTask) {
                  m_histogramDB->getHistogramsByTask(*folderIt, NULL,
                                 &localDatabaseIDS, &histogramTypes);
                } else if (filterCriteria == bySubsystem) {
                  m_histogramDB->getHistogramsBySubsystem(*folderIt, NULL,
                                 &localDatabaseIDS, &histogramTypes);
               }                                                      
                histogramType = histogramTypes.begin();         
                for (histogramIt = localDatabaseIDS.begin();
                     histogramIt != localDatabaseIDS.end(); ++histogramIt, ++histogramType) {
// TODO: H1D/OTMon/OTCommissioningMonitoring/TELL1Mult_$T3/L2/Q0/myTell
                  histoNode = listView->AddItem(node, TString(*histogramIt));
                  listView->SetCheckBox(histoNode, kTRUE);
                  listView->ToggleItem(histoNode);                  
                  setTreeNodeIcon(histoNode, *histogramType);
// TODO: new -> delete...                  
                  histoNode->SetUserData(new TString(*histogramIt));
                }
              }
            }
          }
        }
      } else if (filterCriteria == byHistogramsWithAnalysis ||
                 filterCriteria == byAnalysisHistograms || 
                 filterCriteria == allHistos) {
        histogramType = histogramTypes.begin();
        for (histogramIt = localDatabaseIDS.begin();
             histogramIt != localDatabaseIDS.end(); ++histogramIt) {
// TODO: H1D/OTMon/OTCommissioningMonitoring/TELL1Mult_$T3/L2/Q0/myTell
          histoNode = listView->AddItem(node, TString(*histogramIt));
          setTreeNodeIcon(histoNode, *histogramType);
          listView->SetCheckBox(histoNode, kTRUE);
          listView->ToggleItem(histoNode);
// TODO: new -> delete...                       
          histoNode->SetUserData(new TString(*histogramIt));
          ++histogramType;
        }
      }
      fClient->NeedRedraw(listView);
      m_message = "Histograms read from Database.";
    } else if (!m_connectedToHistogramDatabase) {
      m_message = "Histograms not read from Database.";
    }
    m_statusBar->SetText(m_message.c_str());
    if (m_verbosity >= VERBOSE) { std::cout << m_message << std::endl; }
    
  } catch (SQLException sqlException) {
    // TODO: add error logging backend
    m_message = sqlException.getMessage();
    m_statusBar->SetText(m_message.c_str());  
    if (m_verbosity >= VERBOSE) { std::cout << m_message << std::endl; }
    
    new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                 Form("OnlineHistDB server:\n\n%s\n",               
                 m_message.c_str()), kMBIconExclamation,
                 kMBOk, &m_msgBoxReturnCode);
  }  
}

std::string PresenterMainFrame::getDBName()
{
  return m_dbName;
}

std::string PresenterMainFrame::convDimToHistoID(std::string dimSvcName)
{
  // Convert DIM servicename "HnD/UTGID/Algorithmname/Histogramname" where
  // UTGID is like "node00101_taskname_1" to an OnlineHistDB histogram id
  // such as "Taskname/Algorithmname/Histogramname"
  TPRegexp dimSvcNameWithUTGIDRegexp("^.{3}/[^/]*_([^/]+)_[^/]*(/.+$)");
  TPRegexp dimSvcNameRegexp("^.{3}/([^/]+/.+$)");
  TString dimSvcNameTS(dimSvcName);
  if (dimSvcNameWithUTGIDRegexp.MatchB(dimSvcName)) {
    dimSvcNameWithUTGIDRegexp.Substitute(dimSvcNameTS,"$1$2");
    return std::string(dimSvcNameTS);    
  } else if (dimSvcNameRegexp.MatchB(dimSvcName)) {
    dimSvcNameRegexp.Substitute(dimSvcNameTS,"$1");
    return std::string(dimSvcNameTS);
  } else {
    new TGMsgBox(fClient->GetRoot(), this, "DIM Service name error",
     Form("The DIM servicename\n%s\n does not appear to follow the convention\nPlease use the following format:\nHnD/UTGID/Algorithmname/Histogramname\n where the UTGID normally has the following format:\n node00101_taskname_1 or simply taskname, without \"_\"",dimSvcName.c_str()),
     kMBIconExclamation,kMBOk,&m_msgBoxReturnCode);
    return 0;
  }
}

// TODO: move to an eventual static helper class
void PresenterMainFrame::setTreeNodeIcon(TGListTreeItem* node, std::string type)
{
  const TGPicture*  m_icon;
  if (OnlineHistDBEnv_constants::HistTypeName[0] == type) {
    m_icon = m_iconH1D;
  } else if (OnlineHistDBEnv_constants::HistTypeName[1] == type) {
    m_icon = m_iconH2D;
  } else if (OnlineHistDBEnv_constants::HistTypeName[2] == type ||
             OnlineHistDBEnv_constants::HistTypeName[3] == type ) {
    m_icon = m_iconProfile;
  } else if (OnlineHistDBEnv_constants::HistTypeName[4] == type) {
    m_icon = m_iconCounter;
  } else if ("Page" == type) {
    m_icon = m_iconPage;                                                
  } else {
    m_icon = m_iconQuestion;
  }
  if (node) { node->SetPictures(m_icon, m_icon); }
}


// TODO: move to an eventual static helper class
// Adds all selected list tree items into the list selected.
void PresenterMainFrame::checkedTreeItems(TGListTree* selected, TGListTree* treeList)
{
  if (!selected) return;
  TGListTreeItem *current;
  current = treeList->GetFirstItem();
  if (current->IsChecked() && current->GetUserData()) {
        selected->AddItem(0,0,current->GetUserData());
  }
  while(current) {
     if (current->GetFirstChild())
        checkedTreeItemsChildren(current->GetFirstChild(), selected);
     current = current->GetNextSibling();
  }
}

// TODO: move to an eventual static helper class
// Adds child items into the list selected.
void PresenterMainFrame::checkedTreeItemsChildren(TGListTreeItem *item, TGListTree* selected)
{
  while (item) {
    if (item->IsChecked() && item->GetUserData()) {
      selected->AddItem(0,0,item->GetUserData());
    }
    if (item->GetFirstChild()) {
      checkedTreeItemsChildren(item->GetFirstChild(),selected);
    }
    item = item->GetNextSibling();
  }
}

// TODO: move to an eventual static helper class
void PresenterMainFrame::setStatusBarText(const char* text, int slice)
{
  m_statusBar->SetText(text, slice);
}

// TODO: move to an eventual static helper class
TH1* PresenterMainFrame::getSelectedHisto()
{
//  TPad* pad = dynamic_cast<TPad*>(editorFrame->editorCanvas->GetClickSelectedPad());
//  TVirtualPad* pad = gPad;
  if (gPad) {
    TIter nextPadElem(gPad->GetListOfPrimitives());
    TObject *histoCandidate;
    while (histoCandidate = nextPadElem()) {
      if (histoCandidate->InheritsFrom(TH1::Class())) {
        return dynamic_cast<TH1*>(histoCandidate);
      }
    } return 0;
  } else {
    return 0;
  }
}

void PresenterMainFrame::editHistogramProperties()
{
  // TODO: stopped needs to be killed off...
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
    zoomCanvas->SetBit(kCanDelete);
    zoomCanvas->cd();
    zoomCanvas->SetTitle(selectedHisto->GetTitle());
    selectedHisto->DrawCopy();
  }
  
  if (stopped) {
    startPageRefresh();
  } 
}

void PresenterMainFrame::fitPanel()
{
  // TODO: stopped needs to be killed off...
  bool stopped = false;
  
  if (m_refreshing) {
    stopPageRefresh();
    stopped = true;
  }
  
  if (TH1* selectedHisto = getSelectedHisto()) {
    //TODO: WaitFor
    selectedHisto->FitPanel();
  }

  if (stopped) {
//  startPageRefresh();
  } 
}
