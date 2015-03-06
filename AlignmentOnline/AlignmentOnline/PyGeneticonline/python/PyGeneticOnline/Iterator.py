from time import sleep
from Communicator import *
from ROOT import gROOT, std, TRandom3, TROOT, gSystem, TMVA, TString


def run():
    # Start the communicator:
    com = Communicator('ALIGNITER')

    # FSM loop
    state = State.NOT_READY
    com.set_status(state)

    gROOT.Reset()
    myEvents = std.vector('double')()
    rand = TRandom3()
    mean=2.
    sigma=1.5

    for iev in range (0,1000):
        myEvents.push_back(rand.Gaus(mean,sigma))
  
  
    opt= TString("PopSize=10:Steps=10:Cycles=3:ConvCrit=0.01:SaveBestCycle=5") 
#    opt= TString("PopSize=20:Steps=30:Cycles=3:ConvCrit=0.01:SaveBestCycle=5") 
    name = TString("FitterGA") 


    lo = std.vector('double')()
    hi = std.vector('double')()
    lo.push_back(100.)	
    hi.push_back(400.)
    lo.push_back(20.)	
    hi.push_back(100.)
    lo.push_back(600.)	
    hi.push_back(1300.)
    # lo.push_back(-10.)	
    # hi.push_back(10.)
    # lo.push_back(0.1)	
    # hi.push_back(10.)
    # lo.push_back(0.)	
    # hi.push_back(2000.)


#    fitter = TMVA.GeneticFitterMod(myEvents,name,lo,hi, opt)
    fitter = TMVA.GeneticFitterMod(name,lo,hi, opt)

    pars = std.vector('Double_t')(2)

    cycle = 0
    firstEntry = True  
    StartIterationFirstTime=True 
    FinishIterationAndStartNew = False


    print type(cycle)
    print 'AAAAAAAAAAAAAAAAAAA'
    while True:
        command = com.get_command()
        if command == 'configure' and state == State.NOT_READY:
            fitter.RunInit(pars)
            state = State.READY

        elif command == 'start' and state == State.READY:
            state = State.RUNNING
            print 'iteration %d' % cycle
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
