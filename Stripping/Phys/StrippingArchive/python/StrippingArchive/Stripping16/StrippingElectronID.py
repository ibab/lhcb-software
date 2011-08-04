'''
Module for electron ID studies, including one
Tag-And-Probe line for lifetime biased Jpsi->ee

To include it:

from StrippingSelections.StrippingElectronID import ElectronIDConf
StrippingElectronIDConf =  ElectronIDConf( name = 'Jpsi2eeForElectronID', config = ElectronIDConf.config_default )
stream.appendLines( [
    StrippingElectronIDConf.line
    ])
'''

__author__=['Jibo He']
__date__ = '03/10/2010'
__version__= '$Revision: 1.2 $'


__all__ = (
    'ElectronIDConf',
    'makeEE'
    )


config_default =  {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        'HltFilter'               : None    ,

        'Both_PT'                 :  500.   ,  # MeV
        'Both_P'                  : 3000.   ,  # MeV
        'Both_TRCHI2DOF'          :    5.   ,
        'Both_MIPCHI2'            :   25.   ,
        
        'Tag_PT'                  : 1500.   ,  # MeV
        'Tag_P'                   : 6000.   ,  # MeV
        'Tag_PIDe'                :    5.   ,
        'Tag_MIPCHI2'             :   25.   ,

        'Probe_PT'                :  500.   ,  # MeV
        'Probe_P'                 : 3000.   ,  # MeV
        'Probe_MIPCHI2'           :   25.   ,

        'eeCombMinMass'           : 2100.   ,  # MeV         
        'eeCombMaxMass'           : 4300.   ,  # MeV   
        'eeVCHI2PDOF'             :    9.   ,  
        'eeMinMass'               : 2200.   ,  # MeV 
        'eeMaxMass'               : 4200.   ,  # MeV
        'eePT'                    : 2000.   , 
                
        'eeDLS'                   :   50.   ,
        'eeVZ'                    : -1.0e+9    # mm   
        }


config_convertedPhoton =  {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        'HltFilter'               :  "(HLT_PASS_RE('Hlt1MB.*Bias.*Decision'))",
        
        'Both_PT'                 :  500.   ,  # MeV
        'Both_P'                  : 3000.   ,  # MeV
        'Both_TRCHI2DOF'          :    5.   ,
        'Both_MIPCHI2'            :    9.   ,
        
        'Tag_PT'                  :  500.   ,  # MeV
        'Tag_P'                   : 3000.   ,  # MeV
        'Tag_PIDe'                :    5.   ,
        'Tag_MIPCHI2'             :    9.   ,

        'Probe_PT'                :  500.   ,  # MeV
        'Probe_P'                 : 3000.   ,  # MeV
        'Probe_MIPCHI2'           :    9.   ,

        'eeCombMinMass'           :    0.   ,  # MeV         
        'eeCombMaxMass'           :   70.   ,  # MeV   
        'eeVCHI2PDOF'             :    9.   ,  
        'eeMinMass'               :    0.   ,  # MeV 
        'eeMaxMass'               :   50.   ,  # MeV
        'eePT'                    : 2000.   ,
                
        'eeDLS'                   :   50.   ,  
        'eeVZ'                    : 1100.      # mm 
        }



from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class ElectronIDConf(LineBuilder):
    
    __configuration_keys__ = (
        'LinePrescale',
        'LinePostscale',
        'HltFilter',
        
        'Both_PT',
        'Both_P',
        'Both_TRCHI2DOF',
        'Both_MIPCHI2',
       
        'Tag_PT',
        'Tag_P',
        'Tag_PIDe',
        'Tag_MIPCHI2',

        'Probe_PT',
        'Probe_P',
        'Probe_MIPCHI2',
        
        'eeCombMinMass',
        'eeCombMaxMass',
        'eeVCHI2PDOF',
        'eeMinMass',
        'eeMaxMass',
        'eePT',
        
        'eeDLS',
        'eeVZ'
        )
    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        
        self.name = name 
        self.SelEE = makeEE( 'eeFor'+self.name,                             
                             #
                             Both_PT = config['Both_PT'],
                             Both_P = config['Both_P'],
                             Both_TRCHI2DOF = config['Both_TRCHI2DOF'],
                             Both_MIPCHI2 = config['Both_MIPCHI2'],
                             #
                             Tag_PT = config['Tag_PT'],
                             Tag_P = config['Tag_P'],
                             Tag_PIDe = config['Tag_PIDe'],
                             Tag_MIPCHI2 = config['Tag_MIPCHI2'],
                             #
                             Probe_PT = config['Probe_PT'],
                             Probe_P = config['Probe_P'],
                             Probe_MIPCHI2 = config['Probe_MIPCHI2'],
                             #
                             eeCombMinMass = config['eeCombMinMass'],
                             eeCombMaxMass = config['eeCombMaxMass'],
                             eeVCHI2PDOF = config['eeVCHI2PDOF'],
                             eeMinMass = config['eeMinMass'],
                             eeMaxMass = config['eeMaxMass'],
                             eePT      = config['eePT'],
                             #
                             eeDLS = config['eeDLS'],
                             eeVZ  = config['eeVZ']
                             )
        
        self.line = StrippingLine( self.name+"Line",
                                   HLT       = config['HltFilter'], 
                                   prescale  = config['LinePrescale'],
                                   postscale = config['LinePostscale'],                                   
                                   selection =  self.SelEE 
                                   )

        self.registerLine( self.line )
        
        
       
