#ifndef HLT2MONITORING_SERIALIZE_H
#define HLT2MONITORING_SERIALIZE_H 1

// Include files
#include "Chunk.h"
#include "Histogram.h"

/** Serialize a chunk of data using boost::serialization.
 */
namespace boost {
namespace serialization {

template <typename Archive>
auto serialize(Archive& archive, Monitoring::Chunk& chunk,
               const unsigned int) -> void {
   archive& chunk.runNumber;
   archive& chunk.histId;
   archive& chunk.tck;
   archive& chunk.start;
   archive& chunk.data;
}

}
}

#endif // HLT2MONITORING_SERIALIZE_H
