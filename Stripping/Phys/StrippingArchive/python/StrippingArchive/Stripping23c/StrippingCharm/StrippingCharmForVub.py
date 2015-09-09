# $Id: StrippingDstarD02KMuNu.py,v 1.0 2011-02-17 12:53:17 gmcgrego Exp $
'''
The format of this stripping line is borrowed very heavily from the StrippingDstarD02KMuNu
line, by Grant McGregor.
Stripping of four channels: D*->D0(KMuNu)pi, D*->D0(PiMuNu)pi, D+->K*0(K+pi-)MuNu and D+->Rho(pi+pi-)MuNu for the determination of Vub.
'''

__author__ = ['Patrick Owen']
__date__ = '8/12/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('CharmForVubConf',
           'makeMuons',
	   'makeKaonsFromB',
	   'makePions',
	   'makeKstar',
           'makeDplus2VMuNu',
           'makeBtoDplusVMuNu',
           'makeD02HMuNu',
           'makeBtoD',
           'makeDstarD02HMuNu')

from Gaudi.Configuration import *
from Configurables import SubstitutePID
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseMuons, StdLooseKaons, StdAllLoosePions, StdLoosePions

default_config = {
   'NAME'        : 'CharmForVub',
   'WGs' : ['Charm'],
   'BUILDERTYPE' : 'CharmForVubConf',
   'STREAMS' : [ 'Charm' ],
   'CONFIG' : {'TRACK_MINIPCHI2' : 6,
    'KAON_PIDK' : 3.,
    'KAON_MinPT' : 600.,
    'Kstar_MinPT' : 600.,
    'Kstar_MassMax' : 1040.,
    'Kstar_MassMin' : 750.,
    'Rho_MinPT' : 600.,
    'Rho_MassMax' : 1020,
    'Rho_MassMin' : 530.,
    'MUON_MinPT' : 800.,
    'MUON_PIDmu' : 0,
    'PION_MinPT' : 600.,
    'PION_PIDmu' : -1.,
    'PION_PIDK' : 0,
    'PAIR_SumPTMin' : 2800.,
    'D_MassMax' : 1950.,
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
    'HLT_FILTER_VMuNu' : "(HLT_PASS_RE('Hlt2CharmSemilepD02HMuNu_.*Decision') | HLT_PASS_RE('Hlt2Topo.*Decision'))",
    'HLT_FILTER_HMuNu' : "(HLT_PASS_RE('Hlt2CharmSemilepD02HMuNu_.*Decision') | HLT_PASS_RE('Hlt2CharmHadD02HHXDst_.*Decision'))",
    #Pre and postscale
    'VLinePrescale' : 1.0,
    'HLinePrescale' : 1.0}
}

default_name = "CharmForVub"

