
__author__ = ['Marco Gersabeck']
__date__ = '03/09/2010'
__version__ = '$Revision: 1.4 $'

'''
D0/D*+ cross-section lines
'''

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import MeV
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingD2hhNoPIDXsecConf(LHCbConfigurableUser):
    __slots__ = { 
                  'DaugPt'               : 250.*MeV
                  , 'DaugTrkChi2'        : 9
                  , 'DaugIPChi2'         : 9
                  #
                  , 'D0FDChi2'           : 16
                  , 'D0BPVDira'          : 0.9999
                  , 'D0IPChi2'           : 100
                  , 'D0MassWindow1'      : 80.*MeV
                  , 'D0MassWindow2'      : 75.*MeV
                  #
                  , 'DeltaMassWindow1'   : 160.*MeV
                  , 'DeltaMassWindow2'   : 155.*MeV
                  , 'DstarVtxChi2'       : 100
                }

    _D0Line = None
    _DstarLine = None

    def lines(self) :
        """
        The the main user method that returns a list of the StrippingLine
        objects defined and configured by this configurable.
        """
        lclLineD0 = self.lineD0()
        lclLineDstar = self.lineDstar()

        lineList = [ lclLineD0, lclLineDstar ]

        return lineList

    def lineD0( self ):
        if None == StrippingD2hhNoPIDXsecConf._D0Line:
            from StrippingConf.StrippingLine import StrippingLine
	    D0Sel = self.D0CombPart('SelD02hhNoPIDXsec')
	    D0Seq = SelectionSequence("SeqD02hhNoPIDXsec", TopSelection = D0Sel)
	    StrippingD2hhNoPIDXsecConf._D0Line = StrippingLine('D02hhNoPIDXsecLine', prescale = 1, algos = [D0Seq])

	return StrippingD2hhNoPIDXsecConf._D0Line
     	
    def lineDstar( self ):
        if None == StrippingD2hhNoPIDXsecConf._DstarLine:
            from StrippingConf.StrippingLine import StrippingLine
	    DstarSel = self.DstarCombPart()
	    DstarSeq = SelectionSequence("SeqDstarD02hhNoPIDXsec", TopSelection = DstarSel)
	    StrippingD2hhNoPIDXsecConf._DstarLine = StrippingLine('DstarD02hhNoPIDXsecLine', prescale = 1, algos = [DstarSeq])

	return StrippingD2hhNoPIDXsecConf._DstarLine
     	
    def D0CombPart( self, name ):
        from Configurables import CombineParticles
        pions = DataOnDemand(Location = "Phys/StdNoPIDsPions")
        kaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")

        daugCuts = "(PT > %(DaugPt)s)" \
                   "& (TRCHI2DOF < %(DaugTrkChi2)s)" \
                   "& (MIPCHI2DV(PRIMARY) > %(DaugIPChi2)s)" % self.getProps()

        combCuts = "(ADAMASS('D0') < %(D0MassWindow1)s)" % self.getProps()

        d0Cuts = "(ADMASS('D0') < %(D0MassWindow2)s)" \
                 "& (BPVVDCHI2 > %(D0FDChi2)s)" \
                 "& (BPVDIRA > %(D0BPVDira)s)" \
                 "& (BPVIPCHI2()<%(D0IPChi2)s)" % self.getProps()

        # D0 combine
        from Configurables import CombineParticles
        D0Combine = CombineParticles( "CombD2hhNoPIDXsec" )
        D0Combine.DecayDescriptors = [ "D0 -> pi+ pi-", "D0 -> K- pi+", "D0 -> K+ pi-", "D0 -> K+ K-" ]
        D0Combine.DaughtersCuts = {
          "pi+" : daugCuts,
          "K+" : daugCuts
        }
        D0Combine.CombinationCut = combCuts
        D0Combine.MotherCut = d0Cuts

	D0Sel = Selection(name,
                 	  Algorithm = D0Combine,
                 	  RequiredSelections = [pions,kaons]
			 )

	return D0Sel

    def DstarCombPart( self ):
        from Configurables import CombineParticles
        pions = DataOnDemand(Location = "Phys/StdNoPIDsPions")
        kaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")

        daugCuts = "(TRCHI2DOF < %(DaugTrkChi2)s)" % self.getProps()

        combCuts = "((AM - AM1) < %(DeltaMassWindow1)s)" % self.getProps()

        dstarCuts = "(VFASPF(VCHI2) < %(DstarVtxChi2)s)" \
                    "& ((M - M1) < %(DeltaMassWindow2)s)" % self.getProps()

        # D* combine
        DstarCombine = CombineParticles( "CombStarD2hhNoPIDXsec" )
        DstarCombine.DecayDescriptors = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
        DstarCombine.DaughtersCuts = {
          "pi+" : daugCuts
        }
        DstarCombine.CombinationCut = combCuts
        DstarCombine.MotherCut = dstarCuts

        # D0 and D* selections
        from PhysSelPython.Wrappers import Selection
	D0Sel = self.D0CombPart('SelD02hhForDstarNoPIDXsec')
        DstarSel = Selection("SelDstarD2hhNoPIDXsec", 
                             Algorithm = DstarCombine, 
                             RequiredSelections = [ pions, kaons, D0Sel ]
                            )

	return DstarSel

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
