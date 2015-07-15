## @package Escher
#  High level configuration tools for Escher
#  @author Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.22 2010-10-08 07:48:43 wouter Exp $"
__author__  = "Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>"

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp, RecSysConf, TrackSys,
                            ProcessPhase, GaudiSequencer, DstConf, TAlignment,
                            VeloAlignment, DecodeRawEvent,
                            CountingPrescaler, RecMoniConf, CondDB )

## @class Escher
#  Configurable for Escher application
#  @author Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>
#  @date   15/08/2008
class Escher(LHCbConfigurableUser):

    ##list of ConfigurableUser classes this one is going to query in the apply_configuration method 
    __queried_configurables__ = [ ]

    ##list of ConfigurableUser classes this one is going to modify in the apply_configuration method. 
    __used_configurables__ = [ TrackSys, RecSysConf, RecMoniConf, LHCbApp, DstConf, TAlignment, DecodeRawEvent, CondDB ]
    
    ## Default main sequences for real and simulated data
    DefaultSequence = [ #CountingPrescaler("EscherPrescaler")
                        #,
                        "ProcessPhase/Init"
                        , GaudiSequencer("HltFilterSeq")
			, "ProcessPhase/Reco"
			, "ProcessPhase/Moni"
                        , GaudiSequencer("AlignSequence")
			]

    KnownSpecialData = []

    DefaultRecoSequences = {'Run1':["Decoding"]+RecSysConf().DefaultTrackingSubdets,
                            'Run2':["Decoding"]+RecSysConf().DefaultTrackingSubdetsRun2}


    # Steering options
    __slots__ = {
         "EvtMax"		: -1       # Maximum number of events to process
       , "SkipEvents"		: 0        # events to skip
       , "PrintFreq"		: 100      # The frequency at which to print event numbers
       , "DataType"   		: "2011"   # Data type, can be ['DC06','2008']
       , "WithMC"		: False    # set to True to use MC truth
       , "Simulation"		: False    # set to True to use SimCond
       , "InputType"		: "DST"    # or "DIGI" or "ETC" or "RDST" or "DST"
       , "OutputType"		: "NONE"   # or "RDST" or "NONE". Also forwarded to RecSys
       , "GlobalCuts"		: {}       # global event cuts for tracking
       , "Persistency"          : None     # POOL or ROOT foraward to LHCbApp
       , "PackType"		: "TES"    # Flag whether or not to use packed containers
       , "NoWarnings"		: False    # suppress all messages with MSG::WARNING or below
       , "DatasetName"		: ""       # string used to build file names
       , "DDDBtag"		: ""       # Tag for DDDB. Default as set in DDDBConf for DataType
       , "CondDBtag"		: ""       # Tag for CondDB. Default as set in DDDBConf for DataType
       , "UseOracle"		: False    # if False, use SQLDDDB instead
       , "MainSequence"		: []       # The default main sequence, see self.DefaultSequence
       , "InitSequence"		: ["Escher"] # default init sequence
       , "AlignSequence"	: []
       , "Kalman" 		: True    # run the kalman filter type alignment
       , "Millepede"		: False    # run the Millepede type alignment
       , "OutputLevel" 		: 3        #
       , "Incident"     	:  ""      # for Millepede style alignment, there are two incident handles: GlobalMPedeFit and Converged
                                           # for Kalman style alignment, there is a handle: UpdateConstants.
        # Following are options forwarded to RecSys
#       , "RecoSequence"   	: ["Decoding","VELO","Tr","Vertex"] # The Sub-detector reconstruction sequencing. See RecSys for default
       , "RecoSequence"   	: [] # The Sub-detector reconstruction sequencing. See RecSys for default  
       , "MoniSequence"         : ["VELO","Tr", "OT","ST"]
       , "SpecialData"    	: [] # Various special data processing options. See KnownSpecialData for all options
       , "Context"		: "Offline" # The context within which to run
       , "WriteFSR"             : True #write FSRs to DSTs
       , "UseFileStager"        : False
       , "ExpertTracking"       : [ "simplifiedGeometry"]
       , "HltFilterCode"        : None # Loki filter on Hlt decision
       , "OnlineMode"           : False
       , "UseDBSnapshot"        : False
       , "PartitionName"        : "LHCb"
       , "DBSnapshotDirectory"  : "/group/online/hlt/conditions"
       , "OnlineAligWorkDir"    : "/group/online/AligWork/running"
       , "Upgrade"              : False
        }

    def defineGeometry(self):
        # DIGI is always simulation, as is usage of MC truth!
        if self.getProp( "WithMC" ) or self.getProp( "InputType" ).upper() == 'DIGI':
            self.setProp( "Simulation", True )

        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["DataType","CondDBtag","DDDBtag","Simulation"])
        # specify the use of the oracle database
        if self.getProp("UseOracle"):
            from Configurables import CondDB
            CondDB(UseOracle = True)

    def defineEvents(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])

    def defineOptions(self):
        log.info("Defining options!")

        inputType = self.getProp( "InputType" ).upper()
        if inputType not in [ "MDF", "DST", "DIGI", "ETC", "RDST" ]:
            raise TypeError( "Invalid inputType '%s'"%inputType )

        outputType = self.getProp( "OutputType" ).upper()
        if outputType not in [ "NONE", "DST", "RDST" ]:
            raise TypeError( "Invalid outputType '%s'"%outputType )

        self.configureSequences( )

        self.configurePersistency()

        self.configureInput( inputType )

        self.configureOutput( outputType )

        # ROOT persistency for histograms
        importOptions('$STDOPTS/RootHist.opts')
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc('RootHistCnv').ForceAlphaIds = True

        # Use a default histogram file name if not already set
        if not hasattr( HistogramPersistencySvc(), "OutputFile" ):
            histosName   = self.getProp("DatasetName")
            if (self.evtMax() > 0): histosName += '-' + str(self.evtMax()) + 'ev'
            histosName += '-histos.root'
            HistogramPersistencySvc().OutputFile = histosName

    def configureSequences(self):
        
        # Check for special data options
        for option in self.getProp('SpecialData'):
            if option not in self.KnownSpecialData:
                raise RunTimeError("Unknown SpecialData option '%s'" %option)

        escherSeq = GaudiSequencer("EscherSequencer")
        #escherSeq.Context = self.getProp("Context")
        ApplicationMgr().TopAlg = [ escherSeq ]
        mainSeq = self.getProp("MainSequence")
        if len( mainSeq ) == 0:
            self.MainSequence = self.DefaultSequence
        mainSeq = self.MainSequence
        escherSeq.Members += mainSeq
        ProcessPhase("Init").DetectorList += self.getProp("InitSequence")
        ProcessPhase("Init").Context = self.getProp("Context")

        from Configurables import RecInit, TrackSys
        log.info("Setting up alignment sequence")
        recInit = RecInit( name = "EscherInit",
                           PrintFreq = self.getProp("PrintFreq"))
        GaudiSequencer("InitEscherSeq").Members += [ recInit ]

        # set up the HltFilterSeq
        from Configurables import HltCompositionMonitor
        from Configurables import LoKi__HDRFilter as HDRFilter
        hltFilterSeq = GaudiSequencer( "HltFilterSeq" )

        from DAQSys.Decoders import DecoderDB
        from DAQSys.DecoderClass import decodersForBank
        from itertools import chain
        hltdecs = [DecoderDB.get("HltDecReportsDecoder/Hlt1DecReportsDecoder")]
        if not self.getProp("OnlineMode"):
            ## HLT2 decreports are only used offline.
            hltdecs += [DecoderDB.get("HltDecReportsDecoder/Hlt2DecReportsDecoder")]
        hltFilterSeq.Members = [d.setup() for d in hltdecs]

        ## FIXME: These lines should go back in as soon as an easy to use filter
        ## FIXME: is available that works for HLT1 and HLT2 decreports at the same time.
        ## identifies events that are not of type Hlt1ErrorEvent or Hlt2ErrorEvent
        ## hltErrCode = "HLT_PASS_RE('Hlt1(?!ErrorEvent).*Decision') & HLT_PASS_RE('Hlt2(?!ErrorEvent).*Decision')"
        ## hltErrorFilter = HDRFilter('HltErrorFilter', Code = hltErrCode )   # the filter
        ## hltFilterSeq.Members += [ HltCompositionMonitor(), hltErrorFilter ]
        ## add more hlt filters, if requested
        ## if hasattr(self,"HltFilterCode") and len(self.getProp("HltFilterCode"))>0:
        ##     hltfilter = HDRFilter ( 'HLTFilter',
        ##                             Code = self.getProp("HltFilterCode"))
        ##     hltfilter.Preambulo += [ "from LoKiCore.functions import *" ]
        ##     hltFilterSeq.Members += [ hltfilter ]

        # in Escher we'll always use the DOD
        ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ]

        alignSeq = GaudiSequencer("AlignSequence")

        # if the patter reco is not run, we need the DataOnDemand svc
        # so that e.g. the track container(s) is unpacked:
        if not GaudiSequencer("RecoTrSeq").getProp("Enable"):
            DstConf( EnableUnpack = True )

        # Setup tracking sequence
        trackConf = TrackSys()
        self.setOtherProps(trackConf,["SpecialData","OutputType","DataType","Simulation","GlobalCuts"])
        trackConf.ExpertTracking = self.getProp("ExpertTracking")

        ta = TAlignment()
        ta.Upgrade = self.getProp("Upgrade")
        self.setOtherProps(ta, ["DatasetName","OnlineMode","OnlineAligWorkDir"])
        ta.Sequencer = alignSeq
        if  self.getProp("Millepede") :
            log.info("Using Millepede type alignment!")
            self.setProp("Incident", "GlobalMPedeFit")
            ta.Method = "Millepede"
            ta.Sequencer = GaudiSequencer("MpedeAlignSeq")

    def configurePersistency(self):
        if hasattr(self,"Persistency"):
            if self.getProp("Persistency") is not None:
                LHCbApp().setProp("Persistency", self.getProp("Persistency"))

    def configureInput(self, inputType):
        """
        Tune initialisation according to input type
        """

        # POOL Persistency, now in LHCbApp
        #importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

        # By default, Escher only needs to open one input file at a time
        # Only set to zero if not previously set to something else.
        if not IODataManager().isPropertySet("AgeLimit") : IODataManager().AgeLimit = 0

        if inputType in [ "XDST", "DST", "RDST", "ETC" ]:
            # Kill knowledge of any previous Brunel processing
            from Configurables import ( TESCheck, EventNodeKiller )
            InitReprocSeq = GaudiSequencer( "InitReprocSeq" )
            if ( self.getProp("WithMC") and inputType in ["XDST","DST"] ):
                # Load linkers, to kill them (avoid appending to them later)
                InitReprocSeq.Members.append( "TESCheck" )
                TESCheck().Inputs = ["Link/Rec/Track/Best"]
            InitReprocSeq.Members.append( "EventNodeKiller" )
            EventNodeKiller().Nodes = [ "pRec", "Rec", "Raw", "Link/Rec" ]

        if inputType == "ETC":
            raise DeprecationWarning, "ETC are no longer supported by LHCb software"
            from Configurables import  TagCollectionSvc
            ApplicationMgr().ExtSvc  += [ TagCollectionSvc("EvtTupleSvc") ]
            # Read ETC selection results into TES for writing to DST
            IODataManager().AgeLimit += 1

        #if inputType in [ "MDF", "RDST", "ETC" ]:
        #    # In case raw data resides in MDF file
        #    EventPersistencySvc().CnvServices.append("LHCb::RawDataCnvSvc")
        DecodeRawEvent()

        if self.getProp("UseFileStager") :
            import os, getpass
            from FileStager.Configuration import configureFileStager
            from Configurables import FileStagerSvc
            configureFileStager(keep = True)
            targetdir = '/tmp/' + getpass.getuser() + '/stagedfiles'
            if os.path.isdir( '/pool/spool/' ) :
                targetdir = '/pool/spool/' + getpass.getuser() + '/stagedfiles'
            if not os.path.isdir( targetdir ):
                os.makedirs( targetdir )
            FileStagerSvc().Tempdir=targetdir

    def configureOutput(self, dstType):
        """
        Set up output stream
        """
        if dstType in [ "DST", "RDST" ]:
            if hasattr(self,"Persistency"):
                if self.getProp("Persistency") is not None:
                    DstConf().Persistency=self.getProp("Persistency")

            writerName = "DstWriter"
            packType  = self.getProp( "PackType" )
            # Do not pack DC06 DSTs, for consistency with existing productions
            if self.getProp("DataType") == "DC06": packType = "NONE"

            dstWriter = OutputStream( writerName )
            dstWriter.RequireAlgs += ["Reco"] # Write only if Rec phase completed

            # Set a default output file name if not already defined in the user job options
            if not hasattr( dstWriter, "Output" ):
                DstConf().OutputName = self.outputName()

            # Define the file content
            DstConf().Writer     = writerName
            DstConf().DstType    = dstType
            DstConf().PackType   = packType
            DstConf().setProp("WriteFSR", self.getProp("WriteFSR"))

            if hasattr(self,"Persistency"):
                if self.getProp("Persistency") is not None:
                    DstConf().setProp("Persistency", self.getProp("Persistency"))

            from Configurables import TrackToDST
            if dstType == "DST":
                # Sequence for altering DST content
                ProcessPhase("Output").DetectorList += [ "DST" ]
                # Filter Track States to be written
                trackFilter = TrackToDST()
            else:
                # Sequence for altering content of rDST compared to DST
                ProcessPhase("Output").DetectorList += [ "L0", "DST" ]
                # Filter Track States to be written
                trackFilter = TrackToDST("TrackToRDST")
                trackFilter.veloStates = ["ClosestToBeam"]
                trackFilter.longStates = ["ClosestToBeam"]
                trackFilter.TTrackStates = ["FirstMeasurement"]
                trackFilter.downstreamStates = ["FirstMeasurement"]
                trackFilter.upstreamStates = ["ClosestToBeam"]

            GaudiSequencer("OutputDSTSeq").Members += [ trackFilter ]

            if packType != "NONE":
                # Add the sequence to pack the DST containers
                packSeq = GaudiSequencer("PackDST")
                DstConf().PackSequencer = packSeq
                DstConf().AlwaysCreate  = True
                GaudiSequencer("OutputDSTSeq").Members += [ packSeq ]

        # Always write an ETC if ETC input
        if self.getProp( "InputType" ).upper() == "ETC":
            raise DeprecationWarning, "ETC are no longer supported by LHCb"
            etcWriter = TagCollectionSvc("EvtTupleSvc")
            ApplicationMgr().ExtSvc.append(etcWriter)
            ApplicationMgr().OutStream.append("GaudiSequencer/SeqTagWriter")
            importOptions( "$ESCHEROPTS/DefineETC.opts" )
            if not hasattr( etcWriter, "Output" ):
               etcWriter.Output = [ "EVTTAGS2 DATAFILE='" + self.getProp("DatasetName") + "-etc.root' TYP='POOL_ROOTTREE' OPT='RECREATE' " ]

        # Do not print event number at every event (done already by Brunel)
        EventSelector().PrintFreq = -1
        CountingPrescaler("EscherPrescaler").PrintFreq = self.getProp( "PrintFreq" )
        # Modify printout defaults
        if self.getProp( "NoWarnings" ):
            importOptions( "$ESCHEROPTS/SuppressWarnings.opts" )

    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("DatasetName")
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        outputType = self.getProp("OutputType").lower()
        return outputName + '.' + outputType

    def evtMax(self):
        return LHCbApp().evtMax()

    def configureDBSnapshot(self):
        """
        Configure the database to use the online snapshot
        """
        tag = { "DDDB":     self.getProp('DDDBtag')
                , "LHCBCOND": self.getProp('CondDBtag')
                , "SIMCOND" : self.getProp('CondDBtag')
                , "ONLINE"  : 'fake'
                }

        # https://savannah.cern.ch/bugs/?94454#comment12
        from Configurables import MagneticFieldSvc
        MagneticFieldSvc().UseSetCurrent = True

        from Configurables import CondDB
        cdb = CondDB()
        cdb.Tags = tag
        cdb.setProp('IgnoreHeartBeat', True)
        self.setOtherProps( cdb, [ 'UseDBSnapshot',
                                   'DBSnapshotDirectory',
                                   'PartitionName' ])

        # So, here is the problem: we don't want to run the risk that
        # the CondDB() configurable (which configures /after/ us)
        # overwrites our conditions. Yet, we don't want to miss the
        # default conditions (e.g. velo stepper motor, magnetic field)
        # either. if we add our conditions to its
        # RunChangeHandlerConditions list, then we a) need to fix the
        # path and b) don't know what happens for conditions that
        # appear twice, because we don't control the ordering of the
        # list. So, the hack is:
        # - don't set 'EnableRunChangeHandler'
        # - copy what is hidden behind that flag in CondDB()._configureDBSnapshot
        # - do the test of the RunChangeHandler configuration ourselves:
        cdb.setProp('EnableRunChangeHandler', False)
        from Configurables import RunChangeHandlerSvc
        rch = RunChangeHandlerSvc()
        ApplicationMgr().ExtSvc.append(rch)
        baseloc = self.getProp( "DBSnapshotDirectory" )
        rch.Conditions = dict( (c,'/'.join([baseloc,f])) for f,cs in cdb.getProp("RunChangeHandlerConditions").iteritems() for c in cs )
        
        #path = self.getProp('DBSnapshotDirectory') + "/.."*4 + "/group/online/AligWork/current/"
        allconds = {
            'Velo' : [
                    'Conditions/Alignment/Velo/VeloSystem',
                    'Conditions/Alignment/Velo/VeloRight',
                    'Conditions/Alignment/Velo/VeloLeft']
                 + ['Conditions/Alignment/Velo/Module%02d'%i for i in range(0, 42)] 
                 + ['Conditions/Alignment/Velo/Detector%02d-%02d' % (i, (1 + i / 2) % 2) for i in range(0, 42)],
            'IT' : []
                 + [ 'Conditions/Alignment/IT/ITSystem' ]
                 + [ 'Conditions/Alignment/IT/ITT%d' % i for i in range(1,4) ]
                 + [ 'Conditions/Alignment/IT/ITT%d%sBox' % (i,b) for i in range(1,4) for b in ['Top','Bottom','ASide','CSide' ] ]
                 + [ 'Conditions/Alignment/IT/ITT%d%sLayer%s' % (i,b,l) for i in range(1,4) for b in ['Top','Bottom','ASide','CSide' ] for l in ['X1','U','V','X2' ] ]
                 + [ 'Conditions/Alignment/IT/ITT%d%sLayer%sLadder%d' % (i,b,l,a) for i in range(1,4) for b in ['Top','Bottom','ASide','CSide' ] for l in ['X1','U','V','X2' ] for a in range(1,8) ],
        #        + [ 'Conditions/Alignment/IT/ITT%d%sLayer%sLadder%dSector' % (i,b,l,a) for i in range(1,4) for b in ['Top','Bottom','ASide','CSide' ] for l in ['X1','U','V','X2' ] for a in range(1,8) ]
        #        + [ 'Conditions/Alignment/IT/ITT%d%sLayer%sLadder%dSector_Sensor1' % (i,b,l,a) for i in range(1,4) for b in ['Top','Bottom','ASide','CSide' ] for l in ['X1','U','V','X2' ] for a in range(1,8) ]
        #        + [ 'Conditions/Alignment/IT/ITT%d%sLayer%sLadder%dSector_Sensor2' % (i,b,l,a) for i in range(1,4) for b in ['ASide','CSide' ] for l in ['X1','U','V','X2' ] for a in range(1,8) ] ,
            'OT' : []
                 + [ 'Conditions/Alignment/OT/OTSystem' ]
                 + [ 'Conditions/Alignment/OT/T%d' %i for i in range(1,4) ]
                 + [ 'Conditions/Alignment/OT/T%d%s' % (i,l) for i in range(1,4) for l in ['X1','U','V','X2' ] ]
                 + [ 'Conditions/Alignment/OT/T%d%sQ%d' % (i,l,q) for i in range(1,4) for l in ['X1','U','V','X2' ] for q in range(0,4) ]
                 + [ 'Conditions/Alignment/OT/T%d%sQ%dM%d' % (i,l,q,m) for i in range(1,4) for l in ['X1','U','V','X2' ] for q in range(0,4) for m in range(1,10) ],
            'TT' : []
                 + [ 'Conditions/Alignment/TT/TTSystem' ]
                 + [ 'Conditions/Alignment/TT/TT%s' % i for i in ['a','b' ] ]
                 + [ 'Conditions/Alignment/TT/TT%sLayer' % (l) for l in ['aX','aU','bV','bX' ] ]
                 + [ 'Conditions/Alignment/TT/TT%sLayerR%dModule%d%s' % (l,r,m,w) for w in ['T','B'] for l in ['aX','aU','bV','bX'] for r in range(1,4) for m in range(1,4)]
                 + [ 'Conditions/Alignment/TT/TT%sLayerR%dModule%d%s' % (l,r,m,w) for w in ['T','B'] for l in ['bV','bX']           for r in range(1,4) for m in range(4,6)]
                 + [ 'Conditions/Alignment/TT/TT%sLayerR%dModule%d%s' % (l,r,m,w) for w in ['T','B'] for l in ['aX','aU','bV','bX'] for r in [1,3]      for m in range(6,7)]
                 + [ 'Conditions/Alignment/TT/TT%sLayerR%dModule%d%s' % (l,r,m,w) for w in ['T','B'] for l in ['aX','aU']           for r in [1,3]      for m in range(4,6)],
            'Muon' : []
                    + ['Conditions/Alignment/Muon/MuonSystem']
                    + [ 'Conditions/Alignment/Muon/M%sStation' % i for i in range(1,6) ]
                    + [ 'Conditions/Alignment/Muon/M%sASide' % i for i in range(1,6) ]
                    + [ 'Conditions/Alignment/Muon/M%sCSide' % i for i in range(1,6) ]
        }

        ## This is a bit dirty, since we're supposed to control TAlignment. We
        ## know that this is set from top level, so let's give it a try anyway
        ta = TAlignment()
        sdToWrite = set(ta.getProp("WriteCondSubDetList"))
        pat = self.getProp("OnlineAligWorkDir") + "/xml/%s.xml" 
        conditionmap = dict((pat % sd, f) for (sd, f) in allconds.iteritems() if sd in sdToWrite)
            
        # add to the existing map
        rch.Conditions = dict(rch.Conditions.items() +  dict( (c,f) for f,cs in conditionmap.iteritems() for c in cs ).items() )

        from Configurables import MagneticFieldSvc
        MagneticFieldSvc().UseSetCurrent = True

    ## Check the reconstruction sequence
    def CheckRecoSequence(self):
        RecoSeq = self.getProp("RecoSequence")
        RunType = 'Run2' if self.getProp("DataType") in ['2015'] else 'Run1'
        # Set default sequence if not specified 
        if RecoSeq == []: 
            RecoSeq = self.DefaultRecoSequences[RunType]
            return RecoSeq
        # Inform that the specified sequence contains algorithm which are not in the default one
        if not set(RecoSeq).issubset(set(self.DefaultRecoSequences[RunType])):
            print 'Escher.RecoSequence contains algorithms which are not in '+RunType+' sequence:', \
                self.DefaultRecoSequences[RunType]
        return RecoSeq

    ## Apply the configuration
    def __apply_configuration__(self):

        GaudiKernel.ProcessJobOptions.PrintOff()
        GaudiKernel.ProcessJobOptions.PrintOn()
        log.info("Initializing sequences!")
        self.setOtherProps(RecSysConf(),["SpecialData","Context",
                                         "OutputType","DataType"])

        #if self.isPropertySet("RecoSequence") :
        #self.setOtherProp(RecSysConf(),["RecoSequence"])
        RecSysConf().RecoSequence = self.CheckRecoSequence()

        # there is a bug in setOtherProps, so we cannot use it to set the MoniSequence.
        if not self.getProp("OnlineMode"):
            self.setOtherProps(RecMoniConf(),["Context","DataType"])
            RecMoniConf().MoniSequence = self.getProp("MoniSequence")
        else:
            self.setOtherProps(RecMoniConf(),["Context","DataType"])
            RecMoniConf().MoniSequence = self.getProp("MoniSequence")
            RecMoniConf().Context = "Offline"
            RecMoniConf().OutputLevel = FATAL
            RecMoniConf().Histograms = "OfflineFull"

        self.defineGeometry()
        self.defineEvents()
        self.defineOptions()

        # database hacking for online.
        if self.getProp('UseDBSnapshot') : self.configureDBSnapshot()

        # Use TimingAuditor for timing, suppress printout from SequencerTimerTool
        from Configurables import (ApplicationMgr,AuditorSvc,SequencerTimerTool)
        ApplicationMgr().ExtSvc += [ 'ToolSvc', 'AuditorSvc' ]
        ApplicationMgr().AuditAlgorithms = True
        AuditorSvc().Auditors += [ 'TimingAuditor' ]
        SequencerTimerTool().OutputLevel = 4

        log.info( self )
        log.info( LHCbApp() )
        log.info( RecSysConf() )
        log.info( TrackSys() )
        if not self.getProp("OnlineMode"):
            log.info( RecMoniConf() )
        log.info( TAlignment() )
        log.info( DstConf() )
        GaudiKernel.ProcessJobOptions.PrintOff()
