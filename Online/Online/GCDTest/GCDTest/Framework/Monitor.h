#ifndef FRAMEWORK_IMONITOR_H
#define FRAMEWORK_IMONITOR_H

// C++ include files
#include <string>
#include <sys/time.h>

/*
*    Framework namespace declaration
*/
namespace Framework {
  class Executor;
  class Timing;
  class ExecutorFactory;
  class EventContext;
  class MonitorConfig;

  class Monitor  {
  protected:

    /// Histogram file name
    std::string    m_fileName;
    /// Reference to the configuration block
    MonitorConfig* m_cfg;

  public:
    /// Initializing constructor
    Monitor(const std::string& histogram_fname, bool update);
    /// Default destructor
    virtual ~Monitor();
    /// Monitor executor
    void monitor(Executor* exec, EventContext* context, const struct timeval& start, const struct timeval& stop);
    void add(const std::string& name, long id, long event, const struct timeval& start, const struct timeval& stop);
    void saveSummary(const std::string& name, size_t id, size_t parallel, size_t threads, size_t instances, const Timing& timing);
  };
}      /* End namespace      */
#endif /* FRAMEWORK_IMONITOR_H */
