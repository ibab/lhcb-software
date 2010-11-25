__author__ = ['Harry Cliff','Vladimir Gligorov']
__date__ = '18/10/2010'
__version__ = '$Revision: 1.0 $'

'''
Prompt untagged lifetime unbiased D0->KK/Kpi stripping selection
for running on MB/NB events. For calibration of IP cuts for 2011.
'''

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import MeV, mm, ns
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand

class StrippingD2hhLTUnbiasedMBNBConf(LHCbConfigurableUser):
    __slots__ = { 
                    'DaugPt'             : 750.*MeV
                  , 'DaugTrkChi2'        : 3
                  , 'DaugKaonPIDK'       : 5 
                  , 'DaugPionPIDK'       : 5
                  , 'D0Pt'               : 2500.*MeV
                  , 'D0P'                : 5000.*MeV
                  , 'D0VtxChi2Ndof'      : 10
                  , 'D0MassWindowWidth'  : 50.*MeV 
                  , 'D0DOCA'             : 0.1*mm
                  , 'D0ProperTime'       : 0.00015*ns  
                }

    _LTUnbiasedLine = None

    def lines(self) :
        """
        The the main user method that returns a list of the StrippingLine
        objects defined and configured by this configurable.
        """
        lclLineLTUnbiased = self.lineD2hhLTUnbiasedMBNB()

        lineList = [ lclLineLTUnbiased ]

        return lineList

    def linesMainOnly(self) :
        """
        The the main user method that returns a list of the StrippingLine
        objects defined and configured by this configurable.
        """
        lclLineLTUnbiased = self.lineD2hhLTUnbiasedMBNB()

        lineList = [ lclLineLTUnbiased ]

        return lineList

    def lineD2hhLTUnbiasedMBNB( self ):
        if None == StrippingD2hhLTUnbiasedMBNBConf._LTUnbiasedLine:
            from StrippingConf.StrippingLine import StrippingLine
            D2hhLTUnbiasedMBNBSel = self.D2hhLTUnbiasedMBNBCombPart()
            thisline = StrippingLine(   'D2hhLTUnbiasedMBNBLine', \
                                        HLT = "HLT_PASS_RE('Hlt1MB.*')", \
                                        prescale = 1, \
                                        algos = [D2hhLTUnbiasedMBNBSel])
            StrippingD2hhLTUnbiasedMBNBConf._LTUnbiasedLine = thisline

        return StrippingD2hhLTUnbiasedMBNBConf._LTUnbiasedLine
        
    def D2hhLTUnbiasedMBNBCombPart( self ):
        from Configurables import CombineParticles
        stdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
        stdNoPIDsPions = DataOnDemand(Location = "Phys/StdNoPIDsPions")

        kaonCuts = "(PT > %(DaugPt)s) " \
                   "& (TRCHI2DOF < %(DaugTrkChi2)s) " \
                   "& (PIDK > %(DaugKaonPIDK)s)" % self.getProps()

        pionCuts = "(PT > %(DaugPt)s) " \
                   "& (TRCHI2DOF < %(DaugTrkChi2)s) " \
                   "& (PIDK < %(DaugPionPIDK)s)" % self.getProps()

        combCuts = "(ADAMASS('D0') < %(D0MassWindowWidth)s) "\
                   "& (AP > %(D0P)s) " \
                   "& (APT > %(D0Pt)s) " \
                   "& (ADOCA(1,2) < %(D0DOCA)s)" % self.getProps()

        d0Cuts = "(VFASPF(VCHI2/VDOF) < %(D0VtxChi2Ndof)s) " \
                 "& (BPVLTIME() > %(D0ProperTime)s)" % self.getProps()

        comb = CombineParticles( 'CombineD2hhLTUnbiasedMBNB' )
        comb.DecayDescriptors = ['[D0 -> K- pi+]cc', 'D0 -> K+ K-']
        comb.DaughtersCuts = { 'K+' : kaonCuts , 'pi+' : pionCuts }
        comb.CombinationCut = combCuts
        comb.MotherCut = d0Cuts

        D0Sel = Selection("D2hhLTUnbiasedMBNB",
                      Algorithm = comb,
                      RequiredSelections = [stdNoPIDsKaons, stdNoPIDsPions]
             )

        return D0Sel

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
