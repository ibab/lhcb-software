// $Id: $
// Include files 
//=============================================================================
// STL
#include <iostream>
#include <cstdlib>

// ROOT
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

// Local
#include "ReferenceDialog.h"
#include "PresenterMainFrame.h"

using namespace pres;

ClassImp(ReferenceDialog)

ReferenceDialog::ReferenceDialog(PresenterMainFrame* gui, int width, int height) :
      TGTransientFrame(gClient->GetRoot(), gui, width, height),
      m_mainFrame(gui) {
  SetCleanup(kDeepCleanup);
  Connect("CloseWindow()", "ReferenceDialog", this, "DontCallClose()");
  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputSystemModal);
}

ReferenceDialog::~ReferenceDialog() {
  Cleanup();
}
void ReferenceDialog::setParameters( std::string& tck, int& refRun ) {
  m_tck    = &tck;
  m_refRun = &refRun;
  build();
  MapWindow();
}

void ReferenceDialog::ok() {
  *m_tck    = m_tckTextEntry -> GetDisplayText().Data();
  *m_refRun = atoi( m_refRunTextEntry->GetDisplayText().Data() );
  CloseWindow();
}
void ReferenceDialog::build() {

  // main frame
  SetLayoutBroken(true);

  // composite frame
  TGCompositeFrame *referenceFrame = new TGCompositeFrame(this, 646, 435,  kVerticalFrame);
  referenceFrame->SetLayoutBroken(true);

  TGLayoutHints* layout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2);

  int xBeg = 20;
  int yBeg = 25;
  int xSize = 150;
  int xInputSize = 400;
  int yStep = 30;

  int xButtonSize = 92;
  int yButtonSize = 24;

  TGLabel* m_tckLabel = new TGLabel(referenceFrame,"TCK value    : ");
  m_tckLabel->SetTextJustify(kTextRight);
  referenceFrame->AddFrame(m_tckLabel, layout );
  m_tckLabel->MoveResize( xBeg, yBeg, xSize, 18);
  m_tckTextEntry = new TGTextEntry(referenceFrame, new TGTextBuffer(15), -1);
  m_tckTextEntry->SetMaxLength(255);
  m_tckTextEntry->SetAlignment(kTextLeft);
  m_tckTextEntry->SetText( (*m_tck).c_str() );
  referenceFrame->AddFrame(m_tckTextEntry, layout );
  m_tckTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 22);
  yBeg += yStep;

  TGLabel* m_refRunLabel = new TGLabel(referenceFrame,"ReferenceRun : ");
  m_refRunLabel->SetTextJustify(kTextRight);
  referenceFrame->AddFrame(m_refRunLabel, layout );
  m_refRunLabel->MoveResize( xBeg, yBeg, xSize, 18);
  m_refRunTextEntry = new TGTextEntry(referenceFrame, new TGTextBuffer(15), -1);
  m_refRunTextEntry->SetMaxLength(255);
  m_refRunTextEntry->SetAlignment(kTextLeft);
  char txt[30];
  sprintf( txt, "%d", *m_refRun );
  m_refRunTextEntry->SetText( txt );
  referenceFrame->AddFrame(m_refRunTextEntry, layout );
  m_refRunTextEntry->MoveResize( xBeg + xSize, yBeg, xInputSize, 22);
  yBeg += yStep;

  TGTextButton* okButton = new TGTextButton(referenceFrame,"Accept");
  okButton->SetTextJustify(kTextCenterX);
  referenceFrame->AddFrame(okButton, layout );
  okButton->MoveResize( xBeg + xSize, yBeg, xButtonSize, yButtonSize );
  okButton->Connect("Clicked()", "ReferenceDialog", this, "ok()");

  TGTextButton*  cancelButton = new TGTextButton(referenceFrame,"Cancel");
  cancelButton->SetTextJustify(kTextCenterX);
  referenceFrame->AddFrame(cancelButton, layout );
  cancelButton->MoveResize( xBeg+xSize+xSize, yBeg, xButtonSize, yButtonSize );
  cancelButton->Connect("Clicked()", "ReferenceDialog", this, "CloseWindow()");
  yBeg += yButtonSize + 20;

  AddFrame(referenceFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  referenceFrame->MoveResize(0, 0, 646, yBeg);
  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
  Resize(658, yBeg+12 );
}
void ReferenceDialog::CloseWindow() {
  DeleteWindow();
}
