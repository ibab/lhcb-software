import sys, os
from time import sleep
from Communicator import *
import subprocess
import ctypes
from ROOT import gROOT, std, TRandom3, TROOT, gSystem, TMVA, TString


def run_online():
    fifo = os.environ.get('LOGFIFO', None)
    if not fifo:
        run()
    else:
        old_stdout = os.dup( sys.stdout.fileno() ) 
        fd = os.open(fifo) 
        os.dup2( fd, sys.stdout.fileno() ) 
        run()
        os.close( fd ) 
        os.dup2( old_stdout, sys.stdout.fileno() )

def run():
    # Start the communicator:
    utgid = os.environ.get('UTGID', 'ALIGNITER')
    com = Communicator(utgid)

    # FSM loop
    state = State.NOT_READY
    com.set_status(state)

    DefParameters = std.vector('double')()
    DefParameters.push_back(179.)
    DefParameters.push_back(240.)
    DefParameters.push_back(600.)
    DefParameters.push_back(600.)
    DefParameters.push_back(900.)
    DefParameters.push_back(600.)
    DefParameters.push_back(60.)
    DefParameters.push_back(900.)

 
    LineNames = std.vector('string')()
    LineNames.push_back('mbias')
    LineNames.push_back('Bs2Dsmunu')
    LineNames.push_back('Lb2pmunu')
    LineNames.push_back('Bs2phigamma')
    LineNames.push_back('Bs2mumu')
    LineNames.push_back('Bs2Kstee')
    LineNames.push_back('Bd2Kstmumu')
    LineNames.push_back('Bd2Kpi')
    LineNames.push_back('Bs2phiphi')
    LineNames.push_back('DD')
    LineNames.push_back('Bs2DsPi')
    LineNames.push_back('Bs2JpsiPhi')
    LineNames.push_back('Bs2Jpsikkpipi')
    LineNames.push_back('D2kkpi')
    LineNames.push_back('Dst2Dpi2kskk')







    LineTypes = std.vector('int')()
    LineTypes.push_back(0)
    LineTypes.push_back(1)
    LineTypes.push_back(1)
    LineTypes.push_back(3)
    LineTypes.push_back(2)
    LineTypes.push_back(3)
    LineTypes.push_back(2)
    LineTypes.push_back(1)
    LineTypes.push_back(1)
    LineTypes.push_back(1)
    LineTypes.push_back(1)
    LineTypes.push_back(2)
    LineTypes.push_back(2)
    LineTypes.push_back(1)
    LineTypes.push_back(1)

    MaxEfficiencies = std.vector('double')()
    MaxEfficiencies.push_back(0.4)
    MaxEfficiencies.push_back(0.9)
    MaxEfficiencies.push_back(0.9)
    MaxEfficiencies.push_back(0.6)
    MaxEfficiencies.push_back(0.9)
    MaxEfficiencies.push_back(0.6)
    MaxEfficiencies.push_back(0.3)
    MaxEfficiencies.push_back(0.3)
    MaxEfficiencies.push_back(0.3)
    MaxEfficiencies.push_back(0.2)
    MaxEfficiencies.push_back(0.7)
    MaxEfficiencies.push_back(0.6) 

    name = TString("FitterGA") 


    Channels = std.vector('int')()
    Channels.push_back(1)
    Channels.push_back(2)
    # Channels.push_back(3)
    # Channels.push_back(4)
    # Channels.push_back(5)
    # Channels.push_back(6)
    # Channels.push_back(7)
    # Channels.push_back(8)
    # Channels.push_back(9)
    # Channels.push_back(10)	
    # Channels.push_back(11)
    # Channels.push_back(12)
    # Channels.push_back(13)
    # Channels.push_back(14)	
    # Channels.push_back(15)

    


    lo = std.vector('double')()
    hi = std.vector('double')()
    lo.push_back(100.)	
    hi.push_back(400.)
    # lo.push_back(20.)	
    # hi.push_back(100.)
    # lo.push_back(600.)	
    # hi.push_back(1300.)


    
    UsedParameters = std.vector('int')()
    #UsedParameters.push_back(0)
    UsedParameters.push_back(1)
    # UsedParameters.push_back(3)
    # UsedParameters.push_back(4)
    # UsedParameters.push_back(5)
    # UsedParameters.push_back(6)
    # UsedParameters.push_back(7)


    #fitter.CheckForUnusedOptions()

    pars = std.vector('double')(2)
    subprocess.call('rm -rf $OUTPUTDIR/*.root',shell=True)

  

    cycle = 0
    firstEntry = True  
    StartIterationFirstTime=True 
    FinishIterationAndStartNew = False


    print type(cycle)
    while True:
        command = com.get_command()
        if command == 'configure' and state == State.NOT_READY:


            import pickle

            f=open('BWTest.pkl')
            channels=pickle.load(f)
            trigger_lines=pickle.load(f)
            trigger_thresholds=pickle.load(f)
            statistics=pickle.load(f)
            f.close()



            opt= TString("PopSize=3:Steps=2:Cycles=2:ConvCrit=0.01:SaveBestCycle=5") 
            fitter = TMVA.GeneticFitterMod(name,lo,hi, Channels,opt,DefParameters,UsedParameters,MaxEfficiencies,LineNames,LineTypes)
            fitter.RunInit(pars)
            state = State.READY

        elif command == 'start' and state == State.READY:
            fitter.CycleInit(cycle)
            fitter.ga.WriteParamsToFile(cycle)
            StartIterationFirstTime=True
            FinishIterationAndStartNew=False
            state = State.RUNNING
            print 'WANT TO CALCULATE FITNESS FIRST TIME' 

    
        elif command == 'pause' and state == State.RUNNING:
            state = State.PAUSED
            print 'FITNESS CALCULATED'
            com.set_status(state)
            if cycle < fitter.GenNcycles():
                if StartIterationFirstTime:
                    print 'CYCLE %d' % cycle
                    print 'FIRST ITERATION'
