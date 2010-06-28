# $Id: StrippingBs2JpsiPhi_dev.py,v 1.1 2010-06-28 15:55:47 jpalac Exp $

__author__ = ['Greig Cowan', 'Juan Palacios']
__date__ = '24/01/2010'
__version__ = '$Revision: 1.1 $'

'''
Bs->JpsiPhi lifetime unbiased stripping selection using LoKi::Hybrid and
python configurables. PV refitting is done. Implements the nominal and 
loose lifetime unbiased stripping selections.
'''

__all__ = ('StrippingBs2JpsiPhiConf',
           'makeBs2JpsiPhi',
           'makePhi2KK',
           'makePhi2KKLoose',
           'makeJpsi2MuMuLoose')

#from Gaudi.Configuration import *
#from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from StrippingConf.StrippingLine import StrippingLine
from PhysSelPython.Wrappers import Selection, DataOnDemand
from copy import copy

class StrippingBs2JpsiPhiConf(object):
    """
    Definition of nominal Bs->JpsiPhi stripping. 
    """

    __configuration__keys__ = ("MuonTRCHI2Loose",
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
                               "BsVCHI2Loose"
                               )
    
    def __init__(self,
                 config
                 ) :
        self.checkConfig(config)
        self._config_dict = copy(config)
        for key, val in config.iteritems() :
            self.__setattr__(str(key), val)

        self._jpsi2MuMu = DataOnDemand(Location = "Phys/StdLTUnbiasedJpsi2MuMu")

        self._phi2KK = makePhi2KK("Phi2KKForBs2JpsiPhi",
                                  KaonPIDK = config['KaonPIDK'],
                                  PhiMassWin = config['PhiMassWin'],
                                  PhiPT = config['PhiPT'],
                                  PhiVCHI2 = config['PhiVCHI2'])

        self._Bs2JpsiPhiSel = makeBs2JpsiPhi("SelBs2JpsiPhi",
                                             jpsiSel = self._jpsi2MuMu,
                                             phiSel = self._phi2KK,
                                             BsMassWin = config['BsMassWin'],
                                             BsVCHI2 = config['BsVCHI2'])
        
        self._nominal_line = StrippingLine('Bs2JpsiPhiLine',
                                           prescale = 1,
                                           algos = [self._Bs2JpsiPhiSel])
     	
        self._jpsi2MuMuLooseSel = makeJpsi2MuMuLoose(name = "Jpsi2MuMuForBs2JpsiPhiLoose",
                                                     MuonTRCHI2Loose = config['MuonTRCHI2Loose'],
                                                     JpsiMassWinLoose = config['JpsiMassWinLoose'],
                                                     JpsiVCHI2Loose = config['JpsiVCHI2Loose'])

        self._phi2KKLooseSel = makePhi2KKLoose(name = "Phi2KKForBs2JpsiPhiLoose",
                                               KaonTRCHI2Loose = config['KaonTRCHI2Loose'],
                                               PhiMassWinLoose = config['PhiMassWinLoose'],
                                               PhiPTLoose = config['PhiPTLoose'],
                                               PhiVCHI2Loose = config['PhiVCHI2Loose']
                                               )
        
        self._Bs2JpsiPhiLooseSel = makeBs2JpsiPhi("SelBs2JpsiPhiLoose",
                                                  jpsiSel = self._jpsi2MuMuLooseSel,
                                                  phiSel = self._phi2KKLooseSel,
                                                  BsMassWin = config['BsMassWinLoose'],
                                                  BsVCHI2 = config['BsVCHI2Loose'])
        
        self._loose_line =  StrippingLine('Bs2JpsiPhiLooseLine',
                                          prescale = 1,
                                          algos = [self._Bs2JpsiPhiLooseSel])   

    def lines(self) :
        return (self._loose_line, self._nominal_line)

    def checkConfig(self, configuration) :
        print 'StrippingBs2JpsiPhiConf: checkConfig now implemented yet'

def makeJpsi2MuMuLoose( name,
                        MuonTRCHI2Loose,
                        JpsiMassWinLoose,
                        JpsiVCHI2Loose):
    _params = locals()
    _JpsiFilter = FilterDesktop("JpsiFilterForBs2JpsiPhiLoose")
    _JpsiFilter.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2Loose)s)" \
                       "& (ADMASS('J/psi(1S)') < %(JpsiMassWinLoose)s *MeV)" \
                       "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2Loose)s)" % _params
    stdVeryLooseJpsi2MuMu = DataOnDemand(Location = "Phys/StdVeryLooseJpsi2MuMu")
    return Selection(name,
                     Algorithm = _JpsiFilter,
                     RequiredSelections = [stdVeryLooseJpsi2MuMu])

def makePhi2KKLoose( name,
                     KaonTRCHI2Loose,
                     PhiMassWinLoose,
                     PhiPTLoose,
                     PhiVCHI2Loose):
    _params = locals()
    StdLoosePhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK")
    _phiFilter = FilterDesktop("PhiFilterForBs2JpsiPhiLoose")
    _phiFilter.Code = "  (MAXTREE('K+'==ABSID, TRCHI2DOF) < %(KaonTRCHI2Loose)s)" \
                      "& (ADMASS('phi(1020)') < %(PhiMassWinLoose)s *MeV)" \
                      "& (PT > %(PhiPTLoose)s *MeV)" \
                      "& (VFASPF(VCHI2/VDOF) < %(PhiVCHI2Loose)s)" % _params

    return Selection (name,
                      Algorithm = _phiFilter,
                      RequiredSelections = [StdLoosePhi2KK])

def makePhi2KK( name,
                KaonPIDK,
                PhiMassWin,
                PhiPT,
                PhiVCHI2 ):
    _params = locals()
    StdLoosePhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK")
    _phiFilter = FilterDesktop("PhiFilterForBs2JpsiPhi")
    _phiFilter.Code = "  (MINTREE('K+'==ABSID, PIDK) > %(KaonPIDK)s)" \
                      "& (ADMASS('phi(1020)') < %(PhiMassWin)s *MeV)" \
                      "& (PT > %(PhiPT)s *MeV)" \
                      "& (VFASPF(VCHI2/VDOF) < %(PhiVCHI2)s)" % _params

    return Selection (name,
                      Algorithm = _phiFilter,
                      RequiredSelections = [StdLoosePhi2KK])

def makeBs2JpsiPhi( name,
                    jpsiSel,
                    phiSel,
                    BsMassWin,
                    BsVCHI2):
    _params = locals()
    _Bs = CombineParticles("Bs2JpsiPhi")
    _Bs.DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)"
    _Bs.CombinationCut = "ADAMASS('B_s0') < %(BsMassWin)s *MeV" % _params
    _Bs.MotherCut = "(VFASPF(VCHI2/VDOF) < %(BsVCHI2)s)" % _params
    _Bs.ReFitPVs = True
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Phi
    _Bs.addTool( OfflineVertexFitter() )
    _Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    _Bs.OfflineVertexFitter.useResonanceVertex = False

    return Selection(name,
                     Algorithm = _Bs,
                     RequiredSelections = [jpsiSel, phiSel])

