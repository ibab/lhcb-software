#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utestTMVA
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <sstream>

#include "TMVAClassifierTool.h"

///-------- Basic Test: can we instantiate an object and call a method? ---
bool inittest()
{
  TMVATransform tmva;
  TMVATransform::optmap options;
  options["Name"]="test";
  options["XMLFile"]="../../options/TestPhi2KK.xml";
  return tmva.Init(options,std::cout);
}
 
/// le's use this function in a test case: 
BOOST_AUTO_TEST_CASE(initTest)
{
  BOOST_CHECK(inittest());
}
