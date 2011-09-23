// $Id: ElogDialog.cpp,v 1.9 2010-10-01 12:52:26 frankb Exp $
// STL
#include <iostream>

// ROOT
#include <TSystem.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGTextEdit.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <TObject.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TGListBox.h>
#include <TGButtonGroup.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGComboBox.h>

// Local
#include "ElogDialog.h"
#include "PresenterMainFrame.h"

using namespace pres;

ClassImp(ElogDialog)

ElogDialog::ElogDialog(PresenterMainFrame* gui, int width, int height) :
      TGTransientFrame(gClient->GetRoot(), gui, width, height) {
  SetCleanup(kDeepCleanup);
  Connect("CloseWindow()", "ElogDialog", this, "DontCallClose()");
  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputSystemModal);
}

ElogDialog::~ElogDialog() {
  Cleanup();
}
void ElogDialog::setParameters( std::string& logbook, std::string& username,
                                std::string& system, std::string& subject,
                                std::string& message, std::string& runNumber, int& isOK ) {
  m_logbook  = &logbook;
  m_username = &username;
  m_system   = &system;
  m_subject  = &subject;
  m_message  = &message;
  m_runNumber= &runNumber;
  m_isOK     = &isOK;
  
  build();
  MapWindow();
}

//=========================================================================
//  
//=========================================================================
void ElogDialog::elog ( ) {
  ok();
  *m_isOK = 2;
}


