import sys
from debug import SAY, ERROR, DEBUG
import pydim, debug
from time import sleep

debug._DEBUG = 1


global counter
counter = 1

def dummy(*args):
    print 'I am an unbound dummy func', args

def dis_service1 (tag):
    print 'function servce1 called'
    global counter
    return (counter, counter+1, 10.0, 10.0 , 'CUCU', 'A')

class Struct:
    def dummy(self, *args):
        print 'function servce2 called'
        print "I am dummy func bound to Struct. Received", args

    def service2(self, tag):
        global counter
        counter += 1
        return counter


print dir(pydim)


def test_dis_interface():
    print 80*'-'
    x = pydim.dis_get_dns_node()
    pydim.dis_set_dns_node('tralala')
    if not pydim.dis_get_dns_node() == 'tralala':
        ERORR('get/set dns failed')
        sys.exit(1)
    pydim.dis_set_dns_node(x)
    print 'dis_get/dns_node functions tested'

    print 80*'-'
    x = pydim.dis_get_dns_port()
    pydim.dis_set_dns_port(-2525)
    if not pydim.dis_get_dns_port() == 2525:
        ERROR('get/set dns port failed. Received ports',x,pydim.dis_get_dns_port() )
    pydim.dis_set_dns_port(x)
    print 'dis_get/dns_port functions tested'

    s=Struct()

    pydim.dis_add_exit_handler(dummy)
    pydim.dis_add_exit_handler(s.dummy)

    pydim.dis_add_error_handler(dummy)
    pydim.dis_add_error_handler(s.dummy)

    pydim.dis_add_client_exit_handler(dummy)
    pydim.dis_add_client_exit_handler(s.dummy)

    pydim.dis_start_serving()
    pydim.dis_stop_serving()

    print 'dis_update_service', pydim.dis_update_service(1)
    print 'dis_selective_update_service', pydim.dis_selective_update_service(1,(1,2,3))
    print 'dis_selective_update_service', pydim.dis_selective_update_service(1,[1,2,3])

    print 'dis_set_quality', pydim.dis_set_quality(1, 10)
    print 'dis_set_timestamp: ', pydim.dis_set_timestamp(1,1,1)
    print 'dis_remove_service: ',pydim.dis_remove_service(1)

    print 'dis_get_next_cmnd: ',pydim.dis_get_next_cmnd(10)
    print 'dis_get_client: ', pydim.dis_get_client('fdas')
    print 'dis_get_conn_id: ', pydim.dis_get_conn_id()
    print 'dis_get_timeout: ', pydim.dis_get_timeout(1, 1)
    print 'dis_get_client_services: ', pydim.dis_get_client_services(10)
    print 'dis_set_client_exit_handler', pydim.dis_set_client_exit_handler(1,1)
    print 'dis_get_error_services', pydim.dis_get_error_services()
    i=1
    sleep(1)
    pydim.dis_add_cmnd('test1', "C:20", dummy, 1)
    pydim.dis_add_cmnd('test2', "F:2;D:2;I:3;S:1;C:5;C:1", Struct().dummy, 2)

    svc1 = pydim.dis_add_service('Test Service Nr.1', "F:1;I:1;D:2;C:1;C:1;", service1, 1)
    svc2 = pydim.dis_add_service('Test Service Nr.2', "D:1", Struct().service2, 2)

    print("Starting serving services 1 and 2. Their ids are", svc1, svc2)
    pydim.dis_start_serving()
    while True:
         global counter
         if counter%2:
             counter += 1
             print("Updating service nr. 1")
             print pydim.dis_update_service( svc1, (counter, counter+1, 999.0, 999.0 , 'BAU', 'B') )
             print("Updating service nr. 2")
             print pydim.dis_update_service(svc2, 100)
         else:
             print("Updating service nr. 1")
             print pydim.dis_update_service( svc1 )
             print("Updating service nr. 2")
             print pydim.dis_update_service( svc2 )
         sleep(5)

global __dic_service_not_refreshed
__dic_service_refreshed = 0

def dic_command_callback(*args):
    print 'dic callback called. Args are', args
    global __dic_service_refreshed
    __dic_service_refreshed = 1

def test_dic_interface():    
    pydim.dic_info_service('SERVICE_NO_FUNC', 'C:1;F:1;C:20', dic_command_callback)
    while not __dic_service_refreshed:
       sleep(1)

if __name__=='__main__':
    test_dic_interface()
