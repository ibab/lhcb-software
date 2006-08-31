// $Id: HPDGui.cpp,v 1.3 2006-08-31 06:54:05 ukerzel Exp $
// Include files 

#include <iostream>
#include <stdlib.h>
#include <time.h>

//ROOT
#include <TROOT.h>
#include <TApplication.h>
#include <TColor.h>
#include <TStyle.h>
#include <TFile.h>

// local
#include "HPDGui.h"

//
// DIM - Delphi Information Management system
//
#include <dic.hxx>

//-----------------------------------------------------------------------------
// Implementation file for class : HPDGui
//
// 2006-08-08 : Ulrich Kerzel
//-----------------------------------------------------------------------------


// constructor
HPDGui::HPDGui(const TGWindow *p, UInt_t guiWidth, UInt_t guiHeight)  : 
  TGMainFrame(p,guiWidth,guiHeight),
  m_timerRuns(false),
  m_nTimeSteps(100),
  m_refreshTimeHisto(5),
  m_refreshTimeCounter(1),
  m_counterMin(0),
  m_counterMax(1000),
  m_2DDrawOption(""),
  m_nCanvasRows(0),
  m_nCanvasColumns(0),
  m_connectOK(false)
{

  //
  // main GUI
  //

  // for the main canvas showing the histograms
  const int canvasWidth    = (int) (guiWidth  - 3.0*floor((double)guiWidth/10.0));
  const int canvasHeight   = (int) (guiHeight - 1.0*floor((double)guiHeight/10.0));  
  // for the selection box of the various DIM services
  const int listTreeWidth  = (int) (guiWidth  - 7.5*floor((double)guiWidth/10.0));  
  const int listTreeHeight = (int) ((double)canvasHeight/2.0);

  // pre-define layout options
  m_LayoutTopLeft         =  new TGLayoutHints(kLHintsTop     | kLHintsLeft                   , 5, 5, 5, 5);
  m_LayoutTopLeftExpandX  =  new TGLayoutHints(kLHintsTop     | kLHintsLeft  | kLHintsExpandX , 5, 5, 5, 5);
  m_LayoutTopRight        =  new TGLayoutHints(kLHintsTop     | kLHintsRight                  , 5, 5, 5, 5);
  m_LayoutLeftTop         =  new TGLayoutHints(kLHintsLeft    | kLHintsTop                    , 5, 5, 5, 5);
  m_LayoutBottomLeft      =  new TGLayoutHints(kLHintsBottom  | kLHintsLeft                   , 5, 5, 5, 5);
  m_LayoutBottomRight     =  new TGLayoutHints(kLHintsBottom  | kLHintsRight                  , 5, 5, 5, 5);

  // pre-define colours used
  gClient->GetColorByName("green" , m_ROOTGreen);
  gClient->GetColorByName("red"   , m_ROOTRed);
  gClient->GetColorByName("yellow", m_ROOTYellow);



  //                                               client width height options           background            
  // ?how do the numbers of w/h affect things?
  m_CompositeFrameMaster     = new TGCompositeFrame(this,   200,   100,    kVerticalFrame);
  AddFrame(m_CompositeFrameMaster, m_LayoutLeftTop);                

  m_CompositeFrameMain       =  new TGCompositeFrame(m_CompositeFrameMaster, 200, 100, kHorizontalFrame);  
  m_CompositeFrameMaster     -> AddFrame(m_CompositeFrameMain, m_LayoutTopLeft);

  m_CompositeFrameButtons    =  new TGCompositeFrame(m_CompositeFrameMain, 100, 20, kVerticalFrame);  
  m_CompositeFrameMain       -> AddFrame(m_CompositeFrameButtons, m_LayoutTopRight);

  m_GroupFrameListTree       =  new TGGroupFrame(m_CompositeFrameButtons, "DIM services", kHorizontalFrame);


  //
  // Define the tree-structure like selection box for the various DIM services
  //
  m_CanvasListTree           =  new TGCanvas(m_CompositeFrameButtons, 
                                             listTreeWidth, listTreeHeight, 
                                             kSunkenFrame | kDoubleBorder);
  m_ListTreeDimServices      =  new TGListTree(m_CanvasListTree->GetViewPort(), 200, 500,kHorizontalFrame);
  m_ListTreeDimServices      -> SetCanvas(m_CanvasListTree);
  m_ListTreeDimServices      -> Associate(this);  

  m_ListTreeItemMain         = m_ListTreeDimServices -> AddItem(0, "DIM services");  // the "root" of the tree
                                                                                    // add all servers below this one
  m_CanvasListTree           -> SetContainer(m_ListTreeDimServices);
  m_CompositeFrameButtons    -> AddFrame(m_CanvasListTree,m_LayoutTopLeft);

  //
  // Frame holding "Connect" button and HPD row/column entry fields
  //
  m_GroupFrameHPDControl     =  new TGGroupFrame(m_CompositeFrameButtons, "setup", kHorizontalFrame);
  //                                                                    frame  ,          #rows #colums separator
  m_GroupFrameHPDControl     -> SetLayoutManager(new TGMatrixLayout(m_GroupFrameHPDControl,  0,     2,      5));
  m_CompositeFrameButtons    -> AddFrame(m_GroupFrameHPDControl, m_LayoutTopLeft);
  
  // entry field for refresh time -- affects 2D "ring" histogram
  m_stringRefreshTimeHisto      =  new TGHotString("refresh histo (s)");
  m_labelRefreshTimeHisto       =  new TGLabel(m_GroupFrameHPDControl, m_stringRefreshTimeHisto);
  m_GroupFrameHPDControl     -> AddFrame(m_labelRefreshTimeHisto, m_LayoutTopLeftExpandX);
  m_EntryRefreshTimeHisto       =  new TGNumberEntry(m_GroupFrameHPDControl, 5, 3, -1, 
                                                  TGNumberFormat::kNESInteger,
                                                  TGNumberFormat::kNEAPositive, 
                                                  TGNumberFormat::kNELLimitMinMax, 1, 100);  
  m_GroupFrameHPDControl     -> AddFrame(m_EntryRefreshTimeHisto  , m_LayoutTopLeft);
  
  // entry field for refresh time -- affects HPD hit counter
  m_stringRefreshTimeCounter =  new TGHotString("refresh hit counter (s)");
  m_labelRefreshTimeCounter  =  new TGLabel(m_GroupFrameHPDControl, m_stringRefreshTimeCounter);
  m_GroupFrameHPDControl     -> AddFrame(m_labelRefreshTimeCounter, m_LayoutTopLeftExpandX);
  m_EntryRefreshTimeCounter  =  new TGNumberEntry(m_GroupFrameHPDControl, 1, 3, -1, 
                                                  TGNumberFormat::kNESInteger,
                                                  TGNumberFormat::kNEAPositive, 
                                                  TGNumberFormat::kNELLimitMinMax, 1, 100);  
  m_GroupFrameHPDControl     -> AddFrame(m_EntryRefreshTimeCounter  , m_LayoutTopLeft);

  // entry field for counter min. value
  m_stringCounterMin         =  new TGHotString("counter min.");
  m_labelCounterMin          =  new TGLabel(m_GroupFrameHPDControl, m_stringCounterMin);
  m_GroupFrameHPDControl     -> AddFrame(m_labelCounterMin, m_LayoutTopLeftExpandX);
  m_EntryCounterMin          =  new TGNumberEntry(m_GroupFrameHPDControl, 1, 5, -1, 
                                                  TGNumberFormat::kNESInteger,
                                                  TGNumberFormat::kNEAPositive, 
                                                  TGNumberFormat::kNELLimitMinMax, -10000, 10000);  
  m_GroupFrameHPDControl     -> AddFrame(m_EntryCounterMin  , m_LayoutTopLeft);

  // entry field for counter max. value
  m_stringCounterMax         =  new TGHotString("counter max.");
  m_labelCounterMax          =  new TGLabel(m_GroupFrameHPDControl, m_stringCounterMax);
  m_GroupFrameHPDControl     -> AddFrame(m_labelCounterMax, m_LayoutTopLeftExpandX);
  m_EntryCounterMax          =  new TGNumberEntry(m_GroupFrameHPDControl, 1000, 5, -1, 
                                                  TGNumberFormat::kNESInteger,
                                                  TGNumberFormat::kNEAPositive, 
                                                  TGNumberFormat::kNELLimitMinMax, -10000, 10000);  
  m_GroupFrameHPDControl     -> AddFrame(m_EntryCounterMax  , m_LayoutTopLeft);

  // entry for draw option used for 2D histogram
  m_string2DDrawOption       =  new TGHotString("2D draw option");
  m_label2DDrawOption        =  new TGLabel(m_GroupFrameHPDControl, m_string2DDrawOption);
  m_GroupFrameHPDControl     -> AddFrame(m_label2DDrawOption, m_LayoutTopLeftExpandX);
  m_Entry2DDrawOption        =  new TGComboBox(m_GroupFrameHPDControl, "option", id2DDrawOption);
  m_Entry2DDrawOption        -> Resize(80,20);
  m_Entry2DDrawOption        -> AddEntry("default", id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("box"    , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("colz"   , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("surf"   , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("surf2"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("surf3"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("surf4"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("lego"   , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("lego1"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("lego2"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("lego2"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("cont"   , id2DDrawOption);
  m_GroupFrameHPDControl     -> AddFrame(m_Entry2DDrawOption,  m_LayoutTopLeft);
  

  // define "Connect" buttton
  m_ButtonConnect            =  new TGTextButton(m_GroupFrameHPDControl, "&connect",idConnect);
  m_ButtonConnect            -> Associate(this);  
  m_GroupFrameHPDControl     -> AddFrame(m_ButtonConnect , m_LayoutTopLeft);

  m_ButtonSelect             =  new TGTextButton(m_GroupFrameHPDControl, "&select",idSelect);
  m_ButtonSelect             -> Associate(this);  
  m_GroupFrameHPDControl     -> AddFrame(m_ButtonSelect , m_LayoutTopLeft);

  //
  // Frame holding other control buttons
  //
  m_GroupFrameGuiControl     =  new TGGroupFrame(m_CompositeFrameButtons, "Control buttons", kHorizontalFrame);
  m_GroupFrameGuiControl     -> SetLayoutManager(new TGMatrixLayout(m_GroupFrameGuiControl,  2, 3, 5));
  m_CompositeFrameButtons    -> AddFrame(m_GroupFrameGuiControl, m_LayoutTopLeft);

  // define Play/Pause button
  m_ButtonPause              =  new TGTextButton(m_GroupFrameGuiControl, "&start  ", idPause);
  m_ButtonPause              -> Associate(this);
  m_ButtonPause              -> ChangeBackground(m_ROOTGreen);
  m_GroupFrameGuiControl     -> AddFrame(m_ButtonPause, m_LayoutBottomRight);
  
  // define "Print" button
  m_ButtonPrint   =  new TGTextButton(m_GroupFrameGuiControl, "&print", idPrint);
  m_ButtonPrint   -> Associate(this);
  m_GroupFrameGuiControl->AddFrame(m_ButtonPrint, m_LayoutBottomRight);
  
  // define "exit" button
  m_ButtonExit              =  new TGTextButton(m_GroupFrameGuiControl, "&exit ",idExit);
  m_ButtonExit              -> Associate(this);  
  m_ButtonExit              -> ChangeBackground(m_ROOTRed);  
  m_GroupFrameGuiControl    -> AddFrame(m_ButtonExit , m_LayoutBottomRight);


  //
  // define canvas
  //

  m_EmbeddedCanvas          =  new TRootEmbeddedCanvas("EmbeddedCanvas",m_CompositeFrameMain,canvasWidth,canvasHeight);
  m_Canvas                  =  m_EmbeddedCanvas->GetCanvas();
  m_Canvas                  -> SetFillColor(10); // white background
  m_CompositeFrameMain      -> AddFrame(m_EmbeddedCanvas , m_LayoutTopLeft);  


  
  //
  // status bar
  //
  int statusWidth        =  canvasWidth;
  int statusHeight       =  2;  
  m_StatusBar            =  new TGStatusBar(m_CompositeFrameMaster, statusWidth, statusHeight);
  m_StatusBar            -> SetText("HPD monitor initalised");  
  m_CompositeFrameMaster -> AddFrame(m_StatusBar, m_LayoutBottomLeft);
  

  //
  // now buid up GUI using methods of the base class
  //
  MapSubwindows();
  
  Layout();
  SetWindowName("HPD Monitor");
  MapWindow();

  //
  // setup own colour palette
  // green -> red
  //
  const int colNum     = 128; // Number of colors in the palette
  int       startIndex = 300; // starting index of allocated colors
  int       palette[colNum];
  float     val;
  
  for (int i=0; i < colNum; i++) {
    val = i/(float)colNum;
    TColor *m_colour = new TColor(startIndex+i, val, 1-val, 0, "");
    palette[i] = startIndex + i;
  }
  gStyle->SetPalette(colNum, palette);


} //HPDGui - constructor

// ------------------------------------------------------------------------------------------
HPDGui::~HPDGui() {

  HPDGui::Reset();

  std::vector<TGListTreeItem *>::const_iterator listTreeIter;
  std::vector<TGListTreeItem *>::const_iterator listTreeIterBegin;
  std::vector<TGListTreeItem *>::const_iterator listTreeIterEnd;
  listTreeIterBegin = m_SelectedHistogramVector.begin();
  listTreeIterEnd   = m_SelectedHistogramVector.end();  
  for (listTreeIter = listTreeIterBegin; listTreeIter != listTreeIterEnd; listTreeIter++){
    std::cout << "delete selected  histogram item" << std::endl;
    delete (*listTreeIter);
  } //for  

  listTreeIterBegin = m_SelectedCounterVector.begin();
  listTreeIterEnd   = m_SelectedCounterVector.end();  
  for (listTreeIter = listTreeIterBegin; listTreeIter != listTreeIterEnd; listTreeIter++){
    std::cout << "delete selected  counter item" << std::endl;
    delete (*listTreeIter);
  } //for    
  
  // Delete all created widgets.
  delete m_GroupFrameListTree;
  delete m_CanvasListTree;    
  delete m_ListTreeDimServices;
  delete m_ListTreeItemMain;
  delete m_StatusBar;
  delete m_CompositeFrameMaster;  
  delete m_CompositeFrameMain;   
  delete m_CompositeFrameButtons; 
  delete m_GroupFrameHPDControl; 
  delete m_GroupFrameGuiControl;  
  delete m_ButtonConnect;  
  delete m_ButtonExit; 
  delete m_ButtonPrint;
  delete m_ButtonPause;
  delete m_ButtonSelect;  
  delete m_EntryRefreshTimeHisto;
  delete m_EntryRefreshTimeCounter;
  delete m_EmbeddedCanvas;
  delete m_Canvas;             
  delete m_padUpper;             
  delete m_padLower;  
  delete m_labelRefreshTimeHisto;
  delete m_labelRefreshTimeCounter;
  delete m_stringRefreshTimeHisto;
  delete m_stringRefreshTimeCounter;
  delete m_LayoutTopLeft;
  delete m_LayoutTopLeftExpandX;
  delete m_LayoutTopRight;
  delete m_LayoutLeftTop;
  delete m_LayoutBottomLeft;
  delete m_LayoutBottomRight;
  delete m_externalTimer;
  
} // HPDGui - destructor
// ------------------------------------------------------------------------------------------
Bool_t HPDGui::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) {

  //
  // handles all the events/messages sent from the buttons, etc
  //

  std::vector<TGListTreeItem *>::const_iterator serviceIter;
  std::vector<TGListTreeItem *>::const_iterator serviceIterBegin = m_ListTreeItemVector.begin();
  std::vector<TGListTreeItem *>::const_iterator serviceIterEnd   = m_ListTreeItemVector.end();

  std::string tmpString;
  TFile      *outFile;
  TDirectory *cwd     = gDirectory;	// current working directory, prior
                                        // to opening ROOT file to save output
	
  TDirectory *outDir;
  
  switch (GET_MSG(msg)) {

  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
      
    case kCM_BUTTON:
      switch(parm1) {
      case idConnect:
        if (m_timerRuns) 
          HPDGui::Pause();        
        m_connectOK = HPDGui::Connect2DIM();
        if (m_connectOK) {
          m_StatusBar->SetText("successfully connected to DIM");          
        } else {
          m_StatusBar->SetText("connection to DIM failed");          
        } // if m_connectOK
        break;
        
      case idExit:
        HPDGui::CloseWindow();
        break;

      case idPrint:
	char fileName[200];
	char timeString[100];	
	char msg[1000];

	// get current time
	time_t rawtime;
	tm * ptm;
	time ( &rawtime );
	ptm = gmtime ( &rawtime );
	sprintf(timeString,"%i-%i-%i_%i:%i:%iGMT",
		ptm->tm_year+1900,
		ptm->tm_mon+1,
		ptm->tm_mday,
		ptm->tm_hour,
		ptm->tm_min,
		ptm->tm_sec);
	sprintf(fileName,"HPDMonitor_%s",timeString);

	sprintf(msg,"printing file %s", fileName);
        m_StatusBar->SetText(msg);

	sprintf(msg, "%s.eps", fileName);
	m_Canvas->Print(msg);
	sprintf(msg, "%s.jpg", fileName);
	m_Canvas->Print(msg);
	

	// save as ROOT file
	outFile = new TFile("monitoring.root","UPDATE");	
	outDir  = new TDirectory(fileName,fileName);
	outDir->cd();
	
	if (outFile->IsZombie()){
	  m_StatusBar->SetText("Error opening ROOT file");
	} else {
	  std::vector<H1DHisto>::const_iterator h1DIter;
	  std::vector<H1DHisto>::const_iterator h1DIterBegin = m_histo1DVector.begin();
	  std::vector<H1DHisto>::const_iterator h1DIterEnd   = m_histo1DVector.end();
	  for (h1DIter = h1DIterBegin; h1DIter != h1DIterEnd; h1DIter++) {
	    sprintf(msg,"%s-%s",fileName,h1DIter->h1D->GetTitle());
	    TH1F* newHisto = (TH1F*) h1DIter->h1D->Clone(msg);
	    newHisto       -> SetTitle(msg);	    
	  } // for h1DIter

	  
	  std::vector<H2DHisto>::const_iterator h2DIter;
	  std::vector<H2DHisto>::const_iterator h2DIterBegin = m_histo2DVector.begin();
	  std::vector<H2DHisto>::const_iterator h2DIterEnd   = m_histo2DVector.end();  
	  for (h2DIter = h2DIterBegin; h2DIter != h2DIterEnd; h2DIter++) {
	    sprintf(msg,"%s-%s",fileName,h2DIter->h2D->GetTitle());
	    TH2F* newHisto = (TH2F*) h2DIter->h2D->Clone(msg);
	    newHisto       -> SetTitle(msg);
	  } // for h2DIter

	  std::vector<CounterHisto>::const_iterator counterIter;
	  std::vector<CounterHisto>::const_iterator counterIterBegin = m_counterVector.begin();    
	  std::vector<CounterHisto>::const_iterator counterIterEnd   = m_counterVector.end();
	  for (counterIter = counterIterBegin; counterIter != counterIterEnd; counterIter++){
	    sprintf(msg,"%s-%s",fileName,counterIter->h1DCumulative->GetTitle());
	    TH1F* newHisto = (TH1F*) counterIter->h1DCumulative->Clone(msg);
	    newHisto       -> SetTitle(msg);

	    sprintf(msg,"%s-%s",fileName,counterIter->h1DTrend->GetTitle());
	    TH1F* newHisto2 = (TH1F*) counterIter->h1DTrend->Clone(msg);
	    newHisto2       -> SetTitle(msg);
	  } //for counterIter
	  
	} // if zombie file	
	outFile->Write();
	outFile->Flush();	
	outFile->Close();
	delete outFile;
	//	delete outDir;
	cwd->cd();	
        break;
        
      case idPause:
        if (!m_connectOK) {
          m_connectOK = HPDGui::Connect2DIM();
          if (m_connectOK) {
            m_StatusBar->SetText("successfully connected to DIM");          
          } else {
            m_StatusBar->SetText("connection to DIM failed");          
          } // if m_connectOK          
        } // if !m_connectOK
        
        if (m_timerRuns) {
          HPDGui::Pause();          
        } else {
          HPDGui::Play();          
        } //if m_timerRuns
        break;

      case idSelect:
        if (m_timerRuns) {          
          HPDGui::Pause();
        } // if timer runs
        
        HPDGui::Reset();

        // read out the refresh times for the histogram and counters
        // obtained from DIM server
        m_refreshTimeHisto   = m_EntryRefreshTimeHisto   -> GetIntNumber();
        m_refreshTimeCounter = m_EntryRefreshTimeCounter -> GetIntNumber();        
        m_counterMin         = m_EntryCounterMin         -> GetIntNumber();
        m_counterMax         = m_EntryCounterMax         -> GetIntNumber();
        m_2DDrawOption       = m_Entry2DDrawOption       -> GetTextEntry() -> GetText();
        if (m_2DDrawOption.find("default",0) != std::string::npos) {
          m_2DDrawOption.clear();          
        } // if string find
        
        for (serviceIter = serviceIterBegin; serviceIter != serviceIterEnd;
             serviceIter++) {
          if ((*serviceIter)->IsChecked()) {
            // selected for display
            tmpString = (*serviceIter)->GetText();
            if ((tmpString.substr(0,3) == "H1D") || (tmpString.substr(0,3) == "H2D")) {          
              m_SelectedHistogramVector.push_back(*serviceIter);              
            }  else { 
              m_SelectedCounterVector.push_back(*serviceIter);              
            } // if H1D, H2D
          } // if isChecked
        } // for serviceIter

        HPDGui::SetupCanvas();
        m_StatusBar->SetText("ready to display");
        break;
        
        
      default:
        std::cout << "--> button with ID " << parm1 << " pressed - but not known.." << std::endl;
        break;
      } // switch parm1
    } // switch getSubMessage
  } // switch GetMessage
  

  return kTRUE;
  
} // bool ProcessMessage

// ------------------------------------------------------------------------------------------
void HPDGui::Reset() {

  // reset the list of previously selected items
  m_SelectedHistogramVector.clear();
  m_SelectedCounterVector.clear();

  std::vector<H2DHisto>::const_iterator h2DIter;
  std::vector<H2DHisto>::const_iterator h2DIterBegin = m_histo2DVector.begin();
  std::vector<H2DHisto>::const_iterator h2DIterEnd   = m_histo2DVector.end();
  for (h2DIter = h2DIterBegin; h2DIter != h2DIterEnd; h2DIter++){
    delete (*h2DIter).h2D;
    delete (*h2DIter).dimHisto;
    delete (*h2DIter).oldValue;    
  } //for
  m_histo2DVector.clear();



  std::vector<H1DHisto>::const_iterator h1DIter;
  std::vector<H1DHisto>::const_iterator h1DIterBegin = m_histo1DVector.begin();
  std::vector<H1DHisto>::const_iterator h1DIterEnd   = m_histo1DVector.end();
  for (h1DIter = h1DIterBegin; h1DIter != h1DIterEnd; h1DIter++){
    delete (*h1DIter).h1D;
    delete (*h1DIter).dimHisto;
    delete (*h1DIter).oldValue;    
  } //for
  m_histo1DVector.clear();

  std::vector<CounterHisto>::const_iterator counterIter;
  std::vector<CounterHisto>::const_iterator counterIterBegin;
  std::vector<CounterHisto>::const_iterator counterIterEnd;
  for (counterIter = counterIterBegin; counterIter != counterIterEnd; counterIter++){
    delete (*counterIter).h1DCumulative;
    delete (*counterIter).h1DTrend;
    delete (*counterIter).oldValue;
  } //for
  m_counterVector.clear();
        
} // void Reset
// ------------------------------------------------------------------------------------------
void HPDGui::CloseWindow() {

  std::cout << " terminating HPD monitor " << std::endl;
  
  gApplication->Terminate(0);
  
} // void CloseWindow
// ------------------------------------------------------------------------------------------
void HPDGui::Update() {

  //
  // update all selected 1D and 2D histograms
  //

  bool updateCanvas = false;

  //
  // 1D histograms
  //
  std::vector<H1DHisto>::const_iterator h1DIter;
  std::vector<H1DHisto>::const_iterator h1DIterBegin = m_histo1DVector.begin();
  std::vector<H1DHisto>::const_iterator h1DIterEnd   = m_histo1DVector.end();

  double oldValue = 0;
  double value    = 0;
  int    nBinsX   = 0;
  int    nBinsY   = 0;
  
  for (h1DIter = h1DIterBegin; h1DIter != h1DIterEnd; h1DIter++) {
    nBinsX   = (*h1DIter).h1D ->GetNbinsX();      
    value    = 0;
    oldValue = 0;
    
    for (int i=0; i< nBinsX+1; i++) {
      value     = (*h1DIter).dimHisto->get1DHisto()->GetBinContent(i);
      oldValue += value;      
      (*h1DIter).h1D->SetBinContent(i, value);
        
      value = (*h1DIter).dimHisto->get1DHisto()->GetBinError(i);
      (*h1DIter).h1D->SetBinError(i, value);
    } //for iBin
    
    oldValue  += (*h1DIter).h1D -> GetMean(1);
    
    if (fabs(oldValue - *(*h1DIter).oldValue) > 0.00001) {
      // use as estimate if histogram has changed
      // that the sum of all bins + mean has changed 
      updateCanvas = true;
      *(*h1DIter).oldValue = value;
    } // if oldValue
    
    
  } // for h1DIter
  
  //
  // 2D histograms
  //
  std::vector<H2DHisto>::const_iterator h2DIter;
  std::vector<H2DHisto>::const_iterator h2DIterBegin = m_histo2DVector.begin();
  std::vector<H2DHisto>::const_iterator h2DIterEnd   = m_histo2DVector.end();
  
  for (h2DIter = h2DIterBegin; h2DIter != h2DIterEnd; h2DIter++) {
    (*h2DIter).h2D -> SetStats(kFALSE);    
    nBinsX   = (*h2DIter).h2D -> GetNbinsX();
    nBinsY   = (*h2DIter).h2D -> GetNbinsY();      
    value    = 0.0;
    oldValue = 0.0;
    
    for (int i=0; i<= nBinsX+1; i++) {
      for (int j=0; j<= nBinsY+1; j++) {
        value     = (*h2DIter).dimHisto->get2DHisto()->GetBinContent(i,j);
        if (value == 0.0)
          value = 0.00001; // to show in nice colour
        (*h2DIter).h2D->SetBinContent(i,j,value);
        
        value = (*h2DIter).dimHisto->get2DHisto()->GetBinError(i,j);
        (*h2DIter).h2D->SetBinError(i,j,value);
      } // for j
    } //for i
    
    oldValue = (*h2DIter).h2D -> GetMean(1) * (*h2DIter).h2D -> GetMean(2);

    if (fabs(oldValue - *(*h2DIter).oldValue) > 0.00001) {
      // use as estimate if histogram has changed that  meanX*meanY has changed 
      updateCanvas = true;
      *(*h2DIter).oldValue = oldValue;
    
    } // if oldValue
    
  } // for h2DIter

  //
  // counters
  //  
  std::vector<CounterHisto>::const_iterator counterIter;
  std::vector<CounterHisto>::const_iterator counterIterBegin = m_counterVector.begin();    
  std::vector<CounterHisto>::const_iterator counterIterEnd   = m_counterVector.end();    
  float counterValue;
  
  for (counterIter = counterIterBegin; counterIter != counterIterEnd; counterIter++){
    
    oldValue = *(*counterIter).oldValue;
    
    if ((*counterIter).dimCounter -> getType() == DimInfoCounter::Integer)
      counterValue = (*counterIter).dimCounter->getIntValue();
    else if ((*counterIter).dimCounter ->getType() == DimInfoCounter::FloatingPoint)
      counterValue = (*counterIter).dimCounter->getFloatValue();

    // std::cout << "counter value " << counterValue << " old " << *(*counterIter).oldValue << std::endl;
    
    if (fabs(counterValue - oldValue) > 0.001) {
      // value changed - need to update histograms
      updateCanvas             = true;        
      *(*counterIter).oldValue = counterValue;
        
      //        // now rescale axis of cumulative histogram
      //        int   nBins = (*counterIter).h1DCumulative -> GetNbinsX();        
      //        float xMin  = (*counterIter).h1DCumulative -> GetXaxis() -> GetBinLowEdge(1);
      //        float xMax  = (*counterIter).h1DCumulative -> GetXaxis() -> GetBinUpEdge(nBins);
      //        if (counterValue < xMin) {          
      //          xMin = counterValue - counterValue/10.0;
      //          (*counterIter).h1DCumulative -> GetXaxis()->Set(nBins,xMin, xMax);
      //        } // if value < min
      //        
      //        if (counterValue > xMax) {          
      //          xMax = counterValue + counterValue/10.0;        
      //          (*counterIter).h1DCumulative -> GetXaxis()->Set(nBins,xMin, xMax);
      //        } // if value > max
      
      
      // fill value into histogram
      (*counterIter).h1DCumulative -> Fill(counterValue);
      
      // update the trend plot
      // n.b. bin 0: underflow, nbins+1 : overflow
      // start with bin 2, move everything one bin ahead and then
      // fill current value in last bin
      
      double binContent = 0;      
      for (int i=2; i<=m_nTimeSteps; i++) {
        binContent = (*counterIter).h1DTrend->GetBinContent(i);
        (*counterIter).h1DTrend->SetBinContent(i-1, binContent);        
      } //for i
      (*counterIter).h1DTrend->SetBinContent(m_nTimeSteps, counterValue);
    } // if fabs() - counter has changed
  } //for counterIter
  

  //
  // now update the canvas if necessary
  //
  
  if (updateCanvas) {
    int padCounter = 1;
      
    for (h2DIter = h2DIterBegin; h2DIter != h2DIterEnd; h2DIter++) {
      m_Canvas->GetPad(padCounter)->cd();
      m_Canvas->GetPad(padCounter)->SetFillColor(10);    
      (*h2DIter).h2D -> SetStats(kFALSE);
      (*h2DIter).h2D -> Draw(m_2DDrawOption.c_str());
      padCounter++;      
    } // for h2DIter
      
    for (h1DIter = h1DIterBegin; h1DIter != h1DIterEnd; h1DIter++) {
      m_Canvas->GetPad(padCounter)->cd();
      m_Canvas->GetPad(padCounter)->SetFillColor(10);            
      (*h1DIter).h1D -> SetStats(kFALSE);
      (*h1DIter).h1D -> Draw();      
        padCounter++;      
    } // for h1DIter

      
    for (counterIter = counterIterBegin; counterIter != counterIterEnd; counterIter++){
      // first the cumulative distribution
      m_Canvas->GetPad(padCounter)->cd();
      m_Canvas->GetPad(padCounter)->SetFillColor(10);            
      (*counterIter).h1DCumulative -> Draw();
      padCounter++;
        
      // then the trend plot
      m_Canvas->GetPad(padCounter)->cd();
      m_Canvas->GetPad(padCounter)->SetFillColor(10);            
      (*counterIter).h1DTrend->SetStats(kFALSE);
      (*counterIter).h1DTrend->Draw("P");
    } //for counterIter

  } // if updateCanvas
    

  if (updateCanvas)
    m_Canvas->Update();
  
  
} //void Update
// -----------------------------------------------------------------------------------------
bool HPDGui::Connect2DIM() {
  
  bool returnValue = false;
  
  //
  // DIM
  //
  DimBrowser dimBrowser;
  char       *dimService;
  char       *dimFormat;
  char       *dimServer;     // UTGID used by DIM to identify
                             // a particular server    
  char       *dimServerNode;
  int         dimType;       // 1: DIM service
                             // 2: DIM command
                             // 3: DIM RPC
  int         nDimServers;

  std::string            stringServer;
  std::string            stringService;
  std::string            stringFormat;  
  std::string::size_type stringLocation;
  

  //
  // remove all entries before starting over
  //
  m_ListTreeDimServices->DeleteChildren(m_ListTreeItemMain);
  m_ListTreeItemVector.clear();
  

  //
  // query the DIM service for available servers and
  // services
  //

  // get a list of all servers connected to current DIM DNS server
  nDimServers = dimBrowser.getServers();

  // if some servers found, discover which services
  // are there - otherwise return "fail"
  if (nDimServers > 0) {
    returnValue = true;    
  } else {
    return false;    
  }// if #DIM servers
  
  
  while (dimBrowser.getNextServer(dimServer, dimServerNode)) {
    stringServer = dimServer;
    // std::cout << "DIM server " << stringServer << " @ " << dimServerNode << std::endl;    
    // check if we found a DNS server and remove this from the list
    stringLocation = stringServer.find("DIS_DNS",0);
    if (stringLocation != std::string::npos) {
      // std::cout << " found DIM DNS at node " << dimServerNode << std::endl;
    } else {
      // std::cout << " found other DIM server " << std::endl;    
      m_DimServerNameVector.push_back(stringServer);
      TGListTreeItem *thisDimServer = m_ListTreeDimServices -> AddItem(m_ListTreeItemMain, dimServer);
      
      // now loop get a list of all services offered by this server
      dimBrowser.getServerServices(dimServer);
      while(dimType = dimBrowser.getNextServerService(dimService, dimFormat)) {
        stringService = dimService;
        stringFormat  = dimFormat;
        
        // std::cout << "DIM service " << dimService << " format " << dimFormat << std::endl;
        
        
        if (dimType != DimSERVICE) // only want DIM services
          continue;
        
        // every Gaucho service is complemented by a comment describing it - no need to list
        if (stringService.find("gauchocomment",0) != std::string::npos) {          
          continue;  
        }  // if gauchocomment                                                                           
        
        // cannot display "character" type formats, remove from the list
        if (stringFormat.find("C",0) != std::string::npos) {
          continue;          
        } // if character service
        
        
        // remove the server name from the service ID string
        stringService.replace(stringService.find(stringServer,0),stringServer.length(),"");
        stringService.replace(stringService.find_first_of("/",0),1,"");
        
        // add the services to the list
        TGListTreeItem *thisItem =  m_ListTreeDimServices -> AddItem(thisDimServer, stringService.c_str());        
        m_ListTreeItemVector.push_back(thisItem);
        m_ListTreeDimServices    -> SetCheckBox(thisItem, kTRUE);
        m_ListTreeDimServices    -> ToggleItem(thisItem);
        // void SetToolTipItem(TGListTreeItem *item, const char *string)  // can add GauchoComment later if wanted
        
      } //while dimServerService
    } // if stringLocation
  } //while dimServer

  return returnValue;
  
}// bool Connect2DIM
// -----------------------------------------------------------------------------------------
void HPDGui::SetTimer(TTimer* timer) {

  m_externalTimer = timer;
  
} // void SetTimer
// -----------------------------------------------------------------------------------------
void HPDGui::Pause() {

  m_StatusBar     -> SetText("pausing update");
  m_externalTimer -> TurnOff();
  m_timerRuns     =  false;
  m_ButtonPause   -> ChangeBackground(m_ROOTGreen);
  m_ButtonPause   -> SetText("&start  ");          

} // void Pause
// -----------------------------------------------------------------------------------------

void HPDGui::Play() {
  m_StatusBar     ->  SetText("resuming update");
  m_externalTimer ->  TurnOn();
  m_timerRuns     =   true;          
  m_ButtonPause   ->  ChangeBackground(m_ROOTYellow);
  m_ButtonPause   ->  SetText("&pause");

} // void Play
// -----------------------------------------------------------------------------------------
void HPDGui::SetupCanvas() {

  //
  // define canvas layout
  // N.B. : counters are always displayed with two histograms,
  //        one showing the inclusive distribution accumulated
  //        with time, the other a trend plot: value vs the
  //        last N time-steps
  // if only one histogram, use the whole canvas,
  // in all other cases, split canvas in lower and upper part.
  //

  // for histogram booking
  int    iHisto  = 0;  // counting all histograms booked
  int    nBinsX  = 0;
  int    nBinsY  = 0;
  double xMin    = 0;
  double xMax    = 0;
  double yMin    = 0;
  double yMax    = 0;

  int   iCounter = 0; // counting all histograms booked for counters
                      // n.b. 2 histograms per counter
  

  //
  // reset Canvas
  //
  m_Canvas->Divide(1,1);
  m_Canvas->Clear();
  

  //
  // book histograms to be displayed
  // retrieve properties from DIM service
  //

  if (m_SelectedHistogramVector.size() > 0) {    
    std::vector<TGListTreeItem *>::const_iterator histoIter;
    std::vector<TGListTreeItem *>::const_iterator histoIterBegin = m_SelectedHistogramVector.begin();
    std::vector<TGListTreeItem *>::const_iterator histoIterEnd   = m_SelectedHistogramVector.end();
    
    for (histoIter = histoIterBegin; histoIter != histoIterEnd; histoIter++){
      std::string serviceName = (*histoIter)->GetText();
      
      // sanity check: really histogram?
      if (!((serviceName.substr(0,3) == "H1D") || (serviceName.substr(0,3) == "H2D"))){
      std::cout << "HPDGui::SetupCanvas: not histogram - though should have been" << std::endl;
      continue;
      } //if not histgram
      
      iHisto++;
      char histoID[100];
      sprintf(histoID,"histo_%i",iHisto);          
      
      // insert name of GaudiAlg again into string identifying the object
      std::string parentName = (*histoIter)->GetParent()->GetText();
      parentName.append("/");    
      serviceName.insert(4,parentName);

      if (serviceName.substr(0,3) == "H1D") {
        H1DHisto histo1D;
        histo1D.dimHisto = new DimInfoHisto(serviceName, m_refreshTimeHisto);
        if (histo1D.dimHisto->serviceOK()) {    
          TH1* tmp_h1D = histo1D.dimHisto->get1DHisto();
          if (tmp_h1D) {
            nBinsX = tmp_h1D->GetNbinsX();
            xMin   = tmp_h1D->GetXaxis()->GetXmin();
            xMax   = tmp_h1D->GetXaxis()->GetXmax();
            //std::cout << " nBinsX " << nBinsX << " xMin " << xMin << " xMax " << xMax
            //          << std::endl;
            // book the 1D histo which is to be displayed
            histo1D.h1D       = new TH1F(histoID, serviceName.c_str(), nBinsX, xMin, xMax);
            histo1D.h1D      -> SetMarkerStyle(22);
            histo1D.h1D      -> SetMarkerSize(0.9);
            histo1D.oldValue  = new double;            
            *histo1D.oldValue = -999;
            m_histo1DVector.push_back(histo1D);
          } // if tmp_h1d
        } // if serviceOK
        
      } else if (serviceName.substr(0,3) == "H2D") {              
        H2DHisto histo2D;
        histo2D.dimHisto = new DimInfoHisto(serviceName, m_refreshTimeHisto);
        if (histo2D.dimHisto->serviceOK()) {
          TH2* tmp_h2D = histo2D.dimHisto->get2DHisto();
          if (tmp_h2D) {
            // dimHisto gives back a NULL pointer if something went wrong
            nBinsX = tmp_h2D->GetNbinsX();
            nBinsY = tmp_h2D->GetNbinsY();
            xMin   = tmp_h2D->GetXaxis()->GetXmin();
            xMax   = tmp_h2D->GetXaxis()->GetXmax();
            yMin   = tmp_h2D->GetYaxis()->GetXmin();
            yMax   = tmp_h2D->GetYaxis()->GetXmax();
            //std::cout << " nBinsX " << nBinsX << " xMin " << xMin << " xMax " << xMax 
            //          << " nBinsY " << nBinsY << " yMin " << yMin << " yMax " << yMax
            //          << std::endl;
            
            // book new TH2F - the one which is to be displayed
            histo2D.h2D = new TH2F(histoID, serviceName.c_str(), 
                                   nBinsX, xMin, xMax,
                                   nBinsY, yMin, yMax);
            histo2D.oldValue    = new double;            
            *(histo2D.oldValue) = -999;
            m_histo2DVector.push_back(histo2D);
          } // if tmp_h2D
          
        } // if serviceName = H1D/H2D
        
      } //if serviceOK          
      
    } //for histoIter
  } // if selectedHisto.size() > 0
  

  //
  // book histograms for counters obtained from DIM
  //  
  if (m_SelectedCounterVector.size() > 0) {
    std::vector<TGListTreeItem *>::const_iterator counterIter;
    std::vector<TGListTreeItem *>::const_iterator counterIterBegin = m_SelectedCounterVector.begin();
    std::vector<TGListTreeItem *>::const_iterator counterIterEnd   = m_SelectedCounterVector.end();
    char counterID[5000];

    for (counterIter = counterIterBegin; counterIter != counterIterEnd; counterIter++){
      std::string serviceName = (*counterIter)->GetText();
      // sanity check: really histogram?
      if (((serviceName.substr(0,3) == "H1D") || (serviceName.substr(0,3) == "H2D"))){
        std::cout << "HPDGui::SetupCanvas: histogram - though should have been" << std::endl;
        continue;
      } //if not histgram
      
      // insert name of GaudiAlg again into string identifying the object
      std::string parentName = (*counterIter)->GetParent()->GetText();
      parentName.append("/");    
      serviceName.insert(0,parentName);

      std::string histoName;
      // new instance of struct
      HPDGui::CounterHisto counterHisto;
      counterHisto.dimCounter = new DimInfoCounter(serviceName, m_refreshTimeCounter);    

      if (counterHisto.dimCounter -> serviceOK()){
        counterHisto.oldValue = new float;        
        *(counterHisto.oldValue) = -999; // set to some unlikely value

        // book 1D histo - cumulative distribution
        iCounter++;
        sprintf(counterID,"counter_%i",iCounter);

        histoName = serviceName;
        histoName.append(" - cumulative ");        
        // make large enough - rescaling afterwards possible (and done)
        // but seems to lead to some artifact with overflows
        int nBins = abs(m_counterMax - m_counterMin);
        counterHisto.h1DCumulative =  new TH1F(counterID, histoName.c_str(), nBins, m_counterMin, m_counterMax);

        // book 1D histo - trend
        iCounter++;
        sprintf(counterID,"counter_%i",iCounter);
        histoName = serviceName;
        histoName.append(" - trend ");
        counterHisto.h1DTrend      =  new TH1F(counterID, histoName.c_str(), m_nTimeSteps, 1, m_nTimeSteps);
        counterHisto.h1DTrend      -> SetMarkerStyle(22);
        counterHisto.h1DTrend      -> SetMarkerSize(0.7);
        counterHisto.h1DTrend      -> SetStats(kFALSE);

        m_counterVector.push_back(counterHisto);
        
      } // if service OK
      
      
    } // for counterIter
  } // m_SelectedCounterVector > 0
  

  //
  // setup the Canvas and draw the (empty) histograms
  // choose a setup where #rows = #columns
  // may lead to some unsused parts of the canvas, but 
  // probably the easiest/best setup if no further
  // knowledge about the contents displayed
  //

  // #pads needed to display all information
  int nPads     = m_SelectedHistogramVector.size() +  2*m_SelectedCounterVector.size();
  
  m_nCanvasRows    = (int) ceil(sqrt((double)nPads));
  m_nCanvasColumns = m_nCanvasRows;  

  m_Canvas -> Divide(m_nCanvasColumns,m_nCanvasRows);

  int padCounter    = 1;
  // now loop over all pads to set basic properties
  for (int i = 1; i<= m_nCanvasRows; i++) {
    for (int j = 1; j <= m_nCanvasColumns; j++) {
      m_Canvas->GetPad(padCounter)->SetFillColor(10);
      m_Canvas->GetPad(padCounter)->Draw();      
      padCounter++;      
    } //for j    
  } //for i
  
  
  // now loop over all histograms and draw them: 
  // order: 2D - 1D - Counters

  padCounter = 1;
  std::vector<H2DHisto>::const_iterator h2DIter;
  std::vector<H2DHisto>::const_iterator h2DIterBegin = m_histo2DVector.begin();
  std::vector<H2DHisto>::const_iterator h2DIterEnd   = m_histo2DVector.end();    
  for (h2DIter = h2DIterBegin; h2DIter != h2DIterEnd; h2DIter++) {
    m_Canvas->GetPad(padCounter)->cd();
    m_Canvas->GetPad(padCounter)->SetFillColor(10);
    (*h2DIter).h2D -> SetStats(kFALSE);
    (*h2DIter).h2D -> Draw(m_2DDrawOption.c_str());
    padCounter++;          

  } // for h2DIter

  std::vector<H1DHisto>::const_iterator h1DIter;
  std::vector<H1DHisto>::const_iterator h1DIterBegin = m_histo1DVector.begin();
  std::vector<H1DHisto>::const_iterator h1DIterEnd   = m_histo1DVector.end();    
  for (h1DIter = h1DIterBegin; h1DIter != h1DIterEnd; h1DIter++) {
    m_Canvas->GetPad(padCounter)->cd();
    m_Canvas->GetPad(padCounter)->SetFillColor(10);    
    (*h1DIter).h1D -> SetStats(kFALSE);
    (*h1DIter).h1D -> Draw();      
    padCounter++;      
  } // for h1DIter
    
  std::vector<CounterHisto>::const_iterator counterIter;
  std::vector<CounterHisto>::const_iterator counterIterBegin = m_counterVector.begin();    
  std::vector<CounterHisto>::const_iterator counterIterEnd   = m_counterVector.end();    
  for (counterIter = counterIterBegin; counterIter != counterIterEnd; counterIter++){
    m_Canvas->GetPad(padCounter)->cd();
    m_Canvas->GetPad(padCounter)->SetFillColor(10);
    (*counterIter).h1DCumulative -> Draw();
    padCounter++;
    
    // then the trend plot
    m_Canvas->GetPad(padCounter)->cd();
    m_Canvas->GetPad(padCounter)->SetFillColor(10);
    (*counterIter).h1DTrend->SetStats(kFALSE);
    (*counterIter).h1DTrend->Draw("P");
    padCounter++;
    
  } //for counterIter
  
  m_Canvas         -> Update();
  
  
  
} // void SetupCanvas


// -----------------------------------------------------------------------------------------
