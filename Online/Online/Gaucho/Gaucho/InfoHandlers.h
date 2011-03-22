/*
 * InfoHandlers.h
 *
 *  Created on: Mar 1, 2011
 *      Author: beat
 */

#ifndef INFOHANDLERS_H_
#define INFOHANDLERS_H_
#include <map>
#include <string>
//#include <pair>
#include "dic.hxx"
typedef std::map<std::string, DimInfo*> TaskMap;
typedef std::pair<std::string,DimInfo*> TaskPair;
typedef TaskMap::iterator TaskIter;

class HAdderTaskInfoHandler : public DimInfoHandler
{
public:
  TaskMap m_TaskMap;
  void infoHandler(void);
};
class HAdderServInfoHandler : public DimInfoHandler
{
public:
  void infoHandler(void);
};

#endif /* INFOHANDLERS_H_ */
