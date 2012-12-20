__author__ = ['Marco Adinolfi']
__date__ = '22/08/2012'
__version__ = '$Revision: 1.1 $'

__all__ = ('DstarD02KShh_ForTrackingEffBuilder',
           'filterDaughters',
           'makeKstar',
           'makeDstar')

config_default = {'LongTrackGEC'          :  150
                , 'Trk_PT_MIN'            :  300.0
                , 'Trk_P_MIN'             : 3000.0
                , 'Pair_AMINDOCA_MAX'     :    0.1
                , 'Pair_BPVVD_MIN'        :    0.0
                , 'Pair_SumAPT_MIN'       :    0.0
                , 'Pair_BPVCORRM_MAX'     : 3500.0
                , 'TrkPt_SlowPion'        :  300.0
                , 'TrkP_SlowPion'         : 3000.0
                , 'TrkChi2_SlowPion'      :    2.25
                , 'TrkChi2_MAX_Child_MAX' :    2.25
                , 'IPCHI2_MAX_Child_MIN'  :   36.0
                , 'IPCHI2_PiSlow_MAX'     :    9.0
                , 'PairMaxDoca_Dstar'     :  100.0
                , 'PT_Dstar_MIN'          : 3750.0
                , 'DIRA_D0_MIN'           :    0.99
                , 'FDCHI2_D0_MIN'         :  100.0
                , 'VCHI2_D0_MAX'          :   10.0
                , 'M_MAX'                 : 1900.0
                , 'DeltaM_MIN'            :    0.0
                , 'DeltaM_MAX'            :  250.0
                , 'prescale'              :    1.0
                , 'postscale'             :    1.0
                }

from Gaudi.Configuration                   import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers                import Selection, DataOnDemand
from StrippingConf.StrippingLine           import StrippingLine
from StrippingUtils.Utils                  import LineBuilder

name = "DstarD02KShh_ForTrackingEff"

