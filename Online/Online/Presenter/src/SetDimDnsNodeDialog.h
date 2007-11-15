#ifndef SETDIMDNSNODEDIALOG_H_
#define SETDIMDNSNODEDIALOG_H_

#include <TGFrame.h>
#include "presenter.h"

using namespace pres;

class PresenterMainFrame;
class TGComboBox;
class TGTextButton;
class TGTextBuffer;
class TGTextEntry;

class SetDimDnsNodeDialog : public TGTransientFrame
{
public:
	SetDimDnsNodeDialog(PresenterMainFrame* gui, Int_t Width, Int_t Height,
      MsgLevel v);
	virtual ~SetDimDnsNodeDialog();
  
  void build();
  void ok();

  private:
    PresenterMainFrame* m_mainFrame;
    TGTextButton*       m_okButton;
    TGTextButton*       m_cancelButton;
    MsgLevel            m_verbosity;
    int                 m_msgBoxReturnCode;  
  
  ClassDef(SetDimDnsNodeDialog, 0)
};

#endif /*SETDIMDNSNODEDIALOG_H_*/
