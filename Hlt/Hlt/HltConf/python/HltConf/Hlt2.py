"""
 script to configure HLT2 trigger lines

 @author P. Koppenburg
 @date 2009-07-08
"""
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.72 $"
# =============================================================================
import types
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Hlt2Lines.Hlt2B2DXLines              import Hlt2B2DXLinesConf
from Hlt2Lines.Hlt2B2JpsiXLines           import Hlt2B2JpsiXLinesConf
from Hlt2Lines.Hlt2B2PhiXLines            import Hlt2B2PhiXLinesConf
from Hlt2Lines.Hlt2InclusiveDiMuonLines   import Hlt2InclusiveDiMuonLinesConf
from Hlt2Lines.Hlt2InclusiveMuonLines     import Hlt2InclusiveMuonLinesConf
from Hlt2Lines.Hlt2InclusivePhiLines      import Hlt2InclusivePhiLinesConf
from Hlt2Lines.Hlt2TopologicalLines       import Hlt2TopologicalLinesConf
from Hlt2Lines.Hlt2B2XGammaLines          import Hlt2B2XGammaLinesConf
from Hlt2Lines.Hlt2B2HHLines              import Hlt2B2HHLinesConf
from Hlt2Lines.Hlt2B2HHLTUnbiasedLines    import Hlt2B2HHLTUnbiasedLinesConf
from Hlt2Lines.Hlt2B2LLXLines             import Hlt2B2LLXLinesConf
from Hlt2Lines.Hlt2CharmHadD02HHLines     import Hlt2CharmHadD02HHLinesConf
from Hlt2Lines.Hlt2DisplVerticesLines     import Hlt2DisplVerticesLinesConf
from Hlt2Lines.Hlt2CommissioningLines     import Hlt2CommissioningLinesConf
from Hlt2Lines.Hlt2ExpressLines           import Hlt2ExpressLinesConf
from Hlt2Lines.Hlt2diphotonDiMuonLines    import Hlt2diphotonDiMuonLinesConf
from Hlt2Lines.Hlt2InclusiveDiProtonLines import Hlt2InclusiveDiProtonLinesConf
from Hlt2Lines.Hlt2Dst2D2XXLines          import Hlt2Dst2D2XXLinesConf
from Hlt2Lines.Hlt2InclusiveDiElectronLines import Hlt2InclusiveDiElectronLinesConf
from Hlt2Lines.Hlt2InclusiveElectronLines   import Hlt2InclusiveElectronLinesConf
from Hlt2Lines.Hlt2B2KstareeLines           import Hlt2B2KstareeLinesConf
from Hlt2Lines.Hlt2SecondLoopExampleLine   import Hlt2SecondLoopExampleLinesConf
from Hlt2Lines.Hlt2B2HHLTUnbiasedLines import Hlt2B2HHLTUnbiasedLinesConf
from Hlt2Lines.Hlt2B2HHPi0Lines import Hlt2B2HHPi0LinesConf
from Hlt2Lines.Hlt2B2KsHHLines import Hlt2B2KsHHLinesConf
from Hlt2Lines.Hlt2BeamGasLines import Hlt2BeamGasLinesConf
from Hlt2Lines.Hlt2MuNTrackLines import Hlt2MuNTrackLinesConf
#
# The tracking configurations
#
from HltTracking.Hlt2Tracking import Hlt2Tracking
#
from HltTracking.Hlt2TrackingConfigurations import Hlt2UnfittedForwardTracking
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedLongTracking 
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedRichForProtonsForwardTracking
from HltTracking.Hlt2TrackingConfigurations import setDataTypeForTracking
# Define what categories stand for
# There are the strings used in HltThresholdSettings


