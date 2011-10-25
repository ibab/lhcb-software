#The TISTOS function of the swimming. For TISTOSing the trigger
#we use the regular TISTOS tools, the stripping is done "by hand"
from collections import defaultdict
from GaudiPython.Bindings import gbl
hashParticle = gbl.Swimming.hashParticle

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

def evaluateTisTos(myGlobs,mycand):
    #Evaluate whether or not you TOS-ed on your triggers
    decisions = {}
    if not myGlobs.swimStripping :
        myGlobs.tistostool.setOfflineInput()
        myGlobs.tistostool.addToOfflineInput(mycand)
        for level in myGlobs.triggers :
            passedlevel = False
            for trigger in level :
                if isTos(myGlobs,trigger) :
                    decisions[trigger] = True
                    passedlevel = True
                else :
                    decisions[trigger] = False
            if not passedlevel:
                return myGlobs.HltDecFalse

        # So it passed the trigger, now we need to evaluate
        # which tracks caused HLT1 to fire, and which were
        # reco'd in HLT1 and HLT2
        finalstateparts_mycand = []
        appendToFSP(mycand,finalstateparts_mycand)
        daughterInfo = defaultdict(dict)
        for fsp in finalstateparts_mycand :
            myGlobs.tistostool.setOfflineInput()
            myGlobs.tistostool.setOfflineInput(fsp)
            h = hashParticle(fsp)
            # First how many fired HLT1 
            for trigger in myGlobs.triggers[0]:
                daughterInfo['trTOS_'+trigger][h] = isTos(myGlobs, trigger) or isTob(myGlobs, trigger)
            # Did this track play a part in the HLT2 yes?
            for trigger in myGlobs.triggers[1]:
                daughterInfo['trTOS_'+trigger][h] = isTob(myGlobs, trigger)

            # Now how many were recod in HLT1
            daughterInfo['trRec_HLT1'][h] = isTos(myGlobs,myGlobs.hlt1recoline)
            # Now how many were recod in HLT2
            daughterInfo['trRec_HLT2'][h] = isTos(myGlobs,myGlobs.hlt2recoline)
            # Now the VELO acceptance
            from GetVeloAcceptance import trackInVELOAcceptance
            iv3 = trackInVELOAcceptance(mycand, fsp.proto().track(), 3)
            iv5 = trackInVELOAcceptance(mycand, fsp.proto().track(), 5)
            daughterInfo['trRec_VELO_OFF'][h]   = iv3
            daughterInfo['trRec_VELO_HLT1'][h]  = iv5
                    
        return (True, decisions, dict(daughterInfo))
    else :
        if myGlobs.DEBUGMODE :
            print "Evaluating TISTOS for stripping"
            print "Will look for candidates at",myGlobs.triggers 
        if not myGlobs.TES[myGlobs.triggers+"/Particles"] :
            if myGlobs.DEBUGMODE :
                print "Found no candidates, returning false" 
            return myGlobs.StrDecFalse
        else :
            if myGlobs.DEBUGMODE :
                print "Found", myGlobs.TES[myGlobs.triggers + "/Particles"].size(), "candidates"
                print "About to match them"
            if myGlobs.TES[myGlobs.triggers + "/Particles"].size() == 0:
                return myGlobs.StrDecFalse
            else :
                for cand in myGlobs.TES[myGlobs.triggers + "/Particles"] :
                    if myGlobs.DEBUGMODE :
                        print "########################"
                        "About to match these two canidates"
                        print cand,mycand
                        print "########################"
                    if matchCands(myGlobs,mycand,cand):
                        # If we're not swimming the offline selection, we're done
                        if not myGlobs.swimOffline :
                            return (True, {myGlobs.triggers : True}, {})
                        # Import offline selection
                        OffSelFunc = __import__('Swimming.' + myGlobs.offSelName).OffSelFunc
                        if OffSelFunc(myGlobs,mycand):
                            return (True, {myGlobs.triggers : True}, {})
        return myGlobs.StrDecFalse
#
#
#
#Two functions to match two candidates
#together, one for particles the other one for tracks
def matchLists(list1,list2) :
    num = list1.size()
    nummatch = 0
    for id in list1 :
        for ID in list2 :
            if (id.lhcbID() == ID.lhcbID()) :
                nummatch += 1
                break
    return nummatch/num
#
#
#
def appendToFSP(daughter,finalstateparticles) :
    if daughter.isBasicParticle() : finalstateparticles += [daughter]
    else :
        for gd in daughter.daughtersVector() :
            appendToFSP(gd, finalstateparticles)
#
#
#
def matchCands(myGlobs,cand1,cand2) :
    finalstateparticles_cand1 = []
    finalstateparticles_cand2 = []
    appendToFSP(cand1,finalstateparticles_cand1)
    appendToFSP(cand2,finalstateparticles_cand2)
    num_cand1 = finalstateparticles_cand1.__len__()
    num_cand2 = finalstateparticles_cand2.__len__()
    nummatch = 0
    if myGlobs.DEBUGMODE :
        print "Number of daughters of each candidate"
        print num_cand1,num_cand2
    if num_cand1 <> num_cand2 :
        print "Trying to match unequal candidates!"
        print "You have probably supplied the wrong stripping or offline candidate location!"
        print "The swimming will not work, returning False"
        return False
    else :
        try :
            for kid1 in finalstateparticles_cand1 :
                for kid2 in finalstateparticles_cand2 :
                    if matchLists(kid1.proto().track().lhcbIDs(),kid2.proto().track().lhcbIDs()) :
                        nummatch += 1
        except :
            return False
    if myGlobs.DEBUGMODE:
        print (nummatch*1.)/(num_cand1*1.)
    return (nummatch*1.)/(num_cand1*1.)
