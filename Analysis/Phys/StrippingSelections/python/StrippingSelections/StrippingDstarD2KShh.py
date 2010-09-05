# $Id: StrippingDstarD2KShh.py,v 1.6 2010-09-05 21:37:41 mcharles Exp $

__author__ = [ 'Mat Charles' ]
__date__ = '2010-02-05'
__version = '$Revision: 1.6 $'

# Please contact m.charles1@physics.ox.ac.uk before changing anything. Thanks.

# We have four lines that look very similar, except for differing
# inputs or cuts. Therefore we try to re-use code as much as possible.
#
# All lines look basically like this:
#
#   1) Make a KS
#   2) Combine KS with two tracks to make a D0
#   3) For each D0, make a corresponding D0bar.
#   3) Combine D0/D0bar with a third track to make a D*
#
# If you need to reduce rate quickly, the single best thing to do is raise the
# cut on the PT of the D*+. This is controlled by the variables:
#    DstarCutPT_pp
#    DstarCutPT_KK
# for KSpipi and KSKK, respectively. This will bring down the rate really fast,
# so a step of 500 MeV/c will probably be plenty.

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter  
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine, StrippingMember

class StrippingDstarD2KShhConf(LHCbConfigurableUser) :
    """
    Documentation
    """
    __slots__ = {
        'KSLLCutDIRA'           : 0.9997   ## unitless
        , 'KSDDCutDIRA'         : 0.99995  ## unitless
        , 'KSLLCutMass'         : 11.4     ## in MeV -- correct value 11.4
        , 'KSDDCutMass'         : 24.9     ## in MeV -- correct value 24.9
        , 'KSLLCutFDChi2'       : 100      ## unitless
        , 'KSDDCutFDChi2'       : 100      ## unitless
        , 'trackFromDCutP'      : 1500.0   ## in MeV
        , 'trackFromDCutPIDe'   : 15.0     ## unitless -- orig value 15, alt value 5
        , 'trackFromDCutPIDp'   : 20.0     ## unitless -- orig value 20, alt value 5
        , 'pionFromDCutPIDK'    : 0.0      ## unitless
        , 'kaonFromDCutPIDpi'   : 0.0      ## unitless
        , 'pionFromDCutTRCHI2DOF'   : 5.0  ## unitless
        , 'kaonFromDCutTRCHI2DOF'   : 5.0  ## unitless
        , 'DCutDIRA'            : 0.0      ## unitless -- remove "backwards-going" D0
        , 'DCutTau_pp'          : 0.3      ## ps
        , 'DCutTau_Kp'          : 0.3      ## ps
        , 'DCutTau_KK'          : 0.3      ## ps
        , 'DCutVtxChi2_pp'      : 13.0     ## unitless
        , 'DCutVtxChi2_Kp'      : 15.0     ## unitless
        , 'DCutVtxChi2_KK'      : 21.0     ## unitless
        , 'preFitDCutPT'        : 1500.0   ## in MeV
        , 'preFitDMassCut_LL'   : 130      ## in MeV
        , 'preFitDMassCut_DD'   : 270      ## in MeV
        , 'wideDMassCut_ppLL'   : 110.0    ## in MeV -- orig value 82.5 = 27.5 * 3
        , 'wideDMassCut_KpLL'   : 92.0     ## in MeV -- orig value 69.0 = 23.0 * 3
        , 'wideDMassCut_KKLL'   : 57.2     ## in MeV -- orig value 42.9 = 14.3 * 3
        , 'wideDMassCut_ppDD'   : 140.0    ## in MeV -- orig value 105.0 = 35.0 * 3
        , 'wideDMassCut_KpDD'   : 120.0    ## in MeV -- orig value 90.0 = 30.0 * 3
        , 'wideDMassCut_KKDD'   : 72.0     ## in MeV -- orig value 54.0 = 18.0 * 3
        , 'sidebandDMassOverlap' : 2.0     ## in MeV
        , 'preFitDstarMassCut'  : 200.0    ## in MeV; make sure it's well above the largest D mass window
        , 'wideDMCutLower'      : 0.0      ## in MeV
        , 'wideDMCutUpper'      : 15.0     ## in MeV # was 13.35
        , 'DstarCutPT_pp'       : 3000.0   ## in MeV
        , 'DstarCutPT_Kp'       : 3000.0   ## in MeV
        , 'DstarCutPT_KK'       : 2500.0   ## in MeV
        , 'DstarCutChi2NDOF_pp' : 20.0     ## unitless
        , 'DstarCutChi2NDOF_Kp' : 20.0     ## unitless
        , 'DstarCutChi2NDOF_KK' : 60.0     ## unitless
        , 'SoftPionCutPIDe'     : 5.0      ## unitless
        , 'signalPrescale'      : 1.0
        , 'sidebandPrescale'    : 1.0
        }

    # Make a complete collection of stripping lines
    def MakeLines(self):
        # Code taken from the Topo line -- check whether there are fewer than 240 tracks in the event...
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        modules = CoreFactory('CoreFactory').Modules
        for i in ['LoKiTrigger.decorators']:
          if i not in modules : modules.append(i)
        from Configurables import LoKi__VoidFilter as VoidFilter
        StrippingKillTooManyKShh = VoidFilter('StrippingKillTooManyKShh', Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )")

        # All lines start with one of: KS->LL, KS->DD
        memberKSLL = self.makeKSLL()
        memberKSDD = self.makeKSDD()

        # FIXME: For signalbox line, use narrow mass window up-front.
        # FIXME: For sideband lines, this needs to be wider.
        # Reconstruct D0 -> KS h+ h- with wide mass window:
        memberD2KSpp_LL_wide = self.makeD2KSpp(self.getProps()['preFitDMassCut_LL'], self.getProps()['wideDMassCut_ppLL'])
        memberD2KSpp_DD_wide = self.makeD2KSpp(self.getProps()['preFitDMassCut_DD'], self.getProps()['wideDMassCut_ppDD'])
        memberD2KSKK_LL_wide = self.makeD2KSKK(self.getProps()['preFitDMassCut_LL'], self.getProps()['wideDMassCut_KKLL'])
        memberD2KSKK_DD_wide = self.makeD2KSKK(self.getProps()['preFitDMassCut_DD'], self.getProps()['wideDMassCut_KKDD'])

        # FIXME: I think this becomes redundant once above mass cuts are done properly...
        # Add an optional mass window filter here.
        # By default it requires events to lie within a signal box.
        #memberDMassFilter = StrippingMember(FilterDesktop, "FilterD0", InputLocations = [ "%CombineD0" ], Code = "ALL")
        memberDMass_ppLL = self.makeDSignalFilter(self.getProps()['wideDMassCut_ppLL'])
        memberDMass_ppDD = self.makeDSignalFilter(self.getProps()['wideDMassCut_ppDD'])
        memberDMass_KKLL = self.makeDSignalFilter(self.getProps()['wideDMassCut_KKLL'])
        memberDMass_KKDD = self.makeDSignalFilter(self.getProps()['wideDMassCut_KKDD'])
        
        # Member to make charge-conjugate (very simple & flexible):
        from Configurables import ConjugateNeutralPID
        memberConjugate = StrippingMember(ConjugateNeutralPID, "ConjugateD0", InputLocations = [ "%FilterD0" ])

        # Reconstruct D*+ -> D0 pi+ (&cc), using a wide mass window:
        memberDstar_pp = self.makeDstar(self.getProps()['DstarCutPT_pp'], self.getProps()['DstarCutChi2NDOF_pp'] )
        memberDstar_KK = self.makeDstar(self.getProps()['DstarCutPT_KK'], self.getProps()['DstarCutChi2NDOF_KK'] )

        # Filter to accept only events in a certain part of (DM vs M) space.
        # By default it requires the events to lie in the signal box of DM:
        memberDstarMassFilter = self.makeDstarDeltaMassFilter(self.getProps()['wideDMCutLower'], self.getProps()['wideDMCutUpper'])
        
        # Make lines for signal:
        lineSig_ppLL = StrippingLine('DstarD2KSPiPi_KSLL', prescale = self.getProps()['signalPrescale'], algos = [ StrippingKillTooManyKShh, memberKSLL, memberD2KSpp_LL_wide, memberDMass_ppLL, memberConjugate, memberDstar_pp, memberDstarMassFilter ])
        lineSig_ppDD = StrippingLine('DstarD2KSPiPi_KSDD', prescale = self.getProps()['signalPrescale'], algos = [ StrippingKillTooManyKShh, memberKSDD, memberD2KSpp_DD_wide, memberDMass_ppDD, memberConjugate, memberDstar_pp, memberDstarMassFilter ])
        lineSig_KKLL = StrippingLine('DstarD2KSKK_KSLL',   prescale = self.getProps()['signalPrescale'], algos = [ StrippingKillTooManyKShh, memberKSLL, memberD2KSKK_LL_wide, memberDMass_KKLL, memberConjugate, memberDstar_KK, memberDstarMassFilter ])
        lineSig_KKDD = StrippingLine('DstarD2KSKK_KSDD',   prescale = self.getProps()['signalPrescale'], algos = [ StrippingKillTooManyKShh, memberKSDD, memberD2KSKK_DD_wide, memberDMass_KKDD, memberConjugate, memberDstar_KK, memberDstarMassFilter ])

        # All done:
        lineList = [ lineSig_ppLL, lineSig_ppDD, lineSig_KKLL, lineSig_KKDD ]
        return lineList

    def makeDstarDeltaMassFilter(self, cutLower, cutUpper):
        # Cut on delta mass:
        strCutDeltaMassLower = "( MM - CHILD(MM,1) - CHILD(MM,2) > " + str(cutLower) + "*MeV )"
        strCutDeltaMassUpper = "( MM - CHILD(MM,1) - CHILD(MM,2) < " + str(cutUpper) + "*MeV )"
        strCut = '(' + strCutDeltaMassLower + ' & ' + strCutDeltaMassUpper + ')'
        # Filter:
        f = StrippingMember(FilterDesktop
                            , "FilterDstar"
                            , InputLocations = [ "%CombineDstar" ]
                            , Code = strCut)
        return f

    def makeDstar(self, cutPT, cutChi2):
        str_cutsSoftPi = '( PIDe-PIDpi < %(SoftPionCutPIDe)s )' % self.getProps()
        # FIXME: Instead of applying cut on m(D0pi), cut directly on m(D0pi)-m(D0)-m(pi) at CombinationCut level.
        str_cutsDstarComb = "ADAMASS('D*(2010)+') < %(preFitDstarMassCut)s *MeV" % self.getProps()
        strDstarMotherCutBasepp = '(PT > ' + str(cutPT) + '*MeV) & (VFASPF(VCHI2/VDOF)<' + str(cutChi2) + ')'
        str_cutsDstar_DM_wide = '(MM - CHILD(MM,1) - CHILD(MM,2) > %(wideDMCutLower)s *MeV) & (MM - CHILD(MM,1) - CHILD(MM,2) < %(wideDMCutUpper)s *MeV)' % self.getProps()
        strDstarMothCut = '(' + strDstarMotherCutBasepp + '&' + str_cutsDstar_DM_wide + ')'
        cp = StrippingMember(CombineParticles
                             , "CombineDstar"
                             , InputLocations = [ "%FilterD0", "%ConjugateD0", "StdLoosePions" ]
                             , DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
                             , DaughtersCuts = { "pi+" : str_cutsSoftPi }
                             , CombinationCut = str_cutsDstarComb
                             , MotherCut = strDstarMothCut
                             )
        return cp

    def makeKSLL(self):
        strCode = "(BPVVDZ>-1000.0*mm) & (BPVVDZ<650.0*mm) & (BPVDIRA > %(KSLLCutDIRA)s ) & (ADMASS('KS0') < %(KSLLCutMass)s *MeV) & (BPVVDCHI2> %(KSLLCutFDChi2)s )" % self.getProps()
        fd = StrippingMember(FilterDesktop,
                             "FilterKS", 
                             InputLocations = [ "StdLooseKsLL" ],
                             Code = strCode
                             )
        return fd

    def makeKSDD(self):
        strCode = "(BPVVDZ>0.0*mm) & (BPVVDZ<2300.0*mm) & (BPVDIRA > %(KSDDCutDIRA)s ) & (ADMASS('KS0') < %(KSDDCutMass)s *MeV) & (BPVVDCHI2> %(KSDDCutFDChi2)s )" % self.getProps()
        fd = StrippingMember(FilterDesktop,
                             "FilterKS", 
                             InputLocations = [ "StdLooseKsDD" ],
                             Code = strCode
                             )
        return fd

    # Pions from D decay all use the same daughter cuts, regardless of the exact
    # decay mode or whether the KS is LL or DD:
    def makePionDaughterCutsForD(self):
        strCutP = "(P > %(trackFromDCutP)s *MeV)" % self.getProps()
        strCutPIDe = "(PIDe-PIDpi < %(trackFromDCutPIDe)s )" % self.getProps()
        strCutPIDp = "(PIDp-PIDpi < %(trackFromDCutPIDp)s ) " % self.getProps()
        strCutPIDK = "(PIDK -PIDpi < %(pionFromDCutPIDK)s ) " % self.getProps()
        strCutChi2 = "(TRCHI2DOF < %(pionFromDCutTRCHI2DOF)s )" % self.getProps()
        strCuts = '(' + strCutP + '&' + strCutPIDe + '&' + strCutPIDp \
            + '&' + strCutPIDK + '&' + strCutChi2 + ')'
        return strCuts

    # Kaons from D decay all use the same daughter cuts, regardless of the exact
    # decay mode or whether the KS is LL or DD:
    def makeKaonDaughterCutsForD(self):
        strCutP = "(P > %(trackFromDCutP)s *MeV)" % self.getProps()
        strCutPIDe = "(PIDe-PIDK < %(trackFromDCutPIDe)s )" % self.getProps()
        strCutPIDp = "(PIDp-PIDK < %(trackFromDCutPIDp)s ) " % self.getProps()
        strCutPIDK = "(PIDpi-PIDK < %(kaonFromDCutPIDpi)s ) " % self.getProps()
        strCutChi2 = "(TRCHI2DOF < %(kaonFromDCutTRCHI2DOF)s )" % self.getProps()
        strCuts = '(' + strCutP + '&' + strCutPIDe + '&' + strCutPIDp \
            + '&' + strCutPIDK + '&' + strCutChi2 + ')'
        return strCuts

    # D0 -> KS pi+ pi- (for both KS -> LL and DD)
    def makeD2KSpp(self, preFitMassCut, postFitMassCut):
        strInputKS = '%FilterKS' # Pick up the KS list from earlier in the line
        strInputPi = 'StdLoosePions'
        strDecay = "D0 -> KS0 pi+ pi-"
        strCutsPi = self.makePionDaughterCutsForD()
        strCutsCombDauIP = "(AHASCHILD((ABSID=='pi+') & (MIPCHI2DV(PRIMARY)>44.0)))"
        strCutsCombMass  = "(ADAMASS('D0') < " + str(preFitMassCut) + "*MeV)"
        strCutsCombPT = "(APT > %(preFitDCutPT)s * MeV)" % self.getProps()
        #strCutsComb = '(' + strCutsCombDauIP + '&' + strCutsCombMass + '&' + strCutsCombPT + ')'
        strCutsComb = '(' + strCutsCombMass + '&' + strCutsCombPT + ')'
        strDMotherCut_base  = '(BPVVDZ < 7000.0*mm) & (BPVDIRA > %(DCutDIRA)s )' % self.getProps()
        strDMotherCutpp = strDMotherCut_base + ' & (VFASPF(VCHI2) < %(DCutVtxChi2_pp)s )'  % self.getProps()
        strMassCut = "(ADMASS('D0')<" + str(postFitMassCut) + "*MeV)"
        strTestLifetimeCut = "(BPVLTIME()> %(DCutTau_pp)s * ps)" % self.getProps()
        strCutsMoth = '(' + strDMotherCutpp + ' & ' + strMassCut + '&' + strTestLifetimeCut + ')'
        cp = StrippingMember(CombineParticles
                             , "CombineD0"
                             , InputLocations = [ strInputKS, strInputPi ]
                             , DecayDescriptor = strDecay
                             , DaughtersCuts = { 'pi+' : strCutsPi }
                             , CombinationCut = strCutsComb
                             , MotherCut = strCutsMoth
                             )
        return cp

    # D0 -> KS K+ pi- and D0 -> KS K- pi+ (for both KS -> LL and DD)
    def makeD2KSKp(self, preFitMassCut, postFitMassCut):
        strInputKS = '%FilterKS' # Pick up the KS list from earlier in the line
        strInputPi = 'StdLoosePions'
        strInputK  = 'StdLooseKaons'
        strDecays =  [ "D0 -> KS0 K+ pi-", "D0 -> KS0 K- pi+" ]
        strCutsPi = self.makePionDaughterCutsForD()
        strCutsK  = self.makeKaonDaughterCutsForD()
        strCutsCombDauIP_pi = "(AHASCHILD((ABSID=='pi+') & (MIPCHI2DV(PRIMARY)>20.0)))"
        strCutsCombDauIP_K  = "(AHASCHILD((ABSID=='K+') & (MIPCHI2DV(PRIMARY)>20.0)))"
        strCutsCombDauIP = '(' + strCutsCombDauIP_pi + ' | ' + strCutsCombDauIP_K + ')'
        strCutsCombMass  = "(ADAMASS('D0') < " + str(preFitMassCut) + "*MeV)"
        strCutsCombPT = "(APT > %(preFitDCutPT)s * MeV)" % self.getProps()
        #strCutsComb = '(' + strCutsCombDauIP + '&' + strCutsCombMass + '&' + strCutsCombPT + ')'
        strCutsComb = '(' + strCutsCombMass + '&' + strCutsCombPT + ')'
        strDMotherCut_base  = '(BPVVDZ < 7000.0*mm) & (BPVDIRA > %(DCutDIRA)s )' % self.getProps()
        strDMotherCutKp = strDMotherCut_base + ' & (VFASPF(VCHI2) < %(DCutVtxChi2_Kp)s )'  % self.getProps()
        strMassCut = "(ADMASS('D0')<" + str(postFitMassCut) + "*MeV)"
        strTestLifetimeCut = "(BPVLTIME()> %(DCutTau_Kp)s * ps)" % self.getProps()
        strCutsMoth = '(' + strDMotherCutKp + ' & ' + strMassCut + '&' + strTestLifetimeCut + ')'
        cp = StrippingMember(CombineParticles
                             , "CombineD0"
                             , InputLocations = [ strInputKS, strInputPi, strInputK ]
                             , DecayDescriptors = strDecays
                             , DaughtersCuts = { 'pi+' : strCutsPi, 'K+' : strCutsK }
                             , CombinationCut = strCutsComb
                             , MotherCut = strCutsMoth
                             )
        return cp

    # D0 -> KS K+ K- (for both KS -> LL and DD)
    def makeD2KSKK(self, preFitMassCut, postFitMassCut):
        strInputKS = '%FilterKS' # Pick up the KS list from earlier in the line
        strInputK  = 'StdLooseKaons'
        strDecay = "D0 -> KS0 K+ K-"
        strCutsK  = self.makeKaonDaughterCutsForD()
        strCutsCombDauIP = "(AHASCHILD((ABSID=='K+') & (MIPCHI2DV(PRIMARY)>14.0)))"
        strCutsCombMass  = "(ADAMASS('D0') < " + str(preFitMassCut) + "*MeV)"
        strCutsCombPT = "(APT > %(preFitDCutPT)s * MeV)" % self.getProps()
        #strCutsComb = '(' + strCutsCombDauIP + '&' + strCutsCombMass + '&' + strCutsCombPT + ')'
        strCutsComb = '(' + strCutsCombMass + '&' + strCutsCombPT + ')'
        strDMotherCut_base  = '(BPVVDZ < 7000.0*mm) & (BPVDIRA > %(DCutDIRA)s )' % self.getProps()
        strDMotherCutKK = strDMotherCut_base + ' & (VFASPF(VCHI2) < %(DCutVtxChi2_KK)s )'  % self.getProps()
        strMassCut = "(ADMASS('D0')<" + str(postFitMassCut) + "*MeV)"
        strTestLifetimeCut = "(BPVLTIME()> %(DCutTau_KK)s * ps)" % self.getProps()
        strCutsMoth = '(' + strDMotherCutKK + ' & ' + strMassCut + '&' + strTestLifetimeCut + ')'
        cp = StrippingMember(CombineParticles
                             , "CombineD0"
                             , InputLocations = [ strInputKS, strInputK ]
                             , DecayDescriptor = strDecay
                             , DaughtersCuts = { 'K+' : strCutsK }
                             , CombinationCut = strCutsComb
                             , MotherCut = strCutsMoth
                             )
        return cp
    
    def makeDSignalFilter(self, massCut):
        strInputD = '%CombineD0'
        strCode = "(ADMASS('D0')<" + str(massCut) + ")"
        fd = StrippingMember(FilterDesktop,
                             "FilterD0",
                             InputLocations = [ strInputD ],
                             Code = strCode
                             )
        return fd;

    # Probably this will be obsolete soon...
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d

# EOF
