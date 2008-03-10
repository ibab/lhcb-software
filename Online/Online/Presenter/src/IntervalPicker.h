#ifndef INTERVALPICKER_H_
#define INTERVALPICKER_H_

#include <TGFrame.h>
#include "presenter.h"

class PresenterMainFrame;
class Archive;
class TGTextButton;

class IntervalPicker : public TGTransientFrame
{
public:
	IntervalPicker(PresenterMainFrame* gui);
	virtual ~IntervalPicker();
  void build();
  void CloseWindow();
  
private:
  PresenterMainFrame* m_mainFrame;
  Archive*  m_archive;
  pres::MsgLevel  m_verbosity;
  TGTextButton *m_cancelButton;
  
ClassDef(IntervalPicker, 0) 
};

#endif /*INTERVALPICKER_H_*/