#                    subprocess.call('rm -rf $OUTPUTDIR/bwdhistosRESULT.root',shell=True)
                    subprocess.call('hadd -f $OUTPUTDIR/bwdhistosRESULT.root $OUTPUTDIR/bwdhistosRes*',shell=True)

                    fitter.ga.CalculateFitness()
                    fitter.ga.GetGeneticPopulation().TrimPopulation()
                    fitter.IterationInit(cycle)
                    fitter.ga.WriteParamsToFile(cycle)
                    StartIterationFirstTime = False
                    FinishIterationAndStartNew = True
                    state = State.RUNNING
                    print 'WANT TO CALCULATE FITNESS'
                elif FinishIterationAndStartNew:
 #                   subprocess.call('rm -rf $OUTPUTDIR/bwdhistosRESULT.root',shell=True)
                    subprocess.call('hadd -f $OUTPUTDIR/bwdhistosRESULT.root $OUTPUTDIR/bwdhistosRes*',shell=True)
                    fitter.ga.CalculateFitness()
                    fitter.IterationExit(cycle)
                    if not fitter.ga.HasConverged(fitter.GetNsteps(),fitter.GetConvCrit()):
                        fitter.IterationInit(cycle)
                        fitter.ga.WriteParamsToFile(cycle)
                        print 'WANT TO CALCULATE FITNESS'
                    else:
                        fitter.CycleExit(cycle)
                        print 'CYCLE  ',cycle, '   FINISHED' 
                        cycle += 1
                        if not cycle < fitter.GenNcycles():
                            fitter.gstore.WriteParamsToFile(1)
                            print 'AAAAAAAA 1111'
                        else:
                            fitter.CycleInit(cycle)
                            fitter.ga.WriteParamsToFile(cycle)
                            StartIterationFirstTime=True  
                            print 'CYCLE %d' % cycle  
                            print 'WANT TO CALCULATE FITNESS FIRST TIME'                    
                        FinishIterationAndStartNew = False
                        StartIterationFirstTime=True
                        print 'AAAAAAAA 2222'
                    state = State.RUNNING
            else:
#                subprocess.call('rm -rf $OUTPUTDIR/bwdhistosRESULT.root',shell=True)
                print 'AAAAAAAA 3333'
                subprocess.call('hadd -f $OUTPUTDIR/bwdhistosRESULT.root $OUTPUTDIR/bwdhistosRes*',shell=True)
                print 'AAAAAAAA 4444'
                fitness = fitter.RunExit()  
                print 'AAAAAAAA 5555'
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
