// $ID: $

// Include Files
#include "GaudiUPI/SubMenu.h"
#include "GaudiUPI/DialogMenu.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include <cstdarg>

using namespace std;
extern "C" void upic_write_message(const char*, const char*);

/// Initializing constructor
Gaudi::SubMenu::SubMenu(const string& title, CPP::Interactor* parent)
: m_window(0), m_parent(parent), m_child(0)
{
  DialogMenuCreator win(this);
  m_window = win->createDetached(RTL::processName(),title,RTL::processName());
}

/// Standard destructor
Gaudi::SubMenu::~SubMenu()  {
  if ( m_window )  {
    m_window->destroy();
    delete m_window;
    m_window = 0;
  }
}

void Gaudi::SubMenu::mapWindow(bool defCommands)  {
  if ( m_window )  {
    if ( defCommands )  {
      m_window->addCOM(COM_CLOSE, " ");
      m_window->addCMD(CMD_CLOSE, "Close Window");
    }
    m_window->map();
    m_window->installBackspaceCall();
    m_window->setCursor(m_cursor=CMD_CLOSE);
  }
}

/// Show message in the message window
void Gaudi::SubMenu::print(const char* fmt, ...)   {
  char buffer[1024];
  int len;
  va_list arguments;
  va_start( arguments, fmt);
  len = vsprintf(buffer,fmt,arguments);
  buffer[len] = 0;
  va_end ( arguments );
  upic_write_message(buffer,"");
}

/// Interactor overload: handle menu interupts
void Gaudi::SubMenu::handle (const CPP::Event& event)   {
  if ( event.eventtype == UpiEvent )  {
    int cmd = event.command_id;
    if ( cmd == CMD_CLOSE || cmd == DIALOGMENU_BACK_SPACE )   {
      // Close of submenus
      m_window->destroy();
      delete m_window;
      m_window = 0;
      IOCSENSOR.send(m_parent, CMD_CLOSE, this);
    }
  }
  else if ( event.eventtype == IocEvent )  {
    if ( event.type == CMD_CLOSE )  {
      m_window->setCursor(m_cursor);
      if ( m_child ) delete m_child;
      m_child = 0;
    }
  }
}
