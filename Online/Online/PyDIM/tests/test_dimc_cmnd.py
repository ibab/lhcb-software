from debug import SAY, ERROR, DEBUG
import sys, pydim, debug
from time import sleep

debug._DEBUG = 1

CMND1FORMAT="C:200"
CMND2FORMAT="F:2;D:2;I:1;S:1;C:100;"

def dummy(*args):
    SAY('I am an unbound dummy func. I\'ve received', args)


class Struct:
    def dummy(self, *args):
        SAY("I am dummy func bound to a Struct instance.I\'ve received", args)



if __name__ == '__main__':    
    pydim.dis_add_cmnd('test1', CMND1FORMAT, dummy, 1)
    SAY('Added command test1')
    pydim.dis_add_cmnd('test2', CMND2FORMAT, Struct().dummy, 2)
    SAY('Added command test2')
    pydim.dis_start_serving()
    counter = 0
    while counter<1000:
        counter += 1
        res = pydim.dic_cmnd_service('test1', ('test call no. '+str(counter), ),
                                     CMND1FORMAT
                                     )
        SAY('Command test1 called and received result %d' %res)
        res = pydim.dic_cmnd_service('test2', 
                                     (counter, counter+1, 
                                      99.9, 1.9, 
                                      counter, 
                                      counter-1,
                                      'CUCUBAU'), 
                                      CMND2FORMAT
                                     )
        SAY('Command test2 called and received result %d' %res)
        sleep(1)
