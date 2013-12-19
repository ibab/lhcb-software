/** @file module.cpp
 *
 *  Small Python module to use the CondDB (de)compression.
 *
 */

#include "../component/CondDBCompression.h"

#include <boost/python.hpp>

namespace {
  inline std::string defaultCompress(const std::string& s) {
    return CondDBCompression::compress(s);
  }
}

BOOST_PYTHON_MODULE(CondDBCompression)
{
    using namespace boost::python;
    def("compress", CondDBCompression::compress);
    def("compress", defaultCompress);
    def("decompress", CondDBCompression::decompress);
}
