// $Id: IntervalPicker.cpp,v 1.15 2010-10-01 12:54:11 frankb Exp $
#include "IntervalPicker.h"

// ROOT
#include <TGButton.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGNumberEntry.h>
#include <TDatime.h>
#include <TGMsgBox.h>

// Local
#include "PresenterMainFrame.h"
#include "Archive.h"
#include "RunDB.h"

ClassImp(IntervalPicker)

//===========================================================================
// Constructor
//===========================================================================
  IntervalPicker::IntervalPicker(PresenterInformation * presInfo ,
                                 const TGWindow * main ,
                                 Archive * archive ,
                                 pres::MsgLevel verbosity ,
                                 RunDB * runDb ,
                                 IntervalPickerData * intData ) :
    TGTransientFrame( gClient->GetRoot() , main ) ,
    m_presInfo( presInfo ) ,
    m_archive( archive ) ,
    m_verbosity( verbosity ),
    m_runDb( runDb ) , m_intData( intData )
{
  SetCleanup(kDeepCleanup);
  Connect("CloseWindow()", "IntervalPicker", this, "DontCallClose()");
  SetMWMHints( kMWMDecorAll, kMWMFuncAll, kMWMInputSystemModal ) ;

  build();

  if ( IntervalPickerData::SingleRun != m_intData -> getMode() ) {

    if ( "0-00-00 00:00" == std::string( m_intData->startTimeString() ) ) {
      int yy = m_presInfo->currentTime()->GetYear();
      int mm = m_presInfo->currentTime()->GetMonth();
      int dd = m_presInfo->currentTime()->GetDay();
      int hh = m_presInfo->currentTime()->GetHour();
      int mn = m_presInfo->currentTime()->GetMinute();

      std::vector<int> lastDay;
      lastDay.push_back( 31 );
      lastDay.push_back( 28 );
      lastDay.push_back( 31 );
      lastDay.push_back( 30 );
      lastDay.push_back( 31 );
      lastDay.push_back( 30 );
      lastDay.push_back( 31 );
      lastDay.push_back( 31 );
      lastDay.push_back( 30 );
      lastDay.push_back( 31 );
      lastDay.push_back( 30 );
      lastDay.push_back( 31 );

      //== Correct for default offset
      mn -= 30;
      if ( 0 > mn ) {
        mn += 60;
        hh -= 1;
        if ( 0 > hh ) {
          hh += 24;
          dd -= 1;
          if ( 0 > dd ) {
            mm -= 1;
            dd = lastDay[mm-1];
            if ( 0 > mm ) {
              mm += 12;
              yy -= 1;
            }
          }
        }
      }
      m_startDateNumberEntry->SetDate( yy, mm, dd );
      m_startTimeNumberEntry->SetTime( hh, mn, 0 );
      m_intData -> setStartTime( yy , mm , dd , hh , mn , 0 ) ;
      m_intData->setDuration( 0, 30 );
    } else {
      std::cout << "Current date/time = " << m_intData->startTimeString() << std::endl;
      int yy, mm, dd, hh, mn;
      sscanf( m_intData->startTimeString(), "%4d-%2d-%2d %2d:%2d", &yy, &mm, &dd, &hh, &mn );
      m_startDateNumberEntry->SetDate( yy, mm, dd );
      m_startTimeNumberEntry->SetTime( hh, mn, 0 );
    }
  }
  SelectMode( ) ;
  MapWindow() ;
}

//===========================================================================
// Destructor
//===========================================================================
IntervalPicker::~IntervalPicker() {
  if (IsZombie()) return;
  Cleanup();
}

