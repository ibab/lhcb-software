"""
High level configuration tools for Swimming
"""
__author__ = "V. Gligorov <vladimir.gligorov@cern.ch>"

import os
from LHCbKernel.Configuration import *
#from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
from Configurables import ( LHCbConfigurableUser, LHCbApp,
                            DstConf
                            #                            , CaloDstUnPackConf
                            )

from Gaudi.Configuration import *

import GaudiKernel.ProcessJobOptions

class Swimming(LHCbConfigurableUser) :
    
    __slots__ = {
        # Application Configuration : sent to LHCbApp and Gaudi
          "EvtMax"             :  -1             # Number of events to analyse
        , "SkipEvents"         :   0             # Number of events to skip at beginning for file
        , "DataType"           : ''              # Data type, can be ['DC06','2008','2009', 'MC09', '2010', '2011'] Forwarded to PhysConf. MUST be set.
        , "Simulation"         : False           # set to True to use SimCond. Forwarded to PhysConf
        , "DDDBtag"            : ""              # Tag for DDDB. Default as set in DDDBConf for DataType
        , "CondDBtag"          : ""              # Tag for CondDB. Default as set in DDDBConf for DataType
        , "RunNumber"          : 0               # Run number to be used to obtain CondDB tag, DDDB tag and TCK.
        , "TagDatabase"        : "$CHARMCONFIGROOT/db/tag_database.db" # Python shelve database to be used to obtain CondDB tag, DDDB tag and TCK from the run number.
        # Persistency
        , "Persistency"        : None            # ROOT or POOL, steers the setup of services
        , "XMLSummary"         : 'summary.xml'   # Filename for the XMLSummary disabled if empty.
        # Input
        , "Input"              : []              # Input data. Can also be passed as a second option file.
        , "InputType"          : "DST"           # or "DIGI" or "ETC" or "RDST" or "DST or "MDST" of "SDST". Nothing means the input type is compatible with being a DST. 
        # Output
        , "OutputFile"         : ""              # Name of output file
        , "OutputType"         : ""              # Type of output for swimming the stripping, DST or MDST
        # Swimming Options
        , "Debug"              : False           # Switch on debug mode
        , "OnlyEvents"         : []              # Which events to print debug info for
        , "SwimStripping"      : False           # Swim the stripping
        , "SwimOffSel"         : False           # Swim the offline selection
        , "TCK"                : ''              # The TCK to swim
        , "StrippingStream"    : ''              # The stripping stream name to swim e.g. 'CharmCompleteEvent'
        , "StrippingVersion"   : ''              # The stripping version, e.g. 'Stripping17' 
        , "StrippingLineGroup" : ''              # The group of stripping lines to swim
        , "StrippingLines"     : []             # The specific stripping lines to swim
        , "StrippingFile"      : ''              #
        , "Hlt1Triggers"       : []              # The Hlt1 triggers to swim
        , "Hlt2Triggers"       : []              # The Hlt2 triggers to swim  
        , "OffSelModuleName"   : ''              # The name of the module containing the offline selection
        , "Hlt1RecoLine"       : 'Hlt1TrackForwardPassThroughLooseDecision' # The trigger line used for Hlt1 reconstruction studies
        , "Hlt2RecoLine"       : 'Hlt2ForwardDecision'                      # The trigger line used for Hlt2 reconstruction studies                 
        , "OffCands"           : {}              # The TES location of the offline selected candidates
        , "StripCands"         : {}              # The TES locations of the stripping candidates and corresponding offline locations
        , "MuDSTCands"         : []              # The TES location of the candidates to write to MuDST
        , "OnlinePV"           : 'TES:Hlt/Vertex/PV3D'      # The TES location of the HLT primary vertices
        , "OfflinePV"          : '/Event/Rec/Vertex/Primary'# The TES location of the offline primary vertices
        , "OutPVSel"           : 'PV3D'          # The name of the HLT1 PV3D 'selection' 
        , "RelPVFinder"        : 'GenericParticle2PVRelator<_p2PVWithIPChi2,OfflineDistanceCalculatorName>/P2PVWithIPChi2' # Related PV finder
        , "DistCalc"           : 'LoKi::DistanceCalculator' # The distance calculator
        , "TauCalc"            : 'LoKi::LifetimeFitter'         # The decay time calculator
        , "LifetimeFitter"     : ['LifetimeFitter']
        , "DecayTreeFitterConstraints" : { 'DTF' : { } }     # List of particles whose masses should be constrained in the lifetime fit.
        , "RefitPVs"           : False
        , "PVRefitter"         : 'AdaptivePVReFitter/SwimmingPVReFitter'
        , "HltNodesToKill"     : ["Hlt","Hlt1","Hlt2","Trig","Raw","Rec/Vertices/Hlt2DisplVerticesV3D",
                                  "/Event/Rec/Rich/RecoEvent/Hlt2BiKalmanFittedForwardTracking_RichRecSysConf",
                                  "/Event/Rec/Rich/GlobalPID/Hlt2BiKalmanFittedForwardTracking_RichRecSysConf",   
                                  "/Event/Rec/Rich/RecoEvent/Hlt2BiKalmanFittedRichForLowPTProtonsForwardTracking_RichRecSysConf",
                                  "/Event/Rec/Rich/GlobalPID/Hlt2BiKalmanFittedRichForLowPTProtonsForwardTracking_RichRecSysConf"]#Nodes to kill HLT 
        , "MaxSwimDistance"    : 200.            # The maximum distance to swim with "fine" granularity
        , "InitialGranularity" : 4.              # The "fine" granularity with which to swim.              
        , "GranularityRefinement" : 4.           # The amount by which to refine the granularity
        , "NumberOfRefinements": 5               # The number of refinements 
        , "ExtraSwimDistance"  : 400.            # The maximum distance to swim with "coarse" granularity
        , "ExtraGranularity"   : 40.             # The "coarse" granularity with which to swim.
        , "TransformName"      : ''              # The name of the transform to use
        , "SwimmingPrefix"     : '/Event/Swimming' # The TES location of the swimming reports
        , "SelectMethod"       : 'random'        # The method for selecting offline candidates (random, first, or '')
        , "UseFileStager"      : False           # Use the file stager.
        , "WriteFSR"           : True            # Write FSRs in output DST.
        , "EventPrint"         : "L0Muon INFO - Total number of events processed : %d"
        , "SkipEventIfNoMuDSTCandFound" : False
        , "SkipEventIfNoMuDSTCandsAnywhere" : True
        , "OverrideStageName"  : ""
        , "UseCompositePIDsWhenMatching" : False
        , "StoreExtraTPs"      : False
        , "MicroDSTElements"   : []
        , "MicroDSTStreamConf" : False
        , "StrippingConfigSubstitutions" : { }
       }

    _propertyDocDct = {  
          "EvtMax"             : """ Number of events to analyse """
        , "SkipEvents"         : """ Number of events to skip at beginning for file """
        , "DataType"           : """ Data type, can be ['DC06','2008', '2009', 'MC09', '2010', '2011'] Forwarded to PhysConf, AnalysisConf and LHCbApp """
        , "Simulation"         : """ set to True to use SimCond. Forwarded to PhysConf """
        , "DDDBtag"            : """ Tag for DDDB. Default as set in DDDBConf for DataType """
        , "CondDBtag"          : """ Tag for CondDB. Default as set in DDDBConf for DataType """
        , "RunNumber"          : """ Run number to be used to retrieved CondDB tag, DDDB tag and TCK """
        , "TagDatabase"        : """ Python shelve database to be used to obtain CondDB tag, DDDB tag and TCK from the run number."""
        , "Input"              : """ Input data. Can also be passed as a second option file. """
        , "InputType"          : """ 'DST' or 'DIGI' or 'ETC' or 'RDST' or 'DST' or 'MDST' or 'SDST'. Nothing means the input type is compatible with being a DST.  """
        , "Persistency"        : """ ROOT or POOL, steers the setup of services """
        , "XMLSummary"         : """ Filename for the XMLSummary disabled if empty. """
        , "Input"              : """ Input data. Can also be passed as a second option file."""
        , "InputType"          : """ or 'DIGI' or 'ETC' or 'RDST' or 'DST' or 'MDST' of 'SDST'. Nothing means the input type is compatible with being a DST. """
        , "OutputFile"         : """ Name of output file"""
        , "OutputType"         : """ Type of output for swimming the stripping, either DST or MDST."""
        , "Debug"              : """ Switch on debug mode"""
        , "OnlyEvents"         : """ Which events to run on"""
        , "SwimStripping"      : """ Swim the stripping"""
        , "SwimOffSel"         : """ Swim the offline selection"""
        , "TCK"                : """ The TCK to swim"""
        , "StrippingStream"    : """ The stripping stream name to swim e.g. 'CharmCompleteEvent'"""
        , "StrippingVersion"   : """ The stripping version, e.g. 'Stripping17'  """
        , "StrippingLineGroup" : """ The group of stripping lines to swim"""
        , "StrippingLines"      : """ The specific stripping lines to swim"""
        , "StrippingFile"      : """   """
        , "Hlt1Triggers"       : """ The Hlt1 triggers to swim"""
        , "Hlt2Triggers"       : """ The Hlt2 triggers to swim  """
        , "OffSelModuleName"   : """ The name of the module containing the offline selection"""
        , "Hlt1RecoLine"       : """ The trigger line used for Hlt1 reconstruction studies"""
        , "Hlt2RecoLine"       : """ The trigger line used for Hlt2 reconstruction studies                 """
        , "OffCands"           : """ The TES locations of the offline selected candidates -- { offline loc : prefix for P2TP }"""
        , "StripCands"         : """ The TES locations of the stripping candidates and corresponding offline locations -- { offline loc : [ strip locs ] }"""
        , "MuDSTCands"         : """ The TES locations of extra candidates to write to MuDST"""
        , "OnlinePV"           : """ The TES location of the HLT primary vertices"""
        , "OfflinePV"          : """ The TES location of the offline primary vertices"""
        , "OutPVSel"           : """ The name of the HLT1 PV3D 'selection' """
        , "RelPVFinder"        : """ Related PV finder"""
        , "DistCalc"           : """ The distance calculator"""
        , "TauCalc"            : """ The decay time calculator"""
        , "LifetimeFitter"     : """ Must be either LifetimeFitter or DecayTreeFitter."""
        , "DecayTreeFitterConstraints" : """ List of particles whose masses should be constrained in the lifetime fit. """
        , "RefitPVs"           : """ Refit related PVs. """
        , "PVRefitter"         : """ Tool to use to refit the PV. """
        , "HltNodesToKill"     : """ Which TES nodes must be killed to rerun the HLT multiple times for the same event """
        , "MaxSwimDistance"    : """ The maximum distance to swim with "fine" granularity"""
        , "InitialGranularity" : """ The "fine" granularity with which to swim.              """
        , "GranularityRefinement" : """ The amount by which to refine the granularity"""
        , "NumberOfRefinements": """ The number of refinements """
        , "ExtraSwimDistance"  : """ The maximum distance to swim with "coarse" granularity"""
        , "ExtraGranularity"   : """ The "coarse" granularity with which to swim."""
        , "TransformName"      : """ The name of the transform to use"""
        , "SwimmingPrefix"     : """ The TES prefix used for swimming objects."""
        , "SelectMethod"       : """ The method for selecting offline candidates (random, first, or '')"""
        , "UseFileStager"      : """ Use the file stager. """
        , "WriteFSR"           : """ Write FSRs in output DST. """
        , "EventPrint"         : """ The string to be printed at the end of the job. Should contain once %d """
        , "SkipEventIfNoMuDSTCandFound" : """ If this is True (default is False) not only the OffCands location but also the MuDSTCands location must contain candidates for the event to be processed """
        , "SkipEventIfNoMuDSTCandsAnywhere" : """ No effect if SkipEventIfNoMuDSTCandFound is False. If True (default) then SkipEventIfNoMuDSTCandFound requires *all* locations in MuDSTCands to contain \
            an event, if False then only one location must contain a candidate for the event to be processed. """
        , "OverrideStageName" : """ If not None (default) then override the stage name under which the swimming reports are saved from the default of 'Stripping' or 'Trigger'. For TupleToolSwimmingInfo \
            compatibility 'Trigger_XXX' style names are advised. """
        , "UseCompositePIDsWhenMatching" : """ Include the PID of composite particles when matching candidates (e.g. offline with stripping) """
        , "StoreExtraTPs"     : """ If True (default: False) then each swimming stage will evaluate the candidate info (lifetime etc.) and decision at the RAW values already saved in the Swimming report. """
        , "MicroDSTElements"  : """ List, default []: if not empty then override the list of MicroDST writer elements. """
        , "MicroDSTStreamConf": """ Override the MicroDST output stream configuration. """
        , "StrippingConfigSubstitutions" : """ Dictionary which can be used to tweak the Stripping configuration. This is useful for swimming filtered MC, where the stripping cuts are sometimes modified. """
        }

    __used_configurables__ = [
        DstConf,
        #CaloDstUnPackConf,
        LHCbApp
        ]

