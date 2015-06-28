#ifndef KERNEL_RATECOUNTER_H
#define KERNEL_RATECOUNTER_H 1

// Include files
#include <string>

// local
#include <Kernel/IHltMonitorSvc.h>

/** @class RateCounter RateCounter.h
 *
 *
 *  @author Roel Aaij
 *  @date   2015-06-01
 */
class RateCounter {
public:
   /// Standard constructor
   RateCounter(IHltMonitorSvc* const monSvc, Gaudi::StringKey id)
      : m_id{id},
        m_monSvc{monSvc}
   {

   }

   virtual ~RateCounter( ) { }; ///< Destructor

   // Count at time t
   void count(double t) {
      m_monSvc->count(m_id, t);
   }

   Gaudi::StringKey id() const
   {
      return m_id;
   }

private:

   const Gaudi::StringKey m_id;
   IHltMonitorSvc* const m_monSvc;

};
#endif // KERNEL_RATECOUNTER_H
