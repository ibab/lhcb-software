// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/BaseMenu.cpp,v 1.5 2010-09-07 13:50:43 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: BaseMenu.cpp,v 1.5 2010-09-07 13:50:43 frankb Exp $
//
// C++ include files
#include <map>
#include <stdexcept>
#include <cstdarg>
#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif
#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"
using namespace MBMDump;
static std::map<int, BaseMenu*> s_menuMap;
static int last_menu_id_used = 0;

BaseMenu::BaseMenu() : m_parent(0)  {
  m_ID = ++last_menu_id_used;
  s_menuMap.insert(std::make_pair(m_ID,this));
}
BaseMenu::BaseMenu(BaseMenu* p) : m_parent(p)  {
  m_ID = ++last_menu_id_used;
  s_menuMap.insert(std::make_pair(m_ID,this));
}
BaseMenu::~BaseMenu() {
  std::map<int, BaseMenu*>::iterator i = s_menuMap.find(m_ID);
  if ( i != s_menuMap.end() ) s_menuMap.erase(i);
}
BaseMenu& BaseMenu::parent()  const  {  
  if ( m_parent ) return *m_parent; 
  throw std::runtime_error("Invalid pointer to parent window");
}
int BaseMenu::run()  {
  int menu_id, command_id, param_id, do_run=1; 
  while(do_run>0)  {
    ::upic_get_input(&menu_id,&command_id,&param_id);
  }
  return 1;
}
void BaseMenu::output(const char* fmt, ... )  {
  char buff[1024];
  va_list lst;
  va_start(lst, fmt);
  vsnprintf(buff,sizeof(buff),fmt,lst);
  va_end(lst);
  ::upic_write_message(buff,"");
}
void BaseMenu::openMenu(int par_id, int cmd_id, const char* up, const char* tit, const char* down)  {
  ::upic_open_menu(id(), par_id, cmd_id, up, tit, down);
}
void BaseMenu::openDetached(int par_id, int cmd_id, const char* up, const char* tit, const char* down)  {
  ::upic_open_detached_menu(id(), par_id, cmd_id, up, tit, down);
}
void BaseMenu::changeTitles(const char* up, const char* tit, const char* down) {
  ::upic_change_titles(id(),up,tit,down);
}
void BaseMenu::closeMenu()  {
  ::upic_close_menu();
}
void BaseMenu::deleteMenu()  {
  ::upic_delete_menu(id());
}
void BaseMenu::hideMenu()  {
  ::upic_hide_menu(id());
}
int BaseMenu::quit()  {
  return ::upic_quit();
}
void BaseMenu::setCursor(int cmd_id, int par_id)  {
  ::upic_set_cursor(id(),cmd_id,par_id);
}
void BaseMenu::setCursor(int menu_id, int cmd_id, int par_id)  {
  ::upic_set_cursor(menu_id,cmd_id,par_id);
}
void BaseMenu::addComment(int id, const char* txt)  {
  ::upic_add_comment(id,txt,"");
}
void BaseMenu::replaceComment(int com_id, const char* txt)  {
  ::upic_replace_comment(id(), com_id,txt,"");
}
void BaseMenu::addCommand(int cmd_id, const char* txt, bool enable)  {
  ::upic_add_command(cmd_id,txt,"");
  if ( enable ) ::upic_enable_action_routine(id(),cmd_id, Routine(BaseMenu::dispatch));
}
void BaseMenu::replaceCommand(int com_id, const char* txt)  {
  ::upic_replace_command(id(), com_id,txt,"");
}
void BaseMenu::deleteCommand(int cmd_id)  {
  ::upic_delete_command(id(), cmd_id);
}
void BaseMenu::enableCommand(int cmd_id)  {
  ::upic_enable_command(id(), cmd_id);
}
void BaseMenu::insertCommand(int pos_id, int cmd_id, const char* txt, bool enable)  {
  ::upic_insert_command(id(),pos_id,cmd_id,txt,"");
  if ( enable ) ::upic_enable_action_routine(id(),cmd_id, Routine(BaseMenu::dispatch));
}
void BaseMenu::enableCommands(int n, ...)  {
  va_list ap;
  va_start(ap,n);
  for(int i=0; i<n; ++i) {
    ::upic_enable_command(id(),va_arg(ap,int));
  }
  va_end(ap);
}
void BaseMenu::disableCommands(int n, ...)   {
  va_list ap;
  va_start(ap,n);
  for(int i=0; i<n; ++i)  {
    ::upic_disable_command(id(),va_arg(ap,int));
  }
  va_end(ap);
}
void BaseMenu::setParam(char* var, int par_id, const char* fmt, const char* def, const char* mi, const char* ma, const char** lst,int len,  bool flg)  {
  ::upic_set_param  (var, par_id,fmt,def,mi,ma,lst,len,flg);
}
void BaseMenu::setParam(int* var, int par_id, const char* fmt, const int def, const int mi, const int ma, const int* lst,int len,  bool flg)    {
  ::upic_set_param  (var, par_id,fmt,def,mi,ma,lst,len,flg);
}
void BaseMenu::setParam(float* var, int par_id, const char* fmt, const float def, const float mi, const float ma, const float* lst,float len,  bool flg)    {
  ::upic_set_param  (var, par_id,fmt,def,mi,ma,lst,len,flg);
}
void BaseMenu::setParam(unsigned int* var, int par_id, const char* fmt, const unsigned int def, const unsigned int mi, const unsigned int ma, const unsigned int* lst,int len,  bool flg)    {
  ::upic_set_param  (var, par_id,fmt,def,mi,ma,lst,len,flg);
}
void BaseMenu::openWindow()  {
  ::upic_open_window();
}
void BaseMenu::openOldWindow()  {
  ::upic_open_old_window(id());
}
int BaseMenu::dispatch(int menu_id, int cmd_id, ...)  {
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

