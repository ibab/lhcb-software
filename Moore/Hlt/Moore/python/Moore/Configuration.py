"""
High level configuration tool(s) for Moore
"""
__version__ = "$Id: Configuration.py,v 1.131 2010-09-01 16:39:02 raaij Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ, path
from LHCbKernel.Configuration import *
from Gaudi.Configuration import *
from Configurables import HltConf
from Configurables import GaudiSequencer
from Configurables import LHCbApp
from Configurables import DecodeRawEvent, RawEventFormatConf, DDDBConf

#import helper functions
try:
    import Funcs
except ImportError:
    import Moore.Funcs as Funcs

import GaudiKernel.ProcessJobOptions
from  ctypes import c_uint


class MooreExpert(LHCbConfigurableUser):
    """
    Expert options for Moore, hidden from most users by being placed in a
    different configurable!
    """
    __slots__={
        #########################################
        # Expert options, only set if you know what you are doing
        #########################################
        "prefetchConfigDir" :'MOORE_v14r8'  # which configurations to prefetch
        , "DQFLAGStag" : "latest" # latest in the CondDB for this DataType
        , 'WriteFSR'    :  True #copy FSRs as required
        , "configAlgorithms" : ['Hlt']    # which algorithms to configure (automatically including their children!)...
        , "configServices" :   ['ToolSvc','Hlt::Service','HltANNSvc' ]    # which services to configure (automatically including their dependencies!)...
        , "TCKpersistency" :   'cdb' # which method to use for TCK data? valid is 'file','tarfile', 'zipfile', 'cdb' and 'sqlite'
        , "Hlt2Independent" : False #turn off HLT1 track decoding
        , "DisableMonitors" : False #Disable HLT monitoring
        }

    def __apply_configuration__(self):
        #don't ever do anything here, this configurable is only used for __reading__ by Moore()!
        for prop in self.__slots__:
            if self.isPropertySet(prop) and self.getProp(prop)!=self.getDefaultProperty(prop):
                log.warning("Hey! You're setting an expert property "+prop+" are you an expert? Let's hope so!")
        #seriously, this configurable should not hold any logic, it is only used as a singleton options store
        return