class CharmForVubConf(LineBuilder) :
    """
    Builder of pre-scaled D*->D0(HMuNu)pi, B->(D+->K*0(K+pi-)MuNu)+(3)track and B->(D+->Rho(pi+pi-)MuNu+(3)track stripping Selection and StrippingLine.

    Exports as instance data members:
    selKaons                   : Kaons for D+->K*0(K+pi-)MuNu
    selMuons                   : Muons for D*->D0(HMuNu)pi, D+->K*0(K+pi-)MuNu and D+->Rho(pi+pi-)MuNu
    selPions                   : Hard pions from D0/D+ 
    selSlowPions               : Soft pions from D*+
    selKstar                   : K*0 for D+->K*0MuNu
    selRho                     : Rho0 for D+->RhoMuNu
    selDplus2KstarMuNu         : D+->K*Mu+Nu  selection object
    selDplus2RhoMuNu           : D+->RhoMu+Nu  selection object
    selD02HMuNu               : D0->PiMu+Nu  selection object
    selDstarD02HMuNu          : D*+->D0(HMuNu)pi+ selection object
    selBtoDplus2RhoMuNu        : B->(D+->RhoMu+Nu)+tracks  selection object
    selBtoDplusD02HMuNu       : B->(D+->D+->K*Mu+Nu)+tracks selection object
    prescaled_KstarMuNu        : StrippingLine made out of selBtoDplusVMuNu
    prescaled_HMuNu           : StrippingLine made out of selDstarD02HMuNu 
    lines                      : List of lines

    Exports as class data member:
    CharmForVub.__configuration_keys__ : List of required configuration parameters.
    """
    __configuration_keys__ = (
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
                              'HLT_FILTER_VMuNu',
                              'HLT_FILTER_HMuNu',
                              'VLinePrescale',
                              'HLinePrescale'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        prescaled_name = name

        self.selMuons = makeMuons( 'MuonsFor'+prescaled_name, 
				     MUON_MinPT = config['MUON_MinPT'],
                                     MUON_PIDmu = config['MUON_PIDmu'])

        self.selMuonsFromB = makeMuonsFromB( 'MuonsFromBFor'+prescaled_name,
                                     muonSel = self.selMuons, 
                                     TRACK_MINIPCHI2 = config['TRACK_MINIPCHI2'])



        self.selKaons = makeKaons( 'KaonsFor'+prescaled_name, 
				     KAON_PIDK = config['KAON_PIDK'], 
				     KAON_MinPT = config['KAON_MinPT'])

        self.selKaonsFromB = makeKaonsFromB( 'KaonsFromBFor'+prescaled_name,
                                     kaonSel = self.selKaons,
                                     TRACK_MINIPCHI2 = config['TRACK_MINIPCHI2'])

        self.selPions = makePions( 'PionsFor'+prescaled_name, 
				     PION_PIDmu = config['PION_PIDmu'],
                                     PION_PIDK = config['PION_PIDK'],
                                     PION_MinPT = config['PION_MinPT'])

        self.selPionsFromB = makePionsFromB( 'PionsFromBFor'+prescaled_name,
                                     pionSel = self.selPions,
                                     TRACK_MINIPCHI2 = config['TRACK_MINIPCHI2'])

        self.selSlowPions = makeSlowPions( 'SlowPionsFor'+prescaled_name, 
				     PION_PIDmu = config['PION_PIDmu'])


        self.selRho = makeRho( 'RhoFor'+prescaled_name, 
                                     pionFromBSel = self.selPionsFromB,
				     Rho_MinPT = config['Rho_MinPT'],
				     Rho_MassMax = config['Rho_MassMax'],
				     Rho_MassMin = config['Rho_MassMin'])

        self.selKstar = makeKstar( 'KstarFor'+prescaled_name, 
                                     pionFromBSel = self.selPionsFromB,
                                     kaonFromBSel = self.selKaonsFromB,
				     Kstar_MinPT = config['Kstar_MinPT'],
				     Kstar_MassMax = config['Kstar_MassMax'],
				     Kstar_MassMin = config['Kstar_MassMin'])



        self.selDplus2VMuNu = makeDplus2VMuNu('Dplus2VMuNuFor'+prescaled_name,  
                                            muonFromBSel = self.selMuonsFromB, 
                                            RhoSel = self.selRho,
                                            KstarSel = self.selKstar,
					    PAIR_SumPTMin = config['PAIR_SumPTMin'],
                                            D_MassMax = config['D_MassMax'],
                                            D_DOCA = config['D_DOCA'],
                                            D_MinP = config['D_MinP'],
					    Dplus_FD = config['Dplus_FD'],
					    D_MCORR_MIN = config['D_MCORR_MIN'],
					    D_MCORR_MAX = config['D_MCORR_MAX'],
                                            D_BPVVDZ = config['D_BPVVDZ'],
                                            D_VtxChi2 = config['D_VtxChi2'])

        self.selBtoDplusVMuNu = makeBtoDplusVMuNu('BtoDplusVMuNu'+prescaled_name,  
                                            Dplus2VMuNuSel = self.selDplus2VMuNu, 
                                            PionsFromBSel = self.selPionsFromB,
                                            BtoD_DeltaMass_MIN = config['BtoD_DeltaMass_MIN'],
                                            BtoD_DeltaMass_MAX = config['BtoD_DeltaMass_MAX'],
                                            B_DIRA = config['B_DIRA'],
                                            B_FDCHI2 = config['B_FDCHI2'])



        self.selD02HMuNu = makeD02HMuNu('D0HMuNuFor'+prescaled_name,  
                                            muonSel = self.selMuons, 
                                            pionSel = self.selPions,
                                            kaonSel = self.selKaons,
					    PAIR_SumPTMin = config['PAIR_SumPTMin'],
                                            D_MassMax = config['D_MassMax'],
                                            D_DOCA = config['D_DOCA'],
                                            D_MinP = config['D_MinP'],
                                            D0_FD = config['D0_FD'],
				            D_MCORR_MIN = config['D_MCORR_MIN'],
					    D_MCORR_MAX = config['D_MCORR_MAX'],
                                            D_BPVVDZ = config['D_BPVVDZ'],
                                            D_VtxChi2 = config['D_VtxChi2'])

        self.selDstarD02HMuNu = makeDstarD02HMuNu('DstarD0HMuNuFor'+prescaled_name,  
                                            D02HMuNuSel = self.selD02HMuNu,
					    slowPionSel = self.selSlowPions,
                                            Dstar_MassMin = config['Dstar_MassMin'],
                                            Dstar_DOCA = config['Dstar_DOCA'],
                                            Dstar_VtxChi2 = config['Dstar_VtxChi2'],
                                            Dstar_DeltaMass = config['Dstar_DeltaMass'])



        self.prescaled_lineVMuNu = StrippingLine(prescaled_name+"VMuNuLine",
		prescale = config['VLinePrescale'],
                HLT2=config['HLT_FILTER_VMuNu'],
                selection = self.selBtoDplusVMuNu
		)
        self.prescaled_lineHMuNu = StrippingLine(prescaled_name+"HMuNuLine",
                prescale = config['HLinePrescale'],
                HLT2=config['HLT_FILTER_HMuNu'],
                selection = self.selDstarD02HMuNu
                )
        self.registerLine(self.prescaled_lineVMuNu)
        self.registerLine(self.prescaled_lineHMuNu)


def makeMuons(name, 
              MUON_MinPT,
              MUON_PIDmu) :
    """
    Create muons for all channels
    """
    _code = " ( 'mu+'  == ABSID ) & ISMUON & (PT > %(MUON_MinPT)s *MeV) & (PIDmu > %(MUON_PIDmu)s )" % locals()
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


def makeKaons(name,
	  KAON_PIDK,
	  KAON_MinPT) :
    """
    Create kaons for Dplus->K*MuNu
    """
    _code = " ( 'K+'  == ABSID ) &  "\
    "( %(KAON_PIDK)s < PIDK - PIDpi) & (PT > %(KAON_MinPT)s *MeV)" % locals()
    _kaonFilter = FilterDesktop (Code = _code ) 
    return Selection (name,
	              Algorithm = _kaonFilter,
		      RequiredSelections = [StdLooseKaons]) 

def makeKaonsFromB(name,
          kaonSel, 
          TRACK_MINIPCHI2) :
    """
    Create hard kaons for all channels.
    """
    _code = "(MIPCHI2DV(PRIMARY) > %(TRACK_MINIPCHI2)s )" % locals()
    _kaonFromBFilter = FilterDesktop (Code = _code ) 
    return Selection (name,
	              Algorithm = _kaonFromBFilter,
		      RequiredSelections = [kaonSel]) 

def makePions(name, 
	  PION_PIDmu,
          PION_PIDK,
          PION_MinPT) :
    """
    Create hard pions for all channels
    """
    _code = "(PT > %(PION_MinPT)s ) & (PIDK < %(PION_PIDK)s ) & ( %(PION_PIDmu)s < PIDpi - PIDmu) " % locals()
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
	  PION_PIDmu) :
    """
    Create soft pions for D*->D0 pi
    """
    _code = "( %(PION_PIDmu)s < PIDpi - PIDmu) " % locals()
    _slowPionFilter = FilterDesktop (Code = _code ) 
    return Selection (name,
	              Algorithm = _slowPionFilter,
		      RequiredSelections = [StdAllLoosePions]) 



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
    _Rho = CombineParticles( DecayDescriptor = "rho(770)0 -> pi- pi+",
                            MotherCut = _motherCuts,
                            CombinationCut = _combinationCuts)

    return Selection (name,
                      Algorithm = _Rho,
                      RequiredSelections = [pionFromBSel])


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

