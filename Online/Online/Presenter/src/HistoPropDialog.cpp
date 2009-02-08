#include <iostream>

#include <TSystem.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <TObject.h>
#include <TGMsgBox.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TColor.h>

#include <TGListBox.h>
#include <TGButtonGroup.h>
#include <TGColorSelect.h>
#include <TGedPatternSelect.h>
#include <TGTextEdit.h>
#include <TGButton.h>
#include <TGFSComboBox.h>
#include <TGLabel.h>

#include "HistoPropDialog.h"
#include "PresenterMainFrame.h"

using namespace pres;

ClassImp(HistoPropDialog)

HistoPropDialog::HistoPropDialog(PresenterMainFrame* gui, int width,
    int height, MsgLevel v) :
      TGTransientFrame(gClient->GetRoot(), gui, width, height),
      m_mainFrame(gui),
      m_verbosity(v),
      m_msgBoxReturnCode(0)
{
  SetCleanup(kDeepCleanup);
  Connect("CloseWindow()", "HistoPropDialog", this, "DontCallClose()");
  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputSystemModal);
  build();
  MapWindow();
}
HistoPropDialog::~HistoPropDialog()
{
  Cleanup();
}
void HistoPropDialog::ok()
{
  m_mainFrame->bulkHistoOptions.m_1DRootDrawOption =
    dynamic_cast<TGTextLBEntry*>(m_1DRootDrawOptionComboBox->
    GetSelectedEntry())->GetText()->GetString();
  m_mainFrame->bulkHistoOptions.m_2DRootDrawOption =
    dynamic_cast<TGTextLBEntry*>(m_2DRootDrawOptionComboBox->
    GetSelectedEntry())->GetText()->GetString();
  m_mainFrame->bulkHistoOptions.m_genericRootDrawOption =
    dynamic_cast<TGTextLBEntry*>(m_genericRootDrawOptionComboBox->
    GetSelectedEntry())->GetText()->GetString();
  
  m_mainFrame->bulkHistoOptions.m_description =  
    dynamic_cast<TGText*>(m_histogramDescriptionField->GetText())->AsString();  

    if (1 == m_statOptionComboBox->GetSelected()) {
      m_mainFrame->bulkHistoOptions.m_statsOption = 11111110;
    } else if (0 == m_statOptionComboBox->GetSelected()) {
      m_mainFrame->bulkHistoOptions.m_statsOption = 0;
    } else {
      m_mainFrame->bulkHistoOptions.m_statsOption = 0;      
    }
  m_mainFrame->bulkHistoOptions.m_xLabel =
    m_xAxisLabelTextEntry->GetDisplayText();
  m_mainFrame->bulkHistoOptions.m_yLabel =
    m_yAxisLabelTextEntry->GetDisplayText();
  m_mainFrame->bulkHistoOptions.m_fillStyle =
    (int) m_fillStylePatternSelector->GetPattern();
  m_mainFrame->bulkHistoOptions.m_fillColour =
    (int) TColor::GetColor(m_fillColourSelector->GetColor());
  m_mainFrame->bulkHistoOptions.m_lineWidth =
    (int) m_lineWidthComboBox->GetSelected();
  m_mainFrame->bulkHistoOptions.m_lineStyle =
    (int) m_lineStyleComboBox->GetSelected();
  m_mainFrame->bulkHistoOptions.m_lineColour =
    (int) TColor::GetColor(m_lineColourSelector->GetColor());

  m_mainFrame->bulkHistoOptions.m_drawPatternTextEntry =
        m_drawPatternTextEntry->GetDisplayText();    


  // m_bulkHistoOptions = new obsolete_BulkHistoOptions;
  // return m_bulkHistoOptions;
  CloseWindow();
}
void HistoPropDialog::build()
{

  // main frame
  //   TGMainFrame *fMainFrame916 = new TGMainFrame(gClient->GetRoot(), 10, w0, kMainFrame | kVerticalFrame);
  SetLayoutBroken(true);

  // composite frame
  TGCompositeFrame *fMainFrame648 = new TGCompositeFrame(this, 646, 435,
    kVerticalFrame);
  fMainFrame648->SetLayoutBroken(true);

  // composite frame
  TGCompositeFrame *fCompositeFrame649 = new TGCompositeFrame(fMainFrame648,
    664, 448, kVerticalFrame);
  fCompositeFrame649->SetLayoutBroken(true);
  m_cancelButton = new TGTextButton(fCompositeFrame649,"Default");
  m_cancelButton->SetTextJustify(36);
  m_cancelButton->Resize(92, 24);
  fCompositeFrame649->AddFrame(m_cancelButton,
                               new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                                 2, 2, 2, 2));
  m_cancelButton->MoveResize(536, 392, 92, 24);
  m_cancelButton->Connect("Clicked()", "HistoPropDialog",
                          this, "CloseWindow()");

  // "Display Options" group frame
  TGGroupFrame *fGroupFrame651 = new TGGroupFrame(fCompositeFrame649,
                                                  "Display Options");
  fGroupFrame651->SetLayoutBroken(true);
  TGLabel* m_xLabelLabel = new TGLabel(fGroupFrame651,"X Label");
  m_xLabelLabel->SetTextJustify(36);
  fGroupFrame651->AddFrame(m_xLabelLabel,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));
  m_xLabelLabel->MoveResize(16, 24, 57, 18);
  m_lineWidthComboBox = new TGLineWidthComboBox(fGroupFrame651, -1,
    kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);

  fGroupFrame651->AddFrame(m_lineWidthComboBox,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));

  m_lineWidthComboBox->MoveResize(112, 88, 102, 22);

  TGLabel* m_yLabelLabel = new TGLabel(fGroupFrame651,"Y Label");
  m_yLabelLabel->SetTextJustify(36);
  fGroupFrame651->AddFrame(m_yLabelLabel,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));
  m_yLabelLabel->MoveResize(296, 24, 57, 18);

  m_yAxisLabelTextEntry = new TGTextEntry(fGroupFrame651,
    new TGTextBuffer(15), -1);
  m_yAxisLabelTextEntry->SetMaxLength(255);
  m_yAxisLabelTextEntry->SetAlignment(kTextLeft);
  m_yAxisLabelTextEntry->SetText("");
  m_yAxisLabelTextEntry->Resize(184, m_yAxisLabelTextEntry->
    GetDefaultHeight());
  fGroupFrame651->AddFrame(m_yAxisLabelTextEntry,
    new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_yAxisLabelTextEntry->MoveResize(368, 24, 184, 22);


  // combo box
  m_statOptionComboBox = new TGComboBox(fGroupFrame651,-1,
                                        kHorizontalFrame | kSunkenFrame |
                                        kDoubleBorder | kOwnBackground);
  m_statOptionComboBox->AddEntry("none", 0);    
  m_statOptionComboBox->AddEntry("default", 1);
  m_statOptionComboBox->Resize(102, 22);
  m_statOptionComboBox->Select(0);
  fGroupFrame651->AddFrame(m_statOptionComboBox,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));
  m_statOptionComboBox->MoveResize(112, 56, 102, 22);
  TGLabel* fLabel677 = new TGLabel(fGroupFrame651, "Stats Option");
  fLabel677->SetTextJustify(36);
  fGroupFrame651->AddFrame(fLabel677,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));
  fLabel677->MoveResize(16, 56, 80, 18);
  TGLabel* fLabel678 = new TGLabel(fGroupFrame651,"Fill Style");
  fLabel678->SetTextJustify(36);
  fGroupFrame651->AddFrame(fLabel678,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));
  fLabel678->MoveResize(224, 56, 57, 18);

  // combo box
  m_fillStylePatternSelector = new TGedPatternSelect(fGroupFrame651, 0, -1);
  fGroupFrame651->AddFrame(m_fillStylePatternSelector,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));
  m_fillStylePatternSelector->MoveResize(304, 56, 102, 22);
  TGLabel* fLabel697 = new TGLabel(fGroupFrame651,"Fill Colour");
  fLabel697->SetTextJustify(36);
  fGroupFrame651->AddFrame(fLabel697,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));
  fLabel697->MoveResize(424, 56, 57, 18);

  // color select widget
  m_fillColourSelector = new TGColorSelect(fGroupFrame651, 0, -1);

  fGroupFrame651->AddFrame(m_fillColourSelector,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));
  m_fillColourSelector->MoveResize(504, 56, 45, 23);
  TGLabel* fLabel699 = new TGLabel(fGroupFrame651,"Line Width");
  fLabel699->SetTextJustify(36);
  fGroupFrame651->AddFrame(fLabel699,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));
  fLabel699->MoveResize(16, 88, 80, 18);
  TGLabel* fLabel700 = new TGLabel(fGroupFrame651,"Line Style");
  fLabel700->SetTextJustify(36);
  fGroupFrame651->AddFrame(fLabel700,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));
  fLabel700->MoveResize(216, 88, 80, 18);

  // combo box
  m_lineStyleComboBox = new  TGLineStyleComboBox(fGroupFrame651, -1,
    kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);

  fGroupFrame651->AddFrame(m_lineStyleComboBox, new TGLayoutHints(kLHintsLeft
    | kLHintsTop, 2, 2, 2, 2));
  m_lineStyleComboBox->MoveResize(304, 88, 102, 22);

  TGLabel* fLabel719 = new TGLabel(fGroupFrame651,"Line Colour");
  fLabel719->SetTextJustify(36);
  fGroupFrame651->AddFrame(fLabel719, new TGLayoutHints(kLHintsLeft |
    kLHintsTop, 2, 2, 2, 2));
  fLabel719->MoveResize(424, 88, 72, 18);

  // color select widget
  m_lineColourSelector = new TGColorSelect(fGroupFrame651, 0, -1);
  fGroupFrame651->AddFrame(m_lineColourSelector, new TGLayoutHints(
    kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_lineColourSelector->MoveResize(504, 88, 45, 23);
  TGLabel* fLabel721 = new TGLabel(fGroupFrame651,
    "Generic ROOT Draw Options");
  fLabel721->SetTextJustify(36);
  fGroupFrame651->AddFrame(fLabel721, new TGLayoutHints(kLHintsLeft |
    kLHintsTop, 2, 2, 2, 2));
  fLabel721->MoveResize(16, 120, 176, 18);
  TGLabel* fLabel722 = new TGLabel(fGroupFrame651,"1D ROOT Draw Options");
  fLabel722->SetTextJustify(36);
  fGroupFrame651->AddFrame(fLabel722, new TGLayoutHints(kLHintsLeft |
    kLHintsTop, 2, 2, 2, 2));
  fLabel722->MoveResize(16, 144, 176, 18);
  TGLabel* fLabel723 = new TGLabel(fGroupFrame651,"2D ROOT Draw Options");
  fLabel722->SetTextJustify(36);
  fGroupFrame651->AddFrame(fLabel723, new TGLayoutHints(kLHintsLeft |
    kLHintsTop, 2, 2, 2, 2));
  fLabel723->MoveResize(16, 164, 176, 18);
  TGLabel* fLabel724 = new TGLabel(fGroupFrame651,
    "Pattern to overlay");
  fLabel721->SetTextJustify(36);
  fGroupFrame651->AddFrame(fLabel724, new TGLayoutHints(kLHintsLeft |
    kLHintsTop, 2, 2, 2, 2));
  fLabel724->MoveResize(16, 184, 176, 18);
  
  
  m_drawPatternTextEntry = new TGTextEntry(fGroupFrame651,
    new TGTextBuffer(15), -1);
  m_drawPatternTextEntry->SetMaxLength(255);
  m_drawPatternTextEntry->SetAlignment(kTextLeft);
  m_drawPatternTextEntry->SetText("");
//  m_drawPatternTextEntry->Resize(184, m_drawPatternTextEntry->
//    GetDefaultHeight());
  fGroupFrame651->AddFrame(m_drawPatternTextEntry,
    new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_drawPatternTextEntry->MoveResize(200, 184, 152, 18);  

  // combo box
  m_genericRootDrawOptionComboBox = new TGComboBox(fGroupFrame651,
    "HIST", kHorizontalFrame | kSunkenFrame | kDoubleBorder |
    kOwnBackground);
  m_genericRootDrawOptionComboBox->AddEntry("", 0);
  m_genericRootDrawOptionComboBox->AddEntry("AXIS", 19);
  m_genericRootDrawOptionComboBox->AddEntry("AXIG", 1);
  m_genericRootDrawOptionComboBox->AddEntry("HIST", 2);
  m_genericRootDrawOptionComboBox->AddEntry("SAME", 3);
  m_genericRootDrawOptionComboBox->AddEntry("CYL", 4);
  m_genericRootDrawOptionComboBox->AddEntry("POL", 5);
  m_genericRootDrawOptionComboBox->AddEntry("SPH", 6);
  m_genericRootDrawOptionComboBox->AddEntry("PSR", 7);
  m_genericRootDrawOptionComboBox->AddEntry("LEGO", 8);
  m_genericRootDrawOptionComboBox->AddEntry("LEGO1", 9);
  m_genericRootDrawOptionComboBox->AddEntry("LEGO2", 10);
  m_genericRootDrawOptionComboBox->AddEntry("SURF", 11);
  m_genericRootDrawOptionComboBox->AddEntry("SURF1", 12);
  m_genericRootDrawOptionComboBox->AddEntry("SURF2", 13);
  m_genericRootDrawOptionComboBox->AddEntry("SURF3", 14);
  m_genericRootDrawOptionComboBox->AddEntry("SURF4", 15);
  m_genericRootDrawOptionComboBox->AddEntry("SURF5", 16);
  m_genericRootDrawOptionComboBox->AddEntry("X+", 17);
  m_genericRootDrawOptionComboBox->AddEntry("Y+", 18);

  m_genericRootDrawOptionComboBox->Resize(102, 22);
  m_genericRootDrawOptionComboBox->Select(2);
  fGroupFrame651->AddFrame(m_genericRootDrawOptionComboBox,
    new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_genericRootDrawOptionComboBox->MoveResize(200, 120, 102, 22);

  // combo box
  m_1DRootDrawOptionComboBox = new TGComboBox(fGroupFrame651, "HIST",
    kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);

  m_1DRootDrawOptionComboBox->AddEntry("", 0); //"AC", 1);
  m_1DRootDrawOptionComboBox->AddEntry("AH", 26); //"AC", 1);
  m_1DRootDrawOptionComboBox->AddEntry("][", 1);
  m_1DRootDrawOptionComboBox->AddEntry("B", 2);
  m_1DRootDrawOptionComboBox->AddEntry("C", 3);
  m_1DRootDrawOptionComboBox->AddEntry("E", 4);
  m_1DRootDrawOptionComboBox->AddEntry("E0", 5);
  m_1DRootDrawOptionComboBox->AddEntry("E1", 6);
  m_1DRootDrawOptionComboBox->AddEntry("E2", 7);
  m_1DRootDrawOptionComboBox->AddEntry("E3", 8);
  m_1DRootDrawOptionComboBox->AddEntry("E4", 9);
  m_1DRootDrawOptionComboBox->AddEntry("X0", 10);
  m_1DRootDrawOptionComboBox->AddEntry("L", 11);
  m_1DRootDrawOptionComboBox->AddEntry("P", 12);
  m_1DRootDrawOptionComboBox->AddEntry("P0", 13);
  m_1DRootDrawOptionComboBox->AddEntry("PE", 14);
  m_1DRootDrawOptionComboBox->AddEntry("P0E", 15);
  m_1DRootDrawOptionComboBox->AddEntry("PE0", 16);
  m_1DRootDrawOptionComboBox->AddEntry("PE1", 17);
  m_1DRootDrawOptionComboBox->AddEntry("PE2", 18);
  m_1DRootDrawOptionComboBox->AddEntry("PE3", 19);
  m_1DRootDrawOptionComboBox->AddEntry("PE4", 20);
  m_1DRootDrawOptionComboBox->AddEntry("PIE", 21);
  m_1DRootDrawOptionComboBox->AddEntry("*H", 22);
  m_1DRootDrawOptionComboBox->AddEntry("LF2", 23);
  m_1DRootDrawOptionComboBox->AddEntry("9", 24);
  m_1DRootDrawOptionComboBox->AddEntry("HIST", 25);
  m_1DRootDrawOptionComboBox->Resize(102, 22);
  m_1DRootDrawOptionComboBox->Select(25);
  fGroupFrame651->AddFrame(m_1DRootDrawOptionComboBox,
    new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));
  m_1DRootDrawOptionComboBox->MoveResize(200, 144, 102, 22);

  // combo box
  m_2DRootDrawOptionComboBox = new TGComboBox(fGroupFrame651,"COLZ",
    kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);

  m_2DRootDrawOptionComboBox->AddEntry("", 0);
  m_2DRootDrawOptionComboBox->AddEntry("ARR", 18);
  m_2DRootDrawOptionComboBox->AddEntry("BOX", 1);
  m_2DRootDrawOptionComboBox->AddEntry("BOX1", 2);
  m_2DRootDrawOptionComboBox->AddEntry("COL", 3);
  m_2DRootDrawOptionComboBox->AddEntry("COLZ", 4);
  m_2DRootDrawOptionComboBox->AddEntry("CONT", 5);
  m_2DRootDrawOptionComboBox->AddEntry("CONT0", 6);
  m_2DRootDrawOptionComboBox->AddEntry("CONT1", 7);
  m_2DRootDrawOptionComboBox->AddEntry("CONT2", 8);
  m_2DRootDrawOptionComboBox->AddEntry("CONT3", 9);
  m_2DRootDrawOptionComboBox->AddEntry("CONT4", 10);
  m_2DRootDrawOptionComboBox->AddEntry("CONT5", 11);
  m_2DRootDrawOptionComboBox->AddEntry("LIST", 12);
  m_2DRootDrawOptionComboBox->AddEntry("FB", 13);
  m_2DRootDrawOptionComboBox->AddEntry("BB", 14);
  m_2DRootDrawOptionComboBox->AddEntry("A", 15);
  m_2DRootDrawOptionComboBox->AddEntry("SCAT", 16);
  m_2DRootDrawOptionComboBox->AddEntry("TEXT", 17);
  m_2DRootDrawOptionComboBox->Resize(102, 22);
  m_2DRootDrawOptionComboBox->Select(4);
  fGroupFrame651->AddFrame(m_2DRootDrawOptionComboBox,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));
  m_2DRootDrawOptionComboBox->MoveResize(200, 164, 102, 22);

  m_xAxisLabelTextEntry = new TGTextEntry(fGroupFrame651,
                                          new TGTextBuffer(15), -1);
  m_xAxisLabelTextEntry->SetMaxLength(255);
  m_xAxisLabelTextEntry->SetAlignment(kTextLeft);
  m_xAxisLabelTextEntry->SetText("");
  m_xAxisLabelTextEntry->Resize(184, m_xAxisLabelTextEntry->
    GetDefaultHeight());
  fGroupFrame651->AddFrame(m_xAxisLabelTextEntry,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));
  m_xAxisLabelTextEntry->MoveResize(88, 24, 184, 22);

  fGroupFrame651->SetLayoutManager(new TGVerticalLayout(fGroupFrame651));
  fGroupFrame651->Resize(608, 232);
  fCompositeFrame649->AddFrame(fGroupFrame651,
                               new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                                 2, 2, 2, 2));
  fGroupFrame651->MoveResize(24, 152, 608, 232);
  TGTextButton *fTextButton760 = new TGTextButton(fCompositeFrame649,"OK");
  fTextButton760->SetTextJustify(36);
  fTextButton760->Resize(92, 24);
  fCompositeFrame649->AddFrame(fTextButton760,
                               new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                                 2, 2, 2, 2));
  fTextButton760->MoveResize(432, 392, 92, 24);
  fTextButton760->Connect("Clicked()", "HistoPropDialog", this, "ok()");

  // "Description" group frame
  TGGroupFrame *fGroupFrame761 = new TGGroupFrame(fCompositeFrame649,
                                                  "Description");
  m_histogramDescriptionField = new TGTextEdit(fGroupFrame761, 592, 104);
  m_histogramDescriptionField->EnableMenu(true);
  fGroupFrame761->AddFrame(m_histogramDescriptionField,
                           new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                             2, 2, 2, 2));

  fGroupFrame761->SetLayoutManager(new TGVerticalLayout(fGroupFrame761));
  fGroupFrame761->Resize(608, 140);
  fCompositeFrame649->AddFrame(fGroupFrame761,
                               new TGLayoutHints(kLHintsLeft | kLHintsTop,
                                                 2, 2, 2, 2));
  fGroupFrame761->MoveResize(24, 16, 608, 140);

  fMainFrame648->AddFrame(fCompositeFrame649,
                          new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  fCompositeFrame649->MoveResize(0, 0, 664, 448);

  AddFrame(fMainFrame648, new TGLayoutHints(kLHintsExpandX |
                                            kLHintsExpandY));
  fMainFrame648->MoveResize(0, 0, 646, 435);

  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
  Resize(658, 434);
}
void HistoPropDialog::CloseWindow() {
  DeleteWindow();
}
