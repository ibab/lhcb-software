from Communicator import *
import os, socket, re, importlib, time, random
from RunKali            import fmDSTprod, RunKali
from RunBrunel          import RunBrunel
from PyKaliOnline.Paths import *

def run_online(data_directory):
    fifo = os.environ.get('LOGFIFO', None)
    if not fifo:
        run(0, data_directory)
    else:
        old_stdout = os.dup( sys.stdout.fileno() ) 
        fifo = open(fifo, 'w')
        os.dup2( fifo.fileno(), sys.stdout.fileno() ) 
        run(0, data_directory)
        os.close( fifo.fileno() ) 
        os.dup2( old_stdout, sys.stdout.fileno() )

def run(index, data_directory):
    # Start the communicator:
    com = Communicator("AligWrk_%d" % index)

    # FSM loop
    state = State.NOT_READY
    com.set_status(state)
    n_it = 0
    p_it = 2
    hn = socket.gethostname()
    pt = Paths(index, hn)
    #filout = open(os.path.join(pt.gz_dir,'AnaPrint_%s.txt'%hn),'w')
    while True:
        command = com.get_command()
        if command == 'configure' and state == State.NOT_READY:
            state = State.READY
        elif command == 'start' and state == State.READY:
            state = State.RUNNING
            com.set_status(state)
            n_it += 1

            ## check, if there's anything to fit
            histos = pt.histos_location_an()
            lambdas= pt.lambdas_location_an()

            print "HISTOS", histos
            print "LAMBDAS", lambdas

            ## Clear everything that was
            ## left from the previous run
            ## Uncomment these lines if you feel
            ## that it is necessary
            if n_it == 1 and p_it == 2:
                print "TIME TO REMOVE"
                if os.path.exists(histos) : print "HISTOS", histos      ; os.remove(histos)
                if os.path.exists(lambdas): print "AND LAMBDAS", lambdas; os.remove(lambdas)
                #os.system('rm /group/calo/CalibWork/*.db')
                rootfiles       = pt.getrootfiles()
                fmdstfiles      = pt.getfmdstfiles()
                dstfiles        = pt.getdstfiles()
                brunelhistos    = pt.getbrunelhistos()
                for bh in brunelhistos: os.remove(bh)
                for ds in dstfiles: os.remove(ds)
                for fm in fmdstfiles: os.remove(fm)
                for rf in rootfiles: os.remove(rf)

            ## if there are any histograms - fit them
            if os.path.exists(histos):
                print 'Fitting the histograms from', histos, 'whith no coefficients'
                RunKali(histos,process='Fit',index=index)

            ## if not do whatever is necessary to obtain them
            else:
                ## first check, what files already exist
                fmdstfiles      = pt.getfmdstfiles()
                dstfiles        = pt.getdstfiles()
                histos_location = pt.histos_location_an()

                ## path to the calibration constants
                lambdas_location    = pt.lambdas_location()
                lambdas_db_location = pt.lambdas_db_location()

                ## Run reconstruction if necessary
                list_of_files, first_run, last_run = pt.getinputfiles()
                if not dstfiles:
                    print "Running event reconstruction at first"

                    ## If there are no input files, we have to immediately go to
                    ## paused.
                    if not list_of_files:
                        time.sleep(random.uniform(0.5, 1.5))
                        state = State.PAUSED
                        com.set_status(state)
                        continue
                    
                    RunBrunel(list_of_files, index, first_run, last_run)

                    dstfiles = pt.getdstfiles()
                    rootfiles= pt.getrootfiles()
                    for bh in rootfiles:
                        os.system('chmod a+rwx '+bh)
                        os.system('mv %s %shist'%(bh,bh))

                ## Creat the fmdsts and root files if necessary
                rootfiles       = pt.getrootfiles()
                if not fmdstfiles or not rootfiles:
                    ## If there are no input files, we have to immediately go to
                    ## paused.
                    if not dstfiles:
                        time.sleep(random.uniform(0.5, 1.5))
                        state = State.PAUSED
                        com.set_status(state)
                        continue

                    print "And create the fmDSTs and tuples"
                    fmDSTprod(dstfiles, index = index)
                    rootfiles       = pt.getrootfiles()
                    fmdstfiles      = pt.getfmdstfiles()
                    for i in range(len(rootfiles)):
                        os.system('chmod a+rwx '+rootfiles[i])
                        os.system('chmod a+rwx '+fmdstfiles[i])

                ## Run re-reconstruction, if it's time already
                print "Should the reconstruction start?",n_it, PassIt
                if n_it > PassIt and os.path.exists(lambdas_db_location%p_it):
                    print "=="*20
                    print "Running the re-reconstruction"
                    print "=="*20
                    ## If there are no input files, we have to immediately go to
                    ## paused.
                    if not fmdstfiles:
                        time.sleep(random.uniform(0.5, 1.5))
                        state = State.PAUSED
                        com.set_status(state)
                        continue

                    #fmdst = fmdstfiles[0]
                    print lambdas_db_location%p_it
                    fmDSTprod(fmdstfiles, lambdas_db_location%p_it, index)
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

                ## If there are no input files, we have to immediately go to
                ## paused.
                if not rootfiles:
                    time.sleep(random.uniform(0.5, 1.5))
                    state = State.PAUSED
                    com.set_status(state)
                    continue
                
                ## fill the histograms from the corresponding root file
                print len(rootfiles)
                RunKali(rootfiles[0], lambdas=lambdas, index=index)
            
                #if n_it == PassIt and p_it == 2:
                #    rootfiles = [os.path.join(pt.dst_dir,f) for f in os.listdir(pt.dst_dir) if re.match(r'.*\.(root|ROOT)$', f)]
                #    for rf in rootfiles: os.remove(rf)

                #    fmdstfiles = [os.path.join(pt.dst_dir,f) for f in os.listdir(pt.dst_dir) if re.match(r'.*\.(fmdst|fmDST)$', f)]
                #    for fm in fmdstfiles: os.remove(fm)

                #   dstfiles = [os.path.join(pt.dst_dir,f) for f in os.listdir(pt.dst_dir) if re.match(r'.*\.(dst|DST)$', f)]                
                #    for ds in dstfiles: os.remove(ds)

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
