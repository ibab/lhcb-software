from debug import SAY, ERROR, DEBUG
import sys, pydim, debug
from time import sleep
import gc

debug._DEBUG = 1

CMND1FORMAT="C"
CMND2FORMAT="F:2;D:2;I:1;S:1;C"
CMND3FORMAT="I"
COMMAND_DELAY = 1

def dummy(*args):
    SAY('Server: I am an unbound dummy function. I\'ve received', args)


class MyClass:
    def dummy(self, *args):    
        SAY("Server: I am dummy func bound to a 'MyClass' instance. I\'ve received", args)


def client_callback(tag, ret):
    SAY('The client callback function has received tag %d and return code %d' %(tag, ret))


def server_create_command_services():
    #adding the first command
    pydim.dis_add_cmnd('test1', CMND1FORMAT, dummy, 1)
    SAY('Added command test1 with DIM format %s' %CMND1FORMAT)
    #adding the second command
    pydim.dis_add_cmnd('test2', CMND2FORMAT, MyClass().dummy, 2)
    SAY('Added command test2 with DIM format %s' %CMND2FORMAT)
    #adding the third command
    pydim.dis_add_cmnd('test3', CMND3FORMAT, dummy, 2)
    SAY('Added command test3 with DIM format %s' %CMND3FORMAT)
    #starting DIM
    pydim.dis_start_serving()
  
  
if __name__ == '__main__':
    # Server configuration:
    server_create_command_services()
  
    #The client command execution
    counter = 0  
    while  counter<1000 or True:    
        counter += 1
        print('\n\n'+80*'-')
        print('Iteration number %d' %counter)
        # executing the first command
        tuple_args = ('Test call no. %03d' %counter, )
        res = pydim.dic_cmnd_callback('test1', tuple_args, CMND1FORMAT, client_callback, 1)
        if res:   
            SAY('Client: Successfully executed command test1 (return code %d)' %res)
        else:
            SAY('Client: Command test1 execution failed (return code %d)' %res)
        sleep(COMMAND_DELAY)
        print
  
        # second command
        tuple_args = (counter, counter+1, counter*9, counter*1.9, counter, counter-1, 'ABRACADABRA')
        res = pydim.dic_cmnd_callback('test2', tuple_args, CMND2FORMAT, client_callback, 2)
        if res:   
            SAY('Client: Successfully executed command test2 (return code %d)' %res)
        else:
            SAY('Client: Command test2 execution failed (return code %d)' %res)
        sleep(COMMAND_DELAY)
        print
  
        # last command
        list_args = range(counter, counter+5) #also lists are accepted        
        res = pydim.dic_cmnd_callback('test3', list_args, CMND3FORMAT, client_callback, 3)
        if res:   
            SAY('Client: Successfully executed command test3 (return code %d)' %res)
        else:
            SAY('Client: Command test3 execution failed (return code %d)' %res)
        sleep(COMMAND_DELAY)
        print(80*'-')

