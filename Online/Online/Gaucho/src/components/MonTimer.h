#ifndef Gaucho_MonTimer_H
#define Gaucho_MonTimer_H 1

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Algorithm.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "MonitorSvc.h"
#include <cstring>
#include <string>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


class MonTimer{
public: 
   MonTimer(int refreshTime, ISvcLocator* svclocator, IGauchoMonitorSvc* owner);
   
   virtual ~MonTimer();
   void timerHandler();
  
private:
   
   int  m_refreshTime;
   ISvcLocator* m_svcLocator;
   IGauchoMonitorSvc* m_owner;
   
   IMessageSvc* m_msgSvc;
   
   boost::asio::strand* m_strand;
   boost::asio::io_service m_ioService;
   boost::asio::deadline_timer* m_asyncTimer;
   boost::thread* m_thread;
};   

#endif    // Gaucho_MonTimer_H
