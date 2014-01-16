// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/BankTypesWindow.cpp,v 1.6 2010-08-31 16:02:34 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: BankTypesWindow.cpp,v 1.6 2010-08-31 16:02:34 frankb Exp $
//
// C++ include files
#include <string>

// Framework include files
#include "Event/RawBank.h"
#include "MDF/RawEventPrintout.h"
#include "MDF/RawEventHelpers.h"
#include "MBMDump/MBMDump.h"
using namespace LHCb;
using namespace MBMDump;

BankTypesWindow::BankTypesWindow(BaseMenu* par,int cmd_id, const Format& f, Banks& b)
: BankListWindow(par,cmd_id,f,b,false), m_bankWindow2(0)
{
  char txt[256];
  setParent(par);
  m_parentCmd = cmd_id;
  openDetached(0,0,"Display window"," List of bank types ",procName());
  addCommand(C_DISMISS,"Dismiss");
  addComment(C_COM2,"");
  addComment(C_COM3," Hit return on family to see banks list");
  addComment(C_COM4,"");
  addComment(C_COM5,"+------------------------------------------+");
  addComment(C_COM6,"| Type         Type #      Vsn    No.Banks |");
  addComment(C_COM7,"+------------------------------------------+");
  for(Banks::iterator k=m_banks.begin(); k!=m_banks.end();++k)  {
    const RawBank* bank = (*k).second;
    std::pair<int,int> typ(bank->type(),bank->version());
    BankMap::iterator i = m_map.find(typ);
    if ( i == m_map.end() ) m_map.insert(std::make_pair(typ,std::make_pair(bank,0)));
    m_map[typ].second++;
  }
  int cnt = 0;
  for(BankMap::iterator i=m_map.begin(); i != m_map.end(); ++i, ++cnt)  {
    const RawBank* bank = (*i).second.first;
    ::snprintf(txt,sizeof(txt),"  %-16s %2d %8d %11d",
      RawEventPrintout::bankType(bank->type()).c_str(),bank->type(),
      (*i).first.second,(*i).second.second);
    addCommand(C_TYPES+cnt,txt);
  }
  addCommand(C_DISMISS2,"Dismiss");
  closeMenu();
  setCursor(C_DISMISS,1);
}

BankTypesWindow::~BankTypesWindow()  {
  drop(m_bankWindow);
  deleteMenu();
}

void BankTypesWindow::handleMenu(int cmd_id)    {
  if ( cmd_id != C_DISMISS && cmd_id >= C_TYPES ) {
    int typ = cmd_id-C_TYPES;
    m_banksOfType.clear();
    int cnt = 0;
    for(BankMap::iterator i=m_map.begin(); i != m_map.end(); ++i, ++cnt)  {
      if ( cnt == typ )  {
        for(Banks::iterator j=m_banks.begin(); j!=m_banks.end(); ++j)  {
          if ( (*i).first.first  == (*j).second->type() &&
               (*i).first.second == (*j).second->version() )  {
              m_banksOfType.push_back(*j);
          }
        }
        replace(m_bankWindow2,new BankListWindow(this,cmd_id,m_fmt,m_banksOfType));
        return;
      }
    }
  }
  return this->BankListWindow::handleMenu(cmd_id);
}

