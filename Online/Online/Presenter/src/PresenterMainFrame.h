#ifndef PRESENTERMAINFRAME_H_
#define PRESENTERMAINFRAME_H_

#include <TGFrame.h>
#include <TGListTree.h>
#include <TColor.h>
#include <vector>

#include "LoginDialog.h"
#include "presenter.h"

class TRootEmbeddedCanvas;

class TGButton;
class TGFrame;
class TGMenuBar;
class TGPicture;
class TGPopupMenu;
class TGToolBar;
class TGStatusBar;
class ToolBarData_t;
class TString;
class TCanvas;
class TRootEmbeddedCanvas;
class TH1;
class TGListTreeItem;
class TList;

class DimBrowser;
class OnlineHistDB;
class OMAlib;
class EditorFrame;

using namespace pres;

class PresenterMainFrame : public TGMainFrame
{
  public:

    PresenterMainFrame(const char* name, UInt_t x, UInt_t y, UInt_t w,
      UInt_t h, MsgLevel v);
    //  PresenterMainFrame() {};
    virtual ~PresenterMainFrame();

    // Add button, menu etc. commands to this list (do not define elsewhere:
    // typesafety...)
    enum Command
    {
      X_ENIGMA_COMMAND,
      EXIT_COMMAND,
//    M_FILE_CLOSE_COMMAND,
      LOGOUT_COMMAND,
      LOGIN_COMMAND,
      CLEAR_PAGE_COMMAND,
      SAVE_PAGE_COMMAND,
      DELETE_HISTO_FROM_CANVAS_COMMAND,
      CLEAR_HISTOS_COMMAND,
      EDIT_HISTO_COMMAND,
      FIT_PANEL_COMMAND,
      ZOOM_HISTO_COMMAND,
      START_COMMAND,
      STOP_COMMAND,
      AUTO_LAYOUT_COMMAND
    };

    struct BulkHistoOptions {
      TString m_genericRootDrawOption;
      TString m_1DRootDrawOption;
      TString m_2DRootDrawOption;
      TString m_xLabel;
      TString m_yLabel;
      int m_fillColour;
      int m_lineColour;
      int m_statsOption;
      int m_fillStyle;
      int m_lineWidth;
      int m_lineStyle;

    };

    //slots
    void buildGUI();
    void CloseWindow();
    void handleCommand(Command id = X_ENIGMA_COMMAND);
    void doSomething();
    void clearPage();
    void savePage();
    void autoCanvasLayout();
    void deleteHistoFromCanvas();
    void loginOnlineHistDB();
    bool connectToDatabase(std::string, std::string, std::string);
    OMAlib* getAnalib();
    bool isConnectedToDatabase();
    OnlineHistDB* getDatabase();
    void logoutOnlineHistDB();
    void startPageRefresh();
    void stopPageRefresh();
    void clearHistos();
    void refreshPage();
    void readFromHistoDatabase(TGListTree* listView, FilterCriteria
      filterCriteria, bool histograms);
    void fillTreeNodeWithHistograms(TGListTree* listView,
      TGListTreeItem* node, std::vector<std::string>*  localDatabaseIDS,
      std::vector<std::string>* histogramTypes);
    std::string getDBName();

    void setTreeNodeIcon(TGListTreeItem* node, std::string type);
    void setStatusBarText(const char* text, int slice);
    void editHistogramProperties();
    void zoomHistogram();
    void fitPanel();
    TH1* getSelectedHisto();

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
    EditorFrame*         editorFrame;

    BulkHistoOptions   m_bulkHistoOptions;

  private:

    MsgLevel            m_verbosity;
    DimBrowser*         m_dimBrowser;
    TGCompositeFrame*   m_ef;
    TTimer*             m_refreshTimer;
    bool                m_refreshing;
    bool                m_clearedHistos;
    OnlineHistDB*       m_histogramDB;
    OMAlib*             m_analib;
    bool                m_connectedToHistogramDatabase;
    int                 m_msgBoxReturnCode;
    std::string         m_dbName;
    std::string         m_message;

    int m_mainCanvasWidth;
    int m_mainCanvasHeight;

    TGStatusBar*        m_statusBar;

    // icons
    const TGPicture*    m_icon1;

    // File menu
    TGFrame*            m_runMode;
    TGToolBar*        m_toolBar;
    TGMenuBar*        m_menuBar;
    TGPopupMenu*    m_fileMenu;
    TGHotString*  m_fileText;
    TGHotString*  m_fileNew;
    TGHotString*  m_fileLoginText;
    TGHotString*  m_fileLogoutText;
    TGHotString*  m_fileQuitText;

    // Toolbar
    ToolBarData_t*      m_toolbarElement;
    TGButton*           m_loginButton;
    TGButton*           m_logoutButton;
    TGButton*           m_newPageButton;
    TGButton*           m_startRefreshButton;
    TGButton*           m_stopRefreshButton;
    TGButton*           m_clearHistoButton;
    TGButton*           m_zoomHistoButton;
    TGButton*           m_editHistoButton;
    TGButton*           m_autoCanvasLayoutButton;
    TGButton*           m_deleteHistoFromCanvasButton;

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

//    TObjArray* m_histogramSetItems;
//    TIterator* m_histogramSetItemsIt;
//    TObject*   m_histogramSetItem;
//
    TObjArray* m_histogramIdItems;
    TIterator* m_histogramIdItemsIt;
    TObject*   m_histogramIdItem;

    ClassDef(PresenterMainFrame, 0) // main editor window
};

R__EXTERN PresenterMainFrame* gPresenter;

#endif /*PRESENTERMAINFRAME_H_*/
