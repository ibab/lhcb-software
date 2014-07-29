__author__ = ['Mika Vesterinen']
__date__ = '23/07/2010'
__version__ = '$Revision: 4.0 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from PhysSelPython.Wrappers import AutomaticData
from StandardParticles import StdAllNoPIDsPions, StdAllLoosePions, StdLoosePions, StdLooseMuons, StdAllLooseKaons, StdLooseProtons, StdNoPIDsPions, StdNoPIDsUpPions
from Configurables import ConjugateNeutralPID, FitDecayTrees
from SelPy.utils import ( UniquelyNamedObject,
                          ClonableObject,
                          SelectionBase )
__all__ = ('TrackEffD0ToK3PiAllLinesConf',
           'TOSFilter',
           'confdict')

confdict = {
    }

class TrackEffD0ToK3PiAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config
        
        
        ####################### BASIC FINAL STATE PARTICLE SELECTIONS ##########################

        SelLongPions = Selection("SelLongPionsFor"+name,
                                 Algorithm = FilterDesktop(Code="(MIPCHI2DV(PRIMARY) > 2) & (PT > 200) & (PIDK < 10) & (TRGHOSTPROB < 0.35)"),
                                 RequiredSelections = [StdAllLoosePions])
        
        SelLongKaons = Selection("SelLongKaonsFor"+name,
                                 Algorithm = FilterDesktop(Code="(MIPCHI2DV(PRIMARY) > 2) & (PT > 200) & (PIDK > 4) & (TRGHOSTPROB < 0.35)"),
                                 RequiredSelections = [StdAllLooseKaons])

        SelVeloTracks = Selection("SelVeloTracksFor"+name,
                                  Algorithm = FilterDesktop(Code="(MIPDV(PRIMARY)> 0.05*mm)"),
                                  RequiredSelections = [AutomaticData(Location='Phys/VeloAllPions/Particles')])
        
        ####################### MAKE THE INTERMEDIATE THREE TRACK VERTEX ##########################
        
        CombKst = CombineParticles(DecayDescriptor = '[K*(892)+ -> K- pi+ pi+]cc',
                                   CombinationCut = '(APT > 1500*MeV) & (AM > 0 *MeV) & (AM < 1900*MeV)'\
                                       '& (ANUM(MIPCHI2DV(PRIMARY)>25)>=2)'\
                                       '& (ANUM(PT > 500*MeV)>=2)'\
                                       "& (ADOCACHI2CUT(15,''))",
                                   MotherCut = '(VFASPF(VZ) > 3*mm) & (VFASPF(VCHI2/VDOF)< 4.0)')

        SelKst = Selection('SelKstfor'+name,
                           Algorithm = CombKst,
                           RequiredSelections = [SelLongKaons,SelLongPions])
        
        ######################## ADD THE "PROBE" VELO TRACK #############################################
        CombD0 = CombineParticles(DecayDescriptors = ['D0 -> K*(892)+ pi+','D~0 -> K*(892)- pi+'],## both pion charges
                                  Preambulo = ['D1PHI = ACHILD(CHILD(math.atan2(PY,PX),1),1)',
                                               'D2PHI = ACHILD(CHILD(math.atan2(PY,PX),2),1)',
                                               'D3PHI = ACHILD(CHILD(math.atan2(PY,PX),3),1)',
                                               'D4PHI = ACHILD(math.atan2(PY,PX),2)',
                                               'DPHI1 = D1PHI-D4PHI',
                                               'if DPHI1 < 0: DPHI1  = DPHI1*(-1.0)',
                                               'DPHI2 = D2PHI-D4PHI',
                                               'if DPHI2 < 0: DPHI2  = DPHI2*(-1.0)',
                                               'DPHI3 = D3PHI-D4PHI',
                                               'mypi = 3.1415',
                                               'if DPHI3 < 0: DPHI3  = DPHI3*(-1.0)',
                                               'if DPHI1 > mypi: DPHI1 = 2*mypi - DPHI1',
                                               'if DPHI2 > mypi: DPHI2 = 2*mypi - DPHI2',
                                               'if DPHI3 > mypi: DPHI2 = 2*mypi - DPHI3',
                                               'from LoKiPhysMC.functions import mcMatch',
                                               "from LoKiPhysMC.decorators import *" ],
                                  CombinationCut = "(AMAXDOCA('') < 0.10 * mm )", # & (DPHI1 > 0.1) & (DPHI2 > 0.1) & (DPHI3 > 0.1)",
                                  MotherCut = '(DTF_FUN(M,True) > 1565*MeV) & (DTF_FUN(M,True) < 2165*MeV)')
        
        SelD0 = Selection('SelD0for'+name,
                          Algorithm = CombD0,
                          RequiredSelections = [SelKst,SelVeloTracks])
        
        ####################### ADD THE TAGGING SLOW PION ###################################################
        
        CombDstar = CombineParticles(DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc",
                                     DaughtersCuts = {"pi+": "(MIPCHI2DV(PRIMARY) < 9) & (PIDe < 5) & (PT > 300*MeV)"},
                                     CombinationCut = "(AALLSAMEBPV)",
                                     MotherCut = "(DTF_FUN( M ,True, 'D0') < 2100*MeV)")
        
        SelDstar = Selection("SelDstarfor"+name,
                          Algorithm = CombDstar,
                          RequiredSelections = [SelD0,StdAllLoosePions])
        
        ################## MAKE THE STRIPPING LINE ########################
        LineDstar = StrippingLine(name+'DstarLine', 
                                  HLT = "HLT_PASS_RE('Hlt2.*CharmHadD02HHXDst_hhX.*Decision')",
                                  selection = SelDstar)
        self.registerLine(LineDstar)
        


########## TISTOS FILTERING ##################################
def TOSFilter( name = None, sel = None, trigger = None ):
    if len(trigger) == 0:
        return sel
    from Configurables import TisTosParticleTagger
    _filter = TisTosParticleTagger(name+"_TriggerTos")
    _filter.TisTosSpecs = trigger
    _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = [ sel ], Algorithm = _filter )
    return _sel



