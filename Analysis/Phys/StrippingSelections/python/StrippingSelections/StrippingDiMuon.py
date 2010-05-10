# $Id: StrippingDiMuon.py,v 1.5 2010-05-10 11:58:49 jpalac Exp $
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
                ,       "DiMuonFDCHI2" :        10.     # adimensional
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
	_muons =  DataOnDemand(Location = 'Phys/StdLooseMuons')
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
                  RequiredSelections = [ _muons ]
                  )
	return Jpsi
	
    def JpsiLoose( self ):
	_muons =  DataOnDemand(Location = 'Phys/StdVeryLooseMuons')
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

#########################################################################################
# Biased DiMuon adapted from Antonio Perez-Calero, implemented by P. Koppenburg
#
    def biasedDiMuon_line( self ):
        """
        Biased DiMuon adapted from Antonio Perez-Calero, implemented by P. Koppenburg

        This will need to be tightened
        
        @author P. Koppenburg
        @date 17/2/2010
        """
        from StrippingConf.StrippingLine import StrippingLine
	_muons =  DataOnDemand(Location = 'Phys/StdVeryLooseDiMuon')
        _diMu = FilterDesktop("FilterForB2DiMuon")
        _diMu.Code = "(MAXTREE('mu+'==ABSID,PT)>1*GeV) & (VFASPF(VCHI2/VDOF)<20) & (BPVVDCHI2> %(DiMuonFDCHI2)s )" % self.getProps()

        s = Selection("SelB2DiMuon",
                         Algorithm = _diMu,
                         RequiredSelections = [ _muons ] )
	ss = SelectionSequence("SeqSelB2DiMuon", TopSelection = s )
	return StrippingLine('SelB2DiMuonLine', prescale = 1, algos = [ ss ])   

#########################################################################################
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d

