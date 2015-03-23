'''
The top level algorithm for the swimming
'''

__author__  = "Vladimir V. Gligorov"
__date__    = "2011-04-14"

#! /usr/bin/env python
from os import environ
from pprint import pprint
import GaudiPython 
from Gaudi.Configuration import *
from GaudiPython import InterfaceCast
from Gaudi import *
from math import *
import sys 
from Swimming.decorators import *

# Use the options parser to get the job options
from optparse import OptionParser
parser = OptionParser()
# Toggles : Swim MC or Real data? (Real data is default)
#         : Debug mode? Off by default, this prints out a LOT.
#         : Swim the stripping? Off by default (hence swims trigger)
#         : Swim an offline selection? Off by default, can only be 
#           used when swimming the stripping.   
parser.add_option("--root",             action = "store_true", dest = "ROOT",          default = False)
parser.add_option("--mc",               action = "store_true", dest = "forMC",         default = False)
parser.add_option("--debug",            action = "store_true", dest = "debug",         default = False)
parser.add_option("--swimStripping",    action = "store_true", dest = "swimStripping", default = False)
parser.add_option("--swimOffsel",       action = "store_true", dest = "swimOffsel",    default = False)
# Basic options : number of events to swim
#               : starting event number on DST
#               : TCK to swim (only if swimStripping is False)
#               : stripping stream (only swimStripping is True)
#               : stripping version (only swimStripping is True)
#               : stripping line (only one allowed, only if swimStripping is True)
#               : the HLT1 triggers to swim (only if swimStripping is False)
#               : the HLT2 triggers to swim (only if swimStripping is False)                
#               : the name of the module holding the offline selection (only if swimStripping and swimOffsel are True) 
parser.add_option("-n",                 action = "store", type = "string", dest = "numevents",       default = '10') 
parser.add_option("-s",                 action = "store", type = "string", dest = "firstevent",      default = '0')
parser.add_option("--TCK",              action = "store", type = "string", dest = "TCKid",           default = "0x00730035") 
parser.add_option("--strippingStream",  action = "store", type = "string", dest = "strippingStream", default = "CharmCompleteEvent")
parser.add_option("--strippingVersion", action = "store", type = "string", dest = "strippingVersion",default = "Stripping13b")
parser.add_option("--strippingLine",    action = "store", type = "string", dest = "strippingLine",   default = "D2hh")
parser.add_option("--strippingFile",    action = "store", type = "string", dest = "strippingFile",   default = "none")
parser.add_option("--hlt1triggers",     action = "store", type = "string", dest = "hlt1triggers",    default = "Hlt1TrackAllL0Decision")
parser.add_option("--hlt2triggers",     action = "store", type = "string", dest = "hlt2triggers",    default = "Hlt2CharmHadD02HH_D02KPiDecision")
parser.add_option("--offselmodulename", action = "store", type = "string", dest = "offselmodulename",default = "OfflineSelExample")
# In addition to the "regular" swimming, which tells us the 
# trigger/stripping decision as a function of the candidate
# propertime, we also need to know which parts of the signal
# candidate were reconstructed in the trigger. This is because
# e.g. in HLT1 we only need one track to trigger, but the
# acceptance function is different depending on the number of
# tracks which had the possibility to fire the trigger, i.e.
# the number of tracks reconstructed in the trigger reconstruction.
# The defaults should be sufficient for all 2011 data processing. 
parser.add_option("--hlt1recoline",    action = "store", type = "string", dest="hlt1recoline", default = "Hlt1TrackForwardPassThroughLooseDecision")
parser.add_option("--hlt2recoline",    action = "store", type = "string", dest="hlt2recoline", default = "Hlt2ForwardDecision")      
# TES locations of candidates : offline
#                             : stripping
#                             : for the microDST writer
parser.add_option("--offcands",        action = "store", type = "string", dest="offcands",
                  default='/Event/CharmCompleteEvent/Phys/D2hhPromptD0RSSel')
parser.add_option("--stripcands",      action = "store", type = "string", dest="stripcands",
                  default="/Event/Phys/D2hhPromptD0RSSel")
parser.add_option("--mudstcands",      action = "store", type = "string", dest="mudstlocation",
                  default="/Event/CharmCompleteEvent/Phys/D2hhPromptDst2D2RSLine")
