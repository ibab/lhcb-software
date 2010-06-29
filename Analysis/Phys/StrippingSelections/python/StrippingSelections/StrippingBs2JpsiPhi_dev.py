# $Id: StrippingBs2JpsiPhi_dev.py,v 1.5 2010-06-29 13:13:41 jpalac Exp $
'''
Module for construction of Bs->JpsiPhi lifetime unbiased stripping Selections and StrippingLines.
Provides functions to build Bs, Jpsi, Phi nominal and loose selections.
Provides class StrippingBs2JpsiPhiConf, which constructs the Selections and StrippingLines given
a configuration dictionary.
Exported symbols (use python help!):
   - StrippingBs2JpsiPhiConf
   - makeBs2JpsiPhi
   - makePhi2KK
   - makePhi2KKLoose
   - makeJpsi2MuMuLoose
'''

__author__ = ['Juan Palacios', 'Greig Cowan']
__date__ = '29/06/2010'
__version__ = '$Revision: 1.5 $'


__all__ = ('StrippingBs2JpsiPhiConf',
           'makeBs2JpsiPhi',
           'makePhi2KK',
           'makePhi2KKLoose',
           'makeJpsi2MuMuLoose')

from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from StrippingConf.StrippingLine import StrippingLine
from PhysSelPython.Wrappers import Selection, DataOnDemand
from copy import copy

