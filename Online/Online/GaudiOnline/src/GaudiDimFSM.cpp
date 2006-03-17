#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiOnline/GaudiDimFSM.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/SmartIF.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"

#include <iostream>
#include <fstream>
#include <sstream>

extern "C" 
#ifdef _WIN32
__declspec(dllimport) 
#endif 
int GaudiMain(int, char**);

static void help() {}

extern "C" int GaudiOnline(int argc, char** argv)  {
  RTL::CLI cli(argc, argv, help);
  std::string runable = "AppMgrRunable";
  std::string evtloop = "EventLoopMgr";
  std::string msgsvc  = "OnlineMessageSvc";
  std::string opts    = "jobOptions.txt";
  SmartIF<IProperty> p(Gaudi::createApplicationMgr());
  if(cli.getopt("runable",6,runable)) p->setProperty("Runable",runable);
  if(cli.getopt("evtloop",6,evtloop)) p->setProperty("EventLoop",evtloop);
  if(cli.getopt("msgsvc", 6,msgsvc) ) p->setProperty("MessageSvcType",msgsvc);
  if(cli.getopt("options",6,opts)   ) p->setProperty("JobOptionsPath",opts);

  if ( cli.getopt("debug",5) != 0 )  {
    ::lib_rtl_start_debugger();
  }
  if ( p )  {
    size_t nargs = argc;
    char** args = argv;
    std::stringstream os;
    for ( size_t i=2; i<nargs; ++i ) {
      const char* cptr = *++args;
      if ( *cptr == '-' || *cptr == '/' )  {
        std::cout << "Option:" << cptr << std::endl;
        continue;
      }
      if ( *cptr++ == '"' )  {
        while(*cptr != '"' )  {
          if ( *cptr != '|' ) os << *cptr;
          if ( *cptr == ';' ) os << std::endl;
          cptr++;
        }
      }
    }
    if ( os.str().length() > 0 ) {
      std::fstream file;
#ifdef _WIN32
      std::string tmpnam = ::getenv("TMP") ? ::getenv("TMP") : ::getenv("TEMP");
#else
      std::string tmpnam = "/tmp";
#endif
      tmpnam += "/tmpopts.opts";
      file.open(tmpnam.c_str(), std::ios_base::out|std::ios_base::trunc);
      file << "#include \"" << opts << "\"" << std::endl << os.str() << std::endl;
      file.close();
      p->setProperty("JobOptionsPath",tmpnam);
    }
  }
  SmartIF<IAppMgrUI> ui(p);
  return LHCb::GaudiDimFSM(ui).run();
}

LHCb::GaudiDimFSM::GaudiDimFSM(IAppMgrUI* ui) : DimTaskFSM(true), m_appMgr(ui)  {
  if (m_appMgr) m_appMgr->addRef();
  IOCSENSOR.send(this, CONFIGURE);
}

LHCb::GaudiDimFSM::~GaudiDimFSM()  {
  if ( m_appMgr ) m_appMgr->release();
  m_appMgr= 0;
}

StatusCode LHCb::GaudiDimFSM::cancel()  {
  // Todo:  Need to somehow issue MBM Cancel!
  return StatusCode::SUCCESS;
}

StatusCode LHCb::GaudiDimFSM::config()  {
  return m_appMgr->configure().isSuccess() ? DimTaskFSM::init()
    : printErr(1,"Failed to configure Application manager.");
}

StatusCode LHCb::GaudiDimFSM::init()  {
  return m_appMgr->initialize().isSuccess() ? DimTaskFSM::init()
    : printErr(1,"Failed to initialize Application manager.");
}
	
StatusCode LHCb::GaudiDimFSM::process()  {
  return m_appMgr->nextEvent(1).isSuccess()  ? DimTaskFSM::process()
    : printErr(1,"Failed to execute single shot.");
}
	
StatusCode LHCb::GaudiDimFSM::finalize()  {
  if ( !m_appMgr->finalize().isSuccess() )
    printErr(0,"Failed to finalize Application manager.");
  return DimTaskFSM::finalize();
}

StatusCode LHCb::GaudiDimFSM::terminate()  {
  if ( !m_appMgr->terminate().isSuccess() )
    printErr(0,"Failed to terminate Application manager.");
  return DimTaskFSM::terminate();
}
