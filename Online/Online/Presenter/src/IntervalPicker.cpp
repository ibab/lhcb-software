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
  nowButton();

  if ( m_presInfo -> currentTime() ) {
    m_startDateNumberEntry->SetDate(m_presInfo->currentTime()->GetYear(),
                                    m_presInfo->currentTime()->GetMonth(),
                                    m_presInfo->currentTime()->GetDay());
    m_startTimeNumberEntry->SetTime(m_presInfo->currentTime()->GetHour(),
                                    m_presInfo->currentTime()->GetMinute(),
                                    m_presInfo->currentTime()->GetSecond());
  }                                

  lastMinutesRadioButtonToggled(true);
  lastRunRadioButtonToggled(true);
  SelectMode( ) ;
  MapWindow() ;
}

//===========================================================================
// Destructor
//===========================================================================
IntervalPicker::~IntervalPicker()
{
  if (IsZombie()) return;
  Cleanup();
}

//===========================================================================
// Construct window
//===========================================================================
void IntervalPicker::build()
{
  SetWindowName("History interval selection");
    
  TGHorizontalFrame *fHorizontalFrame655 = new TGHorizontalFrame(this, 80, 26,
                                                                 kHorizontalFrame);
  m_okButton = new TGTextButton(fHorizontalFrame655, "Ok");
  m_okButton->SetTextJustify(36);
  m_okButton->Resize(24, 22);
  fHorizontalFrame655->AddFrame(m_okButton,
                                new TGLayoutHints(kLHintsRight |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));
  m_okButton->Connect("Clicked()", "IntervalPicker",
                      this, "ok()");                                                  
  m_cancelButton = new TGTextButton(fHorizontalFrame655, "Cancel");
  m_cancelButton->SetTextJustify(36);
  m_cancelButton->Resize(48, 22);
  m_cancelButton->Connect("Clicked()", "IntervalPicker",
                          this, "CloseWindow()");
  fHorizontalFrame655->AddFrame(m_cancelButton,
                                new TGLayoutHints(kLHintsRight |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));

  AddFrame(fHorizontalFrame655, new TGLayoutHints(kLHintsRight |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));

  m_mainVerticalFrame = new TGVerticalFrame(this, 392, 358, kVerticalFrame);
  m_begUserLabel = new TGLabel(m_mainVerticalFrame,
                               "Please select interval for History mode:");
  m_begUserLabel->SetTextJustify(36);
  m_mainVerticalFrame->AddFrame(m_begUserLabel,
                                new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                                  2, 2, 2, 2));

  // tab widget
  m_mainTab = new TGTab(m_mainVerticalFrame, 392, 338);

  m_timeTabCompositeFrame = m_mainTab->AddTab("Time");
  m_timeTabCompositeFrame->SetLayoutManager(new TGVerticalLayout(m_timeTabCompositeFrame));
  m_timeTabCompositeFrame->SetLayoutBroken(kTRUE);

  m_intervalTypeRadioBtnGrpVerticalFrame = new TGVerticalFrame(m_timeTabCompositeFrame,
                                                               402, 208,
                                                               kVerticalFrame);

  m_lastMinutesGrpHorizontalFr = new TGHorizontalFrame(m_intervalTypeRadioBtnGrpVerticalFrame,
                                                       227, 30,
                                                       kHorizontalFrame);
  m_lastMinutesRadioButton = new TGRadioButton(m_lastMinutesGrpHorizontalFr,
                                               "Last");
  m_lastMinutesRadioButton->SetState(kButtonDown);
  m_lastMinutesRadioButton->Connect("Toggled(Bool_t)", "IntervalPicker",
                        this, "lastMinutesRadioButtonToggled(Bool_t)");
                          
  m_lastMinutesGrpHorizontalFr->AddFrame(m_lastMinutesRadioButton,
                                         new TGLayoutHints(kLHintsLeft |
                                                           kLHintsCenterY,
                                                           2, 2, 2, 2));

  m_minutesHorizontalFrame = new TGHorizontalFrame(m_lastMinutesGrpHorizontalFr,
                                                   144, 26,
                                                   kHorizontalFrame);
  m_minutesHorizontalFrame->SetLayoutBroken(kTRUE);
  m_minutesEntry = new TGNumberEntry(m_minutesHorizontalFrame,
                                     (Double_t) 0,6,-1,
                                     TGNumberFormat::kNESHourMin,
                                     TGNumberFormat::kNEAPositive);
  m_minutesEntry->SetTime(0, 10, 0);                                     
  m_minutesHorizontalFrame->AddFrame(m_minutesEntry,
                                     new TGLayoutHints(kLHintsLeft |
                                                       kLHintsBottom,
                                                       2, 2, 2, 2));
  m_minutesEntry->MoveResize(2, 2, 64, 22);
  m_minutesLabel = new TGLabel(m_minutesHorizontalFrame, "hours");
  m_minutesLabel->SetTextJustify(36);
  m_minutesHorizontalFrame->AddFrame(m_minutesLabel,
                                     new TGLayoutHints(kLHintsLeft |
                                                       kLHintsTop |
                                                       kLHintsCenterY,
                                                       2, 2, 2, 2));
  m_minutesLabel->Move(70, 5);

  m_lastMinutesGrpHorizontalFr->AddFrame(m_minutesHorizontalFrame,
                                         new TGLayoutHints(kLHintsLeft |
                                                           kLHintsCenterX |
                                                           kLHintsBottom,
                                                           2, 2, 2, 2));

  m_intervalTypeRadioBtnGrpVerticalFrame->AddFrame(m_lastMinutesGrpHorizontalFr,
                                                   new TGLayoutHints(kLHintsLeft |
                                                                     kLHintsTop,
                                                                     2, 2, 2, 2));

  TGVerticalFrame *fVerticalFrame681 = new TGVerticalFrame(m_intervalTypeRadioBtnGrpVerticalFrame,
                                                           398,136,
                                                           kVerticalFrame);

  TGHorizontalFrame *fHorizontalFrame682 = new TGHorizontalFrame(fVerticalFrame681,
                                                                 393, 64,
                                                                 kHorizontalFrame);
  m_timeIntervalRadioButton = new TGRadioButton(fHorizontalFrame682,
                                                "From");
  m_timeIntervalRadioButton->Connect("Toggled(Bool_t)", "IntervalPicker",
                        this, "timeIntervalRadioButtonToggled(Bool_t)");                                                
  fHorizontalFrame682->AddFrame(m_timeIntervalRadioButton,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));

  TGVerticalFrame *fVerticalFrame684 = new TGVerticalFrame(fHorizontalFrame682,
                                                           335, 60,
                                                           kVerticalFrame);

  TGHorizontalFrame *fHorizontalFrame685 = new TGHorizontalFrame(fVerticalFrame684,
                                                                 331, 26,
                                                                 kHorizontalFrame);
  m_fromTimeIntervalLabel = new TGLabel(fHorizontalFrame685, "Time:");
  m_fromTimeIntervalLabel->SetTextJustify(36);
  fHorizontalFrame685->AddFrame(m_fromTimeIntervalLabel, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  m_startTimeNumberEntry = new TGNumberEntry(fHorizontalFrame685,
                                             (Double_t) 0, 6, -1,
                                             TGNumberFormat::kNESHourMinSec,
                                             TGNumberFormat::kNEAPositive);

  m_startTimeNumberEntry->SetWidth(m_startTimeNumberEntry->GetWidth()+25);


                                                     
  fHorizontalFrame685->AddFrame(m_startTimeNumberEntry,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  fVerticalFrame684->AddFrame(fHorizontalFrame685,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop |
                                                kLHintsExpandX,
                                                2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame701 = new TGHorizontalFrame(fVerticalFrame684,
                                                                 301, 26,
                                                                 kHorizontalFrame);
  m_fromTimeIntervalDateLabel = new TGLabel(fHorizontalFrame701, "Date:");
  m_fromTimeIntervalDateLabel->SetTextJustify(36);
  fHorizontalFrame701->AddFrame(m_fromTimeIntervalDateLabel, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  m_startDateNumberEntry = new TGNumberEntry(fHorizontalFrame701,
                                             (Double_t) 0, 6, -1,
                                             TGNumberFormat::kNESDayMYear,
                                             TGNumberFormat::kNEAPositive);

  m_startDateNumberEntry->SetWidth(m_startDateNumberEntry->GetWidth()+25);
                                             
  fHorizontalFrame701->AddFrame(m_startDateNumberEntry, new TGLayoutHints(kLHintsLeft |
                                                                   kLHintsTop |
                                                                   kLHintsCenterY,
                                                                   2, 2, 2, 2));

  fVerticalFrame684->AddFrame(fHorizontalFrame701,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop |
                                                kLHintsExpandX,
                                                2, 2, 2, 2));

  fHorizontalFrame682->AddFrame(fVerticalFrame684,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));

  fVerticalFrame681->AddFrame(fHorizontalFrame682,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop,
                                                2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame722 = new TGHorizontalFrame(fVerticalFrame681,
                                                                 394, 64,
                                                                 kHorizontalFrame);

  TGHorizontalFrame *fHorizontalFrame723 = new TGHorizontalFrame(fHorizontalFrame722,
                                                                 81, 26,
                                                                 kHorizontalFrame);
  m_toLabel = new TGLabel(fHorizontalFrame723, "to");
  m_toLabel->SetTextJustify(36);
  fHorizontalFrame723->AddFrame(m_toLabel, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  m_nowTextButton = new TGTextButton(fHorizontalFrame723, "Now");
  m_nowTextButton->Connect("Clicked()", "IntervalPicker", this, "nowButton()");
  
  m_nowTextButton->SetTextJustify(36);
  m_nowTextButton->Resize(33, 22);
  fHorizontalFrame723->AddFrame(m_nowTextButton,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsCenterX |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));

  fHorizontalFrame722->AddFrame(fHorizontalFrame723,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));

  TGVerticalFrame *fVerticalFrame726 = new TGVerticalFrame(fHorizontalFrame722,
                                                           365,60,
                                                           kVerticalFrame);

  TGHorizontalFrame *fHorizontalFrame727 = new TGHorizontalFrame(fVerticalFrame726,
                                                                 331, 26,
                                                                 kHorizontalFrame);
  m_timeLabel = new TGLabel(fHorizontalFrame727, "Time:");
  m_timeLabel->SetTextJustify(36);
  fHorizontalFrame727->AddFrame(m_timeLabel, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  m_endTimeNumberEntry = new TGNumberEntry(fHorizontalFrame727,
                                           (Double_t) 0, 6, -1,
                                           TGNumberFormat::kNESHourMinSec,
                                           TGNumberFormat::kNEAPositive);

  m_endTimeNumberEntry->SetWidth(m_endTimeNumberEntry->GetWidth()+25);                                      
                                           
  fHorizontalFrame727->AddFrame(m_endTimeNumberEntry,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  fVerticalFrame726->AddFrame(fHorizontalFrame727,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop |
                                                kLHintsExpandX,
                                                2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame743 = new TGHorizontalFrame(fVerticalFrame726,
                                                                 331, 26,
                                                                 kHorizontalFrame);
  m_dateLabel = new TGLabel(fHorizontalFrame743, "Date:");
  m_dateLabel->SetTextJustify(36);
  fHorizontalFrame743->AddFrame(m_dateLabel, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  m_endDateNumberEntry = new TGNumberEntry(fHorizontalFrame743,
                                           (Double_t) 0, 6, -1,
                                           TGNumberFormat::kNESDayMYear,
                                           TGNumberFormat::kNEAPositive);

  m_endDateNumberEntry->SetWidth(m_endDateNumberEntry->GetWidth()+25);
                                           
  fHorizontalFrame743->AddFrame(m_endDateNumberEntry,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  fVerticalFrame726->AddFrame(fHorizontalFrame743,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop |
                                                kLHintsExpandX,
                                                2, 2, 2, 2));

  fHorizontalFrame722->AddFrame(fVerticalFrame726,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));

  fVerticalFrame681->AddFrame(fHorizontalFrame722,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop,
                                                2, 2, 2, 2));

  m_intervalTypeRadioBtnGrpVerticalFrame->AddFrame(fVerticalFrame681,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop,
                                                2, 2, 2, 2));

  m_timeTabCompositeFrame->AddFrame(m_intervalTypeRadioBtnGrpVerticalFrame,
                                    new TGLayoutHints(kLHintsLeft |
                                                      kLHintsTop,
                                                      2, 2, 2, 2));
  m_intervalTypeRadioBtnGrpVerticalFrame->MoveResize(2, 2, 372, 208);

  // "Set navigation step size" group frame
  TGGroupFrame *fGroupFrame911 = new TGGroupFrame(m_timeTabCompositeFrame,
                                                  "Navigation step size (hh::mm)");

  TGHorizontalFrame *fHorizontalFrame1035 = new TGHorizontalFrame(fGroupFrame911,
                                                                  160, 26,
                                                                  kHorizontalFrame);
  m_stepTimeNumberEntry = new TGNumberEntry(fHorizontalFrame1035,
                                            (Double_t) 0, 6, -1,
                                            TGNumberFormat::kNESHourMin,
                                            TGNumberFormat::kNEAPositive);
  m_stepTimeNumberEntry->SetTime(0, 30, 0);
  fHorizontalFrame1035->AddFrame(m_stepTimeNumberEntry,
                                 new TGLayoutHints(kLHintsLeft |
                                                   kLHintsCenterX |
                                                   kLHintsTop |
                                                   kLHintsCenterY,
                                                   2, 2, 2, 2));
  fGroupFrame911->AddFrame(fHorizontalFrame1035,
                           new TGLayoutHints(kLHintsLeft |
                                             kLHintsCenterX |
                                             kLHintsTop |
                                             kLHintsCenterY,
                                             2, 2, 2, 2));

  fGroupFrame911->SetLayoutManager(new TGVerticalLayout(fGroupFrame911));
  fGroupFrame911->Resize(414, 62);
  m_timeTabCompositeFrame->AddFrame(fGroupFrame911,
                                    new TGLayoutHints(kLHintsLeft |
                                                      kLHintsBottom |
                                                      kLHintsExpandX,
                                                      2, 2, 2, 2));
  fGroupFrame911->MoveResize(2, 249, 384, 62);

  // container of "Tab2"  
  //m_runFillTabCompositeFrame = m_mainTab->AddTab("Run/Fill");
  m_runFillTabCompositeFrame = m_mainTab->AddTab("Run");
  //  m_mainTab->SetEnabled(1, false);
  m_runFillTabCompositeFrame->SetLayoutManager(new TGVerticalLayout(m_runFillTabCompositeFrame));
  m_runFillTabCompositeFrame->SetLayoutBroken(kTRUE);

  TGVerticalFrame *fVerticalFrame766 = new TGVerticalFrame(m_runFillTabCompositeFrame,
                                                           267, 98,
                                                           kVerticalFrame);

  // TGHorizontalFrame *fHorizontalFrame767 = new TGHorizontalFrame(fVerticalFrame766,
//                                                                  157, 30,
//                                                                  kHorizontalFrame);
//   m_lastRunRadioButton = new TGRadioButton(fHorizontalFrame767, "Last");
//   fHorizontalFrame767->AddFrame(m_lastRunRadioButton,
//                                 new TGLayoutHints(kLHintsLeft |
//                                                   kLHintsCenterY,
//                                                   2, 2, 2, 2));
//   m_lastRunRadioButton->Connect("Toggled(Bool_t)", "IntervalPicker",
//                         this, "lastRunRadioButtonToggled(Bool_t)");

//   TGHorizontalFrame *fHorizontalFrame769 = new TGHorizontalFrame(fHorizontalFrame767,
//                                                                  104, 26,
//                                                                  kHorizontalFrame);
//   m_lastRunNumberEntry = new TGNumberEntry(fHorizontalFrame769,
//                                                      1,6,-1,
//                                                      TGNumberFormat::kNESInteger,
//                                                      TGNumberFormat::kNEAPositive);
//   fHorizontalFrame769->AddFrame(m_lastRunNumberEntry,
//                                 new TGLayoutHints(kLHintsLeft |
//                                                   kLHintsBottom,
//                                                   2, 2, 2, 2));
//   m_lastRunLabel = new TGLabel(fHorizontalFrame769, "run(s)");
//   m_lastRunLabel->SetTextJustify(36);
//   fHorizontalFrame769->AddFrame(m_lastRunLabel,
//                                 new TGLayoutHints(kLHintsLeft |
//                                                   kLHintsTop |
//                                                   kLHintsCenterY,
//                                                   2, 2, 2, 2));

//   fHorizontalFrame767->AddFrame(fHorizontalFrame769,
//                                 new TGLayoutHints(kLHintsLeft |
//                                                   kLHintsCenterX |
//                                                   kLHintsBottom,
//                                                   2, 2, 2, 2));

//   fVerticalFrame766->AddFrame(fHorizontalFrame767,
//                               new TGLayoutHints(kLHintsLeft |
//                                                 kLHintsTop,
//                                                 2, 2, 2, 2));

  // TGHorizontalFrame *fHorizontalFrame775 = new TGHorizontalFrame(fVerticalFrame766,
//                                                                  151, 30,
//                                                                  kHorizontalFrame);
//   m_lastFillRadioButton = new TGRadioButton(fHorizontalFrame775, "Last");
//   fHorizontalFrame775->AddFrame(m_lastFillRadioButton,
//                                 new TGLayoutHints(kLHintsLeft |
//                                                   kLHintsCenterY,
//                                                   2, 2, 2, 2));
//   m_lastFillRadioButton->Connect("Toggled(Bool_t)", "IntervalPicker",
//                         this, "lastFillRadioButtonToggled(Bool_t)");

//   TGHorizontalFrame *fHorizontalFrame777 = new TGHorizontalFrame(fHorizontalFrame775,
//                                                                  98, 26,
//                                                                  kHorizontalFrame);
//   m_lastFillNumberEntry = new TGNumberEntry(fHorizontalFrame777,
//                                              1,6,-1,
//                                              TGNumberFormat::kNESInteger,
//                                              TGNumberFormat::kNEAPositive);
//   fHorizontalFrame777->AddFrame(m_lastFillNumberEntry,
//                                 new TGLayoutHints(kLHintsLeft |
//                                                   kLHintsBottom,
//                                                   2, 2, 2, 2));
//   m_lastFillLabel = new TGLabel(fHorizontalFrame777, "fill(s)");
//   m_lastFillLabel->SetTextJustify(36);
//   fHorizontalFrame777->AddFrame(m_lastFillLabel, new TGLayoutHints(kLHintsLeft |
//                                                              kLHintsTop |
//                                                              kLHintsCenterY,
//                                                              2, 2, 2, 2));

//   fHorizontalFrame775->AddFrame(fHorizontalFrame777,
//                                 new TGLayoutHints(kLHintsLeft |
//                                                   kLHintsCenterX |
//                                                   kLHintsBottom,
//                                                   2, 2, 2, 2));

//   fVerticalFrame766->AddFrame(fHorizontalFrame775,
//                               new TGLayoutHints(kLHintsLeft | 
//                                                 kLHintsTop,
//                                                 2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame783 = new TGHorizontalFrame(fVerticalFrame766,
                                                                 263, 26,
                                                                 kHorizontalFrame);
  m_runFillIntervalRadioButton = new TGRadioButton(fHorizontalFrame783,
                                                   "From");
  m_runFillIntervalRadioButton->SetState(kButtonDown);    
  m_runFillIntervalRadioButton->Connect("Toggled(Bool_t)", "IntervalPicker",
                          this, "runFillIntervalRadioButtonToggled(Bool_t)");                                                   
  fHorizontalFrame783->AddFrame(m_runFillIntervalRadioButton,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  // combo box
  m_runFillIntervalComboBox = new TGComboBox(fHorizontalFrame783, -1,
                                             kHorizontalFrame |
                                             kSunkenFrame |
                                             kDoubleBorder |
                                             kOwnBackground);
  m_runFillIntervalComboBox->AddEntry("run", 1);
//  fComboBox785->AddEntry("fill", 2);
  m_runFillIntervalComboBox->Resize(48, 22);
  m_runFillIntervalComboBox->Select(1);
  fHorizontalFrame783->AddFrame(m_runFillIntervalComboBox,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));
  m_runFillIntervalFromNumberEntry = new TGNumberEntry(fHorizontalFrame783,
                                                     1, 7, -1,
                                                     TGNumberFormat::kNESInteger,
                                                     TGNumberFormat::kNEAPositive);
  fHorizontalFrame783->AddFrame(m_runFillIntervalFromNumberEntry,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));
  m_runFillIntervalToLabel = new TGLabel(fHorizontalFrame783, "to");
  m_runFillIntervalToLabel->SetTextJustify(36);
  fHorizontalFrame783->AddFrame(m_runFillIntervalToLabel,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));
  m_runFillIntervalToNumberEntry = new TGNumberEntry(fHorizontalFrame783,
                                                     1, 6, -1,
                                                     TGNumberFormat::kNESInteger,
                                                     TGNumberFormat::kNEAPositive);
  fHorizontalFrame783->AddFrame(m_runFillIntervalToNumberEntry,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));

  fVerticalFrame766->AddFrame(fHorizontalFrame783,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsExpandX,
                                                2, 2, 2, 2));

  m_runFillTabCompositeFrame->AddFrame(fVerticalFrame766,
                               new TGLayoutHints(kLHintsLeft |
                                                 kLHintsTop,
                                                 2, 2, 2, 2));
  fVerticalFrame766->MoveResize(2, 2, 267, 98);

  // "Set navigation step size" group frame
  // TGGroupFrame *fGroupFrame1084 = new TGGroupFrame(m_runFillTabCompositeFrame,
