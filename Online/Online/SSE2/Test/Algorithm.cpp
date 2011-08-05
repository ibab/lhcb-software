#include "Framework/Kernel.h"
#include "Framework/DataflowMgr.h"
#include "Framework/ItemRegistry.h"
#include "Algorithm.h"
#include "RTL/rtl.h"

using namespace std;
using namespace Framework;
using namespace FrameworkTest;

/// Default constructor
IODef::IODef() : vector<string>() {
}

/// Initializing constructor
IODef::IODef(const char* in[])  {
  for (size_t i=0; in[i]; ++i) {
    this->push_back(in[i]);
  }
}

/// Copy constructor
IODef::IODef(const IODef& c) : vector<string>(c) {
}

int FrameworkTest::A__declareIO(Framework::ExecutorFactory* f,const StrVector& in, const StrVector& out)  {
  DataflowMgr* mgr = (DataflowMgr*)f->param();
  ItemRegistry& reg = mgr->dataRegistry();
  ItemRegistry::Mask_t maskIn  = reg.registerItems(in);
  ItemRegistry::Mask_t maskOut = reg.registerItems(out);
  f->setInputMask(maskIn);
  f->setOutputMask(maskOut);
  mgr->adoptFactory(f);
  return 1;
}
#include <cmath>
/// Event callback
int FrameworkTest::A__execute(void* instance, const type_info& /* t */,const string& n, void* par, const void* arg)   {
  //Algorithm* a = (Algorithm*)instance;
  EventContext* context = (EventContext*)par;
  pair<int, int>* tmo = (pair<int, int>*)arg;
  double r1 = double(::rand())/double(RAND_MAX);
  double r2 = double(::rand())/double(RAND_MAX);
  double sleep = ::abs(::sqrt(-2.*::log(r1)) * ::cos(2*3.14159*r2)) * double(tmo->second) + double(tmo->first);
  sleep *= 1.0;
  ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT"'%s': Executing. Context:0x%p [%d] time:%7.2f\n",
    "Algorithm", instance, n.c_str(), context, context->id(),sleep);
  if ( sleep > 0 )   {
    ::lib_rtl_sleep(int(sleep));
  }
  return 1;
}

/// Declare input data content to data registry
Status FrameworkTest::ContextFactory::declareData()   {
  DataflowMgr* mgr = (DataflowMgr*)m_param;
  ItemRegistry& reg = mgr->dataRegistry();
  ItemRegistry::Mask_t mask = reg.registerItems(m_data);
  m_mask.set_bits(mask);
  return 1;
}
