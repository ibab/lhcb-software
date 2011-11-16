#include "ROLoggerDefs.h"
#include "CPP/Interactor.h"
#include "CPP/Event.h"
#include <cstdio>

using namespace std;

string ROLogger::fmcLogger(const string& host,const string& facility) {
  char text[128];
  string h=host;
  for(size_t i=0; i<host.length(); ++i)h[i]=char(::toupper(h[i]));
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