//                                                    "Set navigation step size");

//   TGHorizontalFrame *fHorizontalFrame1444 = new TGHorizontalFrame(fGroupFrame1084,
//                                                                   152, 26,
//                                                                   kHorizontalFrame);
//   m_runFillStepSizeNumberEntry = new TGNumberEntry(fHorizontalFrame1444,
//                                                   1, 9, -1,
//                                                   TGNumberFormat::kNESInteger,
//                                                   TGNumberFormat::kNEAPositive);
//   fHorizontalFrame1444->AddFrame(m_runFillStepSizeNumberEntry,
//                                  new TGLayoutHints(kLHintsLeft |
//                                                    kLHintsCenterX |
//                                                    kLHintsTop |
//                                                    kLHintsCenterY,
//                                                    2, 2, 2, 2));

//   // combo box
//   m_runFillStepSizeComboBox = new TGComboBox(fHorizontalFrame1444, -1,
//                                              kHorizontalFrame |
//                                              kSunkenFrame |
//                                              kDoubleBorder |
//                                              kOwnBackground);
//   m_runFillStepSizeComboBox->AddEntry("run(s)", 1);
// //  fComboBox1449->AddEntry("fill(s)", 2);
//   m_runFillStepSizeComboBox->Resize(64, 22);
//   m_runFillStepSizeComboBox->Select(1);
//   m_runFillStepSizeComboBox->SetEnabled(false);
//   fHorizontalFrame1444->AddFrame(m_runFillStepSizeComboBox,
//                                  new TGLayoutHints(kLHintsLeft |
//                                                    kLHintsCenterX |
//                                                    kLHintsTop |
//                                                    kLHintsCenterY,
//                                                    2, 2, 2, 2));

