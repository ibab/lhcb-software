# $Id: StrippingDstarD02KMuNu.py,v 1.0 2011-02-17 12:53:17 gmcgrego Exp $
'''
The format of this stripping line is borrowed very heavily from the StrippingDstarD02KMuNu
line, by Grant McGregor.
Stripping of three channels: D*->D0(PiMuNu)pi, D+->K*0(K+pi-)MuNu and D+->Rho(pi+pi-)MuNu for the determination of Vub.
'''

__author__ = ['Patrick Owen']
__date__ = '8/12/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('CharmForVubConf',
           'makeMuons',
	   'makeKaonsFromB',
	   'makePions',
	   'makeKstar',
           'makeDplus2KstarMuNu',
           'makeDplus2RhoMuNu',
           'makeBtoDplusKstarMuNu',
           'makeBtoDplusRhoMuNu',
           'makeD02PiMuNu',
           'makeBtoD',
           'makeDstarD02PiMuNu')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseMuons, StdLooseKaons, StdAllLoosePions, StdLoosePions

default_config = {'TRACK_Chi2' : 3.,
    'TRACK_MINIPCHI2' : 6,
    'KAON_PIDK' : 3.,
    'KAON_MinPT' : 600.,
    'Kstar_MinPT' : 600.,
    'Kstar_MassMax' : 1020.,
    'Kstar_MassMin' : 770.,
    'Rho_MinPT' : 600.,
    'Rho_MassMax' : 990,
    'Rho_MassMin' : 550.,
    'MUON_MinPT' : 800.,
    'MUON_PIDmu' : 0,
    'PION_MinPT' : 600.,
    'PION_PIDmu' : -1.,
    'PION_PIDK' : 0,
    'PAIR_SumPTMin' : 2800.,
    'D_MassMax' : 1950.,
    'D_DIRA' : 0.9994,
    'D_DOCA' : 0.07,
    'D_MinP' : 20000.,
    'D0_FD'  :  4.0,
    'Dplus_FD'  :  4.0,
    'D_MCORR_MIN'  : 1400.,
    'D_MCORR_MAX'  : 2700.,
    'D_BPVVDZ' : 0.,
    'D_VtxChi2' : 9.,
    'BtoD_DeltaMass_MIN' : 2700.,
    'BtoD_DeltaMass_MAX' : 3700.,
    'B_DIRA' : 0.9994,
    'B_FDCHI2' : 100,
    'Dstar_MassMin' : 300.,
    'Dstar_DOCA' : 0.4,
    'Dstar_VtxChi2' : 9.,
    'Dstar_DeltaMass' : 200.,
    'HLT_FILTER' : "(HLT_PASS_RE('Hlt2CharmSemilepD02HMuNu_.*Decision') | HLT_PASS_RE('Hlt2Topo.*Decision'))",
    'HLT_FILTER_PiMuNu' : "HLT_PASS_RE('Hlt2CharmSemilepD02HMuNu_.*Decision')",
    #Pre and postscale
    'PrescaledKstarLinePrescale' : 1.0,
    'PrescaledRhoLinePrescale' : 1.0,
    'PrescaledPiLinePrescale' : 1.0
                              }


default_name = "CharmForVub"

