# $Id: StrippingB2JpsiXforBeta_s.py,v 1.3 2010-09-05 21:31:13 wouter Exp $
'''
Module for construction of B->JpsiX roadmap lifetime unbiased 
stripping Selections and StrippingLine.
Provides functions to build Bs, Jpsi, Phi selections.
Provides class Bs2JpsiPhiConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   B2JpsiXLines
'''

__author__ = ['Greig Cowan','Juan Palacios']
__date__ = '26/08/2010'
__version__ = '$Revision: 1.3 $'


__all__ = ('Lines')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles import StdLoosePions
from StandardParticles import StdNoPIDsKaons
from StandardParticles import StdLooseKaons
from StandardParticles import StdLooseResolvedEta
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV

class B2JpsiXforBeta_sConf(LineBuilder) :
    __configuration_keys__ = (
					'TRCHI2DOF'
				,	'BPVLTIME'
				,	'DaughterPT'
        			,       'VCHI2PDOF'
        			,       'Jpsi2MuMuPrescale'
        			,       'Bu2JpsiKPrescale'
        			,       'Bd2JpsiKstarPrescale'
        			,       'Bd2JpsiKsPrescale'
        			,       'Bs2JpsiPhiPrescale'
        			,       'Bs2JpsiEtaPrescale'
                              )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.name = name
	self.config = config
	
        # define input daughter lists for various B -> J/psi X selections
        self.JpsiList = DataOnDemand(Location = "Phys/StdLooseJpsi2MuMu/Particles")

        self.KaonList = self.createSubSel( OutputList = "KaonsForBetaS" + self.name,
                                        InputList = StdLooseKaons,
                                        Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s ) & (PIDK > -2)" % self.config )
        
        self.NoPIDKaonList = self.createSubSel( OutputList = "NoPIDKaonsForBetaS" + self.name,
                                            InputList = StdNoPIDsKaons,
                                            Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s)" % self.config )

        self.PhiList = self.createSubSel( OutputList = "Phi2KKForBetaS" + self.name,
                        InputList = DataOnDemand(Location = "Phys/StdLoosePhi2KK/Particles"),
                        Cuts = "(in_range(1008,M,1032))" \
                        "& (PT > 500.*MeV) " \
                        "& (VFASPF(VCHI2) < 16)" \
                        "& (MAXTREE('K+'==ABSID, TRCHI2DOF) < %(TRCHI2DOF)s )" \
                        "& (MINTREE('K+'==ABSID, PIDK) > -2)" % self.config)
      
	self.KstarList = self.createSubSel( OutputList = "Kstar2KpiForBetaS" + self.name,
			InputList = DataOnDemand(Location = "Phys/StdLooseKstar2Kpi/Particles"),
                        Cuts = "(in_range(826,M,966))" \
                        "& (PT > 500.*MeV) " \
                        "& (VFASPF(VCHI2) < 16)" \
                        "& (MAXTREE('K+'==ABSID,  TRCHI2DOF) < %(TRCHI2DOF)s )" \
                        "& (MAXTREE('pi-'==ABSID, TRCHI2DOF) < %(TRCHI2DOF)s )" \
                        "& (MINTREE('K+'==ABSID, PIDK) > -2)" % self.config)
 
        self.KsListLoose = MergedSelection("StdLooseKsMergedForBetaS" + self.name,
                                RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKsDD/Particles"),
                                                    DataOnDemand(Location = "Phys/StdLooseKsLL/Particles")] )

        self.KsList = self.createSubSel(OutputList = "KsForBetaS" + self.name,
                                    InputList = self.KsListLoose,
                                    Cuts = "(VFASPF(VCHI2)<20) & (BPVDLS>5)")

        self.f0List = self.createCombinationsSel( OutputList = "f02PiPiForBetaS" + self.name,
                               DaughterLists = [ self.KaonList, StdLoosePions ],
                               DecayDescriptors = ["f_0(980) -> pi+ pi-", "f_0(980) -> pi- pi-", "f_0(980) -> pi+ pi+", "f_0(980) -> K+ K-", "f_0(980) -> K- K-", "f_0(980) -> K+ K+"],
                               DaughterCuts = { "pi+" : " (MIPCHI2DV(PRIMARY)>6) & (TRCHI2DOF < %(TRCHI2DOF)s)" % self.config,
                                                "K+"  : " (MIPCHI2DV(PRIMARY)>6) & (TRCHI2DOF < %(TRCHI2DOF)s)" % self.config },
                               PreVertexCuts = "(ACHILD(PT,1)+ACHILD(PT,2) > 900.*MeV) & (AM < 2700 *MeV) & (ADOCACHI2CUT(20., ''))",
                               PostVertexCuts = "(VFASPF(VCHI2) < 16)" )


        self.LambdaListLoose = MergedSelection("StdLooseLambdaMergedForBetaS" + self.name,
                                  RequiredSelections =  [DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles"),
                                                         DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")])
        self.LambdaList =  self.createSubSel(OutputList = "LambdaForBetaS" + self.name,
                            InputList = self.LambdaListLoose ,
                            Cuts = "(MAXTREE('p+'==ABSID, PT) > 500.*MeV) & (MAXTREE('pi-'==ABSID, PT) > 100.*MeV) & (ADMASS('Lambda0') < 15.*MeV) & (VFASPF(VCHI2) < 20)")

        self.EtaList = self.createSubSel( OutputList = "EtaForBetaS" + self.name,
                			InputList = StdLooseResolvedEta,
                			Cuts = "(PT > 1500.*MeV)"\
                			"& (MINTREE('gamma'==ABSID, PT) > 500.*MeV)")

        self.makeInclJpsi    ()
        self.makeBu2JpsiK    ()
        self.makeBu2JpsiH    () 
        self.makeBs2JpsiPhi  () 
        self.makeBd2JpsiKstar()  
        self.makeBd2JpsiKs   ()  
        self.makeBs2Jpsif0   ()  
        self.makeLambdab2JpsiLambda() 
        self.makeBs2JpsiEta  ()  


    def createSubSel( self, OutputList, InputList, Cuts ) :
        '''create a selection using a FilterDesktop'''
        filter = FilterDesktop(Code = Cuts)
        return Selection( OutputList,
                      Algorithm = filter,
                      RequiredSelections = [ InputList ] )

    def createCombinationSel( self, OutputList,
                          DecayDescriptor,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL" ) :
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                    DaughtersCuts = DaughterCuts,
                                    MotherCut = PostVertexCuts,
                                    CombinationCut = PreVertexCuts,
                                    ReFitPVs = True)
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)

    def createCombinationsSel( self, OutputList,
	                  DecayDescriptors,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL" ) :
        '''For taking in multiple decay descriptors'''
        combiner = CombineParticles( DecayDescriptors = DecayDescriptors,
                                 DaughtersCuts = DaughterCuts,
                                 MotherCut = PostVertexCuts,
                                 CombinationCut = PreVertexCuts,
                                 ReFitPVs = False)
        return Selection ( OutputList,
                       Algorithm = combiner,
                       RequiredSelections = DaughterLists)

    def makeInclJpsi( self ):
        '''Inclusive J/psi. We keep it for as long as we can'''
        Jpsi2MuMuForBetasLine = StrippingLine("Jpsi2MuMuForBetasLine" + self.name, algos = [ self.JpsiList ], prescale = self.config["Jpsi2MuMuPrescale"])
        Jpsi2MuMuForBetasDetached = self.createSubSel(  OutputList = self.JpsiList.name() + "Detached" + self.name,
                                                        InputList  = self.JpsiList,
                                                        Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )
        Jpsi2MuMuForBetasDetachedLine = StrippingLine("Jpsi2MuMuForBetasDetachedLine" + self.name, algos = [ Jpsi2MuMuForBetasDetached ] )

        self.registerLine(Jpsi2MuMuForBetasLine)
        self.registerLine(Jpsi2MuMuForBetasDetachedLine)

    def makeBu2JpsiK( self ):
        Bu2JpsiK = self.createCombinationSel( OutputList = "Bu2JpsiK" + self.name,
                                 DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                 DaughterLists = [ self.JpsiList, self.KaonList ],
                                 DaughterCuts  = {"K+": "(PT > 0)" },
                                 PreVertexCuts = "in_range(5000,AM,5650)",
                                 PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config )

        Bu2JpsiKPrescaledLine = StrippingLine("Bu2JpsiKPrescaledLine" + self.name, algos = [ Bu2JpsiK ] , prescale = self.config["Bu2JpsiKPrescale"])

        Bu2JpsiKDetached = self.createSubSel( InputList = Bu2JpsiK, OutputList = Bu2JpsiK.name() + "Detached" + self.name,
                                        Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )
        Bu2JpsiKDetachedLine  = StrippingLine("Bu2JpsiKDetachedLine" + self.name, algos = [ Bu2JpsiKDetached ] )

        Bu2JpsiKUnbiased = self.createSubSel( InputList = Bu2JpsiK,
                                        OutputList = Bu2JpsiK.name() + "Unbiased" + self.name,
                                        Cuts = "(MINTREE('K+'==ABSID, PT) > %(DaughterPT)s*MeV)" % self.config)
        Bu2JpsiKUnbiasedLine = StrippingLine("Bu2JpsiKUnbiasedLine" + self.name,
                                        algos = [ Bu2JpsiKUnbiased ] )
    
        self.registerLine(Bu2JpsiKPrescaledLine)
        self.registerLine(Bu2JpsiKDetachedLine)
        self.registerLine(Bu2JpsiKUnbiasedLine)


    def makeBu2JpsiH( self ):
        '''WH: could in fact make lines above a sublist of this one'''
        Bu2JpsiH = self.createCombinationSel( OutputList = "Bu2JpsiKNoPID" + self.name,
                                 DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                 DaughterLists = [ self.JpsiList, self.NoPIDKaonList ],
                                 DaughterCuts  = {"K+": "(PT > 0)"},
                                 PreVertexCuts = "in_range(5000,AM,5650)",
                                 PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config )
        Bu2JpsiHDetachedLine  = StrippingLine("Bu2JpsiKNoPIDDetachedLine" + self.name,
                                      algos = [ self.createSubSel( InputList = Bu2JpsiH,
                                                              OutputList = Bu2JpsiH.name() + "Detached" + self.name,
                                                              Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config ) ] )
        self.registerLine(Bu2JpsiHDetachedLine)

    def makeBs2JpsiPhi( self ):
        Bs2JpsiPhi = self.createCombinationSel( OutputList = "Bs2JpsiPhi" + self.name,
                                   DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                                   DaughterLists  = [ self.JpsiList, self.PhiList ],
                                   PreVertexCuts = "in_range(5000,AM,5650)",
                                   PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config )
        Bs2JpsiPhiPrescaledLine = StrippingLine("Bs2JpsiPhiPrescaledLine" + self.name, algos = [ Bs2JpsiPhi ] , prescale = self.config['Bs2JpsiPhiPrescale'])

        Bs2JpsiPhiDetached = self.createSubSel( InputList = Bs2JpsiPhi,
                                   OutputList = Bs2JpsiPhi.name() + "Detached" + self.name,
                                   Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )
        Bs2JpsiPhiDetachedLine  = StrippingLine("Bs2JpsiPhiDetachedLine" + self.name, algos = [ Bs2JpsiPhiDetached ] )

        Bs2JpsiPhiUnbiased = self.createSubSel( InputList = Bs2JpsiPhi,
                                   OutputList = Bs2JpsiPhi.name() + "Unbiased" + self.name,
                                   Cuts = "(MINTREE('phi(1020)'==ABSID, PT) > %(DaughterPT)s*MeV)" % self.config)
        Bs2JpsiPhiUnbiasedLine =  StrippingLine("Bs2JpsiPhiUnbiasedLine" + self.name, algos = [ Bs2JpsiPhiUnbiased ] )

        self.registerLine(Bs2JpsiPhiPrescaledLine)
        self.registerLine(Bs2JpsiPhiDetachedLine)
        self.registerLine(Bs2JpsiPhiUnbiasedLine)


    def makeBd2JpsiKstar( self ):
        Bd2JpsiKstar = self.createCombinationSel( OutputList = "Bd2JpsiKstar" + self.name,
                                     DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc",
                                     DaughterLists  = [ self.JpsiList, self.KstarList ],
                                     PreVertexCuts = "in_range(5000,AM,5650)",
                                     PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config)
        Bd2JpsiKstarPrescaledLine = StrippingLine("Bd2JpsiKstarPrescaledLine" + self.name, algos = [ Bd2JpsiKstar ] , prescale = self.config['Bd2JpsiKstarPrescale'])

        Bd2JpsiKstarDetached = self.createSubSel( InputList = Bd2JpsiKstar,
                                     OutputList = Bd2JpsiKstar.name() + "Detached" + self.name,
                                     Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )
        Bd2JpsiKstarDetachedLine  = StrippingLine("Bd2JpsiKstarDetachedLine" + self.name,
                                          algos = [ Bd2JpsiKstarDetached ] )

        Bd2JpsiKstarUnbiased = self.createSubSel( InputList = Bd2JpsiKstar,
                                     OutputList = Bd2JpsiKstar.name() + "Unbiased" + self.name,
                                     Cuts = "(PT>2.*GeV) " \
                                     "& (MINTREE('K*(892)0'==ABSID, PT) > %(DaughterPT)s*MeV)" % self.config)
        Bd2JpsiKstarUnbiasedLine  = StrippingLine("Bd2JpsiKstarUnbiasedLine" + self.name,
                                          algos = [ Bd2JpsiKstarUnbiased ] )

        self.registerLine(Bd2JpsiKstarPrescaledLine)
        self.registerLine(Bd2JpsiKstarDetachedLine)
        self.registerLine(Bd2JpsiKstarUnbiasedLine)

    def makeBd2JpsiKs( self ):
        Bd2JpsiKs = self.createCombinationSel( OutputList = "Bd2JpsiKS" + self.name,
                                  DecayDescriptor = "B0 -> J/psi(1S) KS0",
                                  DaughterLists  = [ self.JpsiList, self.KsList ],
                                  PreVertexCuts = "in_range(5000,AM,5650)",
                                  PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                  )
        Bd2JpsiKsPrescaledLine = StrippingLine("Bd2JpsiKsPrescaledLine" + self.name, algos = [ Bd2JpsiKs ] , prescale = self.config['Bd2JpsiKsPrescale'])

        Bd2JpsiKsDetached = self.createSubSel( InputList = Bd2JpsiKs,
                                    OutputList = Bd2JpsiKs.name() + "Detached" + self.name,
                                    Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config)
        Bd2JpsiKsDetachedLine = StrippingLine("Bd2JpsiKsDetachedLine" + self.name,
                                        algos = [ Bd2JpsiKsDetached ] )

        Bd2JpsiKsUnbiased = self.createSubSel( InputList = Bd2JpsiKs,
                                    OutputList = Bd2JpsiKs.name() + "Unbiased" + self.name,
                                    Cuts = "(MINTREE('KS0'==ABSID, PT) > %(DaughterPT)s*MeV)" % self.config)
        Bd2JpsiKsUnbiasedLine = StrippingLine("Bd2JpsiKsUnbiasedLine" + self.name,
                                        algos = [ Bd2JpsiKsUnbiased ] )

        self.registerLine(Bd2JpsiKsPrescaledLine)
        self.registerLine(Bd2JpsiKsDetachedLine)
        self.registerLine(Bd2JpsiKsUnbiasedLine)

    def makeBs2Jpsif0( self ):
        '''Note: the input list is already heavily lifetime biased'''
        Bs2Jpsif0 = self.createCombinationSel( OutputList = "Bs2Jpsif0" + self.name,
                                  DecayDescriptor = "B_s0 -> J/psi(1S) f_0(980)",
                                  DaughterLists  = [ self.JpsiList, self.f0List ],
                                  PreVertexCuts = "ADAMASS('B_s0') < 300",
                                  PostVertexCuts = "(VFASPF(VCHI2PDOF) < 10) & (BPVDIRA >0.999) & (BPVVD > 1.5 *mm)" )

        Bs2Jpsif0Line = StrippingLine("Bs2Jpsif0Line" + self.name, algos = [ Bs2Jpsif0 ])

        self.registerLine(Bs2Jpsif0Line)

    def makeLambdab2JpsiLambda( self ):
        Lambdab2JpsiLambda = self.createCombinationSel( OutputList = "Lambdab2JpsiLambda" + self.name,
                                    DecayDescriptor = "[Lambda_b0 -> Lambda0 J/psi(1S) ]cc",
                                    DaughterLists  = [ self.JpsiList, self.LambdaList ],
                                    PreVertexCuts = "in_range(5020,AM,6220)",
                                    PostVertexCuts = "in_range(5120,M,6120) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                    )

        Lambdab2JpsiLambdaUnbiasedLine = StrippingLine("Lambdab2JpsiLambdaUnbiasedLine" + self.name, algos = [ Lambdab2JpsiLambda ])
        self.registerLine(Lambdab2JpsiLambdaUnbiasedLine)


    def makeBs2JpsiEta( self ):
        Bs2JpsiEta = self.createCombinationSel( OutputList = "Bs2JpsiEta" + self.name,
                                  DecayDescriptor = "B_s0 -> J/psi(1S) eta",
                                  DaughterLists  = [ self.JpsiList, self.EtaList ],
                                  PreVertexCuts = "in_range(5000,AM,5650)",
                                  PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                  )

        Bs2JpsiEtaPrescaledLine = StrippingLine("Bs2JpsiEtaPrescaledLine" + self.name, algos = [ Bs2JpsiEta ] , prescale = self.config['Bs2JpsiEtaPrescale'])
        Bs2JpsiEtaDetachedLine  = StrippingLine("Bs2JpsiEtaDetachedLine" + self.name,
                                            algos = [ self.createSubSel( InputList = Bs2JpsiEta,
                                                               OutputList = Bs2JpsiEta.name() + "Detached" + self.name,
                                                               Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )] )

        Bs2JpsiEtaUnbiasedLine  = StrippingLine("Bs2JpsiEtaUnbiasedLine" + self.name,
                                            algos = [ self.createSubSel( InputList = Bs2JpsiEta,
                                                               OutputList = Bs2JpsiEta.name() + "Unbiased" + self.name,
                                                               Cuts = "(PT > 4.*GeV) & (MINTREE('eta'==ABSID, PT) > 1500.*MeV)" ) ] )
        self.registerLine(Bs2JpsiEtaPrescaledLine)
        self.registerLine(Bs2JpsiEtaDetachedLine)
        self.registerLine(Bs2JpsiEtaUnbiasedLine)