def makeEE( name,
            #
            Both_PT,
            Both_P,
            Both_TRCHI2DOF,
            Both_MIPCHI2,
            #
            Tag_PT,
            Tag_P,
            Tag_PIDe,
            Tag_MIPCHI2,
            #
            Probe_PT,
            Probe_P,
            Probe_MIPCHI2,
            #
            eeCombMinMass,
            eeCombMaxMass,
            eeVCHI2PDOF,
            eeMinMass,
            eeMaxMass,
            eePT,
            #
            eeDLS,
            eeVZ
            ):
    
    from StandardParticles import StdNoPIDsElectrons as NoPIDsElectronsForElectronID
    
    InAccCuts = "(0.5<PPINFO(LHCb.ProtoParticle.InAccEcal,-1))"

    BothCuts = "(PT> %(Both_PT)s *MeV)"\
               " & (TRCHI2DOF < %(Both_TRCHI2DOF)s)"\
               " & (MINTREE('e+'==ABSID,MIPCHI2DV(PRIMARY))> %(Both_MIPCHI2)s )" % locals()
    
    EEComCut = "(in_range(%(eeCombMinMass)s *MeV, AM, %(eeCombMaxMass)s *MeV))" % locals()
    
    EEMomCut = "(VFASPF(VCHI2)< %(eeVCHI2PDOF)s)"\
               " & (in_range(%(eeMinMass)s *MeV, MM, %(eeMaxMass)s *MeV))"\
               " & (PT>%(eePT)s)" \
               " & (BPVDLS>%(eeDLS)s)"\
               " & (VFASPF(VZ)>%(eeVZ)s *mm)" % locals()
    
    Tag1Cuts = "(CHILDCUT((PT>%(Tag_PT)s*MeV),1)) & (CHILDCUT((PIDe>%(Tag_PIDe)s),1)) & (CHILDCUT((P>%(Tag_P)s*MeV),1))"\
               " & (CHILDCUT((MIPCHI2DV(PRIMARY)> %(Tag_MIPCHI2)s),1))" % locals()
    Tag2Cuts = "(CHILDCUT((PT>%(Tag_PT)s*MeV),2)) & (CHILDCUT((PIDe>%(Tag_PIDe)s),2)) & (CHILDCUT((P>%(Tag_P)s*MeV),2))"\
               " & (CHILDCUT((MIPCHI2DV(PRIMARY)> %(Tag_MIPCHI2)s),2))" % locals()
    
    Probe1Cuts = "(CHILDCUT((PT>%(Probe_PT)s*MeV),1)) & (CHILDCUT((P>%(Probe_P)s*MeV),1))"\
                 " & (CHILDCUT((MIPCHI2DV(PRIMARY)> %(Probe_MIPCHI2)s),1))" % locals()
    Probe2Cuts = "(CHILDCUT((PT>%(Probe_PT)s*MeV),2)) & (CHILDCUT((P>%(Probe_P)s*MeV),2))"\
                 " & (CHILDCUT((MIPCHI2DV(PRIMARY)> %(Probe_MIPCHI2)s),2))" % locals()

    Tag1Probe2Cuts = Tag1Cuts + " & " + Probe2Cuts
    Tag2Probe1Cuts = Tag2Cuts + " & " + Probe1Cuts 
    
    _EE = CombineParticles( DecayDescriptor = "J/psi(1S) -> e+ e-",
                            DaughtersCuts = {"e+" : InAccCuts + "&" + BothCuts },
                            CombinationCut = EEComCut,
                            MotherCut = EEMomCut + " & ( ( " + Tag1Probe2Cuts + " ) | (" + Tag2Probe1Cuts + " ) ) "
                            )
    
    return Selection( name,
                      Algorithm = _EE,
                      RequiredSelections = [ NoPIDsElectronsForElectronID ]
                      )    
