"""
High level configuration tools for DaVinciApp
"""
__version__ = "$Id: Configuration.py,v 1.15 2008-12-11 09:37:19 pkoppenb Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
import GaudiKernel.ProcessJobOptions

class DaVinci(LHCbConfigurableUser) :
    __slots__ = {
        "EvtMax"          :  -1,
        "SkipEvents"      :   0,
        "DataType"        : 'DC06', # Data type, can be ['DC06','2008']
        "Simulation"      : True,  # set to True to use SimCond
        "MainOptions"     : '',
        "Input"           : [],
        "UserAlgorithms"  : []
        }

    __used_configurables__ = [ LHCbApp ]


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
        self.setOtherProps(LHCbApp(),["DataType","Simulation"]) 
            
    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions ("$DAVINCIROOT/options/DaVinciCommon.py")
#       @todo Remove this from Common and put it here
#        if ( self.getProp( "DataType" ) == 'DC06' ):
#            importOptions ("$DAVINCIROOT/options/DaVinciProtoPCalibrate.opts")
        if ( self.getProp( "DataType" ) != 'DC06' ):
            GaudiSequencer("ProtoPRecalibration").Members = []
        
        opts = self.getProp( "MainOptions" )
        if not (opts == '') :
            importOptions( self.getProp( "MainOptions" ) )
        else :
            log.warning("No MainOptions specified. DaVinci() will import no options file!")
        self.defineEvents()
        self.defineInput()
        self.defineDB()
        for alg in self.getProp("UserAlgorithms"):
            ApplicationMgr().TopAlg += [ alg ]
