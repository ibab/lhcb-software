"""
High level configuration tools for AnalysisConf
"""
__version__ = "$Id: Configuration.py,v 1.4 2008-12-23 09:08:06 pkoppenb Exp $"
__author__ = "Patrick Koppenburg <Patrick.Koppenburg@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
import GaudiKernel.ProcessJobOptions

class AnalysisConf(LHCbConfigurableUser) :
    
    __slots__ = {
          "DataType"        : 'DC06'                             # Data type, can be ['DC06','2008']
        , "Simulation"      : True                               # set to True to use SimCond
        , "InitSeq"         : GaudiSequencer("DaVinciInitSeq")   # Initialisation sequence in the application. Is set from e.g. DaVinci()
        , "RedoMCLinks"     : False                              # On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association. 
         }

    __used_configurables__ = [ LHCbApp ]
#
# BTaggingTool configuration 
#
    def tagging(self):
        importOptions("$FLAVOURTAGGINGROOT/options/BTaggingTool.py")
#
# Set MC
#
    def redoMCLinks(self):
        """
        Redo MC links.
        """
        if ( self.getProp("RedoMCLinks")):
             from Configurables import (GaudiSequencer,TESCheck,EventNodeKiller,TrackAssociator)
             init = self.getProp("InitSeq")
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
# Set MC
#
    def configureMC(self):
        """
        Define DaVinciAssociators. Do MC unpacking.
        """
        importOptions ("$DAVINCIASSOCIATORSROOT/options/DaVinciAssociators.opts")
        DataOnDemandSvc().Nodes += [ "DATA='/Event/MC' TYPE='DataObject'" ]
        DataOnDemandSvc().AlgMap["MC/Particles"] = "UnpackMCParticle"
        DataOnDemandSvc().AlgMap["MC/Vertices"] = "UnpackMCVertex"
#
# Apply configuration
#
    def __apply_configuration__(self):
        """
        Apply configuration for Analysis
        """
        GaudiKernel.ProcessJobOptions.PrintOff()
        if ( self.getProp("Simulation" )):
            self.redoMCLinks()
            self.configureMC()
        self.tagging()