################################################################################
# Apply configuration
#
    def __apply_configuration__(self):
        """
        Swimming configuration
        """
        log.info("Applying Swimming configuration")
        log.info( self )

        if not self.getProp('SwimStripping') and not self.getProp('TransformName'):
            raise TypeError, "No or invalid HLT transform specified when trying to swim the trigger!"
        if not self.getProp('SwimStripping') and self.getProp('SwimOffSel'):
            raise TypeError, "You cannot swim the trigger and offline selection in the same job!"
                
        if self.getProp('SelectMethod') not in ['random', 'first', 'all', 'none']:
            raise TypeError, 'The selectMethod must be eiter "random", "first", "all" or "none".'

        if type(self.getProp('OffCands')) != dict:
            raise TypeError, 'OffCands must be a dict'
        if type(self.getProp('StripCands')) != dict:
            raise TypeError, 'StripCands must be a dict'
        for key, value in self.getProp('StripCands').iteritems():
            if type(value) != list or not isinstance(key, basestring):
                raise TypeError, 'StripCands must be a { string : [ ] } dict'
        if type(self.getProp('MuDSTCands')) != list:
            raise TypeError, 'MuDSTCands must be a list'
        if type(self.getProp('StrippingLines')) != list:
            raise TypeError, 'StrippingLines must be a list'
        if type(self.getProp('MicroDSTElements')) != list:
            raise TypeError, 'MicroDSTElements must be a list'

        if not self.getProp('OutputType') in ['DST', 'MDST']:
            raise TypeError, "The only supported output types are DST and MDST."
        if not self.getProp('SwimStripping') and self.getProp('OutputType') == 'MDST':
            raise TypeError, "You cannot write a MicroDST when swimming the trigger."

        if type(self.getProp('LifetimeFitter')) != list:
            raise TypeError, "LifetimeFitter must be a list of strings."
        for lifetimefitter in self.getProp('LifetimeFitter'):
            if lifetimefitter not in ['LifetimeFitter', 'DecayTreeFitter']:
                raise TypeError, "Allowed LifetimeFitters are LifetimeFitter and DecayTreeFitter."
            
        if type(self.getProp('DecayTreeFitterConstraints')) != dict:
            raise TypeError, "DecayTreeFitterConstraints must be a dict"
               
        extension = self.getProp("OutputFile").rsplit(os.path.extsep, 1)[-1]
        if extension.upper() != self.getProp('OutputType'):
            log.warning("You have specified a different output file extension " +
                        "than OutputType; this is ignored.")
        
        from Configurables import DataOnDemandSvc

        app = LHCbApp()
        self.setOtherProps(app, ['EvtMax', 'SkipEvents', 'Simulation', 'DataType',
                                 'Persistency'])

        # Configure XMLSummarySvc
        if self.getProp('XMLSummary'):
            app.XMLSummary = self.getProp('XMLSummary')
            from Configurables import XMLSummarySvc
            XMLSummarySvc('CounterSummarySvc').EndEventIncident = 'SwimmingEndEvent'

        DstConf           ( EnableUnpack = ["Reconstruction","Stripping"] )

        # TODO check if we need: CaloDstUnPackConf ( Enable = True )

        importOptions("$STDOPTS/DecodeRawEvent.py")
        appConf = ApplicationMgr()
        appConf.HistogramPersistency = 'ROOT'
        appConf.ExtSvc += ['DataOnDemandSvc']
        EventSelector().PrintFreq = -1
        EventSelector().OutputLevel = 6 
        if not (self.getProp('Input') == []) :
            from GaudiConf import IOHelper
            IOHelper('ROOT').inputFiles(self.getProp('Input'))

        # Swimming::Service
        ApplicationMgr().ExtSvc += ['Swimming::Service']
        from Configurables import Swimming__Service as Service
        Service().RefitPVs = self.getProp('RefitPVs')
        Service().OfflinePVLocation = self.getProp('OfflinePV')

        # FileStager
        if self.getProp('UseFileStager'):
            from FileStager.Configuration import configureFileStager
            configureFileStager()
        IODataManager().UseGFAL=False

        # Disable muons in TisTosTool
        from Configurables import TriggerTisTos
        ToolSvc().addTool(TriggerTisTos,'TriggerTisTos')
        ToolSvc().TriggerTisTos.TOSFracMuon = 0.
        # Disable TT hits also
        ToolSvc().TriggerTisTos.TOSFracTT = 0.

