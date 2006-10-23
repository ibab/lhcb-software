#include "MDF/RawEventPrintout.h"
#include "MDF/RawEventHelpers.h"
#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"
using namespace LHCb;
using namespace MBMDump;

BankTypesWindow::BankTypesWindow(BaseMenu* par,int cmd_id, const Format& f, Banks& b)
: BankListWindow(par,cmd_id,f,b,false), m_bankWindow2(0)
{
  char txt[256];
  setParent(par);
  m_parentCmd = cmd_id;
  ::upic_open_detached_menu(id(),0,0,"Display window"," MEP Fragment structure ",procName());
  ::upic_add_command(C_DISMISS,"Dismiss","");
  ::upic_enable_action_routine(id(),C_DISMISS, Routine(BaseMenu::dispatch));
  ::upic_add_comment(C_COM2,"","");
  ::upic_add_comment(C_COM3," Hit return on family to see banks list","");
  ::upic_add_comment(C_COM4,"","");
  ::upic_add_comment(C_COM5,"+----------------------------------------------+","");
  ::upic_add_comment(C_COM6,"| Type     Type #  Vsn    No.Banks   |","");
  ::upic_add_comment(C_COM7,"+----------------------------------------------+","");
  for(Banks::iterator k=m_banks.begin(); k!=m_banks.end();++k)  {
    const RawBank* b = (*k).second;
    std::pair<int,int> typ(b->type(),b->version());
    BankMap::iterator i = m_map.find(typ);
    if ( i == m_map.end() ) m_map.insert(std::make_pair(typ,std::make_pair(b,0)));
    m_map[typ].second++;
  }
  int cnt = 0;
  for(BankMap::iterator i=m_map.begin(); i != m_map.end(); ++i, ++cnt)  {
    const RawBank* b = (*i).second.first;
    ::sprintf(txt," %-12s %2d %8d %5d",
      RawEventPrintout::bankType(b->type()).c_str(),b->type(),
      (*i).first.second,(*i).second.second);
    ::upic_add_command(C_TYPES+cnt,txt,"");
    ::upic_enable_action_routine(id(),C_TYPES+cnt, Routine(BaseMenu::dispatch));
  }
  ::upic_close_menu();
  ::upic_set_cursor(id(),C_DISMISS,1);
}

BankTypesWindow::~BankTypesWindow()  {
  drop(m_bankWindow);
  ::upic_delete_menu(id());
}

void BankTypesWindow::handleMenu(int cmd_id)    {
  if ( cmd_id != C_DISMISS && cmd_id >= C_TYPES ) {
    int typ = cmd_id-C_TYPES;
    m_banksOfType.clear();
    int cnt = 0;
    for(BankMap::iterator i=m_map.begin(); i != m_map.end(); ++i, ++cnt)  {
      if ( cnt == typ )  {
        for(Banks::iterator j=m_banks.begin(); j!=m_banks.end(); ++j)  {
          if ( (*i).first.first  == (*j).second->version() &&
               (*i).first.second == (*j).second->type() )  {
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

