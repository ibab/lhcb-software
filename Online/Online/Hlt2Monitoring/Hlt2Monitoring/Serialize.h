#ifndef HLT2MONITORING_SERIALIZE_H
#define HLT2MONITORING_SERIALIZE_H 1

// Include files
#include "Chunk.h"
#include "Histogram.h"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

namespace boost {
namespace serialization {

// unordered_map
template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void save(
    Archive & ar,
    const std::unordered_map<Key, Type, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    boost::serialization::stl::save_collection<
        Archive,
        std::unordered_map<Key, Type, Compare, Allocator>
    >(ar, t);
}

template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void load(
    Archive & ar,
    std::unordered_map<Key, Type, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    boost::serialization::stl::load_collection<
        Archive,
        std::unordered_map<Key, Type, Compare, Allocator>,
        boost::serialization::stl::archive_input_map<
            Archive, std::unordered_map<Key, Type, Compare, Allocator>
        >,
        boost::serialization::stl::no_reserve_imp<
            std::unordered_map<Key, Type, Compare, Allocator>
        >
    >(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void serialize(
    Archive & ar,
    std::unordered_map<Key, Type, Compare, Allocator> &t,
    const unsigned int file_version
){
    boost::serialization::split_free(ar, t, file_version);
}

// Serialize Chunk
template <typename Archive>
auto serialize(Archive& archive, Monitoring::Chunk& chunk,
               const unsigned int) -> void {
   archive& chunk.runNumber;
   archive& chunk.histId;
   archive& chunk.tck;
   archive& chunk.data;
}

}
}

#endif // HLT2MONITORING_SERIALIZE_H
