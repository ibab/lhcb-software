// $Id: HPDGui.h,v 1.4 2006-09-21 07:26:49 ukerzel Exp $
#ifndef HPDGUI_H 
#define HPDGUI_H 1

// Include files

//
// ROOT
//

#include "TTimer.h"

// window objects
#include <TGListBox.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGStatusBar.h>
#include <TGListTree.h>

//Canvas, etc
#include <TCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TPad.h>

// histrograms and functions
#include<TH1.h>
#include<TH2.h>


//
// DIM
//
#include "dic.hxx"
#include "DimInfoCounter.h"
#include "DimInfoHisto.h"


//
// main GUI for HPD monitor
// n.b. needs to inherit from TGMainFrame
//
class HPDGui : public TGMainFrame {

  enum histID { idConnect,
                idExit,
                idPrint,
                idPause,
                idSelect,
                id2DDrawOption,
                id1DDrawOption};
  
  struct CounterHisto   {
    
    TH1F*           h1DCumulative;  // "inclusive" distribution of counter aquired over time
    TH1F*           h1DTrend;       // trend: counter value vs time
    DimInfoCounter* dimCounter;
    float         * oldValue;       // check that value has changed before filling histogram

  }; // struct CounterHisto
  
  struct H1DHisto   {
    TH1F*           h1D;
    DimInfoHisto*   dimHisto;    
    double*         oldValue;    
  } ; // struct H1DHisto
  
  struct H2DHisto   {
    TH2F*           h2D;
    DimInfoHisto*   dimHisto;
    double*         oldValue;
  }; // struct H2DHisto
  
  
    

  // ---------------------------------------------------------------
  //                PUBLIC
  // ---------------------------------------------------------------
public:

  HPDGui(const TGWindow *p, UInt_t w, UInt_t h);  
  virtual ~HPDGui(); 
  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  virtual void   CloseWindow();

  void    SetTimer(TTimer* timer);
  
  // sets the verbosity, zero means off, values 
  // greater than zero give more and more output
  void    SetVerbose(int verbose);
  
  
  void    Update();


  // ---------------------------------------------------------------
  //                PRIVATE 
  // ---------------------------------------------------------------
  
private:

  //
  // private functions
  //
  bool Connect2DIM(); // create instances of the DIM variables for
                      // HPD hit counter, 2D "ring" histogram
                      // returns "true" if both services exist and
                      // are found via the DIM DNS, "false" otherwise
                      // N.B. _not_ called in constructor but
                      //      when "connect" button is pressed, i.e.
                      //      when data should be displayed

  void Pause();
  void Play();

  void SetupCanvas(); // setup canvas displaying the histograms and
                      // counters. Counters are always displayed
                      // with two histograms: one shows the inclusive
                      // distribution accumulated with time, the other
                      // shows a trend plot showing the value in the last
                      // n time-steps

  void Reset();       // clear all elements, etc
  
  

  TGStatusBar                  *m_StatusBar;

  TRootEmbeddedCanvas          *m_EmbeddedCanvas;          // "embedded canvas" object which is embedded in the main GUI window
  TCanvas                      *m_Canvas;                  // the "real" canvas to operate with, obtained from the embedded canvas  

  TGCompositeFrame             *m_CompositeFrameMaster;     // "master" frame holding GUI 
                                                            // (=Canvas, buttons, entry fields) and the status bar
  TGCompositeFrame             *m_CompositeFrameMain;       // "main" frame holding the
                                                            // embedded canvas and the CompositeFrame 
                                                            // with the buttons and entry fields
  TGCompositeFrame             *m_CompositeFrameButtons;    // frame holding the control buttons and entry fields
  TGGroupFrame                 *m_GroupFrameHPDControl;     // group holding the entry fields for
                                                            // HPD row/column, refresh time
  TGGroupFrame                 *m_GroupFrameGuiControl;     // group holding start/stop exit buttons
  TGGroupFrame                 *m_GroupFrameListTree;       // frame holding the selection tree for the
                                                            // histograms/counter services obtained by DIM

