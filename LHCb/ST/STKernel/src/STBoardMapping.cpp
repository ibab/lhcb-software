
// Include files
#include <map>
#include "boost/assign/list_of.hpp"


// Kernel
#include "Kernel/STBoardMapping.h"

const STBoardMapping::Map& STBoardMapping::TTSourceIDToNumberMap() {
  static Map s_map;
  if (s_map.empty()){
    s_map
     = boost::assign::map_list_of(0, 17)(1, 18)(2, 19)(3, 20)(4, 1)(5, 2)(6, 3)
       (7, 33)(8, 34)(9, 35)(10, 36)(32, 21)(33, 22)(34, 23)(35, 24)(36, 4)
      (37, 5)(38, 6)(39, 37)(40, 38)(41, 39)(42, 40)(64, 25)(65, 26)(66, 27)
      (67, 28)(68, 7)(69, 8)(70, 9)(71, 10)(72, 11)(73, 41)(74, 42)(75, 43)
      (76, 44)(96, 29)(97, 30)(98, 31)(99, 32)(100, 12)(101, 13)(102, 14)
      (103, 15)(104, 16)(105, 45)(106, 46)(107, 47)(108, 48);
  }
  return s_map;
}

const STBoardMapping::Map& STBoardMapping::TTNumberToSourceIDMap() {
  static Map s_map;
  if (s_map.empty()){
    s_map
      = boost::assign::map_list_of(17, 0)(18, 1)(19, 2)(20, 3)(1, 4)(2, 5)
      (3, 6)(33, 7)(34, 8)(35, 9)(36, 10)(21, 32)(22, 33)(23, 34)(24, 35)
      (4, 36)(5, 37)(6, 38)(37, 39)(38, 40)(39, 41)(40, 42)(25, 64)(26, 65)
      (27, 66)(28, 67)(7, 68)(8, 69)(9, 70)(10, 71)(11, 72)(41, 73)(42, 74)
      (43, 75)(44, 76)(29, 96)(30, 97)(31, 98)(32, 99)(12, 100)(13, 101)
      (14, 102)(15, 103)(16, 104)(45, 105)(46, 106)(47, 107)(48, 108);
  }
  return s_map;
}

const STBoardMapping::Map& STBoardMapping::ITSourceIDToNumberMap(){
  static Map s_map;
  if (s_map.empty()){
    s_map
       = boost::assign::map_list_of(0, 1)(1, 2)(2, 3)(3, 4)(4, 5)(5, 6)(6, 7)
    (7, 8)(8, 9)(9, 10)(10, 11)(11, 12)(12, 13)(13, 14)(32, 15)(33, 16)
    (34, 17)(35, 18)(36, 19)(37, 20)(38, 21)(39, 22)(40, 23)(41, 24)(42, 25)
    (43, 26)(44, 27)(45, 28)(64, 29)(65, 30)(66, 31)(67, 32)(68,33)(69, 34)
    (70, 35)(71, 36)(72, 37)(73, 38)(74, 39)(75, 40)(76, 41)(77, 42);
  }
  return s_map;
}


const STBoardMapping::Map& STBoardMapping::ITNumberToSourceIDMap() {
 static Map s_map;
  if (s_map.empty()){
    s_map
      = boost::assign::map_list_of(1, 0)(2, 1)(3, 2)(4, 3)(5, 4)(6, 5)(7, 6)
    (8, 7)(9, 8)(10, 9)(11, 10)(12, 11)(13, 12)(14, 13)(15, 32)(16, 33)
    (17, 34)(18, 35)(19, 36)(20, 37)(21, 38)(22, 39)(23, 40)(24, 41)(25, 42)
    (26, 43)(27, 44)(28, 45)(29, 64)(30, 65)(31, 66)(32, 67)(33, 68)(34, 69)
    (35, 70)(36, 71)(37, 72)(38, 73)(39, 74)(40, 75)(41, 76)(42, 77);
  }
  return s_map;
}


#include <boost/foreach.hpp>


std::string STBoardMapping::printTTMap(){

  std::ostringstream o;
  o << " *** TT board to sourceID Map ***" << std::endl;
  const Map& theMap = TTSourceIDToNumberMap();
  BOOST_FOREACH(Map::value_type board , theMap) {
    o << "SourceID: " << board.first << " Flat number: " << board.second << std::endl; 
  } // for each
  o << "**************" << std::endl;
  return o.str();
}

std::string STBoardMapping::printITMap() {

  std::ostringstream o;
  o << " *** IT board to sourceID Map ***" << std::endl;
  const Map& theMap = TTSourceIDToNumberMap();
  BOOST_FOREACH(Map::value_type board , theMap) {
    o << "SourceID: " << board.first << " Flat number: " << board.second << std::endl; 
  } // for each
  o << "**************" << std::endl;  
  return o.str();
}
