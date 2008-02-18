# Python script for T alignment
# @author Jan Amoraal
# @date 02-05-2007

# import gaudi python module
import FixGaudiOpts
from GaudiPython import *
from Gaudi.Configuration import*
from GaudiPython import gbl
from ROOT import *
from array import array
from tempfile import mkstemp
from os import write,close

def writeToTmpFile(s,postfix='.opts') :
   (fd,name) = mkstemp(postfix)
   write(fd,s)
   close(fd)
   return name

def getElements(det,prefix,subs) :
    return [ det[prefix + '/'+ s] for s in subs ]

def update(algorithm,appMgr) :
    # get pointer to incident service
    incSvc = appMgr.service('IncidentSvc', 'IIncidentSvc')
    # incident to trigger update of constants
    updateConstants = gbl.Incident(algorithm, 'UpdateConstants')
    incSvc.fireIncident(updateConstants)

def getAlignmentConstants(elements) :
    deltas = []
    for i in elements :
	[ deltas.append(j) for j in i.geometry().alignmentCondition().paramAsDoubleVect('dPosXYZ') ]
	[ deltas.append(j) for j in i.geometry().alignmentCondition().paramAsDoubleVect('dRotXYZ') ]
    return deltas

def printalgo(algname,mgr,prefix=' ') :
   """ print algorithm name, and, if it is a sequencer, recursively those algorithms it calls"""
   print prefix + algname
   alg = mgr.algorithm(algname.split('/')[-1])
   prop = alg.properties()
   if prop.has_key('Members') :
       subs = prop['Members'].value()
       for i in subs : printalgo(i.strip('"'),mgr,prefix+'     ')
   elif prop.has_key('DetectorList') :
       subs = prop['DetectorList'].value()
       for i in subs : printalgo(algname.split('/')[-1] + i.strip('"') + 'Seq',mgr,prefix+'     ')

def printflow(mgr) :
   mp = mgr.properties()
   print '\n\nApplication flow:\n\n'
   for i in mp['TopAlg'].value(): printalgo(i,mgr)
   print '\n\n'

def printdod(dod) :
   dp = dod.properties()
   pat = re.compile(r"DATA=\'(?P<data>[^']+).*TYPE=\'(?P<type>[^']+)",re.IGNORECASE)
   t2d = {}
   for i in dp['Algorithms'].value() :
      r = pat.search(i)
      if not r : break
      (d,t) = (r.group('data'), r.group('type'))
      if not t2d.has_key(t) : t2d[t]=[d]
      else : t2d[t].append(d)
   for (t,d) in t2d.iteritems() :
      print '\n'+str(d)
      printalgo(t,mgr,'    ')

