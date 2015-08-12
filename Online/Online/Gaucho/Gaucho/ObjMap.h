/*
 * ObjMap.h
 *
 *  Created on: Aug 7, 2015
 *      Author: beat
 */

#ifndef SOURCE_DIRECTORY__ONLINE_GAUCHO_GAUCHO_OBJMAP_H_
#define SOURCE_DIRECTORY__ONLINE_GAUCHO_GAUCHO_OBJMAP_H_
#include <string>
#include <map>
class MonObj;
class ObjMap : public std::map<std::string,MonObj*>
{
  public:
    void dump()
    {
      for (auto i=begin();i!=end();i++)
      {
        printf("[WARNING]: %s\n",(*i).first.c_str());
      }
    }
};
typedef ObjMap::iterator SysIter;
#endif /* SOURCE_DIRECTORY__ONLINE_GAUCHO_GAUCHO_OBJMAP_H_ */
