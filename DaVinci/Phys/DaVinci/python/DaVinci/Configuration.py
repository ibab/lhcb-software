"""
High level configuration tools for DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.18 2008-12-18 09:57:52 pkoppenb Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
from Configurables import ( LHCbConfigurableUser, LHCbApp, PhysConf, AnalysisConf )
import GaudiKernel.ProcessJobOptions

class DaVinci(LHCbConfigurableUser) :
    __slots__ = {
         "EvtMax"          :  -1     # Number of events to analyse
       , "SkipEvents"      :   0     # Number of events to skip at beginning for file
       , "PrintFreq"       : 100     # The frequency at which to print event numbers
       , "DataType"        : 'DC06'  # Data type, can be ['DC06','2008'] Forwarded to PhysConf
       , "Simulation"      : True    # set to True to use SimCond. Forwarded to PhysConf
       , "DDDBtag"         : ""      # Tag for DDDB. Default as set in DDDBConf for DataType
       , "CondDBtag"       : ""      # Tag for CondDB. Default as set in DDDBConf for DataType
       , "UseOracle"       : False   # if False, use SQLDDDB instead
       , "RedoMCLinks"     : False   # One some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association.
       , "HistogramFile"   : ""      # Write name of output Histogram file
       , "TupleFile"       : ""      # Write name of output Tuple file
       , "ETCFile"         : ""      # Write name of output ETC file @todo Not yet implemented
       , "DstFile"         : ""      # Write name of output DST file @todo Not yet implemented
       , "MainOptions"     : ""      # Main option file to execute
       , "Input"           : []      # Input data. Can also be passed as a second option file.
       , "UserAlgorithms"  : []      # User algorithms to run.
        }

    __used_configurables__ = [ LHCbApp, PhysConf, AnalysisConf ]


    def checkOptions(self):
        """
        Does nothing but checking that all is fine
        """
        dataType = self.getProp("DataType")
        if dataType not in [ "DC06", "2008" ]:
            raise TypeError( "Invalid dataType '%s'"%dataType )

    def outputFiles(self):
        """
        output files
        """
        if ( self.getProp("HistogramFile") != "" ):
            HistogramPersistencySvc().OutputFile = self.getProp("HistogramFile")
        if ( self.getProp("TupleFile") != "" ):
            tupleFile = self.getProp("TupleFile")
            ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]
            NTupleSvc().Output =  [ "FILE1 DATAFILE='"+tupleFile+"' TYP='ROOT' OPT='NEW'" ]
        if ( self.getProp("ETCFile") != "" ):
            raise TypeError( "ETC not yet implemented in configurables" )
        if ( self.getProp("DstFile") != "" ):
            raise TypeError( "DST not yet implemented in configurables" )
        
#
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
        EventSelector().PrintFreq = -1;
#        ToolSvc().SequencerTimerTool.OutputLevel = 4;  // suppress SequencerTimerTool printout

    def mainSeq(self):
        """
        Initialisation sequence
        """
        from Configurables import (GaudiSequencer, LbAppInit)
        init = GaudiSequencer("DaVinciInitSeq")
        ApplicationMgr().TopAlg = [ init ]  # note the = here
        init.Members += [ LbAppInit("DaVinciAppInit") ]
        init.IgnoreFilterPassed = True
        if ( self.getProp("RedoMCLinks") ) :
            AnalysisConf().redoMCLinks(init)

    def defineEvents(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])

    def defineInput(self):
        input = self.getProp("Input")
#        print "DaVinci input is ", input
        if ( len(input) > 0) :
#            print "Re-defining input to\n", input
            EventSelector().Input = input
#        else :
#            print "Input is\n", getattr(EventSelector(),"Input")

    def evtMax(self):
        return LHCbApp().evtMax()
    
    def defineDB(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["DataType","CondDBtag","DDDBtag","UseOracle","Simulation"])

    def standardParticles(self):
        """
        define standard particles on DoD service
        """
        importOptions("$COMMONPARTICLESROOT/options/StandardOptions.opts")
        if (  self.getProp("DataType")=='DC06'):
            importOptions("$COMMONPARTICLESROOT/options/StandardDC06Options.opts")

    def __apply_configuration__(self):
        print "# applying DaVinci configuration"
        self.checkOptions()
        importOptions("$STDOPTS/LHCbApplication.opts") # to get units in .opts files
        self.defineMonitors()
        self.setOtherProps(PhysConf(),["DataType","Simulation"])
        self.setOtherProps(AnalysisConf(),["DataType","Simulation"])
#       @todo Remove this from Common and put it here
#        if ( self.getProp( "DataType" ) == 'DC06' ):
#            importOptions ("$DAVINCIROOT/options/DaVinciProtoPCalibrate.opts")
#        if ( self.getProp( "DataType" ) != 'DC06' ):
#            GaudiSequencer("ProtoPRecalibration").Members = []
        self.defineEvents()
        self.defineInput()
        self.defineDB()
        self.mainSeq()
        self.standardParticles()
        
        opts = self.getProp( "MainOptions" )
        if not (opts == '') :
            importOptions( self.getProp( "MainOptions" ) )
        else :
            log.warning("No MainOptions specified. DaVinci() will import no options file!")
        for alg in self.getProp("UserAlgorithms"):
            ApplicationMgr().TopAlg += [ alg ]

