
__author__ = ['Liming Zhang']
__date__ = '24/03/2010'
__version__ = '$Revision: 1.7 $'

'''
Bs0->Jpsif0 + JpsiKstbar lifetime biased stripping selection using LoKi::Hybrid and
python configurables. PV refitting was removed. Implements the nominal and 
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
			"MuonTRCHI2"	        : 10.0	# adimensional
		,	"JpsiMassWin"	        : 80.0	# MeV 
                ,       "MuonPIDmu"             : -5.0 # adimentsional
		,	"JpsiVCHI2"	        : 20.0	# adimensional
                ,       "PionMINIPCHI2"         : 4.00  # adimensional
		,	"PionTRCHI2"	        : 10.0	# adimensional
                ,       "PionPIDK"              : 10.0  # adimensional
                ,       "PionPIDmu"             : 10.0  # adimensional
		,	"f0MassWin"	        : 1700.0	# MeV
		,	"f0SUMPT"       	: 900.0 # MeV
		,	"f0VCHI2"         	: 10.0	# adimensional
		,	"BsMassWin"	        : 250.0	# MeV
		,	"BsVCHI2" 	        : 6.0	# adimensional
                ,       "BsDIRA"                : 0.999 # adimensional
                ,       "BsFD"                  : 1.5   # mm
                ,       "KstMassWin"            : 300.0 # MeV                        
                 }

    _Jpsi2MuMuSelection = None

    def nominal_line( self ):
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        return StrippingLine('Bs2Jpsif0Line'
                             , prescale = 1
                             , algos = [ self._Jpsi2MuMu(),
                                         self._f02PiPi(),
                                         self._Bs2Jpsif0() ]
                             )

    
    def nominalKst_line( self ):
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        return StrippingLine('Bs2JpsiKstLine'
                             , prescale = 1
                             , algos = [ self._Jpsi2MuMu(),
                                         self._Kst2Kpi(),
                                         self._Bs2JpsiKst() ]
                             )

    def _Jpsi2MuMu( self ):
        from Configurables import FilterDesktop
        from CommonParticles.StdLooseJpsi2MuMu import StdLooseJpsi2MuMu
        _JpsiFilter = FilterDesktop("JpsiFilterForBs2Jpsif0", InputLocations = ["Phys/StdLooseJpsi2MuMu"])
        _JpsiFilter.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < %(MuonTRCHI2)s)" \
                           "& (MINTREE('mu+'==ABSID, PIDmu) > %(MuonPIDmu)s)" \
        	           "& (ADMASS('J/psi(1S)') < %(JpsiMassWin)s *MeV)" \
        	           "& (VFASPF(VCHI2/VDOF) < %(JpsiVCHI2)s)" % self.getProps()
        
        return _JpsiFilter
	

    def _f02PiPi( self ):
        from Configurables import CombineParticles
        _f0 = CombineParticles("f02PiPi")
        _f0.InputLocations = [ "Phys/StdLoosePions" ]
        _f0.DecayDescriptors = ["f_0(980) -> pi+ pi-", "f_0(980) -> pi- pi-", "f_0(980) -> pi+ pi+"]
#        _f0.CombinationCut = "(ADAMASS('f_0(980)') < %(f0MassWin)s *MeV) & ((ADOCACHI2CUT(30., ''))" % self.getProps()
        _f0.CombinationCut = "(ADAMASS('f_0(980)') < %(f0MassWin)s *MeV) &" % self.getProps()
        _f0.DaughtersCuts = { "pi+" : " (MIPCHI2DV(PRIMARY)> %(PionMINIPCHI2)s)" 
                                      "& (PIDK < %(PionPIDK)s) & (PIDmu < %(PionPIDmu)s)" \
                                      "& (TRCHI2DOF < %(PionTRCHI2)s)" % self.getProps() }
        _f0.MotherCut = "(SUMTREE(PT, ABSID=='pi+') > %(f0SUMPT)s *MeV) & (VFASPF(VCHI2/VDOF) < %(f0VCHI2)s)" % self.getProps()

	return _f0	


    def _Bs2Jpsif0( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
	_Bs = CombineParticles("Bs2Jpsif0")
        _Bs.InputLocations = [ "Phys/JpsiFilterForBs2Jpsif0", "Phys/f02PiPi" ]
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

	return _Bs

    def _Kst2Kpi( self ):
        from Configurables import FilterDesktop
        from CommonParticles.StdVeryLooseDetachedKstar import StdVeryLooseDetachedKst2Kpi
	_KstFilter = FilterDesktop("KstFilterForBs2JpsiKst", InputLocations = ["Phys/StdVeryLooseDetachedKst2Kpi"])
	_KstFilter.Code = "  (INTREE( ('K+'==ABSID) &  (TRCHI2DOF < %(PionTRCHI2)s)))" \
                            "& (INTREE( ('pi+'==ABSID) & (TRCHI2DOF < %(PionTRCHI2)s) & (PIDK< %(PionPIDK)s)))" \
                            "& (ADMASS('K*(892)0') < %(KstMassWin)s *MeV)" \
                            "& (VFASPF(VCHI2/VDOF) < %(f0VCHI2)s)" \
                            "& (SUMTREE(PT, ((ABSID=='pi+') | (ABSID=='K+'))) > %(f0SUMPT)s *MeV)" % self.getProps()

	return _KstFilter
    
    def _Bs2JpsiKst( self ):
        _Bs = self._Bs2Jpsif0().clone("Bs2JpsiKst",
                                     InputLocations = ["Phys/JpsiFilterForBs2Jpsif0", "Phys/KstFilterForBs2JpsiKst" ],
                                     DecayDescriptors = [ '[B_s~0 -> J/psi(1S) K*(892)0]cc' ])
        
	return _Bs
    

	
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
