#include "GaudiUPI/DialogMenu.h"
#include "GaudiUPI/DisplayFactory.h"
#include "RTL/rtl.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "UPI/UpiSensor.h"

namespace {
  DialogMenu* m_window;
  class Test : public Interactor  {
    enum RunControllerCommands
    {
      COMMENT_CONTEXT = 200000,
      CMD_EXPAND_INTERFACE,
      CMD_MICKEY,
      COMMENT0,
      CMD_DAQ_PREPARE,
      CMD_DAQ_STOP,
      CMD_DAQ_FLUSH,
      COMMENT1,
      CMD_CHANGE_RUN,
      COMMENT2,
      CMD_TRIG_ENABLE,
      CMD_TRIG_DISABLE,
      CMD_TRIG_ABORT,
      COMMENT3,
      CMD_RUN_CANCEL,
      CMD_RUN_RESET,
      CMD_RUN_TERMINATE
    };
  public:
    Test()  {
      DialogMenuCreator win(this);
      m_window= win->create("Title1","Title2","Title3");
      win->addCMD(CMD_EXPAND_INTERFACE,"Expand Interface                 ");
      win->addCMD(CMD_MICKEY,          "Recovery Menu (formerly Mickey)  ");
      win->addCOM(COMMENT0,            "---------------------------------");
      win->addCMD(CMD_DAQ_PREPARE,     "Daq>            Prepare          ");
      win->addCMD(CMD_DAQ_STOP,        "                Stop             ");
      win->addCMD(CMD_DAQ_FLUSH,       "Daq Error>      Flush            ");
      win->addCOM(COMMENT1,            "---------------------------------");
      win->addCMD(CMD_TRIG_ENABLE,     "Trigger>        enable           ");
      win->addCMD(CMD_TRIG_DISABLE,    "                disable          ");
      win->addCMD(CMD_TRIG_ABORT,      "Trigger Error>  Abort            ");
      win->addCOM(COMMENT2,            "---------------------------------");
      win->addCMD(CMD_CHANGE_RUN,      "Change>         Change Run       ");
      win->addCOM(COMMENT3,            "---------------------------------");
      win->addCMD(CMD_RUN_RESET,       "Error>          Reset            ");
      win->addCMD(CMD_RUN_CANCEL,      "                Cancel           ");
      win->addCMD(CMD_RUN_TERMINATE,   "Exit>           Terminate        ");
      win->map();
    }

    virtual  void handle (const Event& event)   {
      char buff[132];
      switch ( event.eventtype )    {
        case UpiEvent:
          sprintf(buff,"CMD=%d,Param=%d,IDX=%d",event.command_id, event.param_id, event.index_id );
          upic_write_message(buff,"");
          if ( event.command_id == CMD_RUN_TERMINATE )  {
            m_window->destroy();
            exit(0);
          }
          break;
        default:
          break;
      }
    }
  };
  void help() {}
}

extern "C" int GUPI_test1(int argc, char** argv)  {
  RTL::CLI cli(argc, argv, help);
  if ( cli.getopt("debug",1) ) lib_rtl_start_debugger();
  Sensor& s = UpiSensor::instance();
  DisplayFactory f("UPI");
  Test t;
  s.run();
  return 1;
}