class DstarD02KShh_ForTrackingEffBuilder(LineBuilder):
    __configuration_keys__ = ('LongTrackGEC',
                              'Trk_PT_MIN',
                              'Trk_P_MIN',
                              'Pair_AMINDOCA_MAX',
                              'Pair_BPVVD_MIN',
                              'Pair_SumAPT_MIN',
                              'Pair_BPVCORRM_MAX',
                              'TrkPt_SlowPion',
                              'TrkP_SlowPion',
                              'TrkChi2_SlowPion',
                              'TrkChi2_MAX_Child_MAX',
                              'IPCHI2_MAX_Child_MIN',
                              'IPCHI2_PiSlow_MAX',
                              'PairMaxDoca_Dstar',
                              'PT_Dstar_MIN',
                              'DIRA_D0_MIN',
                              'FDCHI2_D0_MIN',
                              'VCHI2_D0_MAX',
                              'M_MAX',
                              'DeltaM_MIN',
                              'DeltaM_MAX',
                              'prescale',
                              'postscale')
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        # Set up global event cuts.
        # Conceptually these come first, although the place where they're
        # inserted into the line is at the bottom of the code.
        _globalEventCuts = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(LongTrackGEC)s )" % config,
                            "Preambulo": ["from LoKiTracks.decorators import *"]}

        # Select all kaons and pions.
        self.selDauKaon = filterDaughters('KFor'+name, 'Phys/StdAllLooseKaons/Particles',
                                          config['TrkChi2_MAX_Child_MAX'],
                                          config['Trk_PT_MIN'],
                                          config['Trk_P_MIN'],
                                          config['IPCHI2_MAX_Child_MIN'])
        self.selDauPion = filterDaughters('PiFor'+name, 'Phys/StdAllLoosePions/Particles',
                                          config['TrkChi2_MAX_Child_MAX'],
                                          config['Trk_PT_MIN'],
                                          config['Trk_P_MIN'],
                                          config['IPCHI2_MAX_Child_MIN'])

        self.selDauPP = [self.selDauPion]
        self.selDauKK = [self.selDauKaon]
        self.selDauKP = [self.selDauKaon, self.selDauPion]
        
        # Now make the various decays of the K*(892)0
        strDecaysPiPi         = ["K*(892)0 -> pi+ pi-"]
        strDecaysKK           = ["K*(892)0 -> K+ K-"]
        strDecaysKPlusPiMinus = ["K*(892)0 -> K+ pi-"]
        strDecaysKMinusPiPlus = ["K*(892)0 -> K- pi+"]
        
        self.selKstarPiPi         = makeKstar('Kstar2PiPi'        + name, strDecaysPiPi,         self.selDauPP, config['Pair_SumAPT_MIN'], config['Pair_AMINDOCA_MAX'], config['Pair_BPVVD_MIN'], config['Pair_BPVCORRM_MAX'], config['FDCHI2_D0_MIN'], config['DIRA_D0_MIN'], config['M_MAX'], config['VCHI2_D0_MAX'])
        self.selKstarKK           = makeKstar('Kstar2KK'          + name, strDecaysKK  ,         self.selDauKK, config['Pair_SumAPT_MIN'], config['Pair_AMINDOCA_MAX'], config['Pair_BPVVD_MIN'], config['Pair_BPVCORRM_MAX'], config['FDCHI2_D0_MIN'], config['DIRA_D0_MIN'], config['M_MAX'], config['VCHI2_D0_MAX'])
        self.selKstarKPlusPiMinus = makeKstar('Kstar2KPlusPiMinus'+ name, strDecaysKPlusPiMinus, self.selDauKP, config['Pair_SumAPT_MIN'], config['Pair_AMINDOCA_MAX'], config['Pair_BPVVD_MIN'], config['Pair_BPVCORRM_MAX'], config['FDCHI2_D0_MIN'], config['DIRA_D0_MIN'], config['M_MAX'], config['VCHI2_D0_MAX'])
        self.selKstarKMinusPiPlus = makeKstar('Kstar2KMinusPiPlus'+ name, strDecaysKMinusPiPlus, self.selDauKP, config['Pair_SumAPT_MIN'], config['Pair_AMINDOCA_MAX'], config['Pair_BPVVD_MIN'], config['Pair_BPVCORRM_MAX'], config['FDCHI2_D0_MIN'], config['DIRA_D0_MIN'], config['M_MAX'], config['VCHI2_D0_MAX'])

        # Now make the D* -> K*(892)0 pi
        self.selDstarPiPi         = makeDstar('Dstar_PiPi'         + name, [self.selKstarPiPi],         config['IPCHI2_PiSlow_MAX'], config['PairMaxDoca_Dstar'], config['PT_Dstar_MIN'], config['DeltaM_MIN'], config['DeltaM_MAX'])
        self.selDstarKK           = makeDstar('Dstar_KK'           + name, [self.selKstarKK],           config['IPCHI2_PiSlow_MAX'], config['PairMaxDoca_Dstar'], config['PT_Dstar_MIN'], config['DeltaM_MIN'], config['DeltaM_MAX'])
        self.selDstarKPlusPiMinus = makeDstar('Dstar_KPlusPiMinus' + name, [self.selKstarKPlusPiMinus], config['IPCHI2_PiSlow_MAX'], config['PairMaxDoca_Dstar'], config['PT_Dstar_MIN'], config['DeltaM_MIN'], config['DeltaM_MAX'])
        self.selDstarKMinusPiPlus = makeDstar('Dstar_KMinusPiPlus' + name, [self.selKstarKMinusPiPlus], config['IPCHI2_PiSlow_MAX'], config['PairMaxDoca_Dstar'], config['PT_Dstar_MIN'], config['DeltaM_MIN'], config['DeltaM_MAX'])

        # Make the stripping lines
        self.linePiPi          = StrippingLine(name+'PiPiLine',
                                               FILTER    = _globalEventCuts,
                                               prescale  = config['prescale'],
                                               postscale = config['postscale'],
                                               selection = self.selDstarPiPi)
        self.lineKK            = StrippingLine(name + 'KKLine',
                                               FILTER    = _globalEventCuts,
                                               prescale  = config['prescale'],
                                               postscale = config['postscale'],
                                               selection = self.selDstarKK)
        self.lineKPlusPiMinus  = StrippingLine(name + 'KPlusPiMinusLine',
                                               FILTER    = _globalEventCuts,
                                               prescale  = config['prescale'],
                                               postscale = config['postscale'],
                                               selection = self.selDstarKPlusPiMinus)
        self.lineKMinusPiPlus  = StrippingLine(name + 'KMinusPiPlusLine',
                                               FILTER    = _globalEventCuts,
                                               prescale  = config['prescale'],
                                               postscale = config['postscale'],
                                               selection = self.selDstarKMinusPiPlus)

        self.registerLine(self.linePiPi)
        self.registerLine(self.lineKK)
        self.registerLine(self.lineKPlusPiMinus)
        self.registerLine(self.lineKMinusPiPlus)
        
