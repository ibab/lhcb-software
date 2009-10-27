"""
High level configuration tools for AnalysisConf
"""
__version__ = "$Id: Configuration.py,v 1.19 2009-10-27 15:42:08 jpalac Exp $"
__author__ = "Patrick Koppenburg <Patrick.Koppenburg@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions

class AnalysisConf(LHCbConfigurableUser) :
    
    __slots__ = {
          "DataType"        : 'MC09'                             # Data type, can be ['DC06','2008','2009','MC09']
        , "Simulation"      : True                               # set to True to use SimCond
        , "RedoMCLinks"     : False                              # On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association. 
         }

#
# configure reconstruction to be redone
#
    def initSequence(self):
        """
        Init Sequence. Called by master application.
        """
        # only one initialisiation do far
        from Configurables import GaudiSequencer
        init = GaudiSequencer("AnalysisInitSeq")
        self.redoMCLinks(init)
        return init
        
#
# BTaggingTool configuration 
#
    def tagging(self):
        importOptions("$FLAVOURTAGGINGROOT/options/BTaggingTool.py")
        
#
# Set MC
#
    def redoMCLinks(self,init):
        """
        Redo MC links.
        """
        
        if ( self.getProp("Simulation") ):
            redo = self.getProp("RedoMCLinks")
            if ( redo ):
                from Configurables import (GaudiSequencer,TESCheck,EventNodeKiller,TrackAssociator)
                mcKillSeq = GaudiSequencer("KillMCLinks") # The sequence killing the node of it exists
                tescheck = TESCheck("DaVinciEvtCheck")    # Check for presence of node ...
                tescheck.Inputs = ["Link/Rec/Track/Best"] # 
                tescheck.Stop = False                     # But don't stop
                tescheck.OutputLevel = 5                  # don't print warnings
                evtnodekiller = EventNodeKiller("DaVinciEvtNodeKiller") # kill nodes
                evtnodekiller.Nodes = ["Link/Rec"]        # Kill that
                mcKillSeq.Members = [ tescheck, evtnodekiller, TrackAssociator() ]  
                
                mcLinkSeq = GaudiSequencer("RedoMCLinks") # The sequence redoing the links
                mcLinkSeq.IgnoreFilterPassed = True       # Run it always
                mcLinkSeq.Members = [ mcKillSeq, TrackAssociator() ]  
                init.Members += [ mcLinkSeq ]
#
# Set MC
#
    def configureMC(self):
        """
        Define DaVinciAssociators. Do MC unpacking.
        """
        from CaloAssociators import MCTruthOnDemand
        importOptions ("$DAVINCIASSOCIATORSROOT/options/DaVinciAssociators.opts")
        self.unpackMC()
#
# Standard Particles
#
    def standardParticles(self):
        """
        define standard particles on DoD service
        """
        ApplicationMgr().ExtSvc +=  [ DataOnDemandSvc() ]
        import CommonParticles.StandardBasic
        import CommonParticles.StandardIntermediate
#
# Standard Particles
#
    def standardDC06Particles(self):
        """
        Temporary DC06 particles
        @todo Remove
        """
        importOptions("$COMMONPARTICLESROOT/options/StandardDC06Options.opts")

#
# Unpack MC
#
    def unpackMC(self):
        """
        Do MC unpacking
        """
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
        log.info("Applying Analysis configuration")
        log.info( self )
        GaudiKernel.ProcessJobOptions.PrintOff()
        if ( self.getProp("Simulation" )):
            self.configureMC()
        self.tagging()
        self.standardParticles()
        self.standardDC06Particles()
