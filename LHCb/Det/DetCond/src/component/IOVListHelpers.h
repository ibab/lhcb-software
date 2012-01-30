#ifndef IOVLISTHELPERS_H
#define IOVLISTHELPERS_H
#include "DetCond/ICondDBReader.h"

namespace IOVListHelpers {
  /// Find the intervals in passed interval that are not covered by the provided.
  ICondDBReader::IOVList find_holes(const ICondDBReader::IOVList& data, const ICondDBReader::IOV& iov);
}

#endif
