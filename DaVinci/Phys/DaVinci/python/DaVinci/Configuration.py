"""
High level configuration tools for DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.125 2010-09-10 11:31:19 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer
from Configurables import ( LHCbConfigurableUser, LHCbApp,
                            PhysConf, AnalysisConf,
                            DstConf, L0Conf, LumiAlgsConf )
from LumiAlgs.LumiIntegratorConf import LumiIntegratorConf
import GaudiKernel.ProcessJobOptions

#
##
#
from AnalysisPython.Logger import getLogger
log = getLogger ( 'DaVinci' )

class DaVinci(LHCbConfigurableUser) :
    
    __slots__ = {
        # Application Configuration : sent to LHCbApp and Gaudi
          "EvtMax"             :  -1             # Number of events to analyse
        , "SkipEvents"         :   0             # Number of events to skip at beginning for file
        , "PrintFreq"          : 1000            # The frequency at which to print event numbers
        , "DataType"           : ''              # Data type, can be ['2008','2009','MC09','2010','2011','2012'] Forwarded to PhysConf. MUST be set.
        , "Simulation"         : False           # set to True to use SimCond. Forwarded to PhysConf
        , "DDDBtag"            : ""              # Tag for DDDB. Default as set in DDDBConf for DataType
        , "CondDBtag"          : ""              # Tag for CondDB. Default as set in DDDBConf for DataType
        , "DQFLAGStag"         : ""              # Tag for DQFLAGS. Default as set in DDDBConf for DataType
        # Input
        , "Input"              : []              # Input data. Can also be passed as a second option file.
        , "InputType"          : "DST"           # 'DST', 'DIGI', 'RDST', 'MDST', 'XDST' or 'LDST'. Nothing means the input type is compatible with being a DST. 
        , 'EnableUnpack'       : None            # Explicitly enable/disable unpacking for input data (if specified) 
        # Output
        , "HistogramFile"      : ""              # Name of output Histogram file (set to "" to get no output) 
        , "TupleFile"          : ""              # Name of output Tuple file
        , "WriteFSR"           : True            # Flags whether to write out an FSR
        , "RootCompressionLevel" : "LZMA:6"        # ROOT compression level for ntuple files
        # DQ
        , "IgnoreDQFlags"      : False           # If False (default), process only events with good DQ. 
        #, "IgnoredDQFlags"     : []              # List of particular DQ flags to ignore.
        # Monitoring
        , "MoniSequence"       : []              # Add your monitors here
        # DaVinci Options
        , "MainOptions"        : ""              # Main option file to execute
        , "UserAlgorithms"     : []              # User algorithms to run.
        , "RedoMCLinks"        : False           # On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association.
        , "Lumi"               : False           # Run Lumi accounting (should normally be True for user jobs)
        , "EventPreFilters"    : []
        , "VerboseMessages"    : False           # Turn on some verbose printout
        , "ProductionType"     : "None"          # Sets up options needed in various production modes
        , "RootInTES"          : ""              # RootInTES property propagated to MainSequence in case of MDST input type
        , "Detectors"          : ['Velo','PuVeto','Rich1','Rich2','TT','IT','OT','Spd','Prs','Ecal','Hcal','Muon','Magnet','Tr']
       }

    _propertyDocDct = {  
        "EvtMax"             : """ Number of events to analyse """
        , "SkipEvents"         : """ Number of events to skip at beginning for file """
        , "PrintFreq"          : """ The frequency at which to print event numbers """
        , "DataType"           : """ Data type, can be ['2008','2009','MC09','2010','2011','2012','2015'] Forwarded to PhysConf, AnalysisConf and LHCbApp """
        , "Simulation"         : """ set to True to use SimCond. Forwarded to PhysConf """
        , "DDDBtag"            : """ Tag for DDDB. Default as set in DDDBConf for DataType """
        , "CondDBtag"          : """ Tag for CondDB. Default as set in DDDBConf for DataType """
        , "DQFLAGStag"         : """ Tag for DQFLAGS. Default as set in DDDBConf for DataType """
        , "Input"              : """ Input data. Can also be passed as a second option file. """
        , "InputType"          : """ 'DST', 'DIGI', 'RDST', 'MDST', 'XDST' or 'LDST'. Nothing means the input type is compatible with being a DST.  """
        , 'EnableUnpack'       : """Explicitly enable/disable unpacking for input data (if specified) """
        , "HistogramFile"      : """ Write name of output Histogram file """
        , "TupleFile"          : """ Write name of output Tuple file """
        , 'WriteFSR'           : """ Flags whether to write out an FSR """
        , "RootCompressionLevel" : """ ROOT Compression level for ntuples """
        , 'IgnoreDQFlags'      : """ If False, process only events with good DQ. Default is False """
        #, "IgnoredDQFlags"     : """ List of DQ flags to ignore. Default is empty, so events flagged bad for any reason are rejected (Unless IgnoreDQFlags=True) """
        , "MainOptions"        : """ Main option file to execute """
        , "UserAlgorithms"     : """ User algorithms to run. """
        , "RedoMCLinks"        : """ On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association. """
        , "Lumi"               : """ Run event count and Lumi accounting (should normally be True) """
        , "EventPreFilters"    : """Set of event filtering algorithms to be run before DaVinci initializaton sequence. Only events passing these filters will be processed."""
        , "VerboseMessages"    : """ Enable additional verbose printouts """
        , "ProductionType"     : """ Enables special settings for running in production (e.g. stripping) """
        , "RootInTES"          : """ RootInTES (for uDst input type) """
        , "Detectors"          : """ List of detectors """ 
        }

    __used_configurables__ = [
        PhysConf          ,
        AnalysisConf      ,
        DstConf           ,
        L0Conf            ,
        LumiAlgsConf      ,
        LumiIntegratorConf,
        LHCbApp           ]

    __known_datatypes__  = [ "MC09", "2008", "2009", "2010", "2011", "2012", "2015", "Upgrade" ]
    __known_prod_types__ = [ "None", "Stripping" ] 

    ## Known monitoring sequences run by default
    KnownMonitors        = []

    mainSeq = GaudiSequencer("DaVinciUserSequence")
    moniSeq = GaudiSequencer("MonitoringSequence")

################################################################################
# Check Options are OK
#
    def _checkOptions(self):
        """
        Checks options. Changes a few if needed.
        """
        # Temp hack for DV v30r1 (fix need in LHCbApp, but missed the release)
        if self.getProp("Simulation") or self.getProp("DataType") == "MC09":
            self.setProp("IgnoreDQFlags",True)
        # Production mode ?
        prodType = self.getProp("ProductionType")
        if prodType not in self.__known_prod_types__ :
            raise TypeError( "Invalid ProductionType '%s'" %prodType )
        if "Stripping" == prodType :
            # Need to unpack Brunel information, but disable stripping unpacking
            self.setProp("EnableUnpack",["Reconstruction"])
            # Process all events, good or bad
            self.setProp("IgnoreDQFlags",True)
        dataType = self.getProp("DataType")
        if (not dataType):
            raise TypeError( "You must set DataType" )
        if dataType not in self.__known_datatypes__ :
            raise TypeError( "Invalid DataType '%s'" %dataType )
        inputType = self.getProp( "InputType" ).upper()
        if inputType not in [ "MDF", "DST", "DIGI", "RDST", "MDST", "SDST", "XDST", "LDST" ]:
            raise TypeError( "Invalid inputType '%s'"%inputType )
        if ( dataType in [ "MC09" ] ):
            if not self.getProp("Simulation"):
                log.warning("Setting Simulation = True for "+dataType)
                self.setProp("Simulation",True)
        if ( self.getProp("Simulation") & (inputType != "MDF") & (inputType != "DIGI") & (inputType != "MDST") ):
            redo = self.getProp("RedoMCLinks")
            if (inputType == "RDST")  and (redo) :
                log.warning("Re-doing MC links not possible for RDST")
                self.setProp("RedoMCLinks", False )
        
        if self.getProp("Simulation") and self.getProp('Lumi') :
            log.warning('Lumi not valid for Simulation. Setting Lumi = False')
            self.setProp('Lumi', False )
        
################################################################################
# Configure slaves
#
    def _configureSubPackages(self):
        """
        Define DB and so on
        """
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["DataType","CondDBtag","DDDBtag",
                                      "DQFLAGStag","Simulation","IgnoreDQFlags"])
        self.setOtherProps(PhysConf(),["DataType","Simulation","InputType","Detectors"])
        self.setOtherProps(AnalysisConf(),["DataType","Simulation"])
        self.setOtherProps(DstConf(),["DataType"])
    
    def _analysisSeq(self) :
        return GaudiSequencer('DaVinciAnalysisSeq', IgnoreFilterPassed = True)

    def _filteredEventSeq(self) :
        prefilters = self.getProp('EventPreFilters')
        return GaudiSequencer('FilteredEventSeq',
                              Members = prefilters +  [self._init(), self._analysisSeq()])

################################################################################
# Event Initialisation sequence
#
    def _dvInit(self):
        """
        Initialisation sequence
        """
        from Configurables import (DaVinciInit,
                                   MemoryTool)
        
        di = DaVinciInit('DaVinciInitAlg', Increment=self.getProp("PrintFreq"))
        di.addTool(MemoryTool)
        di.MemoryTool.HistoSize = 5000
        return di

    def _init(self):
        from Configurables import (PhysConf,AnalysisConf)
        
        inputType = self.getProp( "InputType" ).upper()

        initSeqs = []

        if ( (inputType != 'MDST' ) & (inputType != "MDF") & (inputType != "DIGI" )) :
            physinit = PhysConf().initSequence() # PhysConf initSequence
            # Analysis
            AnalysisConf().RedoMCLinks = self.getProp("RedoMCLinks") 
            analysisinit = AnalysisConf().initSequence()
            initSeqs = [physinit,analysisinit]
#        if inputType == 'RDST' :
#            log.info('Setting HltDecReportsDecoders RawEventLocations to ["pRec/RawEvent"]')
#            from DAQSys.Decoders import DecoderDB
#            from DAQSys.DecoderClass import decodersForBank
#            for bank in ["Sel","Dec","Vertex","Track"]:
#                for d in decodersForBank(DecoderDB,"Hlt"+bank+"Reports"):
#                    d.overrideInputs("pRec/RawEvent")
#                    #d.setup()
#            from Configurables import ANNDispatchSvc
#            
#            ANNDispatchSvc().RawEventLocation = "pRec/RawEvent"

        return GaudiSequencer('DaVinciEventInitSeq',
                              Members = initSeqs,
                              IgnoreFilterPassed = True)

################################################################################
# Lumi setup
#
    def _lumi(self):
        """
        read FSR and accumulate event and luminosity data
        calculate normalization - toolname:
        """
        log.info("Creating Lumi Algorithms")
        seq = []
        self.setOtherProps(LumiAlgsConf(),["DataType","InputType"])
        
        # add merge, touch-and-count sequence
        lumiSeq = GaudiSequencer("LumiSeq")
        LumiAlgsConf().LumiSequencer = lumiSeq
        seq += [ lumiSeq ]
        
        if self.getProp( "Lumi" ) :
            tupleFile = self.getProp('TupleFile')
            if tupleFile == '' : log.warning('TupleFile has not been set. No Lumi ntuple will be produced.')
	    # add integrator for normalization
            self.setOtherProps(LumiIntegratorConf(),["InputType","TupleFile","IgnoreDQFlags"])
            lumiInt = GaudiSequencer("IntegratorSeq")
            LumiIntegratorConf().LumiSequencer = lumiInt
            seq += [ lumiInt ]

        return seq
        
################################################################################
# Decode DecReports
#
    def _decReports(self):
        """
        Make L0 reports look just like HLT reports
        """
        from Configurables import L0SelReportsMaker, L0DecReportsMaker
        DataOnDemandSvc().AlgMap["HltLikeL0/DecReports"] = L0DecReportsMaker( OutputLevel = 4 )
        DataOnDemandSvc().AlgMap["HltLikeL0/SelReports"] = L0SelReportsMaker( OutputLevel = 4 )        
        from Configurables import L0Conf
        L0Conf().FullL0MuonDecoding = True
        L0Conf().EnableL0DecodingOnDemand = True
        #HLT already done in DecodersDB
       
        
################################################################################
# @todo Stolen from Brunel. Could be common to all apps?
#
    def _defineMonitors(self):
        """
        Define monitors
        """
        from Configurables import (ApplicationMgr,
                                   AuditorSvc,
                                   SequencerTimerTool)
        
        ApplicationMgr().ExtSvc += [ 'ToolSvc', 'AuditorSvc' ]
        ApplicationMgr().AuditAlgorithms = True
        AuditorSvc().Auditors += [ 'TimingAuditor' ]
        SequencerTimerTool().OutputLevel = 4
        # Do not print event number at every event
        printfreq = self.getProp("PrintFreq")
        if ( printfreq == 0 ):
            log.warning("Print frequence cannot be 0")
            printfreq = 1000
        EventSelector().PrintFreq = printfreq

        # Change the column size of Timing table
        from Configurables import TimingAuditor, SequencerTimerTool
        TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
        if not TimingAuditor().TIMER.isPropertySet("NameSize"):
            TimingAuditor().TIMER.NameSize = 50
        
################################################################################
# set EvtMax
#
    def _defineEvents(self):
        """
        Define number of events
        """
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])
        
################################################################################
# returns evtMax
#
    def evtMax(self):
        """
        Get evtMax
        """
        return LHCbApp().evtMax()
    
################################################################################
# Defines input
#
    def _defineInput(self):
        """
        Define Input
        """
        input = self.getProp("Input")

        if ( len(input) > 0 ) :
            from GaudiConf import IOHelper
            
            persistency=None
            inputType = self.getProp( "InputType" ).upper()
            if inputType == "MDF" : persistency = "MDF"
            #support connection strings and lists of files
            input=IOHelper(persistency,persistency).convertConnectionStrings(input,"I")
            #clear selector to maintain the same behaviour
            IOHelper(persistency,persistency).inputFiles(input, clear=True)
    
################################################################################
# Tune initialisation
#
    def _configureInput(self):
        """
        Tune initialisation 
        """
        
        # Input data type
        inputType = self.getProp( "InputType" ).upper()

        # Get the event time (for CondDb) from ODIN
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = "OdinTimeDecoder";
        
        # if property set explcicitly - use it! 
        if self.isPropertySet('EnableUnpack') :
            unPack = self.getProp('EnableUnpack')
            DstConf  ( EnableUnpack = unPack )
            PhysConf ( EnableUnpack = unPack )
        elif inputType != "MDF":
            defaultUnpacking = ["Reconstruction","Stripping"]
            DstConf  ( EnableUnpack = defaultUnpacking )
            PhysConf ( EnableUnpack = defaultUnpacking )
        if inputType != "MDST" :
            if self.getProp("Simulation") :
                DstConf().setProp("SimType","Full")
        return inputType

################################################################################
# ROOT files
#
    def _rootFiles(self):
        """
        output files
        """
        ApplicationMgr().HistogramPersistency = "ROOT"
        
        if ( self.isPropertySet('HistogramFile') and self.getProp("HistogramFile") != "" ):
            HistogramPersistencySvc().OutputFile = self.getProp("HistogramFile")
            
        if ( self.isPropertySet('TupleFile') and self.getProp("TupleFile") != "" ):
            tupleFile = self.getProp("TupleFile")
            
            ntSvc = NTupleSvc()
            ApplicationMgr().ExtSvc +=  [ ntSvc ]
            for _line in ntSvc.Output :
                if 0 <= _line.find ('FILE1') :
                    log.warning ('Replace NTuple-LUN FILE1: ' + _line ) 
                    ntSvc.Output.remove ( i )
                    
            tupleStr = "FILE1 DATAFILE='%s' TYP='ROOT' OPT='NEW'" % tupleFile
            NTupleSvc().Output      += [ tupleStr ]
            # NTupleSvc().OutputLevel  = 1

        # Set the compression level for the ROOT tuple file
        from GaudiKernel.Configurable import ConfigurableGeneric as RFileCnv
        RFileCnv('RFileCnv').GlobalCompression = self.getProp("RootCompressionLevel")
            
################################################################################
################################################################################
# Main sequence
#
    def _mainSequence(self):
        """
        Main Sequence
        """
        self.mainSeq.IgnoreFilterPassed = True
        self._analysisSeq().Members += [ self.mainSeq ]
        opts = self.getProp( "MainOptions" )
        if not (opts == '') :
            importOptions( self.getProp( "MainOptions" ) )
        else :
            log.info("No MainOptions specified. DaVinci() will import no options file!")
        log.info("Creating User Algorithms")
        self.appendToMainSequence( self.getProp("UserAlgorithms")  )        

################################################################################
# Append to Main sequence
#
    def appendToMainSequence(self, algs):
        """
        Append to main sequence. Can be called from other configurables
        """
        log.info("Append to Main Sequence has been called")
        for alg in algs:
            configurable = self._configurable(alg)
            self.mainSeq.Members += [ configurable ]

################################################################################
# Preppend to Main sequence
#
    def prependToMainSequence(self, algs):
        """
        Preppend to main sequence. Can be called from other configurables
        """
        log.info("Append to Main Sequence has been called")
        addseq = [ ]
        for alg in algs:
            configurable = self._configurable(alg)
            addseq += [ configurable ]
        self.mainSeq.Members = addseq + self.mainSeq.Members

    def _configurable(self, obj) :
        _obj2ConfMap = { 'SelectionSequence'         : lambda x : x.sequence(),
                         'MultiSelectionSequence'    : lambda x : x.sequence() }
        return _obj2ConfMap.get(type(obj).__name__, lambda x : x)(obj)

    ## for MDST input type and if RootInTES is specified, propagate it 
    def _root_in_Tes ( self ) :
        """
        Propagate RootInTES :
        
        - use PhysConf.MicroDST
        - set RooInTES property to all members of user sequence
        
        """
        ## 
        itype = self.getProp   ( 'InputType'  ).upper()
        if not 'MDST' == itype : return  ## no action
        #
        if not self.isPropertySet ('RootInTES') : return  ## no action
        rit = self.getProp('RootInTES')
        if rit in ( '' , '/' , '/Event' , '/Event/' ) : return  ## no action
        #
        ## call uDstConf
        #
        from PhysConf.MicroDST import uDstConf
        uDstConf ( rit , logger = getLogger ( 'PhysConf.MicroDST' )  ) ## use it!
        #
        ## setup RootInTES property for mainSeq
        if not hasattr ( self , 'mainSeq' )  : return
        ms = self.mainSeq

        def _set_root_in_tes_ ( s , root  ) :
            #
            if isinstance ( s , str ) : return
            prps = s.getProperties().keys()
            if 'RootInTES' in prps : 
                if not s.isPropertySet ( 'RootInTES' ) : 
                    log.info ( "Set RootInTES=%s for %s" % ( root , s.getName() ) ) 
                    s.RootInTES = root
            #
            if not 'Members' in prps : return 
            #
            for a in s.Members :
                _set_root_in_tes_ ( a , root ) 

        _set_root_in_tes_ ( ms , rit )

################################################################################
# Monitoring sequence
#
    def _moniSequence(self):
        """
        Monitoring sequence
        """
        self.moniSeq.IgnoreFilterPassed = True 
        self.moniSeq.Members = self.KnownMonitors
        self._analysisSeq().Members += [ self.moniSeq ]
        log.info("Creating Moni Algorithms")
        self.appendToMoniSequence( self.getProp("MoniSequence") )

################################################################################
# Append to Moni sequence
#
    def appendToMoniSequence(self, algs):
        """
        Append to moni sequence. Can be called from other configurables
        """
        log.info("Append to Moni Sequence has been called")
        for alg in algs:
            self.moniSeq.Members += [ alg ]

    def sequence(self) :
        return GaudiSequencer('DaVinciEventSeq',
                              IgnoreFilterPassed = True)


    def _setAncestorDepth(self) :
        """
        Calculate depth of ancestry for input files and set IODataManager().AgeLimit accordingly.
        Set to 1 for InputType = 'RDST' or 'SDST' and 0 for others.
        """
        if not IODataManager().isPropertySet('AgeLimit') :
            depthMap = { 'RDST' : 1,
                         'SDST' : 1  }
            inputType = self.getProp('InputType').upper()
            IODataManager().setProp('AgeLimit', depthMap.get(inputType, 0))
 
################################################################################
# Apply configuration
#
    def __apply_configuration__(self):
        """
        DaVinci configuration
        """
        log.info("Applying DaVinci configuration")
        log.info( self )
        
        self._checkOptions()
        
        self._setAncestorDepth()
        
        ApplicationMgr().TopAlg = [self.sequence()]
        self._configureSubPackages()
        importOptions("$STDOPTS/PreloadUnits.opts") # to get units in .opts files
        inputType = self._configureInput()
        self.sequence().Members = [ self._dvInit() ]
        self.sequence().Members += [self._filteredEventSeq()]
        
        # _lumi handles event count, lumi integration and merging independently
        self.sequence().Members += self._lumi()
            
        # Dec reports
        self._decReports()

        # Printouts ...
        verbosePrint = self.getProp("VerboseMessages")
        from Configurables import LoKiSvc
        LoKiSvc().Welcome = verbosePrint
        from Configurables import DataOnDemandSvc
        DataOnDemandSvc().Dump = verbosePrint
        if not verbosePrint :
            msgSvc = getConfigurable("MessageSvc")
            msgSvc.setWarning += ['RFileCnv'] 
        
        self._defineMonitors()
        self._defineEvents()
        self._defineInput()
        self._rootFiles()
        
        # Add main sequence to TopAlg
        self._mainSequence ()
        # monitoring
        self._moniSequence ()

        self._root_in_Tes  () 