class Moore(LHCbConfigurableUser):
    ## Possible used Configurables
    __used_configurables__ = [ HltConf
                             , LHCbApp
                             , DecodeRawEvent
                             , DDDBConf
                             , MooreExpert ]

    __slots__ = {
        #########################################
        # Basic options, used to set LHCbApp
        #########################################
          "EvtMax":            -1    # Maximum number of events to process
        , "SkipEvents":        0
        , "Simulation":        False # True implies use SimCond
        , "DataType":          '2012' # Data type, can be [ 'DC06','2008' ]
        , "DDDBtag" :          'default' # default as set in DDDBConf for DataType
        , "CondDBtag" :        'default' # default as set in DDDBConf for DataType
        #########################################
        # Mandatory options to consider
        #########################################
        # either run from a threshold setting
        , "ThresholdSettings" : ''
        #or decide how to handle a TCK
        , "UseTCK"     :       False # use TCK instead of options...
        , "InitialTCK" :'0x00012009'  # which configuration to use during initialize
        , "CheckOdin"  :       True  # use TCK from ODIN
        #########################################
        # Common options, used to configure Moore
        #########################################
        , "outputFile" :       '' # output filename
        , "inputFiles" :       [ ] # input
        , "EnableTimer" :       None
        , "TimingTableWidth" :   90
        , "PersistReco" :   False
        , "EnableDataOnDemand": False
        , "OutputLevel" : WARNING #if this is set to WARNING (or higher) Moore will become almost silent.
                             #if this is set to DEBUG or VERBOSE, this mimics the previous verbose setting
                             #INFO sets all tools and algs to INFO, since the TCK will have been generated with WARNING
        , 'Split'       : '' # HLT1 or HLT2?
        , "EnableAuditor" :    [ ]  # put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]
        , 'WriterRequires' : [ 'HltDecisionSequence' ] # this contains Hlt1 & Hlt2
        , 'ForceSingleL0Configuration' : True # use one single, fixed L0 configuration location (ToolSvc.L0DUConfig)
        #########################################
        # Options used to make/manipulate TCKs
        #########################################
        , "configLabel" :      ''    # label for generated configuration
        , "TCKData" :          '$HLTTCKROOT' # where do we read/write TCK data from/to?
        , "generateConfig" :   False # whether or not to generate a configuration
        #######################################
        # Options nominally for online running
        #######################################
        , "RunOnline"         : False
        #######################################
        # Options to make processed data look like actual input...
        #######################################
        , "RemoveInputHltRawBanks" : False
        #########################################
        # Deprecated former options
        #########################################
        #, "L0"         :       None # use L0App!
        #, "ReplaceL0BanksWithEmulated" : None # use L0App
        #, "RunL0Emulator" : None # use L0App
        #, "Verbose" :       None # use OutputLevel
        #, 'REQ1' : None # use MooreOnline!
        #, "PartitionName" :  None # use MooreOnline!
        #, "RunMonitoringFarm" :  None # use MooreOnline!
        #, "NbOfSlaves":         None # use MooreOnline!
        #, 'IgnoreDBHeartBeat'  :   None # use CondDB directly or MooreOnline!
        #, "UseDBSnapshot"     : None # use CondDB directly or MooreOnline!
        #, "DBSnapshotDirectory" : None # use CondDB directly or MooreOnline!
        #, 'EnableMonitoring' : None # use HltConf directly or MooreOnline!
        #, 'SkipDisabledL0Channels' : None # use HltConf directly
        #, "prefetchConfigDir" : None # use MooreExpert
        #, "EnableLumiEventWriting"       : None # use HltConf directly
        #, 'EnableAcceptIfSlow' : None # use HltConf directly
        #, 'RequireL0ForEndSequence'     :  None # use HltConf directly
        #, 'SkipHltRawBankOnRejectedEvents' : None # use HltConf directly
        #, 'HistogrammingLevel' : None # use HltConf directly
        #, 'TimeOutThreshold'  : 10000  # Not used anywhere??
        #, 'TimeOutBits'       : 0x200 # Not used anywhere??
        #, 'RequireRoutingBits' : None # use HltConf
        #, 'VetoRoutingBits'    : None # use HltConf
        #, "DQFLAGStag" : None # use MooreExpert
        #, 'WriteFSR'    :  None # use MooreExpert
        #, 'EnableRunChangeHandler' : None # use CondDB directly, only needed there!
        #, "configAlgorithms" : None # use MooreExpert
        #, "configServices" :  None #use MooreExpert
        }

    _propertyDocDct={
        #########################################
        # Basic options, used to set LHCbApp
        #########################################
          "EvtMax":   "Maximum number of events to process"
        , "SkipEvents":        "Events to skip, default 0"
        , "Simulation":        "True implies use SimCond"
        , "DataType":          'Data type, can be 2010, 2012, etc.'
        , "DDDBtag" :          'database tag, default as set in DDDBConf for DataType'
        , "CondDBtag" :        'database tag, default as set in DDDBConf for DataType'
        #########################################
        # Mandatory options to consider
        #########################################
        # either run from a threshold setting
        , "ThresholdSettings" : 'Which threshold setting to use, set either this, or the TCK'
        #or decide how to handle a TCK
        , "UseTCK"     :       "use TCK instead of options. Set either this or ThresholdSettings"
        , "InitialTCK" :       'which configuration to use during initialize. If there is no run change hadler enabled, the this will be the only TCK'
        , "CheckOdin"  :       "use TCK from ODIN, need EnableRunChangeHandler"
        #########################################
        # Common options, used to configure Moore
        #########################################
        , "outputFile" :       'output filename'
        , "inputFiles" :       "input, can be a simple list of files"
        , "EnableTimer" :      "Turn on the timing table, set this to the name of a file you'd like the timing to write out to. True does not create a file."
        , "TimingTableWidth" :      "Width of the first column of the printed timing table."
        , "EnableDataOnDemand": "Activate the DataOnDemand service, sometimes needed during testing"
        , "OutputLevel" : """Multi-level option, keeps same logic as standard OutputLevel.
        if this is set to WARNING (or higher) Moore will become almost silent.
        if this is set to DEBUG or VERBOSE, this mimics the previous verbose setting
        if this is set to INFO info messages are nominally printed"""
        , 'Split'       : 'HLT1 or HLT2?'
        , "EnableAuditor" :    'put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]'
        , 'WriterRequires' : "require certain algs for the end sequence [ 'HltDecisionSequence' ] # this contains Hlt1 & Hlt2"
        , 'ForceSingleL0Configuration' : "True # use one single, fixed L0 configuration location (ToolSvc.L0DUConfig)"
        #########################################
        # Options used to make/manipulate TCKs
        #########################################
        , "configLabel" :      'label for generated configuration'
        , "TCKData" :          "'$HLTTCKROOT' # where do we read/write TCK data from/to?"
        , "generateConfig" :   "False # whether or not to generate a configuration"
        #######################################
        # Options nominally for online running
        #######################################
        , "RunOnline" : "Set now by MooreOnline(), implies we're running with the full online environment"
        #######################################
        # Options to make processed data look like actual input...
        #######################################
        , "RemoveInputHltRawBanks" : "Remove any Hlt rawbank from the input file at the start of the event loop"
        #######################################
        # Options for offline running
        #######################################
        , "PersistReco" : "Do we want toe persist Moore reconstruction containers (if writing DST)"
        #########################################
        # Deprecated former options
        #########################################
        # deprecated options require the word DEPRECATED in the documentation description
        # this will be used to print a deprecation warning
        #, "L0"         :       "None # DEPRECATED. use L0App"
        #, "ReplaceL0BanksWithEmulated" : "None # DEPRECATED.  use L0App"
        #, "RunL0Emulator" : "None # DEPRECATED.  use L0App"
        #, "Verbose" :       "None # DEPRECATED.  use Moore().OutputLevel"
        #, 'REQ1' : "None # DEPRECATED.  use MooreOnline"
        #, "PartitionName" :  "None # DEPRECATED.  use MooreOnline"
        #, "RunMonitoringFarm" :  "None # DEPRECATED.  use MooreOnline"
        #, "NbOfSlaves":         "None # DEPRECATED.  use MooreOnline"
        #, 'IgnoreDBHeartBeat'  :   "None #  DEPRECATED. use CondDB() directly or MooreOnline"
        #, "UseDBSnapshot"     : "None # DEPRECATED.  use CondDB() directly or MooreOnline"
        #, "DBSnapshotDirectory" : "None # DEPRECATED.  use CondDB() directly or MooreOnline"
        #, 'EnableMonitoring' : "None # DEPRECATED.  use HltConf() directly or MooreOnline"
        #, 'SkipDisabledL0Channels' : "None # DEPRECATED.  use HltConf() directly"
        #, "prefetchConfigDir" : "None # DEPRECATED. use MooreExpert"
        #, "EnableLumiEventWriting"       : "None # DEPRECATED.  use HltConf() directly"
        #, 'EnableAcceptIfSlow' : "None # DEPRECATED.  use HltConf directly"
        #, 'RequireL0ForEndSequence'     :  "None # DEPRECATED.  use HltConf() directly"
        #, 'SkipHltRawBankOnRejectedEvents' : "None #  DEPRECATED. use HltConf() directly"
        #, 'HistogrammingLevel' : "None #  DEPRECATED. use HltConf() directly"
        #, 'TimeOutThreshold'  : "10000  #  DEPRECATED. use (no replacement not used anywhere??)"
        #, 'TimeOutBits'       : "0x200 #  DEPRECATED. use (no replacement not used anywhere??)"
        #, 'RequireRoutingBits' : "None #  DEPRECATED. use HltConf()"
        #, 'VetoRoutingBits'    : "None #  DEPRECATED. use HltConf()"
        #, "DQFLAGStag" : "None #  DEPRECATED. use MooreExpert()"
        #, 'WriteFSR'    :  "None #  DEPRECATED. use MooreExpert()"
        #, 'EnableRunChangeHandler' : "None #  DEPRECATED. use CondDB() directly, only needed there!"
        #, "configAlgorithms" : "None #  DEPRECATED. use MooreExpert()"
        #, "configServices" :  "None # DEPRECATED. use MooreExpert()"
        }


    def _configureDataOnDemand(self) :
        if not self.getProp("EnableDataOnDemand") :
            if 'DataOnDemandSvc' in ApplicationMgr().ExtSvc :
                ApplicationMgr().ExtSvc.pop('DataOnDemandSvc')
            from Gaudi.Configuration import appendPostConfigAction
            def disableFaultHandler() :
                from Configurables import EventDataSvc
                EventDataSvc().EnableFaultHandler = False
            appendPostConfigAction(  disableFaultHandler )
        else:
            from Configurables import DataOnDemandSvc
            dod = DataOnDemandSvc()
            if dod not in ApplicationMgr().ExtSvc :
                ApplicationMgr().ExtSvc.append( dod )

    def _configureDQTags(self):
        from Configurables import CondDB
        tag=None
        toset=MooreExpert().getProp("DQFLAGStag")
        if not len(toset) or toset=="latest" or toset=="default":
            from CondDBUI.Admin.TagsFilter import last_gt_lts
            dqtags = last_gt_lts('DQFLAGS', self.getProp("DataType"))
            if dqtags:
                tag=dqtags[0]
        else:
            tag=toset
        if CondDB().isPropertySet("Tags") and 'DQFLAGS' in  CondDB().getProp("Tags") and len(CondDB().getProp("Tags")["DQFLAGS"]):
            #don't set anything if it has already been set elsewhere
            pass
        elif tag:
            CondDB().Tags=Funcs._zipdict(CondDB().Tags,{"DQFLAGS":tag})

    def _configureInput(self):
        files = self.getProp('inputFiles')
        #    #  veto lumi events..
        #    #ApplicationMgr().EvtSel.REQ1 = "EvType=2;TriggerMask=0x0,0x4,0x0,0x0;VetoMask=0,0,0,0;MaskType=ANY;UserType=USER;Frequency=PERC;Perc=100.0"

        if files:
            from GaudiConf import IOExtension
            IOExtension().inputFiles(files,clear=True)
        # remove any HLT rawbanks from the input if so requested...

        if self.getProp('RemoveInputHltRawBanks') :
            from Configurables        import bankKiller, ApplicationMgr
            app=ApplicationMgr()
            hlt_banks = [ 'HltDecReports','HltRoutingBits','HltSelReports','HltVertexReports','HltLumiSummary','HltTrackReports' ]
            bk = bankKiller( 'RemoveInputHltRawBanks',  BankTypes=hlt_banks )
            app.TopAlg.insert(0, bk)



    def _setRawEventLocations(self):
        """
        Check that I can set DecodeRawEvent.py options correctly.
        """
        #if not set, I will override the inputs with the "Pit locations"
        if (not DecodeRawEvent().isPropertySet("OverrideInputs")) or DecodeRawEvent().getProp("OverrideInputs") is None:
            #default version which comes out of the Pit,
            #currently just DAQ/RawEvent
            DecodeRawEvent().OverrideInputs="Pit"
            return
        from RawEventCompat.Configuration import _checkv
        from Configurables import RawEventFormatConf
        RawEventFormatConf().loadIfRequired()
        #if set explicitly to the pit locations, all is good already
        if _checkv(DecodeRawEvent().getProp("OverrideInputs"))==_checkv("Pit"):
            return
        #else it's a screw-up
        if self.getProp("RunOnline"):
            raise ValueError("When running in Online Mode, you'd better not try and reset the RawEventLocations, this is baaaad")
        if self.getProp("UseTCK"):
            raise ValueError("You are trying to reset RawEvent inputs/outputs when running from a TCK, this won't work because they are fixed inside the TCK anyway! (they're part of the Hlt sequence), try adding RecombineRawEvent() or RawEventJuggler() instead, or some other such trick.")
        if self.getProp("generateConfig"):
            raise ValueError("When generating a TCK, you'd better not be trying to overwrite the RawEvent input/output locations, this would be a disaster!, try adding RecombineRawEvent() or RawEventJuggler() instead or some other such trick.")
        # if input is MDF, RAW, DIGI, XDIGI, then raise an error,
        # these locations are always "DAQ/RawEvent"
        files = self.getProp('inputFiles')
        files=files+EventSelector().Input

        ext=files[0].split('.')[-1].strip().split('?')[0].strip().upper()
        if ext in ["MDF","RAW","DIGI","XDIGI"]:
            raise ValueError("When running from a DIGI, XDIGI or RAW file, the only raw event location is DAQ/RawEvent, but you're resetting it into"+DecodeRawEvent().getProp("OverrideInputs"))

    def _configureOutput(self):
        fname = self.getProp('outputFile')
        if not fname : return
        writer = None

        #retrieve the persistency
        persistency=None
        from GaudiConf import IOExtension, IOHelper
        iox=IOExtension()

        #check the file type and use MDF writer or InputCopyStream
        if iox.detectFileType(fname) == 'MDF'  :
            from Configurables import LHCb__MDFWriter as MDFWriter
            writer = IOHelper("MDF","MDF").outputAlgs(fname
                                                       ,writer = MDFWriter( 'Writer' , Compress = 0 )
                                                       ,writeFSR=False)
            if self.getProp('WriterRequires') :
                from Configurables import LoKi__VoidFilter as VoidFilter
                #this next part does not work correctly, and I don't know why
                #writer = GaudiSequencer( 'WriteSequence'
                #                         , Members = self.getProp('WriterRequires') + writer
                #                         )
                writer = GaudiSequencer( 'WriteSequence'
                                         , Members = [ VoidFilter( "WriterFilter"
                                                                   , Preambulo = [ 'from LoKiHlt.algorithms import ALG_EXECUTED, ALG_PASSED' ]
                                                                   , Code = ' & '.join( [ "ALG_EXECUTED('%s') & ALG_PASSED('%s')" % (i,i) for i in self.getProp('WriterRequires') ] )
                                                                   )
                                                       ] + writer
                                         )
                #convert to a smegging list consistently
                writer=[writer]

            ApplicationMgr().OutStream+=writer

        else :
            from Configurables import InputCopyStream
            writer = InputCopyStream("Writer"
                                    , RequireAlgs = self.getProp('WriterRequires')
                                    )
            IOHelper(persistency,persistency).outStream(fname,writer,writeFSR=MooreExpert().getProp('WriteFSR'))
            if self.getProp("PersistReco"):
                HltConf().PersistReco=True


    def getRelease(self):
        import re,fileinput
        #  do NOT pick up the pz in vxrypz
        version = re.compile('^version\s*(v\d+r\d+)(p\d+)?')
        fname = os.environ.get('MOORESYSROOT')+'/cmt/requirements'
        for line in fileinput.input(fname) :
            match = version.match(line)
            if not match: continue
            fileinput.close()
            return 'MOORE_'+match.group(1)
        raise ValueError('Could not deduce MOORE (base)version from %s',fname)

    def getConfigAccessSvc(self):
        method  = MooreExpert().getProp('TCKpersistency').lower()
        TCKData = self.getProp('TCKData')
        if method == 'file' :
            from Configurables import ConfigFileAccessSvc
            return ConfigFileAccessSvc( Directory = TCKData +'/config' )
        if method == 'sqlite' :
            from Configurables import ConfigDBAccessSvc
            return ConfigDBAccessSvc( Connection = 'sqlite_file:' + TCKData +'/db/config.db' )
        if method == 'tarfile' :
            from Configurables import ConfigTarFileAccessSvc
            return ConfigTarFileAccessSvc( File = TCKData +'/config.tar' )
        if method == 'zipfile' :
            from Configurables import ConfigZipFileAccessSvc
            return ConfigZipFileAccessSvc( File = TCKData +'/config.zip' )
        if method == 'cdb' :
            from Configurables import ConfigCDBAccessSvc
            return ConfigCDBAccessSvc( File = TCKData +'/config.cdb' )
        raise TypeError("invalid TCK persistency '%s'"%method)

    def addAuditor(self,x) :
        if  'AuditorSvc' not in ApplicationMgr().ExtSvc :
            ApplicationMgr().ExtSvc.append( 'AuditorSvc' )
        AuditorSvc().Auditors.append( x )
        x.Enable = True

    def _outputLevel(self) :
        """
        Set the print out of various tools, services, all algorithms, and properties of all things like algorithms
        Sensitive to the OutputLevel of Moore

        a) configure things independently of level
        b) configure things depending on level
           - ThresholdSettings via postConfigActions
           - TCKs via transforms

        defaults are now WARNING printouts only, so I need to handle the case the user wishes to switch back if a TCK has been generated with a warning

        """
        #firstly explicitly configure things not seen by TCK
        #firstly configure things which are level-independent

        # Usual output levels for services
        from Configurables import XmlParserSvc
        XmlParserSvc().OutputLevel                = WARNING
        ApplicationMgr().OutputLevel              = INFO #I still want the Application Manager Finalized Sucessfully printout

        # Print algorithm name with 40 characters
        if not MessageSvc().isPropertySet("Format"):
            MessageSvc().Format = '% F%40W%S%7W%R%T %0W%M'

        #this should be OK to do here...
        from Funcs import _minSetFileTypes

        #postconfig away common warnings
        def suppresswarningifneeded():
            #histogram warning isn't needed if I didn't want histograms
            from Configurables import RootHistCnv__PersSvc
            if RootHistCnv__PersSvc().getProp("OutputFile")=="UndefinedROOTOutputFileName" or not len(RootHistCnv__PersSvc().getProp("OutputFile")):
                RootHistCnv__PersSvc().OutputEnabled=False
            if HistogramPersistencySvc().getProp("OutputFile") == '' or not HistogramPersistencySvc().isPropertySet("OutputFile"):
                HistogramPersistencySvc().Warnings = False
            #error from IODataManager is pointless when I have MDFs
            from Funcs import _minSetFileTypes
            if Moore().getProp("RunOnline") or _minSetFileTypes() in ["MDF","RAW"]:
                from Configurables import IODataManager
                IODataManager().DisablePFNWarning=True

        appendPostConfigAction(suppresswarningifneeded)

        #then configure things that depend on the level
        level=self.getProp("OutputLevel")
        from Configurables import LHCbSequencerTimerTool, SequencerTimerTool
        if level>=INFO: LHCbSequencerTimerTool().OutputLevel = WARNING
        if level>=INFO: SequencerTimerTool().OutputLevel = WARNING

        if level>DEBUG:
            from Configurables import LoKi__DistanceCalculator
            LoKi__DistanceCalculator().MaxPrints=0
        if level>VERBOSE:
            from Configurables import LoKiSvc
            LoKiSvc().Welcome = False

        from Configurables import Hlt__Service
        if not Hlt__Service().isPropertySet('Pedantic') : Hlt__Service().Pedantic = (level<DEBUG)
        ###############################################################
        #if level is less than INFO, I don't need to edit anything else
        #it's up to the users to do that themselves!
        ###############################################################
        if level<INFO:
            return

        if level>INFO:
            MessageSvc().OutputLevel = level
            ToolSvc().OutputLevel = level

        if level>INFO and hasattr(self, "EnableTimer") and self.getProp("EnableTimer") and type(self.getProp("EnableTimer")) is not str:
            print "# WARNING: Timing table is too verbose for printing, consider outputting to a file instead please, Moore().EnableTimer='timing.csv', for example."

        #################################################
        # If the OutputLevel is set I need
        # Either a postConfigAction or a transform
        # to suppress the outputs properly
        #################################################

        #################################################
        # Running from thresholds, use post config action
        #################################################
        if not self.getProp("UseTCK") and level>INFO:
            #post config to really reset all the output to null
            from DAQSys.Decoders import DecoderDB
            from GaudiConf.Manipulations import postConfForAll#,fullNameConfigurables
            props={}
            props["OutputLevel"]=level
            props["StatPrint"]=(level<WARNING)
            props["ErrorsPrint"]=(level<WARNING)
            props["PropertiesPrint"]=(level<WARNING)

            from DAQSys.Decoders import DecoderDB
            for k,v in DecoderDB.iteritems():
                for pk,pv in props.iteritems():
                    v.Properties[pk]=pv

            postConfForAll(head=[k for k in DecoderDB], prop_value_dict=props,force=True,recurseToTools=True)

            #only for GaudiHistoAlgs...
            props["HistoCountersPrint"]=(level<WARNING)
            postConfForAll(head=None, prop_value_dict=props,force=True,recurseToTools=True)
            #so, the above works for almost everything, apart from on-demand created tools, of which there are a lot, and these need to be added separately
            #mostly these tools are in the calo

            #now turn off the calo tool finalize printout, there are *a lot* of tools here
            tools={"CaloECorrection/ECorrection":props,
                   "CaloSCorrection/SCorrection":props,
                   "CaloLCorrection/LCorrection":props,
                   "CaloHypoEstimator":props,
                   "CaloExtraDigits/SpdPrsExtraE":props,
                   "CaloExtraDigits/SpdPrsExtraG":props,
                   "CaloExtraDigits/SpdPrsExtraM":props,
                   "CaloExtraDigits/SpdPrsExtraS":props,
                   "CaloSelectCluster/PhotonCluster":props,
                   "CaloSelectCluster/ElectronCluster":props,
                   "CaloSelectChargedClusterWithSpd/ChargedClusterWithSpd":props,
                   "CaloSelectClusterWithPrs/ClusterWithPrs":props,
                   "CaloSelectNeutralClusterWithTracks/NeutralCluster":props,
                   "CaloSelectNeutralClusterWithTracks/NotNeutralCluster":props,

                   "CaloSelectorNOT/ChargedCluster" : props,
                   "CaloSelectNeutralClusterWithTracks/ChargedCluster.NeutralCluster":props
                   }
            #allcalotools=[]

            postConfForAll(head=None, prop_value_dict={},types=["CaloSinglePhotonAlg","CaloElectronAlg","CaloMergedPi0Alg","NeutralProtoPAlg"],force=True,tool_value_dict=tools)
            #three extras for merged pi0
            tools={"CaloCorrectionBase/ShowerProfile":props,
                   "CaloCorrectionBase/Pi0SCorrection":props,
                   "CaloCorrectionBase/Pi0LCorrection":props
                   }
            postConfForAll(head=None, prop_value_dict=props,types=["CaloMergedPi0Alg"],force=True,tool_value_dict=tools)
            #and one calo clustering
            tools={"CaloClusterizationTool":props}
            postConfForAll(head=None, types=["CellularAutomatonAlg"],prop_value_dict=props, tool_value_dict=tools, force=True)
            #same for members of the ToolService
            from Configurables import LoKi__LifetimeFitter, CaloDigitFilterTool, CaloGetterTool, OTChannelMapTool, CaloClusterizationTool
            #public tools
            postConfForAll(head=[LoKi__LifetimeFitter("ToolSvc.lifetime"),CaloDigitFilterTool("ToolSvc.FilterTool"),CaloGetterTool("ToolSvc.CaloGetter"),OTChannelMapTool("ToolSvc.OTChannelMapTool"),CaloClusterizationTool("ToolSvc.CaloClusterizationTool")], prop_value_dict=props,force=True)


            #I still want to print "Application Manager Finalized Successfully"
            #and "End of event input reached" no matter what

            def AppMrgOP():
                if ApplicationMgr().getProp("OutputLevel")>INFO:
                    ApplicationMgr().OutputLevel=INFO
                if EventSelector().getProp("OutputLevel")>INFO:
                    EventSelector().OutputLevel=INFO

            appendPostConfigAction(AppMrgOP)


            def RestoreGenConfig():
                Moore().getConfigAccessSvc().OutputLevel =INFO
                from Configurables import HltGenConfig
                HltGenConfig().OutputLevel =INFO

            if self.getProp("generateConfig"):
                appendPostConfigAction(RestoreGenConfig)

            #################################################
            # Running from TCK define a similar transform
            #################################################
        elif self.getProp("UseTCK"):
            trans={".*":{"OutputLevel"      : {"^.*$":str(level)}}}
            #turn certain things back on if INFO is set again...
            trans[".*"]["StatPrint"]=         {"^.*$":str(level<WARNING)}
            trans[".*"]["ErrorsPrint"]=       {"^.*$":str(level<WARNING)}
            trans[".*"]["PropertiesPrint"]=   {"^.*$":str(level<WARNING)}
            trans[".*"]["HistoCountersPrint"]={"^.*$":str(level<WARNING)}

            Funcs._mergeTransform(trans)

            #kill LoKi warnings
            set=0
            if level<WARNING:
                set=3
            trans={".*DistanceCalculator.*":{"MaxPrints" : {"^.*$":str(set)}}}
            Funcs._mergeTransform(trans)
            #kill one extra loki print in tool service
            from GaudiConf.Manipulations import postConfForAll#,fullNameConfigurables
            props={}
            props["OutputLevel"]=level
            props["StatPrint"]=(level<WARNING)
            props["ErrorsPrint"]=(level<WARNING)
            props["PropertiesPrint"]=(level<WARNING)
            postConfForAll(head=["LoKi::LifetimeFitter/ToolSvc.lifetime"],force=True,prop_value_dict=props)

            from Configurables import HltConfigSvc
            cfg = HltConfigSvc()
            #self-defeating warnings!
            cfg.OutputLevel=ERROR


    def _profile(self) :
        ApplicationMgr().AuditAlgorithms = 1
        auditors = self.getProp('EnableAuditor')
        if hasattr(self, "EnableTimer") and self.getProp('EnableTimer') is not None:
            #print self.getProp('EnableTimer')
            from Configurables import LHCbTimingAuditor, LHCbSequencerTimerTool
            LHCbTimingAuditor('TIMER').addTool(LHCbSequencerTimerTool, name="TIMER")
            LHCbTimingAuditor('TIMER').TIMER.NameSize = self.getProp("TimingTableWidth")
            auditors = [ LHCbTimingAuditor('TIMER') ] + auditors
            if type(self.getProp('EnableTimer')) is not bool and len(self.getProp('EnableTimer')):
                LHCbTimingAuditor('TIMER').TIMER.SummaryFile=self.getProp('EnableTimer')
                #LHCbSequencerTimerTool().SummaryFile=self.getProp('EnableTimer')

        for i in auditors : self.addAuditor( i )

    def _generateConfig(self) :
        from HltConf.ThresholdUtils import Name2Threshold
        settings = Name2Threshold(HltConf().getProp('ThresholdSettings'))
        svcs = MooreExpert().getProp("configServices")
        algs = MooreExpert().getProp("configAlgorithms")
        if MooreExpert().getProp('TCKpersistency').lower() in [ 'tarfile', 'zipfile', 'cdb' ] :
            self.getConfigAccessSvc().Mode = 'ReadWrite'
            #self.getConfigAccessSvc().OutputLevel = 1

        from Configurables import HltGenConfig
        print 'requesting following  svcs: %s ' % svcs
        gen = HltGenConfig( ConfigTop = [ i.rsplit('/')[-1] for i in algs ]
                          , ConfigSvc = [ i.rsplit('/')[-1] for i in svcs ]
                          , ConfigAccessSvc = self.getConfigAccessSvc().getName()
                          , HltType = settings.HltType()
                          , MooreRelease = self.getRelease()
                          , Label = self.getProp('configLabel'))
        # make sure gen is the very first Top algorithm...
        from HltLine.HltDecodeRaw import DecodeODIN
        ApplicationMgr().TopAlg = DecodeODIN.members() + [ gen.getFullName() ] + ApplicationMgr().TopAlg
        def genConfigAction() :
            def gather( c, overrule ) :
                    def check(config,prop,value) :
                        if prop not in config.getDefaultProperties() : return False
                        if hasattr(config,prop) : return getattr(config,prop) == value
                        return config.getDefaultProperties()[prop] == value
                    def addOverrule(config,rule):
                        if c.name() not in overrule.keys() :
                           overrule[c.name()] = []
                        if rule not in overrule[c.name()] :
                           overrule[c.name()] += [ rule ]

                    if check(c,'HistoProduce',False) :
                        addOverrule(c,'HistoProduce:@OnlineEnv.Monitor@False')
                    if c.getType() in ['FilterDesktop','CombineParticles' ] and  check(c,'Monitor',False) :
                        addOverrule(c,'Monitor:@OnlineEnv.Monitor@False')
                    if check(c,'Enable',False) :
                        addOverrule(c,'OutputLevel:3')
                    for p in [ 'Members','Filter0','Filter1' ] :
                        if not hasattr(c,p) : continue
                        x = getattr(c,p)
                        if list is not type(x) : x = [ x ]
                        for i in x : gather(i,overrule)
            from Configurables import HltGenConfig,GaudiSequencer
            HltGenConfig().Overrule = { 'Hlt1ODINTechnicalPreScaler' : [ 'AcceptFraction:@OnlineEnv.AcceptRate@0' ] }
            gather( GaudiSequencer('Hlt'), HltGenConfig().Overrule )
            print HltGenConfig()
        from Gaudi.Configuration import appendPostConfigAction
        appendPostConfigAction( genConfigAction )

    def _l0(self) :
        from Configurables import L0DUFromRawTool
        from DAQSys.Decoders import DecoderDB
        #L0DUFromRawAlg('L0DUFromRaw').ProcessorDataOnTES = False
        l0dutool   = DecoderDB["L0DUFromRawTool"]
        l0dutool.Properties["StatusOnTES"]=False
        DecoderDB["L0DUFromRawAlg/L0DUFromRaw"].setup()

    def _config_with_hltconf(self):
        """
        Propagate settings to HltConf
        """

        hltConf = HltConf()

        self.setOtherProps( hltConf,
                            [ 'ThresholdSettings'
                            , 'DataType'
                            , 'Split'
                            , 'ForceSingleL0Configuration'
                            ]
                          )
        if self.getProp("OutputLevel")<INFO:
            hltConf.setProp("Verbose",True)

        from Configurables import Hlt2Conf
        #cannot do this for the TCK right now. Ideally I want a transform which does the same as this.
        if MooreExpert().getProp("Hlt2Independent") and ("Hlt1TrackOption" in Hlt2Conf().__slots__ or hasattr(Hlt2Conf(),"Hlt1TrackOption")) and not Hlt2Conf().isPropertySet("Hlt1TrackOption"):
            Hlt2Conf().setProp("Hlt1TrackOption","Rerun")


    def _suppressMonitoring(self):
        if self.getProp("UseTCK"):
            trans={}
            trans['.*HltGlobalMonitor']={ 'Enable' : { "^.*$": "False" } }
            trans['.*BeetleSyncMonitor.*']={ 'Enable' : { "^.*$": "False" } }
            trans['HltL0GlobalMonitor']={ 'Enable' : { "^.*$": "False" } }
            Funcs._mergeTransform(trans)
        else:
            hltConf = HltConf()
            hltConf.setProp("EnableHltGlobalMonitor",False)
            hltConf.setProp("EnableBeetleSyncMonitor",False)
            hltConf.setProp("EnableHltL0GlobalMonitor",False)

    def _config_with_tck(self):
        from Configurables import HltConfigSvc
        from Funcs import _tck
        cfg = HltConfigSvc( prefetchDir = MooreExpert().getProp('prefetchConfigDir')
                          , initialTCK =  _tck(self.getProp('InitialTCK'))
                          , checkOdin = self.getProp('CheckOdin')
                          , ConfigAccessSvc = self.getConfigAccessSvc().getFullName() )

        ApplicationMgr().ExtSvc.insert(0,cfg.getFullName())
        # configure services...
        from Configurables import VFSSvc
        VFSSvc().FileAccessTools = ['FileReadTool', 'CondDBEntityResolver/CondDBEntityResolver'];
        from Configurables import LHCb__ParticlePropertySvc
        LHCb__ParticlePropertySvc().ParticlePropertiesFile = 'conddb:///param/ParticleTable.txt';


    def _definePersistency(self):

        #online, do the minimum possible, of only setting up MDF
        if self.getProp("RunOnline") :
            LHCbApp().setProp("Persistency","MDF")

    def _split(self, useTCK ):
        split=self.getProp("Split")
        # rather nasty way of doing this.. but it is 'hidden'
        # if you're reading this: don't expect this to remain like this!!!
        if split not in [ "", "Hlt1", "Hlt2" ]:
            raise ValueError("Invalid option for Moore().Split: '%s'"% split )
        if useTCK and split:
            trans = dict()
            # Hlt1 transform: remove items starting with Hlt2
            #                 enable track reports writers
            #                 remove lumi stripper...
            trans['Hlt1'] = { 'GaudiSequencer/Hlt(Decision|Monitor)Sequence$' : { 'Members' : { ",[^']*'[^/]*/Hlt(2|Afterburner)[^']*'" : "" } }
                            , 'HltTrackReportsWriter/.*'                      : { 'Enable'  : { "^.*$" : 'True' } }
                            , 'GaudiSequencer/HltEndSequence'                 : { 'Members' : { ", 'GaudiSequencer/LumiStripper'": "" } }
            }
            ### Hlt2 transform: remove all algorithms starting with Hlt1
            ###                 enable various reports decoders
            ###                 TODO: remove/disable the 'producers'
            ###                 TODO: if running 'independent' somehow replace track decoder with velo reco ...
            trans['Hlt2'] = { 'GaudiSequencer/Hlt(Decision|Monitor|End)Sequence$' : { 'Members' : { "'[^'/]*/Hlt1[^']*'[^,]*," : ""  } }
                            , 'HltTrackReportsDecoder/.*'                         : { 'Enable' : { '^.*$' : '%s' % ( not MooreExpert().getProp("Hlt2Independent") ) } }
                            , 'HltSelReportsDecoder/.*'                           : { 'Enable' : { '^.*$' : 'True' } }
                            , 'HltDecReportsDecoder/.*'                           : { 'Enable' : { '^.*$' : 'True' } }
            }
            Funcs._mergeTransform(trans[split])
            # Tell the HltConfigSvc that we will only be running
            # one level of the HLT
            from Configurables import HltConfigSvc
            from DAQSys.Decoders import DecoderDB
            # make sure the HltConfigSvc (if used) puts the TCK it used in the relevant HltDecReports
            HltConfigSvc().HltDecReportsLocations = [ DecoderDB["HltDecReportsDecoder/%sDecReportsDecoder" % split].listOutputs()[0] ]

        #check/set WriterRequires, not really needed since now the content of Hlt1/2 is modified in the HltDecisionSequence
        if self.isPropertySet('WriterRequires') and len(self.getProp('WriterRequires')) and len(split):
            for check,fail in [("Hlt1","Hlt2"),("Hlt2","Hlt1")]:
                if split==check:
                    if len([w for w in self.getProp("WriterRequires") if fail in w]):
                        raise ValueError("You have set WriterRequires to something, but that thing cannot be guaranteed to be there in the split scenario! We are splitting into: "+self.getProp('Split')+" and you have asked for: "+self.getProp('WriterRequires').__str__())


    def _setIfNotSet(self,prop,value) :
        if not self.isPropertySet(prop) : self.setProp(prop,value)
        return self.getProp(prop) == value

    def _throwIfNotSet(self,conf,prop,set,reason):
        if not conf.getProp(prop)==set:
            raise AttributeError("You've set mutually exclusive settings! "+prop+":"+str(conf.getProp(prop))+" should be "+str(set)+" "+reason)

    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        # verify mutually exclusive settings:
        #  eg.  Online vs. any L0 setting
        #       Online vs. generateConfig
        #       Online vs. DB tags...
        #       Online vs. EvtMax, SkipEvents, DataType, ...
        #       Online requires UseTCK
        # L0 decoding to look in a single place
        # L0Conf().RawEventLocations = ['DAQ/RawEvent']
        #L0DUFromRawAlg("L0DUFromRaw").Hlt1 = True
        deprecationwarning=""
        for prop in self.getProperties():
            if prop in self._propertyDocDct:
                doc=self._propertyDocDct[prop]
                if "DEPRECATED" in doc and self.isPropertySet(prop):
                    deprecationwarning=deprecationwarning+"You are trying to set the deprecated property Moore()." + prop
                    if " use " in doc and len(doc[doc.find("use"):].strip()):
                        deprecationwarning=deprecationwarning+" please "+doc[doc.find("use"):].strip()+" instead"
                    deprecationwarning=deprecationwarning+"\n"
        if len(deprecationwarning.strip()):
            raise DeprecationWarning(deprecationwarning.strip())

        #check nothing strange is set for running online
        if self.getProp("RunOnline"):
            #things
            for prop,set in [("generateConfig",False),("EvtMax",-1)]:
                self._throwIfNotSet(self,prop,set," because you're running in Online mode!")
            for prop,set in [("DisableMonitors",False)]:
                self._throwIfNotSet(MooreExpert(),prop,set," because you're running in Online mode!")

        #check nothing strange is running to generate a TCK
        if self.getProp("generateConfig"):
            for prop,set in [("Hlt2Independent",False)]:
                self._throwIfNotSet(MooreExpert(),prop,set," because you're trying to generate a TCK!")
            for prop,set in [("OutputLevel",WARNING),("UseTCK",False)]:
                self._throwIfNotSet(self,prop,set," because you're trying to generate a TCK!")


        if not self.getProp("RunOnline") : self._l0()

        from Configurables import MooreInitSvc
        ApplicationMgr().ExtSvc.append( MooreInitSvc() )
        #from Configurables import LbAppInit
        #ApplicationMgr().TopAlg.append(LbAppInit(PreloadGeometry=True))
        ### Could be that MooreOnline has already populated the TopAlg with the Hlt Sequencer...
        hlt = GaudiSequencer('Hlt')
        if hlt not in ApplicationMgr().TopAlg : ApplicationMgr().TopAlg.append( hlt )


        # forward some settings...
        # WARNING: this triggers setup of /dd -- could be avoided in PA only mode...
        app = LHCbApp()
        self.setOtherProps( app, ['EvtMax','SkipEvents','Simulation', 'DataType' ] )

        # this was a hack. Someone thought setOtherProps did not work?
        #app.CondDBtag = self.getProp('CondDBtag')
        #app.DDDBtag   = self.getProp('DDDBtag')
        self.setOtherProps( app, ['CondDBtag','DDDBtag'])
        # Get the event time (for CondDb) from ODIN
        from DAQSys.Configuration import SetEvtClock
        SetEvtClock("ODIN")
        #from Configurables import EventClockSvc
        #import time
        #EventClockSvc().InitialTime = int(time.time()*1e9)  #now
        from Configurables import DDDBConf
        if not DDDBConf().isPropertySet("InitialTime"):
            DDDBConf().InitialTime="Now"

        # make sure we don't pick up small variations of the read current
        # Need a defined HistogramPersistency to read some calibration inputs!!!
        ApplicationMgr().HistogramPersistency = 'ROOT'

        #set the decoders to read from the default location
        self._setRawEventLocations()

        self._configureDQTags()

        if self.getProp('UseTCK') :
            self._config_with_tck()
            self._split( useTCK = True )
        else:
            self._config_with_hltconf()
            self._split( useTCK = False  )

        self._definePersistency()
        self._configureDataOnDemand()

        if self.getProp("Simulation") or MooreExpert().getProp("DisableMonitors"):
            self._suppressMonitoring()

        if not self.getProp("RunOnline") :
            self._profile()
            if self.getProp("generateConfig") : self._generateConfig()
            self._configureInput()
            self._configureOutput()
        #last thing, set output levels
        self._outputLevel()
