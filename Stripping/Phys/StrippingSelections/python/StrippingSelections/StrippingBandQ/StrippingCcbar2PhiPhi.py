'''
Module for selecting Ccbar->PhiPhi, including two lines:
1. Prompt line, with tight PT, PID cuts, requiring Hlt Tis, since there is no lifetime unbiased phi trigger yet.
2. Detached line, with loose PT, PID cuts, but with IPS cuts on Kaons. 

To include it:

from StrippingSelections.StrippingCcbar2PhiPhi import Ccbar2PhiPhiConf
from StrippingSelections.StrippingCcbar2PhiPhi import config_default as config_Ccbar2PhiPhi
confCcbar2PhiPhi = Ccbar2PhiPhiConf( name = 'Ccbar2PhiPhi', config = config_Ccbar2PhiPhi )
stream.appendLines( confCcbar2PhiPhi.lines() )
'''

__author__=['Jibo He']
__date__ = '20/01/2012'

__all__ = (
    'Ccbar2PhiPhiConf'
    )

default_config = {
    'NAME'        : 'Ccbar2Phi', 
    'BUILDERTYPE' : 'Ccbar2PhiPhiConf',
    'CONFIG' : {
        'TRCHI2DOF'        :     5.  ,
        'KaonPIDK'         :     5.  ,
        'KaonPT'           :   650.  , # MeV
        'PhiVtxChi2'       :    16.  ,
        'PhiMassW'         :    12.  , 
        'CombMaxMass'      :  4100.  , # MeV, before Vtx fit
        'CombMinMass'      :  2750.  , # MeV, before Vtx fit
        'MaxMass'          :  4000.  , # MeV, after Vtx fit
        'MinMass'          :  2800.  , # MeV, after Vtx fit
        'Phi_TisTosSpecs'  : { "Hlt1Global%TIS" : 0 },
        'PionCuts'         :  "(PT>0.7*GeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>36.) & (PIDK<10)" ,
        'KaonCuts'         :  "(PT>0.5*GeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>25.) & (PIDK>5)",
        'LooseKaonCuts'    :  "(PT>0.5*GeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>9.)"
        },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'     : [ 'BandQ' ]
    }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdNoPIDsProtons, StdNoPIDsPions, StdNoPIDsDownPions

