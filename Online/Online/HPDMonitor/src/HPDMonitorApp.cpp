// $Id: HPDMonitorApp.cpp,v 1.4 2006-09-21 07:26:49 ukerzel Exp $
// Include files 

#include <iostream>
#include <stdlib.h>
#include <math.h>

// ROOT stuff
#include <TROOT.h>


// local
#include "HPDMonitorApp.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HPDMonitorApp
//
// 2006-08-07 : Ulrich Kerzel
//-----------------------------------------------------------------------------

HPDMonitorApp::HPDMonitorApp(int timerRate,
                             int guiWidth,
                             int guiHeight,
                             int verbose) :
  m_timerRate(timerRate)
{
  int   dummy_argc   = 1;
  char *dummy_argv[] =  { "HPDMonApp.exe", NULL  };
  
  m_TApplication = new TApplication("HPDMonitorApp",&dummy_argc,dummy_argv);

  m_Timer        = new TTimer(timerRate);

  // this takes some time
  m_HPDGui = new HPDGui(gClient->GetRoot(),guiWidth,guiHeight);

  m_HPDGui -> SetTimer(m_Timer);
  
  m_HPDGui -> SetVerbose(verbose);
  
  
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
  // assume format <progName> verbose N
  // 
  int posVerbose = 0;  
  int verbose    = 0;
  for (int i = 1; i < argc; i++) {
    std::string tmpString = argv[i];
    if (tmpString.find("verbose",0) != std::string::npos )
      posVerbose = i;
  } //for  
  if (posVerbose>0 && argc >= 3) {
    // need at least 3 arguments: <progName> verbose <value>
    verbose = atoi(argv[posVerbose+1]);
//    std::cout << "found keyword 'verbose' at position "  << posVerbose
//              << " with verbosity value "                << verbose
//              << std::endl;   
  } // if posVerbose
  
  if (verbose > 0) {    
    std::cout << "GaudiOnline Monitor starts " << std::endl;
  } // if verbose
  

  HPDMonitorApp *theApp = new HPDMonitorApp(200, 800, 600, verbose);  
  theApp->Start();
  return 0;
}
