__author__ = ['Marco Adinolfi']
__date__ = '22/08/2012'
__version__ = '$Revision: 1.1 $'

__all__ = ('DstarD02KShh_ForTrackingEffBuilder',
           'default_config',
           'filterDaughters',
           'makeD0',
           'makeDstar')

default_config = {
        'NAME' : 'DstarD02KShhForTrackingEff',
        'BUILDERTYPE' : 'DstarD02KShh_ForTrackingEffBuilder',
        'CONFIG' : {'LongTrackGEC'          :    150
                  , 'Trk_PT_MIN'            :  600.0
                  , 'Trk_P_MIN'             :10000.0
                  , 'Trk_GHOST_MAX'         :    0.4
                  , 'Pair_AMINDOCA_MAX'     :    0.1
                  , 'Pair_BPVVD_MIN'        :    0.0
                  , 'Pair_SumAPT_MIN'       : 2500.0
                  , 'Pair_BPVCORRM_MAX'     : 3000.0
                  , 'TrkPt_SlowPion'        :    0.0
                  , 'TrkP_SlowPion'         : 3000.0
                  , 'TrkChi2_SlowPion'      :    2.25
                  , 'TrkChi2_MAX_Child_MAX' :    2.25
                  , 'KAON_PIDK_MIN'         :    3.0
                  , 'PION_PIDK_MAX'         :    0.0
                  , 'IPCHI2_MAX_Child_MIN'  :   16.0
                  , 'IPCHI2_PiSlow_MAX'     :    9.0
                  , 'PairMaxDoca_Dstar'     :  100.0
                  , 'PT_Dstar_MIN'          : 2500.0
                  , 'DIRA_D0_MIN'           :    0.999
                  , 'FDCHI2_D0_MIN'         :   80.0
                  , 'VCHI2_D0_MAX'          :    4.0
                  , 'M_MIN'                 :    0.0
                  , 'M_MAX'                 : 1800.0
                  , 'PhiM_MIN'              : 1000.0
                  , 'PhiM_MAX'              : 1040.0
                  , 'DeltaM_MIN'            :    0.0
                  , 'DeltaM_MAX'            :  250.0
                  , 'HLTFILTER'             : "(HLT_PASS_RE('Hlt2CharmHadD02HHXDst.*Decision')|HLT_PASS('Hlt2IncPhiDecision'))"
                  , 'Hlt2TisTosSpec'        : { 'Hlt2CharmHadD02HHXDst.*Decision%TOS' : 0, 'Hlt2IncPhiDecision%TOS' : 0}
                  , 'KKprescale'            :    1.0
                  , 'PiPiprescale'          :    0.1
                  , 'KPlusPiMinusprescale'  :    0.1
                  , 'KMinusPiPlusprescale'  :    0.1
                  , 'postscale'             :    1.0
                  },
        'WGs' : ['ALL'],
        'STREAMS' : ['Calibration']
                }

from Gaudi.Configuration                   import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers                import Selection, DataOnDemand
from StrippingConf.StrippingLine           import StrippingLine
from StrippingUtils.Utils                  import LineBuilder
from Configurables import TisTosParticleTagger

name = "DstarD02KShh_ForTrackingEff"

