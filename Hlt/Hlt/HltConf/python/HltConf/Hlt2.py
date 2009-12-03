"""
 script to configure HLT2 trigger lines

 @author P. Koppenburg
 @date 2009-07-08
"""
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.42 $"
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
from Hlt2Lines.Hlt2DisplVerticesLines   import Hlt2DisplVerticesLinesConf
from Hlt2Lines.Hlt2CommissioningLines   import Hlt2CommissioningLinesConf
from Hlt2Lines.Hlt2ExpressLines         import Hlt2ExpressLinesConf

# Define what categories stand for
# There are the strings used in HltThresholdSettings


class Hlt2Conf(LHCbConfigurableUser):
    from Configurables import Hlt2PID
    __used_configurables__ = [ Hlt2PID
                             , Hlt2TopologicalLinesConf
                             , Hlt2B2DXLinesConf 
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
                             ]
    __slots__ = {
           "DataType"                   : '2009'    # datatype is one of 2009, MC09, DC06...
         , "ThresholdSettings"          : {} # ThresholdSettings predefined by Configuration
         , "Hlt2Tracks"                 : "Hlt/Track/Long"
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
        Hlt2Line( "Global", HLT= "HLT_PASS_SUBSTR('Hlt2') ", priority = 255, PV = False )
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
        from Configurables import Hlt2PID
        Hlt2PID().DataType = self.getProp("DataType")
        Hlt2PID().Hlt2Tracks = self.getProp("Hlt2Tracks")
      
###################################################################################
#
# Main configuration
#
    def __apply_configuration__(self):
        """
        Hlt2 configuration
        """
        Hlt2 = Sequence("Hlt2", Context = 'HLT',ModeOR=True,ShortCircuit=False) 
        if Hlt2 not in Sequence("Hlt").Members : Sequence("Hlt").Members += [ Hlt2 ]
        # reco
        if self.getProp('WithMC'):
            # @todo : make that configurable
            importOptions( "$HLTCONFROOT/options/HltTrackAssociator.py" )
        # set Hlt2 PID
        self.configurePID()
        # lines
        self.hlt2Lines(Hlt2)
        
