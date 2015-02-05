from time import sleep
from Communicator import *
import random
import struct

def run(index):
    # Start the communicator:
    com = Communicator("ALIGNWORK_%02d" % index)

    with open("/dev/urandom", "rb") as rand_src:
        bytes = rand_src.read(4)
        random.seed(struct.unpack("I", bytes)[0])
    
    # FSM loop
    state = State.NOT_READY
    com.set_status(state)
    n_it = 0
    while True:
        command = com.get_command()
        if command == 'configure' and state == State.NOT_READY:
            state = State.READY
        elif command == 'start' and state == State.READY:
            state = State.RUNNING
            com.set_status(state)
            sleep(random.uniform(1, 3))
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

        sleep(random.uniform(0.5, 1.5))
        # Set the status
        com.set_status(state)

    # Set the status one last time.
    com.set_status(state)

if __name__ == '__main__':                
    run(0)
