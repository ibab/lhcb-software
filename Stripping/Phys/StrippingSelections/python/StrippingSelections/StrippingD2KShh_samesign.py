# $Id$
'''
This code looks for D0 -> KS0 h+ h+ (wrong charge) for h = K, pi
The line is a copy of the D*->D0 pi D0->KS0 h+ h- line removing several cuts

'''

__author__ = ['Denis Derkach']
__date__ = '03/01/2014'
__version__ = '$Revision: 1.0 $'

__all__ = ('D2KShhBuilder',
           'filterKaons',
           'filterPions',
           'makeKS',
           'makeD2KShh',
           'confdict')

confdict = {
    'LongTrackGEC'          :  150        ## Global event cut on number of long tracks
    ,'KSLLCutDIRA'          :    0.9997   ## unitless
    ,'KSDDCutDIRA'          :    0.99995  ## unitless
    ,'KSLLCutMass'          :   20.0      ## in MeV -- Mat changed from 11.4 to 20.0 in S20
    ,'KSDDCutMass'          :   40.0      ## in MeV -- Mat changed from 24.9 to 40.0 in S20
    ,'KSLLCutFDChi2'        :  100        ## unitless
    ,'KSDDCutFDChi2'        :  100        ## unitless
    ,'KSCutZFDFromD'        :   10.0      ## in mm -- Mat added in S20
    ,'trackFromDCutP'       : 1500.0      ## in MeV
    ,'trackFromDCutPIDe'    :   10.0      ## unitless -- note that this means (DLLe - DLLx) < 10 for x=K,pi
    ,'trackFromDCutPIDp'    :   15.0      ## unitless -- note that this means (DLLp - DLLx) < 15 for x=K,pi
    ,'pionFromDCutPIDK'     :   -1.0
    ,'kaonFromDCutPIDpi'    :   -3.0
    ,'pionFromDCutTRCHI2DOF':    4.0      ## unitless
    ,'kaonFromDCutTRCHI2DOF':    5.0      ## unitless
    ,'DCutVtxChi2_KK'       :   21.0      ## unitless
    ,'DCutVtxChi2_PP'       :   13.0      ## unitless
    ,'preFitDCutPT'         : 1500.0      ## in MeV
    ,'preFitDMassCut_LL'    :  130        ## in MeV
    ,'preFitDMassCut_DD'    :  270        ## in MeV
    ,'wideDMassCut_KKLL'    :   70.0      ## in MeV -- Mat changed from  57.2 to  70.0 in S20
    ,'wideDMassCut_PPLL'    :  120.0      ## in MeV -- Mat changed from 110.0 to 120.0 in S20
    ,'wideDMassCut_KKDD'    :   85.0      ## in MeV -- Mat changed from  72.0 to  85.0 in S20
    ,'wideDMassCut_PPDD'    :  150.0      ## in MeV -- Mat changed from 140.0 to 150.0 in S20
    ,'preFitDstarMassCut'   :  200.0      ## in MeV; make sure it's well above the largest D mass window
    ,'wideDMCutLower'       :  -10.0      ## in MeV: allow negative tail made by CombineParticles -- Mat changed from -2.0 to -10.0 in S20
    ,'wideDMCutUpper'       :   15.0      ## in MeV -- Mat changed from 15.0 to 25.0 in S20
    ,'DstarCutPT_KK'        : 2200.0      ## in MeV
    ,'DstarCutPT_PP'        : 2200.0      ## in MeV
    ,'DstarCutChi2NDOF_KK'  :   60.0      ## unitless
    ,'DstarCutChi2NDOF_PP'  :   20.0      ## unitless
    ,'SoftPionCutPIDe'      :    2.0      ## unitless -- Mat changed from 5.0 to 2.0 in S20
    ,'KKLLPrescale'         :    0.01
    ,'KKLLPostscale'        :    1.00
    ,'PPLLPrescale'         :    0.01
    ,'PPLLPostscale'        :    1.0
    ,'KKDDPrescale'         :    0.01
    ,'KKDDPostscale'        :    1.0
    ,'PPDDPrescale'         :    0.01
    ,'PPDDPostscale'        :    1.0

    }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

name = "D2KShhBuilder"

