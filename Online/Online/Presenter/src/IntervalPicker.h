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
class RunDB ;


/** @class IntervalPickerData IntervalPicker.h
 *
 *  Data class to choose time or run for history mode 
 */
class IntervalPickerData {
 public:
  /// mode of picker
  enum PickerMode { 
    TimeInterval , 
    SingleRun 
  } ;

  /// Contructor
  IntervalPickerData( ) : m_mode( TimeInterval ) , m_startRun( 0 ) ,
    m_endRun( 0 ) { } ;

  /// Destructor
  virtual ~IntervalPickerData( ) { ; } ;

  /// Start time as a string
  const char * getStartTimeString( ) ;

  /// End time as a string
  const char * getEndTimeString( ) ;

  /// Set start run
  void setStartRun( const int run ) { m_startRun = run ; } ;

  /// Set end run range
  void setEndRun( const int run ) { m_endRun = run ; } ;

  /// Get start run
  int startRun( ) const { return m_startRun ; } ;
  
  /// Get end run
  int endRun( ) const { return m_endRun ; } ;


  /// Set mode
  void setMode( const PickerMode mode ) { m_mode = mode ; } ;

  /// get mode 
  PickerMode getMode( ) const { return m_mode ; } ;

  /// Set start time
  void setStartTime( int year , int month , int day , int hour , int min ,
		     int sec ) { 
    m_startYear  = year ; m_startMonth = month ; m_startDay = day ; 
    m_startHour = hour , m_startMin = min ; m_startSec = sec ;
  } 

  /// Set end time
  void setEndTime( int year , int month , int day , int hour , int min ,
		   int sec ) { 
    m_endYear  = year ; m_endMonth = month ; m_endDay = day ; 
    m_endHour = hour , m_endMin = min ; m_endSec = sec ;
  } 

 private:
  /// Picker mode
  PickerMode m_mode ;

  /// start run
  int m_startRun ;

  /// end run
  int m_endRun ;

  /// Start time information
  int m_startDay , m_startMonth , m_startYear , m_startHour,
    m_startMin , m_startSec ;

  /// End time information
  int m_endDay , m_endMonth , m_endYear , m_endHour , m_endMin , 
    m_endSec ;
};

/** @class IntervalPicker IntervalPicker.h
 *
 *  Dialog window: choose time or run for history mode 
 */
class IntervalPicker : public TGTransientFrame
{
public:
  /// Constructor
  IntervalPicker(PresenterMainFrame* gui , RunDB * runDb , 
		 IntervalPickerData * intData ) ;

  /// Destructor
  virtual ~IntervalPicker();

  /// build widgets on window
  void build();

  /// close window call back
  void CloseWindow();
  
  // Slots
  void lastMinutesRadioButtonToggled(bool on);
  void timeIntervalRadioButtonToggled(bool on);
  void lastRunRadioButtonToggled(bool on);
  void lastFillRadioButtonToggled(bool on);
  void runFillIntervalRadioButtonToggled(bool on);
  void nowButton();
  void ok();

  /// Change window according to mode (SingleRun, Time...)
  void SelectMode( ) ;

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

  /// Link to RunDB interface object
  RunDB              *m_runDb ;

  /// Link to interval data object
  IntervalPickerData *m_intData ;

  ClassDef(IntervalPicker, 0) 
};

#endif /*INTERVALPICKER_H_*/
