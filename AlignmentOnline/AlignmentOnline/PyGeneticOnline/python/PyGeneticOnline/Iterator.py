from time import sleep
from Communicator import *
from ROOT import gROOT, std, TRandom3, TROOT, gSystem, TMVA, TString


def run():
    # Start the communicator:
    com = Communicator('ALIGNITER')

    # FSM loop
    state = State.NOT_READY
    com.set_status(state)

    opt= TString("PopSize=5:Steps=3:Cycles=3:ConvCrit=0.01:SaveBestCycle=5") 
    name = TString("FitterGA") 

    lo = std.vector('double')()
    hi = std.vector('double')()
    lo.push_back(100.)	
    hi.push_back(400.)
    lo.push_back(20.)	
    hi.push_back(100.)
    lo.push_back(600.)	
    hi.push_back(1300.)

    Channels = std.vector('int')()
    Channels.push_back(1)
    Channels.push_back(2)	
    Channels.push_back(6)

    DefParameters = std.vector('double')()
    DefParameters.push_back(179.)
    DefParameters.push_back(240.)
    DefParameters.push_back(600.)
    DefParameters.push_back(600.)
    DefParameters.push_back(900.)
    DefParameters.push_back(600.)
    DefParameters.push_back(60.)
    DefParameters.push_back(900.)

    UsedParameters = std.vector('int')()
    UsedParameters.push_back(1)
    UsedParameters.push_back(6)
    UsedParameters.push_back(7)

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

    RunType = TString("FULL")

    fitter = TMVA.GeneticFitterMod(name,lo,hi, Channels,opt,DefParameters,UsedParameters)
    #fitter.CheckForUnusedOptions()
	    
    pars = std.vector('double')(2)


    import L0_noHlt1_histosGen_Multi
    gROOT.ProcessLine('.! rm -rf $OUTPUTDIR/*.root')

  

    cycle = 0
    firstEntry = True  
    StartIterationFirstTime=True 
    FinishIterationAndStartNew = False


    print type(cycle)
    while True:
        command = com.get_command()
        if command == 'configure' and state == State.NOT_READY:
            fitter.RunInit(pars)
            state = State.READY

        elif command == 'start' and state == State.READY:
            fitter.CycleInit(cycle)
            fitter.ga.WriteParamsToFile(cycle)
            StartIterationFirstTime=True
            StartNewCycle=False
            FinishIterationAndStartNew=False
            print 'WANT TO CALCULATE FITNESS FIRST TIME' 

    
        elif command == 'pause' and state == State.RUNNING:
            state = State.PAUSED
            print 'FITNESS CALCULATED'
            com.set_status(state)
            if cycle < fitter.GenNcycles():
                if StartNewCycle:
                    fitter.CycleInit(cycle)
                    fitter.ga.WriteParamsToFile(cycle)
                    StartNewCycle=False
                    state = State.RUNNING
                    StartIterationFirstTime=True
                    print 'WANT TO CALCULATE FITNESS FIRST TIME' 
                elif StartIterationFirstTime:
                    print 'CYCLE %d' % cycle
                    print 'FIRST ITERATION'
                    gROOT.ProcessLine('.! hadd -f $OUTPUTDIR/bwdhistosRESULT.root $OUTPUTDIR/bwdhistosRes*')
                    fitter.ga.CalculateFitness()
                    fitter.ga.GetGeneticPopulation().TrimPopulation()
                    fitter.IterationInit(cycle)
                    fitter.ga.WriteParamsToFile(cycle)
                    StartIterationFirstTime = False
                    FinishIterationAndStartNew = True
                    state = State.RUNNING
                    print 'WANT TO CALCULATE FITNESS'
                elif FinishIterationAndStartNew:
                    gROOT.ProcessLine('.! hadd -f $OUTPUTDIR/bwdhistosRESULT.root $OUTPUTDIR/bwdhistosRes*')
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
                        FinishIterationAndStartNew = False
                        StartNewCycle = True
                    state = State.RUNNING
            else:
                gROOT.ProcessLine('.! hadd -f $OUTPUTDIR/bwdhistosRESULT.root $OUTPUTDIR/bwdhistosRes*')
                fitness = fitter.RunExit()  
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