class Ccbar2PhiPhiConf(LineBuilder):
    
    __configuration_keys__ = (
        'TRCHI2DOF',
        'KaonPIDK',
        'KaonPT',
        'PhiVtxChi2',
        'PhiMassW', 
        'CombMaxMass',
        'CombMinMass',
        'MaxMass',
        'MinMass',
        'Phi_TisTosSpecs',
        'PionCuts',
        'KaonCuts',
        'LooseKaonCuts'
        )

    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config


        """
        Unbiased, require Hlt Tis 
        """
        self.PhiForJpsiList = self.createSubSel( OutputList = "PhiFor" + self.name,
                                                 InputList =  DataOnDemand( Location = 'Phys/StdTightPhi2KK/Particles' ), 
                                                 Cuts = "(VFASPF(VCHI2/VDOF)<%(PhiVtxChi2)s)"\
                                                 " & (ADMASS('phi(1020)')<%(PhiMassW)s*MeV)"\
                                                 " & (INTREE( ('K+'==ID) & (PT> %(KaonPT)s*MeV) & (TRCHI2DOF < %(TRCHI2DOF)s) & (PIDK> %(KaonPIDK)s) ))"\
                                                 " & (INTREE( ('K-'==ID) & (PT> %(KaonPT)s*MeV) & (TRCHI2DOF < %(TRCHI2DOF)s) & (PIDK> %(KaonPIDK)s) ))" % self.config )

        self.TisPhiForJpsiList = self.filterTisTos( "TisPhiFor" + self.name ,
                                                    PhiInput = self.PhiForJpsiList,
                                                    myTisTosSpecs = config['Phi_TisTosSpecs']
                                                    )        
        
        
        """
        Detached 
        """
        self.DetachedPhiForJpsiList = self.createSubSel( OutputList = "DetachedPhiFor" + self.name,
                                                         InputList =  DataOnDemand( Location = 'Phys/StdLooseDetachedPhi2KK/Particles' ), 
                                                         Cuts = "(VFASPF(VCHI2/VDOF)<%(PhiVtxChi2)s)"\
                                                         " & (ADMASS('phi(1020)')<%(PhiMassW)s*MeV )"\
                                                         " & (MAXTREE('K+'==ABSID, TRCHI2DOF) < %(TRCHI2DOF)s )" \
                                                         " & (MINTREE('K+'==ABSID, PIDK)>0)" % self.config)

        """
        Pion 
        """
        self.PionForCcbar = self.createSubSel( OutputList = "PionFor" + self.name,
                                               InputList =  DataOnDemand( Location = 'Phys/StdLoosePions/Particles' ), 
                                               Cuts = config['PionCuts'] 
                                               )

        """
        Kaon
        """
        self.KaonForCcbar = self.createSubSel( OutputList = "KaonFor" + self.name,
                                               InputList =  DataOnDemand( Location = 'Phys/StdLooseKaons/Particles' ), 
                                               Cuts = config['KaonCuts'] 
                                               )

        """
        Loose Kaon
        """
        self.LooseKaonForCcbar = self.createSubSel( OutputList = "LooseKaonFor" + self.name,
                                                    InputList =  DataOnDemand( Location = 'Phys/StdLooseKaons/Particles' ),
                                                    Cuts = config['LooseKaonCuts'] 
                                                    )        
                                                 



        self.makeJpsi2PhiPhi()
        self.makeDetachedJpsi2PhiPhi()
        self.makeDetachedJpsi2PhiKK()
        self.makeDetachedJpsi2PhiPiPi()
        self.makeBs2TriPhi()
        
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
                              PreVertexCuts = "AALL",
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
    
    
    
    def makeJpsi2PhiPhi(self):
        Jpsi2PhiPhi = self.createCombinationSel( OutputList = "Jpsi2PhiPhi" + self.name,
                                                 DecayDescriptor = " J/psi(1S) -> phi(1020) phi(1020)", 
                                                 DaughterLists = [ self.TisPhiForJpsiList ], 
                                                 PreVertexCuts = "(in_range( %(CombMinMass)s *MeV, AM, %(CombMaxMass)s *MeV))" % self.config,
                                                 PostVertexCuts = "(in_range( %(MinMass)s *MeV, MM, %(MaxMass)s *MeV)) & (VFASPF(VCHI2PDOF) < 16 )" %self.config )
        
        Jpsi2PhiPhiLine = StrippingLine( self.name + "PhiLine",
                                         algos = [ Jpsi2PhiPhi ] )
        
        self.registerLine(Jpsi2PhiPhiLine)
        
        
    def makeDetachedJpsi2PhiPhi(self):
        DetachedJpsi2PhiPhi = self.createCombinationSel( OutputList = "DetachedJpsi2PhiPhi" + self.name,
                                                         DecayDescriptor = " J/psi(1S) -> phi(1020) phi(1020)", 
                                                         DaughterLists = [ self.DetachedPhiForJpsiList ],                                                          
                                                         PreVertexCuts = "AM>2.65*GeV",
                                                         PostVertexCuts = "(MM>2.7*GeV) & (VFASPF(VCHI2PDOF)<16) & (BPVDLS>10)" %self.config )
        
        DetachedJpsi2PhiPhiLine = StrippingLine( self.name + "PhiDetachedLine",
                                                 algos = [ DetachedJpsi2PhiPhi ] )
        
        self.registerLine(DetachedJpsi2PhiPhiLine)

    def makeDetachedJpsi2PhiKK(self):
        DetachedJpsi2PhiKK = self.createCombinationSel( OutputList = "DetachedJpsi2PhiKK" + self.name,
                                                        DecayDescriptor = " J/psi(1S) -> phi(1020) K+ K-", 
                                                        DaughterLists = [ self.DetachedPhiForJpsiList,
                                                                          self.KaonForCcbar
                                                                          ], 
                                                        DaughterCuts  = { "phi(1020)": """
                                                                            (INTREE( (ID=='K+') & (PT>500*MeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>25.) & (PIDK>5)))
                                                                          & (INTREE( (ID=='K-') & (PT>500*MeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>25.) & (PIDK>5)))
                                                                          """ },
                                                        PreVertexCuts = "AM>2.65*GeV",
                                                        PostVertexCuts = "(MM>2.7*GeV) & (VFASPF(VCHI2PDOF)<16) & (BPVDLS>10)" %self.config )
        
        DetachedJpsi2PhiKKLine = StrippingLine( self.name + "KKDetachedLine",
                                                 algos = [ DetachedJpsi2PhiKK ] )
        
        self.registerLine(DetachedJpsi2PhiKKLine)

        """
        Bs->Jpsi Phi
        """
        Bs2JpsiPhi = self.createCombinationSel( OutputList = "Bs2JpsiPhi" + self.name,
                                                DecayDescriptor = " B_s0 -> J/psi(1S) phi(1020)", 
                                                DaughterLists = [ DetachedJpsi2PhiKK, 
                                                                  self.DetachedPhiForJpsiList ], 
                                                PostVertexCuts = "(VFASPF(VCHI2PDOF)<16)" %self.config )

        Bs2JpsiPhiLine = StrippingLine( self.name + "Bs2JpsiPhiLine",
                                        algos = [ Bs2JpsiPhi ] )
        
        self.registerLine( Bs2JpsiPhiLine )

        """
        B2JpsiK
        """
        B2JpsiK = self.createCombinationSel( OutputList = "B2JpsiK" + self.name,
                                             DecayDescriptor = " [ B+ -> J/psi(1S) K+ ]cc", 
                                             DaughterLists = [ DetachedJpsi2PhiKK, 
                                                               self.LooseKaonForCcbar ], 
                                             PostVertexCuts = "(VFASPF(VCHI2PDOF)<16)" %self.config )

        B2JpsiKLine = StrippingLine( self.name + "B2JpsiKLine",
                                        algos = [ B2JpsiK ] )
        
        self.registerLine( B2JpsiKLine )        
        


    def makeDetachedJpsi2PhiPiPi(self):
        DetachedJpsi2PhiPiPi = self.createCombinationSel( OutputList = "DetachedJpsi2PhiPiPi" + self.name,
                                                          DecayDescriptor = " J/psi(1S) -> phi(1020) pi+ pi-", 
                                                          DaughterLists = [ self.DetachedPhiForJpsiList,
                                                                            self.PionForCcbar
                                                                            ], 
                                                          DaughterCuts  = { "phi(1020)": """                                                                          
                                                                            (INTREE( (ID=='K+') & (PT>500*MeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>25.) & (PIDK>5)))
                                                                          & (INTREE( (ID=='K-') & (PT>500*MeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>25.) & (PIDK>5)))
                                                                          """},
                                                          PreVertexCuts = "AM>2.65*GeV",
                                                          PostVertexCuts = "(MM>2.7*GeV) & (VFASPF(VCHI2PDOF)<16) & (BPVDLS>10)" %self.config )
        
        DetachedJpsi2PhiPiPiLine = StrippingLine( self.name + "PiPiDetachedLine",
                                                  algos = [ DetachedJpsi2PhiPiPi ] )
        
        self.registerLine(DetachedJpsi2PhiPiPiLine)     

        
    def makeBs2TriPhi(self):
        Bs2TriPhi = self.createCombinationSel( OutputList = "Bs2TriPhi" + self.name,
                                               DecayDescriptor = " B_s0 -> phi(1020) phi(1020) phi(1020)", 
                                               DaughterLists = [ self.DetachedPhiForJpsiList ], 
                                               PreVertexCuts = "AM>2.65*GeV",
                                               PostVertexCuts = "(MM>2.7*GeV) & (VFASPF(VCHI2PDOF)<16)" %self.config )
        
        Bs2TriPhiLine = StrippingLine( self.name + "Bs2TriPhiLine",
                                       algos = [ Bs2TriPhi ] )
        
        self.registerLine( Bs2TriPhiLine )        


    def filterTisTos(self, name,
                     PhiInput,
                     myTisTosSpecs ) :
        from Configurables import TisTosParticleTagger
        
        myTagger = TisTosParticleTagger(name + "_TisTosTagger")
        myTagger.TisTosSpecs = myTisTosSpecs
        
        # To speed it up, TisTos only with tracking system)
        myTagger.ProjectTracksToCalo = False
        myTagger.CaloClustForCharged = False
        myTagger.CaloClustForNeutral = False
        myTagger.TOSFrac = { 4:0.0, 5:0.0 }
        
        return Selection(name + "_SelTisTos",
                         Algorithm = myTagger,
                         RequiredSelections = [ PhiInput ] )
    
