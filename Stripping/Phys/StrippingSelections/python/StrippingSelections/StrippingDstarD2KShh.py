# $Id$
'''
This code looks for D*+ -> D0 pi+, D0 -> KS0 h+ h- for h = K, pi
This has been completely rewritten in February 2011 to use the new LineBuilder framework.
The old version in the previous framework is still accessible via SVN (version r95389).
The cuts are mostly the same as the code used in late 2010, except:
 * Global event cut is now on number of long tracks instead of best tracks
 * Some PID cuts tightened (trackFromDCutPIDe, trackFromDCutPIDp, kaonFromDCutPIDpi)
 * Track fit chi2/NDF cuts tightened quite a bit, but cut on track fit probability
   removed. The probability was too tight before, so this should be a net loosening.
 * All modes share a common proper lifetime cut (but cut value is the same as before)
 * Separate container for both D0 and D0bar added for KSKK & KSPP (Nick)
'''

__author__ = ['Mat Charles']
__date__ = '24/02/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('DstarD2KShhBuilder',
           'filterKaons',
           'filterPions',
           'makeKS',
           'makeD2KShh',
           'makeDstar')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

name = "DstarD2KShh"

class DstarD2KShhBuilder(LineBuilder) :
    """
    Reconstruct D*+ -> D0 pi+, D0 -> KS0 h+ h- for h = K, pi
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
                              'DCutDIRA',
                              'DCutVtxChi2_KK',
                              'DCutVtxChi2_KP',
                              'DCutVtxChi2_PP',
                              'wideDMassCut_KKLL',
                              'wideDMassCut_KPLL',
                              'wideDMassCut_PPLL',
                              'wideDMassCut_KKDD',
                              'wideDMassCut_KPDD',
                              'wideDMassCut_PPDD',
                              'DCutTau',
                              'preFitDstarMassCut',
                              'DstarCutChi2NDOF_KK',
                              'DstarCutChi2NDOF_KP',
                              'DstarCutChi2NDOF_PP',
                              'SoftPionCutPIDe',
                              'DstarCutPT_KK',
                              'DstarCutPT_KP',
                              'DstarCutPT_PP',
                              'wideDMCutLower',
                              'wideDMCutUpper',
                              'KKLLPrescale',
                              'KKLLPostscale',
                              'KPLLPrescale',
                              'KPLLPostscale',
                              'PPLLPrescale',
                              'PPLLPostscale',
                              'KKDDPrescale',
                              'KKDDPostscale',
                              'KPDDPrescale',
                              'KPDDPostscale',
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
        self.selDauKP = [ self.selDauK, self.selDauP ]
        self.selDauPP = [ self.selDauP ]

        # Now we make the various flavours of D0 decay (KSK+K-, KSpi+pi-, KSK+pi-, KSK-pi+):
        strDecaysPP = [ "D0 -> KS0 pi+ pi-" ]
        strDecaysKP = [ "[D0 -> KS0 K+ pi-]cc", "[D0 -> KS0 K- pi+]cc" ]
        strDecaysKK = [ "D0 -> KS0 K+ K-" ]
        self.selD2KSKKLL = makeD2KShh('D2KSKKLLFor'+name, strDecaysKK, self.selKSLL, self.selDauKK, config['preFitDMassCut_LL'], config['preFitDCutPT'], config['DCutDIRA'], config['DCutVtxChi2_KK'], config['wideDMassCut_KKLL'], config['DCutTau'], config['KSCutZFDFromD'])
        self.selD2KSKPLL = makeD2KShh('D2KSKPLLFor'+name, strDecaysKP, self.selKSLL, self.selDauKP, config['preFitDMassCut_LL'], config['preFitDCutPT'], config['DCutDIRA'], config['DCutVtxChi2_KP'], config['wideDMassCut_KPLL'], config['DCutTau'], config['KSCutZFDFromD'])
        self.selD2KSPPLL = makeD2KShh('D2KSPPLLFor'+name, strDecaysPP, self.selKSLL, self.selDauPP, config['preFitDMassCut_LL'], config['preFitDCutPT'], config['DCutDIRA'], config['DCutVtxChi2_PP'], config['wideDMassCut_PPLL'], config['DCutTau'], config['KSCutZFDFromD'])
        self.selD2KSKKDD = makeD2KShh('D2KSKKDDFor'+name, strDecaysKK, self.selKSDD, self.selDauKK, config['preFitDMassCut_DD'], config['preFitDCutPT'], config['DCutDIRA'], config['DCutVtxChi2_KK'], config['wideDMassCut_KKDD'], config['DCutTau'], config['KSCutZFDFromD'])
        self.selD2KSKPDD = makeD2KShh('D2KSKPDDFor'+name, strDecaysKP, self.selKSDD, self.selDauKP, config['preFitDMassCut_DD'], config['preFitDCutPT'], config['DCutDIRA'], config['DCutVtxChi2_KP'], config['wideDMassCut_KPDD'], config['DCutTau'], config['KSCutZFDFromD'])
        self.selD2KSPPDD = makeD2KShh('D2KSPPDDFor'+name, strDecaysPP, self.selKSDD, self.selDauPP, config['preFitDMassCut_DD'], config['preFitDCutPT'], config['DCutDIRA'], config['DCutVtxChi2_PP'], config['wideDMassCut_PPDD'], config['DCutTau'], config['KSCutZFDFromD'])

        # ConjugateNeutralPID is needed for decays to self-conjugate final state (KSpi+pi-, KSK+K-)
        # It takes a D0 and makes a corresponding D0bar (identical except for PDG code)
        # It's an algorithm and can be wrapped in a Selection:
        from Configurables import ConjugateNeutralPID
        _localConj_KKLL = ConjugateNeutralPID('ConjugateD2KSKKLLFor'+name)
        _localConj_PPLL = ConjugateNeutralPID('ConjugateD2KSPPLLFor'+name)
        _localConj_KKDD = ConjugateNeutralPID('ConjugateD2KSKKDDFor'+name)
        _localConj_PPDD = ConjugateNeutralPID('ConjugateD2KSPPDDFor'+name)
        from PhysSelPython.Wrappers import Selection
        self.selD0Conj2KSKKLL = Selection('SelConjugateD2KSKKLLFor'+name, Algorithm = _localConj_KKLL, RequiredSelections = [self.selD2KSKKLL])
        self.selD0Conj2KSPPLL = Selection('SelConjugateD2KSPPLLFor'+name, Algorithm = _localConj_PPLL, RequiredSelections = [self.selD2KSPPLL])
        self.selD0Conj2KSKKDD = Selection('SelConjugateD2KSKKDDFor'+name, Algorithm = _localConj_KKDD, RequiredSelections = [self.selD2KSKKDD])
        self.selD0Conj2KSPPDD = Selection('SelConjugateD2KSPPDDFor'+name, Algorithm = _localConj_PPDD, RequiredSelections = [self.selD2KSPPDD])

        # Register the D0/D0bar selection as StrippingLine objects (Nick)
        self.lineD2KSPPLL = StrippingLine('StrippingD2KSPPLLFor'+name,
                                          prescale  = 1.0,
                                          postscale = 1.0,
                                          selection = self.selD2KSPPLL)
        
        self.lineD2KSPPDD = StrippingLine('StrippingD2KSPPDDFor'+name,
                                          prescale  = 1.0,
                                          postscale = 1.0,
                                          selection = self.selD2KSPPDD)
        
        self.lineD2KSKKLL = StrippingLine('StrippingD2KSKKLLFor'+name,
                                          prescale  = 1.0,
                                          postscale = 1.0,
                                          selection = self.selD2KSKKLL)
        
        self.lineD2KSKKDD = StrippingLine('StrippingD2KSKKDDFor'+name,
                                          prescale  = 1.0,
                                          postscale = 1.0,
                                          selection = self.selD2KSKKDD)
        
        self.registerLine(self.lineD2KSPPLL)
        self.registerLine(self.lineD2KSPPDD)
        self.registerLine(self.lineD2KSKKLL)
        self.registerLine(self.lineD2KSKKDD)
        
        # Reconstruct D*+ -> D0 pi+ (&cc), using a wide mass window:
        self.selDstar_KKLL = makeDstar('Dstar_KKLLFor'+name, [self.selD2KSKKLL, self.selD0Conj2KSKKLL], config['preFitDstarMassCut'], config['DstarCutChi2NDOF_KK'], config['SoftPionCutPIDe'], config['DstarCutPT_KK'], config['wideDMCutLower'], config['wideDMCutUpper'])
        self.selDstar_KPLL = makeDstar('Dstar_KPLLFor'+name, [self.selD2KSKPLL                       ], config['preFitDstarMassCut'], config['DstarCutChi2NDOF_KP'], config['SoftPionCutPIDe'], config['DstarCutPT_KP'], config['wideDMCutLower'], config['wideDMCutUpper'])
        self.selDstar_PPLL = makeDstar('Dstar_PPLLFor'+name, [self.selD2KSPPLL, self.selD0Conj2KSPPLL], config['preFitDstarMassCut'], config['DstarCutChi2NDOF_PP'], config['SoftPionCutPIDe'], config['DstarCutPT_PP'], config['wideDMCutLower'], config['wideDMCutUpper'])
        self.selDstar_KKDD = makeDstar('Dstar_KKDDFor'+name, [self.selD2KSKKDD, self.selD0Conj2KSKKDD], config['preFitDstarMassCut'], config['DstarCutChi2NDOF_KK'], config['SoftPionCutPIDe'], config['DstarCutPT_KK'], config['wideDMCutLower'], config['wideDMCutUpper'])
        self.selDstar_KPDD = makeDstar('Dstar_KPDDFor'+name, [self.selD2KSKPDD                       ], config['preFitDstarMassCut'], config['DstarCutChi2NDOF_KP'], config['SoftPionCutPIDe'], config['DstarCutPT_KP'], config['wideDMCutLower'], config['wideDMCutUpper'])
        self.selDstar_PPDD = makeDstar('Dstar_PPDDFor'+name, [self.selD2KSPPDD, self.selD0Conj2KSPPDD], config['preFitDstarMassCut'], config['DstarCutChi2NDOF_PP'], config['SoftPionCutPIDe'], config['DstarCutPT_PP'], config['wideDMCutLower'], config['wideDMCutUpper'])

        self.lineKKLL = StrippingLine(name+'KKLLLine',
                                      prescale = config['KKLLPrescale'],
                                      postscale = config['KKLLPostscale'],
                                      FILTER = _globalEventCuts,
                                      selection = self.selDstar_KKLL)
        self.lineKPLL = StrippingLine(name+'KPLLLine',
                                      prescale = config['KPLLPrescale'],
                                      postscale = config['KPLLPostscale'],
                                      FILTER = _globalEventCuts,
                                      selection = self.selDstar_KPLL)
        self.linePPLL = StrippingLine(name+'PPLLLine',
                                      prescale = config['PPLLPrescale'],
                                      postscale = config['PPLLPostscale'],
                                      FILTER = _globalEventCuts,
                                      selection = self.selDstar_PPLL)
        self.lineKKDD = StrippingLine(name+'KKDDLine',
                                      prescale = config['KKDDPrescale'],
                                      postscale = config['KKDDPostscale'],
                                      FILTER = _globalEventCuts,
                                      selection = self.selDstar_KKDD)
        self.lineKPDD = StrippingLine(name+'KPDDLine',
                                      prescale = config['KPDDPrescale'],
                                      postscale = config['KPDDPostscale'],
                                      FILTER = _globalEventCuts,
                                      selection = self.selDstar_KPDD)
        self.linePPDD = StrippingLine(name+'PPDDLine',
                                      prescale = config['PPDDPrescale'],
                                      postscale = config['PPDDPostscale'],
                                      FILTER = _globalEventCuts,
                                      selection = self.selDstar_PPDD)

        self.registerLine(self.lineKKLL)
        self.registerLine(self.lineKPLL)
        self.registerLine(self.linePPLL)
        self.registerLine(self.lineKKDD)
        self.registerLine(self.lineKPDD)
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

def makeD2KShh(name, inputDecayDescriptors, inputKs, inputDaughters, preFitMassCut, preFitDCutPT, DCutDIRA, DCutVtxChi2, postFitMassCut, DCutTau, KSCutZFDFromD) :
    """
    Given lists of D0 daughter tracks and a list of KS, reconstruct D0 -> KS h+ h-.
    The same routine works for all final states and for KS via long/downstream tracks.
    """

    _combCutsPT = "(APT > %(preFitDCutPT)s * MeV) & (APT > %(preFitDCutPT)s * MeV)" % locals()
    _combCutsMass = "(ADAMASS('D0') < %(preFitMassCut)s * MeV)" % locals()
    _combCuts = '( ' + _combCutsPT + ' & ' + _combCutsMass + ' )'
    _motherCutsBase = '((BPVVDZ < 7000.0*mm) & (BPVDIRA > %(DCutDIRA)s ))' % locals()
    _motherCutsVtx  = '(VFASPF(VCHI2) < %(DCutVtxChi2)s )' % locals()
    _motherCutsMass = "(ADMASS('D0') < %(postFitMassCut)s * MeV)" % locals()
    _motherCutsTau = "(BPVLTIME()> %(DCutTau)s * ps)" % locals()
    _motherCutsKSFD = '( (CHILD(VFASPF(VZ),1) - VFASPF(VZ)) > %(KSCutZFDFromD)s * mm)' % locals()
    _motherCuts = '( ' + _motherCutsBase + ' & ' + _motherCutsVtx + ' & ' + _motherCutsMass + ' & ' + _motherCutsTau + ' & ' + _motherCutsKSFD + ')'
    _D0 = CombineParticles( DecayDescriptors = inputDecayDescriptors,
                            CombinationCut = _combCuts,
                            MotherCut = _motherCuts)
    return Selection ( name,
                       Algorithm = _D0,
                       RequiredSelections = [inputKs] + inputDaughters)


def makeDstar(name, inputD0, preFitDstarMassCut, cutChi2, SoftPionCutPIDe, cutPT, wideDMCutLower, wideDMCutUpper) :
    """
    Given a list of D0, try to make D*+ -> D0 pi+
    """

    _softPi = DataOnDemand(Location = 'Phys/StdAllLoosePions/Particles')
    _cutsSoftPi = '( PIDe-PIDpi < %(SoftPionCutPIDe)s )' % locals()
    _cutsDstarComb = "ADAMASS('D*(2010)+') < %(preFitDstarMassCut)s *MeV" % locals()
    _cutsDstarMoth_base = '(PT > %(cutPT)s * MeV) & (VFASPF(VCHI2/VDOF) < %(cutChi2)s )' % locals()
    _cutsDstarMoth_DM = '(MM - CHILD(MM,1) - CHILD(MM,2) > %(wideDMCutLower)s *MeV) & (MM - CHILD(MM,1) - CHILD(MM,2) < %(wideDMCutUpper)s *MeV)' % locals()
    _cutsDstarMoth = '(' + _cutsDstarMoth_base + ' & ' + _cutsDstarMoth_DM + ')'
    _Dstar = CombineParticles( DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc",
                               DaughtersCuts = { "pi+" : _cutsSoftPi },
                               CombinationCut = _cutsDstarComb,
                               MotherCut = _cutsDstarMoth)
    return Selection ( name,
                       Algorithm = _Dstar,
                       RequiredSelections = inputD0 + [_softPi])