//   fGroupFrame1084->AddFrame(fHorizontalFrame1444,
//                             new TGLayoutHints(kLHintsLeft |
//                                               kLHintsCenterX |
//                                               kLHintsCenterY |
//                                               kLHintsBottom,
//                                               2, 2, 2, 2));

//   fGroupFrame1084->SetLayoutManager(new TGVerticalLayout(fGroupFrame1084));
//   fGroupFrame1084->Resize(384, 62);
//   m_runFillTabCompositeFrame->AddFrame(fGroupFrame1084,
//                                new TGLayoutHints(kLHintsLeft |
//                                                  kLHintsBottom |
//                                                  kLHintsExpandX,2, 2, 2, 2));
//   fGroupFrame1084->MoveResize(2, 249, 384, 62);

//  fTab660->SetTab(1);

  m_mainTab->Resize(m_mainTab->GetDefaultSize());
  m_mainVerticalFrame->AddFrame(m_mainTab, new TGLayoutHints(kLHintsLeft |
                                                         kLHintsCenterX));

  AddFrame(m_mainVerticalFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                                kLHintsCenterY,2, 2, 2, 2));
  SetMWMHints(kMWMDecorAll,
              kMWMFuncAll,
              kMWMInputModeless);
  
  MapSubwindows();             
  Resize(GetDefaultSize());

