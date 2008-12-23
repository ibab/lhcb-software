"""
High level configuration tools for DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.25 2008-12-23 10:36:06 pkoppenb Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
from Configurables import ( LHCbConfigurableUser, LHCbApp, PhysConf, AnalysisConf, HltConf )
import GaudiKernel.ProcessJobOptions

class DaVinci(LHCbConfigurableUser) :
    __slots__ = {
        # Application Configuration : sent to LHCbApp and Gaudi
         "EvtMax"             :  -1           # Number of events to analyse
       , "SkipEvents"         :   0           # Number of events to skip at beginning for file
       , "PrintFreq"          : 100           # The frequency at which to print event numbers
       , "DataType"           : 'DC06'        # Data type, can be ['DC06','2008'] Forwarded to PhysConf
       , "Simulation"         : True          # set to True to use SimCond. Forwarded to PhysConf
       , "DDDBtag"            : ""            # Tag for DDDB. Default as set in DDDBConf for DataType
       , "CondDBtag"          : ""            # Tag for CondDB. Default as set in DDDBConf for DataType
       , "UseOracle"          : False         # if False, use SQLDDDB instead
         # Input
       , "Input"              : []            # Input data. Can also be passed as a second option file.
         # Output
       , "HistogramFile"      : ""            # Write name of output Histogram file
       , "TupleFile"          : ""            # Write name of output Tuple file
       , "ETCFile"            : ""            # Write name of output ETC file @todo Not yet implemented
       , "DstFile"            : ""            # Write name of output DST file @todo Not yet implemented
         # DaVinci Options
       , "MainOptions"        : ""            # Main option file to execute
       , "UserAlgorithms"     : []            # User algorithms to run.
       , "RedoMCLinks"        : False         # On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association.
         # Hlt running
       , "HltType"            : ''            # HltType : No Hlt. Use Hlt1+Hlt2 to run Hlt
       , "HltUserAlgorithms"  : [ ]           # put here user algorithms to add
       , "HltOldStyle"        : True          # old style options configuration
       , "ReplaceL0BanksWithEmulated" : False # Re-run L0 
       , "Hlt2IgnoreHlt1Decision" : False     # run Hlt2 even if Hlt1 failed
        }

    __used_configurables__ = [ LHCbApp, PhysConf, AnalysisConf, HltConf ]

    def initSeq(self):
        """
        Initialisation sequence
        """
        from Configurables import (GaudiSequencer, LbAppInit)
        init = GaudiSequencer("DaVinciInitSeq")
        ApplicationMgr().TopAlg = [ init ]  # Note the = here 
        init.Members += [ LbAppInit("DaVinciAppInit") ]
        init.IgnoreFilterPassed = True
        PhysConf().InitSeq = init                    # Declare InitSeq in PhysConf 
        AnalysisConf().InitSeq = init                # Declare InitSeq in AnalysisConf
        AnalysisConf().RedoMCLinks =  self.getProp("RedoMCLinks") 

    def hlt(self):
        HltConf().replaceL0BanksWithEmulated = self.getProp("ReplaceL0BanksWithEmulated") ## enable if you want to rerun L0
        HltConf().Hlt2IgnoreHlt1Decision =  self.getProp("Hlt2IgnoreHlt1Decision")        ## enable if you want Hlt2 irrespective of Hlt1
        HltConf().hltType =  self.getProp("HltType")                                      ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
        HltConf().oldStyle = self.getProp("HltOldStyle")                                  ## Go for the new thing
        HltConf().applyConf()                                                             ## don't forget to actually apply the configuration!!!
        
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
        EventSelector().PrintFreq = self.getProp("PrintFreq");
#        ToolSvc().SequencerTimerTool.OutputLevel = 4;  // suppress SequencerTimerTool printout

    def defineEvents(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])

    def defineInput(self):
        input = self.getProp("Input")
        print "# DaVinci input is ", input
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
        ApplicationMgr().ExtSvc +=  [ DataOnDemandSvc() ]            
        importOptions("$COMMONPARTICLESROOT/options/StandardOptions.opts")
        if (  self.getProp("DataType")=='DC06'):
            importOptions("$COMMONPARTICLESROOT/options/StandardDC06Options.opts")

    def __apply_configuration__(self):
        print "# applying DaVinci configuration"
        self.checkOptions()
        importOptions("$STDOPTS/LHCbApplication.opts") # to get units in .opts files
        # start with init
        self.initSeq()
        if (self.getProp("HltType")!=''):
            self.hlt()
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
        self.standardParticles()
        self.outputFiles()
        
        opts = self.getProp( "MainOptions" )
        if not (opts == '') :
            importOptions( self.getProp( "MainOptions" ) )
        else :
            log.warning("No MainOptions specified. DaVinci() will import no options file!")
        for alg in self.getProp("UserAlgorithms"):
            ApplicationMgr().TopAlg += [ alg ]

