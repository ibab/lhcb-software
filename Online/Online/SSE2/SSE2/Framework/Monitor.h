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
    Monitor(const std::string& histogram_fname);
    /// Default destructor
    virtual ~Monitor();
    /// Monitor executor
    void monitor(Executor* exec, EventContext* context, const struct timeval& start, const struct timeval& stop);
    /// Save data
    void save();
  };
}      /* End namespace      */
#endif /* FRAMEWORK_IMONITOR_H */
