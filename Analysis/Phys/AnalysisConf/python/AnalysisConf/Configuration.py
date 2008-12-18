"""
High level configuration tools for AnalysisConf
"""
__version__ = "$Id: Configuration.py,v 1.2 2008-12-18 09:59:19 pkoppenb Exp $"
__author__ = "Patrick Koppenburg <Patrick.Koppenburg@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
import GaudiKernel.ProcessJobOptions

class AnalysisConf(LHCbConfigurableUser) :
    __slots__ = {
         "DataType"        : 'DC06' # Data type, can be ['DC06','2008']
        ,"Simulation"      : True   # set to True to use SimCond
        }

    __used_configurables__ = [ LHCbApp ]
#
# Redo all MC links. To be called from main application.
#
    def redoMCLinks(init,self):
        """
        Redo MC links
        """
        from Configurables import (GaudiSequencer,TESCheck,EventNodeKiller,TrackAssociator)
        mcLinkSeq = GaudiSequencer("RedoMCLinks")
        tescheck = TESCheck("DaVinciEvtCheck")
        tescheck.Inputs = ["Link/Rec/Track/Best"]
        tescheck.Stop = False
        tescheck.OutputLevel = 5
        evtnodekiller = EventNodeKiller("DaVinciEvtNodeKiller")
        evtnodekiller.Nodes = ["Link/Rec"]
        mcLinkSeq.Members = [ tescheck, evtnodekiller, TrackAssociator() ]
        init.Members += [ mcLinkSeq ]
#
# Apply configuration
#
    def __apply_configuration__(self):
        """
        Apply configuration for Analysis
        """
        GaudiKernel.ProcessJobOptions.PrintOff()
        if ( self.getProp("Simulation" )):
            importOptions ("$DAVINCIASSOCIATORSROOT/options/DaVinciAssociators.opts")
            importOptions ("$ANALYSISCONFROOT/options/UnpackMC.py")
