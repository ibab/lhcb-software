/*
 * AdderSys.h
 *
 *  Created on: Feb 28, 2011
 *      Author: beat
 */
#include <vector>
#include <map>
#include <string>
#ifndef ADDERSYS_H_
#define ADDERSYS_H_
class MonAdder;
class HAdderTaskInfoHandler;
class HAdderServInfoHandler;
class DimInfo;
typedef std::vector<MonAdder*> Adderlist_t;
class AdderSys
{
public:
  DimInfo *gg_DNSInfo;
  Adderlist_t gg_AdderList;
  HAdderTaskInfoHandler *gg_TaskHandler;
  HAdderServInfoHandler *gg_ServHandler;
// AdderMap_t m_addermap;

  AdderSys();
  virtual ~AdderSys();
  void Add(MonAdder *);
  void Remove(MonAdder *);
  void start();
  void stop();
  static AdderSys &Instance();
};

#endif /* ADDERSYS_H_ */
