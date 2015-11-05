#ifndef HLT2MONITORING_HISTOGRAM_H
#define HLT2MONITORING_HISTOGRAM_H 1

#include <iostream>
#include <algorithm>
#include <vector>
#include <ostream>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include "Common.h"

namespace Monitoring {

/**
 */
struct Histogram {
   Histogram() {};
   Histogram(RunNumber runNumber, HistId histId)
      : runNumber{runNumber}, histId{histId}, data(4000, 0) {}

   Histogram(const Histogram& other)
      : runNumber{other.runNumber},
        histId{other.histId},
        data(other.data) { }

   Histogram(Histogram&& other)
   {
      std::swap(runNumber, other.runNumber);
      std::swap(histId, other.histId);
      std::swap(data, other.data);
   }

   Histogram& operator=(const Histogram& other)
   {
      if (&other == this) {
         return *this;
      }
      runNumber = other.runNumber;
      histId = other.histId;
      data = other.data;
      return *this;
   }

   Histogram& operator=(Histogram&& other)
   {
      if (&other == this) {
         return *this;
      }
      std::swap(runNumber, other.runNumber);
      std::swap(histId, other.histId);
      std::swap(data, other.data);
      return *this;
   }

   auto addChunk(const Chunk& c) noexcept -> void {
      assert(runNumber == c.runNumber && histId == c.histId);
      // Skip empty chunks
      if (c.data.empty()) return;

      // Find the highest bin in this chunk.
      auto high = c.highestBin();

      // Resize if needed.
      if (high > data.size()) {
         data.resize(high, 0);
      }

      for (const auto& entry : c.data) {
         auto bin = entry.first;
         if (bin > data.size()) {
            continue;
         }
         data[bin] += entry.second;
      }
   }

   RunNumber runNumber;
   TCK tck = 0; // for backwards compatibility
   HistId histId;
   std::vector<BinContent> data;

};
}

BOOST_CLASS_VERSION(Monitoring::Histogram, 1)

#endif // HLT2MONITORING_HISTOGRAM_H
