__author__ = ['Celestino Rodriguez']
__date__ = '26/04/2010'
__version__ = '$Revision: 1.6 $'

'''
Bs->Kst0Kst0 selection
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from CommonParticles.Utils import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingBs2KstKstConf(LHCbConfigurableUser):
    """
    Definition of Bs->Kst0Kst0 stripping.
    """
    __slots__ = { 
    			"KaonPLoose"		: 1000.0# MeV 
		,	"KaonPTLoose"		: 350.0	# MeV 
		,	"KaonIPLoose"		: 0.03	# mm
                ,       "PionPLoose"            : 1000.0# MeV
                ,       "PionPTLoose"           : 350.0 # MeV
                ,       "PionIPLoose"           : 0.03  # mm
		,	"KstarVCHI2Loose"	: 30.0	# adimensional
		,	"KstarPTLoose"		: 750.0	# MeV
                ,       "KstarIPLoose"          : 0.08	# mm
		,	"KstarMassWinLoose"	: 150.0	# MeV
		,	"BMassWinLoose"		: 500.0	# MeV
		,	"BVCHI2Loose" 		: 10.0	# adimensional
		,	"BMIPLoose" 		: 0.1	# mm
		,	"BDisLoose"		: 0.7	# mm
                ,	"KaonP"			: 1000.0# MeV
                ,	"KaonPT"		: 350.0 # MeV
                ,	"KaonIPCHI2"		: 1.	# adimensional
                ,       "PionP"                 : 1000.0# MeV
                ,       "PionPT"                : 350.0 # MeV
                ,       "PionIPCHI2"            : 1.	# adimensional
                ,	"KstarVCHI2"		: 25.0  # adimensional
                ,	"KstarPT"		: 750.0 # MeV
                ,	"KstarIPCHI2"		: 6.25  # adimensional
                ,       "KaonPIDK"              : -5.0  # adimensional
                ,	"KstarMassWin"		: 150.0 # MeV
                ,	"BMassWin"		: 500.0 # MeV
                ,	"BVCHI2"		: 5.0	# adimensional
                ,	"BMIPCHI2"		: 16	# adimensional
                ,       "BDisCHI2"		: 64	# mm

             }
    
    def nominall( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Bs2KstKstSel = self.Bs2KstKst()
	Bs2KstKstSeq = SelectionSequence("SeqBs2KstKstNominal", TopSelection = Bs2KstKstSel)
	return StrippingLine('BsKst0Kst0Line', prescale = 1, algos = [Bs2KstKstSeq])   
     	
    def loosel( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Bs2KstKstSel = self.Bs2KstKstLoose()
	Bs2KstKstSeq = SelectionSequence("SeqBs2KstKstLoose", TopSelection = Bs2KstKstSel)
	return StrippingLine('Bs2Kst0Kst0LooseLine', prescale = 1, algos = [Bs2KstKstSeq])   
    def Kstar2Kpi( self ):
        StdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
        StdNoPIDsPions = DataOnDemand(Location = "Phys/StdNoPIDsPions")
        _Kstar = CombineParticles("Kstar2KPiForBs2KstKst")
        _Kstar.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
        _Kstar.DaughtersCuts = {  "K+" :"(PT > %(KaonPT)s *MeV) & (MIPCHI2DV(PRIMARY)  > %(KaonIPCHI2)s)& (P > %(KaonP)s *MeV) & (PIDK > %(KaonPIDK)s)" % self.getProps()
                                , "pi+":"(PT > %(PionPT)s *MeV) & (MIPCHI2DV(PRIMARY)  > %(PionIPCHI2)s)& (P > %(PionP)s *MeV)" % self.getProps()
                                }
        _Kstar.CombinationCut = "(ADAMASS('K*(892)0') < %(KstarMassWin)s *MeV)" % self.getProps()
        _Kstar.MotherCut = "(VFASPF(VCHI2/VDOF)< %(KstarVCHI2)s) & (PT > %(KstarPT)s *MeV) & (MIPCHI2DV(PRIMARY) > %(KstarIPCHI2)s)" % self.getProps()

        Kstar = Selection("SelKstarForBs2KstKst",
                        Algorithm = _Kstar,
                        RequiredSelections = [StdNoPIDsKaons, StdNoPIDsPions])
        return Kstar
	

    def Kstar2KpiLoose( self ):
        from Configurables import  OnOfflineTool
        StdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
        StdNoPIDsPions = DataOnDemand(Location = "Phys/StdNoPIDsPions")
	_Kstar = CombineParticles("Kstar2KPiForBs2KstKstLoose")
	_Kstar.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
	_Kstar.addTool(OnOfflineTool())
	_Kstar.OnOfflineTool.OfflinePVRelatorName = 'GenericParticle2PVRelator__p2PVWithIP_OfflineDistanceCalculatorName_'
	_Kstar.DaughtersCuts = {  "K+" :"(PT > %(KaonPTLoose)s *MeV) & (MIPDV(PRIMARY)  > %(KaonIPLoose)s *mm)& (P > %(KaonP)s *MeV)" % self.getProps()
				, "pi+":"(PT > %(PionPTLoose)s *MeV) & (MIPDV(PRIMARY)  > %(PionIPLoose)s *mm)& (P > %(PionP)s *MeV)" % self.getProps()
                	  	}
	_Kstar.CombinationCut = "(ADAMASS('K*(892)0') < %(KstarMassWinLoose)s *MeV)" % self.getProps()
	_Kstar.MotherCut = "(VFASPF(VCHI2/VDOF)< %(KstarVCHI2Loose)s) & (PT > %(KstarPTLoose)s *MeV) & (MIPDV(PRIMARY) > %(KstarIPLoose)s *mm)" % self.getProps()

	Kstar = Selection("SelKstarForBs2KstKstLoose",
                 	Algorithm = _Kstar,
                 	RequiredSelections = [StdNoPIDsKaons, StdNoPIDsPions])
	return Kstar

    def Bs2KstKst( self ):

	Kstar = self.Kstar2Kpi()
	_Bs = CombineParticles("Bs2KstKst")
      	_Bs.DecayDescriptor = "B_s0 -> K*(892)0 K*(892)~0"
        _Bs.CombinationCut = "ADAMASS('B0') < %(BMassWin)s *MeV" % self.getProps()
        _Bs.MotherCut = "(VFASPF(VCHI2/VDOF) < %(BVCHI2)s)"\
			"& (MIPCHI2DV(PRIMARY) < %(BMIPCHI2)s)"\
			"& (BPVVDCHI2	       > %(BDisCHI2)s)"\
			"& (BPVVDSIGN > 0*mm )" % self.getProps()
        _Bs.ReFitPVs = True

	_Bs.addTool( OfflineVertexFitter() )
	_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	_Bs.OfflineVertexFitter.useResonanceVertex = False

	Bs = Selection("SelBs2KstKst",
                 	Algorithm = _Bs,
                 	RequiredSelections = [Kstar])
	return Bs

    def Bs2KstKstLoose( self ):
        from Configurables import  OnOfflineTool
	Kstar = self.Kstar2KpiLoose()
	_Bs = CombineParticles("Bs2KstKstLoose")
      	_Bs.DecayDescriptor = "B_s0 -> K*(892)0 K*(892)~0"
        _Bs.CombinationCut = "ADAMASS('B0') < %(BMassWinLoose)s *MeV" % self.getProps()
        _Bs.MotherCut = "  (VFASPF(VCHI2/VDOF) < %(BVCHI2Loose)s)"\
                        "& (MIPDV(PRIMARY) < %(BMIPLoose)s *mm)"\
                        "& (BPVVDSIGN > %(BDisLoose)s *mm)" % self.getProps()

        _Bs.ReFitPVs = True
	
	_Bs.addTool( OfflineVertexFitter() )
	_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	_Bs.OfflineVertexFitter.useResonanceVertex = False
        _Bs.addTool(OnOfflineTool())
        _Bs.OnOfflineTool.OfflinePVRelatorName = 'GenericParticle2PVRelator__p2PVWithIP_OfflineDistanceCalculatorName_'
	Bs = Selection("SelBs2KstKstLoose",
                 	Algorithm = _Bs,
                 	RequiredSelections = [Kstar])
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
