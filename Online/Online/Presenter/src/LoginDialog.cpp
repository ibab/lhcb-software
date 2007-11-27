#include <TSystem.h>
#include <TGComboBox.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <TSystem.h>

#include "OnlineHistDB/OnlineHistDB.h"

#include "PresenterMainFrame.h"
#include "LoginDialog.h"

ClassImp(LoginDialog)

LoginDialog::LoginDialog(PresenterMainFrame* gui, Int_t Width, Int_t Height) :
  TGTransientFrame(gClient->GetRoot(), gui, Width, Height),
  m_mainFrame(gui),
  m_dbPasswdInput(NULL),
  m_dbPasswd(NULL),
  m_histogramDB(NULL),
  m_dbUsername(NULL),
  m_loginButton(NULL),
  m_cancelButton(NULL)
{
  SetCleanup(kDeepCleanup);
  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputSystemModal);
  build();
  CenterOnParent();
  MapWindow();
}
LoginDialog::~LoginDialog()
{
  if (IsZombie()) return;
  Cleanup();
}
void LoginDialog::build()
{
  SetCleanup(kDeepCleanup);
  SetLayoutBroken(kTRUE);

  SetWindowName("Login to OnlineHistDB");

  m_histogramDB = new TGComboBox(this);
  //m_histogramDB = new TGComboBox(this, m_input);
  m_histogramDB->AddEntry(TString("lbora01:1528/HISTOGRAMDB"), 0);
  m_histogramDB->AddEntry(TString("ora01:1528/HISTOGRAMDB"), 1);
  m_histogramDB->AddEntry(TString("oradev10.cern.ch:10520/D10"), 2);
  m_histogramDB->AddEntry(TString("HISTOGRAMDB"), 3);
  m_histogramDB->Select(1);
  m_histogramDB->Resize(168, 22);
  AddFrame(m_histogramDB, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 2, 2,
    2, 2));
  m_histogramDB->Move(88, 16);

  m_dbUsername = new TGComboBox(this,-1, kHorizontalFrame | kSunkenFrame |
    kDoubleBorder | kOwnBackground);
  
  UserGroup_t* userInfo = gSystem->GetUserInfo(); //returns new...
  m_dbUsername->AddEntry(userInfo->fUser, 0);
  delete userInfo;
  
  m_dbUsername->AddEntry(TString("HIST_READER"), 1);
  m_dbUsername->AddEntry(TString("HIST_WRITER"), 2);
  m_dbUsername->AddEntry(TString("LHCB_MON_GIACOMO"), 3);
  m_dbUsername->AddEntry(TString(""), 4);
  m_dbUsername->Select(2);
  m_dbUsername->Resize(168, 22);
  AddFrame(m_dbUsername, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 2, 2, 2,
    2));
  m_dbUsername->MoveResize(88, 48, 168, 22);
  m_dbUsername->Connect("ChangedBy(const char *)", "LoginDialog", this,
    "defaultPw(const char *)");

  m_loginButton = new TGTextButton(this,"&Login");
  m_loginButton->SetTextJustify(36);
  m_loginButton->Resize(90, 22);
  AddFrame(m_loginButton, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2,
    2, 2));
  m_loginButton->MoveResize(64, 144, 90, 22);
  m_loginButton->Connect("Clicked()", "LoginDialog", this, "login()");

  m_cancelButton = new TGTextButton(this,"&Cancel");
  m_cancelButton->SetTextJustify(36);
  m_cancelButton->Resize(90, 22);
  AddFrame(m_cancelButton, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2,
    2, 2));
  m_cancelButton->MoveResize(168, 144, 90, 22);
  m_cancelButton->Connect("Clicked()", "LoginDialog", this, "cancelLogin()");

  m_dbPasswdInput = new TGTextEntry(this, m_dbPasswd = new TGTextBuffer(30),
    -1);
  m_dbPasswdInput->SetMaxLength(255);
  m_dbPasswdInput->SetAlignment(kTextLeft);
  m_dbPasswdInput->SetEchoMode(TGTextEntry::kPassword);
  m_dbPasswdInput->SetText("histeggia194");
  m_dbPasswdInput->Resize(168, m_dbPasswdInput->GetDefaultHeight());
  AddFrame(m_dbPasswdInput, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 2, 2,
    2, 2));
  m_dbPasswdInput->MoveResize(88, 80, 168, 22);

  TGLabel* fLabel747 = new TGLabel(this, "Database:");
  fLabel747->SetTextJustify(36);
  AddFrame(fLabel747, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));

  fLabel747->MoveResize(8, 16, 57, 18);
  TGLabel* fLabel754 = new TGLabel(this, "Username:");
  fLabel754->SetTextJustify(36);

  AddFrame(fLabel754, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  fLabel754->MoveResize(8, 48, 57, 18);
  TGLabel* fLabel761 = new TGLabel(this, "Password:");
  fLabel761->SetTextJustify(36);

  AddFrame(fLabel761, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  fLabel761->MoveResize(8, 80, 57, 18);

  Resize(GetDefaultSize());
  Resize(285, 208);

  MapSubwindows();
  MapWindow();
}
void LoginDialog::defaultPw(const char *)
{
  if(OnlineHistDBEnv_constants::ACCOUNT ==
       dynamic_cast<TGTextLBEntry*>(m_dbUsername->GetSelectedEntry())->
       GetText()->GetString()) {
    m_dbPasswdInput->SetText("histeggia194");
  } else {
    m_dbPasswdInput->SetText("histeggia194");
  }
}
void LoginDialog::login()
{
  m_loginButton->SetEnabled(false);
  if (m_mainFrame->connectToHistogramDB(m_dbPasswd->GetString(),
                              dynamic_cast<TGTextLBEntry*>(m_dbUsername->
                                GetSelectedEntry())->GetText()->GetString(),
                              dynamic_cast<TGTextLBEntry*>(m_histogramDB->
                                GetSelectedEntry())->GetText()->GetString())){
    DeleteWindow();
  } else {
      //TODO: remove DeleteWindow() ->  bug
      DeleteWindow();
//    m_loginButton->SetEnabled(true);
  }
}
void LoginDialog::cancelLogin()
{
  m_mainFrame->logoutFromHistogramDB();
  DeleteWindow();
}