# Other options : DDDB tag
#               : CondDB tag
#               : TES location of HLT PVs (default is fine for 2011 data)
#               : TES location of Offline PVs (default is fine for 2011 data)
#               : HLT selection name of PVs (default is fine for 2011 data)
#               : Datatype
parser.add_option("--dddb",            action = "store", type = "string", dest = "dddb",       default="head-20110302")
parser.add_option("--conddb",          action = "store", type = "string", dest = "conddb",     default="head-20110407")
parser.add_option("--onlinePV",        action = "store", type = "string", dest = "onlinePV",   default="TES:Hlt/Vertex/PV3D")
parser.add_option("--offlinePV",       action = "store", type = "string", dest = "offlinePV",  default="/Event/Rec/Vertex/Primary")
parser.add_option("--outPVSel",        action = "store", type = "string", dest = "outPVSel",   default="PV3D")
parser.add_option("--datatype",        action = "store", type = "string", dest = "datatype",   default="2011")
parser.add_option("--filestager",      action = "store_true", dest = "filestager", default = False)
# Tool names    : defaults are fine for 2011 data, only for swimming the stripping
parser.add_option("--relPVFinder",     action = "store", type = "string", dest = "relPVFinder", 
                  default = "GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_/P2PVWithIPChi2")
parser.add_option("--distCalc",        action = "store", type = "string", dest = "distCalc",
                  default = "LoKi::DistanceCalculator")
parser.add_option("--tauCalc",         action = "store", type = "string", dest = "tauCalc",
                  default = "LoKi::LifetimeFitter")
# The options for the swimming granularity etc.
# Because we do not apriori know how many turning points there are in each event,
# we need to swim in two steps. First we scan the Velo end-to-end with some given
# granularity (default is 4mm). Then each turning point is iteratively refined
# to a precision of around 16 microns (by default) in steps defined by the granularityRefinement
# variable. For example if the initial granularity is 4mm and the granularityRefinement
# is 4, the steps would be 4mm, 1mm, 0.25mm, 0.0625mm, 0.015625mm.
#
# The swimming is done in the direction of the B/D momentum.
#
# maxSwimDistance       : the maximum distance to swim (forward and backward) with the "fine" granularity 
# initialGranularity    : the "fine" granularity with which to swim. 
# granularityRefinement : how much to "refine" the granularity in each step. The initial granularity is divided by this number.
# numberOfRefinements   : how many times to "refine" each turning point
# extraSwimDistance     : the maximum distance to swim (forward and backward) beyond the "fine" granularity
# extraGranularity      : the "coarse" granularity with which to swim.  
parser.add_option("--maxSwimDistance",       action = "store", type = "float",   dest = "maxSwimDistance",       default = 200.) #mm
parser.add_option("--initialGranularity",    action = "store", type = "float",   dest = "initialGranularity",    default = 4.)   #mm 
parser.add_option("--granularityRefinement", action = "store", type = "float",   dest = "granularityRefinement", default = 4.)   #dimensionless
parser.add_option("--numberOfRefinements",   action = "store", type = "int",     dest = "numberOfRefinements",   default = 4)    #dimensionless
parser.add_option("--extraSwimDistance",     action = "store", type = "float",   dest = "extraSwimDistance",     default = 400.) #mm
parser.add_option("--extraGranularity",      action = "store", type = "float",   dest = "extraGranularity",      default = 40.)  #mm
# The name of the HLT transform to use. This is the code which modifies the TCK in order to allow the
# trigger to be swum. This must match the data -- in particular in 2011 we recognise
# two periods which need different transforms : with and without the beamspot filter for primary vertices.  
parser.add_option("--transformName",         action = "store", type = "string",  dest = "transformName",         default = "2011_WithBeamSpotFilter")
# For writing the data out
parser.add_option("--swimReportLocation",    action = "store", type = "string",  dest = "swimRepLoc",            default = "/Event/Swimming/Reports") 
parser.add_option("--outfilename",           action = "store", type = "string",  dest = "outfilename",           default = "SwimTrigDST.dst")
# How to select a single candidate, none assumes that there is a single
# candidate, random selects one at random and first selects the first candidate
parser.add_option("--selectMethod",          action = "store", type = "string",  dest = "select",                default = "random")
# Switch this on to make a quick interactive test on some Stripping13b charm
# data
parser.add_option("--test",                  action = "store_true",              dest = "interactiveTest",       default = False)
(options, args) = parser.parse_args()

from Swimming.HltTransforms import getTransform
thisTransform = getTransform(options.transformName,options.hlt1triggers.split(',')+options.hlt2triggers.split(','))
# Check for incompatible options!
if not options.swimStripping and not thisTransform :
    print "No or invalid HLT transform specified when trying to swim the trigger!"
    sys.exit(-1)  