def ConfigureMoore():
    config = Swimming()
    from Swimming.HltTransforms import getTransform
    thisTransform = getTransform(config.getProp('TransformName'),
                                 config.getProp('Hlt1Triggers') + config.getProp('Hlt2Triggers'))

    from Configurables import (HltConfigSvc, EventNodeKiller,
                               HltMoveVerticesForSwimming, Moore)

    #Global configuration
    mykiller    = EventNodeKiller("killHlt")
    mykiller.Nodes          = config.getProp('HltNodesToKill')
    deathstar               = GaudiSequencer("killHltSeq")
    deathstar.Members       = [mykiller]
    from Swimming import MooreSetup
    #

    dddb = config.getProp('DDDBtag')
    conddb = config.getProp('CondDBtag')
    tck = config.getProp('TCK')
    run = config.getProp('RunNumber')
    if not dddb and not conddb and not tck and run:
        import shelve 
        tag_db = os.path.expandvars(config.getProp('TagDatabase'))
        if not os.path.exists(tag_db):
            raise OSError, "Tag database file %s does not exist" % config.getProp('TagDatabase')
        tag_db = shelve.open(tag_db, 'r')
        info = tag_db['info']
        tags = info[run]
        Moore().DDDBtag = tags['DDDBtag']
        Moore().CondDBtag = tags['CondDBtag']
        Moore().InitialTCK = tags['TCK']
        Swimming().TCK = tags['TCK']
    elif dddb and conddb and tck and not run:
        Moore().DDDBtag     = dddb
        Moore().CondDBtag   = conddb
        Moore().InitialTCK  = tck
    else:
        raise TypeError, 'You must specify either the CondDB tag, DDDB tag and TCK and _not_ the run number' + \
              ' or only the run number.'

    Moore().Simulation  = config.getProp('Simulation')
    Moore().DataType    = config.getProp('DataType')
    Moore().outputFile  = config.getProp('OutputFile')
    Moore().WriteFSR    = config.getProp('WriteFSR')
    Moore().Persistency = config.getProp('Persistency')
    Moore().WriterRequires = []
    # Add extra locations to writer
    from Configurables import InputCopyStream
    writer = InputCopyStream('Writer')
    writer.ItemList = [config.getProp('SwimmingPrefix') + '/Reports#1']
    writer.OptItemList = list(set([l  + '/P2TPRelations#1' for l in config.getProp('OffCands').values() ]))
    
    #
    # Define the TCK transformation
    # 
    HltConfigSvc().ApplyTransformation = thisTransform
    from pprint import pprint
    pprint( HltConfigSvc().ApplyTransformation )
    #
    # Define the swimming algorithm
    #
    myswimmer                   = HltMoveVerticesForSwimming("HltMovePVs4Swimming")
    myswimmer.SwimmingDistance  = 0.0
    loc = config.getProp('SwimmingPrefix') # TODO check differences with trunk more carefully
    myswimmer.Bcontainer        = loc
    myswimmer.InputSelection    = config.getProp('OnlinePV')
    myswimmer.OutputSelection   = config.getProp('OutPVSel')
    myswimmer.OutputLevel       = 4

    # Configure an extra TisTos Tool and some decoder algos to debug TisTos issues
    prefix = config.getProp('SwimmingPrefix')
    from Configurables import HltDecReportsDecoder, HltSelReportsDecoder
    decoders = [(HltDecReportsDecoder, [('OutputHltDecReportsLocation', 'Hlt/DecReports')]),
                (HltSelReportsDecoder, [('OutputHltSelReportsLocation', 'Hlt/SelReports'),
                                        ('HltDecReportsLocation', 'Hlt/DecReports')])]
    from Configurables import TriggerTisTos
    ToolSvc().addTool(TriggerTisTos, 'SwimmingDebugTisTos')
    ToolSvc().SwimmingDebugTisTos.TOSFracMuon = 0.0
    ToolSvc().SwimmingDebugTisTos.TOSFracTT   = 0.0
    for conf, d in decoders:
        configurable = conf('Swimming' + d[0][1].split('/')[-1])
        print configurable
        try:
            configurable.InputRawEventLocation = 'PrevTrig/RawEvent'
        except:
            configurable.RawEventLocations = [ 'PrevTrig/RawEvent' ] + configurable.RawEventLocations
        output = None
        for prop, loc in d:
            if not output: output = prefix + '/' + loc
            setattr(configurable, prop, prefix + '/' + loc)
        DataOnDemandSvc().AlgMap[output] = configurable
        prop = d[0][0][6:]
        print prop, output
        setattr(ToolSvc().SwimmingDebugTisTos, prop, output)

    class Deathstar(object):
        def __init__(self, seq):
            self._seq = seq
        def insert(self):
            ApplicationMgr().TopAlg.insert(0, self._seq)

    d = Deathstar(deathstar)
    appendPostConfigAction(d.insert)

    # recombine raw event stuff was here in trunk -- it would actually work in future so leaving a comment here.
    
