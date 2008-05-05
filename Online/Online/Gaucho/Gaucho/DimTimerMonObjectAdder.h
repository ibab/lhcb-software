#ifndef DIMTIMERMONOBJECTADDER_H
#define DIMTIMERMONOBJECTADDER_H 1

#include <dis.hxx>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"


class DimInfoMonObject;
class DimServiceMonObject;
class MonObject;

class DimTimerMonObjectAdder: public DimTimer {
private: 
  int m_time;
  std::string  m_name;
  IMessageSvc* m_msgSvc;

  std::vector<std::vector<DimInfoMonObject*> >	m_dimInfoMonObjectVect2D;
  std::vector<DimServiceMonObject*>		m_dimServiceMonObjectVect1D;
public: 
  DimTimerMonObjectAdder (int ttime, std::vector<std::vector<DimInfoMonObject*> > dimInfo2D, std::vector<DimServiceMonObject*> dimService1D);
  void setMsgSvc(IMessageSvc* msgSvc){m_msgSvc = msgSvc;}
  void timerHandler();
  
};   


#endif    // DIMTIMERMONOBJECTADDER_H
