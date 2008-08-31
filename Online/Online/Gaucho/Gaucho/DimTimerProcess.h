#ifndef DIMTIMERPROCESS_H
#define DIMTIMERPROCESS_H 1

#include <dis.hxx>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

//  Author: jotalo, 29/06/2008


class ProcessMgr;
class DimTimerProcess: public DimTimer {
private: 
  std::string  m_name;
  ProcessMgr*  m_processMgr;
  const int    m_refreshTime;
  IMessageSvc* m_msgSvc;

  IMessageSvc* msgSvc() {return m_msgSvc;};
  std::string name() {return m_name;};
public: 
  DimTimerProcess (ProcessMgr *processMgr, const int &refreshTime, IMessageSvc* msgSvc);
  ~DimTimerProcess();
  
  void timerHandler();
  bool statusDiscrepancy();
};   

#endif    // DIMTIMERPROCESS_H