def makeDplus2VMuNu(name,
                   muonFromBSel,
                   RhoSel,
		   KstarSel,
		   PAIR_SumPTMin,
		   D_MassMax,
		   D_DOCA,
		   D_MinP,
		   Dplus_FD,
		   D_MCORR_MIN,
		   D_MCORR_MAX,
		   D_BPVVDZ,
		   D_VtxChi2) :
    """
    Create and return a D+ -> VMuNu selection object.
    """
    
    hadrons = MergedSelection("MergedVectorHadrons"+name,
                                RequiredSelections = [KstarSel,RhoSel])


    _combinationCuts = "(AM < %(D_MassMax)s *MeV) "\
    "& ((APT1+APT2) > %(PAIR_SumPTMin)s *MeV) "\
    "& (AMAXDOCA('') < %(D_DOCA)s *mm )" % locals()
    _motherCuts = "(P > %(D_MinP)s *MeV)  "\
    "& (BPVVD > %(Dplus_FD)s *mm) "\
    "& (in_range(%(D_MCORR_MIN)s *MeV ,BPVCORRM,%(D_MCORR_MAX)s *MeV)) "\
    "& (BPVVDZ > %(D_BPVVDZ)s *mm) "\
    "& (VFASPF(VCHI2/VDOF)<%(D_VtxChi2)s) " % locals()
    _Dplus2VMuNu = CombineParticles( DecayDescriptors = [
                                         "[D+ -> mu+ K*(892)0]cc",
                                         "[D+ -> mu+ rho(770)0]cc"],
                            MotherCut = _motherCuts,
			    CombinationCut = _combinationCuts )

    return Selection (name,
                      Algorithm = _Dplus2VMuNu,
                      RequiredSelections = [muonFromBSel,hadrons])

