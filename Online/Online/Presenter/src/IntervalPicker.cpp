#include "IntervalPicker.h"

#include <TGButton.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGNumberEntry.h>

#include "PresenterMainFrame.h"
#include "Archive.h"

using namespace pres;
using namespace std;

ClassImp(IntervalPicker)

IntervalPicker::IntervalPicker(PresenterMainFrame* gui) :
  TGTransientFrame(gClient->GetRoot(), gui),
  m_mainFrame(gui),
  m_archive(gui->archive()),
  m_verbosity(gui->verbosity())
{
  Connect("CloseWindow()", "IntervalPicker", this, "DontCallClose()");
  SetCleanup(kDeepCleanup);
  build();
  CenterOnParent();
}
IntervalPicker::~IntervalPicker()
{
}
void IntervalPicker::build()
{
  TGHorizontalFrame *fHorizontalFrame655 = new TGHorizontalFrame(this, 80, 26,
                                                                 kHorizontalFrame);
  TGTextButton *fTextButton656 = new TGTextButton(fHorizontalFrame655, "Ok");
  fTextButton656->SetTextJustify(36);
  fTextButton656->Resize(24, 22);
  fHorizontalFrame655->AddFrame(fTextButton656,
                                new TGLayoutHints(kLHintsRight |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));
  m_cancelButton = new TGTextButton(fHorizontalFrame655, "Cancel");
  m_cancelButton->SetTextJustify(36);
  m_cancelButton->Resize(48, 22);
  m_cancelButton->Connect("Clicked()", "ReferencePicker",
                          this, "CloseWindow()");
  fHorizontalFrame655->AddFrame(m_cancelButton,
                                new TGLayoutHints(kLHintsRight |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));

  AddFrame(fHorizontalFrame655, new TGLayoutHints(kLHintsRight |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));

  TGVerticalFrame *fVerticalFrame658 = new TGVerticalFrame(this, 392, 358,
                                                           kVerticalFrame);
  TGLabel *fLabel659 = new TGLabel(fVerticalFrame658,
                                   "Please select interval for History mode:");
  fLabel659->SetTextJustify(36);
  fVerticalFrame658->AddFrame(fLabel659, new TGLayoutHints(kLHintsLeft |
                                                           kLHintsTop,
                                                           2, 2, 2, 2));

  // tab widget
  TGTab *fTab660 = new TGTab(fVerticalFrame658,392,338);

  // container of "Tab1"
  TGCompositeFrame *fCompositeFrame663;
  fCompositeFrame663 = fTab660->AddTab("Time");
  fCompositeFrame663->SetLayoutManager(new TGVerticalLayout(fCompositeFrame663));
  fCompositeFrame663->SetLayoutBroken(kTRUE);

  TGVerticalFrame *fVerticalFrame664 = new TGVerticalFrame(fCompositeFrame663,
                                                           372, 208,
                                                           kVerticalFrame);

  TGHorizontalFrame *fHorizontalFrame665 = new TGHorizontalFrame(fVerticalFrame664,
                                                                 197, 30,
                                                                 kHorizontalFrame);
  TGRadioButton *fRadioButton666 = new TGRadioButton(fHorizontalFrame665,
                                                     "Last");
  fRadioButton666->SetState(kButtonDown);
  fHorizontalFrame665->AddFrame(fRadioButton666,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame667 = new TGHorizontalFrame(fHorizontalFrame665,
                                                                 144, 26,
                                                                 kHorizontalFrame);
  fHorizontalFrame667->SetLayoutBroken(kTRUE);
  TGNumberEntry *fNumberEntry668 = new TGNumberEntry(fHorizontalFrame667,
                                                     (Double_t) 0,6,-1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame667->AddFrame(fNumberEntry668,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));
  fNumberEntry668->MoveResize(2, 2, 64, 22);
  TGLabel *fLabel672 = new TGLabel(fHorizontalFrame667, "minute(s)");
  fLabel672->SetTextJustify(36);
  fHorizontalFrame667->AddFrame(fLabel672, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  fLabel672->MoveResize(70, 5, 50, 16);

  fHorizontalFrame665->AddFrame(fHorizontalFrame667,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsCenterX |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));

  fVerticalFrame664->AddFrame(fHorizontalFrame665,
                              new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                                2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame673 = new TGHorizontalFrame(fVerticalFrame664,
                                                                 197, 30,
                                                                 kHorizontalFrame);
  TGRadioButton *fRadioButton674 = new TGRadioButton(fHorizontalFrame673,
                                                     "Last");
  fRadioButton674->SetState(kButtonDown);
  fHorizontalFrame673->AddFrame(fRadioButton674,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame675 = new TGHorizontalFrame(fHorizontalFrame673,
                                                                 144, 26,
                                                                 kHorizontalFrame);
  fHorizontalFrame675->SetLayoutBroken(kTRUE);
  TGNumberEntry *fNumberEntry676 = new TGNumberEntry(fHorizontalFrame675,
                                                     (Double_t) 0, 6, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame675->AddFrame(fNumberEntry676,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));
  fNumberEntry676->MoveResize(2, 2, 64, 22);
  TGLabel *fLabel680 = new TGLabel(fHorizontalFrame675, "hour(s)");
  fLabel680->SetTextJustify(36);
  fHorizontalFrame675->AddFrame(fLabel680, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  fLabel680->MoveResize(70, 5, 39, 16);

  fHorizontalFrame673->AddFrame(fHorizontalFrame675,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsCenterX |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));

  fVerticalFrame664->AddFrame(fHorizontalFrame673,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop,
                                                2, 2, 2, 2));

  TGVerticalFrame *fVerticalFrame681 = new TGVerticalFrame(fVerticalFrame664,
                                                           368,136,
                                                           kVerticalFrame);

  TGHorizontalFrame *fHorizontalFrame682 = new TGHorizontalFrame(fVerticalFrame681,
                                                                 363, 64,
                                                                 kHorizontalFrame);
  TGRadioButton *fRadioButton683 = new TGRadioButton(fHorizontalFrame682,
                                                     "From");
  fHorizontalFrame682->AddFrame(fRadioButton683,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));

  TGVerticalFrame *fVerticalFrame684 = new TGVerticalFrame(fHorizontalFrame682,
                                                           305, 60,
                                                           kVerticalFrame);

  TGHorizontalFrame *fHorizontalFrame685 = new TGHorizontalFrame(fVerticalFrame684,
                                                                 301, 26,
                                                                 kHorizontalFrame);
  TGLabel *fLabel686 = new TGLabel(fHorizontalFrame685, "Time:");
  fLabel686->SetTextJustify(36);
  fHorizontalFrame685->AddFrame(fLabel686, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  TGNumberEntry *fNumberEntry687 = new TGNumberEntry(fHorizontalFrame685,
                                                     (Double_t) 0, 6, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame685->AddFrame(fNumberEntry687,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));
  TGLabel *fLabel691 = new TGLabel(fHorizontalFrame685, ":");
  fLabel691->SetTextJustify(36);
  fHorizontalFrame685->AddFrame(fLabel691, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  TGNumberEntry *fNumberEntry692 = new TGNumberEntry(fHorizontalFrame685,
                                                     (Double_t) 0, 6, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame685->AddFrame(fNumberEntry692,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));
  TGLabel *fLabel696 = new TGLabel(fHorizontalFrame685, ":");
  fLabel696->SetTextJustify(36);
  fHorizontalFrame685->AddFrame(fLabel696, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  TGNumberEntry *fNumberEntry697 = new TGNumberEntry(fHorizontalFrame685,
                                                     (Double_t) 0, 6, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame685->AddFrame(fNumberEntry697,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  fVerticalFrame684->AddFrame(fHorizontalFrame685,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop |
                                                kLHintsExpandX,
                                                2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame701 = new TGHorizontalFrame(fVerticalFrame684,
                                                                 301, 26,
                                                                 kHorizontalFrame);
  TGLabel *fLabel702 = new TGLabel(fHorizontalFrame701, "Date:");
  fLabel702->SetTextJustify(36);
  fHorizontalFrame701->AddFrame(fLabel702, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  TGNumberEntry *fNumberEntry703 = new TGNumberEntry(fHorizontalFrame701,
                                                     (Double_t) 0, 6, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame701->AddFrame(fNumberEntry703, new TGLayoutHints(kLHintsLeft |
                                                                   kLHintsTop |
                                                                   kLHintsCenterY,
                                                                   2, 2, 2, 2));

  // combo box
  TGComboBox *fComboBox707 = new TGComboBox(fHorizontalFrame701, -1,
                                            kHorizontalFrame |
                                            kSunkenFrame |
                                            kDoubleBorder |
                                            kOwnBackground);
  fComboBox707->Resize(128, 22);
  fComboBox707->Select(-1);
  fHorizontalFrame701->AddFrame(fComboBox707, new TGLayoutHints(kLHintsLeft |
                                                                kLHintsTop |
                                                                kLHintsCenterY,
                                                                2, 2, 2, 2));
  TGNumberEntry *fNumberEntry718 = new TGNumberEntry(fHorizontalFrame701,
                                                     (Double_t) 0, 6, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame701->AddFrame(fNumberEntry718,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  fVerticalFrame684->AddFrame(fHorizontalFrame701,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop |
                                                kLHintsExpandX,
                                                2, 2, 2, 2));

  fHorizontalFrame682->AddFrame(fVerticalFrame684,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));

  fVerticalFrame681->AddFrame(fHorizontalFrame682,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop,
                                                2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame722 = new TGHorizontalFrame(fVerticalFrame681,
                                                                 364, 64,
                                                                 kHorizontalFrame);

  TGHorizontalFrame *fHorizontalFrame723 = new TGHorizontalFrame(fHorizontalFrame722,
                                                                 51, 26,
                                                                 kHorizontalFrame);
  TGLabel *fLabel724 = new TGLabel(fHorizontalFrame723, "to");
  fLabel724->SetTextJustify(36);
  fHorizontalFrame723->AddFrame(fLabel724, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  TGTextButton *fTextButton725 = new TGTextButton(fHorizontalFrame723, "Now");
  fTextButton725->SetTextJustify(36);
  fTextButton725->Resize(33, 22);
  fHorizontalFrame723->AddFrame(fTextButton725,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsCenterX |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));

  fHorizontalFrame722->AddFrame(fHorizontalFrame723,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));

  TGVerticalFrame *fVerticalFrame726 = new TGVerticalFrame(fHorizontalFrame722,
                                                           305,60,
                                                           kVerticalFrame);

  TGHorizontalFrame *fHorizontalFrame727 = new TGHorizontalFrame(fVerticalFrame726,
                                                                 301, 26,
                                                                 kHorizontalFrame);
  TGLabel *fLabel728 = new TGLabel(fHorizontalFrame727, "Time:");
  fLabel728->SetTextJustify(36);
  fHorizontalFrame727->AddFrame(fLabel728, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  TGNumberEntry *fNumberEntry729 = new TGNumberEntry(fHorizontalFrame727,
                                                     (Double_t) 0, 6, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame727->AddFrame(fNumberEntry729,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));
  TGLabel *fLabel733 = new TGLabel(fHorizontalFrame727, ":");
  fLabel733->SetTextJustify(36);
  fHorizontalFrame727->AddFrame(fLabel733, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  TGNumberEntry *fNumberEntry734 = new TGNumberEntry(fHorizontalFrame727,
                                                     (Double_t) 0,6,-1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame727->AddFrame(fNumberEntry734,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));
  TGLabel *fLabel738 = new TGLabel(fHorizontalFrame727, ":");
  fLabel738->SetTextJustify(36);
  fHorizontalFrame727->AddFrame(fLabel738, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  TGNumberEntry *fNumberEntry739 = new TGNumberEntry(fHorizontalFrame727,
                                                     (Double_t) 0, 6, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame727->AddFrame(fNumberEntry739,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  fVerticalFrame726->AddFrame(fHorizontalFrame727,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop |
                                                kLHintsExpandX,
                                                2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame743 = new TGHorizontalFrame(fVerticalFrame726,
                                                                 301, 26,
                                                                 kHorizontalFrame);
  TGLabel *fLabel744 = new TGLabel(fHorizontalFrame743, "Date:");
  fLabel744->SetTextJustify(36);
  fHorizontalFrame743->AddFrame(fLabel744, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));
  TGNumberEntry *fNumberEntry745 = new TGNumberEntry(fHorizontalFrame743,
                                                     (Double_t) 0, 6, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame743->AddFrame(fNumberEntry745,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  // combo box
  TGComboBox *fComboBox749 = new TGComboBox(fHorizontalFrame743, -1,
                                            kHorizontalFrame |
                                            kSunkenFrame |
                                            kDoubleBorder |
                                            kOwnBackground);
  fComboBox749->Resize(128, 22);
  fComboBox749->Select(-1);
  fHorizontalFrame743->AddFrame(fComboBox749,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));
  TGNumberEntry *fNumberEntry760 = new TGNumberEntry(fHorizontalFrame743,
                                                     (Double_t) 0, 6, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame743->AddFrame(fNumberEntry760,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  fVerticalFrame726->AddFrame(fHorizontalFrame743,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop |
                                                kLHintsExpandX,
                                                2, 2, 2, 2));

  fHorizontalFrame722->AddFrame(fVerticalFrame726,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));

  fVerticalFrame681->AddFrame(fHorizontalFrame722,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop,
                                                2, 2, 2, 2));

  fVerticalFrame664->AddFrame(fVerticalFrame681,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop,
                                                2, 2, 2, 2));

  fCompositeFrame663->AddFrame(fVerticalFrame664,
                               new TGLayoutHints(kLHintsLeft |
                                                 kLHintsTop,
                                                 2, 2, 2, 2));
  fVerticalFrame664->MoveResize(2, 2, 372, 208);

  // "Set navigation step size" group frame
  TGGroupFrame *fGroupFrame911 = new TGGroupFrame(fCompositeFrame663,
                                                  "Set navigation step size");

  TGHorizontalFrame *fHorizontalFrame1035 = new TGHorizontalFrame(fGroupFrame911,
                                                                  160, 26,
                                                                  kHorizontalFrame);
  TGNumberEntry *fNumberEntry926 = new TGNumberEntry(fHorizontalFrame1035,
                                                     (Double_t) 0, 9, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame1035->AddFrame(fNumberEntry926,
                                 new TGLayoutHints(kLHintsLeft |
                                                   kLHintsCenterX |
                                                   kLHintsTop |
                                                   kLHintsCenterY,
                                                   2, 2, 2, 2));

  // combo box
  TGComboBox *fComboBox960 = new TGComboBox(fHorizontalFrame1035, -1,
                                            kHorizontalFrame |
                                            kSunkenFrame |
                                            kDoubleBorder |
                                            kOwnBackground);
  fComboBox960->AddEntry("minutes", 1);
  fComboBox960->AddEntry("hours", 2);
  fComboBox960->Resize(72, 22);
  fComboBox960->Select(1);
  fHorizontalFrame1035->AddFrame(fComboBox960,
                                 new TGLayoutHints(kLHintsLeft |
                                                   kLHintsCenterX |
                                                   kLHintsTop |
                                                   kLHintsCenterY,
                                                   2, 2, 2, 2));

  fGroupFrame911->AddFrame(fHorizontalFrame1035,
                           new TGLayoutHints(kLHintsLeft |
                                             kLHintsCenterX |
                                             kLHintsTop |
                                             kLHintsCenterY,
                                             2, 2, 2, 2));

  fGroupFrame911->SetLayoutManager(new TGVerticalLayout(fGroupFrame911));
  fGroupFrame911->Resize(384, 62);
  fCompositeFrame663->AddFrame(fGroupFrame911,
                               new TGLayoutHints(kLHintsLeft |
                                                 kLHintsBottom |
                                                 kLHintsExpandX,
                                                 2, 2, 2, 2));
  fGroupFrame911->MoveResize(2, 249, 384, 62);


  // container of "Tab2"
  TGCompositeFrame *fCompositeFrame765;
  fCompositeFrame765 = fTab660->AddTab("Run/Fill");
  fCompositeFrame765->SetLayoutManager(new TGVerticalLayout(fCompositeFrame765));
  fCompositeFrame765->SetLayoutBroken(kTRUE);

  TGVerticalFrame *fVerticalFrame766 = new TGVerticalFrame(fCompositeFrame765,
                                                           267, 98,
                                                           kVerticalFrame);

  TGHorizontalFrame *fHorizontalFrame767 = new TGHorizontalFrame(fVerticalFrame766,
                                                                 157, 30,
                                                                 kHorizontalFrame);
  TGRadioButton *fRadioButton768 = new TGRadioButton(fHorizontalFrame767,
                                                     "Last");
  fHorizontalFrame767->AddFrame(fRadioButton768,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame769 = new TGHorizontalFrame(fHorizontalFrame767,
                                                                 104, 26,
                                                                 kHorizontalFrame);
  TGNumberEntry *fNumberEntry770 = new TGNumberEntry(fHorizontalFrame769,
                                                     (Double_t) 0,6,-1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame769->AddFrame(fNumberEntry770,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));
  TGLabel *fLabel774 = new TGLabel(fHorizontalFrame769, "run(s)");
  fLabel774->SetTextJustify(36);
  fHorizontalFrame769->AddFrame(fLabel774,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  fHorizontalFrame767->AddFrame(fHorizontalFrame769,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsCenterX |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));

  fVerticalFrame766->AddFrame(fHorizontalFrame767,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsTop,
                                                2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame775 = new TGHorizontalFrame(fVerticalFrame766,
                                                                 151, 30,
                                                                 kHorizontalFrame);
  TGRadioButton *fRadioButton776 = new TGRadioButton(fHorizontalFrame775,
                                                     "Last");
  fHorizontalFrame775->AddFrame(fRadioButton776,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame777 = new TGHorizontalFrame(fHorizontalFrame775,
                                                                 98, 26,
                                                                 kHorizontalFrame);
  TGNumberEntry *fNumberEntry778 = new TGNumberEntry(fHorizontalFrame777,
                                                     (Double_t) 0,6,-1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame777->AddFrame(fNumberEntry778,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));
  TGLabel *fLabel782 = new TGLabel(fHorizontalFrame777, "fill(s)");
  fLabel782->SetTextJustify(36);
  fHorizontalFrame777->AddFrame(fLabel782, new TGLayoutHints(kLHintsLeft |
                                                             kLHintsTop |
                                                             kLHintsCenterY,
                                                             2, 2, 2, 2));

  fHorizontalFrame775->AddFrame(fHorizontalFrame777,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsCenterX |
                                                  kLHintsBottom,
                                                  2, 2, 2, 2));

  fVerticalFrame766->AddFrame(fHorizontalFrame775,
                              new TGLayoutHints(kLHintsLeft | 
                                                kLHintsTop,
                                                2, 2, 2, 2));

  TGHorizontalFrame *fHorizontalFrame783 = new TGHorizontalFrame(fVerticalFrame766,
                                                                 263, 26,
                                                                 kHorizontalFrame);
  TGRadioButton *fRadioButton784 = new TGRadioButton(fHorizontalFrame783,
                                                     "From");
  fHorizontalFrame783->AddFrame(fRadioButton784,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));

  // combo box
  TGComboBox *fComboBox785 = new TGComboBox(fHorizontalFrame783, -1,
                                            kHorizontalFrame |
                                            kSunkenFrame |
                                            kDoubleBorder |
                                            kOwnBackground);
  fComboBox785->AddEntry("run", 1);
  fComboBox785->AddEntry("fill", 2);
  fComboBox785->Resize(48, 22);
  fComboBox785->Select(1);
  fHorizontalFrame783->AddFrame(fComboBox785,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));
  TGNumberEntry *fNumberEntry798 = new TGNumberEntry(fHorizontalFrame783,
                                                     (Double_t) 0, 7, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame783->AddFrame(fNumberEntry798,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));
  TGLabel *fLabel802 = new TGLabel(fHorizontalFrame783, "to");
  fLabel802->SetTextJustify(36);
  fHorizontalFrame783->AddFrame(fLabel802,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop |
                                                  kLHintsCenterY,
                                                  2, 2, 2, 2));
  TGNumberEntry *fNumberEntry803 = new TGNumberEntry(fHorizontalFrame783,
                                                     (Double_t) 0, 6, -1,
                                                     (TGNumberFormat::EStyle) 5);
  fHorizontalFrame783->AddFrame(fNumberEntry803,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsTop,
                                                  2, 2, 2, 2));

  fVerticalFrame766->AddFrame(fHorizontalFrame783,
                              new TGLayoutHints(kLHintsLeft |
                                                kLHintsExpandX,
                                                2, 2, 2, 2));

  fCompositeFrame765->AddFrame(fVerticalFrame766,
                               new TGLayoutHints(kLHintsLeft |
                                                 kLHintsTop,
                                                 2, 2, 2, 2));
  fVerticalFrame766->MoveResize(2, 2, 267, 98);

  // "Set navigation step size" group frame
  TGGroupFrame *fGroupFrame1084 = new TGGroupFrame(fCompositeFrame765,
                                                   "Set navigation step size");

  TGHorizontalFrame *fHorizontalFrame1444 = new TGHorizontalFrame(fGroupFrame1084,
                                                                  152, 26,
                                                                  kHorizontalFrame);
  TGNumberEntry *fNumberEntry1445 = new TGNumberEntry(fHorizontalFrame1444,
                                                      (Double_t) 0, 9, -1,
                                                      (TGNumberFormat::EStyle) 5);
  fHorizontalFrame1444->AddFrame(fNumberEntry1445,
                                 new TGLayoutHints(kLHintsLeft |
                                                   kLHintsCenterX |
                                                   kLHintsTop |
                                                   kLHintsCenterY,
                                                   2, 2, 2, 2));

  // combo box
  TGComboBox *fComboBox1449 = new TGComboBox(fHorizontalFrame1444, -1,
                                             kHorizontalFrame |
                                             kSunkenFrame |
                                             kDoubleBorder |
                                             kOwnBackground);
  fComboBox1449->AddEntry("run(s)", 1);
  fComboBox1449->AddEntry("fill(s)", 2);
  fComboBox1449->Resize(64, 22);
  fComboBox1449->Select(1);
  fHorizontalFrame1444->AddFrame(fComboBox1449,
                                 new TGLayoutHints(kLHintsLeft |
                                                   kLHintsCenterX |
                                                   kLHintsTop |
                                                   kLHintsCenterY,
                                                   2, 2, 2, 2));

  fGroupFrame1084->AddFrame(fHorizontalFrame1444,
                            new TGLayoutHints(kLHintsLeft |
                                              kLHintsCenterX |
                                              kLHintsCenterY |
                                              kLHintsBottom,
                                              2, 2, 2, 2));

  fGroupFrame1084->SetLayoutManager(new TGVerticalLayout(fGroupFrame1084));
  fGroupFrame1084->Resize(384, 62);
  fCompositeFrame765->AddFrame(fGroupFrame1084,
                               new TGLayoutHints(kLHintsLeft |
                                                 kLHintsBottom |
                                                 kLHintsExpandX,2, 2, 2, 2));
  fGroupFrame1084->MoveResize(2, 249, 384, 62);

//  fTab660->SetTab(1);

  fTab660->Resize(fTab660->GetDefaultSize());
  fVerticalFrame658->AddFrame(fTab660, new TGLayoutHints(kLHintsLeft |
                                                         kLHintsCenterX));

  AddFrame(fVerticalFrame658, new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                                kLHintsCenterY,2, 2, 2, 2));
  SetMWMHints(kMWMDecorAll,
              kMWMFuncAll,
              kMWMInputModeless);
  
  MapSubwindows();              
//  MoveResize(8,16,388,286);
  Resize(GetDefaultSize());
  
//
//  fMainFrame5653->Resize(fMainFrame5653->GetDefaultSize());
  MapWindow();
//  fMainFrame5653->Resize(430,316);

}
void IntervalPicker::CloseWindow()
{
  DeleteWindow();
}
