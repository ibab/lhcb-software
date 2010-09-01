# $Id: StrippingB2JpsiXforBeta_s.py,v 1.1 2010-09-01 14:13:31 gcowan Exp $
'''
Module for construction of B->JpsiX roadmap lifetime unbiased 
stripping Selections and StrippingLine.
Provides functions to build Bs, Jpsi, Phi selections.
Provides class Bs2JpsiPhiConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - Bs2JpsiPhiConf
   - makeBs2JpsiPhi
   - makePhi2KK
'''

__author__ = ['Greig Cowan','Juan Palacios']
__date__ = '26/08/2010'
__version__ = '$Revision: 1.1 $'


# Need __all__ for ???
__all__ = ('Jpsi',
	   'Jpsi2MuMuForBetasConf',
	   'allJpsi2MuMu',
	   'JpsiForBetas',
	   'JpsiForBetasLine',
	   'Bs2JpsiPhiConf',
           'makePhi2KK',
           'makeBs2JpsiPhi',
           'Phi', 
           'Bs',
           'Bsline',
	   'Bd2JpsiKstarConf',
           'makeKstar2Kpi',
           'makeBd2JpsiKstar',
           'Kstar', 
           'Bd',
           'Bdline',
	   'Bu2JpsiKConf',
           'makeBu2JpsiK',
           'Bu',
           'Buline'
	   )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig
from GaudiKernel.SystemOfUnits import MeV

##############################################################################
## This is the common Jpsi that is used in all of the subsequent selections ##
##############################################################################
#Jpsi = DataOnDemand(Location = "Phys/StdLTUnbiasedJpsi2MuMu")
Jpsi = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu")

class Jpsi2MuMuForBetasConf(object):
    """
    Builder of Jpsi line for beta_s group. This should be prescaled before end of 2010!
    Constructs JPsi->mumu Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> jpsiConf = Jpsi2MuMuForBetasConf('JpsiTest',config)
    The lines can be used directly to build a StrippingStream object.

    line       : StrippingLine made out of Jpsi2MuMu mass constrained.
    lines              : List of lines, [line]

    Exports as class data member:
    Jpsi2MuMuForBetasConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('LinePrescale',
                              'LinePostscale'
                              )
    def __init__(self,
                 name = "Jpsi2MuMuForBetas",
                 config = None) :

        checkConfig(JpsiConf.__configuration_keys__,
                    config)

        self.name = name

        self.selJpsi2MuMu = allJpsi2MuMu(self.name)

	self.line = StrippingLine(self.name+"Line",
                     prescale = config['LinePrescale'],
                     postscale = config['LinePostscale'],
                     algos = [ self.selJpsi2MuMu ]
                     )
        
        self.lines = [self.line]

def allJpsi2MuMu(name) :
    """
    Takes all of the Jpsi2MuMu
    """
    _jpsiFilter = FilterDesktop("_filterFor"+name)
    _jpsiFilter.Code = "ALL"
    return Selection (name,   
                      Algorithm = _jpsiFilter,
                      RequiredSelections = [Jpsi])

class Bs2JpsiPhiConf(object) :
    """
    Builder of roadmap Bs->JpsiPhi stripping Selection and StrippingLine.
    Constructs Bs -> J/Psi Phi Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> bsConf = Bs2JpsiPhiConf('Bs2JpsiPhiTest',config)
    >>> bsLines = bsConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selJpsi2MuMu       : nominal Jpsi-> Mu+Mu- Selection object
    selPhi2KK          : nominal Phi -> K+K- Selection object
    selBs2JpsiPhiSel   : nominal Bs -> Jpsi(Mu+Mu-) Phi(K+K-) Selection object
    line       : StrippingLine made out of selBs2jpsiPhiSel
    lines              : List of lines, [line]

    Exports as class data member:
    Bs2JpsiPhiConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('PhiMassWin',
			      'PhiPT',
			      'PhiVtxChi2',
			      'KaonPIDK',
                              'BsMassWin',
                              'BsVtxChi2',
                              'LinePrescale',
                              'LinePostscale'
                              )

    def __init__(self, 
                 name = "Bs2JpsiPhi",
                 config = None) :

        checkConfig(Bs2JpsiPhiConf.__configuration_keys__,
                    config)

        self.name = name
	
        self.selJpsi2MuMu = Jpsi
        self.selPhi2KK = makePhi2KK( 'PhiFor'+self.name, 
				     PhiMassWin = config['PhiMassWin'],
                                     PhiPT = config['PhiPT'], 
                                     PhiVtxChi2 = config['PhiVtxChi2'], 
                                     KaonPIDK = config['KaonPIDK']
				   )
        self.selBs2JpsiPhi = makeBs2JpsiPhi(self.name,  
                                            jpsiSel = Jpsi, 
                                            phiSel = Phi,
                                            BsMassWin = config['BsMassWin'],
                                            BsVtxChi2 = config['BsVtxChi2'])
        self.line = StrippingLine(self.name+"Line",
                     prescale = config['LinePrescale'],
                     postscale = config['LinePostscale'],
                     algos = [ self.selBs2JpsiPhi ]
                     )

        self.lines = [self.line]

