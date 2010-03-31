
__author__ = ['Liming Zhang']
__date__ = '24/03/2010'
__version__ = '$Revision: 1.4 $'

'''
Bs0->Jpsif0 + JpsiKstbar lifetime biased stripping selection using LoKi::Hybrid and
python configurables. PV refitting is done. Implements the nominal and 
loose lifetime unbiased stripping selections.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from CommonParticles.StdVeryLooseDetachedKstar import StdVeryLooseDetachedKst2Kpi

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
                ,       "BsFDLoose"             : 0.0   # mm
                ,       "BsFD"                  : 1.5   # mm
                ,       "KstMassWin"            : 300.0 # MeV
                 }

    _Jpsi2MuMuSelection = None

    def nominal_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bs2Jpsif0Line', prescale = 1., algos = [self._nominal_line_sequence()]) 
            	
    def loose_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	return StrippingLine('Bs2Jpsif0LooseLine', prescale = 1, algos = [self._loose_line_sequence()])   
	
    def nominalKst_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bs2JpsiKstLine', prescale = 1., algos = [self._nominalKst_line_sequence()])
                                           
    def _nominal_line_sequence( self ):
        return SelectionSequence('SeqBs2Jpsif0', TopSelection = self._Bs2Jpsif0())

    def _loose_line_sequence( self ):
        return SelectionSequence("SeqBs2Jpsif0Loose", TopSelection = self._Bs2Jpsif0Loose())

    def _nominalKst_line_sequence( self ):
        return SelectionSequence('SeqBs2JpsiKst', TopSelection = self._Bs2JpsiKst())

            
    def _Jpsi2MuMuLoose( self ):
	StdVeryLooseJpsi2MuMu = DataOnDemand("StdVeryLooseJpsi2MuMu", "StdVeryLooseJpsi2MuMu")
	_JpsiFilter = FilterDesktop("JpsiFilterForBs2Jpsif0Loose")
	_JpsiFilter.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2Loose)s)" \
        	           "& (ADMASS('J/psi(1S)') < %(JpsiMassWinLoose)s *MeV)" \
        	           "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2Loose)s)" % self.getProps()

	Jpsi = Selection("Jpsi2MuMuForBs2Jpsif0Loose",
			Algorithm = _JpsiFilter,
			RequiredSelections = [StdVeryLooseJpsi2MuMu])
	return Jpsi
    
    def _Jpsi2MuMu( self ):
	if (StrippingBs2Jpsif0Conf._Jpsi2MuMuSelection == None) : 
	    StdLooseJpsi2MuMu = DataOnDemand("StdLooseJpsi2MuMu", "StdLooseJpsi2MuMu")
	    _JpsiFilter = FilterDesktop("JpsiFilterForBs2Jpsif0")
	    _JpsiFilter.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2)s)" \
                           "& (MINTREE('mu+'==ABSID, PIDmu) > %(MuonPIDmu)s)" \
        	           "& (ADMASS('J/psi(1S)') < %(JpsiMassWin)s *MeV)" \
        	           "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2)s)" % self.getProps()

	    StrippingBs2Jpsif0Conf._Jpsi2MuMuSelection = Selection("Jpsi2MuMuForBs2Jpsif0",
			Algorithm = _JpsiFilter,
			RequiredSelections = [StdLooseJpsi2MuMu])
	return StrippingBs2Jpsif0Conf._Jpsi2MuMuSelection

 
    def _f02PiPiLoose( self ):
        StdNoPIDsPions = DataOnDemand("StdNoPIDsPions", "StdNoPIDsPions")
        _f0 = CombineParticles("f02PiPiLoose")
        _f0.DecayDescriptors = ["f_0(980) -> pi+ pi-", "f_0(980) -> pi- pi-", "f_0(980) -> pi+ pi+"]
        _f0.CombinationCut = "ADAMASS('f_0(980)') < %(f0MassWin)s *MeV" % self.getProps()
        _f0.DaughtersCuts = { "pi+" : "  (MIPDV(PRIMARY) > %(PionMINIPLoose)s *mm )" \
                                      "& (TRCHI2DOF < %(PionTRCHI2Loose)s)" % self.getProps() }
        _f0.MotherCut = "(SUMTREE(PT, ABSID=='pi+') > %(f0SUMPT)s *MeV) & (VFASPF(VCHI2/VDOF) < %(f0VCHI2)s)" % self.getProps()

	f0 =  Selection ("f02PiPiForBs2Jpsif0Loose",
                 Algorithm = _f0,
                 RequiredSelections = [StdNoPIDsPions])
	return f0	

    def _f02PiPi( self ):
        StdLoosePions = DataOnDemand("StdLoosePions", "StdLoosePions")
        _f0 = CombineParticles("f02PiPi")
        _f0.DecayDescriptors = ["f_0(980) -> pi+ pi-", "f_0(980) -> pi- pi-", "f_0(980) -> pi+ pi+"]
        _f0.CombinationCut = "ADAMASS('f_0(980)') < %(f0MassWin)s *MeV" % self.getProps()
        _f0.DaughtersCuts = { "pi+" : "  (MIPDV(PRIMARY) > %(PionMINIP)s *mm )" \
                                      "& (MIPCHI2DV(PRIMARY)> %(PionMINIPCHI2)s)" \
                                      "& (PIDK < %(PionPIDK)s) & (PIDmu < %(PionPIDmu)s)" \
                                      "& (TRCHI2DOF < %(PionTRCHI2)s)" % self.getProps() }
        _f0.MotherCut = "(SUMTREE(PT, ABSID=='pi+') > %(f0SUMPT)s *MeV) & (VFASPF(VCHI2/VDOF) < %(f0VCHI2)s)" % self.getProps()

	f0 =  Selection ("f02PiPiForBs2Jpsif0",
                 Algorithm = _f0,
                 RequiredSelections = [StdLoosePions])
	return f0	

    def _Bs2Jpsif0Loose( self ):
	Jpsi = self._Jpsi2MuMuLoose()
	f0 = self._f02PiPiLoose()
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
                 	RequiredSelections = [ f0, Jpsi])
	return Bs

    def _Bs2Jpsif0( self ):
	Jpsi = self._Jpsi2MuMu()
	f0 = self._f02PiPi()
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
                 	RequiredSelections = [f0, Jpsi])
	return Bs

    def _Kst2Kpi( self ):
        StdVeryLooseDetachedKst2Kpi = DataOnDemand("StdVeryLooseDetachedKst2Kpi", "StdVeryLooseDetachedKst2Kpi")
	_KstFilter = FilterDesktop("KstFilterForBs2JpsiKst")
	_KstFilter.Code = "  (INTREE( ('K+'==ABSID) &  (TRCHI2DOF < %(PionTRCHI2)s)))" \
                            "& (INTREE( ('pi+'==ABSID) & (TRCHI2DOF < %(PionTRCHI2)s) & (PIDK< %(PionPIDK)s)))" \
                            "& (ADMASS('K*(892)0') < %(KstMassWin)s *MeV)" \
                            "& (VFASPF(VCHI2/VDOF) < %(f0VCHI2)s)" \
                            "& (SUMTREE(PT, ((ABSID=='pi+') | (ABSID=='K+'))) > %(f0SUMPT)s *MeV)" % self.getProps()
	Kst = Selection("KstFilterForBs2JpsiKst",
                          Algorithm = _KstFilter,
			RequiredSelections = [StdVeryLooseDetachedKst2Kpi])
	return Kst
    
    def _Bs2JpsiKst( self ):
	Jpsi = self._Jpsi2MuMu()
	Kst = self._Kst2Kpi()
	_Bs = CombineParticles("Bs2JpsiKst")
      	_Bs.DecayDescriptor = "[B_s~0 -> J/psi(1S) K*(892)0]cc"
        _Bs.CombinationCut = "ADAMASS('B_s0') < %(BsMassWin)s *MeV" % self.getProps()
        _Bs.MotherCut = "  (VFASPF(VCHI2/VDOF) < %(BsVCHI2)s)" \
                        "& (BPVDIRA > %(BsDIRA)s)" \
                        "& (BPVVD > %(BsFD)s *mm)" % self.getProps()
        _Bs.ReFitPVs = True
	# Set the OfflineVertexFitter to keep the 4 tracks 
	_Bs.addTool( OfflineVertexFitter() )
	_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	#_Bs.OfflineVertexFitter.useResonanceVertex = False
        _Bs.OfflineVertexFitter.applyDauMassConstraint = True
	Bs = Selection("SelBs2JpsiKst",
                 	Algorithm = _Bs,
                 	RequiredSelections = [Kst, Jpsi])
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
