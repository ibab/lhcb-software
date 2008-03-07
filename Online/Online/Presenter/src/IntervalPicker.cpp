#include "IntervalPicker.h"

#include <TGButton.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGNumberEntry.h>

#include "PresenterMainFrame.h"

using namespace pres;
using namespace std;

ClassImp(IntervalPicker)


IntervalPicker::IntervalPicker(PresenterMainFrame* gui) :
  TGTransientFrame(gClient->GetRoot(), gui),
  m_mainFrame(gui),
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
//  TGMainFrame *fMainFrame5653 = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
//  fMainFrame5653->SetLayoutBroken(kTRUE);

  // composite frame
//  TGCompositeFrame *this = new TGCompositeFrame(fMainFrame5653,388,286,kVerticalFrame);

  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame739 = new TGHorizontalFrame(this,80,26,kHorizontalFrame);
  TGTextButton *fTextButton740 = new TGTextButton(fHorizontalFrame739,"Ok");
  fTextButton740->SetTextJustify(36);
  fTextButton740->Resize(24,22);
  fHorizontalFrame739->AddFrame(fTextButton740, new TGLayoutHints(kLHintsRight | kLHintsBottom,2,2,2,2));
  TGTextButton *m_cancelButton = new TGTextButton(fHorizontalFrame739,"Cancel");
  m_cancelButton->SetTextJustify(36);
  m_cancelButton->Resize(48,22);
  m_cancelButton->Connect("Clicked()", "ReferencePicker",
                        this, "CloseWindow()");
  
  fHorizontalFrame739->AddFrame(m_cancelButton, new TGLayoutHints(kLHintsRight | kLHintsBottom,2,2,2,2));

  AddFrame(fHorizontalFrame739, new TGLayoutHints(kLHintsRight | kLHintsBottom,2,2,2,2));

  // vertical frame
  TGVerticalFrame *fVerticalFrame639 = new TGVerticalFrame(this,384,252,kVerticalFrame);
  TGLabel *fLabel758 = new TGLabel(fVerticalFrame639,"Please select interval for History mode by:");
  fLabel758->SetTextJustify(36);
  fVerticalFrame639->AddFrame(fLabel758, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  // tab widget
  TGTab *fTab640 = new TGTab(fVerticalFrame639,384,232);

  // container of "Tab1"
  TGCompositeFrame *fCompositeFrame643;
  fCompositeFrame643 = fTab640->AddTab("Time");
  fCompositeFrame643->SetLayoutManager(new TGVerticalLayout(fCompositeFrame643));
  fCompositeFrame643->SetLayoutBroken(kTRUE);

  // vertical frame
  TGVerticalFrame *fVerticalFrame3653 = new TGVerticalFrame(fCompositeFrame643,369,194,kVerticalFrame);
  fVerticalFrame3653->SetLayoutBroken(kTRUE);

  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame3654 = new TGHorizontalFrame(fVerticalFrame3653,289,30,kHorizontalFrame);
  TGRadioButton *fRadioButton3655 = new TGRadioButton(fHorizontalFrame3654,"Quick Selection:");
  fHorizontalFrame3654->AddFrame(fRadioButton3655, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2));

  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame3656 = new TGHorizontalFrame(fHorizontalFrame3654,167,26,kHorizontalFrame);
  TGLabel *fLabel3657 = new TGLabel(fHorizontalFrame3656,"last");
  fLabel3657->SetTextJustify(36);
  fHorizontalFrame3656->AddFrame(fLabel3657, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry3658 = new TGNumberEntry(fHorizontalFrame3656, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame3656->AddFrame(fNumberEntry3658, new TGLayoutHints(kLHintsLeft | kLHintsBottom,2,2,2,2));
  TGComboBox *fComboBox3662 = new TGComboBox(fHorizontalFrame3656,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
  fComboBox3662->AddEntry("minutes",1);
  fComboBox3662->AddEntry("hours",2);
  fComboBox3662->AddEntry("days",3);
  fComboBox3662->AddEntry("weeks",4);
  fComboBox3662->Resize(72,22);
  fComboBox3662->Select(1);
  fHorizontalFrame3656->AddFrame(fComboBox3662, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));

  fHorizontalFrame3654->AddFrame(fHorizontalFrame3656, new TGLayoutHints(kLHintsLeft | kLHintsCenterX | kLHintsBottom,2,2,2,2));

  fVerticalFrame3653->AddFrame(fHorizontalFrame3654, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fHorizontalFrame3654->MoveResize(2,2,289,30);

  // vertical frame
  TGVerticalFrame *fVerticalFrame4943 = new TGVerticalFrame(fVerticalFrame3653,367,160,kVerticalFrame);
  fVerticalFrame4943->SetLayoutBroken(kTRUE);

  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame4989 = new TGHorizontalFrame(fVerticalFrame4943,363,64,kHorizontalFrame);
  TGRadioButton *fRadioButton3676 = new TGRadioButton(fHorizontalFrame4989,"From");
  fHorizontalFrame4989->AddFrame(fRadioButton3676, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  // vertical frame
  TGVerticalFrame *fVerticalFrame5010 = new TGVerticalFrame(fHorizontalFrame4989,305,60,kVerticalFrame);

  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame647 = new TGHorizontalFrame(fVerticalFrame5010,301,26,kHorizontalFrame);
  TGLabel *fLabel648 = new TGLabel(fHorizontalFrame647,"Time:");
  fLabel648->SetTextJustify(36);
  fHorizontalFrame647->AddFrame(fLabel648, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry649 = new TGNumberEntry(fHorizontalFrame647, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame647->AddFrame(fNumberEntry649, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGLabel *fLabel653 = new TGLabel(fHorizontalFrame647,":");
  fLabel653->SetTextJustify(36);
  fHorizontalFrame647->AddFrame(fLabel653, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry654 = new TGNumberEntry(fHorizontalFrame647, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame647->AddFrame(fNumberEntry654, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGLabel *fLabel658 = new TGLabel(fHorizontalFrame647,":");
  fLabel658->SetTextJustify(36);
  fHorizontalFrame647->AddFrame(fLabel658, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry659 = new TGNumberEntry(fHorizontalFrame647, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame647->AddFrame(fNumberEntry659, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));

  fVerticalFrame5010->AddFrame(fHorizontalFrame647, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));

  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame663 = new TGHorizontalFrame(fVerticalFrame5010,301,26,kHorizontalFrame);
  TGLabel *fLabel664 = new TGLabel(fHorizontalFrame663,"Date:");
  fLabel664->SetTextJustify(36);
  fHorizontalFrame663->AddFrame(fLabel664, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry665 = new TGNumberEntry(fHorizontalFrame663, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame663->AddFrame(fNumberEntry665, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGComboBox *fComboBox669 = new TGComboBox(fHorizontalFrame663,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
  fComboBox669->Resize(128,22);
  fComboBox669->Select(-1);
  fHorizontalFrame663->AddFrame(fComboBox669, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry687 = new TGNumberEntry(fHorizontalFrame663, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame663->AddFrame(fNumberEntry687, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));

  fVerticalFrame5010->AddFrame(fHorizontalFrame663, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));

  fHorizontalFrame4989->AddFrame(fVerticalFrame5010, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  fVerticalFrame4943->AddFrame(fHorizontalFrame4989, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fHorizontalFrame4989->MoveResize(2,2,363,64);

  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame5099 = new TGHorizontalFrame(fVerticalFrame4943,364,64,kHorizontalFrame);

  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame4149 = new TGHorizontalFrame(fHorizontalFrame5099,51,26,kHorizontalFrame);
  TGLabel *fLabel3694 = new TGLabel(fHorizontalFrame4149,"to");
  fLabel3694->SetTextJustify(36);
  fHorizontalFrame4149->AddFrame(fLabel3694, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGTextButton *fTextButton966 = new TGTextButton(fHorizontalFrame4149,"Now");
  fTextButton966->SetTextJustify(36);
  fTextButton966->Resize(33,22);
  fHorizontalFrame4149->AddFrame(fTextButton966, new TGLayoutHints(kLHintsLeft | kLHintsCenterX | kLHintsTop,2,2,2,2));

  fHorizontalFrame5099->AddFrame(fHorizontalFrame4149, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  // vertical frame
  TGVerticalFrame *fVerticalFrame5168 = new TGVerticalFrame(fHorizontalFrame5099,305,60,kVerticalFrame);

  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame693 = new TGHorizontalFrame(fVerticalFrame5168,301,26,kHorizontalFrame);
  TGLabel *fLabel694 = new TGLabel(fHorizontalFrame693,"Time:");
  fLabel694->SetTextJustify(36);
  fHorizontalFrame693->AddFrame(fLabel694, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry695 = new TGNumberEntry(fHorizontalFrame693, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame693->AddFrame(fNumberEntry695, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGLabel *fLabel699 = new TGLabel(fHorizontalFrame693,":");
  fLabel699->SetTextJustify(36);
  fHorizontalFrame693->AddFrame(fLabel699, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry700 = new TGNumberEntry(fHorizontalFrame693, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame693->AddFrame(fNumberEntry700, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGLabel *fLabel704 = new TGLabel(fHorizontalFrame693,":");
  fLabel704->SetTextJustify(36);
  fHorizontalFrame693->AddFrame(fLabel704, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry705 = new TGNumberEntry(fHorizontalFrame693, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame693->AddFrame(fNumberEntry705, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));

  fVerticalFrame5168->AddFrame(fHorizontalFrame693, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));

  TGHorizontalFrame *fHorizontalFrame709 = new TGHorizontalFrame(fVerticalFrame5168,301,26,kHorizontalFrame);
  TGLabel *fLabel710 = new TGLabel(fHorizontalFrame709,"Date:");
  fLabel710->SetTextJustify(36);
  fHorizontalFrame709->AddFrame(fLabel710, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry711 = new TGNumberEntry(fHorizontalFrame709, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame709->AddFrame(fNumberEntry711, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));

  // combo box
  TGComboBox *fComboBox715 = new TGComboBox(fHorizontalFrame709,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
  fComboBox715->Resize(128,22);
  fComboBox715->Select(-1);
  fHorizontalFrame709->AddFrame(fComboBox715, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry733 = new TGNumberEntry(fHorizontalFrame709, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame709->AddFrame(fNumberEntry733, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));

  fVerticalFrame5168->AddFrame(fHorizontalFrame709, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));

  fHorizontalFrame5099->AddFrame(fVerticalFrame5168, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  fVerticalFrame4943->AddFrame(fHorizontalFrame5099, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fHorizontalFrame5099->MoveResize(2,70,364,64);

  fVerticalFrame3653->AddFrame(fVerticalFrame4943, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fVerticalFrame4943->MoveResize(2,36,367,160);

  fCompositeFrame643->AddFrame(fVerticalFrame3653, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fVerticalFrame3653->MoveResize(2,2,369,194);


  // container of "Tab2"
  TGCompositeFrame *fCompositeFrame738;
  fCompositeFrame738 = fTab640->AddTab("Run");
  fCompositeFrame738->SetLayoutManager(new TGVerticalLayout(fCompositeFrame738));
  fCompositeFrame738->SetLayoutBroken(kTRUE);

  // vertical frame
  TGVerticalFrame *fVerticalFrame2358 = new TGVerticalFrame(fCompositeFrame738,264,64,kVerticalFrame);

  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame2374 = new TGHorizontalFrame(fVerticalFrame2358,260,30,kHorizontalFrame);
  TGRadioButton *fTextButton2044 = new TGRadioButton(fHorizontalFrame2374,"Quick Selection:");
  fHorizontalFrame2374->AddFrame(fTextButton2044, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2));

  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame2565 = new TGHorizontalFrame(fHorizontalFrame2374,138,26,kHorizontalFrame);
  TGLabel *fLabel1842 = new TGLabel(fHorizontalFrame2565,"last");
  fLabel1842->SetTextJustify(36);
  fHorizontalFrame2565->AddFrame(fLabel1842, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry1871 = new TGNumberEntry(fHorizontalFrame2565, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame2565->AddFrame(fNumberEntry1871, new TGLayoutHints(kLHintsLeft | kLHintsBottom,2,2,2,2));
  TGComboBox *fComboBox3515 = new TGComboBox(fHorizontalFrame2565,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
  fComboBox3515->AddEntry("run",1);
  fComboBox3515->AddEntry("fill",2);
  fComboBox3515->Resize(48,22);
  fComboBox3515->Select(1);
  fHorizontalFrame2565->AddFrame(fComboBox3515, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));

  fHorizontalFrame2374->AddFrame(fHorizontalFrame2565, new TGLayoutHints(kLHintsLeft | kLHintsCenterX | kLHintsBottom,2,2,2,2));

  fVerticalFrame2358->AddFrame(fHorizontalFrame2374, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  // horizontal frame
  TGHorizontalFrame *fHorizontalFrame2369 = new TGHorizontalFrame(fVerticalFrame2358,260,26,kHorizontalFrame);
  TGRadioButton *fTextButton2022 = new TGRadioButton(fHorizontalFrame2369,"From");
  fHorizontalFrame2369->AddFrame(fTextButton2022, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));

  // combo box
  TGComboBox *fComboBox3013 = new TGComboBox(fHorizontalFrame2369,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
  fComboBox3013->AddEntry("run",1);
  fComboBox3013->AddEntry("fill",2);
  fComboBox3013->Resize(48,22);
  fComboBox3013->Select(1);
  fHorizontalFrame2369->AddFrame(fComboBox3013, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  TGNumberEntry *fNumberEntry1530 = new TGNumberEntry(fHorizontalFrame2369, (Double_t) 0,7,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame2369->AddFrame(fNumberEntry1530, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  TGLabel *fLabel1529 = new TGLabel(fHorizontalFrame2369,"to");
  fLabel1529->SetTextJustify(36);
  fHorizontalFrame2369->AddFrame(fLabel1529, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));
  TGNumberEntry *fNumberEntry3072 = new TGNumberEntry(fHorizontalFrame2369, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
  fHorizontalFrame2369->AddFrame(fNumberEntry3072, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  fVerticalFrame2358->AddFrame(fHorizontalFrame2369, new TGLayoutHints(kLHintsLeft | kLHintsExpandX,2,2,2,2));

  fCompositeFrame738->AddFrame(fVerticalFrame2358, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fVerticalFrame2358->MoveResize(2,2,264,64);


  fTab640->SetTab(0);

  fTab640->Resize(fTab640->GetDefaultSize());
  fVerticalFrame639->AddFrame(fTab640, new TGLayoutHints(kLHintsLeft | kLHintsCenterX));

  AddFrame(fVerticalFrame639, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,2,2,2,2));

//  fMainFrame5653->AddFrame(this, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

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
