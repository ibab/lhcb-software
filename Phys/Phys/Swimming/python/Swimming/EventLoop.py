from Configurables import Swimming
from os import environ
from pprint import pprint
from math import *
from collections import defaultdict

# I'm being lazy here
swimStripping           = Swimming().getProp('SwimStripping')
nEvents                 = Swimming().getProp('EvtMax')
offCands                = Swimming().getProp('OffCands')
swimCands               = Swimming().getProp('SwimmingPrefix')
offlinePVs              = Swimming().getProp('OfflinePV')
DEBUGMODE               = Swimming().getProp('Debug')
onlyEvents              = Swimming().getProp('OnlyEvents')
startingEvent           = Swimming().getProp('SkipEvents')
skipIfNoMuDSTCand       = Swimming().getProp('SkipEventIfNoMuDSTCandFound')
skipIfNoMuDSTCandsAnywhere = Swimming().getProp('SkipEventIfNoMuDSTCandsAnywhere')
muDSTCands              = Swimming().getProp('MuDSTCands')
overrideStageName       = Swimming().getProp('OverrideStageName')

# Shorter names for some of the options
# The maximum swimming distance is maxSwimDistance (in mm)
# The inital granularity is in mm
# The number of passes is how many times we try to revise the
# precision with which the acceptance function is known  
# maxSwimDistance should be an integer multiple of the
# initialGranularity  
maxSwimDistance         = Swimming().getProp('MaxSwimDistance')
initialGranularity      = Swimming().getProp('InitialGranularity')
# By how much to divide the granularity each time
# and how many times to refine
granularityRefinement   = Swimming().getProp('GranularityRefinement')
numberOfRefinements     = Swimming().getProp('NumberOfRefinements')
# Add some swimming in "outer" regions of propertime to study potential
# upper lifetime acceptance effects
# Here the granularity is deliberately coarser
# Again, extraSwimDistance should be an integer
# multiple of the extraGranularity 
extraSwimDistance       = Swimming().getProp('ExtraSwimDistance')
extraGranularity        = Swimming().getProp('ExtraGranularity')
# Swim the stripping or the trigger?
# default is the trigger, switch this to true
# if you want to swim the stripping (and/or offline)
swimOffsel              = Swimming().getProp('SwimOffSel')

writerName = None
if not swimStripping:
    writerName = 'Writer'
elif Swimming().getProp('OutputType') == 'MDST':
    writerName = 'MicroDSTMainSeq'
elif Swimming().getProp('OutputType') == 'DST':
    writerName = 'FullDSTMainSeq'