//===========================================================================
// Construct window
//===========================================================================
void IntervalPicker::build() {
  SetWindowName("History interval selection");

  TGLayoutHints* rbLayout  = new TGLayoutHints(kLHintsRight | kLHintsBottom, 2, 2, 2, 2);
  TGLayoutHints* ltLayout  = new TGLayoutHints(kLHintsLeft  | kLHintsTop   , 2, 2, 2, 2);
  TGLayoutHints* ltcLayout = new TGLayoutHints(kLHintsLeft  | kLHintsTop |  kLHintsCenterY, 2, 2, 2, 2);

  //== Answer frame
  TGHorizontalFrame * answerHFrame = new TGHorizontalFrame( this, 80, 26, kHorizontalFrame);
  m_okButton = new TGTextButton(answerHFrame, "Ok");
  m_okButton->SetTextJustify(36);
  m_okButton->Resize(24, 22);
  answerHFrame->AddFrame(m_okButton, rbLayout );
  m_okButton->Connect("Clicked()", "IntervalPicker", this, "ok()");
  m_cancelButton = new TGTextButton(answerHFrame, "Cancel");
  m_cancelButton->SetTextJustify(36);
  m_cancelButton->Resize(48, 22);
  m_cancelButton->Connect("Clicked()", "IntervalPicker", this, "CloseWindow()");
  answerHFrame->AddFrame(m_cancelButton, rbLayout );
  AddFrame(answerHFrame, rbLayout );

  //== Main frame
  m_mainVerticalFrame = new TGVerticalFrame(this, 392, 358, kVerticalFrame);
  if ( IntervalPickerData::SingleRun == m_intData -> getMode() ) {
    m_begUserLabel = new TGLabel(m_mainVerticalFrame, "Please select the run:");
  } else {
    m_begUserLabel = new TGLabel(m_mainVerticalFrame, "Please select interval for History mode:");
  }

  m_begUserLabel->SetTextJustify(36);
  m_mainVerticalFrame->AddFrame(m_begUserLabel,ltLayout );

  if ( IntervalPickerData::SingleRun != m_intData -> getMode() ) {
    //== Time selector
    TGHorizontalFrame *timeSelectHFrame = new TGHorizontalFrame( m_mainVerticalFrame,
                                                                 490, 30,
                                                                 kHorizontalFrame);

    m_timeIntervalRadioButton = new TGRadioButton(timeSelectHFrame, "From ");
    m_timeIntervalRadioButton->Connect("Toggled(Bool_t)", "IntervalPicker",
                                       this, "timeIntervalRadioButtonToggled(Bool_t)");
    timeSelectHFrame->AddFrame(m_timeIntervalRadioButton, ltLayout );

    m_startDateNumberEntry = new TGNumberEntry(timeSelectHFrame,
                                               (Double_t) 0, 6, -1,
                                               TGNumberFormat::kNESDayMYear,
                                               TGNumberFormat::kNEAPositive);
    m_startDateNumberEntry->SetWidth(m_startDateNumberEntry->GetWidth()+25);
    timeSelectHFrame->AddFrame(m_startDateNumberEntry, ltcLayout );

    TGLabel* atLabel = new TGLabel( timeSelectHFrame, " at ");
    atLabel->SetTextJustify(36);
    timeSelectHFrame->AddFrame( atLabel, ltcLayout );

    m_startTimeNumberEntry = new TGNumberEntry( timeSelectHFrame,
                                                (Double_t) 0, 6, -1,
                                                TGNumberFormat::kNESHourMinSec,
                                                TGNumberFormat::kNEAPositive);
    m_startTimeNumberEntry->SetWidth(m_startTimeNumberEntry->GetWidth()+25);
    timeSelectHFrame->AddFrame(m_startTimeNumberEntry, ltcLayout );

    TGLabel* forLabel = new TGLabel( timeSelectHFrame, " for ");
    forLabel->SetTextJustify(36);
    timeSelectHFrame->AddFrame( forLabel, ltcLayout );

    m_stepTimeNumberEntry = new TGNumberEntry( timeSelectHFrame,
                                               (Double_t) 0, 6, -1,
                                               TGNumberFormat::kNESHourMin,
                                               TGNumberFormat::kNEAPositive);
    m_stepTimeNumberEntry->SetTime(0, 30, 0);
    timeSelectHFrame->AddFrame( m_stepTimeNumberEntry,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsCenterX |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));
    TGLabel* scaleLabel = new TGLabel( timeSelectHFrame, " hh:mm");
    scaleLabel->SetTextJustify(36);
    timeSelectHFrame->AddFrame( scaleLabel, ltcLayout );

    m_mainVerticalFrame->AddFrame( timeSelectHFrame,
                                   new TGLayoutHints(kLHintsLeft |
                                                     kLHintsExpandX,
                                                     2, 2, 2, 2));
  }

  //===========================================================================
  //== Run destination Frame
  //===========================================================================

  TGHorizontalFrame *runDestHFrame = new TGHorizontalFrame( m_mainVerticalFrame,
                                                            490, 90,
                                                            kHorizontalFrame);

  m_runDestinationLabel = new TGLabel(runDestHFrame, "     Run destination : ");
  m_runDestinationLabel->SetTextJustify(36);
  runDestHFrame->AddFrame(m_runDestinationLabel, ltcLayout );

  m_runDestinationComboBox = new TGComboBox( runDestHFrame, -1,
                                             kHorizontalFrame |
                                             kSunkenFrame |
                                             kDoubleBorder |
                                             kOwnBackground);
  m_runDestinationComboBox->AddEntry( "OFFLINE", 1);
  m_runDestinationComboBox->AddEntry( "any", 2);
  m_runDestinationComboBox->Resize(100, 22);
  if ( m_runDb->getDestination() == "OFFLINE" ) {
    m_runDestinationComboBox->Select(1);
  } else {
    m_runDestinationComboBox->Select(2);
  }
  m_runDestinationComboBox->Connect( "Selected(Int_t)", "IntervalPicker", this, "setDestination(Int_t)");
  runDestHFrame->AddFrame( m_runDestinationComboBox, ltLayout );

  m_mainVerticalFrame->AddFrame( runDestHFrame,
                                 new TGLayoutHints(kLHintsLeft |
                                                   kLHintsExpandX,
                                                   2, 2, 2, 2));

  //== run selection frame
  TGHorizontalFrame* runSelectHFrame = new TGHorizontalFrame( m_mainVerticalFrame,
                                                              490, 26,
                                                              kHorizontalFrame);
  if ( IntervalPickerData::SingleRun == m_intData -> getMode() ) {
    m_runIntervalRadioButton = new TGRadioButton( runSelectHFrame, "Run ");
  } else {
    m_runIntervalRadioButton = new TGRadioButton( runSelectHFrame, "Runs from ");
  }

  m_runIntervalRadioButton->SetState(kButtonDown);
  m_runIntervalRadioButton->Connect("Toggled(Bool_t)", "IntervalPicker",
                                    this, "runIntervalRadioButtonToggled(Bool_t)");
  runSelectHFrame->AddFrame(m_runIntervalRadioButton, ltcLayout );

  m_runIntervalFromNumberEntry = new TGNumberEntry( runSelectHFrame,
                                                    1, 7, -1,
                                                    TGNumberFormat::kNESInteger,
                                                    TGNumberFormat::kNEAPositive);
  runSelectHFrame->AddFrame(m_runIntervalFromNumberEntry, ltLayout );

  if ( IntervalPickerData::SingleRun != m_intData -> getMode() ) {
    m_runIntervalToLabel = new TGLabel(runSelectHFrame, "to");
    m_runIntervalToLabel->SetTextJustify(36);
    runSelectHFrame->AddFrame( m_runIntervalToLabel, ltcLayout );

    m_runIntervalToNumberEntry = new TGNumberEntry(runSelectHFrame,
                                                   1, 6, -1,
                                                   TGNumberFormat::kNESInteger,
                                                   TGNumberFormat::kNEAPositive);
    runSelectHFrame->AddFrame(m_runIntervalToNumberEntry, ltLayout );
  }

  m_mainVerticalFrame->AddFrame( runSelectHFrame,
                                 new TGLayoutHints(kLHintsLeft |
                                                   kLHintsExpandX,
                                                   2, 2, 2, 2));

  //== in offline context, select processing pass and event type
  if ( m_presInfo->offlineContext() ) {
    TGHorizontalFrame* processingSelectHFrame = new TGHorizontalFrame( m_mainVerticalFrame,
                                                                       490, 26,
                                                                       kHorizontalFrame);
    TGLabel* processingLabel = new TGLabel( processingSelectHFrame, "Processing : ");
    processingLabel->SetTextJustify(36);
    processingSelectHFrame->AddFrame( processingLabel, ltcLayout );
    
    m_processingTextEntry = new TGTextEntry( processingSelectHFrame, new TGTextBuffer(15), -1);
    m_processingTextEntry->SetMaxLength(50);
    m_processingTextEntry->SetAlignment(kTextLeft);
    m_processingTextEntry->SetText( m_presInfo->processing().c_str() );
    processingSelectHFrame->AddFrame(m_processingTextEntry, ltcLayout );
  
    m_mainVerticalFrame->AddFrame( processingSelectHFrame,
                                   new TGLayoutHints(kLHintsLeft |
                                                     kLHintsExpandX,
                                                     2, 2, 2, 2));


    TGHorizontalFrame* eventTypeHFrame = new TGHorizontalFrame( m_mainVerticalFrame,
                                                                490, 26,
                                                                kHorizontalFrame);
    TGLabel* eventTypeLabel = new TGLabel( eventTypeHFrame, "Event type : ");
    eventTypeLabel->SetTextJustify(36);
    eventTypeHFrame->AddFrame( eventTypeLabel, ltcLayout );

    m_eventTypeComboBox = new TGComboBox( eventTypeHFrame, -1,
                                          kHorizontalFrame |
                                          kSunkenFrame |
                                          kDoubleBorder |
                                          kOwnBackground);
    m_eventTypeComboBox->AddEntry( "EXPRESS", 1);
    m_eventTypeComboBox->AddEntry( "FULL", 2);
    m_eventTypeComboBox->Resize(100, 22);
    if ( m_presInfo->eventType() == "EXPRESS" ) {
      m_eventTypeComboBox->Select(1);
    } else {
      m_eventTypeComboBox->Select(2);
    } 
    eventTypeHFrame->AddFrame( m_eventTypeComboBox, ltLayout );

    m_mainVerticalFrame->AddFrame( eventTypeHFrame,
                                   new TGLayoutHints(kLHintsLeft |
                                                     kLHintsExpandX,
                                                     2, 2, 2, 2));
    
  }

  AddFrame(m_mainVerticalFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                                  kLHintsCenterY,2, 2, 2, 2));
  SetMWMHints(kMWMDecorAll,
              kMWMFuncAll,
              kMWMInputModeless);

  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow() ;
  Resize(GetDefaultSize());

}

