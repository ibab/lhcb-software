// $Id: STBoardMapping.h,v 1.1 2008-07-15 07:37:35 jluisier Exp $
#ifndef STBOARDMAPPING_H 
#define STBOARDMAPPING_H 1

// Include files
#include <map>
#include "boost/assign/list_of.hpp"

/** @namespace STBoardMapping STBoardMapping.h Kernel/STBoardMapping.h
 *
 *  Namespace for source ID to board number (and vice-versa) for IT and TT
 *
 *  @author Johan Luisier
 *  @date   2008-07-14
 */

namespace STBoardMapping{

  const std::map<unsigned int, unsigned int> TTSourceIDToNumberMap
  = boost::assign::map_list_of(0, 17)(1, 18)(2, 19)(3, 20)(4, 1)(5, 2)(6, 3)
    (7, 33)(8, 34)(9, 35)(10, 36)(32, 21)(33, 22)(34, 23)(35, 24)(36, 4)
    (37, 5)(38, 6)(39, 37)(40, 38)(41, 39)(42, 40)(64, 25)(65, 26)(66, 27)
    (67, 28)(68, 7)(69, 8)(70, 9)(71, 10)(72, 11)(73, 41)(74, 42)(75, 43)
    (76, 44)(96, 29)(97, 30)(98, 31)(99, 32)(100, 12)(101, 13)(102, 14)
    (103, 15)(104, 16)(105, 45)(106, 46)(107, 47)(108, 48);
  
  const std::map<unsigned int, unsigned int> TTNumberToSourceIDMap
  = boost::assign::map_list_of(1, 4)(2, 5)(3, 6)(4, 36)(5, 37)(6, 38)(7, 68)
    (8, 69)(9, 70)(10, 71)(11, 72)(12, 100)(13, 101)(14, 102)(15, 103)
    (16, 104)(17, 0)(18, 1)(19, 2)(20, 3)(21, 32)(22, 33)(23, 34)(24, 35)
    (25, 64)(26, 65)(27, 66)(28, 67)(29, 96)(30, 97)(31, 98)(32, 99)(34, 8)
    (35, 9)(36, 10)(37, 39)(38, 40)(39, 41)(40, 42)(41, 73)(42, 74)(43, 75)
    (44, 76)(45, 105)(46, 106)(47, 107)(48, 108);

  const std::map<unsigned int, unsigned int> ITSourceIDToNumberMap
  = boost::assign::map_list_of(0, 0)(1, 1)(2, 2)(3, 3)(4, 4)(5, 5)(6, 6)
    (7, 7)(8, 8)(9, 9)(10, 10)(11, 11)(12, 12)(13, 13)(32, 14)(33, 15)
    (34, 16)(35, 17)(36, 18)(37, 19)(38, 20)(39, 21)(40, 22)(41, 23)(42, 24)
    (43, 25)(44, 26)(45, 27)(64, 28)(65, 29)(66, 30)(67, 31)(68,32)(69, 33)
    (70, 34)(71, 35)(72, 36)(73, 37)(74, 38)(75, 39)(76, 40)(77, 41);
  
  const std::map<unsigned int, unsigned int> ITNumberToSourceIDMap
  = boost::assign::map_list_of(0, 0)(1, 1)(2, 2)(3, 3)(4, 4)(5, 5)(6, 6)
    (7, 7)(8, 8)(9, 9)(10, 10)(11, 11)(12, 12)(13, 13)(14, 32)(15, 33)
    (16, 34)(17, 35)(18, 36)(19, 37)(20, 38)(21, 39)(22, 40)(23, 41)(24, 42)
    (25, 43)(26, 44)(27, 45)(28, 64)(29, 65)(30, 66)(31, 67)(32, 68)(33, 69)
    (34, 70)(35, 71)(36, 72)(37, 73)(38, 74)(39, 75)(40, 76)(41, 77);
  
};

#endif // STBOARDMAPPING_H
