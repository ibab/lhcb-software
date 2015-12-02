#ifndef KERNEL_IHLTMONITORSVC_H
#define KERNEL_IHLTMONITORSVC_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/StringKey.h"

// Local
class RateCounter;
class HltHistogram;

/** @class IHltMonitorSvc IHltMonitorSvc.h Kernel/IHltMonitorSvc.h
 *
 *
 *  @author Roel Aaij
 *  @date   2015-06-01
 */
class GAUDI_API IHltMonitorSvc : virtual public extend_interfaces<INamedInterface> {
public:

   // Return the interface ID
   DeclareInterfaceID(IHltMonitorSvc, 2, 0);

   virtual ~IHltMonitorSvc() = default;

   /// List of all counters
   virtual std::vector<std::string> counters() const = 0;

   /// Simple wrapper to facilitate counting
   virtual RateCounter& rateCounter(const std::string& identifier) const = 0;

   /// Count counter at time t
   virtual void count(const Gaudi::StringKey& id, double t) = 0;

   /// List of all histograms
   virtual std::vector<std::string> histograms() const = 0;

   /// Simple wrapper to facilitate filling of histograms
   virtual HltHistogram& histogram(const std::string& identifier,
                                   double left, double right,
                                   size_t bins) const = 0;

   /// Fill histogram with value
   virtual void fill(const Gaudi::StringKey& key, size_t bin) = 0;


};
#endif // KERNEL_IHLTMONITORSVC_H