def makeBtoDplusVMuNu(name,
                   Dplus2VMuNuSel,
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
                      RequiredSelections = [Dplus2VMuNuSel, PionsFromBSel])



def makeD02HMuNu(name,
                   muonSel,
		   pionSel,
                   kaonSel,
		   PAIR_SumPTMin,
		   D_MassMax,
		   D_DOCA,
		   D_MinP,
		   D0_FD,
		   D_MCORR_MIN,
		   D_MCORR_MAX,
		   D_BPVVDZ,
		   D_VtxChi2) :

    hadrons = MergedSelection("MergedScalarHadrons"+name,
                                RequiredSelections = [pionSel,kaonSel])

    """
    Create and return a D0 -> HMuNu selection object.
    """
    _combinationCuts = "(AM < %(D_MassMax)s *MeV) "\
    "& ((APT1+APT2) > %(PAIR_SumPTMin)s *MeV) "\
    "& (AMAXDOCA('') < %(D_DOCA)s *mm )" % locals()
    _motherCuts = "(P > %(D_MinP)s *MeV)  "\
    "& (BPVVD > %(D0_FD)s *mm) "\
    "& (in_range(%(D_MCORR_MIN)s *MeV ,BPVCORRM,%(D_MCORR_MAX)s *MeV)) "\
    "& (BPVVDZ > %(D_BPVVDZ)s *mm) "\
    "& (VFASPF(VCHI2/VDOF)<%(D_VtxChi2)s) " % locals()
    _D02HMuNu = CombineParticles( DecayDescriptors = ["[D0 -> mu+ pi-]cc",
                                                      "[D0 -> mu+ pi+]cc",
                                                      "[D0 -> mu+ K+]cc",
                                                      "[D0 -> mu+ K-]cc"
                                                     ],
                            MotherCut = _motherCuts,
			    CombinationCut = _combinationCuts )

    return Selection (name,
                      Algorithm = _D02HMuNu,
                      RequiredSelections = [muonSel,hadrons])

def makeDstarD02HMuNu(name,
                   D02HMuNuSel,
		   slowPionSel,
		   Dstar_MassMin,
		   Dstar_DOCA,
		   Dstar_VtxChi2,
		   Dstar_DeltaMass)  :
    """
    Create and return a D* -> pi D0(HMuNu) selection object.
    """
    _combinationCuts = "(AM > %(Dstar_MassMin)s *MeV) "\
    "& (AMAXDOCA('') < %(Dstar_DOCA)s *mm)" % locals()
    _motherCuts = " (VFASPF(VCHI2/VDOF)< %(Dstar_VtxChi2)s ) & ( ( M - CHILD(M,1) ) < %(Dstar_DeltaMass)s *MeV )  " % locals()

    _DstarD02HMuNu = CombineParticles( DecayDescriptors = [
                                         " [D*(2010)+ -> D0 pi+]cc" ,
					 " [D*(2010)- -> D0 pi-]cc"
					 ],					 
                            MotherCut = _motherCuts,
			    CombinationCut = _combinationCuts )

    return Selection (name,
                      Algorithm = _DstarD02HMuNu,
                      RequiredSelections = [D02HMuNuSel, slowPionSel])


