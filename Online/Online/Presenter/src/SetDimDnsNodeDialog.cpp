#include <TSystem.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <TObject.h>
#include <TGMsgBox.h>

#include "PresenterMainFrame.h"
#include "SetDimDnsNodeDialog.h"

ClassImp(SetDimDnsNodeDialog)

SetDimDnsNodeDialog::SetDimDnsNodeDialog(PresenterMainFrame* gui, Int_t Width,
  Int_t Height, MsgLevel v) :
    TGTransientFrame(gClient->GetRoot(), gui, Width, Height),
    m_mainFrame(gui),
    m_verbosity(v),
    m_msgBoxReturnCode(0)
{
  SetCleanup(kDeepCleanup);
  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputSystemModal);
  build();
  CenterOnParent();
  MapWindow();
}

SetDimDnsNodeDialog::~SetDimDnsNodeDialog()
{
  Cleanup();
}

void SetDimDnsNodeDialog::build()
{
  SetLayoutBroken(kTRUE);
  
  m_okButton = new TGTextButton(this, "OK");
  m_okButton->SetTextJustify(36);
  m_okButton->SetMargins(0,0,0,0);
  m_okButton->SetWrapLength(-1);
  m_okButton->Resize(92,24);
  AddFrame(m_okButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  m_okButton->MoveResize(88,88,92,24);
  m_okButton->Connect("Clicked()", "SetDimDnsNodeDialog", this, "ok()");
  
  m_cancelButton = new TGTextButton(this, "Cancel");
  m_cancelButton->SetTextJustify(36);
  m_cancelButton->SetMargins(0,0,0,0);
  m_cancelButton->SetWrapLength(-1);
  m_cancelButton->Resize(92,24);
  AddFrame(m_cancelButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  m_cancelButton->MoveResize(192,88,92,24);
  m_cancelButton->Connect("Clicked()", "SetDimDnsNodeDialog", this,
                          "DeleteWindow()");  

  TGLabel* fLabel541 = new TGLabel(this, "DIM DNS node:");
  fLabel541->SetTextJustify(36);
  fLabel541->SetMargins(0,0,0,0);
  fLabel541->SetWrapLength(-1);
  AddFrame(fLabel541, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fLabel541->MoveResize(16,8,112,18);
    
  // combo box
  TGComboBox *fComboBox521 = new TGComboBox(this,-1,kHorizontalFrame |
                                            kSunkenFrame | kDoubleBorder |
                                            kOwnBackground);
  fComboBox521->AddEntry("Entry 1 ",0);
  fComboBox521->AddEntry("Entry 2 ",1);
  fComboBox521->AddEntry("Entry 3 ",2);
  fComboBox521->AddEntry("Entry 4 ",3);
  fComboBox521->AddEntry("Entry 5 ",4);
  fComboBox521->AddEntry("Entry 6 ",5);
  fComboBox521->AddEntry("Entry 7 ",6);
  fComboBox521->Resize(256,22);
  fComboBox521->Select(1);
  AddFrame(fComboBox521, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fComboBox521->MoveResize(32,40,256,22);

  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);  
  MapSubwindows();

  Resize(GetDefaultSize());
  MapWindow();
  Resize(320,150);
}
void SetDimDnsNodeDialog::ok()
{
  DeleteWindow();
}
