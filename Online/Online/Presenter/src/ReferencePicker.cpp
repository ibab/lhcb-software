#include "ReferencePicker.h"
#include <TGButton.h>
#include <TCanvas.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include <TRootEmbeddedCanvas.h>

#include "PresenterMainFrame.h"
#include "DbRootHist.h"

using namespace pres;

ClassImp(ReferencePicker)

ReferencePicker::ReferencePicker(PresenterMainFrame* gui) :
  TGTransientFrame(gClient->GetRoot(), gui),
  m_mainFrame(gui),
  m_histogram(gui->selectedDbRootHistogram()),
  m_archive(gui->archive()),
  m_verbosity(gui->verbosity())
{
  build();
  CenterOnParent();
  if (0 != m_histogram) {
    std::cout << m_histogram->histoRootName() << std::endl;
  }  
}
ReferencePicker::~ReferencePicker()
{
}
void ReferencePicker::ok()
{
}
void ReferencePicker::build()
{
  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame637 = new TGHorizontalFrame(this,
                                                                 618,27,
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
  
  TGTextButton *fTextButton669 = new TGTextButton(fHorizontalFrame637,
                                                  "Set selected as reference");
  fTextButton669->SetTextJustify(36);
  fTextButton669->SetMargins(0,0,0,0);
  fTextButton669->SetWrapLength(-1);
  fTextButton669->Resize(151,22);
  fHorizontalFrame637->AddFrame(fTextButton669, new TGLayoutHints(kLHintsRight |
                                                                  kLHintsBottom,
                                                                  2,2,2,2));
  fTextButton669->Move(413,2);
  
  TGTextButton *fTextButton658 = new TGTextButton(fHorizontalFrame637,
                                                  "Cancel");
  fTextButton658->SetTextJustify(36);
  fTextButton658->SetMargins(0,0,0,0);
  fTextButton658->SetWrapLength(-1);
  fTextButton658->Resize(48,22);
  fHorizontalFrame637->AddFrame(fTextButton658,
                                new TGLayoutHints(kLHintsRight |
                                                  kLHintsBottom,
                                                  2,2,2,2));
  fTextButton658->Move(568,2);

  AddFrame(fHorizontalFrame637, new TGLayoutHints(kLHintsCenterX |
                                                  kLHintsRight |
                                                  kLHintsBottom,
                                                  2,2,2,2));

  // embedded canvas
  TRootEmbeddedCanvas *fRootEmbeddedCanvas746 = new TRootEmbeddedCanvas(0,
                                                                        this,
                                                                        399,
                                                                        227);
  Int_t wfRootEmbeddedCanvas746 = fRootEmbeddedCanvas746->GetCanvasWindowId();
  TCanvas *c125 = new TCanvas("c125", 10, 10, wfRootEmbeddedCanvas746);
  fRootEmbeddedCanvas746->AdoptCanvas(c125);
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
}
