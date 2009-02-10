"""
High level configuration tools for DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.48 2009-02-10 12:07:35 pkoppenb Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
from Configurables import ( LHCbConfigurableUser, LHCbApp, PhysConf, AnalysisConf, HltConf, DstConf )
import GaudiKernel.ProcessJobOptions

class DaVinci(LHCbConfigurableUser) :
    
    __slots__ = {
        # Application Configuration : sent to LHCbApp and Gaudi
         "EvtMax"             :  -1           # Number of events to analyse
       , "SkipEvents"         :   0           # Number of events to skip at beginning for file
       , "PrintFreq"          : 100           # The frequency at which to print event numbers
       , "DataType"           : 'DC06'        # Data type, can be ['DC06','2008'] Forwarded to PhysConf
       , "Simulation"         : True          # set to True to use SimCond. Forwarded to PhysConf
       , "DDDBtag"            : "default"     # Tag for DDDB. Default as set in DDDBConf for DataType
       , "CondDBtag"          : "default"     # Tag for CondDB. Default as set in DDDBConf for DataType
       , "UseOracle"          : False         # if False, use SQLDDDB instead
         # Input
       , "Input"              : []            # Input data. Can also be passed as a second option file.
         # Output
       , "HistogramFile"      : ""            # Write name of output Histogram file
       , "TupleFile"          : ""            # Write name of output Tuple file
       , "ETCFile"            : ""            # Name of ETC file
       , "ETCItemList"        : []            # ItemList to put on ETC
         # Monitoring
       , "MoniSequence"       : []            # Add your monitors here
         # DaVinci Options
       , "MainOptions"        : ""            # Main option file to execute
       , "UserAlgorithms"     : []            # User algorithms to run.
       , "RedoMCLinks"        : False         # On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association.
       , "InputType"          : "DST"         # or "DIGI" or "ETC" or "RDST" or "DST". Nothing means the input type is compatible with being a DST. 
         # Hlt running
       , "HltType"            : ''            # HltType : No Hlt. Use Hlt1+Hlt2 to run Hlt
       , "HltUserAlgorithms"  : [ ]           # put here user algorithms to add
       , "ReplaceL0BanksWithEmulated" : False # Re-run L0 
       , "Hlt2IgnoreHlt1Decision" : False     # run Hlt2 even if Hlt1 failed
       }

    _propertyDocDct = {  
         "EvtMax"             : """ Number of events to analyse """
       , "SkipEvents"         : """ Number of events to skip at beginning for file """
       , "PrintFreq"          : """ The frequency at which to print event numbers """
       , "DataType"           : """ Data type, can be ['DC06','2008'] Forwarded to PhysConf """
#       , "PackType"           : """ Type of packing for the DST: ['NONE','TES','MDF'] """
       , "Simulation"         : """ set to True to use SimCond. Forwarded to PhysConf """
       , "DDDBtag"            : """ Tag for DDDB. Default as set in DDDBConf for DataType """
       , "CondDBtag"          : """ Tag for CondDB. Default as set in DDDBConf for DataType """
       , "UseOracle"          : """ if False, use SQLDDDB instead """
       , "Input"              : """ Input data. Can also be passed as a second option file. """
       , "HistogramFile"      : """ Write name of output Histogram file """
       , "TupleFile"          : """ Write name of output Tuple file """
       , "ETCFile"            : """ Write name of output ETC file."""
       , "ETCItemList"        : """ ItemList ot put on ETC """
       , "MainOptions"        : """ Main option file to execute """
       , "UserAlgorithms"     : """ User algorithms to run. """
       , "RedoMCLinks"        : """ On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association. """
       , "InputType"          : """ 'DST' or 'DIGI' or 'ETC' or 'RDST' or 'DST'. Nothing means the input type is compatible with being a DST.  """
         # Hlt running
       , "HltType"            : """ HltType : No Hlt by default. Use Hlt1+Hlt2 to run Hlt """
       , "HltUserAlgorithms"  : """ Put here user algorithms to add to Hlt """
       , "ReplaceL0BanksWithEmulated" : """ Re-run L0  """
       , "Hlt2IgnoreHlt1Decision" : """ Run Hlt2 even if Hlt1 failed """
         }

    __used_configurables__ = [ LHCbApp, PhysConf, AnalysisConf, HltConf, DstConf ]

    ## Known monitoring sequences run by default
    KnownMonitors        = []    

################################################################################
# Configure slaves
#
    def configureSubPackages(self):
        """
        Define DB and so on
        """
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["DataType","UseOracle","Simulation"])
        LHCbApp.DDDBtag = self.getProp("DDDBtag")
        LHCbApp.CondDBtag = self.getProp("CondDBtag")
        log.info("Set DDDBtag "+self.getProp("DDDBtag"))
        self.setOtherProps(PhysConf(),["DataType","Simulation","InputType"])
        self.setOtherProps(AnalysisConf(),["DataType","Simulation"])

################################################################################
# Event Initialisation sequence
#
    def initSeq(self):
        """
        Initialisation sequence
        """
        from Configurables import (GaudiSequencer, LbAppInit, PhysConf, AnalysisConf)
        init = GaudiSequencer("DaVinciInitSeq")
        log.info("Resetting ApplicationMgr")
        ApplicationMgr().TopAlg = [ init ]  # Note the = here
        init.Members += [ LbAppInit("DaVinciAppInit") ]
        init.IgnoreFilterPassed = True
        # Phys
        inputType = self.getProp( "InputType" ).upper()
        if (( inputType != "MDF" ) & (inputType != "DIGI")) :
            physinit = PhysConf().initSequence()         # PhysConf initSequence
            init.Members += [ physinit ]
            # Analysis
            AnalysisConf().RedoMCLinks = self.getProp("RedoMCLinks") 
            analysisinit = AnalysisConf().initSequence()
            init.Members += [ analysisinit ]

################################################################################
# HLT setup
#
    def hlt(self):
        """
        Define HLT. Make sure it runs first.
        """
        HltConf().replaceL0BanksWithEmulated = self.getProp("ReplaceL0BanksWithEmulated") ## enable if you want to rerun L0
        HltConf().Hlt2IgnoreHlt1Decision =  self.getProp("Hlt2IgnoreHlt1Decision")        ## enable if you want Hlt2 irrespective of Hlt1
        HltConf().hltType =  self.getProp("HltType")                                      ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
        from Configurables import (GaudiSequencer, LbAppInit)
        hltSeq = GaudiSequencer("Hlt")
        ApplicationMgr().TopAlg += [ hltSeq ]  # catch the Hlt sequence to make sur it's run first
        
        
################################################################################
# Check Options are OK
#
    def checkOptions(self):
        """
        Does nothing but checking that all is fine
        """
        dataType = self.getProp("DataType")
        if dataType not in [ "DC06", "2008" ]:
            raise TypeError( "Invalid dataType '%s'"%dataType )
        inputType = self.getProp( "InputType" ).upper()
        if inputType not in [ "MDF", "DST", "DIGI", "ETC", "RDST" ]:
            raise TypeError( "Invalid inputType '%s'"%inputType )
        # DST packing, not for  DC06
#        if ( self.getProp("DataType") == "DC06" ):
#            self.setProp('PackType', 'NONE') 

################################################################################
# @todo Stolen from Brunel. Could be common to all apps?
#
    def defineMonitors(self):
        """
        Define monitors
        """
        from Configurables import (ApplicationMgr,AuditorSvc,SequencerTimerTool)
        ApplicationMgr().ExtSvc += [ 'ToolSvc', 'AuditorSvc' ]
        ApplicationMgr().AuditAlgorithms = True
        AuditorSvc().Auditors += [ 'TimingAuditor' ]
        SequencerTimerTool().OutputLevel = 4
        MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
        # Do not print event number at every event
        EventSelector().PrintFreq = self.getProp("PrintFreq");

################################################################################
# set EvtMax
#
    def defineEvents(self):
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
    def defineInput(self):
        """
        Define Input
        """
        input = self.getProp("Input")
        print "# DaVinci input is ", input
        if ( len(input) > 0) :
            EventSelector().Input = input
        inputType = self.getProp( "InputType" ).upper()
        if ( inputType == "MDF" ):
            EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
            importOptions("$STDOPTS/DecodeRawEvent.py")

################################################################################
# Tune initialisation
#
    def configureInput(self):
        """
        Tune initialisation 
        """
        # POOL Persistency
        importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")
        # Get the event time (for CondDb) from ODIN
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = "OdinTimeDecoder";
        # DST unpacking, not for DC06
        if ( self.getProp("DataType") != "DC06" ):
            DstConf().EnableUnpack = True

################################################################################
# Monitoring sequence
#
    def moniSequence(self):
        """
        Monitoring sequence
        """
        from Configurables import GaudiSequencer
        if ( len(self.KnownMonitors)+len( self.getProp("MoniSequence") ) > 0 ):
            monSeq = GaudiSequencer("MonitoringSequence")
            monSeq.IgnoreFilterPassed = True 
            monSeq.Members = self.KnownMonitors
            ApplicationMgr().TopAlg += [ monSeq ]
            for alg in self.getProp("MoniSequence"):
                monSeq.Members += [ alg ]

################################################################################
# Ntuple files
#
    def rootFiles(self):
        """
        output files
        """
        ApplicationMgr().HistogramPersistency = "ROOT"
        if ( self.getProp("HistogramFile") != "" ):
            HistogramPersistencySvc().OutputFile = self.getProp("HistogramFile")
            print "# Histos file will be ``", self.getProp("HistogramFile"), "''"
        if ( self.getProp("TupleFile") != "" ):
            tupleFile = self.getProp("TupleFile")
            ApplicationMgr().ExtSvc +=  [ NTupleSvc() ]
            tuple = "FILE1 DATAFILE='"+tupleFile+"' TYP='ROOT' OPT='NEW'"
            print "# Tuple will be in ``", tupleFile, "''"
            NTupleSvc().Output = [ tuple ]
            NTupleSvc().OutputLevel = 1 
        if ( self.getProp("ETCFile") != "" ):
            etcFile = self.getProp("ETCFile")
            self.etc(etcFile,self.getProp("ETCItemList"))
################################################################################
# ETC
#
    def etc(self,etcFile,iList):
        """
        write out an ETC
        """
        from Configurables import TagCollectionSvc
        tcname = "EvtTupleSvc"
        ets = TagCollectionSvc(tcname)
        ets.Output += [ "EVTTAGS DATAFILE='"+etcFile+"' TYP='POOL_ROOTTREE' OPT='RECREATE' " ]
        ets.OutputLevel = 1 
        ApplicationMgr().ExtSvc  += [ ets ]
        #
        from Configurables import EvtCollectionStream
        tagW = EvtCollectionStream("TagWriter")
        # this somehow matches CollectionName
        tagW.ItemList = iList
        tagW.EvtDataSvc = tcname
        
        from Configurables import Sequencer
        seq = Sequencer("SeqWriteTag")
        ApplicationMgr().OutStream += [ tagW ]
        
        print "# ETC will be in ``", etcFile, "''"

################################################################################
# Main sequence
#
    def mainSequence(self):
        """
        Main Sequence
        """
        opts = self.getProp( "MainOptions" )
        if not (opts == '') :
            importOptions( self.getProp( "MainOptions" ) )
        else :
            log.info("No MainOptions specified. DaVinci() will import no options file!")
        log.info("Creating User Algorithms")
        if ( len ( self.getProp("UserAlgorithms") ) > 0 ):
            from Configurables import GaudiSequencer
            mainSeq = GaudiSequencer("DaVinciMainSequence")
            mainSeq.IgnoreFilterPassed = True 
            ApplicationMgr().TopAlg += [ mainSeq ]
            for alg in self.getProp("UserAlgorithms"):
                mainSeq.Members += [ alg ]

################################################################################
# Apply configuration
#
    def __apply_configuration__(self):
        """
        DaVinci configuration
        """
        log.info("Applying DaVinci configuration")
        log.info( self )

        self.checkOptions()
        self.configureSubPackages()
        importOptions("$STDOPTS/LHCbApplication.opts") # to get units in .opts files
        self.configureInput()
        # start with init
        self.initSeq()
        if (self.getProp("HltType")!=''):
            self.hlt()
        self.defineMonitors()
        self.defineEvents()
        self.defineInput()
        self.rootFiles()
        
        # main sequence
        self.mainSequence()
        # monitoring
        self.moniSequence()
