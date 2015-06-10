#ifndef HLT2MONITORING_HISTOGRAM_H
#define HLT2MONITORING_HISTOGRAM_H 1

#include <algorithm>
#include <vector>
#include <ostream>

#include <boost/serialization/vector.hpp>

#include "Common.h"

namespace Monitoring {

/**
 */
struct Histogram {
  Histogram() = default;
  Histogram(RunNumber runNumber, TCK tck, HistId histId)
      : runNumber{runNumber}, tck{tck}, histId{histId}, data{} {}

  auto addChunk(const Chunk& c) noexcept -> void {
    assert(runNumber == c.runNumber && tck == c.tck && histId == c.histId);
    std::transform(std::begin(c.data), std::end(c.data),
                   std::begin(data) + c.start, std::begin(data) + c.start,
                   std::plus<BinContent>());
  }

  RunNumber runNumber;
  TCK tck;
  HistId histId;
  std::vector<BinContent> data;

};

}
#endif // HLT2MONITORING_HISTOGRAM_H
