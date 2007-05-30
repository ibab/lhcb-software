#ifndef PRESENTERMAINFRAME_H_
#define PRESENTERMAINFRAME_H_

#include <TGFrame.h>

#include "../inc/LoginDialog.h"
#include "../inc/presenter.h"

//class TRootEmbeddedCanvas;
//class TGTab;

class TGPicture;
class TGFrame;
class TGToolBar;
class ToolBarData_t;
class TGMenuBar;
class TGPopupMenu;
class TGButton;
class TString;
class OnlineHistDB;

using namespace pres;

class PresenterMainFrame : public TGMainFrame
{
  public:
  
    PresenterMainFrame(const char *name, UInt_t x = 10, UInt_t y = 10,
                       UInt_t w = 600, UInt_t h = 400, MsgLevel v = SILENT);
    virtual ~PresenterMainFrame();
    
    // Add button, menu etc. commands to this list (do not define elsewhere:
    // typesafety... X: unkown, M: menu, B: button etc...) 
    enum Command
    {
      X_ENIGMA_COMMAND,
      M_FILE_EXIT_COMMAND,
//      M_FILE_CLOSE_COMMAND,
      M_FILE_LOGOUT_COMMAND,
      M_FILE_LOGIN_COMMAND,
      CLEAR_PAGE,
      CLEAR_HISTOS,
      B_START_COMMAND,
      B_STOP_COMMAND
    };
    

    //slots
    void build();
    void closeMainWindow();
    void handleCommand(Command id = X_ENIGMA_COMMAND);
//    virtual bool ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
    void doSomething();
    void clearPage();
    void loginOnlineHistDB();
    void connectToDatabase(std::string, std::string, std::string);
    void logoutOnlineHistDB();
    void startPageRefresh();
    void stopPageRefresh();
    void clearHistos();
    void refreshPage();
    
  private:
    
    MsgLevel            m_verbosity;
    TGCompositeFrame    *ef;
    TTimer              *m_refreshTimer;
    bool                m_histosCleared;
    bool                m_connectedToHistogramDatabase;
    OnlineHistDB        *m_histogramDB;
    
    // icons
    const TGPicture     *m_icon1;
    
    // File menu
    TGFrame           *m_runMode;
      TGToolBar       *m_toolBar;
      TGMenuBar       *m_menuBar;
        TGPopupMenu   *m_fileMenu;
          TGHotString *m_fileText;
          TGHotString *m_fileNew;
          TGHotString *m_fileLoginText;
          TGHotString *m_fileLogoutText;
          TGHotString *m_fileQuitText;

    // Toolbar
    ToolBarData_t     *m_toolbarElement;
    TGButton          *m_loginButton;
    TGButton          *m_logoutButton;
    TGButton          *m_newButton;
    TGButton          *m_startButton;
    TGButton          *m_stopButton;
    TGButton          *m_clearButton;
    //TGButton          *m_quitButton;
   
    ClassDef(PresenterMainFrame, 0) // main editor window        
};

R__EXTERN PresenterMainFrame *gPresenter;

#endif /*PRESENTERMAINFRAME_H_*/
