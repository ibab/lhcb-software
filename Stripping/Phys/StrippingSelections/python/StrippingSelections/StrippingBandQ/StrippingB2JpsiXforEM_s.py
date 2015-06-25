# $Id: StrippingB2JpsiXforBandQ_s.py,v 1.3 2010-09-05 21:31:13 wouter Exp $
'''
Module for construction of B->JpsiX for BandQ Early Measurement, copied from StrippingB2JpsiXforBeta_s by Greig Cowan, Juan Palacios, Francesca Dordei, Carlos Vazquez Sierra
'''

__author__ = ['Greig Cowan','Juan Palacios','Francesca Dordei', 'Carlos Vazquez Sierra']
__date__ = '28/02/2015'
__version__ = '$Revision: 0.0 $'

__all__ = ('B2JpsiXforEM_sConf','default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from Configurables import LoKi__PVReFitter, CombineParticles
from CommonParticles.Utils import updateDoD
from StandardParticles import StdLoosePions
from StandardParticles import StdLooseKaons
from StandardParticles import StdAllLooseKaons
from StandardParticles import StdLooseResolvedPi0
from StandardParticles import StdLooseMergedPi0
from StandardParticles import StdNoPIDsPions
from StandardParticles import StdLooseProtons
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV

default_config = {
    'NAME'              : 'B2JpsiXforEM',
    'WGs'               : ['BandQ'],
    'BUILDERTYPE'       : 'B2JpsiXforEM_sConf',
    'CONFIG'    : {'ReFitPVsFlag'            :  True
                   ,'TRCHI2DOF'         :       5
                   ,'BPVLTIME'          :       0.2
                   ,'JpsiMassWindow'    :       80
                   ,'DaughterPT'        :       1000
                   ,'VCHI2PDOF'         :       10
                   ,'BuVCHI2PDOF'       :       10.0 #ok
                   ,'BdVCHI2PDOF'       :       10.0 #ok
                   ,'BsVCHI2PDOF'       :       10.0 #ok
                   ,'MinBsMass'         :       5150.0
                   ,'MaxBsMass'         :       5550.0
                   ,'MinBuMass'         :       5150.0 #ok
                   ,'MaxBuMass'         :       5550.0 #ok
                   ,'MinBdMass'         :       5150.0
                   ,'MaxBdMass'         :       5550.0
                   ,'CombMinBuMass'     :       5050.0 #ok
                   ,'CombMaxBuMass'     :       5650.0 #ok
                   ,'CombMinBdMass'     :       5050.0
                   ,'CombMaxBdMass'     :       5650.0
                   ,'CombMinBsMass'     :       5050.0
                   ,'CombMaxBsMass'     :       5650.0
                   ,'PIDKaon'           :        -2
                   ,'MinPhiMass'        :       980.0
                   ,'MaxPhiMass'        :       1050.0
                   ,'phiPT'             :       500.0
                   ,'phiVCHI2'          :       16.0
                   ,'MinKstarMass'      :       826.0
                   ,'MaxKstarMass'      :       966.0
                   ,'KstarPT'           :       500.0
                   ,'KstarVCHI2'        :       16.0
                   ,'PTKaon'            :       500.
                   
                   
                   },
    'STREAMS' : {'Dimuon' : ['StrippingB2JpsiXforEMBu2JpsiKDetachedLine',
                             'StrippingB2JpsiXforEMBd2JpsiKstarDetachedLine',
                             'StrippingB2JpsiXforEMBs2JpsiPhiDetachedLine'
                             ]
                 }
    }

### Lines stored in this file:
# StrippingBetaSBu2JpsiKDetachedLine
# StrippingBetaSBs2JpsiPhiDetachedLine
# StrippingBetaSBd2JpsiKstarDetachedLine


class B2JpsiXforEM_sConf(LineBuilder) :
    __configuration_keys__ = ('ReFitPVsFlag',
                              'TRCHI2DOF',
                              'BPVLTIME',
                              'JpsiMassWindow',
                              'DaughterPT',
                              'VCHI2PDOF',
                              'BuVCHI2PDOF',
                              'BdVCHI2PDOF',
                              'BsVCHI2PDOF',
                              'MinBsMass',
                              'MaxBsMass',
                              'MinBuMass',
                              'MaxBuMass',
                              'MinBdMass',
                              'MaxBdMass',
                              'CombMinBuMass',
                              'CombMaxBuMass',
                              'CombMinBdMass',
                              'CombMaxBdMass',
                              'CombMinBsMass',
                              'CombMaxBsMass',
                              'PIDKaon',
                              'MinPhiMass',
                              'MaxPhiMass',        
                              'phiPT',
                              'phiVCHI2',
                              'MinKstarMass',
                              'MaxKstarMass',
                              'KstarPT',
                              'KstarVCHI2',
                              'PTKaon'
                              )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.name = name
        self.config = config
	
        # Define input daughter lists for various B -> J/psi X selections:

        self.WideJpsiList = DataOnDemand(Location = "Phys/StdLooseJpsi2MuMu/Particles")

        self.JpsiList = self.createSubSel( OutputList = 'NarrowJpsiForBandQ' + self.name,
                                           InputList = self.WideJpsiList,
                                           Cuts = "(PFUNA(ADAMASS('J/psi(1S)')) < %(JpsiMassWindow)s * MeV)" % self.config)
        

        self.NoIPKaonList = self.createSubSel( OutputList = "NoIPKaonsForBandQ" + self.name,
                                               InputList = DataOnDemand(Location = "Phys/StdAllNoPIDsKaons/Particles"),
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s )" % self.config )    
        
                        
        self.PhiList = self.createSubSel( OutputList = "Phi2KKForBandQ" + self.name,
                                          InputList = DataOnDemand(Location = "Phys/StdLoosePhi2KK/Particles"),
                                          #Cuts = "(in_range(980,M,1050))" \
                                          Cuts = "(in_range( %(MinPhiMass)s *MeV, M, %(MaxPhiMass)s *MeV) & (PT > %(phiPT)s *MeV ) & (VFASPF(VCHI2) < %(phiVCHI2)s ) & (MAXTREE('K+'==ABSID, TRCHI2DOF) < %(TRCHI2DOF)s ) & (MINTREE('K+'==ABSID, PIDK) > %(PIDKaon)s ))" % self.config)
        self.KstarList = self.createSubSel( OutputList = "Kstar2KpiForBandQ" + self.name,
                                            InputList = DataOnDemand(Location = "Phys/StdLooseKstar2Kpi/Particles"),
                                            #Cuts = "(in_range(826,M,966))" \
                                            Cuts = "(in_range( %(MinKstarMass)s *MeV, M, %(MaxKstarMass)s *MeV ) & (PT > %(KstarPT)s *MeV) & (VFASPF(VCHI2) < %(KstarVCHI2)s ) & (MAXTREE('K+'==ABSID,  TRCHI2DOF) < %(TRCHI2DOF)s ) & (MAXTREE('pi-'==ABSID, TRCHI2DOF) < %(TRCHI2DOF)s )  & (MINTREE('K+'==ABSID, PIDK) > %(PIDKaon)s ))" % self.config)
        #  & (MINTREE('K+'==ABSID, PIDK) > %(PIDKaon)s )
        
        
        
        self.makeBu2JpsiK()
        self.makeBs2JpsiPhi()
        
        self.makeBd2JpsiKstar()
        
        

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
                          ReFitPVs = False ) :
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( name=self.name+"combiner", DecayDescriptor = DecayDescriptor,
                                     DaughtersCuts = DaughterCuts,
                                     MotherCut = PostVertexCuts,
                                     CombinationCut = PreVertexCuts,
                                     ReFitPVs = ReFitPVs)
        
        if ReFitPVs:
            mypvrefitter=LoKi__PVReFitter("mypvrefitter")
            mypvrefitter.CheckTracksByLHCbIDs = True
            mypvrefitter.DeltaChi2 = 1.
            mypvrefitter.DeltaDistance = 100.
            combiner.IgnoreP2PVFromInputLocations = True
            combiner.addTool(mypvrefitter)
            combiner.PVReFitters.update( {"": "LoKi::PVReFitter/mypvrefitter"} )
            
        
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)
        
    def createCombinationsSel( self, OutputList,
	                  DecayDescriptors,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL",
                          ReFitPVs = False ) :
        '''For taking in multiple decay descriptors'''
        combiner = CombineParticles( name=self.name+"combiners",DecayDescriptors = DecayDescriptors,
                                 DaughtersCuts = DaughterCuts,
                                 MotherCut = PostVertexCuts,
                                 CombinationCut = PreVertexCuts,
                                 ReFitPVs = ReFitPVs)
        if ReFitPVs:
            mypvrefitter=LoKi__PVReFitter("mypvrefitter")
            mypvrefitter.CheckTracksByLHCbIDs = True
            mypvrefitter.DeltaChi2 = 1.
            mypvrefitter.DeltaDistance = 100.
            combiner.IgnoreP2PVFromInputLocations = True
            combiner.addTool(mypvrefitter)
            combiner.PVReFitters.update( {"": "LoKi::PVReFitter/mypvrefitter"} )

        
        return Selection ( OutputList,
                       Algorithm = combiner,
                       RequiredSelections = DaughterLists)

    
    def makeBu2JpsiK( self ):
        Bu2JpsiK = self.createCombinationSel( OutputList = "Bu2JpsiK" + self.name,
                                              DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                              DaughterLists = [ self.WideJpsiList, self.NoIPKaonList ],
                                              #DaughterCuts  = {"K+": "(PT > 500.*MeV)" },
                                              #DaughterCuts  = {"K+": "(PT > %(PTKaon)s *MeV)" %self.config },
                                              DaughterCuts  = {"K+": "ALL" %self.config },
                                              PreVertexCuts = "in_range( %(CombMinBuMass)s *MeV, AM, %(CombMaxBuMass)s *MeV)" %self.config,
                                              PostVertexCuts = "in_range( %(MinBuMass)s *MeV,M, %(MaxBuMass)s *MeV) & (VFASPF(VCHI2PDOF) < %(BuVCHI2PDOF)s)" % self.config,
                                              ReFitPVs = self.config["ReFitPVsFlag"]
                                              ) # for the other particles is 10.

        #Bu2JpsiKPrescaledLine = StrippingLine( self.name + "Bu2JpsiKPrescaledLine", algos = [ Bu2JpsiK ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config["Bu2JpsiKPrescale"], EnableFlavourTagging = True, MDSTFlag = True )

        Bu2JpsiKDetached = self.createSubSel( InputList = Bu2JpsiK, OutputList = Bu2JpsiK.name() + "Detached" + self.name,
                                              Cuts = "(CHILD('Beauty -> ^J/psi(1S) X', PFUNA(ADAMASS('J/psi(1S)'))) < %(JpsiMassWindow)s * MeV) & (BPVLTIME() > %(BPVLTIME)s *ps)" % self.config)

        Bu2JpsiKDetachedLine  = StrippingLine( self.name + "Bu2JpsiKDetachedLine", algos = [ Bu2JpsiKDetached ] )
        
        self.registerLine(Bu2JpsiKDetachedLine)
        #self.registerLine(Bu2JpsiKPrescaledLine)

    def makeBs2JpsiPhi( self ):
        Bs2JpsiPhi = self.createCombinationSel( OutputList = "Bs2JpsiPhi" + self.name,
                                                DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                                                DaughterLists  = [ self.WideJpsiList, self.PhiList ],
                                                PreVertexCuts = "in_range( %(CombMinBsMass)s *MeV, AM, %(CombMaxBsMass)s *MeV)" %self.config,
                                                PostVertexCuts = "in_range( %(MinBsMass)s *MeV,M, %(MaxBsMass)s *MeV) & (VFASPF(VCHI2PDOF) < %(BsVCHI2PDOF)s)" % self.config,
                                                ReFitPVs = self.config["ReFitPVsFlag"]
                                                )
				   
        #Bs2JpsiPhiPrescaledLine = StrippingLine( self.name + "Bs2JpsiPhiPrescaledLine", algos = [ Bs2JpsiPhi ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bs2JpsiPhiPrescale'], MDSTFlag = True, EnableFlavourTagging = True )

        Bs2JpsiPhiDetached = self.createSubSel( InputList = Bs2JpsiPhi,
                                                OutputList = Bs2JpsiPhi.name() + "Detached" + self.name,
                                                Cuts = "(CHILD('Beauty -> ^J/psi(1S) X', PFUNA(ADAMASS('J/psi(1S)'))) < %(JpsiMassWindow)s * MeV) & (BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )

        Bs2JpsiPhiDetachedLine  = StrippingLine( self.name + "Bs2JpsiPhiDetachedLine", algos = [ Bs2JpsiPhiDetached ] )
        
        #self.registerLine(Bs2JpsiPhiPrescaledLine)
        self.registerLine(Bs2JpsiPhiDetachedLine)

    def makeBd2JpsiKstar( self ):
        Bd2JpsiKstar = self.createCombinationSel( OutputList = "Bd2JpsiKstar" + self.name,
                                                  DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc",
                                                  DaughterLists  = [ self.WideJpsiList, self.KstarList ],
                                                  PreVertexCuts = "in_range( %(CombMinBdMass)s *MeV, AM, %(CombMaxBdMass)s *MeV)" %self.config,
                                                  #PostVertexCuts = "in_range(5150,M,5450) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config)
                                                  PostVertexCuts = "in_range( %(MinBdMass)s *MeV, M, %(MaxBdMass)s *MeV) & (VFASPF(VCHI2PDOF) < %(BdVCHI2PDOF)s)" % self.config,
                                                  ReFitPVs = self.config["ReFitPVsFlag"]
                                                  )

        #Bd2JpsiKstarPrescaledLine = StrippingLine( self.name + "Bd2JpsiKstarPrescaledLine", algos = [ Bd2JpsiKstar ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bd2JpsiKstarPrescale'], MDSTFlag = True, EnableFlavourTagging = True )


        Bd2JpsiKstarDetached = self.createSubSel( InputList = Bd2JpsiKstar,
                                                  OutputList = Bd2JpsiKstar.name() + "Detached" + self.name,
                                                  Cuts = "(CHILD('Beauty -> ^J/psi(1S) X', PFUNA(ADAMASS('J/psi(1S)'))) < %(JpsiMassWindow)s * MeV) & (BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )

        Bd2JpsiKstarDetachedLine  = StrippingLine( self.name + "Bd2JpsiKstarDetachedLine",
                                          algos = [ Bd2JpsiKstarDetached ]  )

        #self.registerLine(Bd2JpsiKstarPrescaledLine)
        self.registerLine(Bd2JpsiKstarDetachedLine)

        
    def filterTisTos( self, name, DiMuonInput, myTisTosSpecs ) :
        from Configurables import TisTosParticleTagger
    
        myTagger = TisTosParticleTagger(name + "_TisTosTagger")
        myTagger.TisTosSpecs = myTisTosSpecs
        
        # To speed it up, TisTos only with tracking system:
        myTagger.ProjectTracksToCalo = False
        myTagger.CaloClustForCharged = False
        myTagger.CaloClustForNeutral = False
        myTagger.TOSFrac = { 4:0.0, 5:0.0 }

        return Selection(name + "_SelTisTos",
                     Algorithm = myTagger,
                     RequiredSelections = [ DiMuonInput ] ) 

