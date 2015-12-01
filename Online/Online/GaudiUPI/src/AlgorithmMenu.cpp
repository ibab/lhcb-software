// $ID: $

// Include Files
#include "GaudiKernel/System.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiUPI/AlgorithmMenu.h"
#include "GaudiUPI/PropertyMenu.h"
#include "GaudiUPI/DialogMenu.h"
#include "CPP/Event.h"

#include <list>
using namespace std;

/// Initializing constructor
Gaudi::AlgorithmMenu::AlgorithmMenu(ISvcLocator* svc, CPP::Interactor* par) 
: SubMenu("Hit return on algorithm for properties", par), m_svcLoc(svc)
{
  SmartIF<IAlgManager> mgr(svc);
  if ( mgr )  {
    const auto& algs = mgr->getAlgorithms();
    if ( !algs.empty() )  {
      const char* fmt = "%-32s %-32s  %-5s%-5s%-5s";
      int cmd = CMD_SHOW;
      m_window->addCOM(cmd, fmt, "Name","Type","Init","Fini","Exec");
      for(const auto& ia : algs ) { 
        string typ = System::typeinfoName(typeid(*ia));
        m_lines.insert(make_pair(++cmd,const_cast<IAlgorithm*>(ia)));
        m_window->addCMD(cmd, fmt,ia->name().c_str(),typ.c_str(),
          ia->isInitialized() ? "YES" : "NO",
          ia->isFinalized()   ? "YES" : "NO",
          ia->isExecuted()    ? "YES" : "NO");
      }
    }
    else  {
      m_window->addCOM(CMD_SHOW,"No algorithms found!");
    }
  }
  else  {
    m_window->addCOM(CMD_SHOW,"Failed to retrieve the IAlgManager interface.");
  }
  mapWindow();
}

/// Standard destructor
Gaudi::AlgorithmMenu::~AlgorithmMenu()  {
}

/// Interactor overload: handle menu interupts
void Gaudi::AlgorithmMenu::handle (const CPP::Event& event)   {
  if ( event.eventtype == UpiEvent )  {
    int cmd = event.command_id;
    if ( cmd > CMD_SHOW )  {
      Lines::iterator i=m_lines.find(cmd);
      if ( i != m_lines.end() )  {
        m_cursor = cmd;
        if ( m_child ) delete m_child;
        m_child = new PropertyMenu((*i).second->name(),(*i).second,this);
      }
      return;
    }
  }
  SubMenu::handle(event);
}
