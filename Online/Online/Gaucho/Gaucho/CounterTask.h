/*
 * CounterTask.h
 *
 *  Created on: Jan 31, 2011
 *      Author: beat
 */

#ifndef COUNTERTASK_H_
#define COUNTERTASK_H_
#include <vector>
#include <string>
#include <boost/regex.hpp>
#include "dic.hxx"
#include "Gaucho/RPCRec.h"
class CntrDescr;
//#include "TObject.h"
class CounterTask
{
protected:
  std::string m_DNS;
  std::string m_task;
  std::string m_service;
  boost::regex m_taskexp;
  RPCRec *m_RPC;
public:
  CounterTask(const std::string &task,const std::string &dns="");
  virtual ~CounterTask(){};
  int Directory(std::vector<std::string> &Cntr);
  int Counters(std::vector<std::string> &Ctrs);
  int Counters(std::vector<std::string> &CounterNames,std::vector<CntrDescr*> &Cntrs);
  int Counters(std::vector<std::string> &CounterNames,std::map<std::string,CntrDescr*> &Cntrs);
  static int TaskList(std::string dns,std::vector<std::string> &);
};

#endif /* COUNTERTASK_H_ */