void ElogDialog::ok() {
  TGTextLBEntry * logbook_entry = 
    dynamic_cast< TGTextLBEntry * >( m_logbookListBox->GetSelectedEntry() ) ;

  if ( 0 != logbook_entry ) *m_logbook  = logbook_entry -> GetText() -> Data();

  *m_username =  m_usernameTextEntry->GetDisplayText().Data();
  *m_system   =  m_systemTextEntry->GetDisplayText().Data();
  if ( 0 != m_subjectTextEntry   ) *m_subject   = m_subjectTextEntry->GetDisplayText().Data();
  if ( 0 != m_runNumberTextEntry ) *m_runNumber = m_runNumberTextEntry->GetDisplayText().Data();
  *m_message  =  m_messageTextEntry->GetText()->AsString();
  
  *m_isOK     = 1;

  CloseWindow();
}
void ElogDialog::build() {

  // main frame
  SetLayoutBroken(true);

  // composite frame
  TGCompositeFrame *elogFrame = new TGCompositeFrame(this, 646, 435,  kVerticalFrame);
  elogFrame->SetLayoutBroken(true);

  TGLayoutHints* layout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2);

  int xBeg = 20;
  int yBeg = 25;
  int xSize = 150;
  int xInputSize = 400;
  int yStep = 30;

  int xButtonSize = 100;
  int yButtonSize = 24;

  TGLabel* m_usernameLabel = new TGLabel(elogFrame,"Username : ");
  m_usernameLabel->SetTextJustify(kTextRight);
  elogFrame->AddFrame(m_usernameLabel, layout );
  m_usernameLabel->MoveResize( xBeg, yBeg, xSize, 18);
  m_usernameTextEntry = new TGTextEntry(elogFrame, new TGTextBuffer(15), -1);
  m_usernameTextEntry->SetMaxLength(255);
  m_usernameTextEntry->SetAlignment(kTextLeft);
  m_usernameTextEntry->SetText( (*m_username).c_str() );
  elogFrame->AddFrame(m_usernameTextEntry, layout );
  m_usernameTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 22);
  yBeg += yStep;

  TGLabel* m_logbookLabel = new TGLabel(elogFrame,"Logbook name : ");
  m_logbookLabel->SetTextJustify(kTextRight);
  elogFrame->AddFrame( m_logbookLabel, layout );
  m_logbookLabel->MoveResize( xBeg, yBeg, xSize, 20);

  // List box of all meaningful logbook names.
  m_logbookListBox = new TGComboBox( elogFrame ) ;

  m_logbookListBox -> AddEntry( m_logbook->c_str(), 1 ) ;
  m_logbookListBox -> AddEntry( "Shift"           , 2 ) ;
  m_logbookListBox -> AddEntry( "VELO"            , 3 ) ;
  m_logbookListBox -> AddEntry( "ST"              , 4 ) ;
  m_logbookListBox -> AddEntry( "OT"              , 5 ) ;
  m_logbookListBox -> AddEntry( "RICH"            , 6 ) ;
  m_logbookListBox -> AddEntry( "CALO"            , 7 ) ;
  m_logbookListBox -> AddEntry( "MUON"            , 8 ) ;
  m_logbookListBox -> AddEntry( "L0 Trigger"      , 9 ) ;
  m_logbookListBox -> AddEntry( "PileUp"          , 10 ) ;
  m_logbookListBox -> AddEntry( "HLT Trigger"     , 11 ) ;
  m_logbookListBox -> AddEntry( "Online"          , 12 ) ;
  m_logbookListBox -> AddEntry( "Data Quality"    , 13 ) ;
  m_logbookListBox -> AddEntry( "Shift-Test"      , 14 ) ;

  m_logbookListBox -> Select( 1  , kFALSE ) ;

  elogFrame -> AddFrame( m_logbookListBox , layout ) ;
  m_logbookListBox -> MoveResize( xBeg + xSize, yBeg, xInputSize, 22); 
  yBeg += yStep ;

  TGLabel* m_systemLabel = new TGLabel(elogFrame,"System : ");
  m_systemLabel->SetTextJustify(kTextRight);
  elogFrame->AddFrame(m_systemLabel, layout );
  m_systemLabel->MoveResize( xBeg, yBeg, xSize, 18);
  m_systemTextEntry = new TGTextEntry(elogFrame, new TGTextBuffer(15), -1);
  m_systemTextEntry->SetMaxLength(255);
  m_systemTextEntry->SetAlignment(kTextLeft);
  m_systemTextEntry->SetText( (*m_system).c_str() );
  elogFrame->AddFrame(m_systemTextEntry, layout );
  m_systemTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 22);
  yBeg += yStep;

  m_runNumberTextEntry = 0;
  if ( "" != *m_runNumber ) {
    TGLabel* m_runNumberLabel = new TGLabel(elogFrame,"RunNumber: ");
    m_runNumberLabel->SetTextJustify(kTextRight);
    elogFrame->AddFrame(m_runNumberLabel, layout );
    m_runNumberLabel->MoveResize( xBeg, yBeg, xSize, 18);
    m_runNumberTextEntry = new TGTextEntry(elogFrame, new TGTextBuffer(15), -1);
    m_runNumberTextEntry->SetMaxLength(255);
    m_runNumberTextEntry->SetAlignment(kTextLeft);
    m_runNumberTextEntry->SetText( (*m_runNumber).c_str() );
    elogFrame->AddFrame(m_runNumberTextEntry, layout );
    m_runNumberTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 22);
    yBeg += yStep;
  }  

  TGLabel* m_subjectLabel = new TGLabel(elogFrame,"(MANDATORY) Subject: ");
  m_subjectLabel->SetTextJustify(kTextRight);
  elogFrame->AddFrame(m_subjectLabel, layout );
  m_subjectLabel->MoveResize( xBeg, yBeg, xSize, 18);
  m_subjectTextEntry = new TGTextEntry(elogFrame, new TGTextBuffer(15), -1);
  m_subjectTextEntry->SetMaxLength(255);
  m_subjectTextEntry->SetAlignment(kTextLeft);
  m_subjectTextEntry->SetText( (*m_subject).c_str() );
  elogFrame->AddFrame(m_subjectTextEntry, layout );
  m_subjectTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 22);
  yBeg += yStep;  


  TGLabel* m_messageLabel = new TGLabel(elogFrame,"Description : ");
  m_messageLabel->SetTextJustify(kTextRight);
  elogFrame->AddFrame(m_messageLabel, layout );
  m_messageLabel->MoveResize( xBeg, yBeg, xSize, 18);
  m_messageTextEntry = new TGTextEdit(elogFrame, 1, 1, (*m_message).c_str() );
  elogFrame->AddFrame(m_messageTextEntry, layout );
  m_messageTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 10*22);
  yBeg += 10*22+10;
  
  int xFirst = xBeg + xSize;

  if ( std::string("Shift") == *m_logbook ) {
    TGTextButton* okButton = new TGTextButton(elogFrame,"Send to Elog and ProblemDb");
    okButton->SetTextJustify(kTextCenterX);
    elogFrame->AddFrame(okButton, layout );
    okButton->MoveResize( xFirst, yBeg, 2*xButtonSize, yButtonSize );
    okButton->Connect("Clicked()", "ElogDialog", this, "ok()");

    TGLabel* tmp = new TGLabel(elogFrame,"(To ProblemDB ONLY if logbook = Shifts) ");
    tmp->SetTextJustify(kTextLeft);
    elogFrame->AddFrame(tmp, layout );
    tmp->MoveResize( xFirst, yBeg+yStep, 300, 22);

    xFirst += 2*xButtonSize + 20;
  } else {
    TGTextButton* elogButton = new TGTextButton(elogFrame,"Send to Elog");
    elogButton->SetTextJustify(kTextCenterX);
    elogFrame->AddFrame(elogButton, layout );
    elogButton->MoveResize( xFirst, yBeg, xButtonSize, yButtonSize );
    elogButton->Connect("Clicked()", "ElogDialog", this, "elog()");
    xFirst += xButtonSize + 20;
  }
  
  TGTextButton*  cancelButton = new TGTextButton(elogFrame,"Cancel");
  cancelButton->SetTextJustify(kTextCenterX);
  elogFrame->AddFrame(cancelButton, layout );
  cancelButton->MoveResize( xFirst, yBeg, xButtonSize, yButtonSize );
  cancelButton->Connect("Clicked()", "ElogDialog", this, "CloseWindow()");
  yBeg += yButtonSize + 40;

  AddFrame(elogFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  elogFrame->MoveResize(0, 0, 646, yBeg);
  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
  Resize(658, yBeg+38 );
}
void ElogDialog::CloseWindow() {
  DeleteWindow();
}
