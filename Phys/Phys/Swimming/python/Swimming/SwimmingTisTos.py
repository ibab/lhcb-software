#The TISTOS function of the swimming. For TISTOSing the trigger
#we use the regular TISTOS tools, the stripping is done "by hand"
from collections import defaultdict
from GaudiPython.Bindings import gbl
from SwimmingUtils import hashParticle, matchParticles, matchParticlesNoCompositePID

__all__ = ["evaluateTisTos","appendToFSP"]

def isTos(myGlobs,trigger) :
    if (myGlobs.tistostool.hltObjectSummaries(  trigger,
                                                myGlobs.tistosbools[0],
                                                myGlobs.tistosbools[1]
                                             ).size() != 0) :
        return True
    return False 

def isTob(myGlobs,trigger) :
    if (myGlobs.tistostool.hltObjectSummaries(  trigger,
                                                myGlobs.tistosbools[2],
                                                myGlobs.tistosbools[2]
                                             ).size() != 0) :
        return True
    return False 

def evaluateTisTos(myGlobs, mycand, locations, swimPoint):
    # This is the setup for return values,
    # - The first value is a global Pass/Fail
    # - The second a {name : boolean} where the name is a trigger
    # decision or a stripping candidate location
    # - The third is only filled for when swimming the trigger with a {name : {FSP.key() : boolean}}

    #Evaluate whether or not you TOS-ed on your triggers
    decisions = {}
    if not myGlobs.swimStripping :
        myGlobs.tistostool.setOfflineInput()
        myGlobs.tistostool.addToOfflineInput(mycand)
        globalPass = True
        for level in myGlobs.triggers :
            passedlevel = False
            for trigger in level :
                if isTos(myGlobs,trigger) :
                    decisions[trigger] = True
                    passedlevel = True
                else :
                    decisions[trigger] = False
            if not passedlevel:
                globalPass = False

        # So it passed the trigger, now we need to evaluate
        # which tracks caused HLT1 to fire, and which were
        # reco'd in HLT1 and HLT2
        finalstateparts_mycand = []
        appendToFSP(0,mycand,finalstateparts_mycand)
        daughterInfo = defaultdict(dict)
        for fsp in finalstateparts_mycand :
            if globalPass:
                myGlobs.tistostool.setOfflineInput()
                myGlobs.tistostool.setOfflineInput(fsp["child"])
            h = hashParticle(fsp["child"])
            # First how many fired HLT1 
            for trigger in myGlobs.triggers[0]:
                daughterInfo['trTOS_'+trigger][h] = globalPass and (isTos(myGlobs, trigger) or isTob(myGlobs, trigger))
            # Did this track play a part in the HLT2 yes?
            for trigger in myGlobs.triggers[1]:
                daughterInfo['trTOS_'+trigger][h] = globalPass and isTob(myGlobs, trigger)

            # Now how many were recod in HLT1
            daughterInfo['trRec_HLT1'][h] = globalPass and isTos(myGlobs,myGlobs.hlt1recoline)
            # Now how many were recod in HLT2
            daughterInfo['trRec_HLT2'][h] = globalPass and isTos(myGlobs,myGlobs.hlt2recoline)
            # Now the VELO acceptance
            from GetVeloAcceptance import trackInVELOAcceptance
            iv3 = trackInVELOAcceptance(swimPoint, mycand, fsp["parent"], fsp["child"].proto().track(), 3)
            iv5 = trackInVELOAcceptance(swimPoint, mycand, fsp["parent"], fsp["child"].proto().track(), 5)
            daughterInfo['trRec_VELO_OFF'][h]   = globalPass and iv3
            daughterInfo['trRec_VELO_HLT1'][h]  = globalPass and iv5
                    
        return (globalPass, decisions, dict(daughterInfo))
    else :
        decisions = {}
        for offlinelocation in locations:
        #for striplocation, offlinelocation in myGlobs.triggers.iteritems():
            # Find where we should look for stripping candidates given the offline location
            # 'mycand' was found at.
            striplocation = myGlobs.triggers[offlinelocation]
            if myGlobs.DEBUGMODE :
                print "Evaluating TISTOS for stripping"
                print "Will look for candidates at", striplocation
            candidates = myGlobs.TES[striplocation + "/Particles"]
            if not candidates:
                if myGlobs.DEBUGMODE :
                    print "Found no candidate container at %s" % striplocation
                    #print myGlobs.TES.dump()
                decisions[offlinelocation] = False
            elif candidates.size() == 0:
                if myGlobs.DEBUGMODE :
                    print "0 candidates found at %s" % striplocation
                decisions[offlinelocation] = False
            else:
                # Make a list to ensure acces by index does what we want
                #candidates = [c for c in candidates ] 
                if myGlobs.DEBUGMODE :
                    print "Found", candidates.size(), "candidates"
                    print "About to match them"
                for stripCand in candidates:
                    if myGlobs.DEBUGMODE :
                        print "########################"
                        "About to match these two candidates"
                        print stripCand, mycand
                        print "########################"
                    if myGlobs.matchCandsUsingPID:
                      match = matchParticles(mycand, stripCand)
                    else:
                      match = matchParticlesNoCompositePID(mycand, stripCand)
                    if not myGlobs.swimOffline:
                        decisions[offlinelocation] = match
                    else:
                        # Import offline selection
                        OffSelFunc = __import__(myGlobs.offSelName,
                                                fromlist = ['OffSelFunc']).OffSelFunc
                        decisions[offlinelocation] = match and OffSelFunc(myGlobs, mycand)
        globalPass = False
        for d in decisions.itervalues():
            globalPass = globalPass or d
        return (globalPass, decisions, {})

def appendToFSP(parent, daughter,finalstateparticles) :
    if parent and daughter.isBasicParticle() : 
        finalstateparticles += [{"child" :daughter, "parent" : parent}]
    else :
        for gd in daughter.daughtersVector() :
            appendToFSP(daughter, gd, finalstateparticles)