if options.swimOffsel and not options.swimStripping :
    print "You cannot swim the trigger and offline selection in the same job!"
    print "Goodbye!"
    sys.exit(-1)

if options.select not in ['random', 'first', 'none']:
    print 'The selectMethod must be eiter "random", "first" or "none".'
    sys.exit(-1)

# Shorter names for some of the options
nEvents                 = int(options.numevents)
offCands                = options.offcands
swimCands               = '/Event/Swimming'
offlinePVs              = options.offlinePV
DEBUGMODE               = options.debug
startingEvent           = int(options.firstevent)
# The maximum swimming distance is maxSwimDistance (in mm)
# The inital granularity is in mm
# The number of passes is how many times we try to revise the
# precision with which the acceptance function is known  
# maxSwimDistance should be an integer multiple of the
# initialGranularity  
maxSwimDistance         = options.maxSwimDistance
initialGranularity      = options.initialGranularity
# By how much to divide the granularity each time
# and how many times to refine
granularityRefinement   = options.granularityRefinement 
numberOfRefinements     = options.numberOfRefinements
# Add some swimming in "outer" regions of propertime to study potential
# upper lifetime acceptance effects
# Here the granularity is deliberately coarser
# Again, extraSwimDistance should be an integer
# multiple of the extraGranularity 
extraSwimDistance       = options.extraSwimDistance
extraGranularity        = options.extraGranularity
# Swim the stripping or the trigger?
# default is the trigger, switch this to true
# if you want to swim the stripping (and/or offline)
swimStripping           = options.swimStripping
swimOffsel              = options.swimOffsel

if swimStripping :
    from Configurables import DaVinci
    from StrippingConf.Configuration import StrippingConf
    from Configurables import ProcStatusCheck
    from Configurables import EventNodeKiller,GaudiSequencer
    from PhysSelPython.Wrappers import AutomaticData, SelectionSequence
    #
    from StrippingSettings.Utils import lineBuilderConfiguration
    strippingFile = None
    if options.strippingFile != 'none':
        strippingFile = options.strippingFile
    else:
        strippingFile = options.strippingLine
    myconfig = lineBuilderConfiguration(options.strippingVersion, options.strippingLine)
    import StrippingSelections
    mylineconf = getattr(__import__('StrippingSelections.Stripping' + strippingFile,
                                    globals(),locals(),
                                    [myconfig["BUILDERTYPE"]],-1),myconfig["BUILDERTYPE"])
    mylinedict = myconfig["CONFIG"]
    print mylineconf
    print mylinedict
    from StrippingConf.StrippingStream import StrippingStream
    stream = StrippingStream(options.strippingStream + "Swimming")
    allLines = mylineconf(options.strippingLine, mylinedict).lines()
    lines = []
    for l in allLines:
        lineName = options.stripcands.split('/')[-1]
        if l.outputLocation().find(lineName) != -1:
            lines.append(l)
    stream.appendLines(lines)
    #Define the stream
    filterBadEvents = ProcStatusCheck()
    sc = StrippingConf( Streams = [stream],
                        MaxCandidates       = 2000,
                        AcceptBadEvents     = False,
                        BadEventSelection   = filterBadEvents )
    #Define the node killer, and make sure to kill everything corresponding to
    #the stream which we want to swim
    outputs = []
    for l in lines:
        for f in l.filterMembers():
            if not hasattr(f, 'Output'):
                continue
            # Remove the last item so we get everything (Particle, relations,
            # decayVertices, etc...
            o = '/'.join(f.Output.split('/')[:-1])
            outputs.append(o)
    mykiller    = EventNodeKiller("killStripping")
    #Some default nodes which we will want to kill in all cases
    nodestokill = outputs + ['Strip', '/Event/Rec/Vertex/Primary']
    mykiller.Nodes          = nodestokill 
    deathstar               = GaudiSequencer("killStrippingSeq")
    deathstar.Members       = [mykiller]
    # Configure DaVinci
    DaVinci().InputType     = 'DST'
    DaVinci().DataType      = options.datatype
    DaVinci().Simulation    = options.forMC
    DaVinci().DDDBtag       = options.dddb
    DaVinci().CondDBtag     = options.conddb
    # The sequence for the swimming has to be configured 
    # by hand inserting the node killer before it
    DaVinci().appendToMainSequence( [deathstar] )
    DaVinci().appendToMainSequence( [ sc.sequence() ] )
    #uDST writer
    from DSTWriters.__dev__.Configuration import MicroDSTWriter, microDSTStreamConf
    from DSTWriters.__dev__.microdstelements import *
    SwimmingConf = microDSTStreamConf()
    streamConf = { 'default' : SwimmingConf }
    SwimmingElements = [ CloneRecHeader(),
                         CloneODIN(),
                         ClonePVs(),
                         CloneSwimmingReports(),
                         CloneParticleTrees(copyProtoParticles = True),
                         ClonePVRelations("Particle2VertexRelations", True),
                         CloneTPRelations("P2TPRelations", True),
                         ReFitAndClonePVs(),
                         CloneLHCbIDs(),
                         CloneRawBanks(banks = ['HltSelReports', 'HltDecReports',
                                                'L0DU', 'L0Calo', 'L0CaloError', 'L0CaloFull',
                                                'L0Muon', 'L0MuonProcCand', 'L0MuonError'],
                                        inputRawEvent = 'DAQ/RawEvent')]
    elementsConf = { 'default' : SwimmingElements }
    inputData = AutomaticData(Location = offCands + "/Particles")
    selectionSeq = SelectionSequence("SwimmingMicroDST",
                                     TopSelection = inputData)
    dstWriter = MicroDSTWriter('MicroDST',
                               StreamConf           = streamConf,
                               MicroDSTElements     = elementsConf,
                               OutputFileSuffix     = 'Swimming',
                               SelectionSequences   = [selectionSeq])
    DaVinci().appendToMainSequence([dstWriter.sequence()])
    if options.interactiveTest :
        EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/r/raaij/test/SwimTrigDST.dst'"]
