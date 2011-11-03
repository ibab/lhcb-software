#ifndef FRAMEWORK_TIMING_H
#define FRAMEWORK_TIMING_H

// C++ include files
#include <sys/time.h>
#include <string>

/*
*    Framework namespace declaration
*/
namespace Framework {

  /**@class Timing Timing.h Framework/Timing.h
  *
  *  @author  M.Frank
  *  @version 1.0
  *  @date    10/06/2011 
  *
  */
  class Timing   {
    /// Statistics: Number of calls
    long             m_calls;
    /// Statistics: Processing time in micro seconds
    double           m_time;
    /// Statistics: Processing time squared in micro seconds    
    double           m_time2;

  public:
    /// Initializing constructor
    Timing(long calls, double tim, double tim2);
    /// Default constructor
    Timing();
    /// Default destructor
    ~Timing() {}
    /// Add Timing structures
    Timing& operator+=(const Timing& t);
    /// Subtract Timing structures
    Timing& operator-=(const Timing& t);
    /// Re-initialize
    void initialize();
    /// Make standardized printout
    void print(const char* name)  const;
    /// Get timing information in string form
    std::string toString()  const;

    /// Statistics: collect processing time and number of calls
    void collect(const timeval& start, const timeval& stop);
    /// Get the current time stamp
    static struct timeval now();
    /// Access the number of calls
    long   numCalls() const                  { return m_calls;                    }
    /// Set number of calls
    void setCalls(long num)                  { m_calls = num; m_time2 = 0e0;      }
    /// Access the summed processing time
    double totalTime()  const                { return m_time;                     }
    /// Access the summed processing time squared
    double totalTimeSqr()  const             { return m_time2;                    }
    /// Access mean time spent in one call
    double mean()  const;
    /// Access variance of mean time spent in one call
    double sigma()  const;
  };
}      /* End namespace      */
#endif /* FRAMEWORK_TIMING_H */