def run(filename, data, nEvents, nIter, CondTag, MagOff, OTHitsOnly, Constraints, UseCorrelations, UseDrift, ApplyMS, extraOpts, what, group) :
    print "==> Running job " + filename
    # instantiate application manager
    # importOptions(writeToTmpFile(options))
    importOptions("$TALIGNMENTROOT/options/Escher_nikhef.opts")

    #    from Configurables import UpdateManagerSvc
    #    ums = UpdateManagerSvc()
    #    ums.ConditionsOverride +=  [
    #       "Conditions/Alignment/OT/T1Station := double_v dPosXYZ =  0.3 1.5 0.0 ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
    #       "Conditions/Alignment/OT/T2Station := double_v dPosXYZ = -0.1 1.0 0.0 ; double_v dRotXYZ = 0.0  0.0005  0.0001 ;",
    #       "Conditions/Alignment/OT/T3Station := double_v dPosXYZ =  0.2 -1.0 0.0 ; double_v dRotXYZ = 0.0 -0.0001 -0.0005 ;"
    #       ]

    # let's configure the alignment using python options. Ooohh Aaahhh
    # Tracking
    from Configurables import TrackFilterAlg, GetElementsToBeAligned, AlignSelTool
    from Configurables import TrajOTProjector, TrackEventFitter, TrackMasterFitter, TrackMasterExtrapolator, TrackKalmanFilter, TrackSimpleExtraSelector
    from Configurables import TrackContainerCopy
    #from Configurables import TrackSelector
    
    trackFitSeq             = GaudiSequencer("TrackFitSeq")
    trackFitSeq.MeasureTime = True;
    allConfigurables["EscherMainSeq"].Members += [trackFitSeq.getFullName()]

    trackFilterAlg = TrackFilterAlg("FilterTracks",
                                    OutputLevel               = INFO,
                                    TracksInputContainer      = "Rec/Track/Best",
                                    TracksOutputContainer     = "Alignment/FilteredTracks",
                                    TrackType                 = "Long",
                                    StripUnwantedDetectorHits = False,
                                    KeepDetectorHits          = "OT",
                                    MinNHits                  = 0)
    
    trackFilterAlg.GetElementsToBeAligned = GetElementsToBeAligned(OutputLevel   = INFO,
                                                                   GroupElements = group,
                                                                   Elements      = what)
    
    trackFilterAlg.Selector               = AlignSelTool(OutputLevel  = INFO,
                                                         BFieldStatus = True) #,
                                                         #TrackType    = "OT")

    trajOTProjector                       = TrajOTProjector(OutputLevel = INFO,
                                                            UseDrift    = UseDrift)
    
    trackFitAlg                           = TrackEventFitter("FitTracks",
                                                             OutputLevel        = INFO,
                                                             TracksInContainer  = "Alignment/FilteredTracks",
                                                             TracksOutContainer = "Alignment/AlignmentTracks")

    
                                                               

    trackFitAlg.addTool(TrackMasterFitter("Fitter"))
    trackMasterFitter = allConfigurables["FitTracks.Fitter"]
    trackMasterFitter.addTool(TrackMasterExtrapolator("Extrapolator"))
    trackMasterExtrapolator = allConfigurables["FitTracks.Fitter.Extrapolator"]
    trackMasterFitter.addTool(TrackKalmanFilter("NodeFitter"))
    trackMasterNodeFitter = allConfigurables["FitTracks.Fitter.NodeFitter"]
    trackMasterNodeFitter.addTool(TrackMasterExtrapolator("Extrapolator"))
    trackMasterNodeFitterExtrapolator = allConfigurables["FitTracks.Fitter.NodeFitter.Extrapolator"]

    if MagOff == False:
       importOptions("$TALIGNMENTROOT/options/BFieldOn.opts")

    if MagOff == True:
       importOptions("$TALIGNMENTROOT/options/BFieldOff.opts")

    trackContainerCopy                    = TrackContainerCopy("CopyTracks",
                                                               inputLocation = "Alignment/AlignmentTracks",
                                                               outputLocation = "Alignment/CopiedAlignmentTracks",
                                                               copyFailures  = False)
    importOptions("$TALIGNMENTROOT/options/TrackContainerCopy.opts")
    
    allConfigurables["TrackFitSeq"].Members = [trackFilterAlg, trackFitAlg, trackContainerCopy]

    # Alignment
    from Configurables import AlignAlgorithm, GetElementsToBeAligned, gslSVDsolver, DiagSolvTool

    alignSeq             = GaudiSequencer("AlignmentSeq")
    alignSeq.MeasureTime = True
    allConfigurables["EscherMainSeq"].Members += [alignSeq.getFullName()]

    alignAlg                        = AlignAlgorithm("Alignment",
                                                     OutputLevel                 = INFO,
                                                     TracksLocation              = "Alignment/CopiedAlignmentTracks",
                                                     NumberOfIterations          = nIter,
                                                     UseCorrelations             = UseCorrelations,
                                                     CanonicalConstraintStrategy = 1,
                                                     ConstrainZShearings         = True,
                                                     MinNumberOfHits             = 10,
                                                     UsePreconditioning          = True)

