from time import sleep
from Communicator import *

def run():
    # Start the communicator:
    com = Communicator('ALIGNITER')

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
        elif command == 'pause' and state == State.RUNNING:
            state = State.PAUSED
            com.set_status(state)
            if n_it < 4:
                n_it += 1
                print 'iteration %d' % n_it
                state = State.RUNNING
            else:
                print 'iterator done'
                state = State.READY
            sleep(2)
        elif command == 'stop' and state in (State.RUNNING, State.READY):
            state = State.READY
        elif command == 'reset':
            state = State.NOT_READY
            break
        else:
            print 'iterator: bad transition from %s to %s' % (state, command)
            state = State.ERROR
            break
        # Set the status
        com.set_status(state)

    # Set our status one last time
    com.set_status(state)

if __name__ == '__main__':                
    run()