else :
    from Configurables import HltMoveVerticesForSwimming
    from Configurables import Moore,HltConfigSvc
    from Configurables import CondDB
    CondDB(UseOracle = True)
    #Global configuration
    mykiller    = EventNodeKiller("killHlt")
    mykiller.Nodes          = ["Hlt","Hlt1","Hlt2","Trig","Raw"]
    deathstar               = GaudiSequencer("killHltSeq")
    deathstar.Members       = [mykiller]
    from Swimming import MooreSetup
    #
    Moore().InitialTCK = options.TCKid
    Moore().Simulation = options.forMC
    Moore().DataType   = options.datatype
    Moore().DDDBtag    = options.dddb
    Moore().CondDBtag  = options.conddb
    Moore().outputFile = options.outfilename
    Moore().WriteFSR = False
    Moore().WriterRequires = []
    # Add extra locations to writer
    from Configurables import InputCopyStream
    writer = InputCopyStream('Writer')
    writer.ItemList = [offCands + '/P2TPRelations#1', options.swimRepLoc + '#1']
    
    if options.interactiveTest :
        EventSelector().Input = ["DATAFILE='PFN:root://bee28.grid.sara.nl:1094/pnfs/grid.sara.nl/data/lhcb/user/r/raaij/2011_10/26549/26549151/Bs2JpsiPhiPreDet.SingleOffline.dst'"]
    #
    # Define the TCK transformation
    # 
    HltConfigSvc().ApplyTransformation = thisTransform
    #
    # Define the swimming algorithm
    #
    myswimmer                   = HltMoveVerticesForSwimming("HltMovePVs4Swimming")
    myswimmer.SwimmingDistance  = 0.0
    myswimmer.Bcontainer        = offCands if options.select == 'none' else swimCands
    myswimmer.InputSelection    = options.onlinePV
    myswimmer.OutputSelection   = options.outPVSel
    myswimmer.OutputLevel       = 4

#The general setup
from Swimming import GeneralSetup
from Configurables import DstConf, CaloDstUnPackConf
CaloDstUnPackConf ( Enable = True )
DstConf           ( EnableUnpack = True )#
ApplicationMgr().TopAlg = [deathstar] + ApplicationMgr().TopAlg

# New persistency
if options.ROOT:
    from GaudiConf import IOHelper
    IOHelper("ROOT","ROOT").postConfigServices()

# FileStager
if options.filestager:
    from FileStager.Configuration import configureFileStager
    configureFileStager()

# Disable muons in TisTosTool
from Configurables import TriggerTisTos
ToolSvc().addTool(TriggerTisTos,'TriggerTisTos')
ToolSvc().TriggerTisTos.TOSFracMuon = 0.

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
    gaudi.algorithm('MicroDSTMainSeq').Enable   = False
    triggers = options.stripcands
