#include <map>
#include <stdexcept>
#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"

static std::map<int, MBMDump::BaseMenu*> s_menuMap;
static int last_menu_id_used = 0;

MBMDump::BaseMenu::BaseMenu() : m_parent(0)  {
  m_ID = ++last_menu_id_used;
  s_menuMap.insert(std::make_pair(m_ID,this));
}

MBMDump::BaseMenu::BaseMenu(BaseMenu* p) : m_parent(p)  {
  m_ID = ++last_menu_id_used;
  s_menuMap.insert(std::make_pair(m_ID,this));
}

MBMDump::BaseMenu::~BaseMenu() {
  std::map<int, BaseMenu*>::iterator i = s_menuMap.find(m_ID);
  if ( i != s_menuMap.end() ) s_menuMap.erase(i);
}

MBMDump::BaseMenu& MBMDump::BaseMenu::parent()  const  {  
  if ( m_parent ) return *m_parent; 
  throw std::runtime_error("Invalid pointer to parent window");
}

int MBMDump::BaseMenu::run()  {
  int menu_id, command_id, param_id; 
  while(1)  {
    ::upic_get_input(&menu_id,&command_id,&param_id);
  }
  return 1;
}

int MBMDump::BaseMenu::dispatch(int menu_id, int cmd_id, ...)  {
  try  {
    BaseMenu* r = s_menuMap[menu_id];
    r->handleMenu(cmd_id);
  }
  catch(const std::exception& e)  {
    ::upic_write_message2("Error during menu handling: %s",e.what());
  }
  catch(...)  {
    ::upic_write_message("Unknown error during menu handling.","");
  }
  return 1;
}

