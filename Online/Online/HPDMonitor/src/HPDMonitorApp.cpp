// $Id: HPDMonitorApp.cpp,v 1.7 2007-10-31 10:45:37 ukerzel Exp $
// Include files 

#include <iostream>
#include <stdlib.h>
#include <math.h>

// ROOT 
#include <TROOT.h>

// BOOST
#include "boost/lexical_cast.hpp"

// local
#include "HPDMonitorApp.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HPDMonitorApp
//
// 2006-08-07 : Ulrich Kerzel
//-----------------------------------------------------------------------------

HPDMonitorApp::HPDMonitorApp(int         timerRate,
                             int         guiWidth,
                             int         guiHeight,
                             int         verbose,
                             std::string histoDimName) :
  m_timerRate(timerRate)
{
  int   dummy_argc   = 1;
  char *dummy_argv[] =  { "HPDMonApp.exe", NULL  };
  
  m_TApplication = new TApplication("HPDMonitorApp",&dummy_argc,dummy_argv);

  m_Timer        = new TTimer(timerRate);

  // this takes some time
  m_HPDGui = new HPDGui(gClient->GetRoot(),guiWidth,guiHeight,
                        verbose,
                        m_Timer,
                        histoDimName);

  //  m_HPDGui -> SetTimer(m_Timer);
  
  // m_HPDGui -> SetVerbose(verbose);
  
} //constructor
//-----------------------------------------------------------------------------
HPDMonitorApp::~HPDMonitorApp() {
  delete m_TApplication;
  
} //destrucotr
//-----------------------------------------------------------------------------
void HPDMonitorApp::Start() {


  m_Timer->SetObject(this);  
  
  m_TApplication->Run();

  
} //void Start
//-----------------------------------------------------------------------------
Bool_t HPDMonitorApp::HandleTimer(TTimer* timer) {
  // std::cout << "HPDMonitorApp::HandleTimer" << std::endl;

  int verbose = 0;
  if (verbose > 0) {
    TTime now = timer->GetTime();
    std::cout << "HPDMonitorApp::HandleTimer  Time is " << now.AsString() << std::endl;
  } // if verbose


  m_HPDGui->Update();
  
  return kFALSE;
  
} //HandleTimer
//-----------------------------------------------------------------------------
int main(int argc, char **argv) {

  if (gROOT->IsBatch()) {
    std::cerr << " cannot run in batch mode, exit" << std::endl;
    return 1;
  }

  //
  // parse command line options
  // (very simple for now, to be extended)
  // assume format <progName> <variable> <value>
  // so far supported:
  // - verbose
  // - height
  // - width
  // 

  int         verbose    =   0;
  int         height     = 600;
  int         width      = 800;
  std::string histoDimName;
  
  for (int i = 1; i < argc; i++) {
    std::string tmpString = argv[i];

    //
    // "verbose"
    //
    if (tmpString.find("verbose",0) != std::string::npos ) {
      try {        
        verbose = boost::lexical_cast<int>(argv[i+1]);
      } //try      
      catch (boost::bad_lexical_cast &) {        
        std::cout << "conversion of verbosity level " << argv[i+1]
                  << " to integer failed, use level 0"
                  << std::endl;        
        verbose = 0;
      } // catch
    } // if found verbose

    //
    // "width"
    //
    if (tmpString.find("width",0)  != std::string::npos ) {
      try {        
        width = boost::lexical_cast<int>(argv[i+1]);
        if (width < 0)
          width = 800;        
      } //try      
      catch (boost::bad_lexical_cast &) {        
        std::cout << "conversion of width " << argv[i+1]
                  << " to integer failed, use 800"
                  << std::endl;        
        verbose = 0;
      } // catch
    } // if found width

    //
    // "height"
    //
    if (tmpString.find("height",0) != std::string::npos ) {
      try {        
        height = boost::lexical_cast<int>(argv[i+1]);
        if (width < 0)
          width = 600;
      } //try      
      catch (boost::bad_lexical_cast &) {        
        std::cout << "conversion of height " << argv[i+1]
                  << " to integer failed, use 600"
                  << std::endl;        
        verbose = 0;
      } // catch
    } // if found height
    
    //
    // "histo"
    //
    if (tmpString.find("histo",0) != std::string::npos ) {
      histoDimName = (argv[i+1]);
      std::cout << "found histo to monitor " << histoDimName << std::endl;
    }// if histo

    
  } //for  
  
  if (verbose > 0) {    
    std::cout << "GaudiOnline Monitor starts: verbose= " << verbose
              << " height = " << height 
              << " width = "  << width
              << std::endl;
  } // if verbose
  

  HPDMonitorApp *theApp = new HPDMonitorApp(200, width, height, verbose,histoDimName);  
  theApp->Start();
  
    
  return 0;
}
