#include "ROLoggerDefs.h"
#include "CPP/Interactor.h"
#include "CPP/Event.h"

std::string ROLogger::fmcLogger(const std::string& host,const std::string& facility) {
  char text[128];
  std::string h=host;
  for(size_t i=0; i<host.length(); ++i)h[i]=::toupper(h[i]);
  ::sprintf(text,"/FMC/%s/logger/%s/log",h.c_str(),facility.c_str());
  return text;
}

void ROLogger::backSpaceCallBack (int menu,int /* cmd */,int par,const void* param)  {
  Event ev;
  ev.target     = (Interactor*)param;
  ev.eventtype  = UpiEvent;
  ev.menu_id    = menu;
  ev.command_id = CMD_BACKSPACE;
  ev.param_id   = par;
  ev.index_id   = 0;
  ev.target->handle(ev);
}
