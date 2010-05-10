# $Id: StrippingBd2JpsiKstar.py,v 1.5 2010-05-10 11:58:49 jpalac Exp $

__author__ = ['Greig Cowan']
__date__ = '24/01/2010'
__version__ = '$Revision: 1.5 $'

'''
Bd->JpsiKstar lifetime unbiased stripping selection using LoKi::Hybrid and
python configurables. PV refitting is done. Implements the nominal
and looser stripping selections.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingBd2JpsiKstarConf(LHCbConfigurableUser):
    """
    Definition of Bd->JpsiKstar stripping.
    """
    __slots__ = { 
    			"MuonTRCHI2Loose"	: 10.0	# adimensional 
		,	"MuonPTLoose"		: 250.0	# MeV 
		,	"JpsiMassWinLoose"	: 80.0	# MeV
		,	"JpsiVCHI2Loose"	: 20.0	# adimensional
		,	"JpsiPTLoose"		: 500.0	# MeV
		,	"KaonPIDKMinusPIDp"	: -6.0	# adimensional  
		,	"KaonPIDK"		: -5.0	# adimensional  
		,	"KaonTRCHI2Loose"	: 10.0	# adimensional  
		,	"PionTRCHI2Loose"	: 10.0	# adimensional   
		,	"KstarMassWin"		: 90.0	# MeV 
		,	"KstarMassWinLoose"	: 300.0	# MeV
		,	"KstarPT"		: 1000. # MeV
		,	"KstarPTLoose" 		: 500.0	# MeV
		,	"KstarVCHI2" 		: 20.0	# adimensional
		,	"KstarVCHI2Loose" 	: 20.0	# adimensional
		,	"BdMassWin"		: 300.0	# MeV
		,	"BdMassWinLoose"	: 300.0	# MeV
		,	"BdVCHI2" 		: 5.0	# adimensional
		,	"BdVCHI2Loose" 		: 20.0	# adimensional
		,	"BdPT"	 		: 2000.	# adimensional

             }
    
    def nominal_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Bd2JpsiKstarSel = self.Bd2JpsiKstar()
	Bd2JpsiKstarSeq = SelectionSequence("SeqBd2JpsiKstar", TopSelection = Bd2JpsiKstarSel)
	return StrippingLine('Bd2JpsiKstarLine', prescale = 1, algos = [Bd2JpsiKstarSeq])   
     	
    def loose_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Bd2JpsiKstarSel = self.Bd2JpsiKstarLoose()
	Bd2JpsiKstarSeq = SelectionSequence("SeqBd2JpsiKstarLoose", TopSelection = Bd2JpsiKstarSel)
	return StrippingLine('Bd2JpsiKstarLooseLine', prescale = 1, algos = [Bd2JpsiKstarSeq])   
     
    def Jpsi2MuMuLoose( self ):
        StdVeryLooseJpsi2MuMu = DataOnDemand(Location = "Phys/StdVeryLooseJpsi2MuMu")
	_JpsiFilter = FilterDesktop("JpsiFilterForBd2JpsiKstarLoose")
	_JpsiFilter.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2Loose)s)" \
        	           "& (PT > %(JpsiPTLoose)s *MeV)" \
        	           "& (ADMASS('J/psi(1S)') < %(JpsiMassWinLoose)s *MeV)" \
        	           "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2Loose)s)" % self.getProps()

	Jpsi = Selection("Jpsi2MuMuForBd2JpsiKstarLoose",
			Algorithm = _JpsiFilter,
			RequiredSelections = [StdVeryLooseJpsi2MuMu])
	return Jpsi

    def Kstar2Kpi( self ):
        StdLooseKstar2Kpi = DataOnDemand(Location="Phys/StdLooseKstar2Kpi")
	_kstarFilter = FilterDesktop("KstarFilterForBd2JpsiKstar")
	_kstarFilter.Code = "  (MINTREE('K+'==ABSID, (PIDK - PIDp)) > %(KaonPIDKMinusPIDp)s)" \
        	            "& (MINTREE('K+'==ABSID, PIDK) > %(KaonPIDK)s)"\
       		            "& (ADMASS('K*(892)0') < %(KstarMassWin)s *MeV)" \
        	            "& (PT > %(KstarPT)s *MeV)" \
        	            "& (VFASPF(VCHI2/VDOF) < %(KstarVCHI2)s)" % self.getProps()

	Kstar =  Selection ("Kstar2KpiForBd2JpsiKstar",
                 	Algorithm = _kstarFilter,
                 	RequiredSelections = [StdLooseKstar2Kpi])
	return Kstar	

    def Kstar2KpiLoose( self ):
        StdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
        StdNoPIDsPions = DataOnDemand(Location  = "Phys/StdNoPIDsPions")
	_Kstar = CombineParticles("Kstar2KPiForBd2JpsiKstarLoose")
	_Kstar.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
	_Kstar.DaughtersCuts = {  "K+" :"(TRCHI2DOF < %(KaonTRCHI2Loose)s)" % self.getProps()
				, "pi+":"(TRCHI2DOF < %(PionTRCHI2Loose)s)" % self.getProps()
                	  	}
	_Kstar.CombinationCut = "(ADAMASS('K*(892)0') < %(KstarMassWinLoose)s *MeV)" % self.getProps()
	_Kstar.MotherCut = "(VFASPF(VCHI2/VDOF)< %(KstarVCHI2Loose)s) & (PT > %(KstarPTLoose)s *MeV)" % self.getProps()

	Kstar = Selection("SelKstarForBd2JpsiKstar",
                 	Algorithm = _Kstar,
                 	RequiredSelections = [StdNoPIDsKaons, StdNoPIDsPions])
	return Kstar

    def Bd2JpsiKstar( self ):
        Jpsi = DataOnDemand(Location = "Phys/StdLTUnbiasedJpsi2MuMu")
	Kstar = self.Kstar2Kpi()
	_Bd = CombineParticles("Bd2JpsiKstar")
      	_Bd.DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc"
        _Bd.CombinationCut = "ADAMASS('B0') < %(BdMassWin)s *MeV" % self.getProps()
        _Bd.MotherCut = "  (VFASPF(VCHI2/VDOF) < %(BdVCHI2)s)"\
			"& (PT > %(BdPT)s *MeV)" % self.getProps()
        _Bd.ReFitPVs = True
	# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar
	_Bd.addTool( OfflineVertexFitter() )
	_Bd.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	_Bd.OfflineVertexFitter.useResonanceVertex = False

	Bd = Selection("SelBd2JpsiKstar",
                 	Algorithm = _Bd,
                 	RequiredSelections = [Jpsi, Kstar])
	return Bd

    def Bd2JpsiKstarLoose( self ):
	Jpsi = self.Jpsi2MuMuLoose()
	Kstar = self.Kstar2KpiLoose()
	_Bd = CombineParticles("Bd2JpsiKstarLoose")
      	_Bd.DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc"
        _Bd.CombinationCut = "ADAMASS('B0') < %(BdMassWinLoose)s *MeV" % self.getProps()
        _Bd.MotherCut = "(VFASPF(VCHI2/VDOF) < %(BdVCHI2Loose)s)" % self.getProps()
        _Bd.ReFitPVs = True
	# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar
	_Bd.addTool( OfflineVertexFitter() )
	_Bd.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	_Bd.OfflineVertexFitter.useResonanceVertex = False

	Bd = Selection("SelBd2JpsiKstarLoose",
                 	Algorithm = _Bd,
                 	RequiredSelections = [Jpsi, Kstar])
	return Bd

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
