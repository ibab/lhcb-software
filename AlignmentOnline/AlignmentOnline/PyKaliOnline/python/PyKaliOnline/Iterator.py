from time import sleep, asctime
import os
from RunKali            import SplitHistos, CollectLambdas, GetOutput
from PyKaliOnline.Paths import *
from Communicator       import *

def run(directory='.'):
    # Start the communicator:
    com = Communicator('AligDrv_0')
    # FSM loop
    state = State.NOT_READY
    com.set_status(state)
    n_it = 0
    i_it = 0
    p_it = 2
    pt = Paths()
    gz_dir  = pt.gz_dir
    dst_dir = pt.dst_dir
    store_location = pt.store_location()
    ##filout = open(os.path.join(gz_dir,'printouts.txt'),'w')

    while True:
        command = com.get_command()
        if command == 'configure' and state == State.NOT_READY:
            ## Remove, if something has left from the previous run
            if n_it == 0 and p_it == 2:
                #print 'Exterminate!'
                dbases = [os.path.join(gz_dir,f) for f in os.listdir(gz_dir) if re.match(r'.*\.(db)$', f) or re.match(r'.*\.(gz)$', f)]
                #print 'Exterminate!!!',dbases
                for db in dbases: os.remove(db)
                #stored = [os.path.join(pt.store_location(),f) for f in os.listdir(pt.store_location())]
                #print 'EXTERMINATE!!!',stored
                #for st in stored: os.remove(st)

            state = State.READY

        elif command == 'start' and state == State.READY:
            state = State.RUNNING

        elif command == 'pause' and state == State.RUNNING:
            state = State.PAUSED
            com.set_status(state)
            if n_it < MaxIt:
                n_it += 1
                i_it += 1

                print asctime(),"Hi, I'm the iterator and I'm running", p_it, i_it, n_it

                ## If enough primary iterations are done
                ## Set the new global iteration
                if i_it > PassIt:
                    i_it = 0
                    p_it += 1
                print 'iteration %d, pass %d'%(i_it, p_it)
                
                lambdas_location   = pt.lambdas_location()
                histos             = pt.gethistomaps()
                lams               = pt.getlambdamaps()

                for i in range(1+MaxIt/PassIt):
                    if lambdas_location%i in lams:
                        lams.remove(lambdas_location%i)
                        print 'Remove', lambdas_location%i, 'from', lams

                ## if there are any histograms,
                ## split them between the nodes
                ## to prepare for fitting
                if histos:
                    #print "SPLITTING THE HISTOGRAMS"
                    SplitHistos   (gz_dir)

                ## if there are any coefficients,
                ## collect them together and save
                if lams  :
                    #print "COLLECTING THE LAMBDAS"
                    CollectLambdas(gz_dir)
                
                state = State.RUNNING
            else:
                print 'iterator done'
                ## create the output plots
                GetOutput(gz_dir)

                ## clear
                dbases = [os.path.join(gz_dir,f) for f in os.listdir(gz_dir) if re.match(r'.*\.(db)$', f)]
                for db in dbases: os.remove(db)

                stored = [os.path.join(pt.store_location(),f) for f in os.listdir(pt.store_location()) if re.match(r'.*\.(db)$', f)]
                for st in stored: os.remove(st)

                state = State.READY
            #sleep(2)
        elif command == 'stop' and state in (State.RUNNING, State.READY):
            ##filout.close()
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
