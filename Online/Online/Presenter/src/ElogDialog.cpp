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
                                std::string& system, std::string& message, int& isOK ) {
  m_logbook  = &logbook;
  m_username = &username;
  m_system   = &system;
  m_message  = &message;
  m_isOK     = &isOK;
  
  build();
  MapWindow();
}

void ElogDialog::ok() {

  *m_logbook  =  m_logbookTextEntry->GetDisplayText().Data();
  *m_username =  m_usernameTextEntry->GetDisplayText().Data();
  *m_system   =  m_systemTextEntry->GetDisplayText().Data();
  //*m_message  =  m_messageTextEntry->GetDisplayText().Data();
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

  TGTextButton* okButton = new TGTextButton(elogFrame,"OK");
  okButton->SetTextJustify(36);
  okButton->Resize(92, 24);
  elogFrame->AddFrame(okButton, layout );
  okButton->MoveResize(432, 392, 92, 24);
  okButton->Connect("Clicked()", "ElogDialog", this, "ok()");

  TGTextButton*  cancelButton = new TGTextButton(elogFrame,"Cancel");
  cancelButton->SetTextJustify(36);
  cancelButton->Resize(92, 24);
  elogFrame->AddFrame(cancelButton, layout );
  cancelButton->MoveResize(536, 392, 92, 24);
  cancelButton->Connect("Clicked()", "ElogDialog", this, "CloseWindow()");


  int xBeg = 20;
  int yBeg = 25;
  int xSize = 100;
  int xInputSize = 400;
  int yStep = 30;
  
  TGLabel* m_logbookLabel = new TGLabel(elogFrame,"Logbook name : ");
  m_logbookLabel->SetTextJustify(36);
  elogFrame->AddFrame( m_logbookLabel, layout );
  m_logbookLabel->MoveResize( xBeg, yBeg, xSize, 20);
  m_logbookTextEntry = new TGTextEntry(elogFrame, new TGTextBuffer(15), -1);
  m_logbookTextEntry->SetMaxLength(255);
  m_logbookTextEntry->SetAlignment(kTextLeft);
  m_logbookTextEntry->SetText( (*m_logbook).c_str() );
  //m_logbookTextEntry->Resize( 300, m_logbookTextEntry->GetDefaultHeight());
  elogFrame->AddFrame( m_logbookTextEntry, layout );
  m_logbookTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 22);
  yBeg += yStep;

  TGLabel* m_usernameLabel = new TGLabel(elogFrame,"Username name : ");
  m_usernameLabel->SetTextJustify(36);
  elogFrame->AddFrame(m_usernameLabel, layout );
  m_usernameLabel->MoveResize( xBeg, yBeg, xSize, 18);
  m_usernameTextEntry = new TGTextEntry(elogFrame, new TGTextBuffer(15), -1);
  m_usernameTextEntry->SetMaxLength(255);
  m_usernameTextEntry->SetAlignment(kTextLeft);
  m_usernameTextEntry->SetText( (*m_username).c_str() );
  //m_usernameTextEntry->Resize(300, m_usernameTextEntry->GetDefaultHeight());
  elogFrame->AddFrame(m_usernameTextEntry, layout );
  m_usernameTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 22);
  yBeg += yStep;


  TGLabel* m_systemLabel = new TGLabel(elogFrame,"System name : ");
  m_systemLabel->SetTextJustify(36);
  elogFrame->AddFrame(m_systemLabel, layout );
  m_systemLabel->MoveResize( xBeg, yBeg, xSize, 18);
  m_systemTextEntry = new TGTextEntry(elogFrame, new TGTextBuffer(15), -1);
  m_systemTextEntry->SetMaxLength(255);
  m_systemTextEntry->SetAlignment(kTextLeft);
  m_systemTextEntry->SetText( (*m_system).c_str() );
  //m_systemTextEntry->Resize(400, m_systemTextEntry->GetDefaultHeight());
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
