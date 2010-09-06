__author__ = 'Serezha.Barsuk@cern.ch, Jibo.He@cern.ch'
__date__ = '12/04/2010'
__version__ = '$Revision: 1.4 $'

"""
 Charmonium -> p pbar, including eta(1S) -> psi(2S)
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingCcbar2PpbarConf(LHCbConfigurableUser):
    __slots__ = {
                    'SpdMult'          :   600.   # dimensionless, Spd Multiplicy cut 
                  , 'ProtonPT'         :  1950.   # MeV
                  , 'ProtonTrkChi2'    :     4. 
                  , 'ProtonPIDppi'     :    10.   # CombDLL(p-pi)
                  , 'ProtonPIDpK'      :     5.   # CombDLL(p-K)
                  , 'CombUpperMass'    :  3950.   # MeV, before Vtx fit
                  , 'CombLowerMass'    :  2850.   # MeV, before Vtx fit
                  , 'UpperMass'        :  3900.   # MeV, after Vtx fit
                  , 'LowerMass'        :  2900.   # MeV, after Vtx fit
                  , 'VtxCHI2'          :     9.   # dimensionless
                  # Loose selection
                  , 'ProtonPTLoose'         :  1600.   # MeV
                  , 'ProtonPIDppiLoose'     :    -5.   # CombDLL(p-pi)
                  , 'ProtonPIDpKLoose'      :    -5.   # CombDLL(p-K)
                  , 'CombUpperMassLoose'    :  4050.   # MeV, before Vtx fit
                  , 'CombLowerMassLoose'    :  2750.   # MeV, before Vtx fit
                  , 'UpperMassLoose'        :  3950.   # MeV, after Vtx fit
                  , 'LowerMassLoose'        :  2850.   # MeV, after Vtx fit
                  , 'VtxCHI2Loose'          :     9.   # dimensionless 
                  }

    def Nominal_Line(self):
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        StripCcbar2Ppbar = self.combineCcbar()
        return StrippingLine('Ccbar2Ppbar', prescale = 1,  algos = [ StripCcbar2Ppbar ] )

    
    def Loose_Line(self):
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        StripCcbar2PpbarLoose = self.combineCcbarLoose()
        return StrippingLine('Ccbar2PpbarLoose', prescale = 0.01,  algos = [ StripCcbar2PpbarLoose ] )
   
        
    def combineCcbar(self):
        from Configurables import CombineParticles, LoKi__VoidFilter
        from PhysSelPython.Wrappers import DataOnDemand, EventSelection, Selection, SelectionSequence
        
        #---------------------------
        # Selections 
        #---------------------------

        # Requre SpdMult < 600
        SpdMultFilter = LoKi__VoidFilter("_SpdMultFilter",
                                         Code = "( CONTAINS('Raw/Spd/Digits')<%(SpdMult)s )" % self.getProps()
                                         ) 
        
        _TightProtons = DataOnDemand(Location = "Phys/StdTightProtons")
        
        ProtonCut = "(PT > %(ProtonPT)s *MeV) & (TRCHI2DOF < %(ProtonTrkChi2)s) & ((PIDp-PIDpi) > %(ProtonPIDppi)s) & ((PIDp-PIDK) > %(ProtonPIDpK)s)" % self.getProps()
        CombCut = "(%(CombLowerMass)s *MeV < AM) & (AM < %(CombUpperMass)s *MeV)" % self.getProps()
        MomCut = "(VFASPF(VCHI2/VDOF)< %(VtxCHI2)s) & (%(LowerMass)s *MeV < MM) & (MM < %(UpperMass)s *MeV)" % self.getProps()
        
        _Ccbar2Ppbar = CombineParticles("_Ccbar2Ppbar")
        _Ccbar2Ppbar.DecayDescriptor = "J/psi(1S) -> p+ p~-" 
        _Ccbar2Ppbar.DaughtersCuts = { "p+": ProtonCut }
        _Ccbar2Ppbar.CombinationCut = CombCut
        _Ccbar2Ppbar.MotherCut = MomCut
        
        SelCcbar2Ppbar = Selection( "SelCcbar2Ppbar",
                                    Algorithm = _Ccbar2Ppbar,
                                    RequiredSelections = [ _TightProtons ]
                                    )

        return SelectionSequence( "SeqCcbar2Ppbar",
                                  TopSelection = SelCcbar2Ppbar,
                                  EventPreSelector = [ SpdMultFilter ]
                                  )

    def combineCcbarLoose(self):
        from Configurables import CombineParticles
        
        #---------------------------
        # Loose Selections 
        #---------------------------
        ProtonLooseCut = "(PT> %(ProtonPTLoose)s *MeV) & ((PIDp-PIDpi) > %(ProtonPIDppiLoose)s) & ((PIDp-PIDK) > %(ProtonPIDpKLoose)s)" % self.getProps()
        CombLooseCut = "(%(CombLowerMassLoose)s *MeV < AM) & (AM < %(CombUpperMassLoose)s *MeV)" % self.getProps()
        MomLooseCut = "(VFASPF(VCHI2/VDOF)< %(VtxCHI2Loose)s) & (%(LowerMassLoose)s *MeV < MM) & (MM < %(UpperMassLoose)s *MeV)" % self.getProps()
        
        StripCcbar2PpbarLoose = CombineParticles("StripCcbar2PpbarLoose")
        StripCcbar2PpbarLoose.InputLocations = [ "StdLooseProtons" ]
        StripCcbar2PpbarLoose.DecayDescriptor = "J/psi(1S) -> p+ p~-" 
        StripCcbar2PpbarLoose.DaughtersCuts = { "p+": ProtonLooseCut }
        StripCcbar2PpbarLoose.CombinationCut = CombLooseCut
        StripCcbar2PpbarLoose.MotherCut = MomLooseCut
        return StripCcbar2PpbarLoose  
    
    def getProps(self):
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        Copied from StrippingBu2LLK
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