class StrippingBs2JpsiPhiConf(object):
    """
    Builder of nominal Bs->JpsiPhi stripping Selections and StrippingLines.
    Constructs Bs -> J/Psi Phi Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> bsConf = StrippingBs2JpsiPhiConf(config)
    >>> bsLines = bsConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selJpsi2MuMu       : nominal Jpsi-> Mu+Mu- Selection object
    selPhi2KK          : nominal Phi -> K+K- Selection object
    selBs2JpsiPhiSel   : nominal Bs -> Jpsi(Mu+Mu-) Phi(K+K-) Selection object
    nominal_line       : StrippingLine made out of selBs2jpsiPhiSel
    selJpsi2MuMuLoose  : loose Jpsi-> Mu+Mu- Selection object
    selPhi2KKLoose     : loose Phi -> K+K- Selection object
    selBs2JpsiPhiLoose : loose Bs -> Jpsi(Mu+Mu-) Phi(K+K-) Selection object
    loose_line         : StrippingLine made out of selBs2JpsiPhiLoose
    lines              : List of lines, [nominal_line, loose_line]

    Exports as class data member:
    StrippingBs2JpsiPhiConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ("MuonTRCHI2Loose",
                              "JpsiMassWinLoose",
                              "JpsiVCHI2Loose",
                              "KaonTRCHI2Loose",
                              "KaonPIDK",
                              "PhiMassWin",
                              "PhiMassWinLoose",
                              "PhiPT",
                              "PhiPTLoose",
                              "PhiVCHI2",
                              "PhiVCHI2Loose",
                              "BsMassWin",
                              "BsMassWinLoose",
                              "BsVCHI2",
                              "BsVCHI2Loose",
                              "BsReFitPVs"
                              )
    
    def __init__( self, config ) :
        self.checkConfig(config)

        self.selJpsi2MuMu = DataOnDemand(Location = "Phys/StdLTUnbiasedJpsi2MuMu")

        self.selPhi2KK = makePhi2KK("Phi2KKForBs2JpsiPhi",
                                    KaonPIDK = config['KaonPIDK'],
                                    PhiMassWin = config['PhiMassWin'],
                                    PhiPT = config['PhiPT'],
                                    PhiVCHI2 = config['PhiVCHI2'])

        self.selBs2JpsiPhi = makeBs2JpsiPhi("SelBs2JpsiPhi",
                                            jpsiSel = self.selJpsi2MuMu,
                                            phiSel = self.selPhi2KK,
                                            BsMassWin = config['BsMassWin'],
                                            BsVCHI2 = config['BsVCHI2'],
                                            BsReFitPVs = config['BsReFitPVs'])
        
        self.nominal_line = StrippingLine('Bs2JpsiPhiLine',
                                          prescale = 1,
                                          algos = [self.selBs2JpsiPhi])
     	
        self.selJpsi2MuMuLoose = makeJpsi2MuMuLoose(name = "Jpsi2MuMuForBs2JpsiPhiLoose",
                                                     MuonTRCHI2Loose = config['MuonTRCHI2Loose'],
                                                     JpsiMassWinLoose = config['JpsiMassWinLoose'],
                                                     JpsiVCHI2Loose = config['JpsiVCHI2Loose'])

        self.selPhi2KKLoose = makePhi2KKLoose(name = "Phi2KKForBs2JpsiPhiLoose",
                                              KaonTRCHI2Loose = config['KaonTRCHI2Loose'],
                                              PhiMassWinLoose = config['PhiMassWinLoose'],
                                              PhiPTLoose = config['PhiPTLoose'],
                                              PhiVCHI2Loose = config['PhiVCHI2Loose']
                                              )
        
        self.selBs2JpsiPhiLoose = makeBs2JpsiPhi("SelBs2JpsiPhiLoose",
                                                 jpsiSel = self.selJpsi2MuMuLoose,
                                                 phiSel = self.selPhi2KKLoose,
                                                 BsMassWin = config['BsMassWinLoose'],
                                                 BsVCHI2 = config['BsVCHI2Loose'],
                                                 BsReFitPVs = config['BsReFitPVs'])
        
        self.loose_line =  StrippingLine('Bs2JpsiPhiLooseLine',
                                         prescale = 1,
                                         algos = [self.selBs2JpsiPhiLoose])

        self.lines = [self.loose_line, self.nominal_line]

    def checkConfig(self, configuration) :
        """
        Check that all the required configuration parameters are present
        in configuration.keys(), and that the number of keys in configuration
        are as expected.
        """
        absentKeys = []
        for key in StrippingBs2JpsiPhiConf.__configuration_keys__ :
            if key not in configuration.keys():
                absentKeys.append(key)

        if len(absentKeys) != 0 :
            raise KeyError('Keys missing in configuration: '+ str(absentKeys))

        if len(configuration.keys()) != len(StrippingBs2JpsiPhiConf.__configuration_keys__) :
            raise KeyError('Configuration has unexpected number of parameters.')



def makeJpsi2MuMuLoose( name,
                        MuonTRCHI2Loose,
                        JpsiMassWinLoose,
                        JpsiVCHI2Loose):

    """
    Create and return a Jpsi->MuMu Selection object.
    Starts from DataOnDemand 'Phys/StdVeryLooseJpsi2MuMu'.
    Arguments:
    name             : name of the Selection.
    MuonTRCHI2Loose  : Maximum muon track chi2 (per degree of freedom?)
    JpsiMassWinLoose : J/Psi invariant mass window around PDG mass value (MeV).
    JpsiVCHI2Loose   : Maximum J/Psi vertex chi2 (per degree of freedom?)
    """
    
    _params = locals()
    _code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2Loose)s)" \
        "& (ADMASS('J/psi(1S)') < %(JpsiMassWinLoose)s *MeV)" \
        "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2Loose)s)" % _params
    print 'makeJpsi2MuMuLoose Code =', _code
    _JpsiFilter = FilterDesktop("JpsiFilterForBs2JpsiPhiLoose",
                                Code = _code)

    stdVeryLooseJpsi2MuMu = DataOnDemand(Location = "Phys/StdVeryLooseJpsi2MuMu")
    return Selection(name,
                     Algorithm = _JpsiFilter,
                     RequiredSelections = [stdVeryLooseJpsi2MuMu])

def makePhi2KKLoose( name,
                     KaonTRCHI2Loose,
                     PhiMassWinLoose,
                     PhiPTLoose,
                     PhiVCHI2Loose):

    """
    Create and return a Phi -> KK Selection object.
    Starts from DataOnDemand 'Phys/StdLoosePhi2KK'.
    Arguments:
    name             : name of the Selection.
    KaonTRCHI2Loose  : Maximum kaon track chi2 (per degree of freedom?)
    PhiMassWinLoose  : Phi invariant mass window around PDG mass value (MeV).
    PhiVCHI2Loose    : Maximum Phi vertex chi2 (per degree of freedom?)
    """
    
    _params = locals()
    _code = "  (MAXTREE('K+'==ABSID, TRCHI2DOF) < %(KaonTRCHI2Loose)s)" \
        "& (ADMASS('phi(1020)') < %(PhiMassWinLoose)s *MeV)" \
        "& (PT > %(PhiPTLoose)s *MeV)" \
        "& (VFASPF(VCHI2/VDOF) < %(PhiVCHI2Loose)s)" % _params
    print 'makePhi2KKLoose Code =', _code
    StdLoosePhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK")
    _phiFilter = FilterDesktop("PhiFilterForBs2JpsiPhiLoose",
                               Code = _code)
    return Selection (name,
                      Algorithm = _phiFilter,
                      RequiredSelections = [StdLoosePhi2KK])

def makePhi2KK( name,
                KaonPIDK,
                PhiMassWin,
                PhiPT,
                PhiVCHI2 ):

    """
    Create and return a Phi -> KK Selection object.
    Starts from DataOnDemand 'Phys/StdLoosePhi2KK'.
    Arguments:
    name             : name of the Selection.
    KaonPIDK         : ??
    PhiPT            : Minimum transverse momentum of Phi (MeV).
    PhiMassWin       : Phi invariant mass window around PDG mass value (MeV).
    PhiVCHI2         : Maximum Phi vertex chi2 (per degree of freedom?)
    """

    _params = locals()
    _code = "  (MINTREE('K+'==ABSID, PIDK) > %(KaonPIDK)s)" \
        "& (ADMASS('phi(1020)') < %(PhiMassWin)s *MeV)" \
        "& (PT > %(PhiPT)s *MeV)" \
        "& (VFASPF(VCHI2/VDOF) < %(PhiVCHI2)s)" % _params
    print 'makeJpsiPhi2KK Code =', _code
    StdLoosePhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK")
    _phiFilter = FilterDesktop("PhiFilterForBs2JpsiPhi",
                               Code = _code)

    return Selection (name,
                      Algorithm = _phiFilter,
                      RequiredSelections = [StdLoosePhi2KK])

def makeBs2JpsiPhi( name,
                    jpsiSel,
                    phiSel,
                    BsMassWin,
                    BsVCHI2,
                    BsReFitPVs):

    """
    Create and return a Bs -> J/Psi (MuMu) Phi (KK) Selection object.
    Arguments:
    name      : name of the Selection.
    jpsiSel   : J/Psi -> Mu+Mu- Selection object.
    phiSel    : Phi -> K+K- Selection object,
    BsMassWin : Bs invariant mass window around PDG mass value (MeV).
    BsVCHI2   : Maximum Bs vertex chi2 (per degree of freedom?)
    """
    
    _params = locals()
    _combCut =  "ADAMASS('B_s0') < %(BsMassWin)s *MeV" % _params
    _motherCut = "(VFASPF(VCHI2/VDOF) < %(BsVCHI2)s)" % _params
    print 'makeBs2JpsiPhi CombinationCut =', _combCut
    print 'makeBs2JpsiPhi MotherCut =', _motherCut
    _Bs = CombineParticles("Bs2JpsiPhi",
                           DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                           CombinationCut = _combCut,
                           MotherCut = _motherCut,
                           ReFitPVs = BsReFitPVs)
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Phi
    _Bs.addTool( OfflineVertexFitter() )
    _Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    _Bs.OfflineVertexFitter.useResonanceVertex = False

    return Selection(name,
                     Algorithm = _Bs,
                     RequiredSelections = [jpsiSel, phiSel])