def makePhi2KK(name, PhiMassWin, PhiPT, PhiVtxChi2, KaonPIDK) :
    """
    Create and return a Phi -> KK Selection object.
    Starts from DataOnDemand 'Phys/StdLoosePhi2KK'.
    Arguments:
    name             : name of the Selection.
    PhiMassWin	     : Phi mass window (MeV).
    PhiPT            : Minimum transverse momentum of Phi (MeV).
    PhiVtxChi2	     : Phi vertex chi2.
    KaonPIDK	     : Minimum PID of the daughter kaon.
    """
    _phiFilter = FilterDesktop("_filterFor"+name)
    _phiFilter.Code = "  (ADMASS('phi(1020)') < %(PhiMassWin)s)" \
                      "& (PT > %(PhiPT)s)" \
                      "& (VFASPF(VCHI2) < %(PhiVtxChi2)s)" \
		      "& (MINTREE('K+'==ABSID, PIDK) > %(KaonPIDK)s)" % locals()
    _stdPhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK")

    return Selection (name,
                      Algorithm = _phiFilter,
                      RequiredSelections = [_stdPhi2KK])

def makeBs2JpsiPhi(name,
                   jpsiSel,
                   phiSel,
                   BsMassWin,
                   BsVtxChi2) :
    """
    Create and return a Bs -> J/Psi (MuMu) Phi (KK) Selection object.
    Arguments:
    name        : name of the Selection.
    jpsiSel     : J/Psi -> Mu+Mu- Selection object.
    phiSel      : Phi -> K+K- Selection object.
    BsMassWin   : Bs invariant mass window around PDG mass value (MeV).
    BsVtxChi2   : Maximum Bs vertex chi2.
    """
    _motherCuts = "VFASPF(VCHI2) < %(BsVtxChi2)s" % locals()
    _combCuts = "ADAMASS('B_s0') < %(BsMassWin)s" % locals()
    _Bs = CombineParticles( '_'+ name,
                            DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                            MotherCut = _motherCuts,
			    CombinationCut = _combCuts)
    _Bs.ReFitPVs = True
    _Bs.addTool( OfflineVertexFitter() )
    _Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    _Bs.OfflineVertexFitter.useResonanceVertex = False
    return Selection ( name,
                       Algorithm = _Bs,
                       RequiredSelections = [phiSel, jpsiSel])

#########################################
## Now repeat the above but for the Bd ##
#########################################

