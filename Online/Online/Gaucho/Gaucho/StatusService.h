/*
 * StatusService.h
 *
 *  Created on: Jul 4, 2012
 *      Author: Beat Jost
 */

#ifndef STATUSSERVICE_H_
#define STATUSSERVICE_H_

class MonitorSvc;
class MonSys;
class StatusService: public DimService
{
  public:
    MonSys *m_MonSys;
    MonitorSvc *m_monitorSvc;
    StatusService(MonitorSvc *msvc);
    virtual ~StatusService();
    virtual void serviceHandler();
};


#endif /* STATUSSERVICE_H_ */
