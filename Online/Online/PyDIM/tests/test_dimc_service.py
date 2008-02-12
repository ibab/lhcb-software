import sys
from debug import SAY, ERROR, DEBUG
import dimc, debug
from time import sleep
from threading import Thread, Event

debug.DEBUG = 1

global counter
counter = 1
#STOP=Event()
SRV1NAME='Test_Serv1'
SRV2NAME='Test_Serv2'
SRV3NAME='Test_Serv3'
SRV1FORMAT='F:1;I:1;D:2;C:10;C:100' # buffer size is 35 on 32 bits 
SRV2FORMAT='D:1' 
SRV3FORMAT='C:100'


def dummy(*args):
    print 'I am an unbound dummy func', args

def dis_service1 (tag):
    print 'function service1 called'
    global counter
    return (counter, counter+1, 10.0, 10.0 , 'CUCU', 'A')

def dis_service3 (tag):
    print 'function service3 called'
    return '1234567890'

class Struct:
    def dummy(self, *args):
        print 'function service2 called'
        print "I am dummy func bound to Struct. %s Received %s" \
               % (str(self), str(args))

    def service2(self, tag):
        global counter
        counter += 1
        return counter

def client_callback1(*args):
    print 'client callback called for service1. Args are', args

def client_callback2(*args):
    print 'client callback called for service2. Args are', args

def client_callback3(*args):
    print 'client callback called for service3. Args are', args

if __name__=='__main__':
    counter = 0
    #dimc.dic_info_service('Test_Serv1', SRV1FORMAT, client_callback1)
    #dimc.dic_info_service('Test_Serv2', SRV2FORMAT, client_callback2)
    #dimc.dic_info_service('Test_Serv3', SRV3FORMAT, client_callback3)
    svc1 = dimc.dis_add_service(SRV1NAME, SRV1FORMAT, dis_service1, 1 )    
    svc2 = dimc.dis_add_service(SRV2NAME, SRV2FORMAT, Struct().service2, 2)
    svc3 = dimc.dis_add_service(SRV3NAME, SRV3FORMAT, dis_service1, 3 )
    SAY("Starting serving services. Their ids are", svc1, svc2)
    dimc.dis_start_serving()
    while counter < 1000:
         counter += 1
         SAY("Updating service nr. 1")
         values = (counter, counter+1, 999.0, 999.0 , 'BAU', 'B')
         SAY('Update %d clients' %dimc.dis_update_service(svc1, values))
         SAY("Updating service nr. 2")
         values = (counter, )
         SAY('Update %d clients' %dimc.dis_update_service(svc2, values))
         SAY("Updating service nr. 3")
         values = ('ALABALAPORTOCALA', )
         SAY('Update %d clients' %dimc.dis_update_service(svc3, values))
         sleep(1)
