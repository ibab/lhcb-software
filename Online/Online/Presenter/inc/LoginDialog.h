#ifndef LOGINDIALOG_H_
#define LOGINDIALOG_H_

#include <TGFrame.h>
#include "../inc/presenter.h"

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
    LoginDialog(PresenterMainFrame *gui, Int_t Width, Int_t Height);    
    virtual ~LoginDialog();
    
    void build();
    void login();
    void defaultPw(const char *);
    //void connectToDatabase(int intp); //*SIGNAL*
    
  private:
    TGTextEntry *m_dbPasswdInput;
    TGTextBuffer *m_dbPasswd;
    TGComboBox   *m_histogramDB;
    TGComboBox   *m_dbUsername;
    char         *m_input;
    TGTextButton *m_loginButton;
    TGTextButton *m_cancelButton;
    PresenterMainFrame  *m_parent;
    
  ClassDef(LoginDialog, 0)
};

#endif /*LOGINDIALOG_H_*/
