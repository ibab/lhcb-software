#include "Framework/Kernel.h"
#include "Framework/DataflowMgr.h"
#include "Framework/ItemRegistry.h"
#include "Algorithm.h"
#include "RTL/rtl.h"
#include <cmath>

using namespace std;
using namespace Framework;
using namespace FrameworkTest;

/// Default constructor
IODef::IODef() : vector<string>() {}

/// Initializing constructor
IODef::IODef(const char* in[])    {  for (size_t i=0; in[i]; ++i)this->push_back(in[i]);  }

/// Copy constructor
IODef::IODef(const StrVector& c) : vector<string>(c) {}

/// Copy constructor
IODef::IODef(const IODef& c) : vector<string>(c) {}

int FrameworkTest::A__declareIO(Framework::ExecutorFactory* f,const AlgorithmConfig* config)  {
  DataflowMgr* mgr = (DataflowMgr*)f->param();
  f->setInputMask(mgr->ioMask(config->inputs));
  f->setOutputMask(mgr->ioMask(config->outputs));
  mgr->adoptFactory(f);
  return 1;
}

/// Event callback
int FrameworkTest::A__execute(void* instance, const type_info& /* t */,void* evt_context, const AlgorithmConfig* config)   {
  //Algorithm* a = (Algorithm*)instance;
  EventContext* context = (EventContext*)evt_context;
  double r1 = double(::rand())/double(RAND_MAX);
  double r2 = double(::rand())/double(RAND_MAX);
  double sleep = ::abs(::sqrt(-2.*::log(r1)) * ::cos(2*3.14159*r2)) * config->timeSigma + config->time;
  sleep = config->time;
  sleep *= 1.0;
  ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT"'%s': Executing. Context:0x%p [%d] time:%7.2f\n",
    "Algorithm", instance, config->name.c_str(), context, context->id(),sleep);
  if ( sleep > 0 )   {
    ::lib_rtl_usleep(int(sleep));
  }
  return 1;
}

