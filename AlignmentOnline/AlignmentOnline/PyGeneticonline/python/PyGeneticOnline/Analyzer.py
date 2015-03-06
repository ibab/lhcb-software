from time import sleep
from Communicator import *
import random
import struct
import sys

def run(index):
    # Start the communicator:
    com = Communicator("ALIGNWORK_%02d" % index)
    
    # FSM loop
    state = State.NOT_READY
    com.set_status(state)
    n_it = 0
    n_workers = 5
    sleep(0.2)
    while True:
        sleep(0.2)
        command = com.get_command()
        sleep(0.2)
        if command == 'configure' and state == State.NOT_READY:
            sys.path.insert(0, '/home/ausachov/Trigger/TMVA/test')
            import L0_noHlt1_histosGen_Multi
            sleep(1)
            state = State.READY
            sleep(1)
        elif command == 'start' and state == State.READY:
            state = State.RUNNING
            sleep(1)
            com.set_status(state)
#            print index
#            print 'AAAAA'
            sleep(1)
            L0_noHlt1_histosGen_Multi.MultiRun(index, n_workers) 
            sleep(1)
            state = State.PAUSED
            sleep(1)
        elif command == 'stop' and state == State.PAUSED:
            sleep(0.1)
            state = State.READY
            sleep(0.1)
        elif command == 'reset':
            sleep(0.1)
            state = State.NOT_READY
            sleep(0.1)
            break
        else:
            print 'analyzer: bad transition from %s to %s' % (state, command)
            state = State.ERROR
            break


        # Set the status
        com.set_status(state)

    # Set the status one last time.
    com.set_status(state)

if __name__ == '__main__':   
    sleep(0.1)             
    run(0)
    sleep(0.1)