def SwimmingEventLoop(gaudi, nEvents):
    import GaudiPython
    #import PartProp.PartPropAlg
    import PartProp.decorators
    import PartProp.Service
    #from GaudiPython.Bindings import AppMgr
    #from PartProp.Nodes import *

    StatusCode = GaudiPython.gbl.StatusCode
    if gaudi.targetFSMState() < 2:
        gaudi.initialize()
    TES = gaudi.evtsvc()
    SEL = gaudi.evtSel()
    SEL.OutputLevel = 6
    SEL.firstEvent = startingEvent

    #Now after the initialize some more job dependent stuff
    if swimStripping :
        dod = gaudi.service('DataOnDemandSvc')
        svc = gaudi.service('Swimming::Service')
        outputs = gaudi.algorithm('killStripping').Nodes
        for l in dod.AlgMap.keys():
            if l.find('Phys') == -1:
                continue
            l = '/'.join(l.split('/')[:-1])
            outputs.append(l)
        gaudi.algorithm('killStripping').Nodes = outputs
        gaudi.algorithm('killStrippingSeq').Enable  = False
        gaudi.algorithm('StrippingGlobal').Enable   = False
        gaudi.algorithm(writerName).Enable   = False
        triggers = Swimming().getProp('StripCands')
    #Tools needed for for tistosing
    else :
        for algo in ['killHltSeq', 'seqL0', 'Hlt',
                     'HltSelReportsWriter', writerName,
                     'HltDecReportsWriter', 'HltVertexReportsWriter',
                     'HltLumiWriter', 'HltLumiStripper',
                     'HltRoutingBitsWriter']:
            gaudi.algorithm(algo).Enable = False
        hlt1triggers = Swimming().getProp('Hlt1Triggers')
        hlt2triggers = Swimming().getProp('Hlt2Triggers')
        triggers     = [hlt1triggers,hlt2triggers]
        # Disable the FSR writer if FSR writing is enabled.
        if Swimming().getProp('WriteFSR'):
            gaudi.algorithm('FSRInputCopyStreamWriter').Enable = False

    #Make the class of globals for passing information around
    from Swimming.Globals import GlobalParams
    myGlobs = GlobalParams(gaudi, triggers)
    gaudi.__dict__['globs'] = myGlobs

    def __next__():
        # Emit special end event Incident.
        myGlobs.incidentSvc.fireIncident(GaudiPython.gbl.Incident("SwimmingLoop", "SwimmingEndEvent"))

    # Import the utilities
    from Swimming.SwimmingUtils import (getCandidateInfo, runSwimmingStep,
                                        createObject, getSelector, movePVs)
    from Swimming.RefineTPs import refine
    from Swimming.SwimmingTisTos import evaluateTisTos 

    # The turning points contain the following information
    #
    # The raw position of the turning point in mm (relative to the
    # point at which the B/D was measured), the lifetime of the B/D
    # at the turning point, the IP of the B/D at the turning point,
    # and of course the trigger decision at that turning point.
    #
    # In addition we store information about individual tracks in the
    # signal, in order to deal with inclusive triggers which only require
    # a part of the signal to fire.
    #
    # The information on which triggers were TOS at which stage
    # If a logical OR of the triggers is being requested, then 
    # a new turning point is set every time that either one of the trigger
    # decisions changes. 
    #
    # The information on which tracks were TOS in
    # HLT1. To save space this is stored as an integer,
    # whose first and last digits are 9 and the digits
    # inbetween are 1 or 0 depending on whether the
    # given track was TOS in HLT1 or not. This is relevant
    # for swimming the 1Track trigger only.     
    #
    # The information on which tracks played a part in the
    # HLT2 decision. Important for swimming inclusive HLT2
    # triggers or swimming multiple triggers in the same job.
    #
    # Which tracks were Reco'd in HLT1 and HLT2 
    # again this is given in the format specified above
    #
    # Which tracks were Rec'ible in the VELO, offline (3 hits) or 
    # in HLT1 (5 hits)
    #
    # Get the candidate selector.
    if Swimming().getProp('SelectMethod') != 'all':
        selector = getSelector(Swimming().getProp('SelectMethod'))(myGlobs)

    # Initialize at -1 so the eventNumber can be incremented at the top of the loop.
    eventNumber = -1

    from GaudiPython.Bindings import gbl
    SwimmingReport  = gbl.LHCb.SwimmingReport
    SwimmingReports = gbl.KeyedContainer('LHCb::SwimmingReport', 'Containers::KeyedObjectManager<Containers::hashmap>')
    P2TPRelation    = gbl.LHCb.Relation2D('LHCb::Particle', 'LHCb::SwimmingReport')
    SharedParticles = gbl.SharedObjectsContainer('LHCb::Particle')

    while True:
        # Increment event number
        eventNumber += 1

        # Check if we've hit the maximum
        if (nEvents != -1 and eventNumber >= nEvents):
            print "Finished processing events"
            break
        elif (eventNumber % 100) == 0: 
            print "Processing event", eventNumber

        gaudi.run(1)
        if not TES["/Event/Rec/Header"] :
                print "Reached end of file, we're done!"
                break

        odin = TES['/Event/DAQ/ODIN']
        if DEBUGMODE :
            print "Processing %d %d" % (odin.runNumber(), odin.eventNumber())

        if onlyEvents and not (odin.runNumber(), odin.eventNumber()) in onlyEvents:
            __next__()
            continue

        if not swimStripping and not Swimming().getProp('Simulation') and \
               odin.triggerConfigurationKey() != int(Swimming().getProp('TCK'), 16):
            print "WARNING, TCK 0x%08x does not match configured TCK %s, skipping event." \
                  % (odin.triggerConfigurationKey(), Swimming().getProp('TCK'))
            __next__()
            continue
        
        # Safety checks
        if skipIfNoMuDSTCand :
            skipEventBools = [ ]
            for candloc in muDSTCands :
                skipEvent = False
                mdstcands = TES[candloc + "/Particles"]
                if not mdstcands :
                    if DEBUGMODE:
                        print "Incident while processing event number",startingEvent+eventNumber
                        print "No container of muDST candidates "+candloc+" found!" 
                    skipEvent = True
                elif mdstcands.size() == 0 :
                    if DEBUGMODE:
                        print "Incident while processing event number",startingEvent+eventNumber
                        print "Empty container of muDST candidates "+candloc+" found!" 
                    skipEvent = True
                skipEventBools += [ skipEvent ]
            # skipIfNoMuDSTCandsAnywhere is a new flag to decide whether skipIfNoMuDSTCand means *all* MuDSTCand locations have to
            # be populated or just *some*
            if (skipIfNoMuDSTCandsAnywhere and not any(skipEventBools)) or (not skipIfNoMuDSTCandsAnywhere and all(skipEventBools)):
                if DEBUGMODE:  
                    print "Skipping the event"
                __next__()
                continue

        # Create a list of unique candidates
        from Swimming.SwimmingUtils import Candidate
        candidates = defaultdict(list)
        for loc in offCands.keys():
            particles = TES[loc + '/Particles']
            if not particles:
                if DEBUGMODE:
                    print "No particles found at %s" % loc
                continue
            elif DEBUGMODE:
                print "Found %d particles at %s" % (particles.size(), loc)

            for particle in particles:
                candidates[Candidate(myGlobs, particle)].append((loc, particle))
        candidates = [Candidate(myGlobs, c.particle(), dict(selections)) for \
                      c, selections in candidates.iteritems()]
        if DEBUGMODE:
            print "Found %s unique candidates:" % len(candidates)
            for c in candidates:
                print c

        #if len(candidates) < 2:
        #    print "debug: skipping"
        #    __next__()
        #    continue

        turningPoints = {}

        if len(candidates) == 0 :
            if DEBUGMODE:
                print "Problem while processing event number",startingEvent+eventNumber
                print "Container of offline candidates empty!"
                print "Skipping the event but this is bad, check what you are doing!!"
            __next__()
            continue

        if DEBUGMODE:
            print "Passed the safety checks OK"
            print "Candidates in event =", len(candidates)

        # Fire BeginSwimming to signal the swimming service to clear its internal PV container
        myGlobs.incidentSvc.fireIncident(GaudiPython.gbl.Incident("SwimmingLoop", "BeginSwimming"))

        # Select the candidate to swim
        if Swimming().getProp('SelectMethod') != 'all':
            candidates = [selector(candidates)]
        
        for i, candidate in enumerate(candidates):
            # Containers to store the turningpoints
            roughTurningPoints = []
            finalTurningPointsForWriting = []

            if not candidate :
                print "Somehow there is no candidate!!! This should never happen."
                print "Skipping this event."
                __next__()
                continue

            if DEBUGMODE and not swimStripping:
                # Use the debugging TisTos tool to dump info on what the TisTos is
                # wrt all the trigger lines before the swimming starts.
                extraTisTos = gaudi.toolSvc().create('TriggerTisTos/SwimmingDebugTisTos',
                                                     interface = 'ITriggerTisTos')
                lines = []
                for level in triggers: lines.extend(level)
                for trigger in lines:
                    extraTisTos.setOfflineInput()
                    extraTisTos.addToOfflineInput(candidate.particle())
                    extraTisTos.setTriggerInput()
                    extraTisTos.addToTriggerInput(trigger)
                    print extraTisTos.analysisReportTrigger(trigger)

            # If we are swimming the trigger and have a non-trivial selection method,
            # put the selected candidate in a separate location.
            if not swimStripping and Swimming().getProp('SelectMethod') != 'none':
                swimmingParticles = createObject(SharedParticles)
                swimmingParticles.insert(candidate.particle())
                TES.unregisterObject(swimCands + '/Particles')
                TES.registerObject(swimCands + '/Particles', swimmingParticles)

            HltDecLast  = None
            HltDec      = None
            swimLoop    = int(-1. * maxSwimDistance + -1. * extraSwimDistance)
            while (swimLoop <= (maxSwimDistance + extraSwimDistance)) :
                if DEBUGMODE:
                    print "Running over event", startingEvent + eventNumber, "swimming step", swimLoop \
                          , "candidate", i
                # We got this far now get the candidate and, for the "zero" step only
                # fill the corresponding event variables for it 
                # Note that this function call also executes the stripping algorithm
                # having moved the primary vertices around first
                runSwimmingStep(myGlobs, candidate.particle(), swimLoop)

                # Now get the trigger decision for this swimming step
                HltDec = evaluateTisTos(myGlobs, candidate.particle(), candidate.selectedParticles().keys(), swimLoop)
                if DEBUGMODE :
                    print "Retrieved HLT decision", HltDec, "OK for this event" 

                # If this is the very first swimming step, set HltDecLast to the global
                # HltDec at this point, otherwise get it from the previous step.
                if swimLoop == int(-1.*maxSwimDistance + -1.*extraSwimDistance):
                    HltDecLast = HltDec
                elif roughTurningPoints != []:
                    # before we had [raw, tau, ip, dec, declast ]
                    # now we have [raw, {info}, dec, declast ]
                    # hence 3->2 for index
                    HltDecLast = roughTurningPoints[-1][2]

                # The first and last point are a special case for the bookkeeping
                # as we want to record the limits of our swimming and the trigger
                # decisions at these limits 
                if (abs(swimLoop) == int(1. * maxSwimDistance + 1. * extraSwimDistance)) or \
                   (HltDec != HltDecLast): 
                    # Get the lifetime and IP of the B/D with respect to its favourite PV
                    info = getCandidateInfo(myGlobs, candidate.particle())
                    roughTurningPoints.append([swimLoop, info, HltDec, HltDecLast])

                # Now the granularity varies with the position 
                # We swim more finely close to the actual decay point
                if ((swimLoop >= -int(maxSwimDistance)) and (swimLoop < int(maxSwimDistance))) :
                    swimLoop += int(initialGranularity)
                else :
                    swimLoop += int(extraGranularity) 
            # Now the refinement step
            # We have to refine every turning point, but the first and last
            # ones which we save don't count as they are just there to
            # demarcate the limits of our search, so we don't use them
            if roughTurningPoints == [] :
                # no __next__() because this is continuing from the loop over candidates
                continue
            if DEBUGMODE :
                print "****************************"
                print "The rough turning points are"
                pprint(roughTurningPoints)
                print "****************************"
            for thisturn in roughTurningPoints:
                # Write the first and last as is, no refinement
                if (thisturn == roughTurningPoints[0]) or (thisturn == roughTurningPoints[-1]) :
                    finalTurningPointsForWriting.append(thisturn)
                    continue
                if DEBUGMODE :
                    print "****************************"
                    print "About to refine TP"
                    pprint(thisturn)
                    print "****************************"
                if ((thisturn[0] > -int(maxSwimDistance)) and (thisturn[0] <= int(maxSwimDistance))):
                    finalTurningPointsForWriting += refine(myGlobs, candidate.particle(), 
                                                           candidate.selectedParticles().keys(), thisturn,
                                                           initialGranularity, numberOfRefinements)
                else: 
                    finalTurningPointsForWriting += refine(myGlobs, candidate.particle(),
                                                           candidate.selectedParticles().keys(), thisturn,
                                                           extraGranularity, numberOfRefinements)
            if DEBUGMODE :
                print "****************************"
                print "The final turning points for writing are:" 
                pprint(finalTurningPointsForWriting)
                print "****************************" 
            turningPoints[candidate] = finalTurningPointsForWriting
            # Reset the PV positions so we don't end up with junk on the DST
            movePVs(myGlobs, candidate.particle(), 0.0)
        # End of loop over candidates

        # Which stage are we swimming
        stage = None
        if len(overrideStageName):
          stage = overrideStageName
        elif swimStripping:
          stage = 'Stripping'
        else:
          stage = 'Trigger'

        # Get or Create the KeyedContainer for the SwimmingReports
        reportLocation = Swimming().getProp('SwimmingPrefix') + '/Reports'
        reports = TES[reportLocation]
        hadreports = True
        if not reports:
            reports = createObject(SwimmingReports)
            hadreports = False
            if DEBUGMODE:
                print "Created new SwimmingReports object, storing it at " + reportLocation
            # Put the container in the TES
            sc = TES.registerObject(reportLocation, reports)
            if not sc.isSuccess():
                print "Failed to register SwimmingReports in TES"
                return StatusCode(False)

        if DEBUGMODE:
            print turningPoints
        for candidate, tps in turningPoints.iteritems():
            # Look if there is already a report present for one of the locations
            # of a candidate.
            report = reports(candidate.intHash())
            
            if not report:
                report = createObject(SwimmingReport)
                if DEBUGMODE:
                    print "Created new SwimmingReport object, inserting it into the SwimmingReports"
                reports.insert(report, candidate.intHash())
            elif report.hasStage(stage):
                # I think this would be the outcome if the intHash method had a collision.
                print "WARNING, stage %s already present, this is unsupported and will fail!"
                return StatusCode(False)

            if DEBUGMODE:
                print candidate.selectedParticles()

            # Now that we have a report, relate the particles
            for loc, particle in candidate.selectedParticles().iteritems():
                p2tploc = offCands[loc] + '/P2TPRelations'
                relations = TES[p2tploc]
                if not relations:
                    relations = createObject(P2TPRelation)
                    if DEBUGMODE:
                        print "Created new P2TPRelation object, storing it at", p2tploc
                    # Put it in the TES
                    if not TES.registerObject(p2tploc, relations).isSuccess():
                        print "Failed to register relations table in TES at", p2tploc
                        return StatusCode(False)

                rel = relations.relations(particle)
                if rel.empty():
                    relations.relate(particle, report)
                elif not hadreports:
                    print "Found", rel.size(), "relations already for", particle,"from",loc
                    print "WARNING: already have relations for one of our offline candidates, this should only happen for FSPs"
                    #return StatusCode(False)

                # get the list of final state particles
                fsps = [ ]
                from Swimming.SwimmingTisTos import appendToFSP
                appendToFSP(0, particle, fsps)

                for fsp in fsps:
                  #rel = relations.relations(fsp['child'])
                  # a track can be related to more than one report if more than one candidate is swum
                  # this should do nothing if we've already related, which will happen for each swimming
                  # stage after the first (i.e. stripping, or 2nd trigger swimming)
                  relations.relate(fsp['child'], report)
                  #    print "WARNING: Attempted to relate the same FSP to the same report more than once..."
                  #    return StatusCode(False)

            # TODO MAKE THIS MAKE SENSE WITH MULTIPLE CANDIDATE SWIMMING
            # I guess this still makes sense, sort of. To use the information
            # you would have to try and relate a track with all of the different
            # P2TPRelations, and deal with the fact you may find it was part of 
            # more than one B/D candidate which was swum.
            #from Swimming.SwimmingTisTos import appendToFSP
            #rel = relations.relations(mycand)
            #particles = [{"child" : mycand, "parent" : 0}]
            #appendToFSP(0, mycand, particles)
            #for particle in particles:
            #    if rel.empty():
            #        relations.relate(particle["child"], report)
            #    elif rel(0).to().key() != report.key():
            #        print "There is a relation, but it points to another swimming report, this is very bad!!"
            #        return StatusCode(False)
            # END TODO

            # symbols for std::map; create them here the first time we pass by. They
            # used to be in Swimming.decorators, but something goes pear-shaped with
            # the dictionary loading in that case...
            if 'std_map_sb' not in locals():
                from GaudiPython import gbl
                std_map_sinfo = gbl.std.map('string', 'map<unsigned long,bool>')
                std_map_ulb = gbl.std.map('unsigned long', 'bool')
                pair_ulb = gbl.std.pair('const unsigned long', 'bool')
                std_map_ulb.__pair = pair_ulb
                std_map_sb = gbl.std.map('std::string', 'bool')
                pair_sb = gbl.std.pair('const std::string', 'bool')
                std_map_sb.__pair = pair_sb
                #std_map_sd = gbl.std.map('std::string', 'double')
                #pair_sd = gbl.std.pair('std::string', 'double') # Not const according to comment in v2r4b python/Swimming/decorators.py
                #std_map_sd.__pair = pair_sd

                pair_sd = gbl.std.pair('std::string', 'double')
                std_vec_sd = gbl.std.vector('std::pair<std::string,double>')

                def set_map(self, k, v):
                    p = self.__pair(k, v)
                    it = self.find(k)
                    if it != self.end():
                        self.erase(it)
                    self.insert(p)
    
                std_map_ulb.__setitem__ = set_map
                std_map_sb.__setitem__ = set_map
                #std_map_sd.__setitem__ = set_map

                GaudiPython.loaddict("SwimmingHacksDict")
                createTurningPoint = gbl.SwimmingHacks.createTurningPoint

            # Create the TurningPoint objects and put them in the report
            for turn in tps:
                d = std_map_sb()
                for decision, value in turn[2][1].iteritems():
                    d[decision] = value
                m = std_map_sinfo()
                for decision, info in turn[2][2].iteritems():
                    i = std_map_ulb()
                    for k, v in info.iteritems():
                        i[k] = v
                    m[decision] = i
                tau = turn[1].pop('tau')
                mip = turn[1].pop('mip') # TODO make sure this is actually consistet
                # and we call ipchi2 ipchi2...
                e = std_vec_sd()
                for info, value in turn[1].iteritems():
                    e.push_back(pair_sd(info, float(value)))
                #tp = LHCb.TurningPoint(turn[0], tau, mip, turn[2][0], d, m, e)
                tp = createTurningPoint(turn[0], tau, mip, turn[2][0], d, m, e)
                report.addTurningPoint(stage, tp)

        # Write the Output
        myGlobs.gaudi.algorithm(writerName).execute()
        if not swimStripping and Swimming().getProp('WriteFSR'):
            gaudi.algorithm('FSRInputCopyStreamWriter').execute()
        __next__()

    # re-enable the output algorithms to allow correct finalisation
    gaudi.algorithm(writerName).Enable = True
    if not swimStripping and Swimming().getProp('WriteFSR'):
        gaudi.algorithm('FSRInputCopyStreamWriter').Enable = True

    # Print the number of events processed
    print Swimming().getProp('EventPrint') % eventNumber
    return StatusCode(True)
