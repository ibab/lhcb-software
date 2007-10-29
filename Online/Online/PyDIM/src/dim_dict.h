#include "DimPython.h"
#ifdef __GNUC__
#include <stdint.h>
#define longlong aStupidHackForAmd64
#include "Reflex/Kernel.h"
#undef longlong
#endif
#include "dis.hxx"
#include "dic.hxx"

namespace DIM  {

  /** @class InfoHandler
    * 
    * Implement DIM InfoHandler in Python.
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class InfoHandler : public ::DimInfoHandler {
  public:
    /// Standard constructor: Argument self: instance to python object implementation
    InfoHandler(PyObject* self) : call(self) { call.m_type=&typeid(*this); itsService = 0;      }
    /// Default destructor
    virtual ~InfoHandler()          {                          }
    /// DIM overloaded callback calling python itself.
    virtual void infoHandler()      { call("infoHandler");     }
    /// Dressed python callback
    Caller call;
  };

  /** @class InfoHandler
    * 
    * Implement DIM InfoHandler in Python.
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class Info : public ::DimInfo {
  public:
    /// Standard constructor for string infos
    Info(PyObject* self, const std::string& name, const std::string& def) 
    : ::DimInfo(name.c_str(),(char*)def.c_str()), call(self) { call.m_type=&typeid(*this);  }
    /// Standard constructor for int infos
    Info(PyObject* self, const std::string& name, int def) 
    : ::DimInfo(name.c_str(),def), call(self) { call.m_type=&typeid(*this);  }
    /// Standard constructor for float infos
    Info(PyObject* self, const std::string& name, float def) 
    : ::DimInfo(name.c_str(),def), call(self) { call.m_type=&typeid(*this);  }
    /// Standard constructor for double infos
    Info(PyObject* self, const std::string& name, double def) 
    : ::DimInfo(name.c_str(),def), call(self) { call.m_type=&typeid(*this);  }
    /// Default destructor
    virtual ~Info()          {                          }
    /// DIM overloaded callback calling python itself.
    virtual void infoHandler()      { call("infoHandler");     }
    /// Dressed python callback
    Caller call;
  };

  /** @class Python CommandHandler
    * 
    * Implement DIM commandhandler in Python.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class CommandHandler : public ::DimCommandHandler {
  public:
    /// Standard constructor: Argument self: instance to python object implementation
    CommandHandler(PyObject* self) : call(self) { call.m_type=&typeid(*this); itsCommand = 0;   }
    /// Default destructor
    virtual ~CommandHandler()       {                          }
    /// DIM overloaded callback calling python itself.
    virtual void commandHandler()   { call("commandHandler");  }
    /// Dressed python callback
    Caller call;
  };

  /** @class ServiceHandler
    * 
    * Implement DIM ServiceHandler in Python.
    * 
    * @author  M.Frank
    * @version 1.0
    */
  struct ServiceHandler : public ::DimServiceHandler {
    /// Standard constructor: Argument self: instance to python object implementation
    ServiceHandler(PyObject* self) : call(self) { call.m_type=&typeid(*this); itsService = 0;   }
    /// Default destructor
    virtual ~ServiceHandler()       {                          }
    /// DIM overloaded callback calling python itself.
    virtual void serviceHandler()   { call("serviceHandler");  }
    /// Dressed python callback
    Caller call;
  };

  /** @class ClientExitHandler
    * 
    * Implement DIM ClientExitHandler in Python.
    * 
    * @author  M.Frank
    * @version 1.0
    */
  struct ClientExitHandler : public ::DimClientExitHandler {
    /// Standard constructor: Argument self: instance to python object implementation
    ClientExitHandler(PyObject* self) : call(self) { call.m_type=&typeid(*this);                }
    /// Default destructor
    virtual ~ClientExitHandler()    {                            }
    /// DIM overloaded callback calling python itself.
    virtual void clientExitHandler(){ call("clientExitHandler"); }
    /// Dressed python callback
    Caller call;
  };

  /** @class ExitHandler
    * 
    * Implement DIM ExitHandler in Python.
    * 
    * @author  M.Frank
    * @version 1.0
    */
  struct ExitHandler : public ::DimExitHandler {
    /// Standard constructor: Argument self: instance to python object implementation
    ExitHandler(PyObject* self) : call(self) {  call.m_type=&typeid(*this);                     }
    /// Default destructor
    virtual ~ExitHandler()          {                          }
    /// DIM overloaded callback calling python itself.
    virtual void exitHandler()      { call("exitHandler");     }
    /// Dressed python callback
    Caller call;
  };

  template<class T> T _exec(::DimBrowser* b,int (::DimBrowser::*pfn)(char*&,char*&)){
    char* s1, *s2;
    T e;
    e.result = (b->*pfn)(s1,s2);
    if ( e.result )  {
      e.name = s1;
      e.fmt_node = s2;
    }
    return e;
  }

  /** @class Browser
    * 
    * Implement DIM Browser in Python.
    * 
    * @author  M.Frank
    * @version 1.0
    */
  struct Browser {
    ::DimBrowser b;
    /// Due to the ^%@$&%!&@# iteration structure, this is necessary to get anything out.
    struct Entry  {
      int result;
      std::string name, fmt_node;
      Entry() : result(0) {}
      Entry(const Entry& e): result(e.result), name(e.name), fmt_node(e.fmt_node) {}
      ~Entry() {}
    };
    /// Standard constructor
    Browser()  {}
    /// Default destructor
    virtual ~Browser() {}    
    int getServices(const char *nam)        { return b.getServices(nam);        }
    int getServers()                        { return b.getServers();            }
    int getServerServices(const char *nam)  { return b.getServerServices(nam);  }
    int getServerClients(const char *nam)   { return b.getServerClients(nam);   }
    Entry getNextService()       { return _exec<Entry>(&b, &::DimBrowser::getNextService);       }
    Entry getNextServer()        { return _exec<Entry>(&b, &::DimBrowser::getNextServer);        }
    Entry getNextServerService() { return _exec<Entry>(&b, &::DimBrowser::getNextServerService); }
    Entry getNextServerClient()  { return _exec<Entry>(&b, &::DimBrowser::getNextServerClient);  }
  };
}   // End namespace DIM
