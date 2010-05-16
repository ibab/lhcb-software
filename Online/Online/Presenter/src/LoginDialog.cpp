// $Id: LoginDialog.cpp,v 1.40 2010-05-16 18:09:40 robbep Exp $
// ROOT
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TSystem.h>
#include <TGComboBox.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <TSystem.h>
#ifdef WIN32
#pragma warning( pop )
#endif

// Online
#include "OnlineHistDB/OnlineHistDB.h"

// Local
#include "PresenterMainFrame.h"
#include "LoginDialog.h"

using namespace pres;

ClassImp(LoginDialog)

LoginDialog::LoginDialog(PresenterMainFrame* gui, int width, int height,
                         const DatabaseMode databaseMode,
                         const std::vector<std::string*> & dbs,
                         const std::map<std::string*, std::string*> & creds) :
  TGTransientFrame(gClient->GetRoot(), gui, width, height),
  m_mainFrame(gui),
  m_databaseMode(databaseMode),
  m_dbPasswdInput(NULL),
  m_dbPasswd(NULL),
  m_histogramDbSelector(NULL),
  m_dbUsername(NULL),
  m_loginButton(NULL),
  m_cancelButton(NULL),
  m_dbs(dbs),
  m_creds(creds)
{
  if (LoggedOut == m_databaseMode) { m_databaseMode = UserSelected; }
  SetCleanup(kDeepCleanup);
  Connect("CloseWindow()", "LoginDialog", this, "DontCallClose()");

  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputSystemModal);
  build();
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
  SetLayoutBroken(true);

  SetWindowName("Login to OnlineHistDB");

  m_histogramDbSelector = new TGComboBox(this);
  //m_histogramDbSelector = new TGComboBox(this, m_input);
  
  std::vector<std::string*>::iterator dbString;
  int counter(0);
  for (dbString = m_dbs.begin();dbString != m_dbs.end(); ++dbString) {
    m_histogramDbSelector->AddEntry((**dbString).c_str(), counter);
    counter++;
  }
  m_histogramDbSelector->Select(0);
  m_histogramDbSelector->Resize(168, 22);
  AddFrame(m_histogramDbSelector,
           new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,
                             2, 2, 2, 2));
  m_histogramDbSelector->Move(88, 16);

 
  m_dbUsername = new TGComboBox(this,-1, kHorizontalFrame | kSunkenFrame |
                                kDoubleBorder | kOwnBackground);

  counter = 0;
  std::map<std::string*, std::string*>::iterator dbCreds;
  for (dbCreds = m_creds.begin();dbCreds != m_creds.end(); ++dbCreds) {
    m_dbUsername->AddEntry((*(*dbCreds).first).c_str(), counter);
    counter++;    
  }
  //returns new... and Admin on w32, not euid(?) 
  UserGroup_t* userInfo = gSystem->GetUserInfo(gSystem->GetEffectiveUid()); 
  m_dbUsername->AddEntry(userInfo->fUser, counter);
  delete userInfo;