class D2KShhBuilder(LineBuilder) :
    """
    Reconstruct D0 -> KS0 h+ h+ for h = K, pi
    (i.e. KSKK, KSKpi, KSpipi)
    """
    __configuration_keys__ = ('KSLLCutDIRA',
                              'KSDDCutDIRA',
                              'KSLLCutMass',
                              'KSDDCutMass',
                              'KSLLCutFDChi2',
                              'KSDDCutFDChi2',
                              'KSCutZFDFromD',
                              'trackFromDCutP',
                              'trackFromDCutPIDe',
                              'trackFromDCutPIDp',
                              'pionFromDCutPIDK',
                              'kaonFromDCutPIDpi',
                              'pionFromDCutTRCHI2DOF',
                              'kaonFromDCutTRCHI2DOF',
                              'preFitDMassCut_LL',
                              'preFitDMassCut_DD',
                              'preFitDCutPT',
                              'DCutVtxChi2_KK',
                              'DCutVtxChi2_PP',
                              'wideDMassCut_KKLL',
                              'wideDMassCut_PPLL',
                              'wideDMassCut_KKDD',
                              'wideDMassCut_PPDD',
                              'preFitDstarMassCut',
                              'DstarCutChi2NDOF_KK',
                              'DstarCutChi2NDOF_PP',
                              'SoftPionCutPIDe',
                              'DstarCutPT_KK',
                              'DstarCutPT_PP',
                              'wideDMCutLower',
                              'wideDMCutUpper',
                              'KKLLPrescale',
                              'KKLLPostscale',
                              'PPLLPrescale',
                              'PPLLPostscale',
                              'KKDDPrescale',
                              'KKDDPostscale',
                              'PPDDPrescale',
                              'PPDDPostscale',
                              'LongTrackGEC')



    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        # Set up global event cuts.
        # Conceptually these come first, although the place where they're
        # inserted into the line is at the bottom of the code.
        _globalEventCuts = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(LongTrackGEC)s )" % config,
                            "Preambulo": ["from LoKiTracks.decorators import *"]}

        # All lines start with one of: KS->LL, KS->DD
        self.selKSLL = makeKS('KsLLFor'+name, "Phys/StdLooseKsLL/Particles", -1000.0,  650.0, config['KSLLCutDIRA'], config['KSLLCutMass'], config['KSLLCutFDChi2'])
        self.selKSDD = makeKS('KsDDFor'+name, "Phys/StdLooseKsDD/Particles",     0.0, 2300.0, config['KSDDCutDIRA'], config['KSDDCutMass'], config['KSDDCutFDChi2'])

        # Make filtered lists of daughtes
        self.selDauK = filterKaons('KFor'+name, "Phys/StdLooseKaons/Particles", config['trackFromDCutP'], config['trackFromDCutPIDe'], config['trackFromDCutPIDp'], config['kaonFromDCutPIDpi'], config['kaonFromDCutTRCHI2DOF'])
        self.selDauP = filterPions('PFor'+name, "Phys/StdLoosePions/Particles", config['trackFromDCutP'], config['trackFromDCutPIDe'], config['trackFromDCutPIDp'], config['pionFromDCutPIDK'], config['pionFromDCutTRCHI2DOF'])
        self.selDauKK = [ self.selDauK ]
        self.selDauPP = [ self.selDauP ]

        # Now we make the various flavours of D0 decay (KSK+K+, KSpi+pi+, KSK+pi+):
        strDecaysPP = [ "[D0 -> KS0 pi+ pi+]cc" ]
        strDecaysKK = [ "[D0 -> KS0 K+ K+]cc" ]
        self.selD2KSKKLL = makeD2KShh('D2KSKKLLFor'+name, strDecaysKK, self.selKSLL, self.selDauKK, config['preFitDMassCut_LL'], config['preFitDCutPT'], config['wideDMassCut_KKLL'], config['KSCutZFDFromD'])
        self.selD2KSPPLL = makeD2KShh('D2KSPPLLFor'+name, strDecaysPP, self.selKSLL, self.selDauPP, config['preFitDMassCut_LL'], config['preFitDCutPT'], config['wideDMassCut_PPLL'], config['KSCutZFDFromD'])
        self.selD2KSKKDD = makeD2KShh('D2KSKKDDFor'+name, strDecaysKK, self.selKSDD, self.selDauKK, config['preFitDMassCut_DD'], config['preFitDCutPT'], config['wideDMassCut_KKDD'], config['KSCutZFDFromD'])
        self.selD2KSPPDD = makeD2KShh('D2KSPPDDFor'+name, strDecaysPP, self.selKSDD, self.selDauPP, config['preFitDMassCut_DD'], config['preFitDCutPT'], config['wideDMassCut_PPDD'], config['KSCutZFDFromD'])

        self.lineKKLL = StrippingLine(name+'KKLLLine',
                                      prescale = config['KKLLPrescale'],
                                      postscale = config['KKLLPostscale'],
                                      FILTER = _globalEventCuts,
                                      selection = self.selD2KSKKLL)
        self.linePPLL = StrippingLine(name+'PPLLLine',
                                      prescale = config['PPLLPrescale'],
                                      postscale = config['PPLLPostscale'],
                                      FILTER = _globalEventCuts,
                                      selection = self.selD2KSPPLL)
        self.lineKKDD = StrippingLine(name+'KKDDLine',
                                      prescale = config['KKDDPrescale'],
                                      postscale = config['KKDDPostscale'],
                                      FILTER = _globalEventCuts,
                                      selection = self.selD2KSKKDD)
        self.linePPDD = StrippingLine(name+'PPDDLine',
                                      prescale = config['PPDDPrescale'],
                                      postscale = config['PPDDPostscale'],
                                      FILTER = _globalEventCuts,
                                      selection = self.selD2KSPPDD)

        self.registerLine(self.lineKKLL)
        self.registerLine(self.linePPLL)
        self.registerLine(self.lineKKDD)
        self.registerLine(self.linePPDD)