//===========================================================================
// Close Window
//===========================================================================
void IntervalPicker::CloseWindow() {
  m_okButton->SetState(kButtonDisabled);
  m_cancelButton->SetState(kButtonDisabled);
  m_okButton->SetState(kButtonEngaged);
  m_okButton->SetState(kButtonUp);
  m_cancelButton->SetState(kButtonEngaged);
  m_cancelButton->SetState(kButtonUp);
  DeleteWindow();
}
//===========================================================================
// call back function for time interval button
//===========================================================================
void IntervalPicker::timeIntervalRadioButtonToggled(bool on) {
  if (on) {
    m_runIntervalRadioButton->SetDown(true);
    m_runIntervalRadioButton->SetOn(false);
    m_timeIntervalRadioButton->SetDown(false);
    m_timeIntervalRadioButton->SetOn(true);
    m_startTimeNumberEntry->SetState(true);
    m_startDateNumberEntry->SetState(true);
  }
}

//===========================================================================
// call back for fill interval button
//===========================================================================
void IntervalPicker::runIntervalRadioButtonToggled(bool on) {
  if (on) {
    if ( IntervalPickerData::SingleRun != m_intData -> getMode() ) {
      m_timeIntervalRadioButton->SetDown(true);
      m_timeIntervalRadioButton->SetOn(false);
      m_runIntervalRadioButton->SetDown(false);
      m_runIntervalRadioButton->SetOn(true);
      m_runIntervalToLabel->Disable(false);
      m_runIntervalFromNumberEntry->SetState(true);
      m_runIntervalToNumberEntry->SetState(true);
    }
  }
}