//    delete (*dbCreds).first;
//    delete (*dbCreds).second;

  m_dbUsername->Select(0);
  m_dbUsername->Resize(168, 22);
  AddFrame(m_dbUsername,
          new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,
                            2, 2, 2, 2));
  m_dbUsername->MoveResize(88, 48, 168, 22);
  m_dbUsername->Connect("Selected(Int_t)", "LoginDialog",
                        this, "defaultPw(Int_t)");

  m_loginButton = new TGTextButton(this,"&Login");
  m_loginButton->SetTextJustify(36);
  m_loginButton->Resize(90, 22);
  AddFrame(m_loginButton,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_loginButton->MoveResize(64, 144, 90, 22);
  m_loginButton->Connect("Clicked()", "LoginDialog", this, "login()");

  m_cancelButton = new TGTextButton(this,"&Cancel");
  m_cancelButton->SetTextJustify(36);
  m_cancelButton->Resize(90, 22);
  AddFrame(m_cancelButton,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_cancelButton->MoveResize(168, 144, 90, 22);
  m_cancelButton->Connect("Clicked()", "LoginDialog", this, "CloseWindow()");

  m_dbPasswdInput = new TGTextEntry(this,
                                    m_dbPasswd = new TGTextBuffer(30), -1);
  m_dbPasswdInput->SetMaxLength(255);
  m_dbPasswdInput->SetAlignment(kTextLeft);
  m_dbPasswdInput->SetEchoMode(TGTextEntry::kPassword);  
//  m_dbPasswdInput->SetText(s_histReaderPw.c_str());
  m_dbPasswdInput->Resize(168, m_dbPasswdInput->GetDefaultHeight());
  AddFrame(m_dbPasswdInput,
           new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,
                             2, 2, 2, 2));
  m_dbPasswdInput->MoveResize(88, 80, 168, 22);
//  m_creds.begin();
//  m_dbPasswdInput->SetText((*(*m_creds[0]).first).c_str());


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

   switch (m_databaseMode) {
    case ReadOnly:
//      m_dbUsername->Select(1);
//      m_dbPasswdInput->SetText(s_histReaderPw.c_str());
      break;
    case ReadWrite:
//      m_dbUsername->Select(2);
//      m_dbPasswdInput->SetText(s_histWriterPw.c_str());
      break;
    case UserSelected:
//      m_dbUsername->Select(0);
//      m_dbPasswdInput->SetText("");
      break;
    default:
      std::cout << "LoginDialog: something went wrong when setting database mode."
          << std::endl;
      break;
   }
   defaultPw(0);
//  Resize(GetDefaultSize());
  Resize(285, 208);

  MapSubwindows();
  MapWindow();
}
void LoginDialog::defaultPw(int)
{
  std::string selectedLogin(dynamic_cast<TGTextLBEntry*>(m_dbUsername->GetSelectedEntry())->GetText()->GetString());
 
  std::map<std::string*, std::string*>::iterator dbCreds;
  for (dbCreds = m_creds.begin();dbCreds != m_creds.end(); ++dbCreds) {
    if (selectedLogin == (*(*dbCreds).first)) {
//std::cout << "found login: " << (*(*dbCreds).first) << std::endl;
//std::cout << "login pw: " << (*(*dbCreds).second) << std::endl;
      
      m_dbPasswdInput->SetText(((*(*dbCreds).second)).c_str());
    }
  }
  
//  if(OnlineHistDBEnv_constants::ACCOUNT ==
//       dynamic_cast<TGTextLBEntry*>(m_dbUsername->GetSelectedEntry())->
//                                    GetText()->GetString()) {
////    m_dbPasswdInput->SetText(s_histReaderPw.c_str());
//  } else if (account == 0){
//    m_dbPasswdInput->SetText("");    
//  } else if (s_histWriter ==
//       dynamic_cast<TGTextLBEntry*>(m_dbUsername->GetSelectedEntry())->
//                                    GetText()->GetString()) {
//    m_dbPasswdInput->SetText(s_histWriterPw.c_str());
//  }
  DoRedraw();
}
void LoginDialog::login()
{
  m_loginButton->SetState(kButtonDisabled);
  m_cancelButton->SetState(kButtonDisabled);

  if (m_mainFrame->connectToHistogramDB(m_dbPasswd->GetString(),
            dynamic_cast<TGTextLBEntry*>(m_dbUsername->GetSelectedEntry())->
                                         GetText()->GetString(),
            dynamic_cast<TGTextLBEntry*>(m_histogramDbSelector->GetSelectedEntry())->
                                         GetText()->GetString())){
    CloseWindow();
  } else {
    CloseWindow();
  }
}
void LoginDialog::CloseWindow() {
  m_loginButton->SetState(kButtonDisabled);
  m_cancelButton->SetState(kButtonDisabled);
  DeleteWindow();
}
