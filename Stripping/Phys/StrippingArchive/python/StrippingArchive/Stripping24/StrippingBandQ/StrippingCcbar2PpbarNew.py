'''
Module for Charmonium->p pbar, including three lines
1. Normal line
2. Exclusive
3. Detached

'''

__author__=['Jibo He']
__date__ = '03/10/2010'
__version__= '$Revision: 1.2 $'


__all__ = (
    'Ccbar2PpbarConf',
    'makePP'
    )

default_config = {
    'NAME'              : 'Ccbar2Ppbar',  
    'BUILDERTYPE'       : 'Ccbar2PpbarConf',
    'CONFIG'    : { 'LinePrescale'     :    1.   ,
                    'LinePostscale'    :    1.   ,
                    
                    'SpdMult'          :   300.  , # dimensionless, Spd Multiplicy cut 
                    'ProtonPT'         :  1950.  , # MeV
                    'ProtonP'          :    10.  , # GeV
                    'ProtonTRCHI2DOF'  :     4.  ,
                    'ProtonPIDppi'     :    20.  , # CombDLL(p-pi)
                    'ProtonPIDpK'      :    15.  , # CombDLL(p-K)
                    'ProtonIPCHI2Cut'  :    ""   ,
                    'CombMaxMass'      :  4100.  , # MeV, before Vtx fit
                    'CombMinMass'      :  2750.  , # MeV, before Vtx fit
                    'MaxMass'          :  4000.  , # MeV, after Vtx fit
                    'MinMass'          :  2800.  , # MeV, after Vtx fit
                    'VtxCHI2'          :     9.  , # dimensionless
                    'CCCut'            :  " & (PT>6*GeV)"    
                   },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'    : [ 'BandQ' ]
}

Ccbar2PpbarExclusive = {
    'BUILDERTYPE'       : 'Ccbar2PpbarConf',
    'CONFIG'    : {
        'LinePrescale'     :     1.  ,
        'LinePostscale'    :     1.  ,
        
        'SpdMult'          :    20.  , # dimensionless, Spd Multiplicy cut 
        'ProtonPT'         :   550.  , # MeV
        'ProtonP'          :    -2.  , # MeV
        'ProtonTRCHI2DOF'  :     5.  ,
        'ProtonPIDppi'     :    20.  , # CombDLL(p-pi)
        'ProtonPIDpK'      :    15.  , # CombDLL(p-K)
        'ProtonIPCHI2Cut'  :    ""   ,
        'CombMaxMass'      :  1.0e+6 , # MeV, before Vtx fit
        'CombMinMass'      :     0.  , # MeV, before Vtx fit
        'MaxMass'          :  1.0e+6 , # MeV, after Vtx fit
        'MinMass'          :     0.  , # MeV, after Vtx fit
        'VtxCHI2'          :     9.  , # dimensionless
        'CCCut'            :  ""     
        },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'    : [ 'BandQ' ]
    }

Ccbar2PpbarDetached = {
    'BUILDERTYPE'       : 'Ccbar2PpbarConf',
    'CONFIG'    : {
      'LinePrescale'     :     1.  ,
      'LinePostscale'    :     1.  ,
      
      'SpdMult'          :   600.  , # dimensionless, Spd Multiplicy cut 
      'ProtonPT'         :  1000.  , # MeV
      'ProtonP'          :    -2.  , # MeV
      'ProtonTRCHI2DOF'  :     5.  ,
      'ProtonPIDppi'     :    15.  , # CombDLL(p-pi)
      'ProtonPIDpK'      :    10.  , # CombDLL(p-K)
      'ProtonIPCHI2Cut'  : " & (BPVIPCHI2()>9)",
      'CombMaxMass'      :  1.0e+6 , # MeV, before Vtx fit
      'CombMinMass'      :  2650.  , # MeV, before Vtx fit
      'MaxMass'          :  1.0e+6 , # MeV, after Vtx fit
      'MinMass'          :  2700.  , # MeV, after Vtx fit
      'VtxCHI2'          :     9.  , # dimensionless
      'CCCut'            :  " & (BPVDLS>5)"     
      },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'    : [ 'BandQ' ]
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
        'ProtonIPCHI2Cut',
        
        'CombMaxMass',
        'CombMinMass',
        'MaxMass',
        'MinMass',
        'VtxCHI2',
        'CCCut'
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
                             ProtonIPCHI2Cut = config['ProtonIPCHI2Cut'],
                             CombMinMass = config['CombMinMass'],
                             CombMaxMass = config['CombMaxMass'],
                             VtxCHI2 = config['VtxCHI2'],
                             MinMass = config['MinMass'],
                             MaxMass = config['MaxMass'],
                             CCCut = config['CCCut']
                             )
        
        SpdMultForCcbarCut = config['SpdMult']
        
        self.line = StrippingLine( self.name+"Line",
                                   prescale = config['LinePrescale'],
                                   postscale = config['LinePostscale'],
                                   FILTER = {
                                   'Code' : " ( recSummary(LHCb.RecSummary.nSPDhits,'Raw/Spd/Digits') < %(SpdMultForCcbarCut)s )" % locals() ,
                                   'Preambulo' : [ "from LoKiNumbers.decorators import *" ]
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
            ProtonIPCHI2Cut,
            CombMinMass,
            CombMaxMass,
            VtxCHI2,
            MinMass,
            MaxMass,
            CCCut
            ):

    from StandardParticles import StdTightProtons as ProtonsForCcbar2Ppbar
    
    ProtonCut = "(PT > %(ProtonPT)s *MeV) & (P > %(ProtonP)s *GeV) & (TRCHI2DOF < %(ProtonTRCHI2DOF)s) & ((PIDp-PIDpi) > %(ProtonPIDppi)s) & ((PIDp-PIDK) > %(ProtonPIDpK)s)" % locals()
    CombCut = "(in_range( %(CombMinMass)s *MeV, AM, %(CombMaxMass)s *MeV))" % locals()
    MomCut = "(VFASPF(VCHI2)< %(VtxCHI2)s) & (in_range( %(MinMass)s *MeV, MM, %(MaxMass)s *MeV))" % locals() 
    
    _PP = CombineParticles( DecayDescriptor = "J/psi(1S) -> p+ p~-",
                            DaughtersCuts = {"p+" : ProtonCut + ProtonIPCHI2Cut },
                            CombinationCut = CombCut,
                            MotherCut = MomCut + CCCut
                            )
    
    return Selection( name,
                      Algorithm = _PP,
                      RequiredSelections = [ ProtonsForCcbar2Ppbar ]
                      )    
