'''
Module for electron ID studies, including one
Tag-And-Probe line for lifetime biased Jpsi->ee

To include it:

from StrippingSelections.StrippingCcbar2Ppbar import Ccbar2PpbarConf
StrippingCcbar2PpbarConf =  Ccbar2PpbarConf( name = 'Jpsi2eeForCcbar2Ppbar', config = Ccbar2PpbarConf.config_default )
stream.appendLines( [
    StrippingCcbar2PpbarConf.line
    ])
'''

__author__=['Jibo He']
__date__ = '03/10/2010'
__version__= '$Revision: 1.2 $'


__all__ = (
    'Ccbar2PpbarConf',
    'makePP'
    )

config_default =  {
        'LinePrescale'     :    1.   ,
        'LinePostscale'    :    1.   ,
        
        'SpdMult'          :   600.  , # dimensionless, Spd Multiplicy cut 
        'ProtonPT'         :  1900.  , # MeV
        'ProtonP'          :    -2.  , # MeV
        'ProtonTRCHI2DOF' :      5.  ,
        'ProtonPIDppi'     :    10.  , # CombDLL(p-pi)
        'ProtonPIDpK'      :     5.  , # CombDLL(p-K)
        'CombMaxMass'      :  4100.  , # MeV, before Vtx fit
        'CombMinMass'      :  2750.  , # MeV, before Vtx fit
        'MaxMass'          :  4000.  , # MeV, after Vtx fit
        'MinMass'          :  2800.  , # MeV, after Vtx fit
        'VtxCHI2'          :     9.  , # dimensionless
        'CCPT'             :  5000.    
        }

config_exclusive =  {
        'LinePrescale'     :     1.  ,
        'LinePostscale'    :     1.  ,
        
        'SpdMult'          :    20.  , # dimensionless, Spd Multiplicy cut 
        'ProtonPT'         :   500.  , # MeV
        'ProtonP'          :    -2.  , # MeV
        'ProtonTRCHI2DOF' :      5.  ,
        'ProtonPIDppi'     :    10.  , # CombDLL(p-pi)
        'ProtonPIDpK'      :     5.  , # CombDLL(p-K)
        'CombMaxMass'      :  1.0e+6 , # MeV, before Vtx fit
        'CombMinMass'      :     0.  , # MeV, before Vtx fit
        'MaxMass'          :  1.0e+6 , # MeV, after Vtx fit
        'MinMass'          :     0.  , # MeV, after Vtx fit
        'VtxCHI2'          :     9.  , # dimensionless
        'CCPT'             :    -2.    
        }


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class Ccbar2PpbarConf(LineBuilder):
    
    __configuration_keys__ = (
        'LinePrescale',
        'LinePostscale',
        
        'SpdMult',
        'ProtonPT',
        'ProtonP',
        'ProtonTRCHI2DOF',
        'ProtonPIDppi',
        'ProtonPIDpK',
        'CombMaxMass',
        'CombMinMass',
        'MaxMass',
        'MinMass',
        'VtxCHI2',
        'CCPT'
        )

    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        
        self.name = name 
        self.SelPP = makePP( 'eeFor'+self.name,                             
                             #
                             ProtonPT = config['ProtonPT'],
                             ProtonP = config['ProtonP'],
                             ProtonTRCHI2DOF = config['ProtonTRCHI2DOF'],
                             ProtonPIDppi = config['ProtonPIDppi'],
                             ProtonPIDpK = config['ProtonPIDpK'],
                             CombMinMass = config['CombMinMass'],
                             CombMaxMass = config['CombMaxMass'],
                             VtxCHI2 = config['VtxCHI2'],
                             MinMass = config['MinMass'],
                             MaxMass = config['MaxMass'],
                             CCPT = config['CCPT']
                             )

        SpdMultForCcbarCut = config['SpdMult']
        
        self.line = StrippingLine( self.name+"Line",
                                   prescale = config['LinePrescale'],
                                   postscale = config['LinePostscale'],
                                   FILTER = { 'Code'       : "( CONTAINS('Raw/Spd/Digits')<%(SpdMultForCcbarCut)s )" % locals(), 
                                              'Preambulo'  : [ 'from LoKiTracks.decorators import *' ,
                                                               'from LoKiCore.functions    import *' ]
                                              },
                                   checkPV  = False,
                                   selection =  self.SelPP                                   
                                   )

        self.registerLine( self.line )
        
       
def makePP( name,
            #
            ProtonPT,
            ProtonP,
            ProtonTRCHI2DOF,
            ProtonPIDppi,
            ProtonPIDpK,
            CombMinMass,
            CombMaxMass,
            VtxCHI2,
            MinMass,
            MaxMass,
            CCPT
            ):

    from StandardParticles import StdTightProtons as ProtonsForCcbar2Ppbar
    
    ProtonCut = "(PT > %(ProtonPT)s *MeV) & (TRCHI2DOF < %(ProtonTRCHI2DOF)s) & ((PIDp-PIDpi) > %(ProtonPIDppi)s) & ((PIDp-PIDK) > %(ProtonPIDpK)s)" % locals()
    CombCut = "(in_range( %(CombMinMass)s *MeV, AM, %(CombMaxMass)s *MeV))" % locals()
    MomCut = "(VFASPF(VCHI2)< %(VtxCHI2)s) & (in_range( %(MinMass)s *MeV, MM, %(MaxMass)s *MeV))" % locals() 
    
    _PP = CombineParticles( DecayDescriptor = "J/psi(1S) -> p+ p~-",
                            DaughtersCuts = {"p+" : ProtonCut },
                            CombinationCut = CombCut,
                            MotherCut = MomCut
                            )
    
    return Selection( name,
                      Algorithm = _PP,
                      RequiredSelections = [ ProtonsForCcbar2Ppbar ]
                      )    
