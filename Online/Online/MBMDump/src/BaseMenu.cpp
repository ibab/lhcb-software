#include <map>
#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"

static std::map<int, MBMDump::BaseMenu*> s_menuMap;

MBMDump::BaseMenu::BaseMenu() {
  static int last_menu_id_used = 0;
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
    std::string err = "Error during menu handling:";
    err += e.what();
    ::upic_write_message(err.c_str(),"");
  }
  catch(...)  {
    ::upic_write_message("Unknown error during menu handling.","");
  }
  return 1;
}

