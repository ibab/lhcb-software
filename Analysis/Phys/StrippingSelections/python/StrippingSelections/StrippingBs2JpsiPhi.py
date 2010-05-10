# $Id: StrippingBs2JpsiPhi.py,v 1.6 2010-05-10 11:58:49 jpalac Exp $

__author__ = ['Greig Cowan']
__date__ = '24/01/2010'
__version__ = '$Revision: 1.6 $'

'''
Bs->JpsiPhi lifetime unbiased stripping selection using LoKi::Hybrid and
python configurables. PV refitting is done. Implements the nominal and 
loose lifetime unbiased stripping selections.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingBs2JpsiPhiConf(LHCbConfigurableUser):
    """
    Definition of nominal Bs->JpsiPhi stripping. 
    """
    __slots__ = { 
			"MuonTRCHI2Loose"	: 10.0	# adimensional 
		,	"JpsiMassWinLoose"	: 80.0	# MeV
		,	"JpsiVCHI2Loose"	: 20.0	# adimensional
		,	"KaonTRCHI2Loose"	: 10.0	# adimensional  
		,	"KaonPIDK"	: -5.0	# adimensional  
		,	"PhiMassWin"	: 15.0	# MeV
		,	"PhiMassWinLoose"	: 30.0	# MeV
		,	"PhiPT" 	: 1000.0# MeV
		,	"PhiPTLoose" 	: 500.0	# MeV
		,	"PhiVCHI2" 	: 20.0	# adimensional
		,	"PhiVCHI2Loose" 	: 20.0	# adimensional
		,	"BsMassWin"	: 300.0	# MeV
		,	"BsMassWinLoose"	: 300.0	# MeV
		,	"BsVCHI2" 	: 5.0	# adimensional
		,	"BsVCHI2Loose" 	: 20.0	# adimensional
                 }
    
    def nominal_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Bs2JpsiPhiSel = self.Bs2JpsiPhi()
	Bs2JpsiPhiSeq = SelectionSequence("SeqBs2JpsiPhi", TopSelection = Bs2JpsiPhiSel)
	return StrippingLine('Bs2JpsiPhiLine', prescale = 1, algos = [Bs2JpsiPhiSeq])   
     	
    def loose_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Bs2JpsiPhiSel = self.Bs2JpsiPhiLoose()
	Bs2JpsiPhiSeq = SelectionSequence("SeqBs2JpsiPhiLoose", TopSelection = Bs2JpsiPhiSel)
	return StrippingLine('Bs2JpsiPhiLooseLine', prescale = 1, algos = [Bs2JpsiPhiSeq])   
     
    def Jpsi2MuMuLoose( self ):
	StdVeryLooseJpsi2MuMu = DataOnDemand(Location = "Phys/StdVeryLooseJpsi2MuMu")
	_JpsiFilter = FilterDesktop("JpsiFilterForBs2JpsiPhiLoose")
	_JpsiFilter.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2Loose)s)" \
        	           "& (ADMASS('J/psi(1S)') < %(JpsiMassWinLoose)s *MeV)" \
        	           "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2Loose)s)" % self.getProps()

	Jpsi = Selection("Jpsi2MuMuForBs2JpsiPhiLoose",
			Algorithm = _JpsiFilter,
			RequiredSelections = [StdVeryLooseJpsi2MuMu])
	return Jpsi

    def Phi2KKLoose( self ):
	StdLoosePhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK")
	_phiFilter = FilterDesktop("PhiFilterForBs2JpsiPhiLoose")
	_phiFilter.Code = "  (MAXTREE('K+'==ABSID, TRCHI2DOF) < %(KaonTRCHI2Loose)s)" \
                	  "& (ADMASS('phi(1020)') < %(PhiMassWinLoose)s *MeV)" \
                  	  "& (PT > %(PhiPTLoose)s *MeV)" \
                  	  "& (VFASPF(VCHI2/VDOF) < %(PhiVCHI2Loose)s)" % self.getProps()

	Phi =  Selection ("Phi2KKForBs2JpsiPhiLoose",
                 Algorithm = _phiFilter,
                 RequiredSelections = [StdLoosePhi2KK])
	return Phi	

    def Phi2KK( self ):
	StdLoosePhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK")
	_phiFilter = FilterDesktop("PhiFilterForBs2JpsiPhi")
	_phiFilter.Code = "  (MINTREE('K+'==ABSID, PIDK) > %(KaonPIDK)s)" \
                	  "& (ADMASS('phi(1020)') < %(PhiMassWin)s *MeV)" \
                  	  "& (PT > %(PhiPT)s *MeV)" \
                  	  "& (VFASPF(VCHI2/VDOF) < %(PhiVCHI2)s)" % self.getProps()

	Phi =  Selection ("Phi2KKForBs2JpsiPhi",
                 Algorithm = _phiFilter,
                 RequiredSelections = [StdLoosePhi2KK])
	return Phi	

    def Bs2JpsiPhiLoose( self ):
	Jpsi = self.Jpsi2MuMuLoose()
	Phi = self.Phi2KKLoose()
	_Bs = CombineParticles("Bs2JpsiPhiLoose")
      	_Bs.DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)"
        _Bs.CombinationCut = "ADAMASS('B_s0') < %(BsMassWinLoose)s *MeV" % self.getProps()
        _Bs.MotherCut = "(VFASPF(VCHI2/VDOF) < %(BsVCHI2Loose)s)" % self.getProps()
        _Bs.ReFitPVs = True
	# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Phi
	_Bs.addTool( OfflineVertexFitter() )
	_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	_Bs.OfflineVertexFitter.useResonanceVertex = False

	Bs = Selection("SelBs2JpsiPhiLoose",
                 	Algorithm = _Bs,
                 	RequiredSelections = [Jpsi, Phi])
	return Bs

    def Bs2JpsiPhi( self ):
        Jpsi = DataOnDemand(Location = "Phys/StdLTUnbiasedJpsi2MuMu")
	Phi = self.Phi2KK()
	_Bs = CombineParticles("Bs2JpsiPhi")
     	_Bs.DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)"
        _Bs.CombinationCut = "ADAMASS('B_s0') < %(BsMassWin)s *MeV" % self.getProps()
        _Bs.MotherCut = "(VFASPF(VCHI2/VDOF) < %(BsVCHI2)s)" % self.getProps()
        _Bs.ReFitPVs = True
	# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Phi
	_Bs.addTool( OfflineVertexFitter() )
	_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	_Bs.OfflineVertexFitter.useResonanceVertex = False

	Bs = Selection("SelBs2JpsiPhi",
                 	Algorithm = _Bs,
                 	RequiredSelections = [Jpsi, Phi])
	return Bs

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