def ConfigureDaVinci():
    config = Swimming()
    from Configurables import DaVinci
    from StrippingConf.Configuration import StrippingConf
    from Configurables import ProcStatusCheck
    from Configurables import EventNodeKiller,GaudiSequencer
    from PhysSelPython.Wrappers import (AutomaticData, SelectionSequence,
                                        MultiSelectionSequence)

    # Get the stripping line
    from StrippingSettings.Utils import lineBuilderConfiguration
    strippingFile = None
    if config.getProp('StrippingFile') != 'none':
        strippingFile = config.getProp('StrippingFile')
    else:
        strippingFile = config.getProp('StrippingLineGroup')
    myconfig = lineBuilderConfiguration(config.getProp('StrippingVersion'),
                                        config.getProp('StrippingLineGroup'))
    import StrippingArchive
    mylineconf = getattr(__import__('StrippingArchive.'+config.getProp('StrippingVersion')+'.Stripping' + strippingFile,
                                    globals(),locals(),
                                    [myconfig["BUILDERTYPE"]],-1),myconfig["BUILDERTYPE"])
    mylinedict = myconfig["CONFIG"]
    substitutions = config.getProp('StrippingConfigSubstitutions')
    print "mylinedict before substitutions:",mylinedict
    print "stripping config substitutions:",substitutions
    mylinedict.update(substitutions)
    print "mylineconf:",mylineconf
    print "mylinedict after substitutions:",mylinedict

    from StrippingConf.StrippingStream import StrippingStream
    stream = StrippingStream(config.getProp('StrippingStream') + "Swimming")
    allLines = mylineconf(config.getProp('StrippingLineGroup'), mylinedict).lines()
    lines = []
    #lineNames = [l.split('/')[-1] for l in config.getProp('StripCands').keys()]
    lineNames = config.getProp('StrippingLines')
    print "lineNames:",lineNames
    for l in allLines:
        for lineName in lineNames:
            if l.outputLocation().find(lineName) != -1:
                lines.append(l)
                print l.outputLocation()
    stream.appendLines(lines)

    # Define the stream
    filterBadEvents = ProcStatusCheck()
    sc = StrippingConf( Streams = [stream],
                        MaxCandidates       = 2000,
                        AcceptBadEvents     = False,
                        BadEventSelection   = filterBadEvents )

    # Define the node killer, and make sure to kill everything corresponding to
    # the stream which we want to swim
    outputs = [] 
    from Configurables import Swimming__PVReFitter as ReFitter
    for l in lines:
        for f in l.filterMembers():
            if hasattr(f, 'ReFitPVs') and f.ReFitPVs:
                if not config.getProp('RefitPVs'):
                    log.warning('RefitPVs is not set, but stripping line applies refitting. Refitted ' + \
                                'PVs will be used for turning-point lifetime calculations.')
                    config.setProp('RefitPVs', True)
                t = f.PVReFitters['']
                f.PVReFitters = {'' : 'Swimming::PVReFitter/PVReFitter'}
                f.addTool(ReFitter, 'PVReFitter')
                f.PVReFitter.PVReFitter = t
            elif not hasattr(f, 'Output'):
                continue
            # Remove the last item so we get everything (Particle, relations,
            # decayVertices, etc...
            o = '/'.join(f.Output.split('/')[:-1])
            outputs.append(o)
    print "Outputs are",outputs
    mykiller    = EventNodeKiller("killStripping")
    # Some default nodes which we will want to kill in all cases
    nodestokill = outputs + ['Strip', '/Event/Rec/Vertex/Primary']
    mykiller.Nodes          = nodestokill 
    deathstar               = GaudiSequencer("killStrippingSeq")
    deathstar.Members       = [mykiller]

    # Configure DaVinci
    DaVinci().InputType   = config.getProp('InputType')
    DaVinci().DataType    = config.getProp('DataType')
    DaVinci().Simulation  = config.getProp('Simulation')
    DaVinci().DDDBtag     = config.getProp('DDDBtag')
    DaVinci().CondDBtag   = config.getProp('CondDBtag')
    try:
        DaVinci().Persistency = config.getProp('Persistency')
    except AttributeError:
        print "DaVinci doesn't have a Persistency attribute to set"
    
    # The sequence for the swimming has to be configured 
    # by hand inserting the node killer before it
    DaVinci().appendToMainSequence( [deathstar] )
    DaVinci().appendToMainSequence( [ sc.sequence() ] )

    # Since the name of the output file is configured in two places in DaVinci,
    # do some splitting.
    splitName = config.getProp('OutputFile').split(os.path.extsep)
    seqName = ''
    prefix = ''
    if len(splitName) <= 2:
        seqName = splitName[0]
        print "Warning, an output filename in three parts was specified. This does not work well, " + \
              " so 'Swimming.' will be prefixed."
        prefix = 'Swimming'
    else:
        prefix = splitName[0]
        seqName = os.path.extsep.join(splitName[1 : -1])

    dstWriter = None
    print config.getProp('OutputType')

    # Offline candidate selection sequences
    sequences = []
    offCands = config.getProp('OffCands').keys()
    for i, cands in enumerate(offCands):
        data = AutomaticData(Location =  cands + "/Particles")
        offSeq = SelectionSequence("OfflineCandidates_%d" % i, TopSelection = data)
        sequences.append(offSeq)

    # selection sequence for offline candidates
    muCands = config.getProp('MuDSTCands')
    for i, cands in enumerate(muCands):
        # Add extra selections for additional MuDSTCands
        data = AutomaticData(Location = cands + "/Particles")
        seq = SelectionSequence("MuDSTCands_%d" % i, TopSelection = data)
        sequences.append(seq)
        
    selectionSeq = MultiSelectionSequence(seqName, Sequences = sequences)

    if config.getProp('OutputType') == 'MDST':
        pack = False
        isMC = config.getProp("Simulation")
        SwimmingConf = config.getProp('MicroDSTStreamConf')
        SwimmingElements = config.getProp('MicroDSTElements')
        if SwimmingConf == False:
            from DSTWriters.Configuration import stripMicroDSTStreamConf
            SwimmingConf = stripMicroDSTStreamConf(pack = pack, isMC = isMC)
        if len(SwimmingElements) == 0:
            from DSTWriters.Configuration import stripMicroDSTElements
            from DSTWriters.microdstelements import CloneSwimmingReports, CloneParticleTrees, CloneTPRelations
            mdstElements = stripMicroDSTElements(pack = pack, isMC = isMC)
            SwimmingElements = [ CloneSwimmingReports() ]
            for element in mdstElements:
              SwimmingElements += [ element ]
              if type(element) == CloneParticleTrees:
                  SwimmingElements += [ CloneTPRelations("P2TPRelations") ]

        streamConf = { 'default' : SwimmingConf }
        elementsConf = { 'default' : SwimmingElements }
        try:
            from DSTWriters.__dev__.Configuration import MicroDSTWriter
        except:
            from DSTWriters.Configuration import MicroDSTWriter
        dstWriter = MicroDSTWriter('MicroDST',
                                   StreamConf         = streamConf,
                                   MicroDSTElements   = elementsConf,
                                   WriteFSR           = config.getProp('WriteFSR'),
                                   OutputFileSuffix   = prefix,
                                   SelectionSequences = [selectionSeq])
    elif config.getProp('OutputType') == 'DST':
        try :
            from DSTWriters.__dev__.streamconf import OutputStreamConf
            from DSTWriters.__dev__.Configuration import SelDSTWriter
        except ImportError :
            from DSTWriters.streamconf import OutputStreamConf
            from DSTWriters.Configuration import SelDSTWriter

        streamConf = OutputStreamConf(streamType = InputCopyStream,
                                      fileExtension = '.dst',
                                      extraItems = [config.getProp('SwimmingPrefix') + '/Reports#1'] +\
                                      list(set([l + '/P2TPRelations#1' for l in config.getProp('OffCands').values()])))
        SelDSTWriterElements = {'default' : []}
        SelDSTWriterConf = {'default' : streamConf}
        dstWriter = SelDSTWriter('FullDST',
                                 StreamConf         = SelDSTWriterConf,
                                 MicroDSTElements   = SelDSTWriterElements,
                                 WriteFSR           = config.getProp('WriteFSR'),
                                 OutputFileSuffix   = prefix,
                                 SelectionSequences = [selectionSeq])

    DaVinci().appendToMainSequence([dstWriter.sequence()])
