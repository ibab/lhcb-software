#ifndef SETDIMDNSNODEDIALOG_H_
#define SETDIMDNSNODEDIALOG_H_

#include <TGFrame.h>
#include "presenter.h"

class PresenterMainFrame;
class TGComboBox;
class TGTextButton;
class TGTextBuffer;
class TGTextEntry;

class SetDimDnsNodeDialog : public TGTransientFrame
{
public:
	SetDimDnsNodeDialog(PresenterMainFrame* gui, int width, int height,
                      pres::MsgLevel v);
	virtual ~SetDimDnsNodeDialog();

  void build();
  void ok();
  void CloseWindow();

private:
  PresenterMainFrame* m_mainFrame;
  TGTextButton*       m_okButton;
  TGTextButton*       m_cancelButton;
  pres::MsgLevel      m_verbosity;
  int                 m_msgBoxReturnCode;

ClassDef(SetDimDnsNodeDialog, 0)
};

#endif /*SETDIMDNSNODEDIALOG_H_*/
