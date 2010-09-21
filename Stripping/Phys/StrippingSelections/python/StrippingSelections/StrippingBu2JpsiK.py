# $Id: StrippingBu2JpsiK.py,v 1.5 2010-05-10 11:58:49 jpalac Exp $

__author__ = ['Greig Cowan']
__date__ = '24/01/2010'
__version__ = '$Revision: 1.5 $'

'''
Bu->JpsiK lifetime unbiased stripping selection using LoKi::Hybrid and
python configurables. PV refitting is done. Implements the nominal 
and loose stripping selections.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingBu2JpsiKConf(LHCbConfigurableUser):
    """
    Definition of Bu->JpsiK stripping.
    """
    __slots__ = { 
               		"MuonTRCHI2Loose"	: 10.0	# adimensional 
		,	"MuonPTLoose"		: 250.0	# MeV 
		,	"JpsiMassWinLoose"	: 80.0	# MeV
		,	"JpsiVCHI2Loose"	: 20.0	# adimensional
		,	"JpsiPTLoose"		: 500.0	# MeV
		,	"KaonPIDKMinusPIDp"	: -6.0	# adimensional  
		,	"KaonPIDK"		: -5.0	# adimensional  
		,	"KaonTRCHI2"		: 4.0	# adimensional  
		,	"KaonTRCHI2Loose"	: 10.0	# adimensional  
		,	"KaonPT"		: 1300.0# # MeV  
		,	"KaonPTLoose"		: 250.0	# MeV  
		,	"KaonP"			: 10000.0# MeV  
		,	"BuMassWin"		: 300.0	# MeV
		,	"BuMassWinLoose"	: 300.0	# MeV
		,	"BuVCHI2" 		: 5.0	# adimensional
		,	"BuVCHI2Loose" 		: 20.0	# adimensional
          }
    
    def nominal_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Bu2JpsiKSel = self.Bu2JpsiK()
	Bu2JpsiKSeq = SelectionSequence("SeqBu2JpsiK", TopSelection = Bu2JpsiKSel)
	return StrippingLine('Bu2JpsiKLine', prescale = 1, algos = [Bu2JpsiKSeq])   
     	
    def loose_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Bu2JpsiKSel = self.Bu2JpsiKLoose()
	Bu2JpsiKSeq = SelectionSequence("SeqBu2JpsiKLoose", TopSelection = Bu2JpsiKSel)
	return StrippingLine('Bu2JpsiKLooseLine', prescale = 1, algos = [Bu2JpsiKSeq])   
     	
    def Jpsi2MuMuLoose( self ):
        StdVeryLooseJpsi2MuMu = DataOnDemand(Location = "Phys/StdVeryLooseJpsi2MuMu")
	_JpsiFilter = FilterDesktop("JpsiFilterForBu2JpsiKLoose")
	_JpsiFilter.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2Loose)s)" \
        	           "& (PT > %(JpsiPTLoose)s *MeV)" \
        	           "& (ADMASS('J/psi(1S)') < %(JpsiMassWinLoose)s *MeV)" \
        	           "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2Loose)s)" % self.getProps()

	Jpsi = Selection("Jpsi2MuMuForBu2JpsiKLoose",
			Algorithm = _JpsiFilter,
			RequiredSelections = [StdVeryLooseJpsi2MuMu])
	return Jpsi

    def Bu2JpsiK( self ):
	Jpsi = DataOnDemand(Location = "Phys/StdLTUnbiasedJpsi2MuMu")
	StdLooseKaons = DataOnDemand(Location = "Phys/StdLooseKaons")
	_Bu = CombineParticles("Bu2JpsiK")
      	_Bu.DecayDescriptor = "[B+ -> J/psi(1S) K+]cc"
	_Bu.DaughtersCuts = {"K+": "  ((PIDK - PIDp) > %(KaonPIDKMinusPIDp)s)"\
 				   "& (PIDK > %(KaonPIDK)s)"\
                          	   "& (TRCHI2DOF < %(KaonTRCHI2)s)"\
                         	   "& (PT > %(KaonPT)s *MeV)"\
                        	   "& (P > %(KaonP)s *MeV)" % self.getProps()}
 	_Bu.CombinationCut = "ADAMASS('B+') < %(BuMassWin)s *MeV" % self.getProps()
        _Bu.MotherCut = "(VFASPF(VCHI2/VDOF) < %(BuVCHI2)s)" % self.getProps()
        _Bu.ReFitPVs = True
	_Bu.addTool( OfflineVertexFitter() )
	_Bu.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	_Bu.OfflineVertexFitter.useResonanceVertex = False

	Bu = Selection("SelBu2JpsiK",
                 	Algorithm = _Bu,
                 	RequiredSelections = [Jpsi, StdLooseKaons])
	return Bu

    def Bu2JpsiKLoose( self ):
	Jpsi = self.Jpsi2MuMuLoose()
	StdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
	_Bu = CombineParticles("Bu2JpsiKLoose",
        	               DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
			       DaughtersCuts = {"K+": "(PT > %(KaonPTLoose)s *MeV) & (TRCHI2DOF < %(KaonTRCHI2Loose)s)" % self.getProps()},
                	       CombinationCut = "ADAMASS('B+') < %(BuMassWinLoose)s *MeV" % self.getProps(),
              		       MotherCut = "(VFASPF(VCHI2/VDOF) < %(BuVCHI2Loose)s)" % self.getProps(),
                       	       ReFitPVs = True)
	Bu = Selection("SelBu2JpsiKLoose",
                 	Algorithm = _Bu,
                 	RequiredSelections = [Jpsi, StdNoPIDsKaons])
	return Bu

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
