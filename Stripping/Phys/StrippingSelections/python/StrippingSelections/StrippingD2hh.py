
__author__ = ['Marco Gersabeck','Harry Cliff']
__date__ = '02/09/2010'
__version__ = '$Revision: 1.4 $'

'''
Prompt D0->KK/Kpi stripping selection for measuring yCP.
'''

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import MeV, mm
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand

class StrippingD2hhConf(LHCbConfigurableUser):
    __slots__ = { 
                    'DaugPt'             : 500.*MeV
                  , 'DaugPtPID'          : 1100.*MeV   
                  , 'DaugP'              : 2000.*MeV
                  , 'DaugPPID'           : 5000.*MeV
                  , 'DaugTrkChi2'        : 10
                  , 'DaugTrkChi2PID'     : 3  
                  , 'DaugIPChi2'         : 9
                  , 'DaugMaxPID'         : 5 
                  #
                  , 'D0PtNoPID'          : 1500.*MeV
                  , 'D0PtPID'            : 1500.*MeV
                  , 'D0P'                : 5000.*MeV
                  , 'D0VtxChi2Ndof'      : 10
                  , 'D0FDChi2NoPID'      : 16
                  , 'D0FDChi2PID'        : 40
                  , 'D0BPVDira'          : 0.9999
                  , 'D0IPChi2NoPID'      : 100
                  , 'D0IPChi2PID'        : 30
                  , 'D0MassWindowCentre' : 1940.*MeV
                  , 'D0MassWindowWidth'  : 125.*MeV
                  , 'D0DOCA'             : 0.07*mm
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
	    StrippingD2hhConf._NoPIDLine = StrippingLine('D02hhNoPIDLine',
                                                         prescale = 0.0025,
                                                         algos = [D02hhNoPIDSel])

	return StrippingD2hhConf._NoPIDLine
     	
    def lineD02hhPID( self ):
        if None == StrippingD2hhConf._PIDLine:
            from StrippingConf.StrippingLine import StrippingLine
	    D02hhPIDSel = self.D02hhPIDCombPart()
	    StrippingD2hhConf._PIDLine = StrippingLine('D02hhPIDLine',
                                                       prescale = 1,
                                                       algos = [D02hhPIDSel])

	return StrippingD2hhConf._PIDLine
     	
    def lineD02hhControl( self ):
        if None == StrippingD2hhConf._ControlLine:
            from StrippingConf.StrippingLine import StrippingLine
	    D02hhControlSel = self.D02hhControlCombPart()
	    StrippingD2hhConf._ControlLine = StrippingLine('D02hhControlLine',
                                                           prescale = 0.00025,
                                                           algos = [D02hhControlSel])

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

        daugCuts = "(PT > %(DaugPtPID)s)" \
                   "& (ISLONG)" \
                   "& (P > %(DaugPPID)s)" \
                   "& (TRCHI2DOF < %(DaugTrkChi2PID)s)" % self.getProps()

        combCuts = "(APT > %(D0PtPID)s)" \
                   "& (AHASCHILD( PIDK > %(DaugMaxPID)s ) )" \
                   "& (ADAMASS(%(D0MassWindowCentre)s) < %(D0MassWindowWidth)s)" \
                   "&(ADOCA(1,2)< %(D0DOCA)s)"\
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