//===========================================================================
// Call back for OK button
//===========================================================================
void IntervalPicker::ok() {
  m_presInfo -> setHistoryMode( pres::s_timeInterval );
  m_presInfo -> setGlobalHistoryByRun( false ) ;

  if ( IntervalPickerData::SingleRun == m_intData -> getMode() ) {     // Single run selection
    m_presInfo -> setGlobalHistoryByRun( true ) ;
    int testEndRun = m_runIntervalFromNumberEntry->GetIntNumber();
    // Check it is a valid run
    if ( m_runDb -> checkRun( testEndRun ) ) {
      m_intData -> setEndRun( testEndRun ) ;
      std::stringstream endRun;
      endRun << m_intData -> endRun() ;
      m_presInfo->setGlobalTimePoint( endRun.str() ) ;
      m_intData -> setStartRun( m_runIntervalFromNumberEntry->GetIntNumber() ) ;
      std::stringstream runDuration;
      runDuration << (m_intData -> endRun() - m_intData -> startRun() ) ;
      m_presInfo -> setGlobalPastDuration( runDuration.str() ) ;
    } else {
      new TGMsgBox( fClient -> GetRoot() , this , "Bad run selection" ,
                    "You selected a non-existing run for this destination" ,
                    kMBIconExclamation ) ;
    }

    if ( m_presInfo->offlineContext() ) {
      std::string processing( m_processingTextEntry->GetDisplayText().Data() );
      std::string eventType( (dynamic_cast<TGTextLBEntry*>(m_eventTypeComboBox->GetSelectedEntry() ))->GetText()->Data() );
      m_presInfo->setProcessing( processing );
      m_presInfo->setEventType( eventType );
      std::cout << "Selected processing " << processing << ", Event type " << eventType << std::endl;
    }
  } else {
    Int_t year, month, day, hour, min, sec;
    if (m_timeIntervalRadioButton->IsDown()) {
      m_startDateNumberEntry->GetDate(year, month, day);
      m_startTimeNumberEntry->GetTime(hour, min, sec);
      m_intData -> setStartTime( year , month , day , hour , min , sec ) ;
      std::string startTimeIsoString = m_archive -> createIsoTimeString(year, month, day,
                                                                        hour, min, sec);
      m_stepTimeNumberEntry->GetTime(hour, min, sec);
      m_intData->setDuration( hour, min );
      std::string duration =  m_intData->durationString();
      m_presInfo->setGlobalStepSize( duration ) ;
      
      std::cout << "Selected start time " << startTimeIsoString << " duration " << duration << std::endl;
      
      m_presInfo->setGlobalTimePoint( startTimeIsoString ) ;
      m_presInfo->setGlobalPastDuration( duration );

    } else if (m_runIntervalRadioButton->IsDown()) {
      m_presInfo -> setGlobalHistoryByRun( true ) ;
      int lastRun = m_runIntervalToNumberEntry->GetIntNumber();
      if ( !m_runDb->checkRun( lastRun ) ) {
        lastRun = m_runDb->getNextRun( );
      }
      m_intData -> setEndRun( m_runIntervalToNumberEntry->GetIntNumber() ) ;
      std::stringstream endRun;
      endRun << m_intData -> endRun() ;
      m_presInfo -> setGlobalTimePoint( endRun.str() ) ;
      m_intData -> setStartRun( m_runIntervalFromNumberEntry->GetIntNumber() ) ;
      std::stringstream runDuration;
      runDuration << (m_intData->endRun() - m_intData -> startRun() );
      m_presInfo -> setGlobalPastDuration( runDuration.str() ) ;
    }
  }
  CloseWindow();
}

