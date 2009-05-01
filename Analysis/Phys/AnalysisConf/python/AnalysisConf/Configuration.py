"""
High level configuration tools for AnalysisConf
"""
__version__ = "$Id: Configuration.py,v 1.13 2009-05-01 10:38:08 pkoppenb Exp $"
__author__ = "Patrick Koppenburg <Patrick.Koppenburg@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
import GaudiKernel.ProcessJobOptions

class AnalysisConf(LHCbConfigurableUser) :
    
    __slots__ = {
          "DataType"        : 'DC06'                             # Data type, can be ['DC06','2008']
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
            if (( self.getProp("DataType")=="DC06" ) and ( not redo )):
                log.warning("Redoing MC links enforced with DC06")
                redo = True
                self.setProp("RedoMCLinks",True) 
                
            if ( redo ):
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
# Set MC
#
    def configureMC(self):
        """
        Define DaVinciAssociators. Do MC unpacking.
        """
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
        GaudiKernel.ProcessJobOptions.PrintOff()
        if ( self.getProp("Simulation" )):
            self.configureMC()
        self.tagging()
        self.standardParticles()
        self.standardDC06Particles()