#    alignAlg.MatrixSolver           = DiagSolvTool() # gslSVDsolver()
    alignAlg.MatrixSolver           = gslSVDsolver()
    alignAlg.MatrixSolverTool       = alignAlg.MatrixSolver.getType()
    alignAlg.GetElementsToBeAligned = GetElementsToBeAligned(OutputLevel   = INFO,
                                                             GroupElements = group,
                                                             Elements      = what)
    allConfigurables["AlignmentSeq"].Members = [alignAlg]

    from Configurables import CondDBDispatcherSvc, CondDBAccessSvc
    myCondDB = CondDBAccessSvc("LHCBCOND").clone("MyLHCBCOND",
                                                 ConnectionString = "sqlite_file:/afs/cern.ch/user/j/janos/cmtuser/Alignment_v2r3/Alignment/TAlignment/v3r4/python/LHCBCOND.db/LHCBCOND",
                                                 DefaultTAG = CondTag )
    
    mainDBReader = CondDBDispatcherSvc()
    mainDBReader.Alternatives.append("/Conditions/OT=%s"%myCondDB.getFullName())
    
    #     from pprint import PrettyPrinter
    #     PrettyPrinter().pprint(configurationDict())
    #     return

    appMgr = AppMgr()
    #appMgr.initialize()
    #appMgr = GaudiPython.AppMgr(outputlevel=5, joboptions=writeToTmpFile(options))

    printflow(appMgr)

    # get pointer to alignment algorithm
    # need to do this here ... after specifying input, we've already gone through 'initialize'!
    #alignAlg = appMgr.algorithm('Alignment')
    #alignAlg.NumberOfIterations = nIter

    # get pointer to histogram persistency svc
    histoPerSvc = appMgr.service('HistogramPersistencySvc')
    # set histogram filename

    histoPerSvc.OutputFile = '../'+filename+'.root'

    # get pointer to event selector
    sel = appMgr.evtsel()
    
    # open files
    sel.open(data)

    # set print frequency
    sel.PrintFreq = 500

    # get align elements
    elements = getElements(appMgr.detsvc(),'/dd/Structure/LHCb/AfterMagnetRegion/T/OT',['T1','T2','T3'])
    # elements = getElements(appMgr.detsvc(),'/dd/Structure/LHCb/AfterMagnetRegion/T',['OT'])
    #elements = getElements(appMgr.detsvc(),'/dd/Structure/LHCb',['BeforeMagnetRegion/Velo', 'AfterMagnetRegion/T/IT', 'AfterMagnetRegion/T/OT'])
#    elements = getElements(appMgr.detsvc(),'/dd/Structure/LHCb/AfterMagnetRegion/T/OT',['T1/X1layer','T1/Ulayer','T1/Vlayer','T1/X2layer', 'T2/X1layer','T2/Ulayer','T2/Vlayer','T2/X2layer', 'T3/X1layer','T3/Ulayer','T3/Vlayer','T3/X2layer'])

    # get initial constants
    deltaVsIter = [getAlignmentConstants(elements)]

    # run over events
    print '==> Iterating ' + str(nIter) + ' times over ' + str(nEvents) + ' events'

    # i iterations
    for i in range(nIter) :
	#oldDeltas = getAlignmentConstants(elements)
	#print "==> iteration " + str(i) + " : Initial alignment conditions  : " + str(oldDeltas)
	appMgr.run(nEvents)
	# trigger update of alignment parameters
	update(alignAlg.name(),appMgr)
	# print old and new deltas
	#newDeltas = getAlignmentConstants(elements)
	#print "==> iteration " + str(i) + " : Updated alignment conditions  : " + str(newDeltas)
	#deltaVsIter.append(newDeltas)
	sel.rewind()

    print "==> Deltas per iterarion"
    for i in deltaVsIter : print i

    # #make delta plots
    deltaPosPlots = []
    deltaRotPlots = []
    for i in range(len(elements)) :
	dPosXYZHistos = []
	dRotXYZHistos = []
	for i in range(3) :
	    dPosXYZHistos.append(TGraph())
	    dRotXYZHistos.append(TGraph())
	deltaPosPlots.append(dPosXYZHistos)
	deltaRotPlots.append(dRotXYZHistos)

    for i in range(len(elements)) :
	for j in range(len(deltaVsIter)) :
	    deltaPosPlots[i][0].SetPoint(j, j+1, deltaVsIter[j][i*6])
	    deltaPosPlots[i][1].SetPoint(j, j+1, deltaVsIter[j][i*6+1])
	    deltaPosPlots[i][2].SetPoint(j, j+1, deltaVsIter[j][i*6+2])
	    deltaRotPlots[i][0].SetPoint(j, j+1, deltaVsIter[j][i*6+3])
	    deltaRotPlots[i][1].SetPoint(j, j+1, deltaVsIter[j][i*6+4])
	    deltaRotPlots[i][2].SetPoint(j, j+1, deltaVsIter[j][i*6+5])

    # write plots
    file = TFile('$TALIGNMENTROOT/deltas'+filename+'.root', 'recreate')
    if (file) :
	print 'Writing deltas to file'
	for i in range(len(deltaPosPlots)) :
	    for j in range(len(deltaPosPlots[i])) :
		name = "Element"+str(i)+"Parameter"+str(10+j)
		deltaPosPlots[i][j].SetName(name)
		deltaPosPlots[i][j].Write()
	for i in range(len(deltaRotPlots)) :
	    for j in range(len(deltaRotPlots[i])) :
		name = "Element"+str(i)+"Parameter"+str(20+j)
		deltaRotPlots[i][j].SetName(name)
		deltaRotPlots[i][j].Write()
	file.Close()
