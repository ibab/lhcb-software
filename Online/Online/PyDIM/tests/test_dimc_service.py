"""
 @file test_dimc_service.py
 @brief An example of using a DIM service.
 This file contains an example for using a DIM service from python. A DimTask 
 object is at the same time the DIM service and client.
 Like for the DIM functions there are several steps needed to be taken:
   -# server creates a service
   -# server start 'serving' (dis_start_serving() )
   -# server updates the service
   -# client registers to a service
   -# client wait to be updated

@author Radu Stoica (Radu.Stoica@cern.ch)
\addtogroup examples
@{
"""

import sys
from debug import SAY, ERROR, DEBUG
import debug
debug.DEBUG = 1
from pydim import dis_update_service, dis_add_service, dic_info_service, dis_start_serving, dis_stop_serving
from time import sleep
from threading import Thread, Event

class DimTask:
    """ Creates 3 services and registeres to them. Can be run in multiple instances.
    The class will update "UPDATES" times the services.
    """
    SRV1NAME='Test_Serv1_'
    SRV2NAME='Test_Serv2_'
    SRV3NAME='Test_Serv3_'
    SRV1FORMAT='F:1;I:1;D:2;C:10;C' # buffer size is 35 on 32 bits 
    SRV2FORMAT='D' 
    SRV3FORMAT='C'
    DELAY=3
    
    def __init__(self, name, clients, updates=10):
        self.name = name
        self.updates = updates
        self.clients = clients
        self.svc1 = dis_add_service(DimTask.SRV1NAME+self.name, 
                                    DimTask.SRV1FORMAT, 
                                    self.service1, 1)
        self.svc2 = dis_add_service(DimTask.SRV2NAME+self.name, 
                                    DimTask.SRV2FORMAT, 
                                    self.service2, 2)
        self.svc3 = dis_add_service(DimTask.SRV3NAME+self.name, 
                                    DimTask.SRV3FORMAT,
                                    self.service3, 3)
        DEBUG("Service 3 created with ID ", self.svc3)
        DEBUG("DIMTASK %s initialized" %self.name)
        sleep(1)  

    def run(self):
        dis_start_serving(self.name)
        print self.clients
        for x in self.clients: 
            DEBUG("DIMTASK %s : Registering to service %s" 
                  %(self.name, DimTask.SRV1NAME+x)) 
            dic_info_service(DimTask.SRV1NAME+x, DimTask.SRV1FORMAT, 
                                   self.client_callback1)
            DEBUG("DIMTASK %s : Registering to service %s" 
                  %(self.name, DimTask.SRV2NAME+x)) 
            dic_info_service(DimTask.SRV2NAME+x, DimTask.SRV2FORMAT, 
                                   self.client_callback2)
            DEBUG("DIMTASK %s : Registering to service %s" 
                  %(self.name, DimTask.SRV3NAME+x)) 
            dic_info_service(DimTask.SRV3NAME+x, DimTask.SRV3FORMAT,
                                   self.client_callback3)
        counter = 0
        DEBUG("DIMTASK %s : Starting service update " %self.name) 
        while counter < self.updates:
            counter += 1

            SAY("DIMTASK %s : Updating service nr. 1" %self.name)
            values = (counter, counter+1, 999.0, 999.0 , 'BAU', 'B')
            SAY('DIMTASK %s : Updated %d clients'  
                %(self.name, dis_update_service(self.svc1, values))) 

            SAY("DIMTASK %s : Updating service nr. 2" %self.name) 
            values = (counter, )
            SAY('DIMTASK %s : Updated %d clients' \
                %(self.name, dis_update_service(self.svc2, values)))

            SAY("DIMTASK %s : Updating service nr. 3" %self.name)
            values = ('ALABALAPORTOCALA', )
            SAY('DIMTASK %s : Updated %d clients' \
                %(self.name, dis_update_service(self.svc3, values)))
            sleep(DimTask.DELAY)
        dis_stop_serving()

    def service1(self):
        return (1, 2, 3.3, 4.4, "A", "B")

    def service2(self):
        return (666.666)
    
    def service3(self):
        return ("ALABALAPORTOCALA")
    
    def client_callback1(self, *args):
        SAY('DIMTASK %s : client callback called for service1. Args are %s' 
               %(self.name, args))
    
    def client_callback2(self, *args):
        SAY('DIMTASK %s : client callback called for service2. Args are %s' 
               %(self.name, args))
    
    def client_callback3(self, *args):
        SAY('DIMTASK %s : client callback called for service3. Args are %s' 
               %(self.name, args))


def usage():
    print """Usage: python <script name> <DIM Task name> <Other DIM Clients> (comma separated)""" 
               
      
if __name__=='__main__':
    import sys
    print sys.argv
    try:
        task_name = sys.argv[1]
    except:
        task_name = "task1"
    client_names = sys.argv[2:]
    if not client_names:
        client_names = ("task1",)
    task = DimTask(task_name, client_names)
    task.run()
