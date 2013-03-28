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

__author__ = ['Greig Cowan','Juan Palacios','Francesca Dordei']
__date__ = '26/03/2013'
__version__ = '$Revision: 1.3 $'


__all__ = ('B2JpsiXforBeta_sConf')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from CommonParticles.Utils import updateDoD
from StandardParticles import StdLoosePions
from StandardParticles import StdLooseKaons
from StandardParticles import StdAllLoosePions
from StandardParticles import StdAllNoPIDsKaons
from StandardParticles import StdAllLooseKaons
from StandardParticles import StdLooseResolvedEta
from StandardParticles import StdLooseResolvedPi0
from StandardParticles import StdNoPIDsPions
from StandardParticles import StdLooseProtons
from StandardParticles import StdLooseAllPhotons
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
				,	'Jpsi2MuMuDetachedPrescale'
        			,       'Bu2JpsiKPrescale'
        			,       'Bd2JpsiKstarPrescale'
        			,       'Bd2JpsiKsPrescale'
                                ,       'Bs2JpsiPhiPrescale'
				,       'Bs2Jpsif0Prescale'
        			,       'Bs2JpsiEtaPrescale'
				,       'Bs2JpsiEtapPrescale'
				,       'Bs2JpsiPi0Prescale'
				,       'Bs2JpsiRho0Prescale'
                              )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.name = name
	self.config = config
	
        # define input daughter lists for various B -> J/psi X selections
        self.JpsiList = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu/Particles")

        self.KaonList = self.createSubSel( OutputList = "KaonsForBetaS" + self.name,
                                           InputList = DataOnDemand(Location = "Phys/StdLooseKaons/Particles"),
                                           Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s ) & (PIDK >-2)" % self.config )  
        
        self.NoPIDKaonList = self.createSubSel( OutputList = "NoPIDKaonsForBetaS" + self.name,
                                                InputList = DataOnDemand(Location = "Phys/StdAllNoPIDsKaons/Particles"),
                                                Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s)" % self.config )

        self.NoIPKaonList = self.createSubSel( OutputList = "NoIPKaonsForBetaS" + self.name,
                                               InputList = DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles"),
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s ) & (PIDK > 0)" % self.config )    
        
        self.PionList = self.createSubSel( OutputList = "PionsForBetaS" + self.name,
                                           InputList = DataOnDemand(Location = "Phys/StdLoosePions/Particles"),
                                           Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s)" % self.config )
        
        self.NoPIDPionList = self.createSubSel( OutputList = "NoPIDPionsForBetaS" + self.name,
                                                InputList = DataOnDemand(Location = "Phys/StdNoPIDsPions/Particles"),
                                                Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s )" % self.config )
        
        self.NoIPPionList = self.createSubSel( OutputList = "NoIPPionsForBetaS" + self.name,
                                               InputList = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles"),
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s) & (TRGHOSTPROB < 0.5) & (PIDpi - PIDK > -10)" % self.config )
        
        self.ProtonList = self.createSubSel( OutputList = "ProtonsForBetaS" + self.name,
                                        InputList = DataOnDemand(Location = "Phys/StdLooseProtons/Particles"),
                                        Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s )" % self.config )
        
        self.PhiList = self.createSubSel( OutputList = "Phi2KKForBetaS" + self.name,
                                          InputList = DataOnDemand(Location = "Phys/StdLoosePhi2KK/Particles"),
                                          Cuts = "(in_range(980,M,1050))" \
                                          "& (PT > 500.*MeV) " \
                                          "& (VFASPF(VCHI2) < 25)" \
                                          "& (MAXTREE('K+'==ABSID, TRCHI2DOF) < %(TRCHI2DOF)s )" \
                                          "& (MINTREE('K+'==ABSID, PIDK) > 0)" % self.config)
        
	self.KstarList = self.createSubSel( OutputList = "Kstar2KpiForBetaS" + self.name,
                                            InputList = DataOnDemand(Location = "Phys/StdLooseKstar2Kpi/Particles"),
                                            Cuts = "(in_range(826,M,966))" \
                                            "& (PT > 1300.*MeV) " \
                                            "& (VFASPF(VCHI2) < 25)" \
                                            "& (MAXTREE('K+'==ABSID,  TRCHI2DOF) < %(TRCHI2DOF)s )" \
                                            "& (MAXTREE('pi-'==ABSID, TRCHI2DOF) < %(TRCHI2DOF)s )" \
                                            "& (MINTREE('K+'==ABSID, PIDK) > 0)" % self.config)
        
        self.KsListLoose = MergedSelection("StdLooseKsMergedForBetaS" + self.name,
                                           RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKsDD/Particles"),
                                                                 DataOnDemand(Location = "Phys/StdLooseKsLL/Particles")] )
        
        self.KsFromV0ListLoose = MergedSelection("StdKsFromV0MergedForBetaS" + self.name,
                                                 RequiredSelections = [DataOnDemand(Location = "Phys/StdKs2PiPiLL/Particles"),
                                                                       DataOnDemand(Location = "Phys/StdKs2PiPiDD/Particles")] )
        
        
        self.KsList = self.createSubSel(OutputList = "KsForBetaS" + self.name,
                                        InputList = self.KsListLoose,
                                        Cuts = "(VFASPF(VCHI2)<20) & (BPVDLS>5)")
        
        
        
        self.KsLDList = self.createSubSel( OutputList =  "KsLDForBetaS" + self.name,
                                           InputList = DataOnDemand(Location = "Phys/StdLooseKsLD/Particles"),
                                           Cuts = "(VFASPF(VCHI2)<20) & (BPVDLS>5)")
        
        
        self.KsFromV0List = self.createSubSel( OutputList =  "KsFromV0ForBetaS" + self.name,
                                               InputList =  self.KsFromV0ListLoose ,
                                               Cuts = "(VFASPF(VCHI2)<20) & (BPVDLS>5)" )        
        
        
        self.DetachedKstarList = self.createSubSel(OutputList = "DetachedKstarListForBetaS" + self.name,
                                                   InputList = DataOnDemand(Location = "Phys/StdVeryLooseDetachedKst2Kpi/Particles"),
                                                   Cuts = "(INTREE( ('K+'==ABSID) &  (TRCHI2DOF < %(TRCHI2DOF)s) & (MIPCHI2DV(PRIMARY)>6) &(PIDK > -2)))" \
                                                   "& (INTREE( ('pi+'==ABSID) & (TRCHI2DOF < %(TRCHI2DOF)s) & (MIPCHI2DV(PRIMARY)>6) ))" \
                                                   "& (ADMASS('K*(892)0') < 300 *MeV)" \
                                                   "& (VFASPF(VCHI2/VDOF) < 16)" \
                                                   "& (SUMTREE(PT, ((ABSID=='pi+') | (ABSID=='K+'))) > 900. * MeV)" % self.config)
        
        self.DetachedKstarWideList = self.createCombinationsSel( OutputList = "DetachedKstarWideListForBetaS" + self.name,
                                                                 DaughterLists = [ StdLooseKaons, StdLoosePions ],
                                                                 DecayDescriptors = [ "[K*(892)0 -> K+ pi-]cc", "[K*_0(1430)0 -> K+ pi-]cc"  ],
                                                                 DaughterCuts = { "pi-" : " (PT > 500 *MeV) & (PIDK < 0) & (TRGHOSTPROB < 0.8)",
                                                                                  "K+"  : " (PT > 500 *MeV) & (PIDK > 0) & (TRGHOSTPROB < 0.8)"},
                                                                 PreVertexCuts = "(in_range(750,AM,1900))  & (ADOCACHI2CUT(30, ''))",
                                                                 PostVertexCuts = "(VFASPF(VCHI2) < 25)",
                                                                 ReFitPVs = False )
        
        self.f0List = self.createCombinationsSel( OutputList = "f02PiPiForBetaS" + self.name,
                                                  DaughterLists = [ self.KaonList, self.PionList ],
                                                  DecayDescriptors = ["f_0(980) -> pi+ pi-", "f_0(980) -> pi- pi-", "f_0(980) -> pi+ pi+", "f_0(980) -> K+ K-"],
                                                  DaughterCuts = { "pi+" : " (MIPCHI2DV(PRIMARY)>9) " % self.config,
                                                                   "K+"  : " (MIPCHI2DV(PRIMARY)>9) " % self.config },
                                                  PreVertexCuts = "(ACHILD(PT,1)+ACHILD(PT,2) > 900.*MeV) & (AM < 2700 *MeV) & (ADOCACHI2CUT(20., ''))",
                                                  PostVertexCuts = "(VFASPF(VCHI2) < 16)",
                                                  ReFitPVs = False ) # Note that this is false to save CPU time, for the future check with Liming if the kaon list is fine

        self.f0NoIPList = self.createCombinationsSel( OutputList = "f02PiPiNoIPForBetaS" + self.name,
                                                      DaughterLists = [ self.NoIPPionList ],
                                                      DecayDescriptors = ["f_0(980) -> pi+ pi-"],
                                                      DaughterCuts = { "pi+" : "(PT > 250.*MeV)" },
                                                      PreVertexCuts = "in_range(600.,AM,1700.) & (APT > 1300.*MeV) & (ADOCACHI2CUT(20., ''))",
                                                      PostVertexCuts = "(PT > 1500.*MeV) & in_range(700.,M,1600.) & (VFASPF(VCHI2) < 16)",
                                                      ReFitPVs = False ) # Note that this is false to save CPU time

        self.f04PiList = self.createCombinationSel( OutputList = "f04PiForBetaS" + self.name,
                               DaughterLists = [ self.PionList ],
                               DecayDescriptor = "f_0(980) -> pi+ pi- pi+ pi-",
                               DaughterCuts = { "pi+" : "(MIPCHI2DV(PRIMARY)>6) & (PIDK<10) &  (TRGHOSTPROB < 0.8)" % self.config },
                               PreVertexCuts = "(AM < 2700 *MeV) & (APT > 1.0*GeV) & (ADOCACHI2CUT(20., ''))",
                               PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 10)",
                               ReFitPVs = False ) # Note that this is false to save CPU time
        
        self.LambdaListLoose = MergedSelection("StdLooseLambdaMergedForBetaS" + self.name,
                                               RequiredSelections =  [DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles"),
                                                                      DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")])
        self.LambdaList =  self.createSubSel(OutputList = "LambdaForBetaS" + self.name,
                                             InputList = self.LambdaListLoose ,
                                             Cuts = "(MAXTREE('p+'==ABSID, PT) > 500.*MeV) & (MAXTREE('pi-'==ABSID, PT) > 100.*MeV) & (ADMASS('Lambda0') < 15.*MeV) & (VFASPF(VCHI2) < 20)")
        
        self.EtaList = self.createSubSel( OutputList = "EtaForBetaS" + self.name,
                                          InputList = DataOnDemand(Location = "Phys/StdLooseResolvedEta/Particles"),
                                          Cuts = "(PT > 1800.*MeV)"\
                                          "& (MINTREE('gamma'==ABSID, PT) > 300.*MeV)")
        
        self.Pi0List = self.createSubSel( OutputList = "Pi0ForBetaS" + self.name,
                                          InputList = DataOnDemand(Location = "Phys/StdLooseResolvedPi0/Particles"),
                                          Cuts = "(PT > 1500.*MeV)"\
                                          "& (MINTREE('gamma'==ABSID, PT) > 500.*MeV)")
        
        self.makeInclJpsi    ()
        self.makeRho0        ()
        self.makeEtap        ()
        self.makeBu2JpsiK    ()
        self.makeBu2JpsiH    () 
        self.makeBs2JpsiPhi  () 
        self.makeBd2JpsiKstar()  
        self.makeBd2JpsiKs   ()
        self.makeBd2JpsiKsLD ()
        ##self.makeBd2JpsiKsFromV0 ()
        self.makeBs2Jpsif0   ()
        self.makeBs2Jpsif0NoIP()
	self.makeBs2Jpsi4Pi  ()
        self.makeBs2JpsiKstar()
        self.makeBs2JpsiKstarWide()
        self.makeLambdab2JpsiLambda() 
        self.makeBs2JpsiEta  ()
        self.makeBs2JpsiEtap ()  
        self.makeBd2JpsiPi0  ()
        self.makeBd2JpsiRho0 ()  
        self.makeLambdab2Jpsippi() 


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
                          PostVertexCuts = "ALL",
                          ReFitPVs = True ) :
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                     DaughtersCuts = DaughterCuts,
                                     MotherCut = PostVertexCuts,
                                     CombinationCut = PreVertexCuts,
                                     ReFitPVs = ReFitPVs)
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)
        
    def createCombinationsSel( self, OutputList,
	                  DecayDescriptors,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL",
                          ReFitPVs = True ) :
        '''For taking in multiple decay descriptors'''
        combiner = CombineParticles( DecayDescriptors = DecayDescriptors,
                                 DaughtersCuts = DaughterCuts,
                                 MotherCut = PostVertexCuts,
                                 CombinationCut = PreVertexCuts,
                                 ReFitPVs = ReFitPVs)
        return Selection ( OutputList,
                       Algorithm = combiner,
                       RequiredSelections = DaughterLists)

    def makeInclJpsi( self ):
        '''Inclusive J/psi. We keep it for as long as we can'''
        Jpsi2MuMuForBetasLine = StrippingLine( self.name + "Jpsi2MuMuLine", algos = [ self.JpsiList ], HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config["Jpsi2MuMuPrescale"] )
	
        Jpsi2MuMuForBetasDetachedLine = StrippingLine( self.name + "Jpsi2MuMuDetachedLine", algos = [ self.JpsiList ], HLT = "HLT_PASS_RE('Hlt2DiMuonDetachedJPsiDecision')", prescale = self.config["Jpsi2MuMuDetachedPrescale"] )
        
        #Jpsi2MuMuForBetasDetached = self.createSubSel(  OutputList = self.JpsiList.name() + "Detached" + self.name,
        #                                                InputList  = self.JpsiList,
        #                                                Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )
        #Jpsi2MuMuForBetasDetachedLine = StrippingLine( self.name + "Jpsi2MuMuDetachedLine", algos = [ Jpsi2MuMuForBetasDetached ], prescale = 0.1 )

        self.registerLine(Jpsi2MuMuForBetasLine)
        self.registerLine(Jpsi2MuMuForBetasDetachedLine)

    def makeRho0( self ):
        self.Rho0List = self.createCombinationSel( OutputList = "Rho0forBetaS" + self.name,
                                                   DecayDescriptor = "rho(770)0 -> pi+ pi-",
                                                   DaughterLists = [ self.NoIPPionList ],
                                                   DaughterCuts = { "pi+" : "(PT > 250.*MeV)" },
                                                   PreVertexCuts = "(APT > 1100.) & (in_range(400,AM,1050)) & (ADOCACHI2CUT(20, ''))",
                                                   PostVertexCuts = "(PT > 1300.) & (in_range(500,M,950)) & (VFASPF(VCHI2) < 16.)",
                                                   ReFitPVs = False
                                                   )
        
    def makeEtap( self ):
        self.EtapList = self.createCombinationSel( OutputList = "EtapforBetaS" + self.name,
                                                   DecayDescriptor = "eta_prime -> pi+ pi- gamma",
                                                   DaughterLists = [ self.NoIPPionList,
                                                                     DataOnDemand(Location = "Phys/StdLooseAllPhotons/Particles") ],
                                                   DaughterCuts = { "pi+" : "(PT > 250.*MeV)",
                                                                    "gamma" : "(PT > 300.*MeV) & (CL > 0.01)" },
                                                   PreVertexCuts = "(APT > 1600.*MeV) & (in_range(400,AM12,1050)) & (in_range(800,AM,1120))",
                                                   PostVertexCuts = "(PT > 1800.*MeV) & (in_range(500,M12,950)) & (in_range(900,M,1020)) & (VFASPF(VCHI2) < 16.)",
                                                   ReFitPVs = False
                                                   )
        
    def makeBu2JpsiK( self ):
        Bu2JpsiK = self.createCombinationSel( OutputList = "Bu2JpsiK" + self.name,
                                 DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                 DaughterLists = [ self.JpsiList, self.NoIPKaonList ],
                                 DaughterCuts  = {"K+": "(PT > 500.*MeV)" },
                                 PreVertexCuts = "in_range(5050,AM,5550)",
                                 PostVertexCuts = "in_range(5150,M,5450) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config )

        Bu2JpsiKPrescaledLine = StrippingLine( self.name + "Bu2JpsiKPrescaledLine", algos = [ Bu2JpsiK ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config["Bu2JpsiKPrescale"]) 
	
	

        Bu2JpsiKDetached = self.createSubSel( InputList = Bu2JpsiK, OutputList = Bu2JpsiK.name() + "Detached" + self.name,
                                        Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps) &" \
					       "(MINTREE('K+'==ABSID, PT) > 500.*MeV)" % self.config )
        Bu2JpsiKDetachedLine  = StrippingLine( self.name + "Bu2JpsiKDetachedLine", algos = [ Bu2JpsiKDetached ] )

        Bu2JpsiKUnbiased = self.createSubSel( InputList = Bu2JpsiK,
                                        OutputList = Bu2JpsiK.name() + "Unbiased" + self.name,
                                        Cuts = "(MINTREE('K+'==ABSID, PT) > %(DaughterPT)s*MeV) &"\
					       "(MINTREE('K+'==ABSID, P)  > 10*GeV) &"\
					       "(MINTREE('K+'==ABSID, PIDK) > 0) &"\
					       "(MINTREE('K+'==ABSID, PIDK - PIDp) > -2)" % self.config)
        Bu2JpsiKUnbiasedLine = StrippingLine( self.name + "Bu2JpsiKUnbiasedLine",
                                        algos = [ Bu2JpsiKUnbiased ] )
    
        self.registerLine(Bu2JpsiKPrescaledLine)
        self.registerLine(Bu2JpsiKDetachedLine)
        ##self.registerLine(Bu2JpsiKUnbiasedLine)


    def makeBu2JpsiH( self ):
        '''WH: could in fact make lines above a sublist of this one'''
        Bu2JpsiH = self.createCombinationSel( OutputList = "Bu2JpsiKNoPID" + self.name,
                                 DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                 DaughterLists = [ self.JpsiList, self.NoPIDKaonList ],
                                 DaughterCuts  = {"K+": "(PT > 1.*GeV)"},
                                 PreVertexCuts = "in_range(5000,AM,5650)",
                                 PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config )
        Bu2JpsiHDetachedLine  = StrippingLine( self.name + "Bu2JpsiKNoPIDDetachedLine",
                                      algos = [ self.createSubSel( InputList = Bu2JpsiH,
                                                              OutputList = Bu2JpsiH.name() + "Detached" + self.name,
                                                              Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config ) ] )
        self.registerLine(Bu2JpsiHDetachedLine)

    def makeBs2JpsiPhi( self ):
        Bs2JpsiPhi = self.createCombinationSel( OutputList = "Bs2JpsiPhi" + self.name,
                                   DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                                   DaughterLists  = [ self.JpsiList, self.PhiList ],
                                   PreVertexCuts = "in_range(5050,AM,5650)",
                                   #PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config )
				   PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < 20)" % self.config )     # for the other particles is 10
        Bs2JpsiPhiPrescaledLine = StrippingLine( self.name + "Bs2JpsiPhiPrescaledLine", algos = [ Bs2JpsiPhi ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bs2JpsiPhiPrescale'])

        Bs2JpsiPhiDetached = self.createSubSel( InputList = Bs2JpsiPhi,
                                   OutputList = Bs2JpsiPhi.name() + "Detached" + self.name,
                                   Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )
        Bs2JpsiPhiDetachedLine  = StrippingLine( self.name + "Bs2JpsiPhiDetachedLine", algos = [ Bs2JpsiPhiDetached ] )

        Bs2JpsiPhiUnbiased = self.createSubSel( InputList = Bs2JpsiPhi,
                                   OutputList = Bs2JpsiPhi.name() + "Unbiased" + self.name,
                                   Cuts = "(MINTREE('phi(1020)'==ABSID, PT) > %(DaughterPT)s*MeV)" % self.config)
        Bs2JpsiPhiUnbiasedLine =  StrippingLine( self.name + "Bs2JpsiPhiUnbiasedLine", algos = [ Bs2JpsiPhiUnbiased ] )

        self.registerLine(Bs2JpsiPhiPrescaledLine)
        self.registerLine(Bs2JpsiPhiDetachedLine)
        ##self.registerLine(Bs2JpsiPhiUnbiasedLine)


    def makeBd2JpsiKstar( self ):
        Bd2JpsiKstar = self.createCombinationSel( OutputList = "Bd2JpsiKstar" + self.name,
                                     DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc",
                                     DaughterLists  = [ self.JpsiList, self.KstarList ],
                                     PreVertexCuts = "in_range(5050,AM,5550)",
                                     #PostVertexCuts = "in_range(5150,M,5450) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config)
                                     PostVertexCuts = "in_range(5150,M,5450) & (VFASPF(VCHI2PDOF) < 20)" % self.config)      # for the other particles is 10
        Bd2JpsiKstarPrescaledLine = StrippingLine( self.name + "Bd2JpsiKstarPrescaledLine", algos = [ Bd2JpsiKstar ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bd2JpsiKstarPrescale'])


        Bd2JpsiKstarDetached = self.createSubSel( InputList = Bd2JpsiKstar,
                                     OutputList = Bd2JpsiKstar.name() + "Detached" + self.name,
                                     Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )
        Bd2JpsiKstarDetachedLine  = StrippingLine( self.name + "Bd2JpsiKstarDetachedLine",
                                          algos = [ Bd2JpsiKstarDetached ] )

        Bd2JpsiKstarUnbiased = self.createSubSel( InputList = Bd2JpsiKstar,
                                     OutputList = Bd2JpsiKstar.name() + "Unbiased" + self.name,
                                     Cuts = "(PT>2.*GeV) " \
                                     "& (MINTREE('K*(892)0'==ABSID, PT) > %(DaughterPT)s*MeV)" \
				     "& (MINTREE('K+'==ABSID, PIDK) > 0 )" \
				     "& (MINTREE('K+'==ABSID, PIDK - PIDp) > -2 )" % self.config)
        Bd2JpsiKstarUnbiasedLine  = StrippingLine( self.name + "Bd2JpsiKstarUnbiasedLine",
                                          algos = [ Bd2JpsiKstarUnbiased ] )

        self.registerLine(Bd2JpsiKstarPrescaledLine)
        self.registerLine(Bd2JpsiKstarDetachedLine)
        ##self.registerLine(Bd2JpsiKstarUnbiasedLine)

    def makeBd2JpsiKs( self ):
        Bd2JpsiKs = self.createCombinationSel( OutputList = "Bd2JpsiKS" + self.name,
                                  DecayDescriptor = "B0 -> J/psi(1S) KS0",
                                  DaughterLists  = [ self.JpsiList, self.KsList ],
                                  PreVertexCuts = "in_range(5000,AM,5650)",
                                  PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                  )

        Bd2JpsiKsPrescaledLine = StrippingLine( self.name + "Bd2JpsiKsPrescaledLine", algos = [ Bd2JpsiKs ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bd2JpsiKsPrescale'])

        Bd2JpsiKsDetached = self.createSubSel( InputList = Bd2JpsiKs,
                                    OutputList = Bd2JpsiKs.name() + "Detached" + self.name,
                                    Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config)
        Bd2JpsiKsDetachedLine = StrippingLine( self.name + "Bd2JpsiKsDetachedLine",
                                        algos = [ Bd2JpsiKsDetached ] )

        Bd2JpsiKsUnbiased = self.createSubSel( InputList = Bd2JpsiKs,
                                    OutputList = Bd2JpsiKs.name() + "Unbiased" + self.name,
                                    Cuts = "(MINTREE('KS0'==ABSID, PT) > %(DaughterPT)s*MeV)" % self.config)
        Bd2JpsiKsUnbiasedLine = StrippingLine( self.name + "Bd2JpsiKsUnbiasedLine",
                                        algos = [ Bd2JpsiKsUnbiased ] )

        self.registerLine(Bd2JpsiKsPrescaledLine)
        self.registerLine(Bd2JpsiKsDetachedLine)
        ##self.registerLine(Bd2JpsiKsUnbiasedLine)

    def makeBd2JpsiKsLD( self ):
        Bd2JpsiKsLD = self.createCombinationSel( OutputList = "Bd2JpsiKSLD" + self.name,
                                  DecayDescriptor = "B0 -> J/psi(1S) KS0",
                                  DaughterLists  = [ self.JpsiList, self.KsLDList ],
                                  PreVertexCuts = "in_range(5000,AM,5650)",
                                  PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                  )
        

        Bd2JpsiKsLDDetached = self.createSubSel( InputList = Bd2JpsiKsLD,
                                    OutputList = Bd2JpsiKsLD.name() + "Detached" + self.name,
                                                 Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config)
        Bd2JpsiKsLDDetachedLine = StrippingLine( self.name + "Bd2JpsiKsLDDetachedLine",
                                                 algos = [ Bd2JpsiKsLDDetached ] )

        self.registerLine(Bd2JpsiKsLDDetachedLine)

    def makeBd2JpsiKsFromV0( self ):
        Bd2JpsiKsFromV0 = self.createCombinationSel( OutputList = "Bd2JpsiKSFromV0" + self.name,
                                  DecayDescriptor = "B0 -> J/psi(1S) KS0",
                                  DaughterLists  = [ self.JpsiList, self.KsFromV0List ],
                                  PreVertexCuts = "in_range(5000,AM,5650)",
                                  PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                  )
        

        Bd2JpsiKsFromV0Detached = self.createSubSel( InputList = Bd2JpsiKsFromV0,
                                    OutputList = Bd2JpsiKsFromV0.name() + "Detached" + self.name,
                                                 Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config)
        Bd2JpsiKsFromV0DetachedLine = StrippingLine( self.name + "Bd2JpsiKsFromV0DetachedLine",
                                                 algos = [ Bd2JpsiKsFromV0Detached ] )

        self.registerLine(Bd2JpsiKsFromV0DetachedLine)
       

    def makeBs2Jpsif0( self ):
        '''Note: the input list is already heavily lifetime biased'''
        Bs2Jpsif0 = self.createCombinationSel( OutputList = "Bs2Jpsif0" + self.name,
                                  DecayDescriptor = "B_s0 -> J/psi(1S) f_0(980)",
                                  DaughterLists  = [ self.JpsiList, self.f0List ],
                                  PreVertexCuts = "ADAMASS('B_s0') < 300",
                                  PostVertexCuts = "(VFASPF(VCHI2PDOF) < 10) & (BPVDIRA >0.999) & (BPVVD > 1.5 *mm)" )

        Bs2Jpsif0Line = StrippingLine( self.name + "Bs2Jpsif0Line", algos = [ Bs2Jpsif0 ])

        self.registerLine(Bs2Jpsif0Line)

    def makeBs2Jpsif0NoIP( self ):
        Bs2Jpsif0NoIP = self.createCombinationSel( OutputList = "Bs2Jpsif0NoIP" + self.name,
                                                   DecayDescriptor = "B_s0 -> J/psi(1S) f_0(980)",
                                                   DaughterLists  = [ self.JpsiList, self.f0NoIPList ],
                                                   PreVertexCuts = "in_range(5100,AM,5700)",
                                                   PostVertexCuts = "in_range(5200,M,5600) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config )
        Bs2Jpsif0NoIPPrescaledLine = StrippingLine( self.name + "Bs2Jpsif0NoIPPrescaledLine", algos = [ Bs2Jpsif0NoIP ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bs2Jpsif0Prescale'] )
        
        Bs2Jpsif0NoIPDetached = self.createSubSel( InputList = Bs2Jpsif0NoIP,
                                                   OutputList = Bs2Jpsif0NoIP.name() + "Detached" + self.name,
                                                   Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )
        Bs2Jpsif0NoIPDetachedLine  = StrippingLine( self.name + "Bs2Jpsif0NoIPDetachedLine", algos = [ Bs2Jpsif0NoIPDetached ] )
        
        self.registerLine(Bs2Jpsif0NoIPPrescaledLine)
        self.registerLine(Bs2Jpsif0NoIPDetachedLine)
	
    def makeBs2Jpsi4Pi( self ):
        '''Note: the input list is already heavily lifetime biased'''
        Bs2Jpsi4Pi = self.createCombinationSel( OutputList = "Bs2Jpsi4Pi" + self.name,
                                  DecayDescriptor = "B_s0 -> J/psi(1S) f_0(980)",
                                  DaughterLists  = [ self.JpsiList, self.f04PiList],
                                  PreVertexCuts = "ADAMASS('B_s0') < 300",
                                  PostVertexCuts = "(VFASPF(VCHI2PDOF) < 10) & (BPVDIRA >0.999) & (BPVVD > 1.5 *mm)" )

        Bs2Jpsi4PiLine = StrippingLine( self.name + "Bs2Jpsi4PiLine", algos = [ Bs2Jpsi4Pi ])

        self.registerLine(Bs2Jpsi4PiLine)
        
    def makeBs2JpsiKstar( self ):
        '''Note: the input list is already heavily lifetime biased'''
        Bs2JpsiKstar = self.createCombinationSel( OutputList = "Bs2JpsiKstar" + self.name,
                                               DecayDescriptor = "[B_s~0 -> J/psi(1S) K*(892)0]cc",
                                               DaughterLists  = [ self.JpsiList, self.DetachedKstarList ],
                                               PreVertexCuts = "ADAMASS('B_s0') < 300",
                                               PostVertexCuts = "(VFASPF(VCHI2PDOF) < 10) & (BPVDIRA >0.999) & (BPVVD > 1.5 *mm)" )
        
        Bs2JpsiKstarLine = StrippingLine( self.name + "Bs2JpsiKstarLine", algos = [ Bs2JpsiKstar ])
        
        self.registerLine(Bs2JpsiKstarLine)

    def makeBs2JpsiKstarWide( self ):

        Bs2JpsiKstarWide      = self.createCombinationSel( OutputList = "Bs2JpsiKstarWide" + self.name,
                                DecayDescriptor = "[B_s~0 -> J/psi(1S) K*(892)0]cc",
                                DaughterLists  = [ self.JpsiList, self.DetachedKstarWideList ],
                                PreVertexCuts = "in_range(5100,AM,5700)",
                                PostVertexCuts = "(VFASPF(VCHI2PDOF) < 10) & (BPVDIRA >0.999) & (BPVVD > 1.5 *mm)" )

        Bs2JpsiKstarWideLine = StrippingLine( self.name + "Bs2JpsiKstarWideLine", algos = [ Bs2JpsiKstarWide ])

        self.registerLine(Bs2JpsiKstarWideLine)

        
    def makeLambdab2JpsiLambda( self ):
        Lambdab2JpsiLambda = self.createCombinationSel( OutputList = "Lambdab2JpsiLambda" + self.name,
                                    DecayDescriptor = "[Lambda_b0 -> Lambda0 J/psi(1S) ]cc",
                                    DaughterLists  = [ self.JpsiList, self.LambdaList ],
                                    PreVertexCuts = "in_range(5020,AM,6220)",
                                    PostVertexCuts = "in_range(5120,M,6120) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                    )

        Lambdab2JpsiLambdaUnbiasedLine = StrippingLine( self.name + "Lambdab2JpsiLambdaUnbiasedLine", algos = [ Lambdab2JpsiLambda ]) 
        #Lambdab2JpsiLambdaUnbiasedLine = StrippingLine( self.name + "Lambdab2JpsiLambdaUnbiasedLine", algos = [ Lambdab2JpsiLambda ], HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')")
	
        self.registerLine(Lambdab2JpsiLambdaUnbiasedLine)


    def makeBs2JpsiEta( self ):
        Bs2JpsiEta = self.createCombinationSel( OutputList = "Bs2JpsiEta" + self.name,
                                                DecayDescriptor = "B_s0 -> J/psi(1S) eta",
                                                DaughterLists  = [ self.JpsiList, self.EtaList ],
                                                PreVertexCuts = "in_range(4750,AM,6000)",
                                                PostVertexCuts = "in_range(4850,M,5900) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                                )
        
        Bs2JpsiEtaPrescaledLine = StrippingLine( self.name + "Bs2JpsiEtaPrescaledLine", algos = [ Bs2JpsiEta ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bs2JpsiEtaPrescale'] )
        Bs2JpsiEtaDetachedLine  = StrippingLine( self.name + "Bs2JpsiEtaDetachedLine",
                                                 algos = [ self.createSubSel( InputList = Bs2JpsiEta,
                                                                              OutputList = Bs2JpsiEta.name() + "Detached" + self.name,
                                                                              Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )] )
        
        self.registerLine(Bs2JpsiEtaPrescaledLine)
        self.registerLine(Bs2JpsiEtaDetachedLine)


    def makeBs2JpsiEtap( self ):
        Bs2JpsiEtap = self.createCombinationSel( OutputList = "Bs2JpsiEtap" + self.name,
                                                 DecayDescriptor = "B_s0 -> J/psi(1S) eta_prime",
                                                 DaughterLists  = [ self.JpsiList, self.EtapList ],
                                                 PreVertexCuts = "in_range(5000,AM,5800)",
                                                 PostVertexCuts = "in_range(5100,M,5700) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                                 )
        
        Bs2JpsiEtapPrescaledLine = StrippingLine( self.name + "Bs2JpsiEtapPrescaledLine", algos = [ Bs2JpsiEtap ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bs2JpsiEtapPrescale'] )
        Bs2JpsiEtapDetachedLine  = StrippingLine( self.name + "Bs2JpsiEtapDetachedLine",
                                                  algos = [ self.createSubSel( InputList = Bs2JpsiEtap,
                                                                               OutputList = Bs2JpsiEtap.name() + "Detached" + self.name,
                                                                               Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )] )
        
        self.registerLine(Bs2JpsiEtapPrescaledLine)
        self.registerLine(Bs2JpsiEtapDetachedLine)


    def makeBd2JpsiPi0( self ):
        Bd2JpsiPi0 = self.createCombinationSel( OutputList = "Bd2JpsiPi0" + self.name,
                                  DecayDescriptor = "B0 -> J/psi(1S) pi0",
                                  DaughterLists  = [ self.JpsiList, self.Pi0List ],
                                  PreVertexCuts = "in_range(4500,AM,6000)",
                                  PostVertexCuts = "in_range(4700,M,5900) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                  )

        Bd2JpsiPi0PrescaledLine = StrippingLine( self.name + "Bd2JpsiPi0PrescaledLine", algos = [ Bd2JpsiPi0 ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bs2JpsiPi0Prescale'] )
        Bd2JpsiPi0DetachedLine  = StrippingLine( self.name + "Bd2JpsiPi0DetachedLine",
                                            algos = [ self.createSubSel( InputList = Bd2JpsiPi0,
                                                                         OutputList = Bd2JpsiPi0.name() + "Detached" + self.name,
                                                                         Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )] )

        self.registerLine(Bd2JpsiPi0PrescaledLine)
        self.registerLine(Bd2JpsiPi0DetachedLine)


    def makeBd2JpsiRho0( self ):
        Bd2JpsiRho0 = self.createCombinationSel( OutputList = "Bd2JpsiRho0" + self.name,
                                                 DecayDescriptor = "B0 -> J/psi(1S) rho(770)0",
                                                 DaughterLists  = [ self.JpsiList, self.Rho0List ],
                                                 PreVertexCuts = "in_range(5000,AM,5600)",
                                                 PostVertexCuts = "in_range(5100,M,5500) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                                 )

        Bd2JpsiRho0PrescaledLine = StrippingLine( self.name + "Bd2JpsiRho0PrescaledLine", algos = [ Bd2JpsiRho0 ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bs2JpsiRho0Prescale'] )
        Bd2JpsiRho0DetachedLine  = StrippingLine( self.name + "Bd2JpsiRho0DetachedLine",
                                                 algos = [ self.createSubSel( InputList = Bd2JpsiRho0,
                                                                              OutputList = Bd2JpsiRho0.name() + "Detached" + self.name,
                                                                              Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )] )
        
        self.registerLine(Bd2JpsiRho0PrescaledLine)
        self.registerLine(Bd2JpsiRho0DetachedLine)


    def makeLambdab2Jpsippi( self ):
        PartCuts = "(PT>500*MeV) & (MIPCHI2DV(PRIMARY)>9)"
        Lambdab2Jpsippi = self.createCombinationSel( OutputList = "Lambdab2Jpsippi" + self.name,
                                  DecayDescriptor = "[Lambda_b0 -> J/psi(1S) p+ pi-]cc",
                                  DaughterCuts = { "p+" : PartCuts , "pi-" : PartCuts },
                                  DaughterLists  = [ self.JpsiList, self.NoPIDPionList, self.ProtonList ],
                                  PreVertexCuts = "in_range(4800,AM,6200)",
                                  PostVertexCuts = "in_range(4900,M,6100) & (VFASPF(VCHI2PDOF) < 5)" % self.config
                                  )

#        Lambdab2JpsippiPrescaledLine = StrippingLine( self.name + "Lambdab2JpsippiPrescaledLine", algos = [ Lambdab2Jpsippi ] , prescale = self.config['Lambdab2JpsippiPrescale'])
        Lambdab2JpsippiDetachedLine  = StrippingLine( self.name + "Lambdab2JpsippiDetachedLine",
                                            algos = [ self.createSubSel( InputList = Lambdab2Jpsippi,
                                                                         OutputList = Lambdab2Jpsippi.name() + "Detached" + self.name,
                                                                         Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )] )

        self.registerLine(Lambdab2JpsippiDetachedLine)
        

