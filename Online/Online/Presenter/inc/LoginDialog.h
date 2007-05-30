#ifndef LOGINDIALOG_H_
#define LOGINDIALOG_H_

#include <TGFrame.h>
#include "../inc/presenter.h"

using namespace pres;

class PresenterMainFrame;
class TGComboBox;
class TGTextButton;

class LoginDialog : public TGTransientFrame
{
public:
  LoginDialog(PresenterMainFrame *gui, Int_t Width, Int_t Height);
  
  virtual ~LoginDialog();
  
  void build();
  void ok();
  void cancel();
  //void OnElementSelected(TObject *obj); //*SIGNAL*

  void CloseWindow();
  
private:
  TGComboBox  *m_db;
  TGComboBox  *m_user;
  char        *m_inputUser;
  char        *m_inputDB;
  TGTextButton *m_Ok;  
  
  ClassDef(LoginDialog, 0)
};

#endif /*LOGINDIALOG_H_*/