class Bd2JpsiKstarConf(object) :
    """
    Builder of roadmap Bd->JpsiKstar stripping Selection and StrippingLine.
    Constructs Bd -> J/Psi Kstar Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> bdConf = Bd2JpsiPhiConf('Bd2JpsiKstarTest',config)
    >>> bdLines = bdConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selJpsi2MuMu       : nominal Jpsi-> Mu+Mu- Selection object
    selKstar2Kpi       : nominal Phi -> K+pi- Selection object
    selBs2JpsiKstarSel : nominal Bd -> Jpsi(Mu+Mu-) Kstar(K+pi-) Selection object
    line       	       : StrippingLine made out of selBd2jpsiKstarSel
    lines              : List of lines, [line]

    Exports as class data member:
    Bd2JpsiKstarConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('KstarMassWin',
			      'KstarPT',
			      'KstarVtxChi2',
			      'KaonPIDKMinusPIDp',
			      'KaonPIDK',
                              'BdPT',
                              'BdMassWin',
                              'BdVtxChi2',
                              'LinePrescale',
                              'LinePostscale'
                              )

    def __init__(self, 
                 name = "Bd2JpsiKstar",
                 config = None) :

        checkConfig(Bd2JpsiKstarConf.__configuration_keys__,
                    config)

        self.name = name
	
        self.selJpsi2MuMu = Jpsi
        self.selKstar2Kpi = makeKstar2Kpi( 'KstarFor'+self.name, 
				     KstarMassWin = config['KstarMassWin'],
                                     KstarPT = config['KstarPT'], 
                                     KstarVtxChi2 = config['KstarVtxChi2'], 
                                     KaonPIDKMinusPIDp = config['KaonPIDKMinusPIDp'],
                                     KaonPIDK = config['KaonPIDK']
				   )
        self.selBd2JpsiKstar = makeBd2JpsiKstar(self.name,  
                                            jpsiSel = Jpsi, 
                                            kstarSel = Kstar,
                                            BdPT = config['BdPT'],
                                            BdMassWin = config['BdMassWin'],
                                            BdVtxChi2 = config['BdVtxChi2'])
        self.line = StrippingLine(self.name+"Line",
                     prescale = config['LinePrescale'],
                     postscale = config['LinePostscale'],
                     algos = [ self.selBd2JpsiKstar ]
                     )

        self.lines = [self.line]

def makeKstar2Kpi(name, KstarMassWin, KstarPT, KstarVtxChi2, KaonPIDKMinusPIDp, KaonPIDK) :
    """
    Create and return a Kstar -> Kpi Selection object.
    Starts from DataOnDemand 'Phys/StdLooseKstar'.
    Arguments:
    name             : name of the Selection.
    KstarMassWin     : Kstar mass window (MeV).
    KstarPT          : Minimum transverse momentum of Kstar (MeV).
    KstarVtxChi2     : Kstar vertex chi2.
    KaonPIDKMinusPIDp: Minimum PIDK-PIDp of the daughter kaon.
    KaonPIDK	     : Minimum PIDK of the daughter kaon.
    """
    _kstarFilter = FilterDesktop("_filterFor"+name)
    _kstarFilter.Code = "  (ADMASS('K*(892)0') < %(KstarMassWin)s)" \
                        "& (PT > %(KstarPT)s)" \
                        "& (VFASPF(VCHI2) < %(KstarVtxChi2)s)" \
			"& (MINTREE('K+'==ABSID, (PIDK - PIDp)) > %(KaonPIDKMinusPIDp)s)" \
                        "& (MINTREE('K+'==ABSID, PIDK) > %(KaonPIDK)s)" % locals()
    _stdLooseKstar2Kpi = DataOnDemand(Location = "Phys/StdLooseKstar2Kpi")

    return Selection (name,
                      Algorithm = _kstarFilter,
                      RequiredSelections = [_stdLooseKstar2Kpi])

def makeBd2JpsiKstar(name,
                   jpsiSel,
                   kstarSel,
                   BdPT,
                   BdMassWin,
                   BdVtxChi2) :
    """
    Create and return a Bd -> J/Psi (MuMu) Kstar (Kpi) Selection object.
    Arguments:
    name        : name of the Selection.
    jpsiSel     : J/Psi -> Mu+Mu- Selection object.
    kstarSel    : Kstar -> K+pi- Selection object.
    BdPT   	: Bd transverse momentum.
    BdMassWin   : Bd invariant mass window around PDG mass value.
    BdVtxChi2   : Maximum Bd vertex chi2.
    """
    _motherCuts = "(PT > %(BdPT)s) & (VFASPF(VCHI2) < %(BdVtxChi2)s)" % locals()
    _combCuts = "ADAMASS('B0') < %(BdMassWin)s" % locals()
    _Bd = CombineParticles( '_'+ name,
                            DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc",
                            MotherCut = _motherCuts,
			    CombinationCut = _combCuts)
    _Bd.ReFitPVs = True
    _Bd.addTool( OfflineVertexFitter() )
    _Bd.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    _Bd.OfflineVertexFitter.useResonanceVertex = False
    return Selection ( name,
                       Algorithm = _Bd,
                       RequiredSelections = [kstarSel, jpsiSel])

#########################################
## Now repeat the above but for the Bu ##
#########################################

class Bu2JpsiKConf(object) :
    """
    Builder of roadmap Bu->JpsiK stripping Selection and StrippingLine.
    Constructs Bu -> J/Psi K Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> buConf = Bu2JpsiKConf('Bu2JpsiKTest',config)
    >>> buLines = buConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selJpsi2MuMu       : nominal Jpsi-> Mu+Mu- Selection object
    selBu2JpsiKSel : nominal Bu -> Jpsi(Mu+Mu-) K Selection object
    line       	       : StrippingLine made out of selBu2jpsiKSel
    lines              : List of lines, [line]

    Exports as class data member:
    Bu2JpsiKConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('KaonPT',
			      'KaonP',
			      'KaonTRCHI2', 
			      'KaonPIDKMinusPIDp',
			      'KaonPIDK',
                              'BuMassWin',
                              'BuVtxChi2',
                              'LinePrescale',
                              'LinePostscale'
                              )

    def __init__(self, 
                 name = "Bu2JpsiK",
                 config = None) :

        checkConfig(Bu2JpsiKConf.__configuration_keys__,
                    config)

        self.name = name
	
        self.selJpsi2MuMu = Jpsi
        self.selBu2JpsiK = makeBu2JpsiK(self.name,  
                                            jpsiSel = Jpsi, 
                                            BuMassWin = config['BuMassWin'],
                                            BuVtxChi2 = config['BuVtxChi2'],
					    KaonPT = config['KaonPT'],
					    KaonP = config['KaonP'],
					    KaonPIDK = config['KaonPIDK'],
					    KaonPIDKMinusPIDp = config['KaonPIDKMinusPIDp'],
					    KaonTRCHI2 = config['KaonTRCHI2']
					)
        self.line = StrippingLine(self.name+"Line",
                     prescale = config['LinePrescale'],
                     postscale = config['LinePostscale'],
                     algos = [ self.selBu2JpsiK ]
                     )

        self.lines = [self.line]