  TGListTree                   *m_ListTreeDimServices;      // tree-like list structure for all DIM services
  TGCanvas                     *m_CanvasListTree;           // Canvas to display the tree-like list structure
  TGListTreeItem               *m_ListTreeItemMain;         // Item below which are all DIM servers
  TGListTreeItem               *m_ListTreeItemH2D;          // all published 2D histograms
  TGListTreeItem               *m_ListTreeItemH1D;          //               1D
  TGListTreeItem               *m_ListTreeItemOther;        // others
  
  std::vector<TGListTreeItem *> m_ListTreeItemVector;       // all elements in the list-tree

  std::vector<TGListTreeItem *> m_SelectedHistogramVector;  // vector of histograms selected to display
  std::vector<TGListTreeItem *> m_SelectedCounterVector;    // vector of counters selected to display
  
  TGTextButton                 *m_ButtonConnect;  
  TGTextButton                 *m_ButtonExit; 
  TGTextButton                 *m_ButtonPrint;
  TGTextButton                 *m_ButtonPause;
  TGTextButton                 *m_ButtonSelect;
  
  TGNumberEntry                *m_EntryRefreshTimeHisto;
  TGNumberEntry                *m_EntryRefreshTimeCounter;
  TGNumberEntry                *m_EntryCounterMin;          // anticipated min. counter value - used for histogram border
  TGNumberEntry                *m_EntryCounterMax;          //             max.
  TGComboBox                   *m_Entry2DDrawOption;        // option for drawing 2D histogram,
  TGComboBox                   *m_Entry1DDrawOption;        // option for drawing 1D histogram,

  TGLabel                      *m_labelRefreshTimeHisto;
  TGLabel                      *m_labelRefreshTimeCounter;
  TGLabel                      *m_labelCounterMin;
  TGLabel                      *m_labelCounterMax;
  TGLabel                      *m_label2DDrawOption;
  TGLabel                      *m_label1DDrawOption;
  
  TGHotString                  *m_stringRefreshTimeHisto;
  TGHotString                  *m_stringRefreshTimeCounter;
  TGHotString                  *m_stringCounterMin;
  TGHotString                  *m_stringCounterMax;
  TGHotString                  *m_string2DDrawOption;
  TGHotString                  *m_string1DDrawOption;
  

  TGLayoutHints                *m_LayoutTopLeft;
  TGLayoutHints                *m_LayoutTopLeftExpandX;
  TGLayoutHints                *m_LayoutTopRight;
  TGLayoutHints                *m_LayoutLeftTop;
  TGLayoutHints                *m_LayoutBottomLeft;
  TGLayoutHints                *m_LayoutBottomRight;

  TTimer                       *m_externalTimer;            // timer passed from external object
                                                            // needed to control timer from GUI
  bool                          m_timerRuns;

  ULong_t                       m_ROOTRed;
  ULong_t                       m_ROOTYellow;
  ULong_t                       m_ROOTGreen;

  std::vector<H2DHisto>         m_histo2DVector;            // vector of all 2D histograms to be displayed
  std::vector<H1DHisto>         m_histo1DVector;            //               1D
  std::vector<CounterHisto>     m_counterVector;            //               counters 
  
  
  int                           m_nTimeSteps;               // number of time-steps recorded in trend histogram of counters
  int                           m_refreshTimeHisto;         // time after which the histogram is updated
  int                           m_refreshTimeCounter;       // time after which counter is updated
  int                           m_counterMin;               // anticipated min. counter value, used for histogram borders
  int                           m_counterMax;               //             max.
  std::string                   m_2DDrawOption;             // option to be used when drawing 2D histogram
  std::string                   m_1DDrawOption;             //                                1D
  
  int                           m_nCanvasRows;              // number of rows into which the main canvas is divided
  int                           m_nCanvasColumns;           //           columns
  
  
  bool                          m_connectOK;                // could successfully connect to DIM services, 

  int                           m_verbose;

  std::vector<std::string>      m_DimServerNameVector;

  std::map<std::string,TGListTreeItem *> m_GaudiAlgNameMap; // for building up tree-like structure to browse DIM DNS
  
  
}; // class HPGui
#endif // HPDGUI_H
