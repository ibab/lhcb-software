/*
 * HistTask.h
 *
 *  Created on: Jan 31, 2011
 *      Author: beat
 */

#ifndef HISTTASK_H_
#define HISTTASK_H_
#include <vector>
#include <string>
#include <boost/regex.hpp>
#include "dic.hxx"
#include "Gaucho/RPCRec.h"
//#include "TObject.h"

class TObject;
class HistTask
{
protected:
  std::string m_DNS;
  std::string m_task;
  std::string m_service;
  boost::regex m_taskexp;
  RPCRec *m_RPC;
  int Histos(std::vector<std::string> &hists);
public:
  HistTask(const std::string &task,const std::string &dns="",int tmo = 3);
  virtual ~HistTask();
  int Directory(std::vector<std::string> &hists);
  int Histos(std::vector<std::string> &hists,std::vector<TObject*> &histos);
  int Histos(std::vector<std::string> &hists,std::map<std::string,TObject*> &histos);
  static int TaskList(std::string dns,std::vector<std::string> &);
};

#endif /* HISTTASK_H_ */
