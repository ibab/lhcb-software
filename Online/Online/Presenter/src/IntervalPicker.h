#ifndef INTERVALPICKER_H_
#define INTERVALPICKER_H_

#include <TGFrame.h>
#include "presenter.h"

class PresenterMainFrame;
class Archive;
class TGVerticalFrame;
class TGTextButton;
class TGRadioButton;
class TGLabel;
class TGTab;
class TGCompositeFrame;
class TGHorizontalFrame;
class TGNumberEntry;
class TGComboBox;

class IntervalPicker : public TGTransientFrame
{
public:
	IntervalPicker(PresenterMainFrame* gui);
	virtual ~IntervalPicker();
  void build();
  void CloseWindow();
  
  // Slots
  void lastMinutesRadioButtonToggled(bool on);
  void timeIntervalRadioButtonToggled(bool on);
  void lastRunRadioButtonToggled(bool on);
  void lastFillRadioButtonToggled(bool on);
  void runFillIntervalRadioButtonToggled(bool on);
  void nowButton();
  void ok();
  
  const char* startTimeString();
  const char* endTimeString();

  int startRun() {return m_startRun;}
  int endRun() {return m_endRun;}


private:
  PresenterMainFrame* m_mainFrame;
  Archive*  m_archive;
  pres::MsgLevel  m_verbosity;
  
  TGVerticalFrame*    m_mainVerticalFrame;
  TGTextButton*       m_okButton;
  TGTextButton*       m_cancelButton;
  TGLabel*            m_begUserLabel;
  TGTab*              m_mainTab;
  TGCompositeFrame*   m_timeTabCompositeFrame;
  TGVerticalFrame*    m_intervalTypeRadioBtnGrpVerticalFrame;
  TGHorizontalFrame*  m_lastMinutesGrpHorizontalFr;
  TGRadioButton*      m_lastMinutesRadioButton;
  TGLabel*            m_fromTimeIntervalLabel;
  TGNumberEntry*      m_startTimeNumberEntry;
  TGNumberEntry*      m_startDateNumberEntry;
  TGNumberEntry*      m_endTimeNumberEntry;
  TGNumberEntry*      m_endDateNumberEntry;
  TGNumberEntry*      m_stepTimeNumberEntry;
  TGLabel*            m_fromTimeIntervalDateLabel;
  TGHorizontalFrame*  m_minutesHorizontalFrame;
  TGNumberEntry*      m_minutesEntry;
  TGLabel*            m_minutesLabel;
  TGNumberEntry*      m_hoursNumberEntry;
  TGLabel*            m_hoursLabel;
  TGLabel*            m_toLabel;
  TGTextButton*       m_nowTextButton;
  TGLabel*            m_timeLabel;
  TGLabel*            m_dateLabel;
  TGRadioButton*      m_timeIntervalRadioButton;
  TGRadioButton*      m_lastRunRadioButton;
  TGLabel*            m_lastRunLabel;
  TGNumberEntry*      m_lastRunNumberEntry;
  TGRadioButton*      m_lastFillRadioButton;
  TGNumberEntry*      m_lastFillNumberEntry;
  TGLabel*            m_lastFillLabel;
  TGRadioButton*      m_runFillIntervalRadioButton;
  TGComboBox*         m_runFillIntervalComboBox;
  TGNumberEntry*      m_runFillIntervalFromNumberEntry;
  TGLabel*            m_runFillIntervalToLabel;
  TGNumberEntry*      m_runFillIntervalToNumberEntry;
  TGComboBox*         m_runFillStepSizeComboBox;
  TGNumberEntry*      m_runFillStepSizeNumberEntry;
  TGCompositeFrame*   m_runFillTabCompositeFrame;
  int m_startRun;
  int m_endRun;

ClassDef(IntervalPicker, 0) 
};

#endif /*INTERVALPICKER_H_*/
