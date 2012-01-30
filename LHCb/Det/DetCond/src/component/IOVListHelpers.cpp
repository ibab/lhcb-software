#include "IOVListHelpers.h"

namespace IOVListHelpers {
  ICondDBReader::IOVList find_holes(const ICondDBReader::IOVList& data, const ICondDBReader::IOV& iov) {
    typedef ICondDBReader::IOVList IOVList;
    typedef ICondDBReader::IOV IOV;

    IOVList result;

    Gaudi::Time last = iov.since; // keep track of the end of coverage
    // loop over covering interval
    for (IOVList::const_iterator covered = data.begin(); covered != data.end(); ++covered) {
      if (covered->since > last) { // hole between the end of coverage and begin of next IOV
        result.push_back(IOV(last, covered->since));
      }
      last = covered->until; // prepare to look for the next hole
    }
    if (last < iov.until) {
      // we didn't get anything to cover until the end of the requested IOV
      result.push_back(IOV(last, iov.until));
    }

    return result;
  }
}