//  MoveResize(8,16,388,286);
  MapWindow() ;
  Resize(GetDefaultSize());

}

//===========================================================================
// Close Window
//===========================================================================
void IntervalPicker::CloseWindow()
{
  m_okButton->SetState(kButtonDisabled);
  m_cancelButton->SetState(kButtonDisabled);
  m_okButton->SetState(kButtonEngaged);
  m_okButton->SetState(kButtonUp);
  m_cancelButton->SetState(kButtonEngaged);
  m_cancelButton->SetState(kButtonUp);
  DeleteWindow();
}

//===========================================================================
// Handler for last minute button
//===========================================================================
void IntervalPicker::lastMinutesRadioButtonToggled(bool on)
{
  if (on) {
    m_timeIntervalRadioButton->SetDown(true);
    m_timeIntervalRadioButton->SetOn(false);
    m_fromTimeIntervalLabel->Disable(true);
    m_fromTimeIntervalDateLabel->Disable(true);
    m_toLabel->Disable(true);
    m_nowTextButton->SetState(kButtonDisabled);
    m_timeLabel->Disable(true);
    m_dateLabel->Disable(true);
    m_startTimeNumberEntry->SetState(false);
    m_startDateNumberEntry->SetState(false);
    m_endTimeNumberEntry->SetState(false);
    m_endDateNumberEntry->SetState(false);
    
    m_lastMinutesRadioButton->SetDown(false);
    m_lastMinutesRadioButton->SetOn(true);
    m_minutesLabel->Disable(false);
    m_minutesEntry->SetState(true);
  }
}

