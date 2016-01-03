from Communicator import *
import os, socket, re, importlib, time, random
from RunKali            import fmDSTprod, RunKali
from RunBrunel          import RunBrunel
from PyKaliOnline.Paths import *

def run(index, input_directory):
    # Start the communicator:
    com = Communicator("ALIGNWORK_%02d" % index)

    # FSM loop
    state = State.NOT_READY
    com.set_status(state)
    n_it = 0
    p_it = 2
    hn = socket.gethostname()
    pt = Paths(index,hn)
    #pt = Paths(input_file,index,hn)
    while True:
        command = com.get_command()
        if command == 'configure' and state == State.NOT_READY:
            state = State.READY
        elif command == 'start' and state == State.READY:
            state = State.RUNNING
            com.set_status(state)
            n_it += 1

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

            ## check, if there's anything to fit
            histos = pt.histos_location_an()
            #print histos
            lambdas= pt.lambdas_location_an()
            #if pt.lambdas_location() in lambdas: lambdas.remove(pt.lambdas_location())
            #print lambdas

            if os.path.exists(histos):
                print 'Fitting the histograms from', histos, 'whith no coefficients'
                RunKali(histos,process='Fit',index=index)
                state = State.PAUSED

            # if not, fill the histograms, or do whatever is necessary for it etc.
            else:
                ## first check, what files already exist
                fmdstfiles      = pt.getfmdstfiles()
                dstfiles        = pt.getdstfiles()
                histos_location = pt.histos_location_an()
                rootfiles       = pt.getrootfiles()

                ## path to the calibration constants
                lambdas_location    = pt.lambdas_location()
                lambdas_db_location = pt.lambdas_db_location()

                ## Run reconstruction if necessary
                #if not dstfiles:
                #    print "Running event reconstruction at first"
                #    list_of_files = pt.getinputfiles()[index]
                #    RunBrunel(list_of_files, index)
                #    #RunBrunel(input_file, index)
                #
                #    dstfiles = pt.getdstfiles()

                ## Creat the fmdsts and root files if necessary
                #if not fmdstfiles and not rootfiles:
                if not rootfiles:
                    print "And create the fmDSTs and tuples"
                    fmDSTprod(dstfiles[0], index = index)
                    rootfiles       = pt.getrootfiles()
                    fmdstfiles      = pt.getfmdstfiles()

                ## Run re-reconstruction, if it's time already
                print "Should the reconstruction start?",n_it, PassIt
                if n_it > PassIt and os.path.exists(lambdas_db_location%p_it):
                    print "=="*20
                    print "Running the re-reconstruction"
                    print "=="*20
                    fmdsts = pt.getfmdstfiles()
                    #print lambdas_db_location%p_it
                    fmDSTprod(fmdsts[index], lambdas_db_location%p_it, index)
                    rootfiles       = pt.getrootfiles()
                    n_it = 0
                    p_it += 1
                    # get the new root files after the re-reconstruction
                elif n_it > PassIt and not os.path.exists(lambdas_db_location%p_it):
                    print "Can't make re-reconstruction without coefficients"
                    break

                ## check for the calibration constants
                lambdas = None
                if os.path.exists(lambdas_location%p_it): lambdas = lambdas_location%p_it

                ## fill the histograms from the corresponding root file
                RunKali(rootfiles[0], lambdas=lambdas, index=index)
            
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

        time.sleep(random.uniform(0.5, 1.5))
        # Set the status
        com.set_status(state)

    time.sleep(random.uniform(0.5, 1.5))
    # Set the status one last time.
    com.set_status(state)

if __name__ == '__main__':                
    run(0)
