"""
High level configuration tools for DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.56 2009-03-10 10:45:50 pkoppenb Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
from Configurables import ( LHCbConfigurableUser, LHCbApp, PhysConf, AnalysisConf, HltConf, DstConf, L0Conf )
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
         # Input
       , "Input"              : []            # Input data. Can also be passed as a second option file.
         # Output
       , "HistogramFile"      : ""            # Write name of output Histogram file
       , "TupleFile"          : ""            # Write name of output Tuple file
       , "ETCFile"            : ""            # Name of ETC file
         # Monitoring
       , "MoniSequence"       : []            # Add your monitors here
         # DaVinci Options
       , "MainOptions"        : ""            # Main option file to execute
       , "UserAlgorithms"     : []            # User algorithms to run.
       , "RedoMCLinks"        : False         # On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association.
       , "InputType"          : "DST"         # or "DIGI" or "ETC" or "RDST" or "DST". Nothing means the input type is compatible with being a DST. 
         # Trigger running
       , "L0"                 : False         # Run L0. 
       , "ReplaceL0BanksWithEmulated" : False # Re-run L0 
       , "HltType"            : ''            # HltType : No Hlt. Use Hlt1+Hlt2 to run Hlt
       , "HltUserAlgorithms"  : [ ]           # put here user algorithms to add
       , "Hlt2Requires"       : 'L0+Hlt1'     # Say what Hlt2 requires
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
       , "Input"              : """ Input data. Can also be passed as a second option file. """
       , "HistogramFile"      : """ Write name of output Histogram file """
       , "TupleFile"          : """ Write name of output Tuple file """
       , "ETCFile"            : """ Write name of output ETC file."""
       , "MainOptions"        : """ Main option file to execute """
       , "UserAlgorithms"     : """ User algorithms to run. """
       , "RedoMCLinks"        : """ On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association. """
       , "InputType"          : """ 'DST' or 'DIGI' or 'ETC' or 'RDST' or 'DST'. Nothing means the input type is compatible with being a DST.  """
       , "L0"                 : """ Re-Run L0 """
       , "ReplaceL0BanksWithEmulated" : """ Re-run L0 and replace all data with emulation  """
       , "HltType"            : """ HltType : No Hlt by default. Use Hlt1+Hlt2 to run Hlt """
       , "HltUserAlgorithms"  : """ Put here user algorithms to add to Hlt """
       , "Hlt2Requires"       : """ Definition of what Hlt2 requires to run. Default is 'L0+Hlt1'.
                                    'L0' will require only L0, '' (empty string) will run on all events. 'Hlt1' without L0 does not make any sense.
                                    """
         }

    __used_configurables__ = [ PhysConf, AnalysisConf, HltConf, DstConf, L0Conf, LHCbApp ]

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
#        self.setOtherProps(LHCbApp(),["DataType","CondDBtag","DDDBtag","Simulation"])
        self.setOtherProps(LHCbApp(),["DataType","Simulation"])
        # that's a hack. Why does setOtherProps not work?
        LHCbApp().CondDBtag = self.getProp("CondDBtag")
        LHCbApp().DDDBtag = self.getProp("DDDBtag")
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
        from Configurables import (GaudiSequencer, DaVinciInit, PhysConf, AnalysisConf, MemoryTool)
        init = GaudiSequencer("DaVinciInitSeq")
        log.info("Resetting ApplicationMgr")
        ApplicationMgr().TopAlg = [ init ]  # Note the = here
#        init.Members += [ LbAppInit("DaVinciAppInit") ]
        di = DaVinciInit()
        di.addTool(MemoryTool)
        di.MemoryTool.HistoSize = 5000
        init.Members += [ DaVinciInit() ]
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
        if (self.getProp("HltType")!=''):
            HltConf().Hlt2Requires =  self.getProp("Hlt2Requires")                             ## enable if you want Hlt2 irrespective of Hlt1
            HltConf().hltType =  self.getProp("HltType")                                       ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
            from Configurables import (GaudiSequencer, LbAppInit)
            hltSeq = GaudiSequencer("Hlt")
            ApplicationMgr().TopAlg += [ hltSeq ]  # catch the Hlt sequence to make sur it's run first
            log.info("Will run Hlt")
        
        
################################################################################
# L0 setup
#
    def l0(self):
        """
        Define L0. Make sure it runs before HLT.
        """
        if ( (self.getProp("HltType")!='') and (self.getProp("DataType") == 'DC06') and (not self.getProp("L0")) ):
            log.warning("It is mandatory to run the L0 emulation on DC06 data to get the HLT to work correctly")
            log.warning("Will set ReplaceL0BanksWithEmulated = True")
            self.setProp("ReplaceL0BanksWithEmulated",True) 
        if ( self.getProp("ReplaceL0BanksWithEmulated") and (not self.getProp("L0")) ):
            log.warning("You asked to replace L0 banks with emulation. Will set L0 = True")
            self.setProp("L0",True)
        if ( self.getProp("L0") ):
            l0seq = GaudiSequencer("seqL0")
            ApplicationMgr().TopAlg += [ l0seq ]
            L0Conf().setProp( "L0Sequencer", l0seq )
            L0Conf().setProp( "ReplaceL0BanksWithEmulated", self.getProp("ReplaceL0BanksWithEmulated") ) 
            log.info("Will run L0")
        
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
#        MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
        # Do not print event number at every event
        printfreq = self.getProp("PrintFreq")
        if ( printfreq == 0 ):
            log.warning("Print frequence cannot be 0")
            printfreq = 100
        EventSelector().PrintFreq = printfreq

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
            self.etc(self.getProp("ETCFile"))
################################################################################
# ETC
#
    def etc(self,etcFile):
        """
        write out an ETC
        """
        from Configurables import TagCollectionSvc
        tcname = "EvtTupleSvc"
        ets = TagCollectionSvc(tcname)
        ets.Output += [ "EVTCOL DATAFILE='"+etcFile+"' TYP='POOL_ROOTTREE' OPT='RECREATE' " ]
        ets.OutputLevel = 1 
        ApplicationMgr().ExtSvc  += [ ets ]
        #
        from Configurables import EvtCollectionStream
        tagW = EvtCollectionStream("TagWriter")
        # this somehow matches CollectionName
        log.info("Did not defined itemlist for ETC.")
#        tagW.ItemList = iList
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
        importOptions("$STDOPTS/PreloadUnits.opts") # to get units in .opts files
        self.configureInput()
        # start with init
        self.initSeq()
        self.l0()
        self.hlt()
        self.defineMonitors()
        self.defineEvents()
        self.defineInput()
        self.rootFiles()
        
        # main sequence
        self.mainSequence()
        # monitoring
        self.moniSequence()