//===========================================================================
// call back function for time interval button
//===========================================================================
void IntervalPicker::timeIntervalRadioButtonToggled(bool on)
{
  if (on) {
    m_lastMinutesRadioButton->SetDown(true);
    m_lastMinutesRadioButton->SetOn(false);
    m_minutesLabel->Disable(true);
    m_minutesEntry->SetState(false);
  
    m_timeIntervalRadioButton->SetDown(false);
    m_timeIntervalRadioButton->SetOn(true);
    m_fromTimeIntervalLabel->Disable(false);
    m_fromTimeIntervalDateLabel->Disable(false);
    m_toLabel->Disable(false);
    m_nowTextButton->SetEnabled(true);
    m_timeLabel->Disable(false);
    m_dateLabel->Disable(false);
    m_startTimeNumberEntry->SetState(true);
    m_startDateNumberEntry->SetState(true);
    m_endTimeNumberEntry->SetState(true);
    m_endDateNumberEntry->SetState(true);
  }
}

//===========================================================================
// call back function for last run button
//===========================================================================
void IntervalPicker::lastRunRadioButtonToggled(bool on)
{
  if (on) {
    // m_lastFillRadioButton->SetDown(true);
//     m_lastFillRadioButton->SetOn(false);
//     m_lastFillLabel->Disable(true);
//     m_lastFillNumberEntry->SetState(false);
    
    m_runFillIntervalRadioButton->SetDown(true);
    m_runFillIntervalRadioButton->SetOn(false);
    m_runFillIntervalToLabel->Disable(true);
    m_runFillIntervalFromNumberEntry->SetState(false);
    m_runFillIntervalToNumberEntry->SetState(false);
    m_runFillIntervalComboBox->SetEnabled(false);
    
    //m_lastRunRadioButton->SetDown(false);
    //m_lastRunRadioButton->SetOn(true);
    //m_lastRunLabel->Disable(false);
    //m_lastRunNumberEntry->SetState(false);
  }
}

