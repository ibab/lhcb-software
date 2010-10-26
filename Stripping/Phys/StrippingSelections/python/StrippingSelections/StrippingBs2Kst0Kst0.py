__author__ = ['Paula Alvarez Cartelle']
__date__ = '25/10/2010'

'''
Bs->Kst0Kst0 stripping selection
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from CommonParticles.Utils import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine

class StrippingBs2KstKstConf(LHCbConfigurableUser):
    """
    Definition of Bs->Kst0Kst0 stripping.
    """
    __slots__ = { 
    			"KaonPT"		: 500.0 # MeV
                ,	"KaonIPCHI2"		: 9. 	# adimensional
                ,       "PionPT"                : 500.0 # MeV
                ,       "PionIPCHI2"            : 9.	# adimensional
                ,	"KstarVCHI2"		: 9.0   # adimensional
                ,	"KstarPT"		: 900.0 # MeV
                ,	"KaonPIDK"              : -5.0  # adimensional
                ,	"KstarMassWin"		: 150.0 # MeV
                ,	"BMassWin"		: 500.0 # MeV
                ,	"BVCHI2"		: 15.0	# adimensional
                ,	"BDOCA"                 : 0.3   # mm

             }

        
    def nominall( self ):
        
     
        KstarCuts = "(INTREE((ABSID=='K+') & (PT > %(KaonPT)s *MeV) & (MIPCHI2DV(PRIMARY)> %(KaonIPCHI2)s) & (PIDK > %(KaonPIDK)s) ))"\
                    "& (INTREE((ABSID=='pi-') & (PT > %(PionPT)s *MeV) & (MIPCHI2DV(PRIMARY)> %(PionIPCHI2)s) ))"\
                    "& (ADMASS('K*(892)0') < %(KstarMassWin)s *MeV)"\
                    "& (BPVDIRA > 0) & (VFASPF(VCHI2/VDOF)< %(KstarVCHI2)s) & (PT > %(KstarPT)s *MeV)"% self.getProps()

	Kstar = FilterDesktop("Kstar2KPiForBs2KstKst",
                              InputLocations=["StdVeryLooseDetachedKst2Kpi"],
                              Code=KstarCuts)
        
	_Bs = CombineParticles("Bs2KstKst")
        _Bs.InputLocations=["Kstar2KPiForBs2KstKst"]
      	_Bs.DecayDescriptor = "B_s0 -> K*(892)0 K*(892)~0"
        _Bs.CombinationCut = "(ADAMASS('B_s0') < %(BMassWin)s *MeV) & (AMAXDOCA('')< %(BDOCA)s *mm)" % self.getProps() 
        _Bs.MotherCut = "(VFASPF(VCHI2/VDOF) < %(BVCHI2)s)"% self.getProps()

        _Bs.ReFitPVs = True

	_Bs.addTool( OfflineVertexFitter() )
	_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	_Bs.OfflineVertexFitter.useResonanceVertex = False

	return StrippingLine('BsKst0Kst0Line', prescale = 1, algos = [Kstar,_Bs])


    def testsel(self):

        KstarCuts = "(ADMASS('K*(892)0') < %(KstarMassWin)s*MeV)"% self.getProps()

	Kstar = FilterDesktop("Kstar2KPiForBs2KstKstTest",
                              InputLocations=["StdVeryLooseDetachedKst2Kpi"],
                              Code=KstarCuts)
        
	_Bs = CombineParticles("Bs2KstKstTest")
        _Bs.InputLocations=["Kstar2KPiForBs2KstKstTest"]
      	_Bs.DecayDescriptor = "B_s0 -> K*(892)0 K*(892)~0"
        _Bs.CombinationCut = "(ADAMASS('B_s0') < %(BMassWin)s *MeV) & (AMAXDOCA('')< %(BDOCA)s *mm)" % self.getProps() 
        _Bs.MotherCut = "(VFASPF(VCHI2/VDOF) < %(BVCHI2)s)"% self.getProps()#\
#			"& (BPVIPCHI2()< %(BMIPCHI2)s)"\
#			"& (BPVVDCHI2 > %(BDisCHI2)s)"\
#			"& (BPVDIRA > 0)" % self.getProps()
        _Bs.ReFitPVs = True

	_Bs.addTool( OfflineVertexFitter() )
	_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	_Bs.OfflineVertexFitter.useResonanceVertex = False

	return StrippingLine('BsKst0Kst0TestLine', prescale = 1, algos = [Kstar,_Bs])

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d

