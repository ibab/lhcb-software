# $Id: StrippingBs2JpsiPhiPrescaled.py,v 1.7 2010-06-30 16:02:48 jpalac Exp $
'''
Module for construction of Bs->JpsiPhi pre-scaled lifetime unbiased 
stripping Selections and StrippingLines.
Provides functions to build Bs, Jpsi, Phi selections.
Provides class Bs2JpsiPhiPrescaledConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - Bs2JpsiPhiConf
   - makeBs2JpsiPhi
   - makePhi2KK
   - makePhi2KKLoose
   - makeJpsi2MuMuLoose
'''

__author__ = ['Greig Cowan','Juan Palacios']
__date__ = '01/10/2009'
__version__ = '$Revision: 1.7 $'

__all__ = ('Bs2JpsiPhiPrescaledConf',
           'makePhi2KK',
           'makeBs2JpsiPhi',
           'Jpsi',
           'Phi', 
           'Bs',
           'line')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig

name = "Bs2JpsiPhiPrescaled"

class Bs2JpsiPhiPrescaledConf(object) :
    """
    Builder of pre-scaled Bs->JpsiPhi stripping Selection and StrippingLine.
    Constructs Bs -> J/Psi Phi Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> bsConf = Bs2JpsiPhiPrescaledConf('PrescaledBs2JpsiPhiTest',config)
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
    Bs2JpsiPhiPrescaledConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('PhiPT',
                              'BsMassWin',
                              'BsLTFITCHI2',
                              'BsVCHI2',
                              'LinePrescale',
                              'LinePostscale'
                              )

    def __init__(self, 
                 name = name,
                 config = None) :

        checkConfig(Bs2JpsiPhiPrescaledConf.__configuration_keys__,
                    config)

        self.name = name
        self.selJpsi2MuMu = Jpsi
        self.selPhi2KK = makePhi2KK( 'PhiFor'+self.name, 
                                     PhiPT = config['PhiPT'] )
        self.selBs2JpsiPhi = makeBs2JpsiPhi(self.name,  
                                            jpsiSel = Jpsi, 
                                            phiSel = Phi,
                                            BsMassWin = config['BsMassWin'],
                                            BsLTFITCHI2 = config['BsLTFITCHI2'],
                                            BsVCHI2 = config['BsVCHI2'])
        self.line = StrippingLine(self.name+"Line",
                     prescale = config['LinePrescale'],
                     postscale = config['LinePostscale'],
                     algos = [ Bs ]
                     )

        self.lines = [self.line]

def makePhi2KK(name, PhiPT) :
    """
    Create and return a Phi -> KK Selection object.
    Starts from DataOnDemand 'Phys/StdLoosePhi2KK'.
    Arguments:
    name             : name of the Selection.
    PhiPT            : Minimum transverse momentum of Phi (MeV).
    """
    _phiFilter = FilterDesktop("_filterFor"+name)
    _phiFilter.Code = "(PT> (PhiPT)s *MeV)" % locals()
    _stdPhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK")

    return Selection (name,
                      Algorithm = _phiFilter,
                      RequiredSelections = [_stdPhi2KK])

def makeBs2JpsiPhi(name,
                   jpsiSel,
                   phiSel,
                   BsMassWin,
                   BsLTFITCHI2,
                   BsVCHI2) :
    """
    Create and return a Bs -> J/Psi (MuMu) Phi (KK) Selection object.
    Arguments:
    name        : name of the Selection.
    jpsiSel     : J/Psi -> Mu+Mu- Selection object.
    phiSel      : Phi -> K+K- Selection object.
    BsMassWin   : Bs invariant mass window around PDG mass value (MeV).
    BsLTFITCHI2 : Maximum lifetime fit Chi2.
    BsVCHI2     : Maximum Bs vertex chi2 (per degree of freedom?)
    """

    _motherCuts = "(ADMASS('B_s0')<%(BsMassWin)s *MeV) & (BPVLTFITCHI2()<%(BsLTFITCHI2)s) & (VFASPF(VCHI2)<%(BsVCHI2)s)" % locals()

    _Bs = CombineParticles( '_'+ name,
                            DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                            MotherCut = _motherCuts)

    return Selection ( name,
                       Algorithm = _Bs,
                       RequiredSelections = [phiSel, jpsiSel])


# these are here for backwards compatibility.

Jpsi = DataOnDemand(Location = "Phys/StdLooseJpsi2MuMu")
Phi = makePhi2KK( 'PhiFor'+name, PhiPT = 500 )
Bs = makeBs2JpsiPhi(name,  
                    jpsiSel = Jpsi, 
                    phiSel = Phi,
                    BsMassWin = 300.,
                    BsLTFITCHI2 = 14.,
                    BsVCHI2 = 20.
                    )

line = StrippingLine(name+"Line",
                     prescale = 1.,
                     postscale = 0.1,
                     algos = [ Bs ]
                     )