//===========================================================================
// call back for last fill button
//===========================================================================
void IntervalPicker::lastFillRadioButtonToggled(bool on)
{
  if (on) {
    lastRunRadioButtonToggled(true);
  }
}

//===========================================================================
// call back for fill interval button
//===========================================================================
void IntervalPicker::runFillIntervalRadioButtonToggled(bool on)
{
  if (on) {
    //m_lastRunRadioButton->SetDown(true);
    //m_lastRunRadioButton->SetOn(false);
    //m_lastRunLabel->Disable(true);
    //m_lastRunNumberEntry->SetState(false);
    
    // m_lastFillRadioButton->SetDown(true);
//     m_lastFillRadioButton->SetOn(false);
//     m_lastFillLabel->Disable(true);
//     m_lastFillNumberEntry->SetState(false);
    
    m_runFillIntervalRadioButton->SetDown(false);
    m_runFillIntervalRadioButton->SetOn(true);
    m_runFillIntervalToLabel->Disable(false);
    m_runFillIntervalFromNumberEntry->SetState(true);
    m_runFillIntervalToNumberEntry->SetState(true);
    m_runFillIntervalComboBox->SetEnabled(true);
  }
}

//===========================================================================
// call back for now button
//===========================================================================
void IntervalPicker::nowButton()
{
  if (m_presInfo->currentTime()) {
    m_presInfo->currentTime()->Set();
    m_endDateNumberEntry->SetDate(m_presInfo->currentTime()->GetYear(),
                                  m_presInfo->currentTime()->GetMonth(),
                                  m_presInfo->currentTime()->GetDay());
    m_endTimeNumberEntry->SetTime(m_presInfo->currentTime()->GetHour(),
                                  m_presInfo->currentTime()->GetMinute(),
                                  m_presInfo->currentTime()->GetSecond());
  }
}

