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
class EditorFrame;

using namespace pres;

class PresenterMainFrame : public TGMainFrame
{
  public:
  
    PresenterMainFrame(const char* name, UInt_t x = 10, UInt_t y = 10,
                       UInt_t w = 800, UInt_t h = 600, MsgLevel v = SILENT);
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
    void loginOnlineHistDB();
    bool connectToDatabase(std::string, std::string, std::string);
    bool isConnectedToDatabase();
    OnlineHistDB* getDatabase();
    void logoutOnlineHistDB();
    void startPageRefresh();
    void stopPageRefresh();
    void clearHistos();
    void refreshPage();
    void readFromHistoDatabase(TGListTree* listView, FilterCriteria filterCriteria, bool histograms);
    std::string getDBName();
    std::string convDimToHistoID(std::string dimSvcName);    
    void setTreeNodeIcon(TGListTreeItem* node, std::string type);
    void setStatusBarText(const char* text, int slice);
    void editHistogramProperties();
    void zoomHistogram();
    void fitPanel();
    TH1* getSelectedHisto();
    void checkedTreeItems(TGListTree* selected, TGListTree* treeList);
    void checkedTreeItemsChildren(TGListTreeItem *item, TGListTree* selected);
   
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
    bool                m_connectedToHistogramDatabase;
    OnlineHistDB*       m_histogramDB;
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
    TGButton*           m_fitPanelButton;
    TGButton*           m_autoCanvasLayoutButton;

    
//  TGButton*           m_quitButton;
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
   
    std::vector<TColor*>          m_RootColourVector;         // vector of colours defined for own colour palette -
   
    ClassDef(PresenterMainFrame, 0) // main editor window        
};

R__EXTERN PresenterMainFrame* gPresenter;

#endif /*PRESENTERMAINFRAME_H_*/
