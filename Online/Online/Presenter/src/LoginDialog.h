#ifndef LOGINDIALOG_H_
#define LOGINDIALOG_H_

#include <TGFrame.h>
#include "presenter.h"

using namespace pres;

class PresenterMainFrame;
class TGComboBox;
class TGTextButton;
class OnlineHistDB;
class TGTextBuffer;
class TGTextEntry;

class LoginDialog : public TGTransientFrame
{
  public:
    LoginDialog(PresenterMainFrame* gui, Int_t Width, Int_t Height);
    virtual ~LoginDialog();

    void build();
    void login();
    void cancelLogin();
    void defaultPw(const char *);
    //void connectToDatabase(int intp); //*SIGNAL*

  private:
    PresenterMainFrame* m_mainFrame;
    TGTextEntry*  m_dbPasswdInput;
    TGTextBuffer* m_dbPasswd;
    TGComboBox*   m_histogramDB;
    TGComboBox*   m_dbUsername;
    char*         m_input;
    TGTextButton* m_loginButton;
    TGTextButton* m_cancelButton;

  ClassDef(LoginDialog, 0)
};

#endif /*LOGINDIALOG_H_*/
