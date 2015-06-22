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
 * Separate container for both D0 and D0bar replaced by []cc selection.
The ownership of this module has been taken over by Marco Gersabeck on 12 May 2015.
'''

__author__ = ['Marco Gersabeck']
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

default_config = { 
    'NAME'        : name,
    'WGs'         : ['Charm'],
    'STREAMS'     : [ 'CharmToBeSwum' ],
    'BUILDERTYPE' : 'DstarD2KShhBuilder',
    'CONFIG'      : {
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
        ,'DCutDIRA'             :    0.0      ## unitless -- remove "backwards-going" D0
        ,'DCutTau'              :    0.2      ## ps -- Mat changed from 0.3 to 0.2 in S20
        ,'DCutVtxChi2_KK'       :   21.0      ## unitless
        ,'DCutVtxChi2_KP'       :   15.0      ## unitless
        ,'DCutVtxChi2_PP'       :   13.0      ## unitless
        ,'preFitDCutPT'         : 1500.0      ## in MeV
        ,'preFitDMassCut_LL'    :  130        ## in MeV
        ,'preFitDMassCut_DD'    :  270        ## in MeV
        ,'wideDMassCut_KKLL'    :   70.0      ## in MeV -- Mat changed from  57.2 to  70.0 in S20
        ,'wideDMassCut_KPLL'    :  105.0      ## in MeV -- Mat changed from  92.0 to 105.0 in S20
        ,'wideDMassCut_PPLL'    :  120.0      ## in MeV -- Mat changed from 110.0 to 120.0 in S20
        ,'wideDMassCut_KKDD'    :   85.0      ## in MeV -- Mat changed from  72.0 to  85.0 in S20
        ,'wideDMassCut_KPDD'    :  130.0      ## in MeV -- Mat changed from 120.0 to 130.0 in S20
        ,'wideDMassCut_PPDD'    :  150.0      ## in MeV -- Mat changed from 140.0 to 150.0 in S20
        ,'preFitDstarMassCut'   :  200.0      ## in MeV; make sure it's well above the largest D mass window
        ,'wideDMCutLower'       :  -10.0      ## in MeV: allow negative tail made by CombineParticles -- Mat changed from -2.0 to -10.0 in S20
        ,'wideDMCutUpper'       :   15.0      ## in MeV -- Mat changed from 15.0 to 25.0 in S20
        ,'DstarCutPT_KK'        : 2200.0      ## in MeV
        ,'DstarCutPT_KP'        : 2200.0      ## in MeV
        ,'DstarCutPT_PP'        : 2200.0      ## in MeV
        ,'DstarCutChi2NDOF_KK'  :   60.0      ## unitless
        ,'DstarCutChi2NDOF_KP'  :   20.0      ## unitless
        ,'DstarCutChi2NDOF_PP'  :   20.0      ## unitless
        ,'SoftPionCutPIDe'      :    2.0      ## unitless -- Mat changed from 5.0 to 2.0 in S20
        ,'KKLLPrescale'         :    1.0
        ,'KKLLPostscale'        :    1.0
        ,'KPLLPrescale'         :    1.0
        ,'KPLLPostscale'        :    1.0
        ,'PPLLPrescale'         :    1.0
        ,'PPLLPostscale'        :    1.0
        ,'KKDDPrescale'         :    1.0
        ,'KKDDPostscale'        :    1.0
        ,'KPDDPrescale'         :    1.0
        ,'KPDDPostscale'        :    1.0
        ,'PPDDPrescale'         :    1.0
        ,'PPDDPostscale'        :    1.0
    }
}

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
        strDecaysPP = [ "[D0 -> KS0 pi+ pi-]cc" ]
        strDecaysKP = [ "[D0 -> KS0 K+ pi-]cc", "[D0 -> KS0 K- pi+]cc" ]
        strDecaysKK = [ "[D0 -> KS0 K+ K-]cc" ]
        self.selD2KSKKLL = makeD2KShh('D2KSKKLLFor'+name, strDecaysKK, self.selKSLL, self.selDauKK, config['preFitDMassCut_LL'], config['preFitDCutPT'], config['DCutDIRA'], config['DCutVtxChi2_KK'], config['wideDMassCut_KKLL'], config['DCutTau'], config['KSCutZFDFromD'])
        self.selD2KSKPLL = makeD2KShh('D2KSKPLLFor'+name, strDecaysKP, self.selKSLL, self.selDauKP, config['preFitDMassCut_LL'], config['preFitDCutPT'], config['DCutDIRA'], config['DCutVtxChi2_KP'], config['wideDMassCut_KPLL'], config['DCutTau'], config['KSCutZFDFromD'])
        self.selD2KSPPLL = makeD2KShh('D2KSPPLLFor'+name, strDecaysPP, self.selKSLL, self.selDauPP, config['preFitDMassCut_LL'], config['preFitDCutPT'], config['DCutDIRA'], config['DCutVtxChi2_PP'], config['wideDMassCut_PPLL'], config['DCutTau'], config['KSCutZFDFromD'])
        self.selD2KSKKDD = makeD2KShh('D2KSKKDDFor'+name, strDecaysKK, self.selKSDD, self.selDauKK, config['preFitDMassCut_DD'], config['preFitDCutPT'], config['DCutDIRA'], config['DCutVtxChi2_KK'], config['wideDMassCut_KKDD'], config['DCutTau'], config['KSCutZFDFromD'])
        self.selD2KSKPDD = makeD2KShh('D2KSKPDDFor'+name, strDecaysKP, self.selKSDD, self.selDauKP, config['preFitDMassCut_DD'], config['preFitDCutPT'], config['DCutDIRA'], config['DCutVtxChi2_KP'], config['wideDMassCut_KPDD'], config['DCutTau'], config['KSCutZFDFromD'])
        self.selD2KSPPDD = makeD2KShh('D2KSPPDDFor'+name, strDecaysPP, self.selKSDD, self.selDauPP, config['preFitDMassCut_DD'], config['preFitDCutPT'], config['DCutDIRA'], config['DCutVtxChi2_PP'], config['wideDMassCut_PPDD'], config['DCutTau'], config['KSCutZFDFromD'])

        # Reconstruct D*+ -> D0 pi+ (&cc), using a wide mass window:
        self.selDstar_KKLL = makeDstar('Dstar_KKLLFor'+name, [self.selD2KSKKLL], config['preFitDstarMassCut'], config['DstarCutChi2NDOF_KK'], config['SoftPionCutPIDe'], config['DstarCutPT_KK'], config['wideDMCutLower'], config['wideDMCutUpper'])
        self.selDstar_KPLL = makeDstar('Dstar_KPLLFor'+name, [self.selD2KSKPLL], config['preFitDstarMassCut'], config['DstarCutChi2NDOF_KP'], config['SoftPionCutPIDe'], config['DstarCutPT_KP'], config['wideDMCutLower'], config['wideDMCutUpper'])
        self.selDstar_PPLL = makeDstar('Dstar_PPLLFor'+name, [self.selD2KSPPLL], config['preFitDstarMassCut'], config['DstarCutChi2NDOF_PP'], config['SoftPionCutPIDe'], config['DstarCutPT_PP'], config['wideDMCutLower'], config['wideDMCutUpper'])
        self.selDstar_KKDD = makeDstar('Dstar_KKDDFor'+name, [self.selD2KSKKDD], config['preFitDstarMassCut'], config['DstarCutChi2NDOF_KK'], config['SoftPionCutPIDe'], config['DstarCutPT_KK'], config['wideDMCutLower'], config['wideDMCutUpper'])
        self.selDstar_KPDD = makeDstar('Dstar_KPDDFor'+name, [self.selD2KSKPDD], config['preFitDstarMassCut'], config['DstarCutChi2NDOF_KP'], config['SoftPionCutPIDe'], config['DstarCutPT_KP'], config['wideDMCutLower'], config['wideDMCutUpper'])
        self.selDstar_PPDD = makeDstar('Dstar_PPDDFor'+name, [self.selD2KSPPDD], config['preFitDstarMassCut'], config['DstarCutChi2NDOF_PP'], config['SoftPionCutPIDe'], config['DstarCutPT_PP'], config['wideDMCutLower'], config['wideDMCutUpper'])

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

