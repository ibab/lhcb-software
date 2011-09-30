# $Id: StrippingDstarD02KMuNu.py,v 1.0 2011-02-17 12:53:17 gmcgrego Exp $
'''
The format of this stripping line is borrowed very heavily from the StrippingBs2JpsiPhiPrescaledAndDetatched
line, by Greig Cowan and Juan Palacios.
Module for construction of D*->D0(KMuNu)pi  stripping Selections and StrippingLines.
Provides class DstarD02KMuNuConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - DstarD02KMuNuConf
   - makeDstarD02KMuNuRS
   - makeDstarD02KMuNuWS
   - makeD02KMuNuRS
   - makeD02KMuNuWS
'''

__author__ = ['Grant McGregor']
__date__ = '17/02/2010'
__version__ = '$Revision: 2.0 $'

__all__ = ('DstarD02KMuNuConf',
           'makeMuons',
	   'makeKaons',
	   'makePions',
           'makeD02KMuNuWS',
           'makeD02KMuNuRS',
           'makeDstarD02KMuNuWS',
	   'makeDstarD02KMuNuRS')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseMuons, StdLooseKaons, StdAllLoosePions

default_config = {'TRACK_Chi2' : 3.,
    'KAON_PIDK' : 5.,
    'KAON_MinPT' : 600.,
    'MUON_MinPT' : 800.,
    'PION_PIDmu' : -1.,
    'PAIR_SumPTMin' : 2800.,
    'D0_MassMax' : 1950.,
    'D0_DOCA' : 0.07,
    'D0_MinP' : 20000.,
    'D0_FD'  :  4.0,
    'D0_MCORR_MIN'  : 1400.,
    'D0_MCORR_MAX'  : 2700.,
    'D0_BPVVDZ' : 0.,
    'D0_VtxChi2' : 9.,
    'Dstar_MassMin' : 800.,
    'Dstar_DOCA' : 0.4,
    'Dstar_VtxChi2' : 9.,
    'Dstar_DeltaMass' : 200.,
    #Pre and postscale
    'PrescaledLinePrescale' : 1.0,
    'PrescaledLinePostscale' : 1.0
                              }


default_name = "DstarD02KMuNu"

