#ifndef INTERVALPICKER_H_
#define INTERVALPICKER_H_

#include <TGFrame.h>
#include "presenter.h"

class PresenterMainFrame;
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
  pres::MsgLevel  m_verbosity;
  
ClassDef(IntervalPicker, 0) 
};

#endif /*INTERVALPICKER_H_*/
