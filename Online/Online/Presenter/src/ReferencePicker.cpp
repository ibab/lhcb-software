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
}
ReferencePicker::~ReferencePicker()
{
}
void ReferencePicker::setSelectedAsRef()
{
  if (0 != m_histogram) {
    std::cout << m_histogram->histoRootName() << std::endl;
  }
  if (0 != m_archive) {
    std::cout << m_archive->referencePath() << std::endl;
  }

  path referenecePath(m_archive->referencePath() + s_slash +
                       m_histogram->taskName());
  if (exists(referenecePath)) {
    std::vector<path> foundRootFiles = m_archive->listDirectory(referenecePath);
    if (!foundRootFiles.empty()) {
      if (m_verbosity >= Verbose) {
        std::cout << "Found " << foundRootFiles.size() <<
                     " reference file(s)" << std::endl;
      }
      std::vector<path>::const_iterator foundRootFilesIt;
      foundRootFilesIt = foundRootFiles.begin();
      TList* list = new TList;
      while (foundRootFilesIt != foundRootFiles.end()) {
        TFile rootFile((*foundRootFilesIt).file_string().c_str());
        if (rootFile.IsZombie()) {
          cout << "Error opening Root file: " <<
            (*foundRootFilesIt).file_string() << endl;
        } else {
          TH1* referenceHisto;
          rootFile.GetObject((m_histogram->onlineHistogram()->hname()).c_str(),
                             referenceHisto);
          if (referenceHisto) {
            referenceHisto->SetName((*foundRootFilesIt).file_string().c_str());
            list->Add(referenceHisto);            
          }
        }
        ++foundRootFilesIt;
      }

      int numberOfRefs = list->GetSize(); 
      int rows = (int) ceil(sqrt((double)numberOfRefs));
      int columns(rows);
      c125->Divide(rows,columns);
      
      TH1* histo;
      TIter next(list);      
      while (histo = (TH1 *) next()) {
         std::cout << histo->GetName() << std::endl;
        list->Remove(histo);
        delete histo;
      }
      delete list;
    }
  }
//  ../test/x/  
//  default-1.root  default-2.root  default-4.root  tttea-4.root

  

  
//  try {
//    if (0 != m_histogramDB) {
//      OnlineHistTask* currentTask = m_histogramDB->getTask("EXAMPLE");
//      if (0 != currentTask) {
//        std::cout << "ref: "   << currentTask->reference() << std::endl;
//        std::cout << "refRoot: " << currentTask->refRoot() << std::endl;
//      }
//    }
//  } catch (std::string sqlException) {
//    // TODO: add error logging backend
//    if (m_verbosity >= Verbose) { std::cout << sqlException; }
//
//    new TGMsgBox(fClient->GetRoot(), this, "Database Error",
//        Form("OnlineHistDB server:\n\n%s\n",
//             sqlException.c_str()),
//             kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
//  }
  
  CloseWindow();
}
void ReferencePicker::build()
{
  // horizontal frame
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

  TGComboBox* m_normalizationSelector = new TGComboBox(fHorizontalFrame637);
  m_normalizationSelector->AddEntry(s_refOptionArea, 0);
  m_normalizationSelector->AddEntry(s_refOptionEntries, 1);
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
  fHorizontalFrame637->AddFrame(m_setSelectedAsRefButton, new TGLayoutHints(kLHintsRight |
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
  Int_t wfRootEmbeddedCanvas746 = fRootEmbeddedCanvas746->GetCanvasWindowId();
  c125 = new TCanvas("c125", 10, 10, wfRootEmbeddedCanvas746);
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
  if (0 != c125) {delete c125;}
  if (0 != fRootEmbeddedCanvas746) {delete fRootEmbeddedCanvas746;}
  DeleteWindow();
}