//===========================================================================
// Get start time as a string
//===========================================================================
const char* IntervalPickerData::startTimeString() {
  return Form("%d-%02d-%02d %02d:%02d" ,  m_startYear, m_startMonth, m_startDay,
              m_startHour , m_startMin ) ;
}
//===========================================================================
// Get start time as a string
//===========================================================================
const char* IntervalPickerData::durationString() {
  return Form( "%02d:%02d:00", m_durationHour, m_durationMinute );
}

//============================================================================
// Make change to dialog window before showing it
//============================================================================
void IntervalPicker::SelectMode( ) {
  // Select the interesting tab
  int lastrun = m_runDb -> getCurrentRunNumber() ;
  if ( 0 == lastrun ) lastrun = m_runDb -> getLastRun( ) ;
  m_runIntervalFromNumberEntry -> SetIntNumber( lastrun ) ;

  if ( IntervalPickerData::SingleRun == m_intData -> getMode() ) {
    SetWindowName( "History run selection" ) ;
  } else {
    m_runIntervalToNumberEntry -> SetIntNumber( lastrun ) ;
    SetWindowName("History interval selection");
    if ( m_presInfo->globalHistoryByRun() ) {
      runIntervalRadioButtonToggled( true );
    } else {
      timeIntervalRadioButtonToggled( true );
    }
  }
}

//=========================================================================
//
//=========================================================================
void IntervalPicker::setDestination( int index ) {
  if ( 1 == index ) {
    m_runDb->setDestination( "OFFLINE" );
  } else {
    m_runDb->setDestination( "" );
  }
  int lastrun = m_runDb -> getLastRun( ) ;
  m_runIntervalFromNumberEntry -> SetIntNumber( lastrun ) ;
}
