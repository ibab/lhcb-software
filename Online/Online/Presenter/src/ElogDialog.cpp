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

  *m_logbook  =  m_logbookTextEntry->GetDisplayText().Data();
  *m_username =  m_usernameTextEntry->GetDisplayText().Data();
  *m_system   =  m_systemTextEntry->GetDisplayText().Data();
  *m_subject  =  m_subjectTextEntry->GetDisplayText().Data();
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
  //okButton->Resize( xButtonSize, yButtonSize );
  elogFrame->AddFrame(okButton, layout );
  okButton->MoveResize( xBeg + xSize, yButtonPos, xButtonSize, yButtonSize );
  okButton->Connect("Clicked()", "ElogDialog", this, "ok()");

  TGTextButton*  cancelButton = new TGTextButton(elogFrame,"Cancel");
  cancelButton->SetTextJustify(36);
  //cancelButton->->Resize( xButtonSize, yButtonSize );
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
  m_logbookTextEntry = new TGTextEntry(elogFrame, new TGTextBuffer(15), -1);
  m_logbookTextEntry->SetMaxLength(255);
  m_logbookTextEntry->SetAlignment(kTextLeft);
  m_logbookTextEntry->SetText( (*m_logbook).c_str() );
  elogFrame->AddFrame( m_logbookTextEntry, layout );
  m_logbookTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 22);
  yBeg += yStep;

  TGLabel* m_subjectLabel = new TGLabel(elogFrame,"Subject (not for Shift): ");
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
