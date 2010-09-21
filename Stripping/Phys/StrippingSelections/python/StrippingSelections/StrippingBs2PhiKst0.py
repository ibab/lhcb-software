__author__ = ['Celestino Rodriguez']
__date__ = '26/04/2010'
__version__ = '$Revision: 1.2 $'

'''
Bs->PhiKst0~ selection
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from CommonParticles.Utils import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingBs2PhiKstConf(LHCbConfigurableUser):
    """
    Definition of Bs->PhiKst0 stripping.
    """
    __slots__ = { 
    			"KaonPLoose"		: 1000.0# MeV 
		,	"KaonPTLoose"		: 350.0	# MeV 
		,	"KaonIPLoose"		: 0.03	# mm
                ,       "PionPLoose"            : 1000.0# MeV
                ,       "PionPTLoose"           : 350.0 # MeV
                ,       "PionIPLoose"           : 0.03  # mm
                ,       "PhiVCHI2Loose"         : 30.0  # adimensional
                ,       "PhiPTLoose"            : 750.0 # MeV
                ,       "PhiIPLoose"            : 0.08  # mm
                ,       "PhiMassWinLoose"       : 25.0  # MeV
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
                ,       "PhiVCHI2"              : 25.0  # adimensional
                ,       "PhiPT"                 : 750.0 # MeV
                ,       "PhiIPCHI2"             : 6.25  # adimensional
                ,       "PhiMassWin"            : 20.0  # MeV
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
	Bs2PhiKstSel = self.Bs2PhiKst()
	Bs2PhiKstSeq = SelectionSequence("SeqBs2PhiKstNominal", TopSelection = Bs2PhiKstSel)
	return StrippingLine('Bs2PhiKst0bLine', prescale = 1, algos = [Bs2PhiKstSeq])   
     	
    def loosel( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Bs2PhiKstSel = self.Bs2PhiKstLoose()
	Bs2PhiKstSeq = SelectionSequence("SeqBs2PhiKstLoose", TopSelection = Bs2PhiKstSel)
	return StrippingLine('Bs2PhiKst0bLooseLine', prescale = 1, algos = [Bs2PhiKstSeq])   
    def Phi2KKLoose( self ):
        from Configurables import  OnOfflineTool
        StdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
        _Phi = CombineParticles("Kstar2KPiForBs2PhiKst")
        _Phi.DecayDescriptor = "phi(1020) -> K+ K-"
        _Phi.addTool(OnOfflineTool())
        _Phi.OnOfflineTool.OfflinePVRelatorName = 'GenericParticle2PVRelator__p2PVWithIP_OfflineDistanceCalculatorName_'
        _Phi.DaughtersCuts = {  "K+" :"(PT > %(KaonPTLoose)s *MeV) & (MIPDV(PRIMARY)  > %(KaonIPLoose)s *mm)& (P > %(KaonP)s *MeV)" % self.getProps()}
        _Phi.CombinationCut = "(ADAMASS('phi(1020)') < %(PhiMassWinLoose)s *MeV)" % self.getProps()
        _Phi.MotherCut = "(VFASPF(VCHI2/VDOF)< %(PhiVCHI2Loose)s) & (PT > %(PhiPTLoose)s *MeV) & (MIPDV(PRIMARY) > %(PhiIPLoose)s *mm)" % self.getProps()

        Phi = Selection("SelPhiForBs2PhiKstLoose",
                        Algorithm = _Phi,
                        RequiredSelections = [StdNoPIDsKaons])
       
        return Phi

    def Phi2KK( self ):
        StdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
        _Phi = CombineParticles("Phi2KPiForBs2PhiKst")
        _Phi.DecayDescriptor = "phi(1020) -> K+ K-"
        _Phi.DaughtersCuts = {  "K+" :"(PT > %(KaonPT)s *MeV) & (MIPCHI2DV(PRIMARY)  > %(KaonIPCHI2)s )& (P > %(KaonP)s *MeV) &(PIDK> %(KaonPIDK)s)" % self.getProps()}
        _Phi.CombinationCut = "(ADAMASS('phi(1020)') < %(PhiMassWin)s *MeV)" % self.getProps()
        _Phi.MotherCut = "(VFASPF(VCHI2/VDOF)< %(PhiVCHI2)s) & (PT > %(PhiPT)s *MeV) & (MIPCHI2DV(PRIMARY) > %(PhiIPCHI2)s )" % self.getProps()

        Phi = Selection("SelPhiForBs2PhiKst",
                        Algorithm = _Phi,
                        RequiredSelections = [StdNoPIDsKaons])

        return Phi

    def Kstar2Kpi( self ):
        StdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
        StdNoPIDsPions = DataOnDemand(Location = "Phys/StdNoPIDsPions")
        _Kstar = CombineParticles("Kstar2KPiForBs2PhiKst")
        _Kstar.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
        _Kstar.DaughtersCuts = {  "K+" :"(PT > %(KaonPT)s *MeV) & (MIPCHI2DV(PRIMARY)  > %(KaonIPCHI2)s)& (P > %(KaonP)s *MeV) & (PIDK > %(KaonPIDK)s)" % self.getProps()
                                , "pi+":"(PT > %(PionPT)s *MeV) & (MIPCHI2DV(PRIMARY)  > %(PionIPCHI2)s)& (P > %(PionP)s *MeV)" % self.getProps()
                                }
        _Kstar.CombinationCut = "(ADAMASS('K*(892)0') < %(KstarMassWin)s *MeV)" % self.getProps()
        _Kstar.MotherCut = "(VFASPF(VCHI2/VDOF)< %(KstarVCHI2)s) & (PT > %(KstarPT)s *MeV) & (MIPCHI2DV(PRIMARY) > %(KstarIPCHI2)s)" % self.getProps()

        Kstar = Selection("SelKstarForBs2PhiKst",
                        Algorithm = _Kstar,
                        RequiredSelections = [StdNoPIDsKaons, StdNoPIDsPions])
        return Kstar
	

    def Kstar2KpiLoose( self ):
        from Configurables import  OnOfflineTool
        StdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
        StdNoPIDsPions = DataOnDemand(Location = "Phys/StdNoPIDsPions")
	_Kstar = CombineParticles("Kstar2KPiForBs2PhiKstLoose")
	_Kstar.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
	_Kstar.addTool(OnOfflineTool())
	_Kstar.OnOfflineTool.OfflinePVRelatorName = 'GenericParticle2PVRelator__p2PVWithIP_OfflineDistanceCalculatorName_'
	_Kstar.DaughtersCuts = {  "K+" :"(PT > %(KaonPTLoose)s *MeV) & (MIPDV(PRIMARY)  > %(KaonIPLoose)s *mm)& (P > %(KaonP)s *MeV)" % self.getProps()
				, "pi+":"(PT > %(PionPTLoose)s *MeV) & (MIPDV(PRIMARY)  > %(PionIPLoose)s *mm)& (P > %(PionP)s *MeV)" % self.getProps()
                	  	}
	_Kstar.CombinationCut = "(ADAMASS('K*(892)0') < %(KstarMassWinLoose)s *MeV)" % self.getProps()
	_Kstar.MotherCut = "(VFASPF(VCHI2/VDOF)< %(KstarVCHI2Loose)s) & (PT > %(KstarPTLoose)s *MeV) & (MIPDV(PRIMARY) > %(KstarIPLoose)s *mm)" % self.getProps()

	Kstar = Selection("SelKstarForBs2PhiKstLoose",
                 	Algorithm = _Kstar,
                 	RequiredSelections = [StdNoPIDsKaons, StdNoPIDsPions])
	return Kstar

    def Bs2PhiKst( self ):

	Kstar = self.Kstar2Kpi()
        Phi=self.Phi2KK() 
	_Bs = CombineParticles("Bs2PhiKst")
      	_Bs.DecayDescriptors =["[B_s0 -> phi(1020) K*(892)~0]cc"]#,"[B_s0 -> phi(1020) K*(892)0]cc"]
        _Bs.CombinationCut = "ADAMASS('B0') < %(BMassWin)s *MeV" % self.getProps()
        _Bs.MotherCut = "(VFASPF(VCHI2/VDOF) < %(BVCHI2)s)"\
			"& (MIPCHI2DV(PRIMARY) < %(BMIPCHI2)s)"\
			"& (BPVVDCHI2	       > %(BDisCHI2)s)"\
			"& (BPVVDSIGN > 0*mm )" % self.getProps()
        _Bs.ReFitPVs = True

	_Bs.addTool( OfflineVertexFitter() )
	_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
	_Bs.OfflineVertexFitter.useResonanceVertex = False

	Bs = Selection("SelBs2PhiKst",
                 	Algorithm = _Bs,
                 	RequiredSelections = [Kstar,Phi])
	return Bs

    def Bs2PhiKstLoose( self ):
        from Configurables import  OnOfflineTool
	Kstar = self.Kstar2KpiLoose()
        Phi = self.Phi2KKLoose() 
	_Bs = CombineParticles("Bs2PhiKstLoose")
      	_Bs.DecayDescriptors = ["[B_s0 -> phi(1020) K*(892)~0]cc"]#,"B_s0 -> phi(1020) K*(892)0"]
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
	Bs = Selection("SelBs2PhiKstLoose",
                 	Algorithm = _Bs,
                 	RequiredSelections = [Kstar,Phi])
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