class DstarD02KShh_ForTrackingEffBuilder(LineBuilder):
    __configuration_keys__ = ('LongTrackGEC',
                              'Trk_PT_MIN',
                              'Trk_P_MIN',
                              'Trk_GHOST_MAX',
                              'Pair_AMINDOCA_MAX',
                              'Pair_BPVVD_MIN',
                              'Pair_SumAPT_MIN',
                              'Pair_BPVCORRM_MAX',
                              'TrkPt_SlowPion',
                              'TrkP_SlowPion',
                              'TrkChi2_SlowPion',
                              'TrkChi2_MAX_Child_MAX',
                              'PION_PIDK_MAX',
                              'KAON_PIDK_MIN',
                              'IPCHI2_MAX_Child_MIN',
                              'IPCHI2_PiSlow_MAX',
                              'PairMaxDoca_Dstar',
                              'PT_Dstar_MIN',
                              'DIRA_D0_MIN',
                              'FDCHI2_D0_MIN',
                              'VCHI2_D0_MAX',
                              'M_MIN',
                              'M_MAX',
                              'PhiM_MIN',
                              'PhiM_MAX',
                              'DeltaM_MIN',
                              'DeltaM_MAX',
                              'HLTFILTER',
                              'Hlt2TisTosSpec',
                              'KKprescale',
                              'PiPiprescale',
                              'KPlusPiMinusprescale',
                              'KMinusPiPlusprescale',
                              'postscale')
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        # Set up global event cuts.
        # Conceptually these come first, although the place where they're
        # inserted into the line is at the bottom of the code.
        _globalEventCuts = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(LongTrackGEC)s )" % config,
                            "Preambulo": ["from LoKiTracks.decorators import *"]}

        # Select all kaons and pions.
        self.selDauKaon = filterKaons('KFor'+name, 'Phys/StdLooseKaons/Particles',
                                          config['TrkChi2_MAX_Child_MAX'],
                                          config['Trk_PT_MIN'],
                                          config['Trk_P_MIN'],
                                          config['Trk_GHOST_MAX'],
                                          config['KAON_PIDK_MIN'],
                                          config['IPCHI2_MAX_Child_MIN'])
        self.selDauPion = filterPions('PiFor'+name, 'Phys/StdLoosePions/Particles',
                                          config['TrkChi2_MAX_Child_MAX'],
                                          config['Trk_PT_MIN'],
                                          config['Trk_P_MIN'],
                                          config['Trk_GHOST_MAX'],
                                          config['PION_PIDK_MAX'],
                                          config['IPCHI2_MAX_Child_MIN'])

        self.selDauPP = [self.selDauPion]
        self.selDauKK = [self.selDauKaon]
        self.selDauKP = [self.selDauKaon,self.selDauPion]
        
        # Now make the various decays of the D0
        strDecaysPiPi         = ["D0 -> pi+ pi-"]
        strDecaysKPlusPiMinus = ["D0 -> K+ pi-"]
        strDecaysKMinusPiPlus = ["D0 -> K- pi+"]
        strDecaysKK           = ["D0 -> K+ K-"]
        
        self.selD0PiPi         = makeD0('D02PiPi'        + name, strDecaysPiPi,         self.selDauPP, config['Pair_SumAPT_MIN'], config['Pair_AMINDOCA_MAX'], config['Pair_BPVVD_MIN'], config['Pair_BPVCORRM_MAX'], config['FDCHI2_D0_MIN'], config['DIRA_D0_MIN'],config['M_MIN'], config['M_MAX'], config['VCHI2_D0_MAX'])
        self.selD0KK           = makeD0('D02KK'          + name, strDecaysKK  ,         self.selDauKK, config['Pair_SumAPT_MIN'], config['Pair_AMINDOCA_MAX'], config['Pair_BPVVD_MIN'], config['Pair_BPVCORRM_MAX'], config['FDCHI2_D0_MIN'], config['DIRA_D0_MIN'],config['PhiM_MIN'], config['PhiM_MAX'], config['VCHI2_D0_MAX'])
        self.selD0KMinusPiPlus         = makeD0('D02KMinusPiPlus'        + name, strDecaysKMinusPiPlus,self.selDauKP, config['Pair_SumAPT_MIN'], config['Pair_AMINDOCA_MAX'], config['Pair_BPVVD_MIN'], config['Pair_BPVCORRM_MAX'], config['FDCHI2_D0_MIN'], config['DIRA_D0_MIN'],config['M_MIN'], config['M_MAX'], config['VCHI2_D0_MAX'])
        self.selD0KPlusPiMinus         = makeD0('D02KPlusPiMinus'        + name, strDecaysKPlusPiMinus,self.selDauKP, config['Pair_SumAPT_MIN'], config['Pair_AMINDOCA_MAX'], config['Pair_BPVVD_MIN'], config['Pair_BPVCORRM_MAX'], config['FDCHI2_D0_MIN'], config['DIRA_D0_MIN'],config['M_MIN'], config['M_MAX'], config['VCHI2_D0_MAX'])



        # Now make the D* -> D)0 pi
        self.selDstarPiPi         = makeDstar('Dstar_PiPi'         + name, [self.selD0PiPi],         config['IPCHI2_PiSlow_MAX'], config['PairMaxDoca_Dstar'], config['PT_Dstar_MIN'], config['DeltaM_MIN'], config['DeltaM_MAX'])
        self.selDstarKK           = makeDstar('Dstar_KK'           + name, [self.selD0KK],           config['IPCHI2_PiSlow_MAX'], config['PairMaxDoca_Dstar'], config['PT_Dstar_MIN'], config['DeltaM_MIN'], config['DeltaM_MAX'])
        self.selDstarKPlusPiMinus = makeDstar('Dstar_KPlusPiMinus' + name, [self.selD0KPlusPiMinus], config['IPCHI2_PiSlow_MAX'], config['PairMaxDoca_Dstar'], config['PT_Dstar_MIN'], config['DeltaM_MIN'], config['DeltaM_MAX'])
        self.selDstarKMinusPiPlus = makeDstar('Dstar_KMinusPiPlus' + name, [self.selD0KMinusPiPlus], config['IPCHI2_PiSlow_MAX'], config['PairMaxDoca_Dstar'], config['PT_Dstar_MIN'], config['DeltaM_MIN'], config['DeltaM_MAX'])


        self.selDstarPiPi_TisTos = makeTisTos(name+'SelDstarPiPi'
                                   , selection = self.selDstarPiPi
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selDstarKK_TisTos = makeTisTos(name+'SelDstarKK'
                                   , selection = self.selDstarKK
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selDstarKPlusPiMinus_TisTos = makeTisTos(name+'SelDstarKPlusPiMinus'
                                   , selection = self.selDstarKPlusPiMinus
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selDstarKMinusPiPlus_TisTos = makeTisTos(name+'SelDstarKMinusPiPlus'
                                   , selection = self.selDstarKMinusPiPlus
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])

        
        # Make the stripping lines
        self.linePiPi          = StrippingLine(name+'PiPiLine',
                                               FILTER    = _globalEventCuts,
                                               prescale  = config['PiPiprescale'],
                                               postscale = config['postscale'],
                                               RequiredRawEvents = ["Tracker","Velo"],
                                               HLT2 = config['HLTFILTER'],
                                               selection = self.selDstarPiPi_TisTos)
        self.lineKK            = StrippingLine(name + 'KKLine',
                                               FILTER    = _globalEventCuts,
                                               prescale  = config['KKprescale'],
                                               postscale = config['postscale'],
                                               RequiredRawEvents = ["Tracker","Velo"],
                                               HLT2 = config['HLTFILTER'],
                                               selection = self.selDstarKK_TisTos)
        self.lineKPlusPiMinus  = StrippingLine(name+'KPlusPiMinusLine',
                                               FILTER    = _globalEventCuts,
                                               prescale  = config['KPlusPiMinusprescale'],
                                               postscale = config['postscale'],
                                               RequiredRawEvents = ["Tracker","Velo"],
                                               HLT2 = config['HLTFILTER'],
                                               selection = self.selDstarKPlusPiMinus_TisTos)
        self.lineKMinusPiPlus  = StrippingLine(name+'KMinusPiPlusLine',
                                               FILTER    = _globalEventCuts,
                                               prescale  = config['KMinusPiPlusprescale'],
                                               postscale = config['postscale'],
                                               RequiredRawEvents = ["Tracker","Velo"],
                                               HLT2 = config['HLTFILTER'],
                                               selection = self.selDstarKMinusPiPlus_TisTos)

        self.registerLine(self.linePiPi)
        self.registerLine(self.lineKK)
        self.registerLine(self.lineKPlusPiMinus)
        self.registerLine(self.lineKMinusPiPlus)

        
def filterPions(name, inputName, TrkChi2_MAX_Child_MAX, Trk_PT_MIN, Trk_P_MIN,Trk_GHOST_MAX,PION_PIDK_MAX,IPCHI2_MAX_Child_MIN):
    """
    Apply cuts to daughters of D0
    """
    _strCutChi2   = "(TRCHI2DOF < %(TrkChi2_MAX_Child_MAX)s)"     % locals()
    _strCutPT     = "(PT > %(Trk_PT_MIN)s)"                       % locals()
    _strCutP      = "(P  > %(Trk_P_MIN)s)"                        % locals()
    _strCutPID    = "(PIDK  < %(PION_PIDK_MAX)s)"                 % locals()
    _strCutGHOST  = "(TRGHP  < %(Trk_GHOST_MAX)s)"                % locals()
    _strCutIPChi2 = "(MIPCHI2DV(PRIMARY)> %(IPCHI2_MAX_Child_MIN)s)" % locals()
    
    _strCuts = '((~ISMUON)&' + _strCutChi2 + '&' + _strCutPT + '&' + _strCutP \
                   + '&' + _strCutPID \
                   + '&' + _strCutGHOST + '&' + _strCutIPChi2 + ')'
    _trackFilter = FilterDesktop(Code = _strCuts)
    inputTracks = DataOnDemand(Location = inputName)
    return Selection(name,
                     Algorithm          = _trackFilter,
                     RequiredSelections = [inputTracks])
                     
def filterKaons(name, inputName, TrkChi2_MAX_Child_MAX, Trk_PT_MIN, Trk_P_MIN,Trk_GHOST_MAX,KAON_PIDK_MIN, IPCHI2_MAX_Child_MIN):
    """
    Apply cuts to daughters of D0
    """
    _strCutChi2   = "(TRCHI2DOF < %(TrkChi2_MAX_Child_MAX)s)"     % locals()
    _strCutPT     = "(PT > %(Trk_PT_MIN)s)"                       % locals()
    _strCutP      = "(P  > %(Trk_P_MIN)s)"                        % locals()
    _strCutPID    = "(PIDK  > %(KAON_PIDK_MIN)s)"                 % locals()
    _strCutGHOST  = "(TRGHP  < %(Trk_GHOST_MAX)s)"                % locals()
    _strCutIPChi2 = "(MIPCHI2DV(PRIMARY)> %(IPCHI2_MAX_Child_MIN)s)" % locals()
    
    _strCuts = '((~ISMUON)&' + _strCutChi2 + '&' + _strCutPT + '&' + _strCutP \
                   + '&' + _strCutPID \
                   + '&' + _strCutGHOST + '&' + _strCutIPChi2 + ')'
    _trackFilter = FilterDesktop(Code = _strCuts)
    inputTracks = DataOnDemand(Location = inputName)
    return Selection(name,
                     Algorithm          = _trackFilter,
                     RequiredSelections = [inputTracks])                     

def makeD0(name, inputDecayDescriptors, inputDaughters, Pair_SumAPT_MIN, Pair_AMINDOCA_MAX, Pair_BPVVD_MIN, Pair_BPVCORRM_MAX, FDCHI2_D0_MIN, DIRA_D0_MIN,M_MIN, M_MAX, VCHI2_D0_MAX):
    """
    Given lists of D0 daughter tracks  reconstruct D0 -> h+ h-.
    The same routine works for all final states.
    """

    _combCutsMass = "(AM<1200*MeV)" 
    _combCutsPT   = "((APT1+APT2)> %(Pair_SumAPT_MIN)s)"                                % locals()
    _combCutsDOCA = "(AMINDOCA('LoKi::TrgDistanceCalculator') < %(Pair_AMINDOCA_MAX)s)" % locals()
    _combCutsPV   = "(AALLSAMEBPV)"
    
    _combCuts = '(' + _combCutsMass + '&' + _combCutsPT + '&' \
                    + _combCutsDOCA + '&' + _combCutsPV + ')'
    
    _motherCutsFD       = '(BPVVD> %(Pair_BPVVD_MIN)s)'            % locals()
    _motherCutsCorrMass = '(BPVCORRM < %(Pair_BPVCORRM_MAX)s)'     % locals()
    _motherCutsFDChi2   = '(BPVVDCHI2> %(FDCHI2_D0_MIN)s)'         % locals()
    _motherCutsDira     = '(BPVDIRA > %(DIRA_D0_MIN)s)'            % locals()
    _motherCutsMass_MIN     = '(M > %(M_MIN)s)'                        % locals()
    _motherCutsMass_MAX     = '(M < %(M_MAX)s)'                        % locals()
    _motherCutsVtx      = '(VFASPF(VCHI2PDOF) < %(VCHI2_D0_MAX)s)' % locals()
    
    _motherCuts = '(' + _motherCutsFD           + '&' + \
                        _motherCutsCorrMass     + '&' + \
                        _motherCutsFDChi2       + '&' + \
                        _motherCutsDira         + '&' + \
                        _motherCutsMass_MIN     + '&' + \
                        _motherCutsMass_MAX     + '&' + \
                        _motherCutsVtx          + ')'
    
    _D0 = CombineParticles(DecayDescriptors = inputDecayDescriptors,
                            CombinationCut   = _combCuts,
                            MotherCut        = _motherCuts)
    return Selection(name,
                     Algorithm = _D0,
                     RequiredSelections = inputDaughters)

def makeDstar(name, inputD0, IPCHI2_PiSlow_MAX, PairMaxDoca_Dstar, PT_Dstar_MIN, DeltaM_MIN, DeltaM_MAX):
    """
    Given a list of D0, try to make D*+ -> D0 pi+
    """
    _softPi = DataOnDemand(Location = 'Phys/StdAllLoosePions/Particles')
    
    
    _cutsSoftPi = '( MIPCHI2DV(PRIMARY) < %(IPCHI2_PiSlow_MAX)s)' % locals()
    
    _cutsDstarCombBase = '(AALLSAMEBPV)' 
    _cutsDstarCombDoca = "((AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Dstar)s))" % locals()
    
    _cutsDstarComb = '(' + _cutsDstarCombBase + '&' + \
                           _cutsDstarCombDoca + ')'
    
    
    _cutsDstarMothPT       = '(PT > %(PT_Dstar_MIN)s)' % locals()
    _cutsDstarMothMassLow  = "(M-MAXTREE('D0'==ABSID,M)>%(DeltaM_MIN)s)" % locals()
    _cutsDstarMothMassHigh = "(M-MAXTREE('D0'==ABSID,M)<%(DeltaM_MAX)s)" % locals()
    
    _cutsDstarMoth = '(' + _cutsDstarMothPT + '&' + \
                     _cutsDstarMothMassLow  + '&' + \
                     _cutsDstarMothMassHigh + ')'
    
    _Dstar = CombineParticles(DecayDescriptors = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"],
                              DaughtersCuts    = {"pi+" : _cutsSoftPi},
                              CombinationCut   = _cutsDstarComb,
                              MotherCut        = _cutsDstarMoth)
    return Selection(name,
                     Algorithm = _Dstar,
                     RequiredSelections = inputD0 + [_softPi])
                     
def makeTisTos( name, selection, hltTisTosSpec = { } ) :
    outSel = selection
    if len(hltTisTosSpec) > 0:
        _tisTosFilter = TisTosParticleTagger( name + 'TisTos'
                                              , TisTosSpecs = hltTisTosSpec )

        outSel = Selection( name
                            , Algorithm = _tisTosFilter
                            , RequiredSelections = [ selection ] )

    return outSel
    
    
