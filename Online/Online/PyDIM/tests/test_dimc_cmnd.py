"""
@file test_dimc_cmnd.py
@brief An example of using a command service from Python. 
Contains both the server and client parts.

@author Radu Stoica (Radu.Stoica@cern.ch)
\addtogroup examples
@{
"""

from debug import SAY, ERROR, DEBUG
import sys, pydim, debug
from time import sleep
import gc

debug._DEBUG = 1


COMMAND_DELAY=3

CMND1FORMAT="C"
CMND2FORMAT="F:2;D:2;I:1;S:1;C"
CMND3FORMAT="I"

def dummy(*args):
  SAY('Server: I am an unbound dummy function. I\'ve received', args)


class MyClass:
  def dummy(self, *args):    
    SAY("Server: I am dummy func bound to a 'MyClass' instance I\'ve received", args)


def server_create_command_services():
  pydim.dis_add_cmnd('test1', CMND1FORMAT, dummy, 1)
  SAY('Added command test1 with DIM format %s' %CMND1FORMAT)

  pydim.dis_add_cmnd('test2', CMND2FORMAT, MyClass().dummy, 2)
  SAY('Added command test2 with DIM format %s' %CMND2FORMAT)

  pydim.dis_add_cmnd('test3', CMND3FORMAT, dummy, 2)
  SAY('Added command test3 with DIM format %s' %CMND3FORMAT)

  pydim.dis_start_serving()
  

if __name__ == '__main__':
  # Server configuration:
  server_create_command_services()
  
  # The client command execution
  counter = 0  
  while  counter<1000 or True:    
    counter += 1
    # executing the first command
    print ""
    tuple_args = ('Test call no. %03d' %counter, )
    res = pydim.dic_cmnd_service('test1', tuple_args, CMND1FORMAT)
    if res:   
      SAY('Client: Successfully executed command test1 (return code %d)' %res)
    else:
      SAY('Client: Command test1 execution failed (return code %d)' %res)
    sleep(COMMAND_DELAY)
  
    # second command
    print ""
    tuple_args = (counter, counter+1, counter*9, counter*1.9, counter, counter-1, 'ABRACA\x00DABRA')
    res = pydim.dic_cmnd_service('test2', tuple_args, CMND2FORMAT)
    if res:   
      SAY('Client: Successfully executed command test2 (return code %d)' %res)
    else:
      SAY('Client: Command test2 execution failed (return code %d)' %res)
    sleep(COMMAND_DELAY)
  
    # last command
    print ""
    list_args = range(counter, counter+2) # lists are also accepted   
    res = pydim.dic_cmnd_service('test3', list_args, CMND3FORMAT)
    if res:   
      SAY('Client: Successfully executed command test3 (return code %d)' %res)
    else:
      SAY('Client: Command test3 execution failed (return code %d)' %res)

    sleep(COMMAND_DELAY)
