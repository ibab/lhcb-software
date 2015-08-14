#!/usr/bin/env python 
# =============================================================================
"""
High level configuration tools for AnalysisConf
"""
# =============================================================================
__version__ = "$Id: Configuration.py,v 1.23 2010-01-22 13:11:30 pkoppenb Exp $"
__author__ = "Patrick Koppenburg <Patrick.Koppenburg@cern.ch>"
# =============================================================================

from LHCbKernel.Configuration import *
import GaudiKernel.ProcessJobOptions

class AnalysisConf(LHCbConfigurableUser) :
    
    __slots__ = {
          "DataType"        : ''       # Data type, can be ['2008','2009','MC09','2010','2011']
        , "Simulation"      : False    # set to True for MC
        , "RedoMCLinks"     : False    # On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association.
        , "OutputLevel"     : INFO     # The global output level
         }
    
    _propertyDocDct = { 
        "DataType"          : """ Data type, can be ['2008','2009','MC09','2010','2011'] """ 
        , "Simulation"      : """ set to True to use SimCond """ 
        , "RedoMCLinks"     : """ On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association """
        , "OutputLevel"     : """ The global output level """ 
        }
    
    __used_configurables__ = (
        'CaloAssociatorsConf' ,
        'FlavourTaggingConf'
        )
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
        from Configurables import FlavourTaggingConf
        # Just create an instance. Nothing to configure as of yet
        ftConf = FlavourTaggingConf()
        
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
        from Configurables import CaloAssociatorsConf
        
        CaloAssociatorsConf ( EnableMCOnDemand = True, OutputLevel = self.getProp("OutputLevel") )

        from CaloKernel.ConfUtils import getAlgo
        from Configurables        import NeutralPP2MC

        ## offline neutral protoparticles 
        alg1 = getAlgo (
            NeutralPP2MC                    , ## type 
            'NeutralPP2MC'                  , ## base-name 
            'Offline'                       , ## context 
            'Relations/Rec/ProtoP/Neutrals' , ## base-location
            True                            ) ## on-demand 

        ## hlt neutral protoparticles 
        alg2 = getAlgo (
            NeutralPP2MC                    , ## type 
            'NeutralPP2MC'                  , ## base-name 
            'Hlt'                           , ## context 
            'Relations/Rec/ProtoP/Neutrals' , ## base-location 
            True                            ) ## on-demand
        
        from Configurables import DataOnDemandSvc
        DataOnDemandSvc().AlgMap['/Event/Relations/Rec/ProtoP/Charged' ]  = 'ChargedPP2MC' 
        DataOnDemandSvc().AlgMap['/Event/Relations/Rec/ProtoP/Upstream' ] = 'ChargedPP2MC' 

        self.unpackMC()
#
# Standard Particles
#
    def standardParticles(self):
        """
        define standard particles on DoD service
        """
        import CommonParticles.StandardBasic
        import CommonParticles.StandardIntermediate

#
# Unpack MC
#
    def unpackMC(self):
        """
        Do MC unpacking
        """
        DataOnDemandSvc().NodeMap['/Event/MC']   = 'DataObject'
        DataOnDemandSvc().AlgMap["MC/Particles"] = "UnpackMCParticle"
        DataOnDemandSvc().AlgMap["MC/Vertices"]  = "UnpackMCVertex"
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
        
        if ( self.getProp("Simulation") ):
            self.configureMC()

        # Setup DataOnDemand, and make sure ToolSvc is done before hand
        ApplicationMgr().ExtSvc += [ ToolSvc(), DataOnDemandSvc() ]
                    
        self.tagging()
        self.standardParticles()


if '__main__' == __name__ :
    print __doc__
    print __author__
    print __version__
    
# =============================================================================
# The END 
# =============================================================================
