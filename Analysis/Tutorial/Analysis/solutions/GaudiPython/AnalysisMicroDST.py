
##
## initialise Gaudi, open file, etc
## (N.B. override namespace and load into current namespace)
##

from InitGaudi import *

##
## book histograms, etc
##
hMass = ROOT.TH1F("hMass","J/psi mass", 120, 2.5, 3.5)

##
## Event loop
##

def EventLoop(nEvents=10, verbose=0):
    iEvent = 0
    while iEvent < nEvents:
        iEvent = iEvent+1

        #read in the next event from Gaudi
        appMgr.run(1)

        #
        # set general event information
        #
        runNr       = -999
        eventNr     = -999
        eventHeader = evt['/Event/microDST/Rec/Header']
        if not eventHeader:
            print "eventHeader not found"
        else:
            runNr   = eventHeader.runNumber()
            eventNr = eventHeader.evtNumber()

            if verbose > 0:
                print "runNr %i, eventNr %i " % (runNr, eventNr)

            
        #
        # get muons
        #
        JPsiContainer = evt['/Event/microDST/Phys/JPsiMuMu/Particles']
        nJPsi = JPsiContainer.size()
        if verbose > 0:
            print "Found %i J/psi in this event" % nJPsi

        # break out of loop if container not found
        # (N.B. containers are required to exist by LHCb convention,
        #       i.e. if the container is not there we have reached the
        #       end of the file) 
        if not JPsiContainer:
            print "No further event found"
            break

        for JPsi in JPsiContainer:

            mass = JPsi.measuredMass()/1000
            chi2 = JPsi.endVertex().chi2()

            if verbose > 0:
                print "found new J/psi with mass %lf and chi2 %lf" %(mass,chi2)


            #
            # fill ntuple - one entry per candidate
            #
            EventInfo.runNr                = runNr
            EventInfo.eventNr              = eventNr
            
            JPsiInfo.mass                  = mass
            JPsiInfo.chi2                  = chi2

            rootTree.Fill()

def CloseRootNtuple():
    print "===> closing ROOT files"
    rootTree.Print()
    rootTree.Write()
    rootFile.Close()
                    
