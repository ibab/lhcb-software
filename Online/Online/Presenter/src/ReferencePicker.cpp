#include "ReferencePicker.h"
#include <TGButton.h>
#include <TCanvas.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include <TRootEmbeddedCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TList.h>

#include <boost/filesystem.hpp>

#include "PresenterMainFrame.h"
#include "Archive.h"
#include "DbRootHist.h"

//class boost::mutex;

using namespace pres;
using namespace std;
using namespace boost::filesystem;
using namespace boost::posix_time;

ClassImp(ReferencePicker)

ReferencePicker::ReferencePicker(PresenterMainFrame* gui, DbRootHist* histogram) :
  TGTransientFrame(gClient->GetRoot(), gui),
  m_mainFrame(gui),
  m_histogram(histogram),
  m_archive(gui->archive()),
  m_verbosity(gui->verbosity())
{
  Connect("CloseWindow()", "ReferencePicker", this, "DontCallClose()");
  build();
  CenterOnParent();
  
  if (0 != m_histogram && 0 != m_archive) {
    path referenecePath(m_archive->referencePath() + s_slash +
                        m_histogram->taskName());
    if (exists(referenecePath)) {
      std::vector<path> foundRootFiles = m_archive->listRootFilesDirectory(referenecePath);
      if (!foundRootFiles.empty()) {
        if (m_verbosity >= Verbose) {
          std::cout << "Found " << foundRootFiles.size() <<
                       " reference file(s)" << std::endl;
        }
        std::vector<path>::const_iterator foundRootFilesIt;
        foundRootFilesIt = foundRootFiles.begin();
        TList* list = new TList;
        while (foundRootFilesIt != foundRootFiles.end()) {
//          TFile *f TFile::Open()...
          TFile rootFile((*foundRootFilesIt).file_string().c_str());
          if (rootFile.IsZombie()) {
            cout << "Error opening Root file: " <<
              (*foundRootFilesIt).file_string() << endl;
          } else {
            TH1* referenceHisto;
            rootFile.GetObject((m_histogram->onlineHistogram()->hname()).append(";1").c_str(),
                               referenceHisto);
            if (referenceHisto) {
              referenceHisto->SetName((*foundRootFilesIt).file_string().c_str());
              referenceHisto->SetTitle((*foundRootFilesIt).leaf().c_str());
              list->Add(referenceHisto);        
            }
          }
          ++foundRootFilesIt;
        }
  
        int numberOfRefs = list->GetSize(); 
        int rows = (int) ceil(sqrt((double)numberOfRefs));
        int columns(rows);
        c125->Clear();
        c125->Divide(rows,columns);

        TH1* histo;
        TIter next(list);
        while ((histo = (TH1 *) next())) {
          gPad = c125->GetPad(numberOfRefs);
          histo->DrawCopy();
          --numberOfRefs;
          list->Remove(histo);
          delete histo;
        }
        delete list;
        c125->Update();
      }
    }
  }
}
ReferencePicker::~ReferencePicker()
{
}
void ReferencePicker::setSelectedAsRef()
{
  DbRootHist* selectedDbHisto = m_mainFrame->selectedDbRootHistogram();
  if (0 != selectedDbHisto) {
    TH1* selectedHisto = selectedDbHisto->rootHistogram;
    try {
      if (0 != m_histogram->dbSession() &&
          0 != selectedDbHisto &&
          0 != m_histogram->onlineHistogram()->getTask() &&
          ReadWrite == m_mainFrame->databaseMode()) {
        std::string referenceEntry(selectedHisto->GetName());
        m_histogram->onlineHistogram()->getTask()->setReference(referenceEntry);
  // TODO: read Combobox
  
        std::string referenceOption(dynamic_cast<TGTextLBEntry*>(m_normalizationSelector->GetSelectedEntry())->
                                                    GetText()->GetString());
        m_histogram->setReferenceOption(referenceOption);
        m_histogram->saveTH1ToDB(0);      
        m_histogram->dbSession()->commit();
      }
    } catch (std::string sqlException) {
      // TODO: add error logging backend
      if (m_verbosity >= Verbose) {
        std::cout << "Could not set reference histogram: " <<
        selectedHisto->GetName() << std::endl <<
        sqlException << std::endl;
      }
    }
//    new TGMsgBox(fClient->GetRoot(), this, "Database Error",
//        Form("Could not set reference histogram:\n\n%s\n",
//             sqlException.c_str()),
//             kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);    
  }
  CloseWindow();
}
void ReferencePicker::build()
{
  // horizontal frame
  SetCleanup(kDeepCleanup);
  TGHorizontalFrame *fHorizontalFrame637 = new TGHorizontalFrame(this,
                                                                 618, 27,
                                                                 kHorizontalFrame);
  fHorizontalFrame637->SetLayoutBroken(kTRUE);

  TGLabel* fLabel747 = new TGLabel(fHorizontalFrame637, "Normalization:");
  fLabel747->SetTextJustify(36);
  fHorizontalFrame637->AddFrame(fLabel747, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsBottom,
                                                             2, 2, 2, 2));
  fLabel747->Move(220, 2);

  m_normalizationSelector = new TGComboBox(fHorizontalFrame637);
  m_normalizationSelector->AddEntry(s_Area.c_str(), 0);
  m_normalizationSelector->AddEntry(s_Entries.c_str(), 1);
  m_normalizationSelector->AddEntry(s_NoNormalization.c_str(), 2);
  m_normalizationSelector->AddEntry(s_NoReference.c_str(), 3);
  m_normalizationSelector->Select(0);
  m_normalizationSelector->Resize(80, 22);
  fHorizontalFrame637->AddFrame(m_normalizationSelector,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));
  m_normalizationSelector->Move(310, 2);
  
  m_setSelectedAsRefButton = new TGTextButton(fHorizontalFrame637,
                                              "Set selected as reference");
  m_setSelectedAsRefButton->SetTextJustify(36);
  m_setSelectedAsRefButton->SetMargins(0,0,0,0);
  m_setSelectedAsRefButton->SetWrapLength(-1);
  m_setSelectedAsRefButton->Resize(151,22);
  m_setSelectedAsRefButton->Connect("Clicked()", "ReferencePicker",
                                    this, "setSelectedAsRef()");  
  fHorizontalFrame637->AddFrame(m_setSelectedAsRefButton,
                                new TGLayoutHints(kLHintsRight |
                                                  kLHintsBottom,
                                                  2,2,2,2));
  m_setSelectedAsRefButton->Move(413,2);
  
  m_cancelButton = new TGTextButton(fHorizontalFrame637, "Cancel");
  m_cancelButton->SetTextJustify(36);
  m_cancelButton->SetMargins(0,0,0,0);
  m_cancelButton->SetWrapLength(-1);
  m_cancelButton->Resize(48,22);
  m_cancelButton->Connect("Clicked()", "ReferencePicker",
                          this, "CloseWindow()");
  fHorizontalFrame637->AddFrame(m_cancelButton,
                                new TGLayoutHints(kLHintsRight |
                                                  kLHintsBottom,
                                                  2,2,2,2));
  m_cancelButton->Move(568,2);

  AddFrame(fHorizontalFrame637, new TGLayoutHints(kLHintsCenterX |
                                                  kLHintsRight |
                                                  kLHintsBottom,
                                                  2,2,2,2));

  // embedded canvas
  fRootEmbeddedCanvas746 = new TRootEmbeddedCanvas(0, this, 399, 227);
  fRootEmbeddedCanvas746->SetCleanup(kDeepCleanup);
  c125 = fRootEmbeddedCanvas746->GetCanvas();
  AddFrame(fRootEmbeddedCanvas746, new TGLayoutHints(kLHintsLeft |
                                                     kLHintsTop |
                                                     kLHintsExpandX |
                                                     kLHintsExpandY,
                                                     2,2,2,2));

  SetMWMHints(kMWMDecorAll,
              kMWMFuncAll,
              kMWMInputModeless);
  MapSubwindows();

  Resize(GetDefaultSize());
  MapWindow();
  Resize(403,262);
}
void ReferencePicker::CloseWindow()
{
  DeleteWindow();
}
