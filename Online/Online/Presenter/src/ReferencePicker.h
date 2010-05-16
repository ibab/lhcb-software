// $Id: ReferencePicker.h,v 1.4 2010-05-16 18:10:09 robbep Exp $

#ifndef REFERENCEPICKER_H_
#define REFERENCEPICKER_H_

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
class DbRootHist;
class Archive;
class TGTextButton;

class TCanvas;
class TRootEmbeddedCanvas;
class TGComboBox;

class ReferencePicker : public TGTransientFrame
{
public:
	ReferencePicker(PresenterMainFrame* gui, DbRootHist* histogram);
	virtual ~ReferencePicker();

  void setSelectedAsRef();
  void build();
  void CloseWindow();

private:
  PresenterMainFrame* m_mainFrame;
  DbRootHist*   m_histogram;
  Archive*  m_archive;
  pres::MsgLevel  m_verbosity;
  TGTextButton *m_setSelectedAsRefButton;
  TGTextButton* m_cancelButton;
  int m_msgBoxReturnCode;
  TGComboBox* m_normalizationSelector;
  TCanvas *c125;
  TRootEmbeddedCanvas *fRootEmbeddedCanvas746;
  
  ClassDef(ReferencePicker, 0)  
  
};

#endif /*REFERENCEPICKER_H_*/
