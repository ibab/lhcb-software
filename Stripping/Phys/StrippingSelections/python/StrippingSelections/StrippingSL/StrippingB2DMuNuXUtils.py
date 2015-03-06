__author__ = ['Mika Vesterinen']
__date__ = '06/03/2015'
__version__ = '$Revision: 0.0 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons, StdNoPIDsPions, StdLooseElectrons, StdNoPIDsMuons

def BtoDlnuLine(module_name,
                name,
                BDecays,
                DDecays,
                CONFIG,
                CHARM_DAUGHTERS,
                MUON):
    
    DEFAULT_GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" %CONFIG,
                     "Preambulo": ["from LoKiTracks.decorators import *"]}
    
    DEFAULT_HLT = CONFIG["HLT_FILTER"]
    
    CHARM_DaugCuts = {}
    CHARM_ComboCuts = CONFIG["CharmComboCuts"]
    CHARM_MotherCuts = CONFIG["CharmMotherCuts"]
    
    if "CharmDaugCuts" in CONFIG.keys():
        CHARM_DaugCuts = CONFIG["CharmDaugCuts"]
    if "CharmExtraComboCuts" in CONFIG.keys():
        CHARM_ComboCuts += CONFIG["CharmExtraComboCuts"]
    if "CharmExtraMotherCuts" in CONFIG.keys():
        CHARM_MotherCuts += CONFIG["CharmExtraMotherCuts"]
    
    CHARM = Selection("CharmSelFor"+name+module_name,
                      Algorithm=CombineParticles(DecayDescriptors = DDecays,
                                                 DaughtersCuts = CHARM_DaugCuts,
                                                 CombinationCut = CHARM_ComboCuts,
                                                 MotherCut = CHARM_MotherCuts),
                      RequiredSelections = CHARM_DAUGHTERS)

    USED_CHARM = CHARM
    if "D*" in BDecays:
        DST = makeDstar("CharmSelDstFor"+name+module_name,CHARM,CONFIG)
        USED_CHARM = DST

    B_combinationCut = "(AM > %(BMassMin)s*GeV) & (AM < %(BMassMax)s*GeV) & (ADOCACHI2CUT( %(B_DocaChi2Max)s, ''))" %CONFIG
    B_motherCut = " (MM>%(BMassMin)s*GeV) & (MM<%(BMassMax)s*GeV) &  (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(B_DIRA)s)  " \
        "& (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > %(B_D_DZ)s *mm ) " %CONFIG
    if "ExtraComboCuts" in CONFIG.keys():
        B_combinationCut += CONFIG["ExtraComboCuts"]
    if "ExtraMotherCuts" in CONFIG.keys():
        B_motherCut += CONFIG["ExtraMotherCuts"]
        
    B_DaugCuts = {}
    if "ExtraMuonCuts" in CONFIG.keys():
        B_DaugCuts = {"mu+":CONFIG["ExtraMuonCuts"]}
    if "ExtraElectronCuts" in CONFIG.keys():
        B_DaugCuts = {"e+":CONFIG["ExtraElectronCuts"]}
    _B = CombineParticles(DecayDescriptors = BDecays,
                          DaughtersCuts = B_DaugCuts,
                          CombinationCut = B_combinationCut,
                          MotherCut = B_motherCut)
                     
    BSel = Selection ("BSelFor"+name+module_name,
                      Algorithm = _B,
                      RequiredSelections = [MUON,USED_CHARM])
    
    ### invert the order for the fakes line
    ### to be more efficient
    if "Fake" in name:
        BSel.RequiredSelections = [USED_CHARM,MUON]
    
    BSelTOS = TOSFilter( "BSelFor"+name+module_name+"TOS"
                         ,BSel
                         ,CONFIG["TTSpecs"])
    
    debug = False
    if debug:
        print [name,DEFAULT_HLT,CHARM_DaugCuts,Charm_ComboCuts,CHARM_MotherCuts,B_DaugCuts,B_combinationCut,B_motherCut]
    _prescale = 1.0
    if name in CONFIG["prescales"].keys():
        _prescale = CONFIG["prescales"][name]

    return StrippingLine(name + module_name + 'Line', 
                         HLT = DEFAULT_HLT,
                         FILTER=DEFAULT_GECs,
                         prescale = _prescale,
                         selection = BSelTOS)

########### HELP WITH MAKING A DSTAR ########################
def makeDstar(_name, inputD0,CONFIG) : 
    _softPi = DataOnDemand(Location = 'Phys/StdAllLoosePions/Particles')
    _inputD0_conj = Selection("SelConjugateD0For"+_name,
                             Algorithm = ConjugateNeutralPID('ConjugateD0For'+_name),
                             RequiredSelections = [inputD0])
    _cutsSoftPi = '( PT > %(Dstar_SoftPion_PT)s *MeV )' % CONFIG
    _cutsDstarComb = '(AM - ACHILD(M,1) + 5 > %(Dstar_wideDMCutLower)s *MeV) & (AM - ACHILD(M,1) - 5 < %(Dstar_wideDMCutUpper)s *MeV)' % CONFIG
    _cutsDstarMoth_base = '(VFASPF(VCHI2/VDOF) < %(Dstar_Chi2)s )' % CONFIG
    _cutsDstarMoth_DM = '(M - CHILD(M,1) > %(Dstar_wideDMCutLower)s *MeV) & (M - CHILD(M,1) < %(Dstar_wideDMCutUpper)s *MeV)' % CONFIG
    _cutsDstarMoth = '(' + _cutsDstarMoth_base + ' & ' + _cutsDstarMoth_DM + ')'
    _Dstar = CombineParticles( DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc",
                               DaughtersCuts = { "pi+" : _cutsSoftPi },
                               CombinationCut = _cutsDstarComb,
                               MotherCut = _cutsDstarMoth)
    return Selection (name = "Sel"+_name,Algorithm = _Dstar,RequiredSelections = [inputD0,_inputD0_conj] + [_softPi])

########## TISTOS FILTERING ##################################
def TOSFilter( name = None, sel = None, trigger = None ):
    if len(trigger) == 0:
        return sel
    from Configurables import TisTosParticleTagger
    _filter = TisTosParticleTagger(name+"_TriggerTos")
    _filter.TisTosSpecs = trigger
    _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = [ sel ], Algorithm = _filter )
    return _sel

