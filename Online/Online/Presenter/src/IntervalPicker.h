// $Id: IntervalPicker.h,v 1.11 2010-08-12 15:43:00 robbep Exp $
#ifndef INTERVALPICKER_H_
#define INTERVALPICKER_H_
#include <string>

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TGFrame.h>
#ifdef WIN32
#pragma warning( pop )
#endif

#include "presenter.h"

class PresenterInformation;
class Archive;
class TGVerticalFrame;
class TGTextButton;
class TGTextEntry;
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
                          m_endRun( 0 ),
                          m_startDay(0), m_startMonth(0), m_startYear(0), 
                          m_startHour(0), m_startMin(0), m_startSec(0) { } ;

  /// Destructor
  virtual ~IntervalPickerData( ) { ; } ;

  /// duration as a string
  const char * durationString( ) ;

  /// End time as a string
  const char * startTimeString( ) ;

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
  } ;

  void setDuration( int h, int m ) { m_durationHour = h; m_durationMinute = m; } ;

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

  int m_durationHour;
  int m_durationMinute;
};

/** @class IntervalPicker IntervalPicker.h
 *
 *  Dialog window: choose time or run for history mode
 */
class IntervalPicker : public TGTransientFrame {
public:
  /// Constructor
  IntervalPicker( PresenterInformation * presInfo ,
                  const TGWindow * main ,
                  Archive * archive ,
                  pres::MsgLevel verbosity ,
                  RunDB * runDb ,
                  IntervalPickerData * intData ) ;

  /// Destructor
  virtual ~IntervalPicker();

  /// build widgets on window
  void build();

  /// close window call back
  void CloseWindow();

  // Slots
  //  void lastMinutesRadioButtonToggled(bool on);
  void timeIntervalRadioButtonToggled(bool on);
  void runIntervalRadioButtonToggled(bool on);
  void ok();

  /// Change window according to mode (SingleRun, Time...)
  void setDestination( int index );
  void SelectMode( ) ;

private:
  PresenterInformation * m_presInfo; ///< presenter information
  Archive*             m_archive;   ///< archive object
  pres::MsgLevel       m_verbosity; ///< verbosity from presenter

  TGVerticalFrame*    m_mainVerticalFrame;
  TGTextButton*       m_okButton;
  TGTextButton*       m_cancelButton;
  TGLabel*            m_begUserLabel;
  TGTab*              m_mainTab;
  TGCompositeFrame*   m_timeTabCompositeFrame;
  TGVerticalFrame*    m_intervalTypeRadioBtnGrpVerticalFrame;
  TGLabel*            m_fromTimeIntervalLabel;
  TGNumberEntry*      m_startTimeNumberEntry;
  TGNumberEntry*      m_startDateNumberEntry;
  TGNumberEntry*      m_endTimeNumberEntry;
  TGNumberEntry*      m_endDateNumberEntry;
  TGNumberEntry*      m_stepTimeNumberEntry;
  TGLabel*            m_fromTimeIntervalDateLabel;
  TGNumberEntry*      m_hoursNumberEntry;
  TGLabel*            m_hoursLabel;
  TGLabel*            m_toLabel;
  TGLabel*            m_timeLabel;
  TGLabel*            m_dateLabel;
  TGRadioButton*      m_timeIntervalRadioButton;
  TGNumberEntry*      m_lastRunNumberEntry;

  TGRadioButton*      m_runIntervalRadioButton;
  TGComboBox*         m_runIntervalComboBox;
  TGNumberEntry*      m_runIntervalFromNumberEntry;
  TGLabel*            m_runIntervalToLabel;
  TGNumberEntry*      m_runIntervalToNumberEntry;
  TGComboBox*         m_runDestinationComboBox;
  TGLabel*            m_runDestinationLabel;

  TGComboBox*         m_eventTypeComboBox;
  TGTextEntry*        m_processingTextEntry;

  /// Link to RunDB interface object
  RunDB              *m_runDb ;

  /// Link to interval data object
  IntervalPickerData *m_intData ;

  ClassDef(IntervalPicker, 0)
};

#endif /*INTERVALPICKER_H_*/
