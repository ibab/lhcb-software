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
    // Skip empty chunks
    if (c.data.empty()) return;

    // Find the highest bin in this chunk.
    auto high = c.highestBin();

    // Resize if needed.
    if (high > data.size()) {
       data.resize(high);
    }

    for (const auto& entry : c.data) {
       data[entry.first] += entry.second;
    }
  }

  RunNumber runNumber;
  TCK tck;
  HistId histId;
  std::vector<BinContent> data;

};

}
#endif // HLT2MONITORING_HISTOGRAM_H
