#ifndef HLT2MONITORING_SERIALIZE_H
#define HLT2MONITORING_SERIALIZE_H 1

// STD & STL
#include <unordered_map>

// Gaudi
#include <GaudiKernel/HistoDef.h>

// Local include files
#include "Chunk.h"
#include "RunInfo.h"
#include "Histogram.h"

// Boost
#include "boost/version.hpp"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>

#if (BOOST_VERSION >= 105800)
#include <boost/serialization/unordered_map.hpp>
#endif

namespace boost {
namespace serialization {

// Boost < 1.58 needs some special stuff for unordered_map.
#if (BOOST_VERSION < 105800)

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
// non intrusive save/load member functions for std::unordered_map<Key, Type, Compare, Allocator>
template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void serialize(
    Archive & ar,
    std::unordered_map<Key, Type, Compare, Allocator> &t,
    const unsigned int file_version
){
    boost::serialization::split_free(ar, t, file_version);
}
#endif

// Gaudi::Histo1DDef
template<class Archive>
inline void save(
    Archive& archive,
    const Gaudi::Histo1DDef &def,
    const unsigned int /* file_version */
){
   std::string title = def.title();
   int bins = def.bins();
   double low = def.lowEdge();
   double high = def.highEdge();

   archive& title;
   archive& bins;
   archive& low;
   archive& high;
}

template<class Archive>
inline void load(
    Archive & archive,
    Gaudi::Histo1DDef &def,
    const unsigned int /* file_version */
){
   std::string title;
   int bins;
   double low, high;

   archive& title;
   archive& bins;
   archive& low;
   archive& high;

   def.setTitle(title);
   def.setLowEdge(low);
   def.setHighEdge(high);
   def.setBins(bins);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions for Histo1DDef
template<class Archive>
auto serialize(
    Archive& archive,
    Gaudi::Histo1DDef &def,
    const unsigned int file_version) -> void {
    boost::serialization::split_free(archive, def, file_version);
}

// Serialize Chunk
template <typename Archive>
auto serialize(Archive& archive, Monitoring::Chunk& chunk,
               const unsigned int v) -> void {
   archive& chunk.runNumber;
   if (v == 0)
      archive& chunk.tck;
   archive& chunk.histId;
   archive& chunk.data;
}

// Serialize Histogram
template <typename Archive>
auto serialize(Archive& archive, Monitoring::Histogram& histo,
               const unsigned int v) -> void {
   archive& histo.runNumber;
   if (v == 0)
      archive& histo.tck;
   archive& histo.histId;
   archive& histo.data;
}

// Serialize RunInfo
template <typename Archive>
auto serialize(Archive& archive, Monitoring::RunInfo& info,
               const unsigned int v) -> void {
   archive& info.lumiPars;
   archive& info.run;
   archive& info.start;
   if (v > 0) {
      archive& info.tck;
      archive& info.application;
   }
}

}
}

#endif // HLT2MONITORING_SERIALIZE_H
