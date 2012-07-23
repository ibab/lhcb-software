from Configurables import Swimming
from os import environ
from pprint import pprint
from math import *

# I'm being lazy here
swimStripping           = Swimming().getProp('SwimStripping')
nEvents                 = Swimming().getProp('EvtMax')
offCands                = Swimming().getProp('OffCands')
swimCands               = Swimming().getProp('SwimmingPrefix')
offlinePVs              = Swimming().getProp('OfflinePV')
DEBUGMODE               = Swimming().getProp('Debug')
startingEvent           = Swimming().getProp('SkipEvents')
skipIfNoMuDSTCand       = Swimming().getProp('SkipEventIfNoMuDSTCandFound')
muDSTCands              = Swimming().getProp('MuDSTCands')

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
    StatusCode = GaudiPython.gbl.StatusCode
    from Swimming.decorators import (LHCb,
                                     SwimmingReports,
                                     P2TPRelation,
                                     SharedParticles,
                                     std_map_ulb,
                                     std_map_sb,
                                     std_map_sinfo)
    gaudi = GaudiPython.AppMgr(outputlevel = 3)
    gaudi.initialize()
    TES = gaudi.evtsvc()
    SEL = gaudi.evtSel()
    SEL.OutputLevel = 6
    SEL.firstEvent = startingEvent

    #Now after the initialize some more job dependent stuff
    if swimStripping :
        dod = gaudi.service('DataOnDemandSvc')
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

    #The tis tos tool
    tistostool      = gaudi.toolsvc().create('TriggerTisTos'    ,interface='ITriggerTisTos')
    tistosbools     = [tistostool.kAnything,tistostool.kTrueRequired,tistostool.kFalseRequired]
    #The tools used for swimming the stripping/offline
    m_lifetimeTool  = gaudi.toolsvc().create(Swimming().getProp('TauCalc'), interface = 'ILifetimeFitter')
    m_distanceTool  = gaudi.toolsvc().create(Swimming().getProp('DistCalc'), interface = 'IDistanceCalculator')
    m_relatedPVTool = gaudi.toolsvc().create(Swimming().getProp('RelPVFinder'), interface = 'IRelatedPVFinder')

    #Make the class of globals for passing information around

    from GaudiPython import InterfaceCast
    tmpSvc = gaudi.service("IncidentSvc")
    incidentSvc = InterfaceCast("IIncidentSvc")(tmpSvc.getInterface())

    from Swimming.Globals import globalParams
    myGlobs = globalParams( GaudiPython, gaudi, SEL, TES, incidentSvc,
                            m_lifetimeTool, m_distanceTool, m_relatedPVTool,
                            tistostool, tistosbools, triggers, 
                            Swimming().getProp('Hlt1RecoLine'), Swimming().getProp('Hlt2RecoLine'),
                            granularityRefinement,
                            DEBUGMODE, startingEvent, swimStripping, Swimming().getProp('SwimOffSel'),
                            Swimming().getProp('OffSelModuleName'), offCands, offlinePVs
                          )
    gaudi.__dict__['globs'] = myGlobs

    def __next__():
        # Emit special end event Incident.
        incidentSvc.fireIncident(GaudiPython.gbl.Incident("SwimmingLoop", "SwimmingEndEvent"))

    # Import the utilities
    from Swimming.SwimmingUtils import (getBinfo, runSwimmingStep,
                                        createObject, getSelector)
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

    selector = getSelector(Swimming().getProp('SelectMethod'))(myGlobs)

    # Initialize at -1 so the eventNumber can be incremented at the top of the loop.
    eventNumber = -1
    while True:
        # Increment event number
        eventNumber += 1

        # Check if we've hit the maximum
        if (nEvents != -1 and eventNumber >= nEvents):
            print "Finished processing events"
            break
        elif (eventNumber % 100) == 0: 
            print "Processing event", eventNumber

        # Containers to store the turningpoints
        roughTurningPoints = []
        finalTurningPointsForWriting = []

        gaudi.run(1)
        if not TES["/Event/Rec/Header"] :
                print "Reached end of file, we're done!"
                break

        odin = TES['/Event/DAQ/ODIN']
        if DEBUGMODE :
            print "Processing %d %d" % (odin.runNumber(), odin.eventNumber())
        if not swimStripping and not Swimming().getProp('Simulation') and \
               odin.triggerConfigurationKey() != int(Swimming().getProp('TCK'), 16):
            print "WARNING, TCK 0x%08x does not match configured TCK %s, skipping event." \
                  % (odin.triggerConfigurationKey(), Swimming().getProp('TCK'))
            __next__()
            continue
        
        #Safety checks
        mycands = TES[offCands + "/Particles"]
        if not mycands :
            if DEBUGMODE :
                print "Problem while processing event number",startingEvent+eventNumber
                print "No container of offline candidates found!"
                print "Skipping the event but this is bad, check what you are doing!!"   
            __next__()
            continue
        elif mycands.size() == 0 : 
            if DEBUGMODE :
                print "Problem while processing event number",startingEvent+eventNumber
                print "Container of offline candidates empty!" 
                print "Skipping the event but this is bad, check what you are doing!!"
            __next__()
            continue
        if skipIfNoMuDSTCand :
            skipEvent = False
            for candloc in muDSTCands :
                mdstcands = TES[candloc + "/Particles"]
                if not mdstcands :
                    if DEBUGMODE :
                        print "Incident while processing event number",startingEvent+eventNumber
                        print "No container of muDST candidates "+candloc+" found!" 
                        print "Skipping the event"
                    skipEvent = True
                elif mdstcands.size() == 0 :
                    if DEBUGMODE :
                        print "Incident while processing event number",startingEvent+eventNumber
                        print "Empty container of muDST candidates "+candloc+" found!" 
                        print "Skipping the event"
                    skipEvent = True
            if skipEvent :
                __next__()
                continue
        if DEBUGMODE :
            print "Passed the safety checks OK"
            print "Candidates in event =", mycands.size()

        mycand = selector(mycands)
        
        if not mycand:
            print "Somehow there is no candidate!!! This should never happen."
            print "Skipping this event."
            __next__()
            continue

        if DEBUGMODE :
            print "Got the offline candidate OK"
            print "About to store its information"
            print mycand

        # If we are swimming the trigger and have a non-trivial selection method,
        # put the selected candidate in a separate location.
        if not swimStripping and Swimming().getProp('SelectMethod') != 'none':
            swimmingParticles = createObject(SharedParticles)
            swimmingParticles.insert(mycand)
            TES.unregisterObject(swimCands + '/Particles')
            TES.registerObject(swimCands + '/Particles', swimmingParticles)

        HltDecLast  = None
        HltDec      = None
        swimLoop    = int(-1.*maxSwimDistance + -1.*extraSwimDistance)
        while (swimLoop <= (maxSwimDistance + extraSwimDistance)) :
            if DEBUGMODE:
                print "Running over event", startingEvent+eventNumber,"swimming step",swimLoop            
            # We got this far now get the candidate and, for the "zero" step only
            # fill the corresponding event variables for it 
            # Note that this function call also executes the stripping algorithm
            # having moved the primary vertices around first
            runSwimmingStep(myGlobs, mycand, swimLoop)

            # Now get the trigger decision for this swimming step
            HltDec = evaluateTisTos(myGlobs,mycand)
            if DEBUGMODE :
                print "Retrieved HLT decision",HltDec,"OK for this event" 

            # If this is the very first swimming step, set HltDecLast to the global
            # HltDec at this point, otherwise get it from the previous step.
            if swimLoop == int(-1.*maxSwimDistance + -1.*extraSwimDistance):
                HltDecLast = HltDec
            elif roughTurningPoints != []:
                HltDecLast = roughTurningPoints[-1][3]

            # The first and last point are a special case for the bookkeeping
            # as we want to record the limits of our swimming and the trigger
            # decisions at these limits 
            if (abs(swimLoop) == int(1.*maxSwimDistance + 1.*extraSwimDistance)) or \
               (HltDec != HltDecLast): 
                # Get the lifetime and IP of the B/D with respect to its favourite PV
                bParams = getBinfo(myGlobs,mycand)
                roughTurningPoints.append([swimLoop,bParams[0],bParams[1],HltDec,HltDecLast])

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
            __next__()
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
            if ((thisturn[0] > -int(maxSwimDistance)) and (thisturn[0] <= int(maxSwimDistance))) :
                finalTurningPointsForWriting += refine(myGlobs,mycand,thisturn,initialGranularity,numberOfRefinements)
            else : 
                finalTurningPointsForWriting += refine(myGlobs,mycand,thisturn,extraGranularity,numberOfRefinements)
        if DEBUGMODE :
            print "****************************"
            print "The final turning points for writing are:" 
            pprint(finalTurningPointsForWriting)
            print "****************************" 

        # Which stage are we swimming?
        stage = None
        if swimStripping:
            stage = 'Stripping'
        else:
            stage = 'Trigger'

        # Get or Create the KeyedContainer for the SwimmingReports
        reportLocation = Swimming().getProp('SwimmingPrefix') + '/Reports'
        reports = TES[reportLocation]
        if not reports:
            reports = createObject(SwimmingReports)
            # Put the container in the TES
            sc = TES.registerObject(reportLocation, reports)
            if not sc.isSuccess():
                print "Failed to register SwimmingReports in TES"
                break
        # Get or create the SwimmingReport
        report = reports(mycand.key())
        if not report:
            report = createObject(LHCb.SwimmingReport, mycand)
            # Put the SwimmingReport in the container.
            reports.insert(report)
        elif report.hasStage(stage):
            print "WARNING, stage %s already present, this is unsupported and will fail!"
            return StatusCode(False)

        # Create the TurningPoint objects and put them in the report
        for turn in finalTurningPointsForWriting:
            d = std_map_sb()
            for decision, value in turn[3][1].iteritems():
                d[decision] = value
            m = std_map_sinfo()
            for decision, info in turn[3][2].iteritems():
                i = std_map_ulb()
                for k, v in info.iteritems():
                    i[k] = v
                m[decision] = i
            tp = LHCb.TurningPoint(turn[0], turn[1], turn[2], turn[3][0], d, m)
            report.addTurningPoint(stage, tp)

        # Create the relations table
        relations = TES[offCands + '/P2TPRelations']
        if not relations:
            relations = createObject(P2TPRelation)
            # Put the relations table in the TES
            sc = TES.registerObject(offCands + '/P2TPRelations', relations)
            if not sc.isSuccess():
                print "Failed to register SwimmingReports in TES"
                break

        from Swimming.SwimmingTisTos import appendToFSP
        rel = relations.relations(mycand)
        particles = [mycand]
        appendToFSP(mycand, particles)
        for particle in particles:
            if rel.empty():
                relations.relate(particle, report)
            elif rel(0).to().key() != report.key():
                print "There is a relation, but it points to another swimming report, this is very bad!!"
                return StatusCode(False)

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