class Hlt2Conf(LHCbConfigurableUser):
    __used_configurables__ = [ (Hlt2Tracking, "Hlt2BiKalmanFittedRichForProtonsForwardTracking")
                             , (Hlt2Tracking, "Hlt2BiKalmanFittedDownstreamTracking") 
                             , (Hlt2Tracking, "Hlt2BiKalmanFittedForwardTracking")
                             , (Hlt2Tracking, "Hlt2BiKalmanFittedLongTracking")
                             , (Hlt2Tracking, "Hlt2UnfittedForwardTracking")
                             , Hlt2TopologicalLinesConf
                             , Hlt2B2DXLinesConf 
                             , Hlt2CharmHadD02HHLinesConf
                             , Hlt2InclusiveDiMuonLinesConf
                             , Hlt2InclusiveMuonLinesConf 
                             , Hlt2InclusivePhiLinesConf 
                             , Hlt2B2JpsiXLinesConf
                             , Hlt2B2PhiXLinesConf
                             , Hlt2B2XGammaLinesConf
                             , Hlt2B2HHLinesConf
                             , Hlt2B2HHLTUnbiasedLinesConf
                             , Hlt2B2HHPi0LinesConf
                             , Hlt2B2KsHHLinesConf
                             , Hlt2B2LLXLinesConf                         
                             , Hlt2DisplVerticesLinesConf
                             , Hlt2CommissioningLinesConf
                             , Hlt2ExpressLinesConf
                             , Hlt2diphotonDiMuonLinesConf
                             , Hlt2InclusiveDiProtonLinesConf
                             , Hlt2Dst2D2XXLinesConf
                             , Hlt2InclusiveDiElectronLinesConf
                             , Hlt2InclusiveElectronLinesConf
                             , Hlt2B2KstareeLinesConf  
                             , Hlt2BeamGasLinesConf
                             , Hlt2SecondLoopExampleLinesConf
                             , Hlt2MuNTrackLinesConf
                             ]
    __slots__ = { "DataType"                   : '2010'    # datatype is one of 2009, MC09, DC06...
                , "ThresholdSettings"          : {} # ThresholdSettings predefined by Configuration
                , "WithMC"                     : False 
                , "DefaultVoidFilter"          : ''
                , "Hlt2ForwardMaxVelo"         : 350
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
        Hlt2Line( "Global", HLT= "HLT_PASS_SUBSTR('Hlt2') ", priority = 255, VoidFilter = '' )
        ThresholdSettings = self.getProp("ThresholdSettings")

        #
        # check if this threshold setting has some global event cut...
        #  if so, set it, and add a line which does (prescaled) the opposite
        #
        if self.getProp('DefaultVoidFilter') :
            from HltLine.HltLine import Hlt2Line
            Hlt2Line.setDefaultVoidFilter( self.getProp('DefaultVoidFilter') )
            Hlt2Line( 'PileUp', VoidFilter = '  ~ ( %s ) ' % self.getProp('DefaultVoidFilter') , postscale = 0.01 )
        

        #
        # Loop over thresholds
        #
        from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
        for i in self.__used_configurables__ :
            # Time for some python magic
            if type(i) is tuple : # if we are dealing with a named instance in the used configurables
                i, i_name = i # copy what is done in GaudiKernel/Configurable.py 
            if not issubclass(i,HltLinesConfigurableUser) : continue
            from ThresholdUtils import setThresholds
            setThresholds(ThresholdSettings,i)
       
###################################################################################
#
# Reconstruction
#
    def configureReconstruction(self):

        definedTrackings = [ Hlt2BiKalmanFittedDownstreamTracking()
                           , Hlt2BiKalmanFittedForwardTracking()
                           , Hlt2BiKalmanFittedLongTracking() 
                           , Hlt2BiKalmanFittedRichForProtonsForwardTracking()
                           , Hlt2UnfittedForwardTracking() ]


        # And now we have to, for each of the configurables we just created, 
        # tell it the data type and tell it to use all the Hlt2 lines...
        from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
        from Gaudi.Configuration import ConfigurableUser
        for thistracking in definedTrackings :
            setDataTypeForTracking(thistracking,self.getProp("DataType"))
            if self.getProp('Hlt2ForwardMaxVelo') : thistracking.Hlt2ForwardMaxVelo = self.getProp("Hlt2ForwardMaxVelo")
###################################################################################
#
# MC
#
    def withMC(self):
        """
        MC options for DaVinci 
        """
        from Configurables import DataOnDemandSvc, TrackAssociator

        # TODO: fix hardcoding!
        m  = { "HltTrackAssociator": "Hlt2/Track/Unfitted/Forward"
             , "HltMuonAssociator": "Hlt2/Track/Unfitted/Forward/PID/MuonSegments"
             , "HltSeedAssociator": "Hlt2/Track/Unfitted/SeedTT"  
             , "HltTFAssociator": "Hlt2/Track/BiKalmanFitted/Forward" }

        for (i,j) in m.iteritems() :
              DataOnDemandSvc().AlgMap.update( { 'Link/'+j : TrackAssociator(i, TracksInContainer = j ).getFullName() } )


        from Configurables import CaloClusterMCTruth, ChargedPP2MC
        DataOnDemandSvc().AlgMap['/Event/Relations/Hlt2/ProtoP/Unfitted/Forward/Charged' ] = ChargedPP2MC()
        
        # TODO: fix MC truth for neutrals, broken right now
        #DataOnDemandSvc().AlgMap['/Event/Relations/Hlt/Calo/Clusters' ] = CaloClusterMCTruth("CaloClusterMCTruthForHlt", Context = 'Hlt')
        
###################################################################################
#
# Main configuration
#
    def __apply_configuration__(self):
        """
        Hlt2 configuration
        """
        Hlt2 = Sequence("Hlt2", Context = 'HLT',ModeOR=True,ShortCircuit=False) 
        # set Hlt2 PID
        self.configureReconstruction()
        # lines
        if self.getProp("DefaultVoidFilter") :
            from HltLine.HltLine import Hlt2Line  
            Hlt2Line.setDefaultVoidFilter( self.getProp("DefaultVoidFilter") )
        self.hlt2Lines(Hlt2)
        # reco
        if self.getProp('WithMC'): self.withMC()
