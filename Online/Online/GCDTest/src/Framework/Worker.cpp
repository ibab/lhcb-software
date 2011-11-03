#include "RTL/rtl.h"
#include "Framework/Kernel.h"
#include "Framework/Worker.h"
#include "Framework/IExecutor.h"
#include "Framework/IDataflowMgr.h"

#include <memory>

using namespace Framework;
using namespace std;

/// Static thread startup routing
void Worker::exec(void* arg) {
  Worker::Args* args((Worker::Args*)arg);
  args->worker->run(args->executor, args->context);
}

/// Thread ACTIVE routine: here the work load is executed by the thread
void Worker::run(Executor* executor, EventContext* context)    {
  struct timeval start = {0,0};
  ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT "Started...","Worker", this);
  try  {
    ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT "Start processing event: 0x%p Context:0x%p.",
		     "Worker", this, executor, context);
    start = Timing::now();
    executor->execute(context);
  }
  catch(const std::exception& e) {
    ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT "Worker exception: %s.",
		     "Worker", this,e.what());
  }
  catch(...) {
    ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT "Worker exception [Unknown].",
		     "Worker", this);
  }
  // Now finalize the request
  m_master->workerDone(this,executor,context,start,Timing::now());
}
