#ifndef KERNEL_IDQSCANNER_H
#define KERNEL_IDQSCANNER_H 1
// Include files
// from STL
#include <string>

// from LHCb
#include "Kernel/IDQFilter.h"

// Forward declaration
namespace Gaudi {
  class Time;
}

/** Interface for a tool extracting a summary of the Data Quality flags from the
 * conditions database in a given period of time.
 *
 * @author Marco Clemencic
 * @date 04/11/2011
 */
class IDQScanner: virtual public IAlgTool {
public:
  /// InterfaceID
  DeclareInterfaceID(IDQScanner, 1, 0);
  virtual ~IDQScanner() {}

  /// Scan all the Data Quality flags in the give time range.
  /// @return merged list of DQ flags
  virtual IDQFilter::FlagsType scan(const Gaudi::Time& since, const Gaudi::Time& until) const = 0;

protected:
private:
};

#endif // KERNEL_IDQSCANNER_H
