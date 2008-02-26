#ifndef REFERENCEPICKER_H_
#define REFERENCEPICKER_H_

#include <TGFrame.h>
#include "presenter.h"

class PresenterMainFrame;
class DbRootHist;
class Archive;
class TGTextButton;

class ReferencePicker : public TGTransientFrame
{
public:
	ReferencePicker(PresenterMainFrame* gui);
	virtual ~ReferencePicker();

  void ok();
  void build();
  void CloseWindow();

private:
  PresenterMainFrame* m_mainFrame;
  DbRootHist*   m_histogram;
  Archive*  m_archive;
  pres::MsgLevel  m_verbosity;
  TGTextButton* m_okButton;
  TGTextButton* m_cancelButton;
  int m_msgBoxReturnCode;
  
  ClassDef(ReferencePicker, 0)  
  
};

#endif /*REFERENCEPICKER_H_*/
