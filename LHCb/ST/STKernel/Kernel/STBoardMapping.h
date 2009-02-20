// $Id: STBoardMapping.h,v 1.5 2009-02-20 08:02:08 mneedham Exp $
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

#endif // STBOARDMAPPING_H
