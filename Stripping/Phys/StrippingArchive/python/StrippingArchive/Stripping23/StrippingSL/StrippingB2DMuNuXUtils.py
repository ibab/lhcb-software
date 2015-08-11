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
                MUON,
                FAKE_MUON = None):
    
    DEFAULT_GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" %CONFIG,
                     "Preambulo": ["from LoKiTracks.decorators import *"]}
    
    CHARM_DaugCuts = {}
    CHARM_ComboCuts = CONFIG["CharmComboCuts"]
    CHARM_MotherCuts = CONFIG["CharmMotherCuts"]
    CHARM_ComboCuts += " & (ADOCACHI2CUT( %(D_DocaChi2Max)s, ''))" % CONFIG
    CHARM_MotherCuts += "& (VFASPF(VCHI2/VDOF) < %(D_VCHI2DOF)s) " \
        "& (BPVVDCHI2 > %(D_FDCHI2)s) &  (BPVDIRA> %(D_BPVDIRA)s)"  % CONFIG
    
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

    B_combinationCut = "(AM > %(B_MassMin)s) & (AM < %(B_MassMax)s) & (ADOCACHI2CUT( %(B_DocaChi2Max)s, ''))" %CONFIG
    B_motherCut = " (MM>%(B_MassMin)s) & (MM<%(B_MassMax)s)"\
        "&(VFASPF(VCHI2/VDOF)< %(B_VCHI2DOF)s) & (BPVDIRA> %(B_DIRA)s)"\
        "&(MINTREE(((ABSID=='D+')|(ABSID=='D0')|(ABSID=='Lambda_c+')|(ABSID=='Omega_c0')|(ABSID=='Xi_c+'))"\
        ", VFASPF(VZ))-VFASPF(VZ) > %(B_D_DZ)s  ) " %CONFIG
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
    
    if CONFIG["Monitor"] == True :
        _B.Preambulo    = [
            "hdm1 = Gaudi.Histo1DDef ( 'D_M' , 1700 , 2800  , 200 )"
            ]
        _B.Monitor      = True  
        _B.MotherMonitor  = "process ( monitor ( CHILD(M,1) , hdm1 , 'D_M' ) )  >> ~EMPTY """                                                                   
    
    BSel = Selection ("BSelFor"+name+module_name,
                      Algorithm = _B,
                      RequiredSelections = [MUON,USED_CHARM])
    
    
    BSelTOS = TOSFilter( "BSelFor"+name+module_name+"TOS"
                         ,BSel
                         ,CONFIG["TTSpecs"])
    
    LINE_NAME = module_name + "_"+name
    _prescale = 1.0
    if LINE_NAME in CONFIG["prescales"].keys():
        _prescale = CONFIG["prescales"][LINE_NAME]
    main_line = StrippingLine(LINE_NAME, 
                              selection = BSelTOS,
                              HLT1 = CONFIG["HLT1"],
                              HLT2 = CONFIG["HLT2"],
                              FILTER=DEFAULT_GECs,
                              prescale = _prescale)

    if FAKE_MUON == None:
        return main_line
    else:
        BSelFake = Selection ("BSelFakeFor"+name+module_name,
                              Algorithm = _B,
                              RequiredSelections = [FAKE_MUON,USED_CHARM])
        
        BSelFakeTOS = TOSFilter( "BSelFakeFor"+name+module_name+"TOS"
                                 ,BSelFake
                                 ,CONFIG["TTSpecs"])
        return {"RealMuon":main_line,
                "FakeMuon":StrippingLine(LINE_NAME + '_FakeMuon',
                                         selection = BSelFakeTOS,
                                         HLT1 = CONFIG["HLT1"],
                                         HLT2 = CONFIG["HLT2"],
                                         FILTER=DEFAULT_GECs,
                                         prescale = CONFIG["prescaleFakes"])
                }


########### HELP WITH MAKING A DSTAR ########################
def makeDstar(_name, inputD0,CONFIG) : 
    _softPi = DataOnDemand(Location = 'Phys/StdAllLoosePions/Particles')
    _inputD0_conj = Selection("SelConjugateD0For"+_name,
                             Algorithm = ConjugateNeutralPID('ConjugateD0For'+_name),
                             RequiredSelections = [inputD0])
    _cutsSoftPi = '( PT > %(Dstar_SoftPion_PT)s  )' % CONFIG
    _cutsDstarComb = '(AM - ACHILD(M,1) + 5 > %(Dstar_wideDMCutLower)s ) & (AM - ACHILD(M,1) - 5 < %(Dstar_wideDMCutUpper)s )' % CONFIG
    _cutsDstarMoth_base = '(VFASPF(VCHI2/VDOF) < %(Dstar_Chi2)s )' % CONFIG
    _cutsDstarMoth_DM = '(M - CHILD(M,1) > %(Dstar_wideDMCutLower)s ) & (M - CHILD(M,1) < %(Dstar_wideDMCutUpper)s )' % CONFIG
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

