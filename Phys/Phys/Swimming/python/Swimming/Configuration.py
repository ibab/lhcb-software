"""
High level configuration tools for Swimming
"""
__author__ = "V. Gligorov <vladimir.gligorov@cern.ch>"

import os
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer
from Configurables import ( LHCbConfigurableUser, LHCbApp,
                            DstConf, CaloDstUnPackConf )

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
        , "SwimStripping"      : False           # Swim the stripping
        , "SwimOffSel"         : False           # Swim the offline selection
        , "TCK"                : ''              # The TCK to swim
        , "StrippingStream"    : ''              # The stripping stream name to swim e.g. 'CharmCompleteEvent'
        , "StrippingVersion"   : ''              # The stripping version, e.g. 'Stripping17'  
        , "StrippingLineGroup" : ''              # The group of stripping lines to swim
        , "StrippingLine"      : ''              # The specific single stripping line to swim
        , "StrippingFile"      : ''              #
        , "Hlt1Triggers"       : []              # The Hlt1 triggers to swim
        , "Hlt2Triggers"       : []              # The Hlt2 triggers to swim  
        , "OffSelModuleName"   : ''              # The name of the module containing the offline selection
        , "Hlt1RecoLine"       : 'Hlt1TrackForwardPassThroughLooseDecision' # The trigger line used for Hlt1 reconstruction studies
        , "Hlt2RecoLine"       : 'Hlt2ForwardDecision'                      # The trigger line used for Hlt2 reconstruction studies                 
        , "OffCands"           : ''              # The TES location of the offline selected candidates
        , "StripCands"         : ''              # The TES location of the stripping candidates
        , "MuDSTCands"         : []              # The TES location of the candidates to write to MuDST
        , "OnlinePV"           : 'TES:Hlt/Vertex/PV3D'      # The TES location of the HLT primary vertices
        , "OfflinePV"          : '/Event/Rec/Vertex/Primary'# The TES location of the offline primary vertices
        , "OutPVSel"           : 'PV3D'          # The name of the HLT1 PV3D 'selection' 
        , "RelPVFinder"        : 'GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_/P2PVWithIPChi2' # Related PV finder
        , "DistCalc"           : 'LoKi::DistanceCalculator' # The distance calculator
        , "TauCalc"            : 'PropertimeFitter'         # The decay time calculator
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
        , "SwimStripping"      : """ Swim the stripping"""
        , "SwimOffSel"         : """ Swim the offline selection"""
        , "TCK"                : """ The TCK to swim"""
        , "StrippingStream"    : """ The stripping stream name to swim e.g. 'CharmCompleteEvent'"""
        , "StrippingVersion"   : """ The stripping version, e.g. 'Stripping17'  """
        , "StrippingLineGroup" : """ The group of stripping lines to swim"""
        , "StrippingLine"      : """ The specific single stripping line to swim"""
        , "StrippingFile"      : """   """
        , "Hlt1Triggers"       : """ The Hlt1 triggers to swim"""
        , "Hlt2Triggers"       : """ The Hlt2 triggers to swim  """
        , "OffSelModuleName"   : """ The name of the module containing the offline selection"""
        , "Hlt1RecoLine"       : """ The trigger line used for Hlt1 reconstruction studies"""
        , "Hlt2RecoLine"       : """ The trigger line used for Hlt2 reconstruction studies                 """
        , "OffCands"           : """ The TES location of the offline selected candidates"""
        , "StripCands"         : """ The TES location of the stripping candidates"""
        , "MuDSTCands"         : """ The TES locations of extra candidates to write to MuDST"""
        , "OnlinePV"           : """ The TES location of the HLT primary vertices"""
        , "OfflinePV"          : """ The TES location of the offline primary vertices"""
        , "OutPVSel"           : """ The name of the HLT1 PV3D 'selection' """
        , "RelPVFinder"        : """ Related PV finder"""
        , "DistCalc"           : """ The distance calculator"""
        , "TauCalc"            : """ The decay time calculator"""
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
        }

    __used_configurables__ = [
        DstConf,
        CaloDstUnPackConf,
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
                
        if self.getProp('SelectMethod') not in ['random', 'first', 'none']:
            raise TypeError, 'The selectMethod must be eiter "random", "first", or "none".'

        if not self.getProp('OutputType') in ['DST', 'MDST']:
            raise TypeError, "The only supported output types are DST and MDST."
        if not self.getProp('SwimStripping') and self.getProp('OutputType') == 'MDST':
            raise TypeError, "You cannot write a MicroDST when swimming the trigger."
               
        if type(self.getProp('MuDSTCands')) != list:
            raise TypeError, "MuDSTCands must be a list."

        extension = self.getProp("OutputFile").rsplit(os.path.extsep, 1)[-1]
        if extension.upper() != self.getProp('OutputType'):
            log.warning("You have specified a different output file extension " +
                        "than OutputType; this is ignored.")
        
        from Configurables import DataOnDemandSvc,PhysConf

        app = LHCbApp()
        self.setOtherProps(app, ['EvtMax', 'SkipEvents', 'Simulation', 'DataType',
                                 'Persistency'])
        # Configure XMLSummarySvc
        if self.getProp('XMLSummary'):
            app.XMLSummary = self.getProp('XMLSummary')
            from Configurables import XMLSummarySvc
            XMLSummarySvc('CounterSummarySvc').EndEventIncident = 'SwimmingEndEvent'
            
        DstConf           ( EnableUnpack = ["Reconstruction","Stripping"] ) 

        importOptions("$STDOPTS/DecodeRawEvent.py")
        appConf = ApplicationMgr()
        appConf.HistogramPersistency = 'ROOT'
        appConf.ExtSvc += ['DataOnDemandSvc']
        EventSelector().PrintFreq = -1
        EventSelector().OutputLevel = 6
        if not (self.getProp('Input') == []) :
            from Gaudi.Configuration import * 
            from GaudiConf import IOHelper
            IOHelper('ROOT').inputFiles(self.getProp('Input'))

        # FileStager
        if self.getProp('UseFileStager'):
            from FileStager.Configuration import configureFileStager
            configureFileStager()
        IODataManager().UseGFAL=False

        # Disable muons in TisTosTool
        from Configurables import TriggerTisTos
        ToolSvc().addTool(TriggerTisTos,'TriggerTisTos')
        ToolSvc().TriggerTisTos.TOSFracMuon = 0.


def ConfigureMoore():
    config = Swimming()
    from Swimming.HltTransforms import getTransform
    thisTransform = getTransform(config.getProp('TransformName'),
                                 config.getProp('Hlt1Triggers') + config.getProp('Hlt2Triggers'))

    from Configurables import HltMoveVerticesForSwimming
    from Configurables import Moore,HltConfigSvc

    #Global configuration
    from Configurables import EventNodeKiller
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
    writer.ItemList = [config.getProp('OffCands') + '/P2TPRelations#1',
                       config.getProp('SwimmingPrefix') + '/Reports#1']
    
    #
    # Define the TCK transformation
    # 
    HltConfigSvc().ApplyTransformation = thisTransform
    #
    # Define the swimming algorithm
    #
    myswimmer                   = HltMoveVerticesForSwimming("HltMovePVs4Swimming")
    myswimmer.SwimmingDistance  = 0.0
    loc = None
    if config.getProp('SelectMethod') == 'none':
        loc = config.OffCands 
    else:
        loc = config.getProp('SwimmingPrefix')
    myswimmer.Bcontainer        = loc
    myswimmer.InputSelection    = config.getProp('OnlinePV')
    myswimmer.OutputSelection   = config.getProp('OutPVSel')
    myswimmer.OutputLevel       = 4

    class Deathstar(object):
        def __init__(self, seq):
            self._seq = seq
        def insert(self):
            ApplicationMgr().TopAlg.insert(0, self._seq)

    d = Deathstar(deathstar)
    appendPostConfigAction(d.insert)
   
    # Need to rebuild the raw event which is in pieces on the DST
    from Configurables import RecombineRawEvent
    RecombineRawEvent()
 
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
    print mylineconf
    print mylinedict

    from StrippingConf.StrippingStream import StrippingStream
    stream = StrippingStream(config.getProp('StrippingStream') + "Swimming")
    allLines = mylineconf(config.getProp('StrippingLineGroup'), mylinedict).lines()
    lines = []
    for l in allLines:
        lineName = config.getProp('StrippingLine')
        if l.outputLocation().find(lineName) != -1:
            lines.append(l)
            print l.outputLocation()
    stream.appendLines(lines)

    # Define the stream
    filterBadEvents = ProcStatusCheck()
    sc = StrippingConf( Streams = [stream],
                        MaxCandidates       = 2000,
                        AcceptBadEvents     = False,
                        BadEventSelection   = filterBadEvents)

    # Define the node killer, and make sure to kill everything corresponding to
    # the stream which we want to swim
    outputs = []
    for l in lines:
        for f in l.filterMembers():
            if not hasattr(f, 'Output'):
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
    DaVinci().Persistency = config.getProp('Persistency')
    
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
    if config.getProp('OutputType') == 'MDST':
        # Try the dev version, if not...
        try :
            from DSTWriters.__dev__.Configuration import MicroDSTWriter, microDSTStreamConf
            from DSTWriters.__dev__.microdstelements import (CloneRecHeader,
                                                         CloneODIN,
                                                         ClonePVs,
                                                         CloneSwimmingReports,
                                                         CloneParticleTrees,
                                                         ClonePVRelations,
                                                         CloneTPRelations,
                                                         ReFitAndClonePVs,
                                                         CloneLHCbIDs,
                                                         CloneRawBanks)
        except ImportError :
            from DSTWriters.Configuration import MicroDSTWriter, microDSTStreamConf
            from DSTWriters.microdstelements import (CloneRecHeader,
                                                         CloneODIN,
                                                         ClonePVs,
                                                         CloneSwimmingReports,
                                                         CloneParticleTrees,
                                                         ClonePVRelations,
                                                         CloneTPRelations,
                                                         ReFitAndClonePVs,
                                                         CloneLHCbIDs,
                                                         CloneRawBanks)
        # selection sequence for offline candidates
        offData = AutomaticData(Location = config.getProp('OffCands') + "/Particles")
        offSeq = SelectionSequence("OfflineCandidates", TopSelection = offData)
        sequences = [offSeq]
        muCands = config.getProp('MuDSTCands')
        for i, cands in enumerate(muCands):
            # Add extra selections for additional MuDSTCands
            data = AutomaticData(Location = cands + "/Particles")
            seq = SelectionSequence("MuDSTCands_%d" % i, TopSelection = data)
            sequences.append(seq)
        selectionSeq = MultiSelectionSequence(seqName, Sequences = sequences)

        SwimmingConf = microDSTStreamConf()
        streamConf = { 'default' : SwimmingConf }
        SwimmingElements = [ CloneRecHeader(),
                             CloneODIN(),
                             ClonePVs(),
                             CloneSwimmingReports(),
                             CloneParticleTrees(),
                             ClonePVRelations("Particle2VertexRelations", True),
                             CloneTPRelations("P2TPRelations", True),
                             ReFitAndClonePVs(),
                             CloneLHCbIDs(),
                             CloneRawBanks(banks = ['HltSelReports', 'HltDecReports',
                                                    'L0DU', 'L0Calo', 'L0CaloError', 'L0CaloFull',
                                                    'L0Muon', 'L0MuonProcCand', 'L0MuonError'],
                                           inputRawEvent = 'DAQ/RawEvent')]
        elementsConf = { 'default' : SwimmingElements }
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
        # Output
        inputData = AutomaticData(Location = config.getProp('OffCands') + "/Particles")
        selectionSeq = SelectionSequence(seqName, TopSelection = inputData)

        streamConf = OutputStreamConf(streamType = InputCopyStream,
                                      fileExtension = '.dst',
                                      extraItems = [config.getProp('OffCands') + '/P2TPRelations#1',
                                                    config.getProp('SwimmingPrefix') + '/Reports#1'])
        SelDSTWriterElements = {'default' : []}
        SelDSTWriterConf = {'default' : streamConf}
        dstWriter = SelDSTWriter('FullDST',
                                 StreamConf         = SelDSTWriterConf,
                                 MicroDSTElements   = SelDSTWriterElements,
                                 WriteFSR           = config.getProp('WriteFSR'),
                                 OutputFileSuffix   = prefix,
                                 SelectionSequences = [selectionSeq])

    DaVinci().appendToMainSequence([dstWriter.sequence()])
