# $Id: StrippingDstarD2KShh.py,v 1.3 2010-04-26 07:41:36 mcharles Exp $

__author__ = [ 'Mat Charles' ]
__date__ = '2010-02-05'
__version = '$Revision: 1.3 $'

# Please contact m.charles1@physics.ox.ac.uk before changing anything. Thanks.

# We have many lines that look very similar, except for differing
# inputs or cuts. Therefore we try to re-use code as much as possible.
#
# All lines look basically like this:
#
#   1) Make a KS
#   2) Combine KS with two tracks to make a D0
#   3) For each D0, make a corresponding D0bar.
#   3) Combine D0/D0bar with a third track to make a D*
#
# The main complication is that we have "signal" and "background"
# lines, which have the same cuts except for the 2D mass windows
# (signal box vs sidebands).

# 26 Apr 2010:
#   Loosened some cuts for early data running. This increases
#   retention rate by about a factor of 10 in a control sample
#   of L0*HLT1-stripped min-bias MC events. This change is
#   aimed at getting more statistics in the early running, and
#   is NOT intended to be kept long-term. The changes made were:
#     DstarCutPT_pp: 3400.0 --> 2000.0 MeV
#     DstarCutPT_Kp: 3400.0 --> 2000.0 MeV
#     DstarCutPT_KK: 3200.0 --> 2000.0 MeV
#     sidebandPrescale: 0.2 --> 1.0
#   If the rate starts becoming too high, these changes can be
#   reversed safely. Please change the sideband prescale first,
#   and then the PT cuts if further reduction is needed.

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
        , 'KSLLCutMass'         : 11.4     ## in MeV
        , 'KSDDCutMass'         : 24.9     ## in MeV
        , 'KSLLCutFDChi2'       : 100      ## unitless
        , 'KSDDCutFDChi2'       : 100      ## unitless
        , 'trackFromDCutP'      : 1500.0   ## in MeV
        , 'trackFromDCutPIDe'   : 15.0     ## unitless
        , 'trackFromDCutPIDp'   : 20.0     ## unitless
        , 'pionFromDCutPIDK'    : 10.0     ## unitless
        , 'kaonFromDCutPIDpi'   : 10.0     ## unitless
        , 'pionFromDCutTRPCHI2' : 0.00001  ## unitless
        , 'kaonFromDCutTRPCHI2' : -0.1     ## everything passes
        , 'DCutDIRA'            : 0.995    ## unitless
        , 'DCutVtxChi2_pp'      : 13.0     ## unitless
        , 'DCutVtxChi2_Kp'      : 15.0     ## unitless
        , 'DCutVtxChi2_KK'      : 21.0     ## unitless
        , 'preFitDMassCut_LL'   : 100      ## in MeV
        , 'preFitDMassCut_DD'   : 200      ## in MeV
        , 'wideDMassCut_ppLL'   : 82.5     ## in MeV
        , 'wideDMassCut_KpLL'   : 69.0     ## in MeV
        , 'wideDMassCut_KKLL'   : 42.9     ## in MeV
        , 'wideDMassCut_ppDD'   : 105.0    ## in MeV
        , 'wideDMassCut_KpDD'   : 90.0     ## in MeV
        , 'wideDMassCut_KKDD'   : 54.0     ## in MeV
        , 'signalDMassCut_ppLL' : 27.5     ## in MeV
        , 'signalDMassCut_KpLL' : 23.0     ## in MeV
        , 'signalDMassCut_KKLL' : 14.3     ## in MeV
        , 'signalDMassCut_ppDD' : 35.0     ## in MeV
        , 'signalDMassCut_KpDD' : 30.0     ## in MeV
        , 'signalDMassCut_KKDD' : 18.0     ## in MeV
        , 'sidebandDMassOverlap' : 2.0     ## in MeV
        , 'preFitDstarMassCut'  : 200.0    ## in MeV; make sure it's well above the largest D mass window
        , 'signalDMCutLower'    : 3.35     ## in MeV
        , 'signalDMCutUpper'    : 8.35     ## in MeV
        , 'wideDMCutLower'      : 0.0      ## in MeV
        , 'wideDMCutUpper'      : 13.35    ## in MeV
        , 'DstarCutPT_pp'       : 2000.0   ## in MeV
        , 'DstarCutPT_Kp'       : 2000.0   ## in MeV
        , 'DstarCutPT_KK'       : 2000.0   ## in MeV
        , 'DstarCutChi2NDOF_pp' : 20.0     ## unitless
        , 'DstarCutChi2NDOF_Kp' : 20.0     ## unitless
        , 'DstarCutChi2NDOF_KK' : 60.0     ## unitless
        , 'SoftPionCutPIDe'     : 5.0      ## unitless
        , 'signalPrescale'      : 1.0
        , 'sidebandPrescale'    : 1.0
        }

    # Make a complete collection of stripping lines
    def MakeLines(self):
        # All lines start with one of: KS->LL, KS->DD
        memberKSLL = self.makeKSLL()
        memberKSDD = self.makeKSDD()

        # Reconstruct D0 -> KS h+ h- with wide mass window:
        memberD2KSpp_LL_wide = self.makeD2KSpp(self.getProps()['preFitDMassCut_LL'], self.getProps()['wideDMassCut_ppLL'])
        memberD2KSpp_DD_wide = self.makeD2KSpp(self.getProps()['preFitDMassCut_DD'], self.getProps()['wideDMassCut_ppDD'])
        memberD2KSKp_LL_wide = self.makeD2KSKp(self.getProps()['preFitDMassCut_LL'], self.getProps()['wideDMassCut_KpLL'])
        memberD2KSKp_DD_wide = self.makeD2KSKp(self.getProps()['preFitDMassCut_DD'], self.getProps()['wideDMassCut_KpDD'])
        memberD2KSKK_LL_wide = self.makeD2KSKK(self.getProps()['preFitDMassCut_LL'], self.getProps()['wideDMassCut_KKLL'])
        memberD2KSKK_DD_wide = self.makeD2KSKK(self.getProps()['preFitDMassCut_DD'], self.getProps()['wideDMassCut_KKDD'])

        # Add an optional mass window filter here.
        # By default it requires events to lie within a signal box.
        #memberDMassFilter = StrippingMember(FilterDesktop, "FilterD0", InputLocations = [ "%CombineD0" ], Code = "ALL")
        memberDMass_ppLL = self.makeDSignalFilter(self.getProps()['signalDMassCut_ppLL'])
        memberDMass_ppDD = self.makeDSignalFilter(self.getProps()['signalDMassCut_ppDD'])
        memberDMass_KpLL = self.makeDSignalFilter(self.getProps()['signalDMassCut_KpLL'])
        memberDMass_KpDD = self.makeDSignalFilter(self.getProps()['signalDMassCut_KpDD'])
        memberDMass_KKLL = self.makeDSignalFilter(self.getProps()['signalDMassCut_KKLL'])
        memberDMass_KKDD = self.makeDSignalFilter(self.getProps()['signalDMassCut_KKDD'])
        
        # Member to make charge-conjugate (very simple & flexible):
        from Configurables import ConjugateNeutralPID
        memberConjugate = StrippingMember(ConjugateNeutralPID, "ConjugateD0", InputLocations = [ "%FilterD0" ])

        # Reconstruct D*+ -> D0 pi+ (&cc), using a wide mass window:
        memberDstar_pp = self.makeDstar(self.getProps()['DstarCutPT_pp'], self.getProps()['DstarCutChi2NDOF_pp'] )
        memberDstar_Kp = self.makeDstar(self.getProps()['DstarCutPT_Kp'], self.getProps()['DstarCutChi2NDOF_Kp'] )
        memberDstar_KK = self.makeDstar(self.getProps()['DstarCutPT_KK'], self.getProps()['DstarCutChi2NDOF_KK'] )

        # Filter to accept only events in a certain part of (DM vs M) space.
        # By default it requires the events to lie in the signal box of DM:
        memberDstarMassFilter = self.makeDstarSignalFilter()
        
        # Make lines for signal:
        lineSig_ppLL = StrippingLine('DstarD2KSPiPi_KSLL', prescale = self.getProps()['signalPrescale'], algos = [ memberKSLL, memberD2KSpp_LL_wide, memberDMass_ppLL, memberConjugate, memberDstar_pp, memberDstarMassFilter ])
        lineSig_ppDD = StrippingLine('DstarD2KSPiPi_KSDD', prescale = self.getProps()['signalPrescale'], algos = [ memberKSDD, memberD2KSpp_DD_wide, memberDMass_ppDD, memberConjugate, memberDstar_pp, memberDstarMassFilter ])
        lineSig_KpLL = StrippingLine('DstarD2KSKPi_KSLL',  prescale = self.getProps()['signalPrescale'], algos = [ memberKSLL, memberD2KSKp_LL_wide, memberDMass_KpLL, memberConjugate, memberDstar_Kp, memberDstarMassFilter ])
        lineSig_KpDD = StrippingLine('DstarD2KSKPi_KSDD',  prescale = self.getProps()['signalPrescale'], algos = [ memberKSDD, memberD2KSKp_DD_wide, memberDMass_KpDD, memberConjugate, memberDstar_Kp, memberDstarMassFilter ])
        lineSig_KKLL = StrippingLine('DstarD2KSKK_KSLL',   prescale = self.getProps()['signalPrescale'], algos = [ memberKSLL, memberD2KSKK_LL_wide, memberDMass_KKLL, memberConjugate, memberDstar_KK, memberDstarMassFilter ])
        lineSig_KKDD = StrippingLine('DstarD2KSKK_KSDD',   prescale = self.getProps()['signalPrescale'], algos = [ memberKSDD, memberD2KSKK_DD_wide, memberDMass_KKDD, memberConjugate, memberDstar_KK, memberDstarMassFilter ])

        # Make lines for sidebands. These are obtained by cloning the signal lines,
        # but applying different mass windows. Specifically:
        #  * We don't apply a narrow mass window to the D0 candidates. (They've already
        #    had the wide mass window applied in CombineParticles, so in the end we don't
        #    need to do anything.)
        #  * We apply a complicated 2D mass cut to the D* candidates, requiring them to lie
        #    in the 2D sidebands and NOT in the signalbox (aside from a small overlap for
        #    book-keeping purposes).
        
        # The 2D sideband cut looks like:
        #    [in wide D mass window AND DM sidebands] OR [in D mass sidebands and wide DM window]
        # The wide cuts (both on D mass and DM) have already been applied in CombineParticles so
        # we don't need to redo them explicitly.
        strCutInDMLowerSideband = '(MM - CHILD(MM,1) - CHILD(MM,2) > %(wideDMCutLower)s *MeV) & (MM - CHILD(MM,1) - CHILD(MM,2) < %(signalDMCutLower)s *MeV)' % self.getProps()
        strCutInDMUpperSideband = '(MM - CHILD(MM,1) - CHILD(MM,2) > %(signalDMCutUpper)s *MeV) & (MM - CHILD(MM,1) - CHILD(MM,2) < %(wideDMCutUpper)s *MeV)' % self.getProps()
        strCutInDMSidebands = '(' + strCutInDMLowerSideband + ' | ' + strCutInDMUpperSideband + ')'
        # For the sidebands, expand inwards by 2 MeV to give some overlap.
        cutMassSidebands_ppLL = self.getProps()['signalDMassCut_ppLL'] - self.getProps()['sidebandDMassOverlap']
        cutMassSidebands_ppDD = self.getProps()['signalDMassCut_ppDD'] - self.getProps()['sidebandDMassOverlap']
        cutMassSidebands_KpLL = self.getProps()['signalDMassCut_KpLL'] - self.getProps()['sidebandDMassOverlap']
        cutMassSidebands_KpDD = self.getProps()['signalDMassCut_KpDD'] - self.getProps()['sidebandDMassOverlap']
        cutMassSidebands_KKLL = self.getProps()['signalDMassCut_KKLL'] - self.getProps()['sidebandDMassOverlap']
        cutMassSidebands_KKDD = self.getProps()['signalDMassCut_KKDD'] - self.getProps()['sidebandDMassOverlap']
        strCutInMassSidebands_ppLL = "( (CHILD(ADMASS('D0'),1) > " + str(cutMassSidebands_ppLL) + "*MeV) & (CHILD(ADMASS('D0'),1) < %(wideDMassCut_ppLL)s *MeV) )" % self.getProps()
        strCutInMassSidebands_ppDD = "( (CHILD(ADMASS('D0'),1) > " + str(cutMassSidebands_ppDD) + "*MeV) & (CHILD(ADMASS('D0'),1) < %(wideDMassCut_ppDD)s *MeV) )" % self.getProps()
        strCutInMassSidebands_KpLL = "( (CHILD(ADMASS('D0'),1) > " + str(cutMassSidebands_KpLL) + "*MeV) & (CHILD(ADMASS('D0'),1) < %(wideDMassCut_KpLL)s *MeV) )" % self.getProps()
        strCutInMassSidebands_KpDD = "( (CHILD(ADMASS('D0'),1) > " + str(cutMassSidebands_KpDD) + "*MeV) & (CHILD(ADMASS('D0'),1) < %(wideDMassCut_KpDD)s *MeV) )" % self.getProps()
        strCutInMassSidebands_KKLL = "( (CHILD(ADMASS('D0'),1) > " + str(cutMassSidebands_KKLL) + "*MeV) & (CHILD(ADMASS('D0'),1) < %(wideDMassCut_KKLL)s *MeV) )" % self.getProps()
        strCutInMassSidebands_KKDD = "( (CHILD(ADMASS('D0'),1) > " + str(cutMassSidebands_KKDD) + "*MeV) & (CHILD(ADMASS('D0'),1) < %(wideDMassCut_KKDD)s *MeV) )" % self.getProps()
        # Combine to make 2D filter:
        strCutIn2DSidebands_ppLL = '(' + strCutInDMSidebands + ' | ' + strCutInMassSidebands_ppLL + ')'
        strCutIn2DSidebands_ppDD = '(' + strCutInDMSidebands + ' | ' + strCutInMassSidebands_ppDD + ')'
        strCutIn2DSidebands_KpLL = '(' + strCutInDMSidebands + ' | ' + strCutInMassSidebands_KpLL + ')'
        strCutIn2DSidebands_KpDD = '(' + strCutInDMSidebands + ' | ' + strCutInMassSidebands_KpDD + ')'
        strCutIn2DSidebands_KKLL = '(' + strCutInDMSidebands + ' | ' + strCutInMassSidebands_KKLL + ')'
        strCutIn2DSidebands_KKDD = '(' + strCutInDMSidebands + ' | ' + strCutInMassSidebands_KKDD + ')'

        # Make the sideband lines:
        lineBkg_ppLL = lineSig_ppLL.clone('DstarD2KSPiPi_KSLL_SB', prescale = self.getProps()['sidebandPrescale'], FilterDesktopFilterD0 = { "Code" : "(ALL)" }, FilterDesktopFilterDstar = { "Code" : strCutIn2DSidebands_ppLL })
        lineBkg_ppDD = lineSig_ppDD.clone('DstarD2KSPiPi_KSDD_SB', prescale = self.getProps()['sidebandPrescale'], FilterDesktopFilterD0 = { "Code" : "(ALL)" }, FilterDesktopFilterDstar = { "Code" : strCutIn2DSidebands_ppDD })
        lineBkg_KpLL = lineSig_KpLL.clone('DstarD2KSKPi_KSLL_SB',  prescale = self.getProps()['sidebandPrescale'], FilterDesktopFilterD0 = { "Code" : "(ALL)" }, FilterDesktopFilterDstar = { "Code" : strCutIn2DSidebands_KpLL })
        lineBkg_KpDD = lineSig_KpDD.clone('DstarD2KSKPi_KSDD_SB',  prescale = self.getProps()['sidebandPrescale'], FilterDesktopFilterD0 = { "Code" : "(ALL)" }, FilterDesktopFilterDstar = { "Code" : strCutIn2DSidebands_KpDD })
        lineBkg_KKLL = lineSig_KKLL.clone('DstarD2KSKK_KSLL_SB',   prescale = self.getProps()['sidebandPrescale'], FilterDesktopFilterD0 = { "Code" : "(ALL)" }, FilterDesktopFilterDstar = { "Code" : strCutIn2DSidebands_KKLL })
        lineBkg_KKDD = lineSig_KKDD.clone('DstarD2KSKK_KSDD_SB',   prescale = self.getProps()['sidebandPrescale'], FilterDesktopFilterD0 = { "Code" : "(ALL)" }, FilterDesktopFilterDstar = { "Code" : strCutIn2DSidebands_KKDD })

        # All done:
        lineList = [ lineSig_ppLL, lineSig_ppDD, lineSig_KpLL, lineSig_KpDD, lineSig_KKLL, lineSig_KKDD, lineBkg_ppLL, lineBkg_ppDD, lineBkg_KpLL, lineBkg_KpDD, lineBkg_KKLL, lineBkg_KKDD ]
        return lineList

    def makeDstarSignalFilter(self):
        # Cut on delta mass:
        strCutDeltaMassLower = "( MM - CHILD(MM,1) - CHILD(MM,2) > %(signalDMCutLower)s *MeV )" % self.getProps()
        strCutDeltaMassUpper = "( MM - CHILD(MM,1) - CHILD(MM,2) < %(signalDMCutUpper)s *MeV )" % self.getProps()
        # Combined 2D cut:
        strCut = '(' + strCutDeltaMassLower + ' & ' + strCutDeltaMassUpper + ')'
        # Filter:
        f = StrippingMember(FilterDesktop
                            , "FilterDstar"
                            , InputLocations = [ "%CombineDstar" ]
                            , Code = strCut)
        return f

    def makeDstar(self, cutPT, cutChi2):
        str_cutsSoftPi = '( PIDe-PIDpi < %(SoftPionCutPIDe)s )' % self.getProps()
        str_cutsDstarComb = "ADAMASS('D*(2010)+') < %(preFitDstarMassCut)s *MeV" % self.getProps()
        strDstarMotherCutBasepp = '(PT > ' + str(cutPT) + '*MeV) & (VFASPF(VCHI2/VDOF)<' + str(cutChi2) + ')'
        str_cutsDstar_DM_wide = '(MM - CHILD(MM,1) - CHILD(MM,2) > %(wideDMCutLower)s *MeV) & (MM - CHILD(MM,1) - CHILD(MM,2) < %(signalDMCutUpper)s *MeV)' % self.getProps()
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
        strCutChi2 = "(TRPCHI2 > %(pionFromDCutTRPCHI2)s )" % self.getProps()
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
        strCuts = '(' + strCutP + '&' + strCutPIDe + '&' + strCutPIDp \
            + '&' + strCutPIDK + ')'
        return strCuts

    # D0 -> KS pi+ pi- (for both KS -> LL and DD)
    def makeD2KSpp(self, preFitMassCut, postFitMassCut):
        strInputKS = '%FilterKS' # Pick up the KS list from earlier in the line
        strInputPi = 'StdLoosePions'
        strDecay = "D0 -> KS0 pi+ pi-"
        strCutsPi = self.makePionDaughterCutsForD()
        strCutsCombDauIP = "(AHASCHILD((ABSID=='pi+') & (MIPCHI2DV(PRIMARY)>44.0)))"
        strCutsCombMass  = "(ADAMASS('D0') < " + str(preFitMassCut) + "*MeV)"
        strCutsComb = '(' + strCutsCombDauIP + '&' + strCutsCombMass + ')'
        strDMotherCut_base  = '(BPVVDZ < 7000.0*mm) & (BPVDIRA > %(DCutDIRA)s )' % self.getProps()
        strDMotherCutpp = strDMotherCut_base + ' & (VFASPF(VCHI2) < %(DCutVtxChi2_pp)s )'  % self.getProps()
        strMassCut = "(ADMASS('D0')<" + str(postFitMassCut) + "*MeV)"
        strCutsMoth = '(' + strDMotherCutpp + ' & ' + strMassCut + ')'
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
        strCutsComb = '(' + strCutsCombDauIP + '&' + strCutsCombMass + ')'
        strDMotherCut_base  = '(BPVVDZ < 7000.0*mm) & (BPVDIRA > %(DCutDIRA)s )' % self.getProps()
        strDMotherCutKp = strDMotherCut_base + ' & (VFASPF(VCHI2) < %(DCutVtxChi2_Kp)s )'  % self.getProps()
        strMassCut = "(ADMASS('D0')<" + str(postFitMassCut) + "*MeV)"
        strCutsMoth = '(' + strDMotherCutKp + ' & ' + strMassCut + ')'
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
        strCutsComb = '(' + strCutsCombDauIP + '&' + strCutsCombMass + ')'
        strDMotherCut_base  = '(BPVVDZ < 7000.0*mm) & (BPVDIRA > %(DCutDIRA)s )' % self.getProps()
        strDMotherCutKK = strDMotherCut_base + ' & (VFASPF(VCHI2) < %(DCutVtxChi2_KK)s )'  % self.getProps()
        strMassCut = "(ADMASS('D0')<" + str(postFitMassCut) + "*MeV)"
        strCutsMoth = '(' + strDMotherCutKK + ' & ' + strMassCut + ')'
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
