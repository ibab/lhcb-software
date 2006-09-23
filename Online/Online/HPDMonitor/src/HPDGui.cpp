// $Id: HPDGui.cpp,v 1.23 2006-09-23 15:44:20 ukerzel Exp $
// Include files 

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>


// BOOST
#include "boost/lexical_cast.hpp"

//ROOT
#include <TROOT.h>
#include <TApplication.h>
#include <TColor.h>
#include <TStyle.h>
#include <TFile.h>
#include <TGaxis.h>

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
  m_ZoomCanvasActive(false),
  m_nTimeSteps(100),
  m_refreshTimeHisto(1),
  m_refreshTimeCounter(1),
  m_counterMin(0),
  m_counterMax(50),
  m_2DDrawOption(""),
  m_1DDrawOption(""),
  m_nCanvasRows(0),
  m_nCanvasColumns(0),
  m_connectOK(false),
  m_verbose(0)
{
  //
  // check if environment variable pointing to DIM DNS server is set
  //
  char *dnsNode = getenv("DIM_DNS_NODE");
  if (dnsNode) {    
    if (m_verbose > 0) 
      std::cout << "DIM DNS node environment variable set to "  << dnsNode << std::endl;
  } else {
    std::cout << "Please set environment variable DIM_DNS_NODE to correct value"
              << std::endl;
    exit (-1);
  } // if dnsNode
  
  
  //
  // main GUI
  //

  // for the main canvas showing the histograms
  const int canvasWidth    = (int) (guiWidth  - 3.0*floor((double)guiWidth/10.0));
  const int canvasHeight   = (int) (guiHeight - 0.5*floor((double)guiHeight/10.0));  
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
  m_CompositeFrameMaster     = new TGCompositeFrame(this,   1,   1,    kVerticalFrame);
  AddFrame(m_CompositeFrameMaster, m_LayoutLeftTop);                

  m_CompositeFrameMain       =  new TGCompositeFrame(m_CompositeFrameMaster, 1, 1, kHorizontalFrame);  
  m_CompositeFrameMaster     -> AddFrame(m_CompositeFrameMain, m_LayoutTopLeft);

  m_CompositeFrameButtons    =  new TGCompositeFrame(m_CompositeFrameMain, 1, 1, kVerticalFrame);  
  m_CompositeFrameMain       -> AddFrame(m_CompositeFrameButtons, m_LayoutTopRight);

  m_GroupFrameListTree       =  new TGGroupFrame(m_CompositeFrameButtons, "DIM services", kHorizontalFrame);

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
  m_StatusBar            -> SetText("Gaudi Online monitor initalised");  
  m_CompositeFrameMaster -> AddFrame(m_StatusBar, m_LayoutBottomLeft);

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
  m_EntryRefreshTimeHisto       =  new TGNumberEntry(m_GroupFrameHPDControl, 1, 3, -1, 
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
                                                  TGNumberFormat::kNESReal,
                                                  TGNumberFormat::kNEAAnyNumber, 
                                                  TGNumberFormat::kNELLimitMinMax, -10000, 10000);  
  m_GroupFrameHPDControl     -> AddFrame(m_EntryCounterMin  , m_LayoutTopLeft);

  // entry field for counter max. value
  m_stringCounterMax         =  new TGHotString("counter max.");
  m_labelCounterMax          =  new TGLabel(m_GroupFrameHPDControl, m_stringCounterMax);
  m_GroupFrameHPDControl     -> AddFrame(m_labelCounterMax, m_LayoutTopLeftExpandX);
  m_EntryCounterMax          =  new TGNumberEntry(m_GroupFrameHPDControl, 50, 5, -1, 
                                                  TGNumberFormat::kNESReal,
                                                  TGNumberFormat::kNEAAnyNumber, 
                                                  TGNumberFormat::kNELLimitMinMax, -10000, 10000);  
  m_GroupFrameHPDControl     -> AddFrame(m_EntryCounterMax  , m_LayoutTopLeft);

  // entry field for max digits displayed on axis (before swithching to 10^n)
  m_stringAxisMaxDigits      =  new TGHotString("axis digit max.");  
  m_labelAxisMaxDigits       =  new TGLabel(m_GroupFrameHPDControl, m_stringAxisMaxDigits);
  m_GroupFrameHPDControl     -> AddFrame(m_labelAxisMaxDigits, m_LayoutTopLeftExpandX);  
  m_EntryAxisMaxDigits       =  new TGNumberEntry(m_GroupFrameHPDControl, 3, 5, -1,
                                                  TGNumberFormat::kNESInteger,
                                                  TGNumberFormat::kNEAPositive,
                                                  TGNumberFormat::kNELLimitMinMax, 0, 10);
  m_GroupFrameHPDControl     -> AddFrame(m_EntryAxisMaxDigits  , m_LayoutTopLeft);
  

  // entry for draw option used for 2D histogram
  m_string2DDrawOption       =  new TGHotString("2D draw option");
  m_label2DDrawOption        =  new TGLabel(m_GroupFrameHPDControl, m_string2DDrawOption);
  m_GroupFrameHPDControl     -> AddFrame(m_label2DDrawOption, m_LayoutTopLeftExpandX);
  m_Entry2DDrawOption        =  new TGComboBox(m_GroupFrameHPDControl, "option", id2DDrawOption);
  m_Entry2DDrawOption        -> Resize(80,20);
  m_Entry2DDrawOption        -> AddEntry("default", id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("box"    , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("box1"   , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("colz"   , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("surf"   , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("surf2"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("surf3"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("surf4"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("lego"   , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("lego1"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("lego2"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("cont"   , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("cont2"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("cont3"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("cont4"  , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("CYL"    , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("POL"    , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("SPH"    , id2DDrawOption);
  m_Entry2DDrawOption        -> AddEntry("PSR"    , id2DDrawOption);
  m_GroupFrameHPDControl     -> AddFrame(m_Entry2DDrawOption,  m_LayoutTopLeft);

  // entry for draw option used for 2D histogram
  m_string1DDrawOption       =  new TGHotString("1D draw option");
  m_label1DDrawOption        =  new TGLabel(m_GroupFrameHPDControl, m_string1DDrawOption);
  m_GroupFrameHPDControl     -> AddFrame(m_label1DDrawOption, m_LayoutTopLeftExpandX);
  m_Entry1DDrawOption        =  new TGComboBox(m_GroupFrameHPDControl, "option", id1DDrawOption);
  m_Entry1DDrawOption        -> Resize(80,20);
  m_Entry1DDrawOption        -> AddEntry("HIST", id1DDrawOption);
  m_Entry1DDrawOption        -> AddEntry("P"   , id1DDrawOption);
  m_Entry1DDrawOption        -> AddEntry("PE"  , id1DDrawOption);
  m_Entry1DDrawOption        -> AddEntry("P0E" , id1DDrawOption);
  m_Entry1DDrawOption        -> AddEntry("PE0" , id1DDrawOption);
  m_Entry1DDrawOption        -> AddEntry("PE1" , id1DDrawOption);
  m_Entry1DDrawOption        -> AddEntry("PE2" , id1DDrawOption);
  m_Entry1DDrawOption        -> AddEntry("PE3" , id1DDrawOption);
  m_Entry1DDrawOption        -> AddEntry("PE4" , id1DDrawOption);
  m_Entry1DDrawOption        -> AddEntry("L"   , id1DDrawOption);
  m_Entry1DDrawOption        -> AddEntry("C"   , id1DDrawOption);
  m_Entry1DDrawOption        -> AddEntry("B"   , id1DDrawOption);
  m_GroupFrameHPDControl     -> AddFrame(m_Entry1DDrawOption,  m_LayoutTopLeft);
  

  // entry field used to determine whether the stat box should be shown or not
  m_stringStats              =  new TGHotString("show stats");
  m_labelStats               =  new TGLabel(m_GroupFrameHPDControl,m_stringStats);
  m_GroupFrameHPDControl     -> AddFrame(m_labelStats,m_LayoutTopLeftExpandX);
  m_EntryStats               =  new TGComboBox(m_GroupFrameHPDControl, "option", idStats);
  m_EntryStats               -> Resize(80,20);
  m_EntryStats               -> AddEntry("None"    , idStats);
  m_EntryStats               -> AddEntry("1D"      , idStats);
  m_EntryStats               -> AddEntry("2D"      , idStats);
  m_EntryStats               -> AddEntry("1D + 2D" , idStats);
  m_GroupFrameHPDControl     -> AddFrame(m_EntryStats,  m_LayoutTopLeft);

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
  //                                                                    frame  ,          #rows #colums separator
  m_GroupFrameGuiControl     -> SetLayoutManager(new TGMatrixLayout(m_GroupFrameGuiControl,  1,   4,      2));
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
  
  // define "zoom" button opening extra canvas displaying 
  // the histogram the mouse is currently hovering over
  m_ButtonZoom              =  new TGTextButton(m_GroupFrameGuiControl, "&detail", idZoom);
  m_ButtonZoom              -> Associate(this);
  m_GroupFrameGuiControl    -> AddFrame(m_ButtonZoom,  m_LayoutBottomRight);

  // define "exit" button
  m_ButtonExit              =  new TGTextButton(m_GroupFrameGuiControl, "&exit ",idExit);
  m_ButtonExit              -> Associate(this);  
  m_ButtonExit              -> ChangeBackground(m_ROOTRed);  
  m_GroupFrameGuiControl    -> AddFrame(m_ButtonExit , m_LayoutBottomRight);

  
  //
  // now buid up GUI using methods of the base class
  //
  MapSubwindows();
  
  Layout();
  SetWindowName("Gaudi Online Monitor");
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

  delete m_StatusBar;
  delete m_EmbeddedCanvas;          
  delete m_Canvas;
  delete m_CanvasZoom;
  delete m_CompositeFrameMaster;  
  delete m_CompositeFrameMain;    
  delete m_CompositeFrameButtons; 
  delete m_GroupFrameHPDControl;  
  delete m_GroupFrameGuiControl;  
  delete m_GroupFrameListTree;    
  delete m_ListTreeDimServices; 
  delete m_CanvasListTree;       
  delete m_ListTreeItemMain;     

  delete m_ButtonConnect;  
  delete m_ButtonExit; 
  delete m_ButtonPrint;
  delete m_ButtonPause;
  delete m_ButtonSelect;  
  delete m_ButtonZoom;

  delete m_EntryRefreshTimeHisto;
  delete m_EntryRefreshTimeCounter;
  delete m_EntryCounterMin; 
  delete m_EntryCounterMax; 
  delete m_EntryAxisMaxDigits; 
  delete m_Entry2DDrawOption;
  delete m_Entry1DDrawOption;
  delete m_EntryStats;

  delete m_labelRefreshTimeHisto;
  delete m_labelRefreshTimeCounter;
  delete m_labelCounterMin;
  delete m_labelCounterMax;
  delete m_labelAxisMaxDigits;
  delete m_label2DDrawOption;
  delete m_label1DDrawOption;
  delete m_labelStats;
         
  delete m_stringRefreshTimeHisto;
  delete m_stringRefreshTimeCounter;
  delete m_stringCounterMin;
  delete m_stringCounterMax;
  delete m_stringAxisMaxDigits;
  delete m_string2DDrawOption;
  delete m_string1DDrawOption;
  delete m_stringStats;
         
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
  // local variables
  //
  std::string timeString;
  std::string fileName;
  std::string statusMessage;
  std::string tmpString;

  //
  // handles all the events/messages sent from the buttons, etc
  //

  std::vector<TGListTreeItem *>::const_iterator serviceIter;
  std::vector<TGListTreeItem *>::const_iterator serviceIterBegin = m_ListTreeItemVector.begin();
  std::vector<TGListTreeItem *>::const_iterator serviceIterEnd   = m_ListTreeItemVector.end();


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
        break;
        
      case idExit:
        HPDGui::CloseWindow();
        break;

      case idPrint:        
        time_t rawtime; // get current time
        tm * ptm;
        time ( &rawtime );
        ptm = gmtime ( &rawtime );
        timeString = 
          boost::lexical_cast<std::string>(ptm->tm_year+1900) +"-"+
          boost::lexical_cast<std::string>(ptm->tm_mon+1)     +"-"+
          boost::lexical_cast<std::string>(ptm->tm_mday)      +"_"+
          boost::lexical_cast<std::string>(ptm->tm_hour)      +"-"+
          boost::lexical_cast<std::string>(ptm->tm_min)       +"-"+
          boost::lexical_cast<std::string>(ptm->tm_sec);
        if (m_verbose > 0)
          std::cout << "print at time " << timeString << std::endl;
        
        statusMessage = "printing current snapshot at " + timeString;
        m_StatusBar->SetText(statusMessage.c_str());
        
        fileName = "monitor_" + timeString + ".eps";
        m_Canvas->Print(fileName.c_str());
        
        fileName = "monitor_" + timeString + ".jpg";
        m_Canvas->Print(fileName.c_str());
        
        // save as ROOT file
        outFile   = new TFile("monitoring.root","UPDATE");	
        tmpString = "monitor_" + timeString;
        outDir    = new TDirectory(tmpString.c_str(),tmpString.c_str());
        outDir->cd();
        
        if (outFile->IsZombie()){
          m_StatusBar->SetText("Error opening ROOT file");
        } else {
          std::vector<H1DHisto>::const_iterator h1DIter;
          std::vector<H1DHisto>::const_iterator h1DIterBegin = m_histo1DVector.begin();
          std::vector<H1DHisto>::const_iterator h1DIterEnd   = m_histo1DVector.end();
          for (h1DIter = h1DIterBegin; h1DIter != h1DIterEnd; h1DIter++) {
            tmpString      = "monitor_" + timeString + h1DIter->h1D->GetTitle();
            TH1F* newHisto = (TH1F*) h1DIter->h1D->Clone(tmpString.c_str());
            newHisto       -> SetTitle(tmpString.c_str());	    
          } // for h1DIter
          
          
          std::vector<H2DHisto>::const_iterator h2DIter;
          std::vector<H2DHisto>::const_iterator h2DIterBegin = m_histo2DVector.begin();
          std::vector<H2DHisto>::const_iterator h2DIterEnd   = m_histo2DVector.end();  
          for (h2DIter = h2DIterBegin; h2DIter != h2DIterEnd; h2DIter++) {
            tmpString      = "monitor_" + timeString + h2DIter->h2D->GetTitle();      
            TH2F* newHisto = (TH2F*) h2DIter->h2D->Clone(tmpString.c_str());
            newHisto       -> SetTitle(tmpString.c_str());
          } // for h2DIter
          
          std::vector<CounterHisto>::const_iterator counterIter;
          std::vector<CounterHisto>::const_iterator counterIterBegin = m_counterVector.begin();    
          std::vector<CounterHisto>::const_iterator counterIterEnd   = m_counterVector.end();
          for (counterIter = counterIterBegin; counterIter != counterIterEnd; counterIter++) {
            tmpString      = "monitor_" + timeString + counterIter->h1DCumulative->GetTitle();
            TH1F* newHisto = (TH1F*) counterIter->h1DCumulative->Clone(tmpString.c_str());
            newHisto       -> SetTitle(tmpString.c_str());
            
            tmpString       = "monitor_" + timeString + counterIter->h1DTrend->GetTitle();
            TH1F* newHisto2 = (TH1F*) counterIter->h1DTrend->Clone(tmpString.c_str());
            newHisto2       -> SetTitle(tmpString.c_str());
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
        if (!m_connectOK) 
          m_connectOK = HPDGui::Connect2DIM();
        
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
        m_1DDrawOption       = m_Entry1DDrawOption       -> GetTextEntry() -> GetText();
        
        for (serviceIter = serviceIterBegin; serviceIter != serviceIterEnd;
             serviceIter++) {
          if ((*serviceIter)->IsChecked()) {
            // selected for display
            tmpString = (*serviceIter)->GetParent()->GetText();
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

      case idZoom:
        m_ZoomCanvasActive = !m_ZoomCanvasActive;        
        if (m_ZoomCanvasActive) {
          // create new canvas
          if (!m_CanvasZoom)
            m_CanvasZoom =  new TCanvas();
        } else {
          //delete extra canvas
          m_CanvasZoom->Close(); // this should delete the canvas but leaves the 
                                 // address in place          
          m_CanvasZoom = NULL;
        }
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


  if (m_verbose > 0)
    std::cout << "reset method of HPDGui called" << std::endl;
  
  // reset the list of previously selected items
  m_SelectedHistogramVector.clear();
  m_SelectedCounterVector.clear();

  //
  // N.B. "delete" of dimHisto causes crash
  //      on Windows -> need to follow up
  //      how to dispose of ROOT histograms properly
  //

  if (m_verbose > 1)
    std::cout << "delete 2D histograms " << std::endl;  
  std::vector<H2DHisto>::const_iterator h2DIter;
  std::vector<H2DHisto>::const_iterator h2DIterBegin = m_histo2DVector.begin();
  std::vector<H2DHisto>::const_iterator h2DIterEnd   = m_histo2DVector.end();
  for (h2DIter = h2DIterBegin; h2DIter != h2DIterEnd; h2DIter++){
    delete (*h2DIter).h2D;
    delete (*h2DIter).dimHisto;
    delete (*h2DIter).oldValue;    
  } //for
  m_histo2DVector.clear();


  if (m_verbose > 1)
    std::cout << "delete 1D histograms" << std::endl;  
  std::vector<H1DHisto>::const_iterator h1DIter;
  std::vector<H1DHisto>::const_iterator h1DIterBegin = m_histo1DVector.begin();
  std::vector<H1DHisto>::const_iterator h1DIterEnd   = m_histo1DVector.end();
  for (h1DIter = h1DIterBegin; h1DIter != h1DIterEnd; h1DIter++){
    delete (*h1DIter).h1D;
    delete (*h1DIter).dimHisto;
    delete (*h1DIter).oldValue;    
  } //for
  m_histo1DVector.clear();

  if (m_verbose > 1)
    std::cout << "delete counters" << std::endl;  
  std::vector<CounterHisto>::const_iterator counterIter;
  std::vector<CounterHisto>::const_iterator counterIterBegin = m_counterVector.begin();
  std::vector<CounterHisto>::const_iterator counterIterEnd   = m_counterVector.end();
  for (counterIter = counterIterBegin; counterIter != counterIterEnd; counterIter++){
    delete (*counterIter).h1DCumulative;
    delete (*counterIter).h1DTrend;
    delete (*counterIter).oldValue;
  } //for
  m_counterVector.clear();
        
} // void Reset
// ------------------------------------------------------------------------------------------
void HPDGui::CloseWindow() {

  std::cout << " terminating Gaudi Online  monitor " << std::endl;
  
  gApplication->Terminate(0);
  
} // void CloseWindow
// ------------------------------------------------------------------------------------------
void HPDGui::Update() {


  if (m_verbose > 1 )
    std::cout << "update canvas " << std::endl;

  //
  // read out draw options
  //
  m_2DDrawOption       = m_Entry2DDrawOption       -> GetTextEntry() -> GetText();
  if (m_2DDrawOption.find("default",0) != std::string::npos) {
    m_2DDrawOption.clear();          
  } // if string find
  m_1DDrawOption       = m_Entry1DDrawOption       -> GetTextEntry() -> GetText();

  int maxAxisDitgs = m_EntryAxisMaxDigits-> GetIntNumber();  
  if (m_verbose > 1)
    std::cout << "setting max axis digits to " << maxAxisDitgs << std::endl;
  TGaxis::SetMaxDigits(maxAxisDitgs);

  HPDGui::histStatOption statOption = HPDGui::DetermineStatOption();


  //
  // update all selected 1D and 2D histograms
  //

  bool updateCanvas = false;
  updateCanvas = true; // always update

  bool histosOK     = true; // by default assume histograms
                            // obtained from DIM are OK
  

  /**
   * some code below is commented out which should determine whether
   * or not to update the canvas. This is done such that the values
   * of the counters and histograms are examined and an update
   * flag is set when these change. However, with static test-patterns, etc.
   * this logic does not work and is hence currently commented out - forcing
   * always an update.
   */
  
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
    if ((*h1DIter).dimHisto->serviceOK()) {

      (*h1DIter).h1D ->Reset();      
      nBinsX   = (*h1DIter).h1D ->GetNbinsX();      
      value    = 0;
      oldValue = 0;

      double nEntries = (*h1DIter).dimHisto->get1DHisto()->GetEntries();

      if (statOption == HPDGui::stat1D ||
          statOption == HPDGui::stat1D2D) {
        (*h1DIter).h1D->SetStats(kTRUE);
      } else {
        (*h1DIter).h1D->SetStats(kFALSE);
      } // if statOption

      for (int i=0; i< nBinsX+1; i++) {
        value     = (*h1DIter).dimHisto->get1DHisto()->GetBinContent(i);
        oldValue += value;      
        (*h1DIter).h1D->SetBinContent(i, value);
        
        value = (*h1DIter).dimHisto->get1DHisto()->GetBinError(i);
        (*h1DIter).h1D->SetBinError(i, value);
      } //for iBin

      (*h1DIter).h1D -> SetEntries(nEntries);
      
      if (m_verbose > 0)
        std::cout << "DIM histo entries " << nEntries
                  << " histo entries " << (*h1DIter).h1D->GetEntries()
                  << std::endl;
      
      //    oldValue  += (*h1DIter).h1D -> GetMean(1);
      //    
      //    if (fabs(oldValue - *(*h1DIter).oldValue) > 0.00001) {
      //      // use as estimate if histogram has changed
      //      // that the sum of all bins + mean has changed 
      //      updateCanvas = true;
      //      *(*h1DIter).oldValue = value;
      //    } // if oldValue

    } else  { // if serviceOK
      histosOK = false;
      std::string hTitle = (*h1DIter).h1D->GetTitle();      
      if (m_verbose > 1)
        std::cout << "H1D not OK " << hTitle << std::endl;
      HPDGui::Pause(); // pause updates
      std::string message = "1D histogram " + hTitle + " not OK, stopping update";      
      m_StatusBar     -> SetText(message.c_str());
      return;      
    } // if serviceOK    
  } // for h1DIter
  
  //
  // 2D histograms
  //
  std::vector<H2DHisto>::const_iterator h2DIter;
  std::vector<H2DHisto>::const_iterator h2DIterBegin = m_histo2DVector.begin();
  std::vector<H2DHisto>::const_iterator h2DIterEnd   = m_histo2DVector.end();
  
  for (h2DIter = h2DIterBegin; h2DIter != h2DIterEnd; h2DIter++) {

    if ((*h2DIter).dimHisto->serviceOK()) {
      (*h2DIter).h2D->Reset();      

      double nEntries = (*h2DIter).dimHisto->get2DHisto()->GetEntries();

      if (statOption == HPDGui::stat2D ||
          statOption == HPDGui::stat1D2D) {
        (*h2DIter).h2D->SetStats(kTRUE);
      } else {
        (*h2DIter).h2D->SetStats(kFALSE);
      } // if statOption      
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

      (*h2DIter).h2D->SetEntries(nEntries);

      if (m_verbose > 0)
        std::cout << "DIM histo entries " << nEntries
                  << " histo entries " << (*h2DIter).h2D->GetEntries()
                  << std::endl;

    
      //    oldValue = (*h2DIter).h2D -> GetMean(1) * (*h2DIter).h2D -> GetMean(2);
      //
      //    if (fabs(oldValue - *(*h2DIter).oldValue) > 0.00001) {
      //      // use as estimate if histogram has changed that  meanX*meanY has changed 
      //      updateCanvas = true;
      //      *(*h2DIter).oldValue = oldValue;
      //    
      //    } // if oldValue
      
    } else { // if serviceOK
      histosOK = false;
      std::string hTitle = (*h2DIter).h2D->GetTitle();      
      if (m_verbose > 1)
        std::cout << "H2D not OK " << hTitle << std::endl;
      HPDGui::Pause(); // pause updates
      std::string message = "2D histogram " + hTitle + " not OK, stopping update";
      m_StatusBar     -> SetText(message.c_str());
      return;      
    } // if serviceOK

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
    else if ((*counterIter).dimCounter ->getType() == DimInfoCounter::Float)
      counterValue = (*counterIter).dimCounter->getFloatValue();
    else if ((*counterIter).dimCounter ->getType() == DimInfoCounter::Double)
      counterValue = (*counterIter).dimCounter->getDoubleValue();
    if (m_verbose > 1)
      std::cout << "counter value " << counterValue << " old " << *(*counterIter).oldValue << std::endl;
    
//    if (fabs(counterValue - oldValue) > 0.001) {
//      // value changed - need to update histograms
//      updateCanvas             = true;        
//      *(*counterIter).oldValue = counterValue;
        
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
//    } // if fabs() - counter has changed
  } //for counterIter
  

  //
  // now update the canvas if necessary
  //

  updateCanvas = true; // always update canvas  
  if (updateCanvas) {
    int padCounter = 1;
      
    for (h2DIter = h2DIterBegin; h2DIter != h2DIterEnd; h2DIter++) {
      m_Canvas->GetPad(padCounter)->cd();
      m_Canvas->GetPad(padCounter)->SetFillColor(10);    
      (*h2DIter).h2D -> Draw(m_2DDrawOption.c_str());
      padCounter++;      
    } // for h2DIter
      
    for (h1DIter = h1DIterBegin; h1DIter != h1DIterEnd; h1DIter++) {
      m_Canvas->GetPad(padCounter)->cd();
      m_Canvas->GetPad(padCounter)->SetFillColor(10);            
      (*h1DIter).h1D -> Draw(m_1DDrawOption.c_str());      
      padCounter++;      
    } // for h1DIter

      
    for (counterIter = counterIterBegin; counterIter != counterIterEnd; counterIter++){
      // first the cumulative distribution
      m_Canvas->GetPad(padCounter) -> cd();
      m_Canvas->GetPad(padCounter) -> SetFillColor(10);
      (*counterIter).h1DCumulative -> SetStats(kTRUE);      
      (*counterIter).h1DCumulative -> Draw();
      padCounter++;
        
      // then the trend plot
      m_Canvas->GetPad(padCounter)->cd();
      m_Canvas->GetPad(padCounter)->SetFillColor(10);            
      (*counterIter).h1DTrend->SetStats(kFALSE);
      (*counterIter).h1DTrend->Draw("P");
      padCounter++;      
    } //for counterIter

  } // if updateCanvas
    

  if (updateCanvas)
    m_Canvas->Update();

  if (m_verbose > 1 )
    std::cout << "update extra canvas if necessary " << std::endl;
  
  if (m_ZoomCanvasActive)
    HPDGui::UpdateCanvasZoom(m_1DDrawOption, m_2DDrawOption);
  
  
    

  
  
} //void Update
// -----------------------------------------------------------------------------------------
void HPDGui::UpdateCanvasZoom(std::string drawOption1D, std::string drawOption2D){

  // if the extra canvas showing only one histogram 
  // is active, determine, which histogram to display
  // and draw it, othewise return
  if (!m_ZoomCanvasActive)
    return;

  int canvasEventX = m_Canvas->GetEventX();
  int canvasEventY = m_Canvas->GetEventY();

  if (m_verbose > 1)
    std::cout << "X " << canvasEventX << " Y " << canvasEventY << std::endl;

  // loop over all pads
  int selectedPadNr = 0;
  for (int iPad = 1; iPad <= m_nCanvasColumns*m_nCanvasRows; iPad++){
    if (m_verbose > 1)
      std::cout << "now check pad " << iPad << std::endl;
    TVirtualPad *thisPad = m_Canvas->GetPad(iPad);
    thisPad->cd();
    int distance = thisPad->DistancetoPrimitive(canvasEventX, canvasEventY);    
    if (m_verbose > 1)
      std::cout << "distance " << distance << std::endl;
    if (distance == 0)
      selectedPadNr = iPad;
  } //for
  
  if (m_verbose > 1)
    std::cout << "selected Pad number " << selectedPadNr << std::endl;

  if (selectedPadNr > 0 && selectedPadNr <= m_nCanvasColumns*m_nCanvasRows) {
    TVirtualPad *selected = m_Canvas->GetPad(selectedPadNr);
    // selected->GetPad(selectedPadNr)->SetTopMargin(0.5);
    // selected->GetPad(selectedPadNr)->SetBottomMargin(0.5);
    selected->GetPad(selectedPadNr)->SetLeftMargin(0.15);
    selected->GetPad(selectedPadNr)->SetRightMargin(0.15);
    // the list seems to be of format: {TFrame, <histo>, title}
    TList *theList = selected->GetListOfPrimitives();
    
    if (m_verbose > 2)
      std::cout << "list size " << theList->GetEntries() << std::endl;

    // N.B. TH2 inherits from TH1
    if (theList->At(1)->InheritsFrom("TH2")) {
      TH2 *hZoom = (TH2*)theList->At(1);
      if (m_verbose > 1)
        std::cout << "found 2D histo with title " << hZoom->GetTitle() << std::endl;
      m_CanvasZoom->cd();
      hZoom->Draw(drawOption2D.c_str());
      m_CanvasZoom->Update();      
      m_Canvas->cd();      
      m_Canvas->Update();      
      
    } else if (theList->At(1)->InheritsFrom("TH1")) {        
      TH1 *hZoom = (TH1*)theList->At(1);
      if (m_verbose > 1 )
        std::cout << "found 1D histo with title " << hZoom->GetTitle() << std::endl;
      m_CanvasZoom->cd();
      hZoom->Draw(drawOption1D.c_str());
      m_CanvasZoom->Update();      
      m_Canvas->cd();      
      m_Canvas->Update();      
    } else {
      if (m_verbose > 1)
        std::cout << "found unknown object" << std::endl;
    } // if theList
  } // if selectedPadNr > 0
  
      

  
} // void UpdateCanvasZoom
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
  std::string            tmpString;

  std::vector<std::string> serviceH1DNameVector;
  std::vector<std::string> serviceH2DNameVector;
  std::vector<std::string> serviceOtherNameVector;

  //
  // remove all entries before starting over
  //
  m_ListTreeDimServices->DeleteChildren(m_ListTreeItemMain);
  m_ListTreeItemVector.clear();
  m_GaudiAlgNameMap.clear();


  //
  // query the DIM service for available servers and
  // services
  //

  // get a list of all servers connected to current DIM DNS server
  nDimServers = dimBrowser.getServers();

  // if some servers found, discover which services
  // are there - otherwise return "fail"
  if (nDimServers > 0) {
    const char* dimDnsServerNode = DimClient::getDnsNode();
    tmpString = "successfully connected to DIM, DNS at node " + 
      boost::lexical_cast<std::string>(dimDnsServerNode);    
    m_StatusBar     -> SetText(tmpString.c_str());
    returnValue = true;    
  } else {
    m_StatusBar     -> SetText("failed to connect to DIM");
    return false;    
  }// if #DIM servers
  
  //
  // loop over all DIM servers and get published services
  //
  while (dimBrowser.getNextServer(dimServer, dimServerNode)) {

    //
    // reset
    //
    serviceH1DNameVector.clear();
    serviceH2DNameVector.clear();
    serviceOtherNameVector.clear();
    
    // get name of this server
    stringServer = dimServer;
    if (m_verbose > 0)
      std::cout << "DIM server " << stringServer << " @ " << dimServerNode << std::endl;    

    // check if we found a DNS server and remove this from the list
    stringLocation = stringServer.find("DIS_DNS",0);
    if (stringLocation != std::string::npos) {
      if (m_verbose > 1)
        std::cout << " found DIM DNS at node " << dimServerNode << std::endl;
    } else {
      if (m_verbose > 1)
        std::cout << " found other DIM server " << std::endl;    
      m_DimServerNameVector.push_back(stringServer);
      TGListTreeItem *thisDimServer = m_ListTreeDimServices -> AddItem(m_ListTreeItemMain, dimServer);

      // now loop get a list of all services offered by this server
      dimBrowser.getServerServices(dimServer);
      while(dimType = dimBrowser.getNextServerService(dimService, dimFormat)) {
        stringService = dimService;
        stringFormat  = dimFormat;
        
        if (m_verbose > 1)
          std::cout << "DIM service " << dimService << " format " << dimFormat << std::endl;
        
        
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

        // determine name of algorithm publishing
        if (stringService.find("VERSION_NUMBER",0) == std::string::npos)  {
          
          int algNameStart = stringService.find(stringServer,0)+stringServer.length()+1;
          int algNameStop  = algNameStart;
          while (stringService.substr(algNameStop,1) != "/") {          
            algNameStop++;
          } // while
          std::string GaudiAlgName = stringService.substr(algNameStart, algNameStop-algNameStart);          
          
          // check if GaudiAlgName already in map, otherwise add
          std::map<std::string,TGListTreeItem *>::const_iterator mapIter;
          mapIter = m_GaudiAlgNameMap.find(GaudiAlgName);          
          if (mapIter == m_GaudiAlgNameMap.end()){
            TGListTreeItem *thisGaudiAlg    = m_ListTreeDimServices -> AddItem(thisDimServer, GaudiAlgName.c_str());
            m_GaudiAlgNameMap[GaudiAlgName] = thisGaudiAlg;            
            TGListTreeItem *thisH2D         = m_ListTreeDimServices -> AddItem(thisGaudiAlg , "H2D");
            TGListTreeItem *thisH1D         = m_ListTreeDimServices -> AddItem(thisGaudiAlg , "H1D");
            TGListTreeItem *thisOther       = m_ListTreeDimServices -> AddItem(thisGaudiAlg , "Other");
          } // if mapIter

          // remove the server name from the service ID string
          stringService.replace(stringService.find(stringServer,0),stringServer.length(),"");
          stringService.replace(stringService.find_first_of("/",0),1,"");
          
          // add the services to the list
          // 2D histograms
          if (stringService.find("H2D/",0) != std::string::npos) {          
            // remove "H2D"/ from string
            stringService.replace(stringService.find("H2D/",0),4,"");          
            serviceH2DNameVector.push_back(stringService);            
          } else  if (stringService.find("H1D/",0) != std::string::npos) {          
            stringService.replace(stringService.find("H1D/",0),4,"");          
            serviceH1DNameVector.push_back(stringService);
          } else {
            serviceOtherNameVector.push_back(stringService);
          } // if H2D, H1D

        } else { // if "version_number" not found (DIM specific?)
          // if this item is "VERSION_NUMBER"
          TGListTreeItem *thisItem =  m_ListTreeDimServices -> AddItem(thisDimServer, stringService.c_str());        
          m_ListTreeItemVector.push_back(thisItem);
          m_ListTreeDimServices    -> SetCheckBox(thisItem, kTRUE);
          m_ListTreeDimServices    -> ToggleItem(thisItem);          
        } // if "version_number" not found (DIM specific?)        
        
      } //while dimServerService
    } // if stringLocation

    //
    // sort the string-vectors 
    //
    std::sort(serviceH1DNameVector.begin()  , serviceH1DNameVector.end());
    std::sort(serviceH2DNameVector.begin()  , serviceH2DNameVector.end());
    std::sort(serviceOtherNameVector.begin(), serviceOtherNameVector.end());
    
    
    // 
    // loop over all Gaudi Algorithms and add services
    // to tree structure
    //
    
    std::vector<std::string>::const_iterator stringIter;
    std::vector<std::string>::const_iterator stringIterBegin;
    std::vector<std::string>::const_iterator stringIterEnd;
    
    std::map<std::string,TGListTreeItem *>::const_iterator mapIter;
    std::map<std::string,TGListTreeItem *>::const_iterator mapIterBegin = m_GaudiAlgNameMap.begin();
    std::map<std::string,TGListTreeItem *>::const_iterator mapIterEnd   = m_GaudiAlgNameMap.end();
    
    for (mapIter = mapIterBegin; mapIter != mapIterEnd; mapIter ++){
      
      std::string GaudiAlgName = mapIter->first;
      if (m_verbose > 1)
        std::cout << "now consider GaudiAlg " << GaudiAlgName << std::endl;
      
      stringIterBegin = serviceH2DNameVector.begin();
      stringIterEnd   = serviceH2DNameVector.end();
      for (stringIter = stringIterBegin; stringIter != stringIterEnd; stringIter++) {
        std::string stringService = *stringIter;
        if (stringService.find(GaudiAlgName,0) != std::string::npos) {
          // found correct algorithm, remove GaudiAlg name from service ID string
          if (m_verbose > 1)
            std::cout << "this H2D service " << stringService << std::endl;
          stringService.replace(stringService.find(GaudiAlgName,0),GaudiAlgName.length(),"");
          stringService.replace(stringService.find_first_of("/",0),1,"");
          TGListTreeItem *thisH2D    =  m_ListTreeDimServices -> FindChildByName(m_GaudiAlgNameMap[GaudiAlgName], "H2D");            
          TGListTreeItem *thisItem   =  m_ListTreeDimServices -> AddItem(thisH2D, stringService.c_str());        
          m_ListTreeItemVector.push_back(thisItem);
          m_ListTreeDimServices      -> SetCheckBox(thisItem, kTRUE);
          m_ListTreeDimServices      -> ToggleItem(thisItem);
        } // if stringService
      } // for stringIter
      
      stringIterBegin = serviceH1DNameVector.begin();
      stringIterEnd   = serviceH1DNameVector.end();
      for (stringIter = stringIterBegin; stringIter != stringIterEnd; stringIter++) {
        std::string stringService = *stringIter;
        if (stringService.find(GaudiAlgName,0) != std::string::npos) {
          // found correct algorithm, remove GaudiAlg name from service ID string
          if (m_verbose > 1)
            std::cout << "this H1D service " << stringService << std::endl;
          stringService.replace(stringService.find(GaudiAlgName,0),GaudiAlgName.length(),"");
          stringService.replace(stringService.find_first_of("/",0),1,"");
          TGListTreeItem *thisH1D    =  m_ListTreeDimServices -> FindChildByName(m_GaudiAlgNameMap[GaudiAlgName], "H1D");
          TGListTreeItem *thisItem   =  m_ListTreeDimServices -> AddItem(thisH1D, stringService.c_str());        
          m_ListTreeItemVector.push_back(thisItem);
          m_ListTreeDimServices      -> SetCheckBox(thisItem, kTRUE);
          m_ListTreeDimServices      -> ToggleItem(thisItem);
          // void SetToolTipItem(TGListTreeItem *item, const char *string)  // can add GauchoComment later if wanted
        } // if stringService        
      } //for stringIter
      
      stringIterBegin = serviceOtherNameVector.begin();
      stringIterEnd   = serviceOtherNameVector.end();
      for (stringIter = stringIterBegin; stringIter != stringIterEnd; stringIter++) {
        std::string stringService = *stringIter;
        if (stringService.find(GaudiAlgName,0) != std::string::npos) {
          // found correct algorithm, remove GaudiAlg name from service ID string
          if (m_verbose > 1 )
            std::cout << "this other service " << stringService << std::endl;
          stringService.replace(stringService.find(GaudiAlgName,0),GaudiAlgName.length(),"");
          stringService.replace(stringService.find_first_of("/",0),1,"");
          TGListTreeItem *thisOther  =  m_ListTreeDimServices -> FindChildByName(m_GaudiAlgNameMap[GaudiAlgName], "Other");
          TGListTreeItem *thisItem   =  m_ListTreeDimServices -> AddItem(thisOther, stringService.c_str());        
          m_ListTreeItemVector.push_back(thisItem);
          m_ListTreeDimServices      -> SetCheckBox(thisItem, kTRUE);
          m_ListTreeDimServices      -> ToggleItem(thisItem);        
        } // if stringService        
      } // for stringIter
      
    } //for mapIter

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

  /* structure of tree-like selection canvas is:
   * GaudiOnline Name
   *    |
   *    - GaudiAlg Name
   *          |
   *          -  Service Type (Histo/counter)
   *                  |
   *                  -  service Name
   */


  if (m_SelectedHistogramVector.size() > 0) {    
    std::vector<TGListTreeItem *>::const_iterator histoIter;
    std::vector<TGListTreeItem *>::const_iterator histoIterBegin = m_SelectedHistogramVector.begin();
    std::vector<TGListTreeItem *>::const_iterator histoIterEnd   = m_SelectedHistogramVector.end();
    
    for (histoIter = histoIterBegin; histoIter != histoIterEnd; histoIter++){
      std::string serviceName     = (*histoIter)->GetText();      
      std::string serviceType     = (*histoIter)->GetParent()->GetText();
      std::string GaudiAlgName    = (*histoIter)->GetParent()->GetParent()->GetText();
      std::string GaudiOnlineName = (*histoIter)->GetParent()->GetParent()->GetParent()->GetText();
      if (m_verbose > 2)
        std::cout << "serviceName " << serviceName << " serviceType " << serviceType
                  << " GaudiAlgName " << GaudiAlgName
                  << " GaudiOnlineName " << GaudiOnlineName << std::endl;      

      
      // sanity check: really histogram?
      if (!((serviceType.substr(0,3) == "H1D") || (serviceType.substr(0,3) == "H2D"))){
      std::cout << "HPDGui::SetupCanvas: not histogram - though should have been" << std::endl;
      continue;
      } //if not histgram
      
      iHisto++;
      std::string histoID = "histo_" + boost::lexical_cast<std::string>(iHisto);
      
      // build up again ID string recognised by DIM
      std::string serviceNameFQ = serviceType+"/"+GaudiOnlineName+"/"+GaudiAlgName+"/"+serviceName;
      if (m_verbose > 1)
        std::cout << "try to find service " << serviceNameFQ << std::endl;
      
      
      if (serviceType.substr(0,3) == "H1D") {
        H1DHisto histo1D;
        histo1D.dimHisto = new DimInfoHisto(serviceNameFQ, m_refreshTimeHisto, m_verbose);
        if (histo1D.dimHisto->serviceOK()) {    
          TH1* tmp_h1D = histo1D.dimHisto->get1DHisto();
          if (tmp_h1D) {
            nBinsX             = tmp_h1D->GetNbinsX();
            xMin               = tmp_h1D->GetXaxis()->GetXmin();
            xMax               = tmp_h1D->GetXaxis()->GetXmax();
            std::string hTitle = tmp_h1D->GetTitle();
            if (m_verbose > 3)
              std::cout << " nBinsX " << nBinsX << " xMin " << xMin << " xMax " << xMax
                        << std::endl;

            // book the 1D histo which is to be displayed
            histo1D.h1D       = new TH1F(histoID.c_str(), hTitle.c_str(), nBinsX, xMin, xMax);
            histo1D.h1D      -> SetMarkerStyle(22);
            histo1D.h1D      -> SetMarkerSize(0.9);
            histo1D.oldValue  = new double;            
            *histo1D.oldValue = -999;
            m_histo1DVector.push_back(histo1D);
          } // if tmp_h1d
        } // if serviceOK
        
      } else if (serviceType.substr(0,3) == "H2D") {              
        H2DHisto histo2D;
        histo2D.dimHisto = new DimInfoHisto(serviceNameFQ, m_refreshTimeHisto, m_verbose);
        if (histo2D.dimHisto->serviceOK()) {
          TH2* tmp_h2D = histo2D.dimHisto->get2DHisto();
          if (tmp_h2D) {
            // dimHisto gives back a NULL pointer if something went wrong
            nBinsX             = tmp_h2D->GetNbinsX();
            nBinsY             = tmp_h2D->GetNbinsY();
            xMin               = tmp_h2D->GetXaxis()->GetXmin();
            xMax               = tmp_h2D->GetXaxis()->GetXmax();
            yMin               = tmp_h2D->GetYaxis()->GetXmin();
            yMax               = tmp_h2D->GetYaxis()->GetXmax();
            std::string hTitle = tmp_h2D->GetTitle();            
            if (m_verbose > 3)
              std::cout << " nBinsX " << nBinsX << " xMin " << xMin << " xMax " << xMax 
                        << " nBinsY " << nBinsY << " yMin " << yMin << " yMax " << yMax
                        << std::endl;
            
            // book new TH2F - the one which is to be displayed
            histo2D.h2D = new TH2F(histoID.c_str(), hTitle.c_str(), 
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
    std::string counterID;

    for (counterIter = counterIterBegin; counterIter != counterIterEnd; counterIter++){
      std::string serviceName     = (*counterIter)->GetText();      
      std::string serviceType     = (*counterIter)->GetParent()->GetText();
      std::string GaudiAlgName    = (*counterIter)->GetParent()->GetParent()->GetText();
      std::string GaudiOnlineName = (*counterIter)->GetParent()->GetParent()->GetParent()->GetText();
      if (m_verbose > 2)
        std::cout << "serviceName " << serviceName << " serviceType " << serviceType
                  << " GaudiAlgName " << GaudiAlgName
                  << " GaudiOnlineName " << GaudiOnlineName << std::endl;      

      // sanity check: really not histogram?
      if (((serviceType.substr(0,3) == "H1D") || (serviceType.substr(0,3) == "H2D"))){
        std::cout << "HPDGui::SetupCanvas: histogram - though should have been counter" << std::endl;
        continue;
      } //if not histgram
      
      // build up again ID string recognised by DIM
      std::string serviceNameFQ = GaudiOnlineName+"/"+GaudiAlgName+"/"+serviceName;
      if (m_verbose > 1)
        std::cout << "try to find service " << serviceNameFQ << std::endl;

      std::string histoName;
      // new instance of struct
      HPDGui::CounterHisto counterHisto;
      counterHisto.dimCounter = new DimInfoCounter(serviceNameFQ, m_refreshTimeCounter, m_verbose);    

      if (counterHisto.dimCounter -> serviceOK()){
        counterHisto.oldValue = new float;        
        *(counterHisto.oldValue) = -999; // set to some unlikely value

        // book 1D histo - cumulative distribution
        iCounter++;
        counterID = "counter_"+ boost::lexical_cast<std::string>(iCounter);

        histoName = serviceName;
        histoName.append(" - cumulative ");        
        // make large enough - rescaling afterwards possible (and done)
        // but seems to lead to some artifact with overflows
        int nBins = abs(m_counterMax - m_counterMin);
        counterHisto.h1DCumulative =  new TH1F(counterID.c_str(), histoName.c_str(), nBins, m_counterMin, m_counterMax);

        // book 1D histo - trend
        iCounter++;
        counterID = "counter_"+ boost::lexical_cast<std::string>(iCounter);
        histoName = serviceName;
        histoName.append(" - trend ");
        counterHisto.h1DTrend      =  new TH1F(counterID.c_str(), histoName.c_str(), m_nTimeSteps, 1, m_nTimeSteps);
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
      // m_Canvas->GetPad(padCounter)->SetTopMargin(0.5);
      // m_Canvas->GetPad(padCounter)->SetBottomMargin(0.5);
      m_Canvas->GetPad(padCounter)->SetLeftMargin(0.15);
      m_Canvas->GetPad(padCounter)->SetRightMargin(0.15);
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
    (*h1DIter).h1D -> Draw(m_1DDrawOption.c_str());      
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
void HPDGui::SetVerbose(int verbose){
  
  m_verbose = verbose;

  if (m_verbose>0)
    std::cout << "setting verbosity level to " << m_verbose << std::endl;

} // void SetVerbose
// -----------------------------------------------------------------------------------------
HPDGui::histStatOption HPDGui::DetermineStatOption() {

  HPDGui::histStatOption returnValue =  HPDGui::none;
  
  if (m_verbose > 1)
    std::cout << "determine if statistics box should be shown" << std::endl;
  
  std::string statOption = m_EntryStats -> GetTextEntry() -> GetText();

  if (statOption == "None")
    returnValue = HPDGui::none;
  else if (statOption == "1D")
    returnValue = HPDGui::stat1D;
  else if (statOption == "2D")
    returnValue = HPDGui::stat2D;
  else if (statOption == "1D + 2D")
    returnValue = HPDGui::stat1D2D;
  else
    returnValue = HPDGui::none;

  if (m_verbose > 0)
    std::cout << "stat option chosen " << statOption << " mapped to " << returnValue << std::endl;

  return returnValue;
} // DetermineStatOption

// -----------------------------------------------------------------------------------------

  
