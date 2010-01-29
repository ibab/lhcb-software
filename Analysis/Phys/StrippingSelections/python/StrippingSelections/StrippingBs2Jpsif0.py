# $Id: StrippingBs2Jpsif0.py,v 1.2 2010-01-29 18:59:51 gcowan Exp $

__author__ = ['Liming Zhang']
__date__ = '28/01/2010'
__version__ = '$Revision: 1.2 $'

'''
Bs->Jpsif0 lifetime unbiased stripping selection using LoKi::Hybrid and
python configurables. PV refitting is done. Implements the nominal and 
loose lifetime unbiased stripping selections.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingBs2Jpsif0Conf(LHCbConfigurableUser):
    """
    Definition of nominal Bs->Jpsif0 stripping. 
    """
    __slots__ = { 
			"MuonTRCHI2Loose"	: 10.0	# adimensional
		,	"MuonTRCHI2"	        : 10.0	# adimensional
		,	"JpsiMassWinLoose"	: 80.0	# MeV
		,	"JpsiMassWin"	        : 50.0	# MeV 
                ,       "MuonPIDmu"             : -10.0 # adimentsional
		,	"JpsiVCHI2Loose"	: 20.0	# adimensional
		,	"JpsiVCHI2"	        : 20.0	# adimensional
                ,       "PionMINIPLoose"        : 0.025 # mm
                ,       "PionMINIP"             : 0.05  # mm
                ,       "PionMINIPCHI2"         : 2.25  # adimensional
		,	"PionTRCHI2Loose"	: 10.0	# adimensional
		,	"PionTRCHI2"	        : 10.0	# adimensional
                ,       "PionPIDK"              : 10.0  # adimensional
                ,       "PionPIDmu"             : 10.0  # adimensional
		,	"f0MassWin"	        : 500.0	# MeV
		,	"f0SUMPT"       	: 900.0 # MeV
		,	"f0VCHI2"         	: 20.0	# adimensional
		,	"f0VCHI2Loose"   	: 20.0	# adimensional
		,	"BsMassWin"	        : 250.0	# MeV
		,	"BsMassWinLoose"	: 250.0	# MeV
		,	"BsVCHI2" 	        : 6.0	# adimensional
		,	"BsVCHI2Loose" 	        : 10.0	# adimensional
		,       "BsDIRALoose"           : 0.99  # adimensional
                ,       "BsDIRA"                : 0.999 # adimensional
                ,       "BsFDLoose"             : 0.8   # mm
                ,       "BsFD"                  : 1.5   # mm
                 }
    
    def nominal_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        Jpsi2MuMuSel = self.Jpsi2MuMu()
        f02PiPiSel = self.f02PiPi()
	Bs2Jpsif0Sel = self.Bs2Jpsif0()
	Jpsi2MuMuSeq = SelectionSequence("SeqJpsi", TopSelection = Jpsi2MuMuSel)
	f02PiPiSeq = SelectionSequence("Seqf0", TopSelection = f02PiPiSel)
	Bs2Jpsif0Seq = SelectionSequence("SeqBs2Jpsif0", TopSelection = Bs2Jpsif0Sel)	
	return StrippingLine('Bs2Jpsif0Line', prescale = 1, algos = [Jpsi2MuMuSeq, f02PiPiSeq, Bs2Jpsif0Seq])   
     	
    def loose_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        Jpsi2MuMuSel = self.Jpsi2MuMuLoose()
        f02PiPiSel = self.f02PiPiLoose()
	Bs2Jpsif0Sel = self.Bs2Jpsif0Loose()
	Jpsi2MuMuSeq = SelectionSequence("SeqJpsiLoose", TopSelection = Jpsi2MuMuSel)
	f02PiPiSeq = SelectionSequence("Seqf0Loose", TopSelection = f02PiPiSel)
	Bs2Jpsif0Seq = SelectionSequence("SeqBs2Jpsif0Loose", TopSelection = Bs2Jpsif0Sel)
	return StrippingLine('Bs2Jpsif0LooseLine', prescale = 1, algos = [Jpsi2MuMuSeq, f02PiPiSeq, Bs2Jpsif0Seq])   
     
    def Jpsi2MuMuLoose( self ):
	StdVeryLooseJpsi2MuMu = DataOnDemand("StdVeryLooseJpsi2MuMu", "StdVeryLooseJpsi2MuMu")
	_JpsiFilter = FilterDesktop("JpsiFilterForBs2Jpsif0Loose")
	_JpsiFilter.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2Loose)s)" \
        	           "& (ADMASS('J/psi(1S)') < %(JpsiMassWinLoose)s *MeV)" \
        	           "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2Loose)s)" % self.getProps()

	Jpsi = Selection("Jpsi2MuMuForBs2Jpsif0Loose",
			Algorithm = _JpsiFilter,
			RequiredSelections = [StdVeryLooseJpsi2MuMu])
	return Jpsi
    
    def Jpsi2MuMu( self ):
	StdLooseJpsi2MuMu = DataOnDemand("StdLooseJpsi2MuMu", "StdLooseJpsi2MuMu")
	_JpsiFilter = FilterDesktop("JpsiFilterForBs2Jpsif0")
	_JpsiFilter.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2)s)" \
                           "& (MINTREE('mu+'==ABSID, PIDmu) > %(MuonPIDmu)s)" \
        	           "& (ADMASS('J/psi(1S)') < %(JpsiMassWin)s *MeV)" \
        	           "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2)s)" % self.getProps()

	Jpsi = Selection("Jpsi2MuMuForBs2Jpsif0",
			Algorithm = _JpsiFilter,
			RequiredSelections = [StdLooseJpsi2MuMu])
	return Jpsi

 
    def f02PiPiLoose( self ):
        StdNoPIDsPions = DataOnDemand("StdNoPIDsPions", "StdNoPIDsPions")
        _f0 = CombineParticles("f02PiPiLoose")
        _f0.DecayDescriptors = ["f_0(980) -> pi+ pi-", "f_0(980) -> pi- pi-", "f_0(980) -> pi+ pi+"]
        _f0.CombinationCut = "ADAMASS('f_0(980)') < %(f0MassWin)s *MeV" % self.getProps()
        _f0.DaughtersCuts = { "pi+" : "  (MIPDV(PRIMARY) > %(PionMINIPLoose)s *mm )" \
                                      "& (TRCHI2DOF < %(PionTRCHI2Loose)s)" % self.getProps() }
        _f0.MotherCut = "(SUMTREE(PT, ABSID=='pi+') > %(f0SUMPT)s *MeV)" % self.getProps()

	f0 =  Selection ("f02PiPiForBs2Jpsif0Loose",
                 Algorithm = _f0,
                 RequiredSelections = [StdNoPIDsPions])
	return f0	

    def f02PiPi( self ):
        StdLoosePions = DataOnDemand("StdLoosePions", "StdLoosePions")
        _f0 = CombineParticles("f02PiPi")
        _f0.DecayDescriptors = ["f_0(980) -> pi+ pi-", "f_0(980) -> pi- pi-", "f_0(980) -> pi+ pi+"]
        _f0.CombinationCut = "ADAMASS('f_0(980)') < %(f0MassWin)s *MeV" % self.getProps()
        _f0.DaughtersCuts = { "pi+" : "  (MIPDV(PRIMARY) > %(PionMINIP)s *mm )" \
                                      "& (MIPCHI2DV(PRIMARY)> %(PionMINIPCHI2)s)" \
                                      "& (PIDK < %(PionPIDK)s) & (PIDmu < %(PionPIDmu)s)" \
                                      "& (TRCHI2DOF < %(PionTRCHI2)s)" % self.getProps() }
        _f0.MotherCut = "(SUMTREE(PT, ABSID=='pi+') > %(f0SUMPT)s *MeV)" % self.getProps()

	f0 =  Selection ("f02PiPiForBs2Jpsif0",
                 Algorithm = _f0,
                 RequiredSelections = [StdLoosePions])
	return f0	

    def Bs2Jpsif0Loose( self ):
	Jpsi = self.Jpsi2MuMuLoose()
	f0 = self.f02PiPiLoose()
	_Bs = CombineParticles("Bs2Jpsif0Loose")
      	_Bs.DecayDescriptor = "B_s0 -> J/psi(1S) f_0(980)"
        _Bs.CombinationCut = "ADAMASS('B_s0') < %(BsMassWinLoose)s *MeV" % self.getProps()
        _Bs.MotherCut = "  (VFASPF(VCHI2/VDOF) < %(BsVCHI2Loose)s)" \
                        "& (BPVDIRA > %(BsDIRALoose)s)" \
                        "& (BPVVD > %(BsFDLoose)s *mm)" % self.getProps()
        _Bs.ReFitPVs = True
	# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi f0
	_Bs.addTool( OfflineVertexFitter() )
	_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	_Bs.OfflineVertexFitter.useResonanceVertex = False

	Bs = Selection("SelBs2Jpsif0Loose",
                 	Algorithm = _Bs,
                 	RequiredSelections = [Jpsi, f0])
	return Bs

    def Bs2Jpsif0( self ):
	Jpsi = self.Jpsi2MuMu()
	f0 = self.f02PiPi()
	_Bs = CombineParticles("Bs2Jpsif0")
      	_Bs.DecayDescriptor = "B_s0 -> J/psi(1S) f_0(980)"
        _Bs.CombinationCut = "ADAMASS('B_s0') < %(BsMassWin)s *MeV" % self.getProps()
        _Bs.MotherCut = "  (VFASPF(VCHI2/VDOF) < %(BsVCHI2)s)" \
                        "& (BPVDIRA > %(BsDIRA)s)" \
                        "& (BPVVD > %(BsFD)s *mm)" % self.getProps()
        _Bs.ReFitPVs = True
	# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi f0
	_Bs.addTool( OfflineVertexFitter() )
	_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	_Bs.OfflineVertexFitter.useResonanceVertex = False

	Bs = Selection("SelBs2Jpsif0",
                 	Algorithm = _Bs,
                 	RequiredSelections = [Jpsi, f0])
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
