#include <TSystem.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <TObject.h>
#include <TGMsgBox.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TGResourcePool.h>

#include <TPRegexp.h>
#include <TObjString.h>
#include <TObjArray.h>

#include <TH1.h>
#include <TH2.h>

#include "OnlineHistDB/OnlineHistDB.h"
#include "DbRootHist.h"

#include "PresenterMainFrame.h"
#include "PageSaveDialog.h"

using namespace pres;

ClassImp(PageSaveDialog)

PageSaveDialog::PageSaveDialog(PresenterMainFrame* gui, int width,
  int height, MsgLevel v) :
    TGTransientFrame(gClient->GetRoot(), gui, width, height),
    m_mainFrame(gui),
    m_verbosity(v),
    m_msgBoxReturnCode(0)
{
  SetCleanup(kDeepCleanup);
  Connect("CloseWindow()", "PageSaveDialog", this, "CloseWindow()");
  DontCallClose();
  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputSystemModal);
  build();
  MapWindow();
}
PageSaveDialog::~PageSaveDialog()
{
  Cleanup();
}
void PageSaveDialog::build()
{
  SetLayoutBroken(true);

  TGLabel *fLabel530 = new TGLabel(this,"Enter or select the parent folder:");
  fLabel530->SetTextJustify(36);
  AddFrame(fLabel530, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  fLabel530->MoveResize(16, 8, 192, 18);

  m_folderNameTextEntry = new TGTextEntry(this, new TGTextBuffer(15),-1,
                              kSunkenFrame | kDoubleBorder | kOwnBackground);
  m_folderNameTextEntry->SetMaxLength(255);
  m_folderNameTextEntry->SetAlignment(kTextLeft);
  m_folderNameTextEntry->Resize(456, m_folderNameTextEntry->GetDefaultHeight());
  AddFrame(m_folderNameTextEntry,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_folderNameTextEntry->MoveResize(16, 32, 456, 22);
  m_folderNameTextEntry->Connect("ReturnPressed()",
    "PageSaveDialog", this,
    "setOkButton()");

  // canvas widget
  TGCanvas *fCanvas694 = new TGCanvas(this, 456, 192);

  // canvas viewport
  TGViewPort *fViewPort695 = fCanvas694->GetViewPort();

  // list tree
  m_pageFolderListTree = new TGListTree(fCanvas694, kHorizontalFrame);
  m_pageFolderListTree->AddRoot("Pages");
  m_mainFrame->listFromHistogramDB(m_pageFolderListTree,
      FoldersAndPages, s_withoutHistograms);
  m_pageFolderListTree->Connect(
    "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", "PageSaveDialog", this,
    "updateTextFields(TGListTreeItem*, Int_t, Int_t, Int_t)");

  fViewPort695->AddFrame(m_pageFolderListTree);
  m_pageFolderListTree->SetLayoutManager(
    new TGHorizontalLayout(m_pageFolderListTree));
  m_pageFolderListTree->MapSubwindows();
  fCanvas694->SetContainer(m_pageFolderListTree);
  fCanvas694->MapSubwindows();
  AddFrame(fCanvas694,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  fCanvas694->MoveResize(16, 56, 456, 192);

  TGLabel *fLabel717 = new TGLabel(this,"Page name:");
  fLabel717->SetTextJustify(36);
  AddFrame(fLabel717,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  fLabel717->MoveResize(16, 256, 72, 18);

  m_pageNameTextEntry = new TGTextEntry(this, new TGTextBuffer(15),-1,
    kSunkenFrame | kDoubleBorder | kOwnBackground);
  m_pageNameTextEntry->SetMaxLength(255);
  m_pageNameTextEntry->SetAlignment(kTextLeft);
  m_pageNameTextEntry->Resize(376, m_pageNameTextEntry->GetDefaultHeight());
  AddFrame(m_pageNameTextEntry,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_pageNameTextEntry->MoveResize(96, 256, 376, 22);
  m_pageNameTextEntry->Connect("ReturnPressed()", "PageSaveDialog", this,
                             "setOkButton()");

  m_okButton = new TGTextButton(this,"OK");
  m_okButton->SetTextJustify(36);
  m_okButton->Resize(80, 24);
  m_okButton->SetState(kButtonDisabled);
  AddFrame(m_okButton,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_okButton->MoveResize(280, 288, 80, 24);
  m_okButton->Connect("Clicked()", "PageSaveDialog", this, "ok()");

  m_cancelButton = new TGTextButton(this,"Cancel");
  m_cancelButton->SetTextJustify(36);
  m_cancelButton->Resize(80, 24);
  AddFrame(m_cancelButton,
           new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_cancelButton->MoveResize(376, 288, 80, 24);
  m_cancelButton->Connect("Clicked()", "PageSaveDialog", this,
                          "CloseWindow()");

  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
  Resize(493, 339);
}
void PageSaveDialog::ok()
{
  m_okButton->SetState(kButtonDisabled);
  m_cancelButton->SetState(kButtonDisabled);

  if (m_mainFrame->isConnectedToHistogramDB()) {

    OnlineHistDB* m_histogramDB = m_mainFrame->histogramDB();
    std::string folderName = m_folderNameTextEntry->GetText();
    std::string pageName = m_pageNameTextEntry->GetText();

    try {
      OnlineHistPage* page = m_histogramDB->getPage(s_slash+folderName+s_slash+pageName);
      page->removeAllHistograms();
      double xlow, ylow, xup, yup;

      m_DbHistosOnPageIt = m_mainFrame->dbHistosOnPage.begin();
      while (m_DbHistosOnPageIt != m_mainFrame->dbHistosOnPage.end()) {
        if ((*m_DbHistosOnPageIt)->hostingPad) {
          ((*m_DbHistosOnPageIt)->hostingPad)->GetPadPar(xlow, ylow, xup, yup);
          OnlineHistogram* onlineHistogram = page->addHistogram((*m_DbHistosOnPageIt)->onlineHistogram(),
              (float)xlow , (float)ylow, (float)xup, (float)yup);
          if (onlineHistogram) {
            (*m_DbHistosOnPageIt)->setOnlineHistogram(onlineHistogram);
          }
        }
        m_DbHistosOnPageIt++;
      }

      if (page->save()) {
        // now save current ROOT display options of histograms on page
        m_DbHistosOnPageIt = m_mainFrame->dbHistosOnPage.begin();
        while (m_DbHistosOnPageIt != m_mainFrame->dbHistosOnPage.end()) {
          if((*m_DbHistosOnPageIt)->onlineHistogram()->page() == page->name()) {
// TODO: get rid of ->cd() because it stealthily diverts gPad
(*m_DbHistosOnPageIt)->hostingPad->cd();
            (*m_DbHistosOnPageIt)->saveTH1ToDB((*m_DbHistosOnPageIt)->hostingPad);
          }
          m_DbHistosOnPageIt++;
        }
      }
      m_histogramDB->commit();
    } catch (std::string Exc) {
      // TODO: add error logging backend - MsgStream?
      m_mainFrame->setStatusBarText(Exc.c_str(), 0);
      if (m_verbosity >= Verbose) { std::cout << Exc << std::endl; }

      new TGMsgBox(fClient->GetRoot(), this, "Database Error",
          Form("Could save the page to OnlineHistDB:\n\n%s\n",
            Exc.c_str()), kMBIconExclamation,
          kMBOk, &m_msgBoxReturnCode);
    }
    CloseWindow();
  }
}
void PageSaveDialog::updateTextFields(TGListTreeItem* node, int, int, int)
{
  if (node) {
    char path[1024];

    m_pageFolderListTree->GetPathnameFromItem(node, path);
    std::string dbPath = std::string(path);
    // Drop DB url
    dbPath = dbPath.erase(0, std::string(
                          m_pageFolderListTree->GetFirstItem()->GetText()).
                            length() + 2);

    // find pagename in path
    TObjArray* pathSlices = TPRegexp("^((?:(?:[^/]*)/)*)([^/]*)$").
                                      MatchS(dbPath.c_str());

    TString folderName = ((TObjString *)pathSlices->At(1))->GetString();
    folderName = folderName.Strip(TString::kTrailing,'/');
    TString pageName = ((TObjString *)pathSlices->At(2))->GetString();

    pathSlices->Delete();
    delete pathSlices;

    if (NULL != node->GetUserData()) {
       m_pageNameTextEntry->SetText(pageName);
       m_folderNameTextEntry->SetText(folderName);
    } else {
       m_folderNameTextEntry->SetText(dbPath.c_str());
    }

    setOkButton();
  }
}
void PageSaveDialog::setOkButton() {
  if (TString(m_folderNameTextEntry->GetText()).Sizeof() > 1  &&
      TString(m_pageNameTextEntry->GetText()).Sizeof() > 1 ) {
    m_okButton->SetEnabled(true);
  } else {
    m_okButton->SetEnabled(false);
  }
}
void PageSaveDialog::CloseWindow() {
  // disabling is a beauty patch for crashes on double-click crash
  m_okButton->SetState(kButtonDisabled);
  m_cancelButton->SetState(kButtonDisabled);
  DeleteWindow();
}