def filterDaughters(name, inputName, TrkChi2_MAX_Child_MAX, Trk_PT_MIN, Trk_P_MIN, IPCHI2_MAX_Child_MIN):
    """
    Apply cuts to daughters of K*(892)0
    """
    _strCutChi2   = "(TRCHI2DOF < %(TrkChi2_MAX_Child_MAX)s)"     % locals()
    _strCutPT     = "(PT > %(Trk_PT_MIN)s)"                       % locals()
    _strCutP      = "(P  > %(Trk_P_MIN)s)"                        % locals()
    _strCutIPChi2 = "(MIPCHI2DV(PRIMARY)> %(IPCHI2_MAX_Child_MIN)s)" % locals()
    
    _strCuts = '(' + _strCutChi2 + '&' + _strCutPT + '&' + _strCutP \
                   + '&' + _strCutIPChi2 + ')'
    _trackFilter = FilterDesktop(Code = _strCuts)
    inputTracks = DataOnDemand(Location = inputName)
    return Selection(name,
                     Algorithm          = _trackFilter,
                     RequiredSelections = [inputTracks])

def makeKstar(name, inputDecayDescriptors, inputDaughters, Pair_SumAPT_MIN, Pair_AMINDOCA_MAX, Pair_BPVVD_MIN, Pair_BPVCORRM_MAX, FDCHI2_D0_MIN, DIRA_D0_MIN, M_MAX, VCHI2_D0_MAX):
    """
    Given lists of K*(892)0 daughter tracks  reconstruct K*(892)0 -> h+ h-.
    The same routine works for all final states.
    """

    _combCutsMass = "(AM<2100*MeV)" 
    _combCutsPT   = "((APT1+APT2)> %(Pair_SumAPT_MIN)s)"                                % locals()
    _combCutsDOCA = "(AMINDOCA('LoKi::TrgDistanceCalculator') < %(Pair_AMINDOCA_MAX)s)" % locals()
    _combCutsPV   = "(AALLSAMEBPV)"
    
    _combCuts = '(' + _combCutsMass + '&' + _combCutsPT + '&' \
                    + _combCutsDOCA + '&' + _combCutsPV + ')'
    
    _motherCutsFD       = '(BPVVD> %(Pair_BPVVD_MIN)s)'            % locals()
    _motherCutsCorrMass = '(BPVCORRM < %(Pair_BPVCORRM_MAX)s)'     % locals()
    _motherCutsFDChi2   = '(BPVVDCHI2> %(FDCHI2_D0_MIN)s)'         % locals()
    _motherCutsDira     = '(BPVDIRA < %(DIRA_D0_MIN)s)'            % locals()
    _motherCutsMass     = '(M < %(M_MAX)s)'                        % locals()
    _motherCutsVtx      = '(VFASPF(VCHI2PDOF) < %(VCHI2_D0_MAX)s)' % locals()
    
    _motherCuts = '(' + _motherCutsFD       + '&' + \
                        _motherCutsCorrMass + '&' + \
                        _motherCutsFDChi2   + '&' + \
                        _motherCutsDira     + '&' + \
                        _motherCutsMass     + '&' + \
                        _motherCutsVtx      + ')'
    
    _Kst = CombineParticles(DecayDescriptors = inputDecayDescriptors,
                            CombinationCut   = _combCuts,
                            MotherCut        = _motherCuts)
    return Selection(name,
                     Algorithm = _Kst,
                     RequiredSelections = inputDaughters)

def makeDstar(name, inputKstar, IPCHI2_PiSlow_MAX, PairMaxDoca_Dstar, PT_Dstar_MIN, DeltaM_MIN, DeltaM_MAX):
    """
    Given a list of K*(892)0, try to make D*+ -> K*(892)0 pi+
    """
    _softPi = DataOnDemand(Location = 'Phys/StdAllLoosePions/Particles')
    
    
    _cutsSoftPi = '( MIPCHI2DV(PRIMARY) < %(IPCHI2_PiSlow_MAX)s)' % locals()
    
    _cutsDstarCombBase = '(AALLSAMEBPV)' 
    _cutsDstarCombDoca = "((AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Dstar)s))" % locals()
    
    _cutsDstarComb = '(' + _cutsDstarCombBase + '&' + \
                           _cutsDstarCombDoca + ')'
    
    
    _cutsDstarMothPT       = '(PT > %(PT_Dstar_MIN)s)' % locals()
    _cutsDstarMothMassLow  = "(M-MAXTREE('K*(892)0'==ABSID,M)>%(DeltaM_MIN)s)" % locals()
    _cutsDstarMothMassHigh = "(M-MAXTREE('K*(892)0'==ABSID,M)<%(DeltaM_MAX)s)" % locals()
    
    _cutsDstarMoth = '(' + _cutsDstarMothPT + '&' + \
                     _cutsDstarMothMassLow  + '&' + \
                     _cutsDstarMothMassHigh + ')'
    
    _Dstar = CombineParticles(DecayDescriptors = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-"],
                              DaughtersCuts    = {"pi+" : _cutsSoftPi},
                              CombinationCut   = _cutsDstarComb,
                              MotherCut        = _cutsDstarMoth)
    return Selection(name,
                     Algorithm = _Dstar,
                     RequiredSelections = inputKstar + [_softPi])
    
    