class CharmForVubConf(LineBuilder) :
    """
    Builder of pre-scaled D*->D0(PiMuNu)pi, B->(D+->K*0(K+pi-)MuNu)+(3)track and B->(D+->Rho(pi+pi-)MuNu+(3)track stripping Selection and StrippingLine.

    Exports as instance data members:
    selKaons                   : Kaons for D+->K*0(K+pi-)MuNu
    selMuons                   : Muons for D*->D0(PiMuNu)pi, D+->K*0(K+pi-)MuNu and D+->Rho(pi+pi-)MuNu
    selPions                   : Hard pions from D0/D+ 
    selSlowPions               : Soft pions from D*+
    selKstar                   : K*0 for D+->K*0MuNu
    selRho                     : Rho0 for D+->RhoMuNu
    selDplus2KstarMuNu         : D+->K*Mu+Nu  selection object
    selDplus2RhoMuNu           : D+->RhoMu+Nu  selection object
    selD02PiMuNu               : D0->PiMu+Nu  selection object
    selDstarD02PiMuNu          : D*+->D0(PiMuNu)pi+ selection object
    selBtoDplus2RhoMuNu        : B->(D+->RhoMu+Nu)+tracks  selection object
    selBtoDplusD02PiMuNu       : B->(D+->D+->K*Mu+Nu)+tracks selection object
    prescaled_KstarMuNu        : StrippingLine made out of selBtoDplusKstarMuNu
    prescaled_RhoMuNu          : StrippingLine made out of selBtoDplusRhoMuNu 
    prescaled_PiMuNu           : StrippingLine made out of selDstarD02PiMuNu 
    lines                      : List of lines

    Exports as class data member:
    CharmForVub.__configuration_keys__ : List of required configuration parameters.
    """
    __configuration_keys__ = ('TRACK_Chi2',
                              'TRACK_MINIPCHI2',
                              'KAON_PIDK',
                              'KAON_MinPT',
                              'Kstar_MinPT',
                              'Kstar_MassMax',
                              'Kstar_MassMin',
                              'Rho_MinPT',
                              'Rho_MassMax',
                              'Rho_MassMin',
                              'MUON_MinPT',
                              'MUON_PIDmu',
			      'PION_MinPT',
			      'PION_PIDmu',
                              'PION_PIDK',
                              'PAIR_SumPTMin',
			      'D_MassMax',
                              'D_DIRA',
			      'D_DOCA',
			      'D_MinP',
			      'D0_FD',
			      'Dplus_FD',
			      'D_MCORR_MIN',
			      'D_MCORR_MAX',
			      'D_BPVVDZ',
			      'D_VtxChi2',
                              'BtoD_DeltaMass_MIN',
                              'BtoD_DeltaMass_MAX',
                              'B_DIRA',
                              'B_FDCHI2',
			      'Dstar_MassMin',
			      'Dstar_DOCA',
			      'Dstar_VtxChi2',
			      'Dstar_DeltaMass',
                              'HLT_FILTER',
                              'HLT_FILTER_PiMuNu',
                              'PrescaledKstarLinePrescale',
                              'PrescaledRhoLinePrescale',
                              'PrescaledPiLinePrescale'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        prescaled_name = name+'Prescaled'

        self.selMuons = makeMuons( 'MuonsFor'+prescaled_name, 
                                     TRACK_Chi2 = config['TRACK_Chi2'],
				     MUON_MinPT = config['MUON_MinPT'],
                                     MUON_PIDmu = config['MUON_PIDmu'])

        self.selMuonsFromB = makeMuonsFromB( 'MuonsFromBFor'+prescaled_name,
                                     muonSel = self.selMuons, 
                                     TRACK_MINIPCHI2 = config['TRACK_MINIPCHI2'])

        self.selKaonsFromB = makeKaonsFromB( 'KaonsFromBFor'+prescaled_name, 
				     TRACK_MINIPCHI2 = config['TRACK_MINIPCHI2'],
                                     TRACK_Chi2 = config['TRACK_Chi2'],
				     KAON_PIDK = config['KAON_PIDK'], 
				     KAON_MinPT = config['KAON_MinPT'])

        self.selPions = makePions( 'PionsFor'+prescaled_name, 
                                     TRACK_Chi2 = config['TRACK_Chi2'],
				     PION_PIDmu = config['PION_PIDmu'],
                                     PION_PIDK = config['PION_PIDK'],
                                     PION_MinPT = config['PION_MinPT'])

        self.selPionsFromB = makePionsFromB( 'PionsFromBFor'+prescaled_name,
                                     pionSel = self.selPions,
                                     TRACK_MINIPCHI2 = config['TRACK_MINIPCHI2'])

        self.selSlowPions = makeSlowPions( 'SlowPionsFor'+prescaled_name, 
                                     TRACK_Chi2 = config['TRACK_Chi2'],
				     PION_PIDmu = config['PION_PIDmu'])

        self.selKstar = makeKstar( 'KstarFor'+prescaled_name, 
                                     pionFromBSel = self.selPionsFromB,
                                     kaonFromBSel = self.selKaonsFromB,
				     Kstar_MinPT = config['Kstar_MinPT'],
				     Kstar_MassMax = config['Kstar_MassMax'],
				     Kstar_MassMin = config['Kstar_MassMin'])

        self.selRho = makeRho( 'RhoFor'+prescaled_name, 
                                     pionFromBSel = self.selPionsFromB,
				     Rho_MinPT = config['Rho_MinPT'],
				     Rho_MassMax = config['Rho_MassMax'],
				     Rho_MassMin = config['Rho_MassMin'])


        self.selDplus2KstarMuNu = makeDplus2KstarMuNu('Dplus2KstarMuNuFor'+prescaled_name,  
                                            muonFromBSel = self.selMuonsFromB, 
                                            KstarSel = self.selKstar,
					    PAIR_SumPTMin = config['PAIR_SumPTMin'],
                                            D_MassMax = config['D_MassMax'],
                                            D_DIRA = config['D_DIRA'],
                                            D_DOCA = config['D_DOCA'],
                                            D_MinP = config['D_MinP'],
					    Dplus_FD = config['Dplus_FD'],
					    D_MCORR_MIN = config['D_MCORR_MIN'],
					    D_MCORR_MAX = config['D_MCORR_MAX'],
                                            D_BPVVDZ = config['D_BPVVDZ'],
                                            D_VtxChi2 = config['D_VtxChi2'])

        self.selDplus2RhoMuNu = makeDplus2RhoMuNu('Dplus2RhoMuNuFor'+prescaled_name,  
                                            muonFromBSel = self.selMuonsFromB, 
                                            RhoSel = self.selRho,
					    PAIR_SumPTMin = config['PAIR_SumPTMin'],
                                            D_MassMax = config['D_MassMax'],
                                            D_DIRA = config['D_DIRA'],
                                            D_DOCA = config['D_DOCA'],
                                            D_MinP = config['D_MinP'],
					    Dplus_FD = config['Dplus_FD'],
					    D_MCORR_MIN = config['D_MCORR_MIN'],
					    D_MCORR_MAX = config['D_MCORR_MAX'],
                                            D_BPVVDZ = config['D_BPVVDZ'],
                                            D_VtxChi2 = config['D_VtxChi2'])

        self.selBtoDplusRhoMuNu = makeBtoDplusRhoMuNu('BtoDplusRhoMuNu'+prescaled_name,  
                                            Dplus2RhoMuNuSel = self.selDplus2RhoMuNu, 
                                            PionsFromBSel = self.selPionsFromB,
                                            BtoD_DeltaMass_MIN = config['BtoD_DeltaMass_MIN'],
                                            BtoD_DeltaMass_MAX = config['BtoD_DeltaMass_MAX'],
                                            B_DIRA = config['B_DIRA'],
                                            B_FDCHI2 = config['B_FDCHI2'])

        self.selBtoDplusKstarMuNu = makeBtoDplusKstarMuNu('BtoDplusKstarMuNu'+prescaled_name,  
                                            Dplus2KstarMuNuSel = self.selDplus2KstarMuNu,
                                            PionsFromBSel = self.selPionsFromB,
                                            BtoD_DeltaMass_MIN = config['BtoD_DeltaMass_MIN'],
                                            BtoD_DeltaMass_MAX = config['BtoD_DeltaMass_MAX'],
                                            B_DIRA = config['B_DIRA'],
                                            B_FDCHI2 = config['B_FDCHI2'])

        self.selD02PiMuNu = makeD02PiMuNu('D0PiMuNuFor'+prescaled_name,  
                                            muonSel = self.selMuons, 
                                            pionSel = self.selPions,
					    PAIR_SumPTMin = config['PAIR_SumPTMin'],
                                            D_MassMax = config['D_MassMax'],
                                            D_DIRA = config['D_DIRA'],
                                            D_DOCA = config['D_DOCA'],
                                            D_MinP = config['D_MinP'],
                                            D0_FD = config['D0_FD'],
				            D_MCORR_MIN = config['D_MCORR_MIN'],
					    D_MCORR_MAX = config['D_MCORR_MAX'],
                                            D_BPVVDZ = config['D_BPVVDZ'],
                                            D_VtxChi2 = config['D_VtxChi2'])

        self.selDstarD02PiMuNu = makeDstarD02PiMuNu('DstarD0PiMuNuFor'+prescaled_name,  
                                            D02PiMuNuSel = self.selD02PiMuNu,
					    slowPionSel = self.selSlowPions,
                                            Dstar_MassMin = config['Dstar_MassMin'],
                                            Dstar_DOCA = config['Dstar_DOCA'],
                                            Dstar_VtxChi2 = config['Dstar_VtxChi2'],
                                            Dstar_DeltaMass = config['Dstar_DeltaMass'])



        self.prescaled_lineRhoMuNu = StrippingLine(prescaled_name+"RhoMuNuLine",
		prescale = config['PrescaledRhoLinePrescale'],
                HLT=config['HLT_FILTER'],
                selection = self.selBtoDplusRhoMuNu
		)
        self.prescaled_lineKstarMuNu = StrippingLine(prescaled_name+"KstarMuNuLine",
		prescale = config['PrescaledKstarLinePrescale'],
                HLT=config['HLT_FILTER'],
                selection = self.selBtoDplusKstarMuNu
		)
        self.prescaled_linePiMuNu = StrippingLine(prescaled_name+"PiMuNuLine",
                prescale = config['PrescaledPiLinePrescale'],
                HLT=config['HLT_FILTER_PiMuNu'],
                selection = self.selDstarD02PiMuNu
                )
        self.registerLine(self.prescaled_lineRhoMuNu)
        self.registerLine(self.prescaled_lineKstarMuNu)
        self.registerLine(self.prescaled_linePiMuNu)


def makeMuons(name, 
              TRACK_Chi2, 
              MUON_MinPT,
              MUON_PIDmu) :
    """
    Create muons for all channels
    """
    _code = " ( 'mu+'  == ABSID ) & ISMUON & (TRCHI2DOF < %(TRACK_Chi2)s ) &  (PT > %(MUON_MinPT)s *MeV) & (PIDmu > %(MUON_PIDmu)s )" % locals()
    _muonFilter = FilterDesktop (Code = _code ) 
    return Selection (name,
	              Algorithm = _muonFilter,
		      RequiredSelections = [StdLooseMuons]) 

def makeMuonsFromB(name,
              muonSel, 
              TRACK_MINIPCHI2) :
    """
    Create muons for all channels
    """
    _code = "(MIPCHI2DV(PRIMARY) > %(TRACK_MINIPCHI2)s )" % locals()
    _muonFilter = FilterDesktop (Code = _code ) 
    return Selection (name,
	              Algorithm = _muonFilter,
		      RequiredSelections = [muonSel]) 


def makeKaonsFromB(name,
          TRACK_MINIPCHI2,
          TRACK_Chi2, 
	  KAON_PIDK,
	  KAON_MinPT) :
    """
    Create kaons for Dplus->K*MuNu
    """
    _code = " ( 'K+'  == ABSID ) & (TRCHI2DOF < %(TRACK_Chi2)s ) &  "\
    "( %(KAON_PIDK)s < PIDK - PIDpi) & (PT > %(KAON_MinPT)s *MeV) & (MIPCHI2DV(PRIMARY) > %(TRACK_MINIPCHI2)s )" % locals()
    _kaonFilter = FilterDesktop (Code = _code ) 
    return Selection (name,
	              Algorithm = _kaonFilter,
		      RequiredSelections = [StdLooseKaons]) 

def makePions(name, 
          TRACK_Chi2, 
	  PION_PIDmu,
          PION_PIDK,
          PION_MinPT) :
    """
    Create hard pions for all channels
    """
    _code = "(TRCHI2DOF < %(TRACK_Chi2)s ) & (PT > %(PION_MinPT)s ) & (PIDK < %(PION_PIDK)s ) & ( %(PION_PIDmu)s < PIDpi - PIDmu) " % locals()
    _pionFilter = FilterDesktop (Code = _code ) 
    return Selection (name,
	              Algorithm = _pionFilter,
		      RequiredSelections = [StdLoosePions]) 

def makePionsFromB(name,
          pionSel, 
          TRACK_MINIPCHI2) :
    """
    Create hard pions for all channels
    """
    _code = "(MIPCHI2DV(PRIMARY) > %(TRACK_MINIPCHI2)s )" % locals()
    _pionFromBFilter = FilterDesktop (Code = _code ) 
    return Selection (name,
	              Algorithm = _pionFromBFilter,
		      RequiredSelections = [pionSel]) 

def makeSlowPions(name, 
          TRACK_Chi2, 
	  PION_PIDmu) :
    """
    Create soft pions for D*->D0 pi
    """
    _code = "(TRCHI2DOF < %(TRACK_Chi2)s ) & ( %(PION_PIDmu)s < PIDpi - PIDmu) " % locals()
    _slowPionFilter = FilterDesktop (Code = _code ) 
    return Selection (name,
	              Algorithm = _slowPionFilter,
		      RequiredSelections = [StdAllLoosePions]) 

def makeKstar(name,
                   pionFromBSel,
		   kaonFromBSel,
		   Kstar_MinPT,
		   Kstar_MassMax,
		   Kstar_MassMin) :
    """
    Create and return a Kstar selection object.
    """
    _combinationCuts = "(AM < %(Kstar_MassMax)s *MeV) "\
        "& (AM > %(Kstar_MassMin)s *MeV) " % locals()
    _motherCuts = "(PT > %(Kstar_MinPT)s *MeV)  " % locals()
    _Kstar = CombineParticles( DecayDescriptor = "[K*(892)0 -> K- pi+]cc",
                            MotherCut = _motherCuts,
                            CombinationCut = _combinationCuts)

    return Selection (name,
                      Algorithm = _Kstar,
                      RequiredSelections = [kaonFromBSel,pionFromBSel])

def makeRho(name,
                   pionFromBSel,
		   Rho_MinPT,
		   Rho_MassMax,
		   Rho_MassMin) :
    """
    Create and return a Rho selection object.
    """
    _combinationCuts = "(AM < %(Rho_MassMax)s *MeV) "\
        "& (AM > %(Rho_MassMin)s *MeV) " % locals()
    _motherCuts = "(PT > %(Rho_MinPT)s *MeV) " % locals()
    _Rho = CombineParticles( DecayDescriptor = "[rho(770)0 -> pi- pi+]cc",
                            MotherCut = _motherCuts,
                            CombinationCut = _combinationCuts)

    return Selection (name,
                      Algorithm = _Rho,
                      RequiredSelections = [pionFromBSel])

def makeDplus2KstarMuNu(name,
                   muonFromBSel,
		   KstarSel,
		   PAIR_SumPTMin,
		   D_MassMax,
                   D_DIRA,
		   D_DOCA,
		   D_MinP,
		   Dplus_FD,
		   D_MCORR_MIN,
		   D_MCORR_MAX,
		   D_BPVVDZ,
		   D_VtxChi2) :
    """
    Create and return a D+ -> K*MuNu selection object.
    """
    _combinationCuts = "(AM < %(D_MassMax)s *MeV) "\
    "& ((APT1+APT2) > %(PAIR_SumPTMin)s *MeV) "\
    "& (AMAXDOCA('') < %(D_DOCA)s *mm )" % locals()
    _motherCuts = "(P > %(D_MinP)s *MeV)  "\
    "& (BPVVD > %(Dplus_FD)s *mm) "\
    "& (in_range(%(D_MCORR_MIN)s *MeV ,BPVCORRM,%(D_MCORR_MAX)s *MeV)) "\
    "& (BPVVDZ > %(D_BPVVDZ)s *mm) "\
    "& (VFASPF(VCHI2/VDOF)<%(D_VtxChi2)s) " % locals()
    _Dplus2KstarMuNu = CombineParticles( DecayDescriptor = "[D+ -> mu+ K*(892)0]cc",
                            MotherCut = _motherCuts,
			    CombinationCut = _combinationCuts )

    return Selection (name,
                      Algorithm = _Dplus2KstarMuNu,
                      RequiredSelections = [muonFromBSel, KstarSel])

def makeBtoDplusKstarMuNu(name,
                   Dplus2KstarMuNuSel,
		   PionsFromBSel,
                   BtoD_DeltaMass_MIN,
                   BtoD_DeltaMass_MAX,
                   B_DIRA,
                   B_FDCHI2)  :
    """
    Create and return a B-> D + (3) track selection object.
    """
    _motherCuts = "(in_range(%(BtoD_DeltaMass_MIN)s *MeV ,( M - CHILD(M,1) ),%(BtoD_DeltaMass_MAX)s *MeV)) "\
                  " & (BPVDIRA > %(B_DIRA)s)"\
                  " & (BPVVDCHI2 > %(B_FDCHI2)s)" % locals()
    _BtoDplus2KstarMuNu = CombineParticles( DecayDescriptors = [
                                         " [B0 -> D+ pi-]cc" ,
					 " [B0 -> D+ pi- pi+ pi-]cc"
					 ],					 
                            MotherCut = _motherCuts)
    return Selection (name,
                      Algorithm = _BtoDplus2KstarMuNu,
                      RequiredSelections = [Dplus2KstarMuNuSel, PionsFromBSel])

def makeDplus2RhoMuNu(name,
                   muonFromBSel,
		   RhoSel,
		   PAIR_SumPTMin,
		   D_MassMax,
                   D_DIRA,
		   D_DOCA,
		   D_MinP,
		   D_MCORR_MIN,
		   D_MCORR_MAX,
		   D_BPVVDZ,
		   Dplus_FD,
		   D_VtxChi2) :
    """
    Create and return a B->(D+ -> RhoMuNu) + (3) track selection object.
    """
    _combinationCuts = "(AM < %(D_MassMax)s *MeV) "\
    "& ((APT1+APT2) > %(PAIR_SumPTMin)s *MeV) "\
    "& (AMAXDOCA('') < %(D_DOCA)s *mm )" % locals()
    _motherCuts = "(P > %(D_MinP)s *MeV)  "\
    "& (BPVVD > %(Dplus_FD)s *mm) "\
    "& (in_range(%(D_MCORR_MIN)s *MeV ,BPVCORRM,%(D_MCORR_MAX)s *MeV)) "\
    "& (BPVVDZ > %(D_BPVVDZ)s *mm) "\
    "& (VFASPF(VCHI2/VDOF)<%(D_VtxChi2)s)  " % locals()
    _Dplus2RhoMuNu = CombineParticles( DecayDescriptor = "[D+ -> mu+ rho(770)0]cc",
                            MotherCut = _motherCuts,
			    CombinationCut = _combinationCuts )

    return Selection (name,
                      Algorithm = _Dplus2RhoMuNu,
                      RequiredSelections = [muonFromBSel, RhoSel])

def makeBtoDplusRhoMuNu(name,
                   Dplus2RhoMuNuSel,
		   PionsFromBSel,
                   BtoD_DeltaMass_MIN,
                   BtoD_DeltaMass_MAX,
                   B_DIRA,
                   B_FDCHI2)  :
    """
    Create and return a B->(D+ -> KstarMuNu) + (3) track selection object.
    """
    _motherCuts = "(in_range(%(BtoD_DeltaMass_MIN)s *MeV ,( M - CHILD(M,1) ),%(BtoD_DeltaMass_MAX)s *MeV)) "\
                  " & (BPVDIRA > %(B_DIRA)s)"\
                  " & (BPVVDCHI2 > %(B_FDCHI2)s)" % locals()  
    _BtoDplusRhoMuNu = CombineParticles( DecayDescriptors = [
                                         " [B0 -> D+ pi-]cc" ,
					 " [B0 -> D+ pi- pi+ pi-]cc"
					 ],					 
                            MotherCut = _motherCuts)
    return Selection (name,
                      Algorithm = _BtoDplusRhoMuNu,
                      RequiredSelections = [Dplus2RhoMuNuSel, PionsFromBSel])



def makeD02PiMuNu(name,
                   muonSel,
		   pionSel,
		   PAIR_SumPTMin,
		   D_MassMax,
                   D_DIRA,
		   D_DOCA,
		   D_MinP,
		   D0_FD,
		   D_MCORR_MIN,
		   D_MCORR_MAX,
		   D_BPVVDZ,
		   D_VtxChi2) :
    """
    Create and return a D0 -> PiMuNu selection object.
    """
    _combinationCuts = "(AM < %(D_MassMax)s *MeV) "\
    "& ((APT1+APT2) > %(PAIR_SumPTMin)s *MeV) "\
    "& (AMAXDOCA('') < %(D_DOCA)s *mm )" % locals()
    _motherCuts = "(P > %(D_MinP)s *MeV)  "\
    "& (BPVVD > %(D0_FD)s *mm) "\
    "& (BPVDIRA > %(D_DIRA)s) "\
    "& (in_range(%(D_MCORR_MIN)s *MeV ,BPVCORRM,%(D_MCORR_MAX)s *MeV)) "\
    "& (BPVVDZ > %(D_BPVVDZ)s *mm) "\
    "& (VFASPF(VCHI2/VDOF)<%(D_VtxChi2)s) " % locals()
    _D02PiMuNu = CombineParticles( DecayDescriptor = "[D0 -> mu+ pi-]cc",
                            MotherCut = _motherCuts,
			    CombinationCut = _combinationCuts )

    return Selection (name,
                      Algorithm = _D02PiMuNu,
                      RequiredSelections = [muonSel, pionSel])

def makeDstarD02PiMuNu(name,
                   D02PiMuNuSel,
		   slowPionSel,
		   Dstar_MassMin,
		   Dstar_DOCA,
		   Dstar_VtxChi2,
		   Dstar_DeltaMass)  :
    """
    Create and return a D* -> pi D0(PiMuNu) selection object.
    """
    _combinationCuts = "(AM > %(Dstar_MassMin)s *MeV) "\
    "& (AMAXDOCA('') < %(Dstar_DOCA)s *mm)" % locals()
    _motherCuts = " (VFASPF(VCHI2/VDOF)< %(Dstar_VtxChi2)s ) & ( ( M - CHILD(M,1) ) < %(Dstar_DeltaMass)s *MeV )  " % locals()

    _DstarD02PiMuNu = CombineParticles( DecayDescriptors = [
                                         " [D*(2010)+ -> D0 pi+]cc" ,
					 " [D*(2010)- -> D0 pi-]cc"
					 ],					 
                            MotherCut = _motherCuts,
			    CombinationCut = _combinationCuts )

    return Selection (name,
                      Algorithm = _DstarD02PiMuNu,
                      RequiredSelections = [D02PiMuNuSel, slowPionSel])


