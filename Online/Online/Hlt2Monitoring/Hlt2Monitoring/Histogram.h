#ifndef HLT2MONITORING_HISTOGRAM_H
#define HLT2MONITORING_HISTOGRAM_H 1

#include <iostream>
#include <algorithm>
#include <vector>
#include <ostream>

#include <boost/serialization/vector.hpp>

#include "Common.h"

namespace Monitoring {

/**
 */
struct Histogram {
   Histogram() {};
   Histogram(RunNumber runNumber, TCK tck, HistId histId)
      : runNumber{runNumber}, tck{tck}, histId{histId}, data(200) {}

   Histogram(const Histogram& other)
      : runNumber{other.runNumber},
        tck{other.tck},
        histId{other.histId},
        data{other.data} { }

   Histogram(Histogram&& other)
   {
      std::swap(runNumber, other.runNumber);
      std::swap(tck, other.tck);
      std::swap(histId, other.histId);
      std::swap(data, other.data);
   }

   Histogram& operator=(const Histogram& other)
   {
      if (&other == this) {
         return *this;
      }
      runNumber = other.runNumber;
      tck = other.tck;
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
      std::swap(tck, other.tck);
      std::swap(histId, other.histId);
      std::swap(data, other.data);
      return *this;
   }

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