//===========================================================================
// Call back for OK button
//===========================================================================
void IntervalPicker::ok() {  
  if (0 == m_mainTab->GetCurrent()) {
    m_presInfo -> setHistoryMode( pres::s_timeInterval );
    m_presInfo -> setGlobalHistoryByRun( false ) ;
    Int_t year, month, day, hour, min, sec;
    
    if (m_lastMinutesRadioButton->IsDown()) {
      if (m_presInfo->currentTime() ) {
        m_presInfo->currentTime() ->Set();
        year = m_presInfo->currentTime()->GetYear();
        month = m_presInfo->currentTime()->GetMonth();
        day = m_presInfo->currentTime()->GetDay(); 
        hour = m_presInfo->currentTime()->GetHour();
        min = m_presInfo->currentTime()->GetMinute();
        sec = m_presInfo->currentTime()->GetSecond();
        
        m_presInfo-> setGlobalTimePoint( m_archive -> createIsoTimeString(year, month, day,
									  hour, min, sec) ) ;
      }

      m_minutesEntry->GetTime(hour, min, sec);
      std::stringstream pastDurationStringStream;
      pastDurationStringStream << std::setfill('0') << std::setw(2) << hour << ":" <<
                          std::setfill('0') << std::setw(2) << min << ":" <<
                          std::setfill('0') << std::setw(2) << sec;          
      m_presInfo -> setGlobalPastDuration( pastDurationStringStream.str() ) ;
      
    } else if (m_timeIntervalRadioButton->IsDown()) {
      m_startDateNumberEntry->GetDate(year, month, day);
      m_startTimeNumberEntry->GetTime(hour, min, sec);
      std::string startTimeIsoString = m_archive -> createIsoTimeString(year, month, day,
									hour, min, sec);
      m_endDateNumberEntry->GetDate(year, month, day);
      m_endTimeNumberEntry->GetTime(hour, min, sec);          
      std::string  endTimeIsoString = m_archive->createIsoTimeString(year, month, day,
								     hour, min, sec);
      
      m_presInfo->setGlobalTimePoint( endTimeIsoString ) ;
      m_presInfo->setGlobalPastDuration( m_archive ->timeDiff(startTimeIsoString,
							      endTimeIsoString) ) ;

    }
    m_stepTimeNumberEntry->GetTime(hour, min, sec);    
    std::stringstream stepTimeStringStream;
    stepTimeStringStream << std::setfill('0') << std::setw(2) << hour << ":" <<
      std::setfill('0') << std::setw(2) << min << ":" <<
      std::setfill('0') << std::setw(2) << sec;          
    m_presInfo -> setGlobalStepSize( stepTimeStringStream.str() ) ;
  } else if (1 == m_mainTab->GetCurrent()) {
    if (m_runFillIntervalRadioButton->IsDown()) {
      m_presInfo -> setGlobalHistoryByRun( true ) ;
      if ( IntervalPickerData::SingleRun != m_intData -> getMode() ) {
	m_intData -> setEndRun( m_runFillIntervalToNumberEntry->GetIntNumber() ) ;
	std::stringstream endRun;
	endRun << m_intData -> endRun() ;
	m_presInfo -> setGlobalTimePoint( endRun.str() ) ;
	m_intData -> setStartRun( m_runFillIntervalFromNumberEntry->GetIntNumber() ) ;
	std::stringstream runDuration;
	runDuration << (m_intData->endRun() - m_intData -> startRun() );
	m_presInfo -> setGlobalPastDuration( runDuration.str() ) ;
      } else {
	// Single run selection
	int testEndRun = m_runFillIntervalFromNumberEntry->GetIntNumber();

	// Check it is a valid run
	if ( m_runDb -> checkRun( testEndRun ) ) {
	  m_intData -> setEndRun( testEndRun ) ;
	  std::stringstream endRun;
	  endRun << m_intData -> endRun() ;
	  m_presInfo->setGlobalTimePoint( endRun.str() ) ;
	  m_intData -> setStartRun( m_runFillIntervalFromNumberEntry->GetIntNumber() ) ;
	  std::stringstream runDuration;
	  runDuration << (m_intData -> endRun() - m_intData -> startRun() ) ;
	  m_presInfo -> setGlobalPastDuration( runDuration.str() ) ;
	} else {
	  new TGMsgBox( fClient -> GetRoot() , this , "Bad run selection" , 
			"You selected a non-existing run" , kMBIconExclamation ) ;
	}
      }
    }
  }  

  Int_t year , month, day, hour, min, sec;
  m_startDateNumberEntry -> GetDate(year, month, day) ;
  m_startTimeNumberEntry -> GetTime(hour, min, sec)   ;
  m_intData -> setStartTime( year , month , day , hour , min , sec ) ;

  m_endDateNumberEntry->GetDate(year, month, day);
  m_endTimeNumberEntry->GetTime(hour, min, sec);  
  m_intData -> setEndTime( year , month , day , hour , min , sec ) ;

  CloseWindow();
}

