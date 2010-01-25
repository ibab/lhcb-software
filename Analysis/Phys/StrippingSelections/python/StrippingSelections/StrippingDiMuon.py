# $Id: StrippingDiMuon.py,v 1.3 2010-01-25 20:10:27 gcowan Exp $
## #####################################################################
# A stripping selection for inclusive J/psi(1S) -> mu+ mu- decays
#
# @authors G.Lanfranchi, A.Sarti 
# @date 2009-Sept-1
# 
## #####################################################################

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingDiMuonConf(LHCbConfigurableUser):
    """
    Definition of Jpsi -> mu mu stripping.
    """
    __slots__ = {
			"MuonPT":		500.	# MeV
		,	"MuonPTLoose":		500.	# MeV
		,	"MuonTRCHI2":		3.	# adimensional
		,	"MuonTRCHI2Loose":	10.	# adimensional
		,	"JpsiAM":		2700.	# MeV
		,	"JpsiAMLoose":		2700.	# MeV
		,	"JpsiVCHI2":		20.	# adimensional
		,	"JpsiVCHI2Loose":	20.	# adimensional
		}

    def nominal_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	JpsiSel = self.Jpsi()
	JpsiSeq = SelectionSequence("SeqDiMuonInc", TopSelection = JpsiSel)
	return StrippingLine('DiMuonIncLine', prescale = 1, algos = [JpsiSeq])   
     	
    def loose_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	JpsiSel = self.JpsiLoose()
	JpsiSeq = SelectionSequence("SeqDiMuonIncLoose", TopSelection = JpsiSel)
	return StrippingLine('DiMuonIncLooseLine', prescale = 1, algos = [JpsiSeq])   

    def Jpsi( self ):
	_muons =  DataOnDemand('stdLooseMuons', Location = 'StdLooseMuons')
	mucut = '(PT > %(MuonPT)s *MeV) & (TRCHI2DOF < %(MuonTRCHI2)s) & (ISLONG)' % self.getProps()
	_Jpsi = CombineParticles("DiMuonIncCombine",
                         DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
                         DaughtersCuts = {'mu+': mucut}, 
                         CombinationCut = " (AM > %(JpsiAM)s *MeV)" % self.getProps(),
                         MotherCut = "(VFASPF(VCHI2/VDOF) < %(JpsiVCHI2)s)" % self.getProps(),
                         WriteP2PVRelations = False
                         )
	Jpsi = Selection("SelDiMuonInc",
                  Algorithm = _Jpsi,
                  RequiredSelections = [_muons]
                  )
	return Jpsi
	
    def JpsiLoose( self ):
	_muons =  DataOnDemand('stdVeryLooseMuons', Location = 'Phys/StdVeryLooseMuons')
	mucut = '(PT > %(MuonPTLoose)s *MeV) & (TRCHI2DOF < %(MuonTRCHI2Loose)s) & (ISLONG)' % self.getProps()
	_Jpsi = CombineParticles("DiMuonIncCombineLoose",
                         DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
                         DaughtersCuts = {'mu+': mucut},
                         CombinationCut = " (AM > %(JpsiAMLoose)s *MeV)" % self.getProps(),
                         MotherCut = "(VFASPF(VCHI2/VDOF) < %(JpsiVCHI2Loose)s)" % self.getProps(),
                         WriteP2PVRelations = False
                         )
	Jpsi = Selection("SelDiMuonInciLoose",
                  Algorithm = _Jpsi,
                  RequiredSelections = [_muons]
                  )
	return Jpsi

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
