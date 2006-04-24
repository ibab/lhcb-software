#include "Gaudi/DialogMenu.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"

static DialogFactory* s_dlgFactory;

DialogFactory::~DialogFactory() {
  s_dlgFactory = 0;
}

DialogFactory::DialogFactory () {
  s_dlgFactory = this;
}

DialogFactory* DialogFactory::instance() {
  return s_dlgFactory;
}

void DialogMenu::handle(const Event& event)  {
  if ( actor() != 0 ) 
    actor()->handle(event);
  else
    printf("DialogMenu::handle> handle not connected to any interactor !\n");
}

long DialogMenu::BackSpaceCallBack (int menu,int cmd,int par,ClientData param)  {
  Event ev;
  ev.target     = (Interactor*)param;
  ev.eventtype  = UpiEvent;
  ev.menu_id    = menu;
  ev.command_id = -1;
  ev.param_id   = par;
  ev.index_id   = 0;
  ev.target->handle(ev);
  return 1;
}

DialogStatus DialogMenu::AccumulateComment (int pos_id,int& com_id,int max_lines,char* def_text,std::string& display_text)  {
  int status = DIALOG_SUCCESS;
  size_t width = 25;
  if ( display_text.length() > width )  {
    std::string def(def_text);
    def += display_text;
    status = (com_id < max_lines) ? replCOM(com_id++,def.c_str()) : insertCOM(pos_id,com_id++,def.c_str());
    display_text = "";
  }
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}

DialogStatus DialogMenu::FlushComment (int pos_id,int& com_id,int max_lines,char* def_text,std::string& display_text)
{
  int status = DIALOG_SUCCESS;  
  size_t width = 25;
  if ( display_text.length() > width )  {
    std::string def(def_text);
    def += display_text;
    status = (com_id < max_lines) ? replCOM(com_id++,def.c_str()) : insertCOM(pos_id,com_id++,def.c_str());
    display_text = "";
  }
  return ::lib_rtl_is_success(status) ? DIALOG_SUCCESS : DIALOG_ERROR;
}
