
__author__ = ['Marco Gersabeck','Harry Cliff']
__date__ = '13/08/2010'
__version__ = '$Revision: 1.2 $'

'''
Prompt D0->KK/Kpi stripping selection for measuring yCP.
'''

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import MeV
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingD2hhConf(LHCbConfigurableUser):
    __slots__ = { 
                  'DaugPt'               : 500.*MeV
                  , 'DaugP'              : 2000.*MeV
                  , 'DaugTrkChi2'        : 10
                  , 'DaugIPChi2'         : 9
                  , 'DaugMaxPID'         : 5 
                  #
                  , 'D0PtNoPID'          : 1500.*MeV
                  , 'D0PtPID'            : 1000.*MeV
                  , 'D0P'                : 5000.*MeV
                  , 'D0VtxChi2Ndof'      : 10
                  , 'D0FDChi2NoPID'      : 16
                  , 'D0FDChi2PID'        : 35
                  , 'D0BPVDira'          : 0.9999
                  , 'D0IPChi2NoPID'      : 100
                  , 'D0IPChi2PID'        : 9
                  , 'D0MassWindowCentre' : 1940.*MeV
                  , 'D0MassWindowWidth' : 125.*MeV
                }

    _NoPIDLine = None
    _PIDLine = None
    _ControlLine = None

    def lines(self) :
        """
        The the main user method that returns a list of the StrippingLine
        objects defined and configured by this configurable.
        """
        lclLineNoPID = self.lineD02hhNoPID()
        lclLinePID = self.lineD02hhPID()
        lclLineCtrl = self.lineD02hhControl()

        lineList = [ lclLineNoPID, lclLinePID, lclLineCtrl ]

        return lineList

    def linesMainOnly(self) :
        """
        The the main user method that returns a list of the StrippingLine
        objects defined and configured by this configurable.
        """
        lclLineNoPID = self.lineD02hhNoPID()
        lclLinePID = self.lineD02hhPID()

        lineList = [ lclLineNoPID, lclLinePID ]

        return lineList

    def lineD02hhNoPID( self ):
        if None == StrippingD2hhConf._NoPIDLine:
            from StrippingConf.StrippingLine import StrippingLine
	    D02hhNoPIDSel = self.D02hhNoPIDCombPart()
	    D02hhNoPIDSeq = SelectionSequence("SeqD02hhNoPID", TopSelection = D02hhNoPIDSel)
	    StrippingD2hhConf._NoPIDLine = StrippingLine('D02hhNoPIDLine', prescale = 1, algos = [D02hhNoPIDSeq])

	return StrippingD2hhConf._NoPIDLine
     	
    def lineD02hhPID( self ):
        if None == StrippingD2hhConf._PIDLine:
            from StrippingConf.StrippingLine import StrippingLine
	    D02hhPIDSel = self.D02hhPIDCombPart()
	    D02hhPIDSeq = SelectionSequence("SeqD02hhPID", TopSelection = D02hhPIDSel)
	    StrippingD2hhConf._PIDLine = StrippingLine('D02hhPIDLine', prescale = 1, algos = [D02hhPIDSeq])

	return StrippingD2hhConf._PIDLine
     	
    def lineD02hhControl( self ):
        if None == StrippingD2hhConf._ControlLine:
            from StrippingConf.StrippingLine import StrippingLine
	    D02hhControlSel = self.D02hhControlCombPart()
	    D02hhControlSeq = SelectionSequence("SeqD02hhControl", TopSelection = D02hhControlSel)
	    StrippingD2hhConf._ControlLine = StrippingLine('D02hhControlLine', prescale = 0.01, algos = [D02hhControlSeq])

	return StrippingD2hhConf._ControlLine
     	
    def D02hhNoPIDCombPart( self ):
        from Configurables import CombineParticles
	stdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")

        daugCuts = "(PT > %(DaugPt)s)" \
                   "& (ISLONG)" \
                   "& (P > %(DaugP)s)" \
                   "& (TRCHI2DOF < %(DaugTrkChi2)s)" \
                   "& (MIPCHI2DV(PRIMARY) > %(DaugIPChi2)s)" % self.getProps()

        combCuts = "(APT > %(D0PtNoPID)s)" \
                   "& (ADAMASS(%(D0MassWindowCentre)s) < %(D0MassWindowWidth)s)" \
                   "& (AP > %(D0P)s)" % self.getProps()

        d0Cuts = "(VFASPF(VCHI2PDOF) < %(D0VtxChi2Ndof)s)" \
                 "& (BPVVDCHI2 > %(D0FDChi2NoPID)s)" \
                 "& (BPVDIRA > %(D0BPVDira)s)" \
                 "& (BPVIPCHI2()<%(D0IPChi2NoPID)s)" % self.getProps()

        comb = CombineParticles( 'CombineD02hhNoPID' )
        comb.DecayDescriptor = 'D0 -> K- K+'
        comb.DaughtersCuts = { 'K+' : daugCuts }
        comb.CombinationCut = combCuts
        comb.MotherCut = d0Cuts

	D0Sel = Selection("D02hhNoPID",
                 	  Algorithm = comb,
                 	  RequiredSelections = [stdNoPIDsKaons]
			 )

	return D0Sel

    def D02hhPIDCombPart( self ):
        from Configurables import CombineParticles
	stdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")

        daugCuts = "(PT > %(DaugPt)s)" \
                   "& (ISLONG)" \
                   "& (P > %(DaugP)s)" \
                   "& (TRCHI2DOF < %(DaugTrkChi2)s)" % self.getProps()

        combCuts = "(APT > %(D0PtPID)s)" \
                   "& (AHASCHILD( PIDK > %(DaugMaxPID)s ) )" \
                   "& (ADAMASS(%(D0MassWindowCentre)s) < %(D0MassWindowWidth)s)" \
                   "& (AP > %(D0P)s)" % self.getProps()

        d0Cuts = "(VFASPF(VCHI2PDOF) < %(D0VtxChi2Ndof)s)" \
                 "& (BPVVDCHI2 > %(D0FDChi2PID)s)" \
                 "& (BPVDIRA > %(D0BPVDira)s)" \
                 "& (BPVIPCHI2()<%(D0IPChi2PID)s)" % self.getProps()

        comb = CombineParticles( 'CombineD02hhPID' )
        comb.DecayDescriptor = 'D0 -> K- K+'
        comb.DaughtersCuts = { 'K+' : daugCuts }
        comb.CombinationCut = combCuts
        comb.MotherCut = d0Cuts

	D0Sel = Selection("D02hhPID",
                 	  Algorithm = comb,
                 	  RequiredSelections = [stdNoPIDsKaons]
			 )

	return D0Sel

    def D02hhControlCombPart( self ):
        from Configurables import CombineParticles
	stdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")

        daugCuts = "(PT > %(DaugPt)s)" \
                   "& (ISLONG)" \
                   "& (P > %(DaugP)s)" \
                   "& (TRCHI2DOF < %(DaugTrkChi2)s)" % self.getProps()

        combCuts = "(APT > %(D0PtNoPID)s)" \
                   "& (ADAMASS(%(D0MassWindowCentre)s) < %(D0MassWindowWidth)s)" \
                   "& (AP > %(D0P)s)" % self.getProps()

        d0Cuts = "(VFASPF(VCHI2PDOF) < %(D0VtxChi2Ndof)s)" \
                 "& (BPVVDCHI2 > %(D0FDChi2NoPID)s)" \
                 "& (BPVDIRA > %(D0BPVDira)s)" \
                 "& (BPVIPCHI2()<%(D0IPChi2NoPID)s)" % self.getProps()

        comb = CombineParticles( 'CombineD02hhNoPID' )
        comb.DecayDescriptor = 'D0 -> K- K+'
        comb.DaughtersCuts = { 'K+' : daugCuts }
        comb.CombinationCut = combCuts
        comb.MotherCut = d0Cuts

	D0Sel = Selection("D02hhControl",
                 	  Algorithm = comb,
                 	  RequiredSelections = [stdNoPIDsKaons]
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
