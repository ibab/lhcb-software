// $Id: $
#ifndef REFERENCEDIALOG_H 
#define REFERENCEDIALOG_H 1

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TGFrame.h>
#ifdef WIN32
#pragma warning( pop )
#endif

#include "presenter.h"

class PresenterMainFrame;
class TGComboBox;
class TGTextButton;
class TGTextBuffer;
class TGTextEntry;
class TGTextEdit;
class TGedPatternSelect;
class TGLineWidthComboBox;
class TGColorSelect;
class TGLineStyleComboBox;
class TGListBox ;
class TGComboBox ;

class ReferenceDialog : public TGTransientFrame
{
public:
  ReferenceDialog(PresenterMainFrame* gui, int width, int height);
  virtual ~ReferenceDialog();
  
  void setParameters( std::string& tck, int& rRun );
  
  void ok();
  void build();
  void CloseWindow();

private:
  PresenterMainFrame *m_mainFrame;
  TGTextButton       *m_okButton;
  TGTextButton       *m_cancelButton;

  std::string* m_tck;
  int*         m_refRun;

  TGTextEntry        *m_tckTextEntry;
  TGTextEntry        *m_refRunTextEntry;

  ClassDef(ReferenceDialog, 0);
};
#endif // REFERENCEDIALOG_H
