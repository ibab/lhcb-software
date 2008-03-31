
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
        eventHeader = evt['/Event/Rec/Header']
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
        MuonContainer = evt['/Event/Phys/StdLooseMuons/Particles']
        nMuons = MuonContainer.size()
        if verbose > 0:
            print "Found %i muons in this event" % nMuons

        # break out of loop if container not found
        # (N.B. containers are required to exist by LHCb convention,
        #       i.e. if the container is not there we have reached the
        #       end of the file) 
        if not MuonContainer:
            print "No further event found"
            break

        if nMuons > 0:
            MuPlusList  = []
            MuMinusList = []
            #loop over muons and separate into pos. / neg. charged ones
            for muon in MuonContainer:
                if muon.charge() > 0:
                    MuPlusList.append(muon)
                else:
                    MuMinusList.append(muon)

            if verbose > 0:        
                print "found #Mu+ %i, #Mu- %i" %(len(MuPlusList), len(MuMinusList))        

            #now loop over all Mu+, Mu- combinations and create a J/psi -> MuMu candidate
            for i in range(len(MuPlusList)):
                for j in range(len(MuMinusList)):
                    MuPlus  = MuPlusList[i]
                    MuMinus = MuMinusList[j]

                    #some massaging to get the objects right for the vertex fitter

                    #from the definition in InitGaudi.py -> std::vector of type LHCb::Particle*
                    daughterVect = particleConstVector()
                    daughterVect.push_back(MuPlus)
                    daughterVect.push_back(MuMinus)

                    JPsiVertex = LHCb.Vertex()
                    pid        = LHCb.ParticleID(443)
                    JPsiCand   = LHCb.Particle(pid)

                    sc = OfflineVertexFitter.fit(daughterVect, JPsiCand, JPsiVertex)

                    mass = JPsiCand.measuredMass()/1000
                    chi2 = JPsiVertex.chi2()

                    if verbose > 0:
                        print "Fitted m= %lf with chi2= %lf" %(mass, chi2)
                    hMass.Fill(mass)

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
                    
