#!/usr/bin/env python

#
# initialise Gaudi
#
from InitGaudi import *


#
# define some basic cuts
#
JPsiMassMin  =   2.95
JPsiMassMax  =   3.25
JPsiChi2Max  =  15

#
# ROOT: book histograms, etc
#
hMass      = ROOT.TH1F('hMass'    , 'J/psi mass'         , 120,  JPsiMassMin ,  JPsiMassMax)
hMassAll   = ROOT.TH1F('hMassAll' , 'J/psi mass - no cut', 500,    0         ,  5.0)
hMuDLL     = ROOT.TH1F('hMuDLL'   , 'DLL pi-mu'          , 120,  -50.0       , 50.0)
hChi2      = ROOT.TH1F('hChi2'    , 'J/psi vertex chi2'  , 500,    0         , 30)

#
# the event loop
#
def eventLoop(nEvents=100,verbose=0):
    iEvent=0
    nFittedVertices=0
    while iEvent<nEvents:
        iEvent = iEvent + 1
        if verbose > 0:
            print "==> now process loop count %i" % iEvent
        appMgr.run(1)
        MuonContainer = evt['/Event/Phys/PreLoadMuons/Particles']
        if verbose > 1:
            print " container  at %s" % MuonContainer

        # check if still events left in the files    
        if not MuonContainer: 
            print "no further event found"
            break
        if verbose > 0:
            print " found # muons %i" % MuonContainer.size()
        if MuonContainer.size() > 0:
            MuPlusList  = []
            MuMinusList = []

            # loop over all kaons found
            for muon in MuonContainer:

                # some information about the kaons
                # e.g. result of RICH particle ID
                DLLPiMu = muon.proto().info(gaudimodule.gbl.LHCb.ProtoParticle.CombDLLmu,-999)
                hMuDLL.Fill(DLLPiMu)
                
                if verbose > 1:
                    print "process new muon"
                    print "----------------"
                    # some information from the particle
                    print "charge     %i  "  % muon.charge()
                    print "p          %lf "  % muon.p()
                    
                    # some information from the proto-particle this particle was created from
                    print "DLL mu %lf "  % DLLPiMu
                    
                    # some information from the track associated to the proto-particle
                    print "track chi2 %lf "  % muon.proto().track().chi2()

                # separate K+ and K- into two lists, do other cuts if wanted
                if muon.charge() > 0:
                    MuPlusList.append(muon)
                else:
                    MuMinusList.append(muon)

            # end of the for .. loop
            if verbose > 0:
                print "found #Mu+: %i, #Mu-: %i" % (len(MuPlusList), len(MuMinusList))


            # loop over Mu+, Mu- to get J/psi -> MuMu candidates
            for i in range(len(MuPlusList)):
                for j in range(len(MuMinusList)):

                    if verbose > 2:
                        print ""
                        print "index i= %i, j=%i" %(i,j)
                    
                    MuPlus  = MuPlusList[i]
                    MuMinus = MuMinusList[j]
                    
                    if verbose > 2:
                        print "PID Mu+ = %i, Mu- = %i" % (MuPlus.particleID().pid(),MuMinus.particleID().pid())

                    # create a J/psi -> Mu+ Mu- candidate
                    daughterVect = particleVector()  #from vector definition in InitGaudi.py
                    daughterVect.push_back(MuPlus)
                    daughterVect.push_back(MuMinus)
                    if verbose > 1:
                        print "number of daughters for vertex fit %i" % daughterVect.size()
                    
                    JPsiVertex = LHCb.Vertex()
                    pid        = LHCb.ParticleID(443)
                    JPsiCand   = LHCb.Particle(pid)

                    # fit vertex without creating a mother particle
                    #sc = OfflineVertexFitter.fit(MuPlus, MuMinus, JPsiVertex)

                    # N.B. This doesn't work because the interface looks the same
                    # to Python whereas it's different in C++ (const vs non-const arguments)
                    #sc = OfflineVertexFitter.fit(MuPlus, KMinus,JPsiCand, JPsiVertex)

                    # same as above but use other interface with vector of daughter paricles
                    #sc = OfflineVertexFitter.fit(daughterVect, JPsiVertex)

                    # use this interface to fit vertex and create Bs mother particle
                    sc = OfflineVertexFitter.fit(daughterVect, JPsiCand, JPsiVertex)

                    JPsiMass = JPsiCand.measuredMass()/1000;  #converstion to GeV/c**2
                    JPsiChi2 = JPsiVertex.chi2()

                    # fill histo
                    hChi2.Fill(JPsiChi2)
                    hMassAll.Fill(JPsiMass)

                    if verbose > 1:
                        print "some vertex properties"
                        print "   status code %s " % sc 
                        print "   mass        %lf" % JPsiMass
                        print "   chi2        %lf" % JPsiChi2
                        print ""

                    if JPsiMass > 0:
                        nFittedVertices = nFittedVertices+1

                    if JPsiMass >= JPsiMassMin and JPsiMass < JPsiMassMax and JPsiChi2 < JPsiChi2Max:                
                        hMass.Fill(JPsiMass)



            #summary
            if verbose > 0:
                print "---> end of event  <----"
                print "# JPsi cand %i" % nFittedVertices


