#ifndef Gaucho_MonTimer_H
#define Gaucho_MonTimer_H 1

#include <dis.hxx>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "MonitorSvc.h"
#include <time.h>
#include <cstring>
#include <string>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/ObjectContainerBase.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/KeyedContainer.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/SmartDataPtr.h"



using namespace std;
class MonitorSvc;

class MonTimer: public DimTimer {
public: 
   //constructor
   MonTimer(int xtime, ISvcLocator* svclocator,IInterface* owner);
   //IInterface pure virtual member functions
   void timerHandler();
   int xtime;
   
private:
   bool m_timerElapsed;   
   ISvcLocator* m_svcloc;
   IMessageSvc* m_msgsvc;
   IInterface* m_owner;
   
};   


#endif    // Gaucho_MonTimer_H
