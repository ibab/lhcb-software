import sys
from debug import SAY, ERROR, DEBUG
import dimc, debug
from time import sleep
from threading import Thread, Event

debug.DEBUG = 1

SRV1NAME='Test_Serv1'
SRV2NAME='Test_Serv2'
SRV3NAME='Test_Serv3'
SRV1FORMAT='F:1;I:1;D:2;C:10;C:100' # buffer size is 35 on 32 bits 
SRV2FORMAT='D:1' 
SRV3FORMAT='C:100'

def client_callback1(*args):
    print 'client callback called for service1. Args are', args

def client_callback2(*args):
    print 'client callback called for service2. Args are', args

def client_callback3(*args):
    print 'client callback called for service3. Args are', args

if __name__=='__main__':
    dimc.dic_info_service(SRV1NAME, SRV1FORMAT, client_callback1)
    dimc.dic_info_service(SRV2NAME, SRV2FORMAT, client_callback2)
    dimc.dic_info_service(SRV3NAME, SRV3FORMAT, client_callback3)
    sleep(1000)
    