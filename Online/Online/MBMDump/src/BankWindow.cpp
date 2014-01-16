// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/BankWindow.cpp,v 1.5 2008-02-12 17:13:47 frankb Exp $
//  ====================================================================
//  BankWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: BankWindow.cpp,v 1.5 2008-02-12 17:13:47 frankb Exp $
//
// C++ include files
#include <string>

// Framework include files
#include "Event/RawBank.h"
#include "MDF/RawEventPrintout.h"
#include "MBMDump/MBMDump.h"

using namespace LHCb;
using namespace MBMDump;

BankWindow::BankWindow(BaseMenu* par,int cmd_id, const Format& f, const RawBank* b)
: BaseMenu(par), m_parentCmd(cmd_id), m_fmt(f)
{
  char txt[256], tmp[32];
  const unsigned int *data = b->data(), *end = b->end<unsigned int>();

  openDetached(0,0,"Display window"," MEP Fragment structure ",procName());
  addCommand(C_DISMISS,"Dismiss");
  ::snprintf(txt,sizeof(txt),"RawBank  %s",RawEventPrintout::bankHeader(b).c_str());
  addComment(C_COM1,txt);
  txt[0] = 0;

  int nw = m_fmt.words_per_line;
  int cw = 10;  // Column 1 width
  int dw = 4;   // Distance between HEX and ascii dump
  int fw = 11;  // Width of HEX   format
  int aw = m_fmt.ascii_flag ? 4 : 0;  // Width of ASCII format
  int cnt, num, lines;
  for(cnt = 0, lines = 0, num=C_DATA; data<end; ++cnt, ++data)  {
    if ( (cnt%nw) == 0 )  {
      ++lines;
      if ( 0 == (lines%10) )
        addCommand(++num,txt,false);
      else  
        addComment(++num,txt);

      memset(txt,' ',sizeof(txt));
      // 0=nothing,1=offset,2=line number
      if(m_fmt.column_one_flag == 1)  {
        ::snprintf(tmp,sizeof(tmp),"%4d",(int)((char*)data-(char*)b->data()));
        memcpy(txt+3,tmp,4);
      }
      else if(m_fmt.column_one_flag == 2)  {
        ::snprintf(tmp,sizeof(tmp),"%4d",(int)((data-b->data())/m_fmt.words_per_line));
        ::memcpy(txt+3,tmp,4);
      }
      else  {
        cw = 2;
      }
      txt[cw+nw*fw+dw+aw*nw] = 0;
      cnt = 0;
    }
    ::snprintf(tmp,sizeof(tmp),m_fmt.fmt[cnt],*data);
    // sprintf(tmp,"%08X",*data);
    ::memcpy(&txt[cw+cnt*fw],tmp,strlen(tmp));
    if ( m_fmt.ascii_flag )  {
      rconv(tmp,*data);
      ::memcpy(&txt[cw+fw*nw+dw+aw*cnt],tmp,4);
    }
  }
  addComment(++num,txt);
  addCommand(C_DISMISS2,"Dismiss");
  closeMenu();
  setCursor(C_DISMISS,1);
}

BankWindow::~BankWindow()  {
  deleteMenu();
}

void BankWindow::handleMenu(int cmd_id)    {
  switch(cmd_id)  {
    case C_DISMISS:
    case C_DISMISS2:
      hideMenu();
      parent().setCursor(m_parentCmd,1);
      break;
    default:
      break;
  }
}

