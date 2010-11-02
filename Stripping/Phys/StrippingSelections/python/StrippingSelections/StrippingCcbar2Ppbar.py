__author__ = 'Serezha.Barsuk@cern.ch, Jibo.He@cern.ch'
__date__ = '12/04/2010'
__version__ = '$Revision: 1.4 $'

"""
 Charmonium -> p pbar, including eta(1S) -> psi(2S)
 Including
 1. Nominal line
 2. Exclusive line, for exclusive production study
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingCcbar2PpbarConf(LHCbConfigurableUser):
    __slots__ = {
                    'SpdMult'          :   600.   # dimensionless, Spd Multiplicy cut 
                  , 'ProtonPT'         :  1950.   # MeV
                  , 'ProtonTrkChi2'    :     5. 
                  , 'ProtonPIDppi'     :    10.   # CombDLL(p-pi)
                  , 'ProtonPIDpK'      :     5.   # CombDLL(p-K)
                  , 'CombUpperMass'    :  4050.   # MeV, before Vtx fit
                  , 'CombLowerMass'    :  2850.   # MeV, before Vtx fit
                  , 'UpperMass'        :  4000.   # MeV, after Vtx fit
                  , 'LowerMass'        :  2900.   # MeV, after Vtx fit
                  , 'VtxCHI2'          :     9.   # dimensionless
                  # Exclusive production
                  , 'SpdMultExclusive'          :   100.   # dimensionless, Spd Multiplicy cut   
                  , 'ProtonPTExclusive'         :   500.   # MeV
                  , 'ProtonPIDppiExclusive'     :    10.   # CombDLL(p-pi)
                  , 'ProtonPIDpKExclusive'      :     5.   # CombDLL(p-K)
                  , 'VtxCHI2Exclusive'          :     9.   # dimensionless                    
                  }

    def Nominal_Line(self):
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        StripCcbar2Ppbar = self.combineCcbar()
        return StrippingLine('Ccbar2Ppbar',
                             prescale = 1,
                             checkPV  = False,
                             FILTER = { 'Code'       : "( CONTAINS('Raw/Spd/Digits')<%(SpdMult)s )" % self.getProps(), 
                                        'Preambulo'  : [ 'from LoKiTracks.decorators import *' ,
                                                         'from LoKiCore.functions    import *' ]
                                        },
                             algos = [ StripCcbar2Ppbar ] )
    
    def Exclusive_Line(self):
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        StripCcbar2PpbarExclusive = self.combineCcbarExclusive()
        return StrippingLine('Ccbar2PpbarExclusive',
                             prescale = 1,
                             checkPV  = False,
                             FILTER = { 'Code'       : "( CONTAINS('Raw/Spd/Digits')<%(SpdMultExclusive)s )" % self.getProps(), 
                                        'Preambulo'  : [ 'from LoKiTracks.decorators import *' ,
                                                         'from LoKiCore.functions    import *' ]
                                        },
                             algos = [ StripCcbar2PpbarExclusive ] )
    
        
    def combineCcbar(self):
        from Configurables import CombineParticles, LoKi__VoidFilter
        from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence
        
        #---------------------------
        # Selections 
        #---------------------------
        _TightProtons = DataOnDemand(Location = "Phys/StdTightProtons")
        
        ProtonCut = "(PT > %(ProtonPT)s *MeV) & (TRCHI2DOF < %(ProtonTrkChi2)s) & ((PIDp-PIDpi) > %(ProtonPIDppi)s) & ((PIDp-PIDK) > %(ProtonPIDpK)s)" % self.getProps()
        CombCut = "(%(CombLowerMass)s *MeV < AM) & (AM < %(CombUpperMass)s *MeV)" % self.getProps()
        MomCut = "(VFASPF(VCHI2/VDOF)< %(VtxCHI2)s) & (%(LowerMass)s *MeV < MM) & (MM < %(UpperMass)s *MeV)" % self.getProps()
        
        _Ccbar2Ppbar = CombineParticles("_Ccbar2Ppbar")
        _Ccbar2Ppbar.DecayDescriptor = "J/psi(1S) -> p+ p~-" 
        _Ccbar2Ppbar.DaughtersCuts = { "p+": ProtonCut }
        _Ccbar2Ppbar.CombinationCut = CombCut
        _Ccbar2Ppbar.MotherCut = MomCut
        
        return Selection( "SelCcbar2Ppbar",
                          Algorithm = _Ccbar2Ppbar,
                          RequiredSelections = [ _TightProtons ]
                          )
        

    def combineCcbarExclusive(self):
        from Configurables import CombineParticles, LoKi__VoidFilter
        from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence
        
        #---------------------------
        # Selections 
        #---------------------------
        _TightProtons = DataOnDemand(Location = "Phys/StdTightProtons")
        
        ProtonCut = "(PT > %(ProtonPTExclusive)s *MeV) & ((PIDp-PIDpi) > %(ProtonPIDppiExclusive)s) & ((PIDp-PIDK) > %(ProtonPIDpKExclusive)s)" % self.getProps()
        #CombCut = "(%(CombLowerMassExclusive)s *MeV < AM) & (AM < %(CombUpperMassExclusive)s *MeV)" % self.getProps()
        #MomCut = "(VFASPF(VCHI2/VDOF)< %(VtxCHI2Exclusive)s) & (%(LowerMassExclusive)s *MeV < MM) & (MM < %(UpperMassExclusive)s *MeV)" % self.getProps()
        MomCut = "(VFASPF(VCHI2/VDOF)< %(VtxCHI2Exclusive)s)" % self.getProps()        

        _Ccbar2PpbarExclusive = CombineParticles("_Ccbar2PpbarExclusive")
        _Ccbar2PpbarExclusive.DecayDescriptor = "J/psi(1S) -> p+ p~-" 
        _Ccbar2PpbarExclusive.DaughtersCuts = { "p+": ProtonCut }
        _Ccbar2PpbarExclusive.MotherCut = MomCut
        
        return Selection( "SelCcbar2PpbarExclusive",
                          Algorithm = _Ccbar2PpbarExclusive,
                          RequiredSelections = [ _TightProtons ]
                          )
    
    
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