def filterKaons(name, inputName, trackFromDCutP, trackFromDCutPIDe, trackFromDCutPIDp, kaonFromDCutPIDpi, kaonFromDCutTRCHI2DOF) :
    """
    Apply cuts to charged kaon daughters of D0
    """
    _strCutP = "(P > %(trackFromDCutP)s *MeV)" % locals()
    _strCutPIDe = "(PIDe-PIDK < %(trackFromDCutPIDe)s )" % locals()
    _strCutPIDp = "(PIDp-PIDK < %(trackFromDCutPIDp)s ) " % locals()
    _strCutPIDK = "(PIDpi-PIDK < %(kaonFromDCutPIDpi)s ) " % locals()
    _strCutChi2 = "(TRCHI2DOF < %(kaonFromDCutTRCHI2DOF)s )" % locals()
    _strCuts = '(' + _strCutP + '&' + _strCutPIDe + '&' + _strCutPIDp \
        + '&' + _strCutPIDK + '&' + _strCutChi2 + ')'
    _trackFilter = FilterDesktop(Code = _strCuts)
    inputTracks = DataOnDemand(Location = inputName)
    return Selection (name,
                      Algorithm = _trackFilter,
                      RequiredSelections = [inputTracks])

def filterPions(name, inputName, trackFromDCutP, trackFromDCutPIDe, trackFromDCutPIDp, pionFromDCutPIDK, pionFromDCutTRCHI2DOF) :
    """
    Apply cuts to pion daughters of D0
    """
    _strCutP = "(P > %(trackFromDCutP)s *MeV)" % locals()
    _strCutPIDe = "(PIDe-PIDpi < %(trackFromDCutPIDe)s )" % locals()
    _strCutPIDp = "(PIDp-PIDpi < %(trackFromDCutPIDp)s ) " % locals()
    _strCutPIDK = "(PIDK -PIDpi < %(pionFromDCutPIDK)s ) " % locals()
    _strCutChi2 = "(TRCHI2DOF < %(pionFromDCutTRCHI2DOF)s )" % locals()
    _strCuts = '(' + _strCutP + '&' + _strCutPIDe + '&' + _strCutPIDp \
        + '&' + _strCutPIDK + '&' + _strCutChi2 + ')'
    _trackFilter = FilterDesktop(Code = _strCuts)
    inputTracks = DataOnDemand(Location = inputName)
    return Selection (name,
                      Algorithm = _trackFilter,
                      RequiredSelections = [inputTracks])

def makeKS(name, inputName, minDz, maxDz, KSCutDIRA, KSCutMass, KSCutFDChi2) :
    """
    Given a set of input tracks, look for KS -> pi+ pi-.
    Used for both long and downstream tracks.
    """
    _code = "(BPVVDZ > %(minDz)s * mm) & (BPVVDZ < %(maxDz)s * mm) & (BPVDIRA > %(KSCutDIRA)s ) & (ADMASS('KS0') < %(KSCutMass)s *MeV) & (BPVVDCHI2> %(KSCutFDChi2)s )" % locals()
    _KsFilter = FilterDesktop(Code = _code)
    _stdLooseKs = DataOnDemand(Location = inputName)
    return Selection (name,
                      Algorithm = _KsFilter,
                      RequiredSelections = [_stdLooseKs])

def makeD2KShh(name, inputDecayDescriptors, inputKs, inputDaughters, preFitMassCut, preFitDCutPT, postFitMassCut, KSCutZFDFromD) :
    """
    Given lists of D0 daughter tracks and a list of KS, reconstruct D0 -> KS h+ h+.
    The same routine works for all final states and for KS via long/downstream tracks.
    """

    _combCutsPT = "(APT > %(preFitDCutPT)s * MeV) & (APT > %(preFitDCutPT)s * MeV)" % locals()
    _combCutsMass = "(ADAMASS('D0') < %(preFitMassCut)s * MeV)" % locals()
    _combCuts = '( ' + _combCutsPT + ' & ' + _combCutsMass + ' )'
    _motherCutsBase = '((BPVVDZ < 7000.0*mm) )' % locals()
    _motherCutsMass = "(ADMASS('D0') < %(postFitMassCut)s * MeV)" % locals()
    _motherCutsKSFD = '( (CHILD(VFASPF(VZ),1) - VFASPF(VZ)) > %(KSCutZFDFromD)s * mm)' % locals()
    _motherCuts = '( ' + _motherCutsBase + ' & ' + _motherCutsMass + ' & ' + _motherCutsKSFD + ')'
    _D0 = CombineParticles( DecayDescriptors = inputDecayDescriptors,
                            CombinationCut = _combCuts,
                            MotherCut = _motherCuts)
    return Selection ( name,
                       Algorithm = _D0,
                       RequiredSelections = [inputKs] + inputDaughters)