class DstarD02KMuNuConf(LineBuilder) :
    """
    Builder of pre-scaled D*->D(KMuNu)pi stripping Selection and StrippingLine.
    Constructs D* -> D0 Pi Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> from StrippingSelections.StrippingDstarD02KMuNu import DstarD02KMuNuConf
    >>> from StrippingSelections.StrippingDstarD02KMuNu  import default_config as DstarD02KMuNu_config
    >>> DstarD02KMuNuConf = DstarD02KMuNuConf('DstarD02KMuNu', DstarD02KMuNu_config)
    >>> DstarD02KMuNuLines = DstarD02KMuNuConf.lines()
    >>> for line in DstarD02KMuNuLines :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selKaons                   : Kaons for D0->KMuNu
    selMuons                   : Muons for D0->KMuNu
    selPions                   : Pions for D*->D0pi
    selD02KMuNuRS              : D0->K-Mu+Nu  Right sign selection object
    selD02KMuNuWS              : D0->K-Mu-Nu  Wrong sign selection object
    selDstarD02KMuNuRS         :  D* -> D0(KMuNu)pi   right sign selection object
    selDstarD02KMuNuWS         :  D* -> D0(KMuNu)pi   wrong sign selection object
    prescaled_lineRS           : StrippingLine made out of selDstarD02KMuNuRS 
    prescaled_lineWS           : StrippingLine made out of selDstarD02KMuNuWS 
    lines                      : List of lines, [prescaled_lineRS, prescaled_lineWS]

    Exports as class data member:
    DstarD02KMuNuConf.__configuration_keys__ : List of required configuration parameters.
    """
    __configuration_keys__ = ('TRACK_Chi2',
                              'KAON_PIDK',
                              'KAON_MinPT',
                              'MUON_MinPT',
			      'PION_PIDmu',
                              'PAIR_SumPTMin',
			      'D0_MassMax',
			      'D0_DOCA',
			      'D0_MinP',
			      'D0_FD',
			      'D0_MCORR_MIN',
			      'D0_MCORR_MAX',
			      'D0_BPVVDZ',
			      'D0_VtxChi2',
			      'Dstar_MassMin',
			      'Dstar_DOCA',
			      'Dstar_VtxChi2',
			      'Dstar_DeltaMass',
                              'PrescaledLinePrescale',
                              'PrescaledLinePostscale'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        prescaled_name = name+'Prescaled'

        self.selMuons = makeMuons( 'MuonsFor'+prescaled_name, 
                                     TRACK_Chi2 = config['TRACK_Chi2'],
				     MUON_MinPT = config['MUON_MinPT'] )

        self.selKaons = makeKaons( 'KaonsFor'+prescaled_name, 
                                     TRACK_Chi2 = config['TRACK_Chi2'],
				     KAON_PIDK = config['KAON_PIDK'], 
				     KAON_MinPT = config['KAON_MinPT'])

        self.selPions = makePions( 'PionsFor'+prescaled_name, 
                                     TRACK_Chi2 = config['TRACK_Chi2'],
				     PION_PIDmu = config['PION_PIDmu'])

        self.selD02KMuNuRS = makeD02KMuNuRS('D0RSFor'+prescaled_name,  
                                            muonSel = self.selMuons, 
                                            kaonSel = self.selKaons,
					    PAIR_SumPTMin = config['PAIR_SumPTMin'],
                                            D0_MassMax = config['D0_MassMax'],
                                            D0_DOCA = config['D0_DOCA'],
                                            D0_MinP = config['D0_MinP'],
					    D0_FD = config['D0_FD'],
					    D0_MCORR_MIN = config['D0_MCORR_MIN'],
					    D0_MCORR_MAX = config['D0_MCORR_MAX'],
                                            D0_BPVVDZ = config['D0_BPVVDZ'],
                                            D0_VtxChi2 = config['D0_VtxChi2'])

        self.selD02KMuNuWS = makeD02KMuNuWS('D0WSFor'+prescaled_name,  
                                            muonSel = self.selMuons, 
                                            kaonSel = self.selKaons,
					    PAIR_SumPTMin = config['PAIR_SumPTMin'],
                                            D0_MassMax = config['D0_MassMax'],
                                            D0_DOCA = config['D0_DOCA'],
                                            D0_MinP = config['D0_MinP'],
					    D0_FD = config['D0_FD'],
					    D0_MCORR_MIN = config['D0_MCORR_MIN'],
					    D0_MCORR_MAX = config['D0_MCORR_MAX'],
                                            D0_BPVVDZ = config['D0_BPVVDZ'],
                                            D0_VtxChi2 = config['D0_VtxChi2'])


        self.selDstarD02KMuNuRS = makeDstarD02KMuNuRS('DstarRSFor'+prescaled_name,  
                                            D0RSSel = self.selD02KMuNuRS,
					    pionSel = self.selPions,
                                            Dstar_MassMin = config['Dstar_MassMin'],
                                            Dstar_DOCA = config['Dstar_DOCA'],
                                            Dstar_VtxChi2 = config['Dstar_VtxChi2'],
                                            Dstar_DeltaMass = config['Dstar_DeltaMass'])

        self.selDstarD02KMuNuWS = makeDstarD02KMuNuWS('DstarWSFor'+prescaled_name,  
                                            D0WSSel = self.selD02KMuNuWS,
					    pionSel = self.selPions,
                                            Dstar_MassMin = config['Dstar_MassMin'],
                                            Dstar_DOCA = config['Dstar_DOCA'],
                                            Dstar_VtxChi2 = config['Dstar_VtxChi2'],
                                            Dstar_DeltaMass = config['Dstar_DeltaMass'])

	self.prescaled_lineRS = StrippingLine(prescaled_name+"RSLine",
		prescale = config['PrescaledLinePrescale'],
		postscale = config['PrescaledLinePostscale'],
		selection = self.selDstarD02KMuNuRS
		)
	self.prescaled_lineWS = StrippingLine(prescaled_name+"WSLine",
		prescale = config['PrescaledLinePrescale'],
		postscale = config['PrescaledLinePostscale'],
		selection = self.selDstarD02KMuNuWS
		)

        self.registerLine(self.prescaled_lineRS)
        self.registerLine(self.prescaled_lineWS)


def makeMuons(name, 
              TRACK_Chi2, 
              MUON_MinPT) :
    """
    Create muons for D0->KMuNu
    Starts from StandardParticles/StdLooseMuons
    name     : name of the Selection.
    """
    _code = " ( 'mu+'  == ABSID ) & ISMUON & (TRCHI2DOF < %(TRACK_Chi2)s ) &  (PT > %(MUON_MinPT)s *MeV)" % locals()
    _muonFilter = FilterDesktop (Code = _code ) 
    return Selection (name,
	              Algorithm = _muonFilter,
		      RequiredSelections = [StdLooseMuons]) 

def makeKaons(name, 
          TRACK_Chi2, 
	  KAON_PIDK,
	  KAON_MinPT) :
    """
    Create kaons for D0->KMuNu
    Starts from StandardParticles/StdLooseKaons
    name     : name of the Selection.
    """
    _code = " ( 'K+'  == ABSID ) & (TRCHI2DOF < %(TRACK_Chi2)s ) &  "\
    "( %(KAON_PIDK)s < PIDK - PIDpi) & (PT > %(KAON_MinPT)s *MeV) " % locals()
    _kaonFilter = FilterDesktop (Code = _code ) 
    return Selection (name,
	              Algorithm = _kaonFilter,
		      RequiredSelections = [StdLooseKaons]) 

def makePions(name, 
          TRACK_Chi2, 
	  PION_PIDmu) :
    """
    Create pions for D*->D0 pi
    Starts from StandardParticles/StdAllLoosePions
    name     : name of the Selection.
    """
    _code = "(TRCHI2DOF < %(TRACK_Chi2)s ) & ( %(PION_PIDmu)s < PIDpi - PIDmu) " % locals()
    _pionFilter = FilterDesktop (Code = _code ) 
    return Selection (name,
	              Algorithm = _pionFilter,
		      RequiredSelections = [StdAllLoosePions]) 

def makeD02KMuNuRS(name,
                   muonSel,
		   kaonSel,
		   PAIR_SumPTMin,
		   D0_MassMax,
		   D0_DOCA,
		   D0_MinP,
		   D0_FD,
		   D0_MCORR_MIN,
		   D0_MCORR_MAX,
		   D0_BPVVDZ,
		   D0_VtxChi2) :
    """
    Create and return a RS D0 -> KMuNu selection object.
    Arguments:
    name             : name of the Selection.
    """
    _combinationCuts = "(AM < %(D0_MassMax)s *MeV) "\
    "& ((APT1+APT2) > %(PAIR_SumPTMin)s *MeV) "\
    "& (AMAXDOCA('') < %(D0_DOCA)s *mm )" % locals()
    _motherCuts = "(P > %(D0_MinP)s *MeV)  "\
    "& (BPVVD > %(D0_FD)s *mm) "\
    "& (in_range(%(D0_MCORR_MIN)s *MeV ,BPVCORRM,%(D0_MCORR_MAX)s *MeV)) "\
    "& (BPVVDZ > %(D0_BPVVDZ)s *mm) "\
    "& (VFASPF(VCHI2/VDOF)<%(D0_VtxChi2)s) " % locals()
    _D02KMuNuRS = CombineParticles( DecayDescriptor = "[D0 -> mu+ K-]cc",
                            MotherCut = _motherCuts,
			    CombinationCut = _combinationCuts )

    return Selection (name,
                      Algorithm = _D02KMuNuRS,
                      RequiredSelections = [muonSel, kaonSel])

def makeD02KMuNuWS(name,
                   muonSel,
		   kaonSel,
		   PAIR_SumPTMin,
		   D0_MassMax,
		   D0_DOCA,
		   D0_MinP,
		   D0_MCORR_MIN,
		   D0_MCORR_MAX,
		   D0_BPVVDZ,
		   D0_FD,
		   D0_VtxChi2) :
    """
    Create and return a wrong sign D0 -> KMuNu selection object.
    Arguments:
    name             : name of the Selection.
    """
    _combinationCuts = "(AM < %(D0_MassMax)s *MeV) "\
    "& ((APT1+APT2) > %(PAIR_SumPTMin)s *MeV) "\
    "& (AMAXDOCA('') < %(D0_DOCA)s *mm )" % locals()
    _motherCuts = "(P > %(D0_MinP)s *MeV)  "\
    "& (BPVVD > %(D0_FD)s *mm) "\
    "& (in_range(%(D0_MCORR_MIN)s *MeV ,BPVCORRM,%(D0_MCORR_MAX)s *MeV)) "\
    "& (BPVVDZ > %(D0_BPVVDZ)s *mm) "\
    "& (VFASPF(VCHI2/VDOF)<%(D0_VtxChi2)s)  " % locals()
    _D02KMuNuWS = CombineParticles( DecayDescriptor = "[D0 -> mu+ K+]cc",
                            MotherCut = _motherCuts,
			    CombinationCut = _combinationCuts )

    return Selection (name,
                      Algorithm = _D02KMuNuWS,
                      RequiredSelections = [muonSel, kaonSel])



def makeDstarD02KMuNuRS(name,
                   D0RSSel,
		   pionSel,
		   Dstar_MassMin,
		   Dstar_DOCA,
		   Dstar_VtxChi2,
		   Dstar_DeltaMass)  :
    """
    Create and return a D* -> pi D0(KMuNu) right sign selection object.
    Arguments:
    name             : name of the Selection.
    """
    _combinationCuts = "(AM > %(Dstar_MassMin)s *MeV) "\
    "& (AMAXDOCA('') < %(Dstar_DOCA)s *mm)" % locals()
    _motherCuts = "(VFASPF(VCHI2/VDOF)<  %(Dstar_VtxChi2)s ) & ( ( M - CHILD(M,1) ) < %(Dstar_DeltaMass)s *MeV ) " % locals()

    _DstarD02KMuNuRS = CombineParticles( DecayDescriptors = [
                                         " [D*(2010)+ -> D0 pi+]cc" ,
					 " [D*(2010)- -> D0 pi-]cc"
					 ],					 
                            MotherCut = _motherCuts,
			    CombinationCut = _combinationCuts )

    return Selection (name,
                      Algorithm = _DstarD02KMuNuRS,
                      RequiredSelections = [D0RSSel, pionSel])

def makeDstarD02KMuNuWS(name,
                   D0WSSel,
		   pionSel,
		   Dstar_MassMin,
		   Dstar_DOCA,
		   Dstar_VtxChi2,
		   Dstar_DeltaMass)  :
    """
    Create and return a D* -> pi D0(KMuNu) wrong sign selection object.
    Arguments:
    name             : name of the Selection.
    """
    _combinationCuts = "(AM > %(Dstar_MassMin)s *MeV) "\
    "& (AMAXDOCA('') < %(Dstar_DOCA)s *mm)" % locals()
    _motherCuts = " (VFASPF(VCHI2/VDOF)< %(Dstar_VtxChi2)s ) & ( ( M - CHILD(M,1) ) < %(Dstar_DeltaMass)s *MeV )  " % locals()

    _DstarD02KMuNuWS = CombineParticles( DecayDescriptors = [
                                         " [D*(2010)+ -> D0 pi+]cc" ,
					 " [D*(2010)- -> D0 pi-]cc"
					 ],					 
                            MotherCut = _motherCuts,
			    CombinationCut = _combinationCuts )

    return Selection (name,
                      Algorithm = _DstarD02KMuNuWS,
                      RequiredSelections = [D0WSSel, pionSel])
