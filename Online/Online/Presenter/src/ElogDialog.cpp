#include <iostream>

#include <TSystem.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGTextEdit.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <TObject.h>
#include <TGMsgBox.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TColor.h>

#include <TGListBox.h>
#include <TGButtonGroup.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGComboBox.h>

#include "ElogDialog.h"
#include "PresenterMainFrame.h"

using namespace pres;

ClassImp(ElogDialog)

ElogDialog::ElogDialog(PresenterMainFrame* gui, int width, int height) :
      TGTransientFrame(gClient->GetRoot(), gui, width, height),
      m_mainFrame(gui),
      m_msgBoxReturnCode(0) {
  SetCleanup(kDeepCleanup);
  Connect("CloseWindow()", "ElogDialog", this, "DontCallClose()");
  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputSystemModal);
}

ElogDialog::~ElogDialog() {
  Cleanup();
}
void ElogDialog::setParameters( std::string& logbook, std::string& username,
                                std::string& system, std::string& subject,
                                std::string& message, int& isOK ) {
  m_logbook  = &logbook;
  m_username = &username;
  m_system   = &system;
  m_subject  = &subject;
  m_message  = &message;
  m_isOK     = &isOK;
  
  build();
  MapWindow();
}

void ElogDialog::ok() {
  TGTextLBEntry * logbook_entry = 
    dynamic_cast< TGTextLBEntry * >( m_logbookListBox->GetSelectedEntry() ) ;

  if ( 0 != logbook_entry ) *m_logbook  = logbook_entry -> GetText() -> Data();

  *m_username =  m_usernameTextEntry->GetDisplayText().Data();
  *m_system   =  m_systemTextEntry->GetDisplayText().Data();
  if ( 0 != m_subjectTextEntry ) *m_subject  =  m_subjectTextEntry->GetDisplayText().Data();
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

  int xButtonSize = 92;
  int yButtonSize = 24;
  int yButtonPos  = 392;

  TGTextButton* okButton = new TGTextButton(elogFrame,"Send to Elog");
  okButton->SetTextJustify(36);
  elogFrame->AddFrame(okButton, layout );
  okButton->MoveResize( xBeg + xSize, yButtonPos, xButtonSize, yButtonSize );
  okButton->Connect("Clicked()", "ElogDialog", this, "ok()");

  TGTextButton*  cancelButton = new TGTextButton(elogFrame,"Cancel");
  cancelButton->SetTextJustify(36);
  elogFrame->AddFrame(cancelButton, layout );
  cancelButton->MoveResize( xBeg+xSize+xSize, yButtonPos, xButtonSize, yButtonSize );
  cancelButton->Connect("Clicked()", "ElogDialog", this, "CloseWindow()");

  TGLabel* m_usernameLabel = new TGLabel(elogFrame,"Username : ");
  m_usernameLabel->SetTextJustify(36);
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
  m_logbookLabel->SetTextJustify(36);
  elogFrame->AddFrame( m_logbookLabel, layout );
  m_logbookLabel->MoveResize( xBeg, yBeg, xSize, 20);

  // List box of all meaningful logbook names.
  m_logbookListBox = new TGComboBox( elogFrame ) ;
  m_logbookListBox -> AddEntry( "Shift"        , 1 ) ;
  m_logbookListBox -> AddEntry( "VELO"         , 2 ) ;
  m_logbookListBox -> AddEntry( "ST"           , 3 ) ;
  m_logbookListBox -> AddEntry( "OT"           , 4 ) ;
  m_logbookListBox -> AddEntry( "RICH"         , 5 ) ;
  m_logbookListBox -> AddEntry( "CALO"         , 6 ) ;
  m_logbookListBox -> AddEntry( "MUON"         , 7 ) ;
  m_logbookListBox -> AddEntry( "L0 Trigger"   , 8 ) ;
  m_logbookListBox -> AddEntry( "PileUp"       , 9 ) ;
  m_logbookListBox -> AddEntry( "HLT Trigger"  , 10 ) ;
  m_logbookListBox -> AddEntry( "Online"       , 11 ) ;

  m_logbookListBox -> Select( 1 , kFALSE ) ;

  elogFrame -> AddFrame( m_logbookListBox , layout ) ;
  m_logbookListBox -> MoveResize( xBeg + xSize, yBeg, xInputSize, 22); 
  yBeg += yStep ;

  m_subjectTextEntry = 0;
  if ( "-none-" != *m_subject ) {
    TGLabel* m_subjectLabel = new TGLabel(elogFrame,"Subject: ");
    m_subjectLabel->SetTextJustify(36);
    elogFrame->AddFrame(m_subjectLabel, layout );
    m_subjectLabel->MoveResize( xBeg, yBeg, xSize, 18);
    m_subjectTextEntry = new TGTextEntry(elogFrame, new TGTextBuffer(15), -1);
    m_subjectTextEntry->SetMaxLength(255);
    m_subjectTextEntry->SetAlignment(kTextLeft);
    m_subjectTextEntry->SetText( (*m_subject).c_str() );
    elogFrame->AddFrame(m_subjectTextEntry, layout );
    m_subjectTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 22);
    yBeg += yStep;
  }  

  TGLabel* m_systemLabel = new TGLabel(elogFrame,"System : ");
  m_systemLabel->SetTextJustify(36);
  elogFrame->AddFrame(m_systemLabel, layout );
  m_systemLabel->MoveResize( xBeg, yBeg, xSize, 18);
  m_systemTextEntry = new TGTextEntry(elogFrame, new TGTextBuffer(15), -1);
  m_systemTextEntry->SetMaxLength(255);
  m_systemTextEntry->SetAlignment(kTextLeft);
  m_systemTextEntry->SetText( (*m_system).c_str() );
  elogFrame->AddFrame(m_systemTextEntry, layout );
  m_systemTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 22);
  yBeg += yStep;

  TGLabel* m_messageLabel = new TGLabel(elogFrame,"Message : ");
  m_messageLabel->SetTextJustify(36);
  elogFrame->AddFrame(m_messageLabel, layout );
  m_messageLabel->MoveResize( xBeg, yBeg, xSize, 18);
  m_messageTextEntry = new TGTextEdit(elogFrame, 1, 1, (*m_message).c_str() );
  elogFrame->AddFrame(m_messageTextEntry, layout );
  m_messageTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 10*22);

  AddFrame(elogFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  elogFrame->MoveResize(0, 0, 646, 435);

  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
  Resize(658, 434);
}
void ElogDialog::CloseWindow() {
  DeleteWindow();
}
