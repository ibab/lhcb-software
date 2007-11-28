#ifndef PRESENTERMAINFRAME_H_
#define PRESENTERMAINFRAME_H_

#include <TGFrame.h>
#include <TGListTree.h>
#include <TColor.h>
#include <vector>

#include "LoginDialog.h"
#include "presenter.h"

class TObject;
class TGDockableFrame;
class TString;
class TGButton;
class TGFrame;
class TGPopupMenu;
class TGMenuBar;
class TGToolBar;
struct ToolBarData_t;
class TGStatusBar;
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

class DbRootHist;
class TH1;

class DimBrowser;
class OnlineHistDB;
class OnlineHistogram;
class OMAlib;

using namespace pres;

class PresenterMainFrame : public TGMainFrame
{
  public:

    PresenterMainFrame(const char* name, UInt_t x, UInt_t y, UInt_t width,
                       UInt_t height, MsgLevel verbosity);
    //  PresenterMainFrame() {};
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
      SAVE_PAGE_TO_FILE_COMMAND,
      SAVE_PAGE_TO_DB_COMMAND,
      REMOVE_HISTO_FROM_CANVAS_COMMAND,
      CLEAR_HISTOS_COMMAND,
      EDIT_HISTO_COMMAND,
      EDIT_PAGE_PROPERTIES_COMMAND,
      INSPECT_HISTO_COMMAND,
      INSPECT_PAGE_COMMAND,
      START_COMMAND,
      STOP_COMMAND,
      SET_UTGID_TASK_COMMAND,
      AUTO_LAYOUT_COMMAND,
      OVERLAY_REFERENCE_HISTO_COMMAND,
      SET_REFERENCE_HISTO_COMMAND,
      HELP_CONTENTS_COMMAND,
      HELP_ABOUT_COMMAND,
      UNDOCK_PAGE_COMMAND,
      DOCK_ALL_COMMAND,
      ONLINE_MODE_COMMAND,
      OFFLINE_MODE_COMMAND,
      PAGE_EDITOR_MODE_COMMAND,
      M_AddDimToDB_COMMAND,
      M_AddDimToPage_COMMAND,
      M_AddDBHistoToPage_COMMAND,
      M_DimCheckAllChildren_COMMAND,
      M_DimUnCheckAllChildren_COMMAND,
      M_DimCollapseAllChildren_COMMAND,
      M_DeleteDBHisto_COMMAND,
      M_DBHistoCheckAllChildren_COMMAND,
      M_DBHistoUnCheckAllChildren_COMMAND,
      M_DBHistoCollapseAllChildren_COMMAND,
      M_SetHistoPropertiesInDB_COMMAND,
      M_RefreshHistoDBListTree_COMMAND,
      M_RefreshHistoDIMListTree_COMMAND,
      M_RefreshDBPagesListTree_COMMAND,
      M_LoadPage_COMMAND,
      M_DeletePage_COMMAND,
      M_DeleteFolder_COMMAND,
    };

    struct BulkHistoOptions {
      TString m_genericRootDrawOption;
      TString m_1DRootDrawOption;
      TString m_2DRootDrawOption;
      TString m_xLabel;
      TString m_yLabel;
      int m_fillColour;
      int m_lineColour;
      bool m_statsOption;
      int m_fillStyle;
      int m_lineWidth;
      int m_lineStyle;
    };

    //slots
    void buildGUI();
    void CloseWindow();
    void dockAllFrames();
    void handleCommand(Command id = X_ENIGMA_COMMAND);
    void savePageToHistogramDB();
    void savePageToFile();
    void printPage();
    void autoCanvasLayout();
    void loginToHistogramDB();
    bool connectToHistogramDB(std::string dbPassword, std::string dbUsername,
                              std::string dbName);
    OMAlib* analysisLib();
    bool isConnectedToHistogramDB();
    OnlineHistDB* histogramDB();
    void logoutFromHistogramDB();
    void startPageRefresh();
    void stopPageRefresh();
    void clearHistos();
    void refreshPage();
    void listFromHistogramDB(TGListTree* listView,
                             FilterCriteria filterCriteria,
                             bool histograms);
    void fillTreeNodeWithHistograms(TGListTree* listView,
      TGListTreeItem* node, std::vector<std::string>*  localDatabaseIDS,
      std::vector<std::string>* histogramTypes);
    void changeDimDnsNode();
    std::string histogramDBName();

    void setTreeNodeIcon(TGListTreeItem* node, std::string type);
    void setStatusBarText(const char* text, int slice);
    void editHistogramProperties();
    void inspectHistogram();
    void inspectPage();
    TH1* selectedHistogram();

    void checkedTreeItems(TGListTree* selected, TGListTree* treeList);
    void checkedTreeItemsChildren(TGListTreeItem* node,
                                  TGListTree* selected);

    void checkTreeChildrenItems(TGListTreeItem* node, bool check);
    void checkTreeChildrenItemsChildren(TGListTreeItem* node, bool check);

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

    BulkHistoOptions   m_bulkHistoOptions;

    void refreshHistoDBListTree();
    void refreshPagesDBListTree();
    void refreshDimSvcListTree();
    void hideDBTools();
    void showDBTools();

    void enableHistogramClearing();
    void disableHistogramClearing();

    void enablePageRefresh();
    void disablePageRefresh();
    
    void enablePageUndocking();
    void disablePageUndocking();

    void enablePageLoading();

    void deleteSelectedHistoFromCanvas();

    void clickedDimTreeItem(TGListTreeItem* item, MouseButton btn,
                            Int_t x, Int_t y);
    void clickedHistoDBTreeItem(TGListTreeItem* item, MouseButton btn,
                                Int_t x, Int_t y);
    void clickedPageTreeItem(TGListTreeItem* item, MouseButton btn,
                             Int_t x, Int_t y);

    TGPopupMenu* getDimContextMenu() const { return m_dimContextMenu; }

    void addDimSvcToHistoDB();
    void addDimSvcToPage();
    void addDbHistoToPage();
    void dimCheckAllChildren();
    void dimUnCheckAllChildren();
    void dimCollapseAllChildren();

    void setHistogramPropertiesInDB();
    void dbHistoCheckAllChildren();
    void dbHistoUnCheckAllChildren();
    void dbHistoCollapseAllChildren();

    void deleteSelectedHistoFromDB();
    void loadSelectedPageFromDB();
    void deleteSelectedPageFromDB();
    void deleteSelectedFolderFromDB();
    void setHistogramParametersFromDB(TH1* histogram,
                                      OnlineHistogram* onlineHistogram);
    void paintHist(DbRootHist* histogram);

    void removeHistogramsFromPage();
    void EventInfo(int event, int px, int py, TObject *selected);

    TCanvas* editorCanvas;
    TRootEmbeddedCanvas* editorEmbCanvas;

    std::vector<DbRootHist*>      m_DbHistosOnPage;
    std::vector<DbRootHist*>::iterator m_DbHistosOnPageIt;

  private:

    MsgLevel            m_verbosity;
    DimBrowser*         m_dimBrowser;
    TGCompositeFrame*   m_ef;
    TTimer*             m_refreshTimer;
    bool                m_refreshing;
    bool                m_clearedHistos;
    OnlineHistDB*       m_histogramDB;
    OMAlib*             m_analysisLib;
    bool                m_connectedToHistogramDB;
    int                 m_msgBoxReturnCode;
    std::string         m_dbName;
    std::string         m_message;

    int m_mainCanvasWidth;
    int m_mainCanvasHeight;
    
    TGDockableFrame* m_menuDock;
    TGDockableFrame* m_toolBarDock;
    TGDockableFrame* m_databaseHistogramsDock;
    TGDockableFrame* m_pageDock;
    TGDockableFrame* m_mainCanvasInfoDock;
    TGDockableFrame* m_dimBrowserDock;
    TGDockableFrame* m_databasePagesDock;

    TGStatusBar*        m_mainStatusBar;
    TGStatusBar*        m_statusBarTop;

    // icons
    const TGPicture*    m_icon1;

    // File menu
    TGToolBar*    m_toolBar;
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
      TGHotString*  m_editSetReferenceHistogramText;
      TGHotString*  m_editAutoLayoutText;
      TGHotString*  m_editPagePropertiesText;
      TGHotString*  m_editRemoveHistoText;

    TGPopupMenu*  m_viewMenu;
    TGHotString*  m_viewText;
      TGHotString*  m_viewStartRefreshText;
      TGHotString*  m_viewStopRefreshText;
      TGHotString*  m_viewOverlayReferenceHistogramText;
      TGHotString*  m_viewInspectHistoText;
      TGHotString*  m_viewInspectPageText;
      TGHotString*  m_viewClearHistosText;
      TGHotString*  m_viewUnDockPageText;
      TGHotString*  m_viewDockAllText;
      
    TGPopupMenu*  m_toolMenu;
    TGHotString*  m_toolText;
      TGHotString*  m_toolMode;
      TGHotString*  m_toolPageEditor;
      TGHotString*  m_toolOnline;
      TGHotString*  m_toolOffline;
      TGHotString*  m_toolSetUtgidTaskText;      
      TGHotString*  m_toolSetDimDnsText;
      
    TGPopupMenu*  m_helpMenu;
    TGHotString*  m_helpText;
      TGHotString*  m_helpContentsText;
      TGHotString*  m_helpAboutText;

    // Toolbar
    ToolBarData_t*      m_toolbarElement;
    TGButton*           m_loginButton;
    TGButton*           m_logoutButton;
    TGButton*           m_savePageToDatabaseButton;
    TGButton*           m_newPageButton;
    TGButton*           m_startRefreshButton;
    TGButton*           m_stopRefreshButton;
    TGButton*           m_clearHistoButton;
    TGButton*           m_inspectHistoButton;
    TGButton*           m_editHistoButton;
    TGButton*           m_autoCanvasLayoutButton;
    TGButton*           m_deleteHistoFromCanvasButton;
    TGButton*           m_overlayReferenceHistoButton;
    TGButton*           m_setReferenceHistoButton;
    

    //  TGButton*         m_quitButton;
    //  TGPicturePool*    m_picturePool;
    const TGPicture*  m_openedFolderIcon;
    const TGPicture*  m_closedFolderIcon;

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
    
    std::string convDimToHistoID(std::string dimSvcName);

    TGCompositeFrame*    m_editorCanvasMainFrame;

    int m_histoPadOffset;

    TString m_drawOption; // bulk DO
    TString m_1ddrawOption; // bulk DO
    TString m_2ddrawOption; // bulk DO

    TGListTree*          m_databaseHistogramTreeList;
    TGListTree*          m_pagesFromHistoDBListTree;
    TGListTree*          m_dimSvcListTree;
    TGPopupMenu*         m_dimContextMenu;
    TGPopupMenu*         m_histoDBContextMenu;
    TGPopupMenu*         m_pagesContextMenu;
    TGViewPort*          m_histoDBCanvasViewPort;
    TGViewPort*          m_pagesFromHistoDBViewPort;
    TGCanvas*            m_dimSvcListTreeContainterCanvas;
    TGCanvas*            m_histoDBCanvas;
    TGVerticalFrame*     m_histoDBFilterFrame;
    TGComboBox*          m_histoDBFilterComboBox;

    std::vector<std::string>      m_knownDimServices;
    std::vector<std::string>::const_iterator m_knownDimServicesIt;

    std::vector<OnlineHistogram*>      m_onlineHistosOnPage;
    std::vector<OnlineHistogram*>::const_iterator m_onlineHistosOnPageIt;


    ClassDef(PresenterMainFrame, 0) // main editor window
};

R__EXTERN PresenterMainFrame* gPresenter;

#endif /*PRESENTERMAINFRAME_H_*/