def makeBu2JpsiK(name,
                   jpsiSel,
                   BuMassWin,
                   BuVtxChi2,
		   KaonPT,		
		   KaonP,		
		   KaonPIDK,		
		   KaonPIDKMinusPIDp,
		   KaonTRCHI2		
		) :
    """
    Create and return a Bu -> J/Psi (MuMu) K Selection object.
    Arguments:
    name        : name of the Selection.
    jpsiSel     : J/Psi -> Mu+Mu- Selection object.
    BuMassWin   : Bu invariant mass window around PDG mass value.
    BuVtxChi2   : Maximum Bu vertex chi2.
    KaonPT	: Minimum Kaon PT.
    KaonP	: Minimum Kaon P.
    KaonPIDK	: Minimum Kaon PIDK hypothesis.
    KaonPIDKMinusPIDp : PIDK - PIDp for Kaon.
    KaonTRCHI2	: Minimum Kaon track chi2.	
    """
    _stdLooseKaons = DataOnDemand(Location = "Phys/StdLooseKaons")
    _motherCuts = "VFASPF(VCHI2) < %(BuVtxChi2)s" % locals()
    _combCuts = "ADAMASS('B+') < %(BuMassWin)s" % locals()
    _daugCuts = {"K+": "  (PT > %(KaonPT)s)" \
		       "& (P  > %(KaonP)s)" \
                       "& (TRCHI2DOF < %(KaonTRCHI2)s)"\
                       "& (PIDK > %(KaonPIDK)s)"\
		       "& ((PIDK - PIDp) > %(KaonPIDKMinusPIDp)s)" % locals()}
    _Bu = CombineParticles( '_'+ name,
                            DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                            MotherCut = _motherCuts,
			    CombinationCut = _combCuts,
			    DaughtersCuts = _daugCuts)
    _Bu.ReFitPVs = True
    _Bu.addTool( OfflineVertexFitter() )
    _Bu.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    _Bu.OfflineVertexFitter.useResonanceVertex = False
    return Selection ( name,
                       Algorithm = _Bu,
                       RequiredSelections = [_stdLooseKaons, jpsiSel])

