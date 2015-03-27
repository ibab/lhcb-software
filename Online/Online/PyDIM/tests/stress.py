#!/usr/bin/python 
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
from pydim import DEBUG, dic_cmnd_service, dis_update_service, dis_add_service, dic_info_service, dis_start_serving, dis_stop_serving, dic_release_service
from time import sleep

i = 0
DEBUG=6    
def stress_callback( *args):
    global i
    if i % 10000 == 0:        
       print "stress_callback: %d" % i
    i += 1

def service_callback(tag):
    global i
    if i % 10000 == 0:
        print "stress service updated: %d" % i
    return (i,)    
      
if __name__=='__main__':
    import sys
    n = 500000
    if len(sys.argv) > 1: n = int(sys.argv[1])
    si = dic_info_service("STRESS_SVC", "I", stress_callback)
    s = dis_add_service("PYSTRESS_SVC", "I", service_callback, 0)
    dis_start_serving("PYSTRESS") 
    while i < n or n == 0:
        res = dic_cmnd_service('STRESS_CMD', ('STRESSYOU!\0',), "C")
        dis_update_service(s)
        sleep(0.001)
    print "Stopping stress serving"
    dis_stop_serving()
    print "Stopping subscription"
    dic_release_service(si)
    print "Waiting for cleanup"
    sleep(5)
