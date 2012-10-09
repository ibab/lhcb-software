#ifndef TESTRACER_ITESTRACER_H
#define TESTRACER_ITESTRACER_H

#include <string>
class IRegistry;
class IService;

enum TracerCommand  {
  TRACE_CLEAR          =1<<0,
  TRACE_CLEAR_ROOT     =1<<1,
  TRACE_TRAVERSE       =1<<2,
  TRACE_SETROOT        =1<<3,
  TRACE_REGADDR        =1<<4,
  TRACE_UNREGADDR      =1<<5,
  TRACE_REGOBJ         =1<<6,
  TRACE_UNREGOBJ       =1<<7,
  TRACE_DATAFAULT      =1<<8,
  TRACE_DATAFAULT_OK   =1<<9,
  TRACE_DATAFAULT_FAIL =1<<10,
  TRACE_LOAD           =1<<11,
  TRACE_RETRIEVE       =1<<12,
  TRACE_FIND           =1<<13,
  TRACE_LINK           =1<<14,
  TRACE_UNLINK         =1<<15,
  TRACE_NONE,
  LAST
};


struct TESTracer {
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void trace(IService* svc, TracerCommand cmd, IRegistry* pReg) = 0;
  virtual void trace(IService* svc, TracerCommand cmd, IRegistry* pReg, const std::string& path) = 0;
  static TESTracer& instance();
};

struct TESTracerGuard {
  TESTracerGuard()  { TESTracer::instance().start(); }
  ~TESTracerGuard() { TESTracer::instance().stop();  }
};
#define _TRACE_TES TESTracerGuard guard; TESTracer::instance().trace

#endif // TESTRACER_ITESTRACER_H
