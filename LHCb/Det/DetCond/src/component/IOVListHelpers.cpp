#include "IOVListHelpers.h"

namespace IOVListHelpers {
  ICondDBReader::IOVList find_holes(const ICondDBReader::IOVList& data, const ICondDBReader::IOV& iov) {

    ICondDBReader::IOVList result;

    Gaudi::Time last = iov.since; // keep track of the end of coverage
    // loop over covering interval
    for (const auto& covered : data ) {
      if (covered.since > last) { // hole between the end of coverage and begin of next IOV
        result.emplace_back(last, covered.since);
      }
      last = covered.until; // prepare to look for the next hole
    }
    if (last < iov.until) {
      // we didn't get anything to cover until the end of the requested IOV
      result.emplace_back(last, iov.until);
    }

    return result;
  }
}
