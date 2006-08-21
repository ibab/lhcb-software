// $Id: HPDMonitorApp.cpp,v 1.1.1.1 2006-08-21 13:49:59 ukerzel Exp $
// Include files 
#include <iostream>
#include <stdlib.h>



// local
#include "HPDMonitorApp.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HPDMonitorApp
//
// 2006-08-07 : Ulrich Kerzel
//-----------------------------------------------------------------------------

HPDMonitorApp::HPDMonitorApp(int timerRate,
			     int guiWidth,
			     int guiHeight) :
  m_timerRate(timerRate)
{
  int    dummy_argc = 1;
  char **dummy_argv;
  m_TApplication = new TApplication("HPDMonitorApp",&dummy_argc,dummy_argv);

  m_Timer        = new TTimer(timerRate);

  // this takes some time
  m_HPDGui = new HPDGui(gClient->GetRoot(),guiWidth,guiHeight);

  m_HPDGui -> SetTimer(m_Timer);
  
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
