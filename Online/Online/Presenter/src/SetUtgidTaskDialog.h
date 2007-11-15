#ifndef SETUTGIDTASKDIALOG_H_
#define SETUTGIDTASKDIALOG_H_

#include <TGFrame.h>
#include "presenter.h"

using namespace pres;

class PresenterMainFrame;

class SetUtgidTaskDialog : public TGTransientFrame
{
public:
	SetUtgidTaskDialog();
	virtual ~SetUtgidTaskDialog();
  
  void build();
  void ok();
  
  private:
    PresenterMainFrame *m_mainFrame;
    TGTextButton       *m_okButton;
    TGTextButton       *m_cancelButton;
    MsgLevel            m_verbosity;
    int                 m_msgBoxReturnCode;  
  
  ClassDef(SetUtgidTaskDialog, 0)
};

#endif /*SETUTGIDTASKDIALOG_H_*/
