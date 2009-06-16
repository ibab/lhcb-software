#ifndef LOGINDIALOG_H_
#define LOGINDIALOG_H_

#include <TGFrame.h>
#include "presenter.h"

class PresenterMainFrame;
class TGComboBox;
class TGTextButton;
class OnlineHistDB;
class TGTextBuffer;
class TGTextEntry;

class LoginDialog : public TGTransientFrame
{
  public:
    LoginDialog(PresenterMainFrame* gui, int width, int height,
               const pres::DatabaseMode databaseMode,
               const std::vector<std::string*> & knownDatabases,
               const std::map<std::string*, std::string*> & knownDbCredentials);
    virtual ~LoginDialog();

    void build();
    void login();
    void CloseWindow();
    void defaultPw(int account);
    //void connectToDatabase(int intp); //*SIGNAL*

  private:
    PresenterMainFrame* m_mainFrame;
    pres::DatabaseMode m_databaseMode;
    TGTextEntry*  m_dbPasswdInput;
    TGTextBuffer* m_dbPasswd;
    TGComboBox*   m_histogramDbSelector;
    TGComboBox*   m_dbUsername;
    char*         m_input;
    TGTextButton* m_loginButton;
    TGTextButton* m_cancelButton;
    std::vector<std::string*> m_dbs;
    std::map<std::string*, std::string*> m_creds;

  ClassDef(LoginDialog, 0)
};

#endif /*LOGINDIALOG_H_*/