//===========================================================================
// Get start time as a string
//===========================================================================
const char* IntervalPickerData::getStartTimeString() {     
  return Form("%d/%d/%d %2d:%2d" , m_startDay , m_startMonth , 
	      m_startYear , m_startHour , m_startMin ) ;
}

//============================================================================
// Get end time as a string
//============================================================================
const char* IntervalPickerData::getEndTimeString() {       
  return Form("%d/%d/%d %02d:%02d" , m_endDay , m_endMonth , 
	      m_endYear , m_endHour , m_endMin ) ;
}

//============================================================================
// Make change to dialog window before showing it
//============================================================================
void IntervalPicker::SelectMode( ) {
  // Select the interesting tab
  if ( IntervalPickerData::SingleRun == m_intData -> getMode() ) {
    int lastrun = m_runDb -> getCurrentRunNumber() ; 
    if ( 0 == lastrun ) 
      lastrun = m_runDb -> getLastRun( ) ;

    SetWindowName( "History run selection" ) ;
    m_mainTab -> GetTabTab( "Time" ) -> SetEnabled( kFALSE ) ; 
    m_mainTab -> SetTab( "Run" , kFALSE ) ;
    m_runFillIntervalRadioButton -> SetState( kButtonDown , kTRUE ) ;

    m_runFillIntervalToLabel -> Disable() ;
    m_runFillIntervalToNumberEntry -> SetState( kFALSE ) ;
   
    m_runFillIntervalFromNumberEntry -> SetIntNumber( lastrun ) ;
  } else {
    SetWindowName("History interval selection");
    
    m_mainTab -> GetTabTab( "Time" ) -> SetEnabled( kTRUE ) ; 
    m_mainTab -> SetTab( "Time" , kFALSE ) ;
    m_runFillIntervalToLabel -> Enable() ;
    m_runFillIntervalToNumberEntry -> SetState( kTRUE ) ;
  }
}
