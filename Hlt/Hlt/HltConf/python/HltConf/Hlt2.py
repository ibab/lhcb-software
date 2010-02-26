"""
 script to configure HLT2 trigger lines

 @author P. Koppenburg
 @date 2009-07-08
"""
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.50 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Hlt2Lines.Hlt2B2DXLines            import Hlt2B2DXLinesConf
from Hlt2Lines.Hlt2B2JpsiXLines         import Hlt2B2JpsiXLinesConf
from Hlt2Lines.Hlt2B2PhiXLines          import Hlt2B2PhiXLinesConf
from Hlt2Lines.Hlt2InclusiveDiMuonLines import Hlt2InclusiveDiMuonLinesConf
from Hlt2Lines.Hlt2InclusiveMuonLines   import Hlt2InclusiveMuonLinesConf
from Hlt2Lines.Hlt2InclusivePhiLines    import Hlt2InclusivePhiLinesConf
from Hlt2Lines.Hlt2TopologicalLines     import Hlt2TopologicalLinesConf
from Hlt2Lines.Hlt2B2XGammaLines        import Hlt2B2XGammaLinesConf
from Hlt2Lines.Hlt2B2HHLines            import Hlt2B2HHLinesConf
from Hlt2Lines.Hlt2B2LLXLines           import Hlt2B2LLXLinesConf
from Hlt2Lines.Hlt2CharmLines           import Hlt2CharmLinesConf
from Hlt2Lines.Hlt2DisplVerticesLines   import Hlt2DisplVerticesLinesConf
from Hlt2Lines.Hlt2CommissioningLines   import Hlt2CommissioningLinesConf
from Hlt2Lines.Hlt2ExpressLines         import Hlt2ExpressLinesConf
from Hlt2Lines.Hlt2diphotonDiMuonLines  import Hlt2diphotonDiMuonLinesConf
from HltLine.Hlt2Tracking import Hlt2Tracking
# Define what categories stand for
# There are the strings used in HltThresholdSettings


class Hlt2Conf(LHCbConfigurableUser):
    __used_configurables__ = [ Hlt2Tracking
                             , Hlt2TopologicalLinesConf
                             , Hlt2B2DXLinesConf 
                             , Hlt2CharmLinesConf
                             , Hlt2InclusiveDiMuonLinesConf
                             , Hlt2InclusiveMuonLinesConf 
                             , Hlt2InclusivePhiLinesConf 
                             , Hlt2B2JpsiXLinesConf
                             , Hlt2B2PhiXLinesConf
                             , Hlt2B2XGammaLinesConf
                             , Hlt2B2HHLinesConf
                             # , Hlt2B2LLXLinesConf                         # DO NOT RUN UNTIL CALO IS FIXED
                             , Hlt2DisplVerticesLinesConf
                             , Hlt2CommissioningLinesConf
                             , Hlt2ExpressLinesConf
                             , Hlt2diphotonDiMuonLinesConf  
                             ]
    __slots__ = {
           "DataType"                   : '2009'    # datatype is one of 2009, MC09, DC06...
         , "ThresholdSettings"          : {} # ThresholdSettings predefined by Configuration
         , "WithMC"                     : False 
         }

###################################################################################
#
# Threshold settings
#
# This is copied and edited from Hlt1.confType().
# One could outsource that to some function, but who cares?
#
    def hlt2Lines(self,Hlt2):
        """
        The actual lines
        """
        from HltLine.HltLine     import Hlt2Line
        Hlt2Line( "Global", HLT= "HLT_PASS_SUBSTR('Hlt2') ", priority = 255 )
        ThresholdSettings = self.getProp("ThresholdSettings")
        #
        # Loop over thresholds
        #
        from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
        for i in self.__used_configurables__ :
            if not issubclass(i,HltLinesConfigurableUser) : continue
            from ThresholdUtils import setThresholds
            setThresholds(ThresholdSettings,i)
       
###################################################################################
#
# PID
#
    def configurePID(self):
        from HltLine.HltTrackNames import Hlt2TracksPrefix, HltUnfittedTracksSuffix, Hlt2ForwardTracksName 
        Hlt2Tracking().DataType = self.getProp("DataType")
        Hlt2Tracking().Prefix = Hlt2TracksPrefix
        Hlt2Tracking().FastFitType = HltUnfittedTracksSuffix #track fit is off by default
        Hlt2Tracking().Hlt2Tracks = Hlt2ForwardTracksName
        Hlt2Tracking().DoFastFit = False
        Hlt2Tracking().UseRICH = False
        Hlt2Tracking().UseCALO = False
        Hlt2Tracking().DoSeeding = False
        Hlt2Tracking().DoCloneKilling = False
      
###################################################################################
#
# MC
#
    def withMC(self):
        """
        MC options for DaVinci 
        """
        from Configurables import DataOnDemandSvc, TrackAssociator

        DataOnDemandSvc().AlgMap.update(  { 'Link/Hlt/Track/Long': 'TrackAssociator/HltTrackAssociator'
                                          , 'Link/Hlt/Track/Muons':   'TrackAssociator/HltMuonAssociator'
                                          , 'Link/Hlt/Track/SeedTT':  'TrackAssociator/HltSeedAssociator'
                                          , 'Link/Hlt/Track/TFForwardForTopo':  'TrackAssociator/HltTFAssociator'
                                          } )

        TrackAssociator("HltTrackAssociator").TracksInContainer = "Hlt/Track/Long" 
        TrackAssociator("HltMuonAssociator").TracksInContainer = "Hlt/Track/Muons" 
        TrackAssociator("HltSeedAssociator").TracksInContainer = "Hlt/Track/SeedTT"
        TrackAssociator("HltTFAssociator").TracksInContainer = "Hlt/Track/TFForwardForTopo"
        from Configurables import CaloClusterMCTruth, ChargedPP2MC
        DataOnDemandSvc().AlgMap['/Event/Relations/Hlt/ProtoP/Charged' ] = ChargedPP2MC()
        DataOnDemandSvc().AlgMap['/Event/Relations/Hlt/Calo/Clusters' ] = CaloClusterMCTruth("CaloClusterMCTruthForHlt", Context = 'Hlt')
        
###################################################################################
#
# Main configuration
#
    def __apply_configuration__(self):
        """
        Hlt2 configuration
        """
        print 'CONFIGURING HLT2!!!!'
        Hlt2 = Sequence("Hlt2", Context = 'HLT',ModeOR=True,ShortCircuit=False) 
        if Hlt2 not in Sequence("Hlt").Members : Sequence("Hlt").Members += [ Hlt2 ]
        # reco
        if self.getProp('WithMC'): self.withMC()
        # set Hlt2 PID
        self.configurePID()
        # lines
        self.hlt2Lines(Hlt2)
        
