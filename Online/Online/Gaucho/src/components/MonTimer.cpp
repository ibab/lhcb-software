#include "MonTimer.h"
IMonitorSvc* m_MonitorSvc;


MonTimer::MonTimer(int ttime, ISvcLocator* svclocator, IInterface* owner)
  : DimTimer(ttime)
 {      
      StatusCode sc;
      m_MonitorSvc = 0;
      // get pointer to ServiceLocator
      m_svcloc = svclocator;    
  
      // get msg logging
      sc = svclocator->service("MessageSvc", m_msgsvc);
      MsgStream log(m_msgsvc, "MonTimer");  
      if ( sc.isFailure() ) { 
        log << MSG::FATAL << "Error locating MessageSvc." << endreq;
      }
      else {
        log << MSG::DEBUG << "MessageSvc located." << endreq;
      }
      //get pointer to MonitorSvc 
      sc = svclocator->service("MonitorSvc", m_MonitorSvc, true );
      if( sc.isSuccess() )   {
        log << MSG::INFO << "Found the IMonitorSvc interface" << endreq; 	
      }
      else {    
        log << MSG::FATAL << "Unable to locate the IMonitorSvc interface." << endreq;
      }  
      m_owner=owner;   
      xtime=ttime;
      cout << "Montimer starting." << endl;
      start(xtime);
 };
 

void MonTimer::timerHandler()
   {       	 
      
      MsgStream log(m_msgsvc, "MonTimer");

      m_timerElapsed=m_MonitorSvc->getTimerElapsed();
      // if timerElapsed is true, we're probably waiting for triggers
      // update immediately
      if (m_timerElapsed) {
        if(m_MonitorSvc == m_owner) m_MonitorSvc->updateAll(true);     
        else m_MonitorSvc->updateAll( true, m_owner);
      }
      // if timerElapsed false, set it to true
      else m_MonitorSvc->setTimerElapsed(true);
      // update will occur start of next event 
       
      
      //check if time elapsed much bigger than time of last update
      //then we could be waiting for an event and should update
      
      //updateAll(xowner);
      start(xtime);
   };
  
