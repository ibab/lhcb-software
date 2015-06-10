#ifndef HLT2MONITORING_CHUNK_H
#define HLT2MONITORING_CHUNK_H 1

#include <algorithm>
#include <vector>
#include <ostream>

#include <boost/serialization/vector.hpp>

#include "Common.h"

namespace Monitoring {

/** A serialize-able chunk of data that spans DELTA_T seconds.
 */
struct Chunk {
   Chunk() = default;

   Chunk(RunNumber rn, TCK t, HistId id, Seconds s, size_t n = 10)
      : runNumber{rn}, tck{t}, histId{id}, start{s}, data(n) {}

   // Copy constructor
   Chunk(const Chunk& other)
      : runNumber{other.runNumber},
        tck{other.tck},
        histId{other.histId},
        start{other.start}
   {
      data.reserve(other.data.size());
      std::copy(begin(other.data), end(other.data), std::back_inserter(data));
   }

   // Move constructor
   Chunk(Chunk&& other)
      : runNumber{0},
        tck{0},
        histId{0},
        start{0}
   {
      std::swap(runNumber, other.runNumber);
      std::swap(tck, other.tck);
      std::swap(histId, other.histId);
      std::swap(start, other.start);
      std::swap(data, other.data);
   }

   // Assignment operator
   Chunk& operator=(const Chunk& other) {
      if (&other == this) {
         return *this;
      }
      runNumber = other.runNumber;
      tck = other.tck;
      histId = other.histId;
      start = other.start;
      data = other.data;
      return *this;
   }

   // Move assignment operator
   Chunk& operator=(Chunk&& other) {
      if (&other == this) {
         return *this;
      }
      std::swap(runNumber, other.runNumber);
      std::swap(tck, other.tck);
      std::swap(histId, other.histId);
      std::swap(start, other.start);
      std::swap(data, other.data);
      return *this;
   }

   // operator+=
   Chunk& operator+=(const Chunk& c) {
      auto size = data.size();
      auto new_size = c.start + c.data.size();
      if (new_size > data.size()) {
         data.resize(new_size);
         for (unsigned int i = size; i < new_size; ++i) data[i] = 0;
      }
      for (unsigned int i = 0; i < c.data.size(); ++i)
         data[c.start + i] += c.data[i];
      return *this;
   }

   RunNumber runNumber;
   TCK tck;
   HistId histId;
   Seconds start;
   std::vector<BinContent> data;
};
}

#endif // HLT2MONITORING_CHUNK_H
