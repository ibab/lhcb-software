# $Id: StrippingBs2JpsiPhiPrescaledAndDetatched.py,v 1.1 2010-06-30 12:53:17 jpalac Exp $
'''
Module for construction of Bs->JpsiPhi pre-scaled and detatched 
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
__version__ = '$Revision: 1.1 $'

__all__ = ('Bs2JpsiPhiPrescaledAndDetatchedConf',
           'makePhi2KK',
           'makeBs2JpsiPhi')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class Bs2JpsiPhiPrescaledAndDetatchedConf(LineBuilder) :
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
    selJpsi2MuMu           : nominal Jpsi-> Mu+Mu- Selection object
    selPhi2KK              : nominal Phi -> K+K- Selection object
    selBs2JpsiPhi          : nominal Bs -> Jpsi(Mu+Mu-) Phi(K+K-) Selection object
    selBs2JpsiPhiDetatched : Bs -> Jpsi(Mu+Mu-) Phi(K+K-) with lifetime cut Selection object
    prescaled_line         : StrippingLine made out of selBs2jpsiPhi
    detatched_line         : StrippingLine made out of selBs2jpsiPhiDetatched
    lines                  : List of lines, [prescaled_line, detatched_line]

    Exports as class data member:
    Bs2JpsiPhiPrescaledAndDetatchedConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('PhiPT',
                              'BsMassWin',
                              'BsLTFITCHI2',
                              'BsVCHI2',
                              'DetatchedBsLTIME',
                              'PrescaledLinePrescale',
                              'PrescaledLinePostscale',
                              'DetatchedLinePrescale',
                              'DetatchedLinePostscale'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        prescaled_name = name+'Prescaled'
        detatched_name = name+'Detatched'
        self.selJpsi2MuMu = DataOnDemand(Location = "Phys/StdLooseJpsi2MuMu/Particles")
        self.selPhi2KK = makePhi2KK( 'PhiFor'+prescaled_name, 
                                     PhiPT = config['PhiPT'] )
        self.selBs2JpsiPhi = makeBs2JpsiPhi(prescaled_name,  
                                            jpsiSel = self.selJpsi2MuMu, 
                                            phiSel = self.selPhi2KK,
                                            BsMassWin = config['BsMassWin'],
                                            BsLTFITCHI2 = config['BsLTFITCHI2'],
                                            BsVCHI2 = config['BsVCHI2'])

        self.selBs2JpsiPhiDetatched = makeBs2JpsiPhi(detatched_name,  
                                                     jpsiSel = self.selJpsi2MuMu, 
                                                     phiSel = self.selPhi2KK,
                                                     BsMassWin = config['BsMassWin'],
                                                     BsLTFITCHI2 = config['BsLTFITCHI2'],
                                                     BsVCHI2 = config['BsVCHI2'],
                                                     BsLTIME = config['DetatchedBsLTIME']
                                                     )
        
        self.prescaled_line = StrippingLine(prescaled_name+"Line",
                                            prescale = config['PrescaledLinePrescale'],
                                            postscale = config['PrescaledLinePostscale'],
                                            selection = self.selBs2JpsiPhi
                                            )
        self.detatched_line = StrippingLine(detatched_name+"Line",
                                            prescale = config['DetatchedLinePrescale'],
                                            postscale = config['DetatchedLinePostscale'],
                                            selection =  self.selBs2JpsiPhiDetatched
                                            )

        self.registerLine(self.prescaled_line)
        self.registerLine(self.detatched_line)


def makePhi2KK(name, PhiPT) :
    """
    Create and return a Phi -> KK Selection object.
    Starts from DataOnDemand 'Phys/StdLoosePhi2KK'.
    Arguments:
    name             : name of the Selection.
    PhiPT            : Minimum transverse momentum of Phi.
    """
    _code = "(PT> %(PhiPT)s)" % locals()
    _phiFilter = FilterDesktop(Code = _code)
    _stdPhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK/Particles")

    return Selection (name,
                      Algorithm = _phiFilter,
                      RequiredSelections = [_stdPhi2KK])

def makeBs2JpsiPhi(name,
                   jpsiSel,
                   phiSel,
                   BsMassWin,
                   BsLTFITCHI2,
                   BsVCHI2,
                   BsLTIME = None
                   ) :
    """
    Create and return a Bs -> J/Psi (MuMu) Phi (KK) Selection object.
    Arguments:
    name        : name of the Selection.
    jpsiSel     : J/Psi -> Mu+Mu- Selection object.
    phiSel      : Phi -> K+K- Selection object.
    BsMassWin   : Bs invariant mass window around PDG mass value.
    BsLTFITCHI2 : Maximum lifetime fit Chi2.
    BsVCHI2     : Maximum Bs vertex chi2 (per degree of freedom?)
    BsLTIME     : Minimum lifetime (ps).
    """
    _motherCuts = "(ADMASS('B_s0')<%(BsMassWin)s) & (BPVLTFITCHI2()<%(BsLTFITCHI2)s) & (VFASPF(VCHI2)<%(BsVCHI2)s)" % locals()
    if BsLTIME != None :
        _motherCuts += "& (BPVLTIME()> %(BsLTIME)s*ps)" % locals()
    #print 'makeBs2JpsiPhi', name, 'MotherCuts:', _motherCuts
    _Bs = CombineParticles( DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                            MotherCut = _motherCuts)

    return Selection ( name,
                       Algorithm = _Bs,
                       RequiredSelections = [phiSel, jpsiSel])
