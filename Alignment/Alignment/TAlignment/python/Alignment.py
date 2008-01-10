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
    #options = '''
    #/// Include default Escher options
    ##include "$TALIGNMENTROOT/options/Escher_nikhef.opts"
    #'''

    #options += extraOpts;
    #print options

    # instantiate application manager
    # importOptions(writeToTmpFile(options))
    importOptions("$TALIGNMENTROOT/options/Escher_nikhef.opts")

    # let's configure the alignment using python options. Ooohh Aaahhh
    # Tracking
    from Configurables import TrackFilterAlg, GetElementsToBeAligned, AlignSelTool
    from Configurables import TrajOTProjector, TrackEventFitter, TrackMasterFitter, TrackMasterExtrapolator, TrackKalmanFilter, TrackSimpleExtraSelector   

    trackFitSeq             = GaudiSequencer("TrackFitSeq")
    trackFitSeq.MeasureTime = True;
    allConfigurables["EscherMainSeq"].Members += [trackFitSeq.getFullName()]

    trackFilterAlg = TrackFilterAlg("FilterTracks",
                                    OutputLevel               = DEBUG,
                                    TracksInputContainer      = "Rec/Track/Best",
                                    TracksOutputContainer     = "Alignment/FilteredTracks",
                                    TrackType                 = "Long",
                                    StripUnwantedDetectorHits = True,
                                    KeepDetectorHits          = "OT",
                                    MinNHits                  = 3)
    trackFilterAlg.GetElementsToBeAligned = GetElementsToBeAligned(OutputLevel   = DEBUG,
                                                                   GroupElements = group,
                                                                   Elements      = what)
    trackFilterAlg.Selector               = AlignSelTool(OutputLevel  = DEBUG,
                                                         BFieldStatus = False,
                                                         TrackType    = "OT")
    trackFilterAlg.TrackSelector          = trackFilterAlg.Selector.getType()


    trajOTProjector                  = TrajOTProjector(OutputLevel = DEBUG,
                                                       UseDrift    = UseDrift)
    trackFitAlg                      = TrackEventFitter("FitTracks",
                                                        OutputLevel        = DEBUG,
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
    
    
    if MagOff == True:
       importOptions("$TALIGNMENTROOT/options/BFieldOff.opts")
       #trackMasterFitter.ErrorP                       = [1e-3, 1e-8]
       #trackMasterFitter.NumberFitIterations          = 4
       #trackMasterExtrapolator.ApplyEnergyLossCorr    = False
       ### The following doesn't work (i think it might be an 'inheritance' problem
       #trackMasterExtrapolator.addTool(TrackSimpleExtraSelector("ExtraSelector"))
       #trackMasterExtrapolatorExtraSelector = allConfigurables["FitTracks.Fitter.Extrapolator.ExtraSelector"]
       #trackMasterExtrapolatorExtraSelector.ExtrapolatorName  = "TrackLinearExtrapolator"
       #trackMasterNodeFitterExtrapolator.ApplyEnergyLossCorr  = False
       #trackMasterNodeFitterExtrapolator.addTool(TrackSimpleExtraSelector("ExtraSelector"))
       #trackMasterNodeFitterExtrapolatorExtraSelector = allConfigurables["FitTracks.Fitter.NodeFitter.Extrapolator.ExtraSelector"]
       #trackMasterNodeFitterExtrapolatorExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
    
    # These are magnet off options for the fitter
    allConfigurables["TrackFitSeq"].Members = [trackFilterAlg, trackFitAlg]
    
    # Alignment
    from Configurables import AlignAlgorithm, GetElementsToBeAligned, gslSVDsolver

    alignSeq             = GaudiSequencer("AlignmentSeq")
    alignSeq.MeasureTime = True                    
    allConfigurables["EscherMainSeq"].Members += [alignSeq.getFullName()]

    alignAlg                        = AlignAlgorithm("Alignment",
                                                     OutputLevel        = DEBUG,
                                                     TracksLocation     = "Alignment/AlignmentTracks",
                                                     NumberOfIterations = nIter,
                                                     UseCorrelations    = UseCorrelations)
    
    alignAlg.MatrixSolver           = gslSVDsolver()
    alignAlg.MatrixSolverTool       = alignAlg.MatrixSolver.getType()
    alignAlg.GetElementsToBeAligned = GetElementsToBeAligned(OutputLevel   = DEBUG,
                                                             GroupElements = group,
                                                             Elements      = what)
    allConfigurables["AlignmentSeq"].Members = [alignAlg]

    #return
#     from Configurables import CondDBDispatcherSvc, CondDBAccessSvc
#     myCondDB = CondDBAccessSvc("LHCBCOND").clone("MyLHCBCOND",
#                                                  ConnectionString = "sqlite_file:/project/bfys/janos/AlignmentDev/Alignment_v2r0/Alignment/TAlignment/v3r1/python/LHCBCOND.db/LHCBCOND",
#                                                  DefaultTAG = CondTag )
    
#     mainDBReader = CondDBDispatcherSvc()
#     mainDBReader.Alternatives.append("/Conditions/OT=%s"%myCondDB.getFullName())

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
    #-- For Event Type = 13144002 / Data type = DST 1
    #--     Configuration = DC06 - phys-v2-lumi2
    #--     DST 1 datasets produced by Brunel - v30r17
    #--     From DIGI 1 datasets produced by Boole - v12r10
    #--     From SIM 1 datasets produced by Gauss - v25r10
    #--     Database version = v30r14
    #--     Cards content = physical-physical
    #--
    #-- Datasets replicated at CERN
    #-- 200 dataset(s) - NbEvents = 99368
    sel.open(data)

    # set print frequency
    sel.PrintFreq = 500

    # get align elements
    elements = getElements(appMgr.detsvc(),'/dd/Structure/LHCb/AfterMagnetRegion/T/OT',['T1','T2','T3'])
    # elements = getElements(appMgr.detsvc(),'/dd/Structure/LHCb/AfterMagnetRegion/T',['OT'])

    # get initial constants
    deltaVsIter = [getAlignmentConstants(elements)]

    # run over events
    print '==> Iterating ' + str(nIter) + ' times over ' + str(nEvents) + ' events'

    # i iterations
    for i in range(nIter) :
	oldDeltas = getAlignmentConstants(elements)
	print "==> iteration " + str(i) #+ " : Initial alignment conditions  : " + str(oldDeltas)
	appMgr.run(nEvents)
	# trigger update of alignment parameters
	update(alignAlg.name(),appMgr)
	# print old and new deltas
	newDeltas = getAlignmentConstants(elements)
	print "==> iteration " + str(i) + " : Updated alignment conditions  : " + str(newDeltas)
	deltaVsIter.append(newDeltas)
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
