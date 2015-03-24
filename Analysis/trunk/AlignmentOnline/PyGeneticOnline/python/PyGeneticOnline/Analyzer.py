from time import sleep
from Communicator import *
import random
import struct
import sys, os

def run(index):
    # Start the communicator:
    com = Communicator("ALIGNWORK_%02d" % index)
    
    # FSM loop
    state = State.NOT_READY
    com.set_status(state)
    n_it = 0
    n_workers = 2
    sleep(0.2)
    while True:
        command = com.get_command()
        if command == 'configure' and state == State.NOT_READY:
            import L0_noHlt1_histosGen_Multi
            state = State.READY
        elif command == 'start' and state == State.READY:
            state = State.RUNNING
            com.set_status(state)
            L0_noHlt1_histosGen_Multi.MultiRun(index, 2, 1, [0,1])  
            sleep(2)
            state = State.PAUSED
        elif command == 'stop' and state == State.PAUSED:
            state = State.READY
        elif command == 'reset':
            state = State.NOT_READY
            break
        else:
            print 'analyzer: bad transition from %s to %s' % (state, command)
            state = State.ERROR
            break

        sleep(2)
        # Set the status
        com.set_status(state)

    # Set the status one last time.
    com.set_status(state)

if __name__ == '__main__':              
    run(0)
