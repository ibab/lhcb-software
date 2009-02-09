// $Id: STBoardMapping.h,v 1.4 2009-02-09 10:38:23 mneedham Exp $
#ifndef STBOARDMAPPING_H 
#define STBOARDMAPPING_H 1

/** @namespace STBoardMapping STBoardMapping.h Kernel/STBoardMapping.h
 *
 *  Namespace for source ID to board number (and vice-versa) for IT and TT
 *
 *  @author Johan Luisier
 * @author M Needham
 *  @date   2008-07-14
 */

#include <sstream>
#include <map>
#include <string>

namespace STBoardMapping{

  typedef std::map<unsigned int, unsigned int> Map;

  const Map& TTSourceIDToNumberMap();

  const Map& TTNumberToSourceIDMap();

  const Map& ITSourceIDToNumberMap();

  const Map& ITNumberToSourceIDMap();

  enum endValue {notFound = 999};

  /** trivial helper to find entries in the map safely */ 
  unsigned int find(const unsigned int key, const Map& testMap); 

  /** printout the TT mapping */
  std::string printTTMap();
 
  /** printout the IT mapping */
  std::string printITMap();

}

inline unsigned int STBoardMapping::find(const unsigned int key, const STBoardMapping::Map& testMap){
  STBoardMapping::Map::const_iterator iter = testMap.find(key);
  return (iter == testMap.end() ? (unsigned int)STBoardMapping::notFound : iter->second);
} 


#include <boost/foreach.hpp>


inline std::string STBoardMapping::printTTMap(){

  std::ostringstream o;
  o << " *** TT board to sourceID Map ***" << std::endl;
  const Map& theMap = TTSourceIDToNumberMap();
  BOOST_FOREACH(Map::value_type board , theMap) {
    o << "SourceID: " << board.first << " Flat number: " << board.second << std::endl; 
  } // for each
  o << "**************" << std::endl;
  return o.str();
}

inline std::string STBoardMapping::printITMap() {

  std::ostringstream o;
  o << " *** IT board to sourceID Map ***" << std::endl;
  const Map& theMap = TTSourceIDToNumberMap();
  BOOST_FOREACH(Map::value_type board , theMap) {
    o << "SourceID: " << board.first << " Flat number: " << board.second << std::endl; 
  } // for each
  o << "**************" << std::endl;  
  return o.str();
}

#endif // STBOARDMAPPING_H