#Tools needed for for tistosing
else :
    for algo in ['killHltSeq', 'seqL0', 'Hlt',
                 'HltSelReportsWriter', 'Writer',
                 'HltDecReportsWriter', 'HltVertexReportsWriter',
                 'HltLumiWriter', 'HltLumiStripper',
                 'HltRoutingBitsWriter']:
        gaudi.algorithm(algo).Enable = False
    hlt1triggers = options.hlt1triggers.split(',')
    hlt2triggers = options.hlt2triggers.split(',')
    triggers     = [hlt1triggers,hlt2triggers]

#The tis tos tool
tistostool      = gaudi.toolsvc().create('TriggerTisTos'    ,interface='ITriggerTisTos')
tistosbools     = [tistostool.kAnything,tistostool.kTrueRequired,tistostool.kFalseRequired]
#The tools used for swimming the stripping/offline
m_lifetimeTool  = gaudi.toolsvc().create(options.tauCalc     ,interface='ILifetimeFitter')
m_distanceTool  = gaudi.toolsvc().create(options.distCalc    ,interface='IDistanceCalculator')
m_relatedPVTool = gaudi.toolsvc().create(options.relPVFinder ,interface='IRelatedPVFinder')

#Make the class of globals for passing information around
from Swimming.Globals import globalParams
tmpSvc = gaudi.service("IncidentSvc")
m_incidentSvc = InterfaceCast("IIncidentSvc")(tmpSvc.getInterface())
myGlobs = globalParams( GaudiPython,gaudi,SEL,TES,m_incidentSvc,
                        m_lifetimeTool,m_distanceTool,m_relatedPVTool,
                        tistostool,tistosbools,triggers, 
                        options.hlt1recoline, options.hlt2recoline,
                        granularityRefinement,
                        DEBUGMODE,startingEvent,swimStripping,options.swimOffsel,
                        options.offselmodulename,offCands,offlinePVs
                      )

# Import the utilities
from Swimming.SwimmingUtils import * 
from Swimming.RefineTPs import refine
from Swimming.SwimmingTisTos import evaluateTisTos 

# Set up for the ntuple writing
import ROOT
from ROOT import *
from math import *
import GaudiPython

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
selector = getSelector(options.select)(myGlobs)

# Initialize at -1 so the eventNumber can be incremented at the top of the loop.
eventNumber = -1
while True:
    # Increment event number
    eventNumber += 1

    # Check if we've hit the maximum
    if (nEvents != -1 and eventNumber > nEvents):
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
    if not swimStripping and \
           odin.triggerConfigurationKey() != int(options.TCKid, 16):
        print "WARNING, TCK 0x%08x does not match configured TCK %s, skipping event." \
              % (odin.triggerConfigurationKey(), options.TCKid)
        continue
    
    #Safety checks
    mycands = TES[offCands+"/Particles"]
    if not mycands :
        if DEBUGMODE :
            print "Problem while processing event number",startingEvent+eventNumber
            print "No container of offline candidates found!"
            print "Skipping the event but this is bad, check what you are doing!!"   
        continue
    if mycands.size() == 0 : 
        if DEBUGMODE :
            print "Problem while processing event number",startingEvent+eventNumber
            print "Container of offline candidates empty!" 
            print "Skipping the event but this is bad, check what you are doing!!"
        continue
    if DEBUGMODE :
        print "Passed the safety checks OK"
        print "Candidates in event =", mycands.size()

    # Select the candidate to swim
    mycand = selector(mycands)
    if DEBUGMODE :
        print "Got the offline candidate OK"
        print "About to store its information"
        print mycand

    # If we are swimming the trigger and have a non-trivial selection method,
    # put the selected candidate in a separate location.
    if not swimStripping and options.select != 'none':
        swimmingParticles = createObject(SharedParticles)
        swimmingParticles.insert(mycand)
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
        runSwimmingStep(myGlobs,swimLoop)

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
    reports = TES[options.swimRepLoc]
    if not reports:
        reports = createObject(SwimmingReports)
        # Put the container in the TES
        sc = TES.registerObject(options.swimRepLoc, reports)
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
    particles = [mycand]
    appendToFSP(mycand, particles)
    for particle in particles:
        rel = relations.relations(particle)
        if rel.empty():
            relations.relate(particle, report)
        elif rel(0).to().key() != report.key():
            print "There is a relation, but it points to another swimming report, this is very bad!!"
            break

    # If swimming the stripping write out the muDST
    if swimStripping : 
        myGlobs.gaudi.algorithm("MicroDSTMainSeq").execute()
    else :
        myGlobs.gaudi.algorithm("Writer").execute()

gaudi.stop()
gaudi.finalize()
gaudi.exit()
