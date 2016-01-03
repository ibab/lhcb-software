from time import sleep
import os
from RunKali            import SplitHistos, CollectLambdas, GetOutput
from PyKaliOnline.Paths import *
from Communicator       import *

def run(directory='.'):
    # Start the communicator:
    com = Communicator('ALIGNITER')

    # FSM loop
    state = State.NOT_READY
    com.set_status(state)
    n_it = 0
    i_it = 0
    p_it = 2
    pt = Paths()
    gz_dir  = pt.gz_dir
    dst_dir = pt.dst_dir
    #pt = Paths(directory)
    store_location = pt.store_location()
    while True:
        command = com.get_command()
        if command == 'configure' and state == State.NOT_READY:
            state = State.READY
        elif command == 'start' and state == State.READY:
            state = State.RUNNING
        elif command == 'pause' and state == State.RUNNING:
            state = State.PAUSED
            com.set_status(state)
            if n_it < MaxIt:
                n_it += 1
                i_it += 1

                ## find the last one saved and
                ## save current as the next one
                if n_it == 1:
                    StoredHistoPath  = pt.histos_location_it()
                    ih   = 1
                    Pass = 1
                    while True:
                        if ih > PassIt/2:
                            Pass += 1
                            ih    = 1
                        if os.path.exists(StoredHistoPath%(Pass,ih)): ## look through all the existing files
                            ih += 1
                            continue
                        else:                                  ## write to the file next to the latest existing
                            n_it = ih
                            p_it = Pass
                            break


                ## If enough primary iterations are done
                ## Set the new global iteration
                if i_it > PassIt:
                    i_it = 0
                    p_it += 1
                print 'iteration %d, pass %d'%(i_it, p_it)
                
                #RunKali(lambdas_location, 'Fit')
                lambdas_location   = pt.lambdas_location()
                #lambdas_location_it= pt.lambdas_location_it()
                histos             = pt.gethistomaps()
                lams               = pt.getlambdamaps()

                for i in range(1+MaxIt/PassIt):
                    if lambdas_location%i in lams:
                        lams.remove(lambdas_location%i)
                        print 'Remove', lambdas_location%i, 'from', lams
                #if lambdas_location in lams: lams.remove(lambdas_location) ; print 'Remove', lambdas_location, 'from', lams

                if histos:
                    #print "SPLITTING THE FILLED HISTOGRAMS"
                    #SplitHistos   (directory)
                    SplitHistos   (gz_dir)
                    state = State.RUNNING

                if lams  :
                    #print "COLLECTING THE CALIBRATION COEFFICIENTS"
                    #CollectLambdas(directory)
                    CollectLambdas(gz_dir)
                
                #print 'Copying lambdas from ', lambdas_location, ' to ', lambdas_location_it
                #os.system('cp '+lambdas_location+' '+lambdas_location_it)
                
                state = State.RUNNING
            else:
                print 'iterator done'
                #GetOutput(directory)
                GetOutput(gz_dir)
                ## clear
                dbases = [os.path.join(gz_dir,f) for f in os.listdir(gz_dir) if re.match(r'.*\.(db)$', f)]
                #for db in dbases: os.remove(db)

                rootfiles = [os.path.join(dst_dir,f) for f in os.listdir(dst_dir) if re.match(r'.*\.(root|ROOT)$', f)]
                #for rf in rootfiles: os.remove(rf)

                fmdstfiles = [os.path.join(dst_dir,f) for f in os.listdir(dst_dir) if re.match(r'.*\.(fmdst|fmDST)$', f)]
                #for fm in fmdstfiles: os.remove(fm)

                dstfiles = [os.path.join(dst_dir,f) for f in os.listdir(dst_dir) if re.match(r'.*\.(dst|DST)$', f)]                
                #for ds in dstfiles: os.remove(ds)

                stored = [os.path.join(pt.store_location(),f) for f in os.listdir(pt.store_location()) if re.match(r'.*\.(gz)$', f)]
                #for st in stored: os.remove(st)

                state = State.READY
            #sleep(2)
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