# We need this for the moment until we have a StrippingSettings package that
# can be used to hold the configuration parameters. Until this is done, the
# line below can just be imported into the definition of the stream.

name = "Jpsi2MuMuForBetas"
JpsiForBetas = allJpsi2MuMu(name)
JpsiForBetasLine = StrippingLine(name+"Line",
			prescale = 1.,
			postscale = 1.,
			algos = [ JpsiForBetas ]
			)

name = "Bs2JpsiPhi"
Phi = makePhi2KK('PhiFor'+name,
		 PhiMassWin = 20.*MeV,
		 PhiPT = 700.*MeV,
		 PhiVtxChi2 = 20.,
		 KaonPIDK = -5.
		 )

Bs = makeBs2JpsiPhi(name,  
                    jpsiSel = Jpsi, 
                    phiSel = Phi,
                    BsMassWin = 300.*MeV,
                    BsVtxChi2 = 60.
                    )

Bsline = StrippingLine(name+"Line",
                     prescale = 1.,
                     postscale = 1,
                     algos = [ Bs ]
                     )

name = "Bd2JpsiKstar"
Kstar = makeKstar2Kpi('KstarFor'+name,
		 KstarMassWin = 90.*MeV,
		 KstarPT = 1000.*MeV,
		 KstarVtxChi2 = 20.,
		 KaonPIDKMinusPIDp = -6.,
		 KaonPIDK = -5.
		 )

Bd = makeBd2JpsiKstar(name,  
                    jpsiSel = Jpsi, 
                    kstarSel = Kstar,
                    BdPT = 2000.*MeV,
                    BdMassWin = 300.*MeV,
                    BdVtxChi2 = 60.
                    )

Bdline = StrippingLine(name+"Line",
                     prescale = 1.,
                     postscale = 1,
                     algos = [ Bd ]
                     )

name = "Bu2JpsiK"
Bu = makeBu2JpsiK(name,  
                    jpsiSel = Jpsi, 
                    BuMassWin = 300.*MeV,
                    BuVtxChi2 = 36.,
		    KaonPT = 1300.*MeV,		
		    KaonP = 10000.*MeV,		
		    KaonPIDK = -5.,		
		    KaonPIDKMinusPIDp = -6.,
		    KaonTRCHI2 = 5.)

Buline = StrippingLine(name+"Line",
                     prescale = 1.,
                     postscale = 1,
                     algos = [ Bu ]
                     )
