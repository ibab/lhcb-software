// $Id: $
#ifndef SWIMMING_FUNCTIONS_H 
#define SWIMMING_FUNCTIONS_H 1

#include <string>

#include <boost/integer_traits.hpp>
#include <boost/integer/integer_mask.hpp>

namespace Random {
   boost::uint32_t mix(boost::uint32_t state);
   boost::uint32_t mix32(boost::uint32_t state, boost::uint32_t extra);
   boost::uint32_t mix64(boost::uint32_t state, boost::uint64_t extra);
   boost::uint32_t mixString(boost::uint32_t state, const std::string& extra);
}

#endif // SWIMMING_FUNCTIONS_H
