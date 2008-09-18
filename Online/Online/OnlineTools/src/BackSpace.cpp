#include "Defs.h"
#include "CPP/Interactor.h"
#include "CPP/Event.h"
void OnlineTools::backSpaceCallBack (int menu,int /* cmd */,int par,const void* param)  {
  Event ev;
  ev.target     = (Interactor*)param;
  ev.eventtype  = UpiEvent;
  ev.menu_id    = menu;
  ev.command_id = CMD_BACKSPACE;
  ev.param_id   = par;
  ev.index_id   = 0;
  ev.target->handle(ev);
}
