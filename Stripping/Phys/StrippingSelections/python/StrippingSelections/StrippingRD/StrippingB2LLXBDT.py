'''
Module for selecting B->LL (mumu, ee) X,
RelatedInfoToos stolen from StrippingBu2LLK
Note:
*. CPU-intensive cuts like IPCHI2 are not re-applied
   if being identical to those in the common particles
'''

__author__=['Jibo He']
__date__ = '20/11/2015'
__version__= '$Revision: 1.0 $'


__all__ = (
    'B2LLXBDTConf',
    'default_config'
    )

default_config = {
    'NAME'              :  'B2LLXBDT',
    'BUILDERTYPE'       :  'B2LLXBDTConf',
    'CONFIG'    : {
        'DiElectronCuts': """
                           (HASVERTEX) & (VFASPF(VCHI2)<16) & (MM<5.0*GeV)
                           & (INTREE( (ID=='e+') & (PT>200*MeV) & (BPVIPCHI2()>1.) & (PIDe>-2) & (TRGHOSTPROB<0.5) ))
                           & (INTREE( (ID=='e-') & (PT>200*MeV) & (BPVIPCHI2()>1.) & (PIDe>-2) & (TRGHOSTPROB<0.5) ))
                          """,
        'DiMuonCuts'    : """
                           (HASVERTEX) & (VFASPF(VCHI2)<16) & (MM<5.0*GeV)
                           & (INTREE( (ID=='mu+') & (PT>200*MeV) & (BPVIPCHI2()>1.) & (TRGHOSTPROB<0.5) ))
                           & (INTREE( (ID=='mu-') & (PT>200*MeV) & (BPVIPCHI2()>1.) & (TRGHOSTPROB<0.5) ))
                          """,
        'PionCuts'      : "(PROBNNpi > 0.2) & (PT > 250*MeV) & (TRGHOSTPROB<0.4)",
        'KaonCuts'      : "(PROBNNk > 0.1)  & (PT > 300*MeV) & (TRGHOSTPROB<0.4)",
        'ProtonCuts'    : "(PROBNNp > 0.1)  & (PT > 300*MeV) & (TRGHOSTPROB<0.4)",

        'Kaon4BuCuts'   : "(PROBNNk > 0.3) & (PT > 800*MeV) & (TRGHOSTPROB<0.4) & (BPVIPCHI2()>9.)",

        'KstarCuts'     : "(VFASPF(VCHI2/VDOF)<16) & (ADMASS('K*(892)0')< 300*MeV)",
        'KsDDCuts'      : "(ADMASS('KS0') < 30.*MeV) & (BPVDLS>5)",
        'KsLLCuts'      : "(ADMASS('KS0') < 30.*MeV) & (BPVDLS>5)",
        
        'PhiCuts'       : """
                          (MAXTREE(ABSID=='K+',TRGHOSTPROB) < 0.4)
                          & (MM<1.05*GeV)
                          & (PT> 800*MeV)
                          & (MIPCHI2DV(PRIMARY) > 3.)
                          & (VFASPF(VCHI2) < 9.)
                          """ ,

        'LambdaDDCuts'  : "(ADMASS('Lambda0') < 30.*MeV) & (BPVDLS>5)",
        'LambdaLLCuts'  : "(ADMASS('Lambda0') < 30.*MeV) & (BPVDLS>5)",

        'LambdastarComCuts' : "(AM < 5.6*GeV)",
        'LambdastarCuts': "(VFASPF(VCHI2) < 25.)",
        
        'BComCuts'      : "(AM > 4.0*GeV) & (AM < 6.2*GeV)",
        'BMomCuts'      : "(VFASPF(VCHI2/VDOF) < 25.) & (BPVDIRA> 0.999) & (BPVIPCHI2()<25) & (BPVDLS>0)",

        'LbComCuts'      : "(AM > 4.0*GeV) & (AM < 6.6*GeV)",
        'LbMomCuts'      : "(VFASPF(VCHI2/VDOF) < 25.) & (BPVDIRA> 0.999) & (BPVIPCHI2()<25) & (BPVDLS>0)",

        'Bd2eeKstarMVACut'   :  "-0.98",
        'Bd2mumuKstarMVACut' :  "-0.98",
        'Bd2LLKstarXmlFile'  :  '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml',

        'Lb2eePKMVACut'    :  "-0.99",
        'Lb2mumuPKMVACut'  :  "-0.99",
        'Lb2LLPKXmlFile'   :  '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml',

        'RelatedInfoTools'     : [
            {'Type'              : 'RelInfoVertexIsolation',
             'Location'          : 'VertexIsoInfo'},
            {'Type'              : 'RelInfoVertexIsolationBDT',
             'Location'          : 'VertexIsoBDTInfo'},
            {'Type'              : 'RelInfoConeVariables',
             'ConeAngle'         : 0.5,
             'IgnoreUnmatchedDescriptors': True, 
             'DaughterLocations' : { 
                    # OPPOSITE SIGN
                    # 3-body
                    "[Beauty -> ^StableCharged (X0 ->  l+  l-)]CC" : "TrackIsoInfoH",
                    "[Beauty ->  StableCharged (X0 -> ^l+  l-)]CC" : "TrackIsoInfoL1",
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l-)]CC" : "TrackIsoInfoL2",
                    # 4-body
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  StableCharged  StableCharged)]CC" : "TrackIsoInfoH1",
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  StableCharged  StableCharged)]CC" : "TrackIsoInfoH2",
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^StableCharged  StableCharged)]CC" : "TrackIsoInfoL1",
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  StableCharged ^StableCharged)]CC" : "TrackIsoInfoL2"
                    }
             },
            {'Type'              : 'RelInfoConeIsolation',
             'ConeSize'          : 0.5,
             'IgnoreUnmatchedDescriptors': True, 
             'DaughterLocations' : { 
                    # OPPOSITE SIGN
                    # 3-body
                    "[Beauty -> ^StableCharged (X0 ->  l+  l-)]CC" : "ConeIsoInfoH",
                    "[Beauty ->  StableCharged (X0 -> ^l+  l-)]CC" : "ConeIsoInfoL1",
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l-)]CC" : "ConeIsoInfoL2",
                    # 4-body
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  StableCharged  StableCharged)]CC" : "ConeIsoInfoH1",
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  StableCharged  StableCharged)]CC" : "ConeIsoInfoH2",
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^StableCharged  StableCharged)]CC" : "ConeIsoInfoL1",
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  StableCharged ^StableCharged)]CC" : "ConeIsoInfoL2",
                    }
             },
            {'Type'              : 'RelInfoTrackIsolationBDT',
             'IgnoreUnmatchedDescriptors': True,
             'DaughterLocations' : { 
                    # OPPOSITE SIGN
                    # 3-body
                    "[Beauty -> ^StableCharged (X0 ->  l+  l-)]CC" : "TrackIsoBDTInfoH",
                    "[Beauty ->  StableCharged (X0 -> ^l+  l-)]CC" : "TrackIsoBDTInfoL1",
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l-)]CC" : "TrackIsoBDTInfoL2",
                    # 4-body
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  StableCharged  StableCharged)]CC" : "TrackIsoBDTInfoH1",
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  StableCharged  StableCharged)]CC" : "TrackIsoBDTInfoH2",
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^StableCharged  StableCharged)]CC" : "TrackIsoBDTInfoL1",
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  StableCharged ^StableCharged)]CC" : "TrackIsoBDTInfoL2"
                    }
             },
            {'Type'              : 'RelInfoBs2MuMuTrackIsolations',
             'IgnoreUnmatchedDescriptors': True,
             'DaughterLocations' : { 
                    # OPPOSITE SIGN
                    # 3-body
                    "[Beauty -> ^StableCharged (X0 ->  l+  l-)]CC" : "TrackIsoBs2MMInfoH",
                    "[Beauty ->  StableCharged (X0 -> ^l+  l-)]CC" : "TrackIsoBs2MMInfoL1",
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l-)]CC" : "TrackIsoBs2MMInfoL2",
                    # 4-body
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  StableCharged  StableCharged)]CC" : "TrackIsoBs2MMInfoH1",
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  StableCharged  StableCharged)]CC" : "TrackIsoBs2MMInfoH2",
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^StableCharged  StableCharged)]CC" : "TrackIsoBs2MMInfoL1",
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  StableCharged ^StableCharged)]CC" : "TrackIsoBs2MMInfoL2"
                    },
             }
            ]        
        },
    'STREAMS'           : ['Leptonic' ],
    'WGs'               : ['RD']
    }


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles 
from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays
from PhysSelPython.Wrappers import Selection, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class B2LLXBDTConf(LineBuilder):
    
    __configuration_keys__ = (
        'DiElectronCuts',
        'DiMuonCuts',

        'KaonCuts',
        'PionCuts',
        'ProtonCuts',

        'Kaon4BuCuts',
        
        'KstarCuts',
        'KsDDCuts',
        'KsLLCuts',
        
        'PhiCuts',

        'LambdaDDCuts',
        'LambdaLLCuts',

        'LambdastarComCuts',
        'LambdastarCuts',
        
        'BComCuts',
        'BMomCuts',

        'LbComCuts',
        'LbMomCuts',
        
        'Bd2eeKstarMVACut',
        'Bd2mumuKstarMVACut',
        'Bd2LLKstarXmlFile',

        'Lb2eePKMVACut',
        'Lb2mumuPKMVACut',
        'Lb2LLPKXmlFile',
        
        'RelatedInfoTools'
        )

    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config

        """
        Di letpons
        """
        from StandardParticles import StdDiElectronFromTracks, StdLooseDiMuon

        self.SelDiElectron = self.createSubSel( OutputList = self.name + "SelDiElectron",
                                                InputList = StdDiElectronFromTracks , 
                                                Cuts = config['DiElectronCuts']
                                                )
        
        self.SelDiMuon = self.createSubSel( OutputList = self.name + "SelDiMuon",
                                             InputList = StdLooseDiMuon , 
                                             Cuts = config['DiMuonCuts']
                                             )
                
        """
        Basic particles, long tracks
        """
        from StandardParticles import StdLooseANNPions, StdLooseANNKaons, StdLooseANNProtons
        
        self.SelPions = self.createSubSel( OutputList = self.name + "SelPions",
                                           InputList =  StdLooseANNPions , 
                                           Cuts = config['PionCuts']
                                           )
        
        self.SelKaons = self.createSubSel( OutputList = self.name + "SelKaons",
                                           InputList = StdLooseANNKaons, 
                                           Cuts = config['KaonCuts']
                                           )
                                                   
        self.SelProtons = self.createSubSel( OutputList = self.name + "SelProtons",
                                           InputList = StdLooseANNProtons, 
                                           Cuts = config['ProtonCuts']
                                           )

        """
        Kstar
        """
        from StandardParticles import StdVeryLooseDetachedKst2Kpi
        
        self.SelKstar = self.createSubSel( OutputList = self.name + "SelKstar",
                                           InputList =  StdVeryLooseDetachedKst2Kpi, 
                                           Cuts = config['KstarCuts']
                                           )
        
        """
        Kshort, both LL and DD
        """
        from StandardParticles import StdLooseKsDD, StdLooseKsLL
        
        self.SelKsDD = self.createSubSel( OutputList = self.name + "SelKsDD",
                                          InputList = StdLooseKsDD,
                                          Cuts = config['KsDDCuts'] )

        self.SelKsLL = self.createSubSel( OutputList = self.name + "SelKsLL",
                                          InputList = StdLooseKsLL,
                                          Cuts = config['KsLLCuts'] )
        
        self.SelKs = MergedSelection( self.name + "SelKs",
                                      RequiredSelections = [ self.SelKsDD,
                                                             self.SelKsLL ] )

                                                           
        """
        Phi 
        """
        from StandardParticles import StdLoosePhi2KK
        
        self.SelPhi = self.createSubSel( OutputList = self.name + "SelPhi",
                                         InputList =  StdLoosePhi2KK, 
                                         Cuts = config['PhiCuts']
                                         )



        """
        Lambda, both LL and DD
        """
        from StandardParticles import StdLooseLambdaDD, StdLooseLambdaLL

        self.SelLambdaDD = self.createSubSel( OutputList = self.name + "SelLambdaDD",
                                              InputList = StdLooseLambdaDD,
                                              Cuts = config['LambdaDDCuts'] )
        
        self.SelLambdaLL = self.createSubSel( OutputList = self.name + "SelLambdaLL",
                                              InputList = StdLooseLambdaLL,
                                              Cuts = config['LambdaLLCuts'] )
        
        self.SelLambda = MergedSelection( self.name + "SelLambda",
                                          RequiredSelections = [ self.SelLambdaDD,
                                                                 self.SelLambdaLL ] )

        """
        Lambda* -> p K
        """
        self.SelLambdastar = self.createCombinationSel( OutputList = self.name + "SelLambdastar",
                                                        DecayDescriptor = "[Lambda(1520)0 -> p+ K-]cc",
                                                        DaughterLists = [ self.SelProtons, self.SelKaons ],
                                                        PreVertexCuts  = config['LambdastarComCuts'],
                                                        PostVertexCuts = config['LambdastarCuts'] )


        #
        #  Stripping lines here 
        #
        self.SelKaons4Bu = self.createSubSel( OutputList = self.name + "SelKaons4Bu",
                                              InputList = StdLooseANNKaons, 
                                              Cuts = config['Kaon4BuCuts']
                                              ) 
        
        """
        Bu2eeK
        """
        self.SelBu2eeK = self.createCombinationSel( OutputList = self.name + "SelBu2eeK",
                                                    DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                                    DaughterLists = [ self.SelDiElectron, self.SelKaons4Bu ],
                                                    PreVertexCuts  = config['BComCuts'],
                                                    PostVertexCuts = config['BMomCuts'] )

        self.Bu2eeKLine = StrippingLine( self.name + '_Bu2eeKLine',                                                
                                         RelatedInfoTools = config[ 'RelatedInfoTools'],                                        
                                         algos     = [ self.SelBu2eeK ],
                                         MDSTFlag  = True
                                         )

        self.registerLine( self.Bu2eeKLine )


        """
        Bu2mumuK
        """
        self.SelBu2mumuK = self.createCombinationSel( OutputList = self.name + "SelBu2mumuK",
                                                      DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                                      DaughterLists = [ self.SelDiMuon, self.SelKaons4Bu ],                    
                                                      PreVertexCuts  = config['BComCuts'],
                                                      PostVertexCuts = config['BMomCuts'] )
        
        self.Bu2mumuKLine = StrippingLine( self.name + '_Bu2mumuKLine',                                                
                                           RelatedInfoTools = config[ 'RelatedInfoTools'],
                                           algos     = [ self.SelBu2mumuK ],
                                           MDSTFlag  = True
                                           )

        self.registerLine( self.Bu2mumuKLine )
        

        #
        # Bd2LLKstar
        #
        """
        Bd2LLKstarVars
        """
        self.Bd2LLKstarVars = {
            "L1_IPCHI2_OWNPV"      : "CHILD(MIPCHI2DV(), 1, 1)",
            "L2_IPCHI2_OWNPV"      : "CHILD(MIPCHI2DV(), 1, 2)",
            "Jpsi_IPCHI2_OWNPV"    : "CHILD(MIPCHI2DV(), 1 )",
            "Kaon_IPCHI2_OWNPV"    : "CHILD(MIPCHI2DV(), 2, 1)",
            "Pion_IPCHI2_OWNPV"    : "CHILD(MIPCHI2DV(), 2, 2)",
            "Kstar_IPCHI2_OWNPV"   : "CHILD(MIPCHI2DV(), 2)",
            "B_IPCHI2_OWNPV"       : "BPVIPCHI2()" ,
            "L1_PT"                : "CHILD(PT, 1, 1)",
            "L2_PT"                : "CHILD(PT, 1, 2)", 
            "Jpsi_PT"              : "CHILD(PT, 1)",
            "Kaon_PT"              : "CHILD(PT, 2, 1)",
            "Pion_PT"              : "CHILD(PT, 2, 2)",
            "Kstar_PT"             : "CHILD(PT, 2)",
            "B_PT"                 : "PT",
            "Jpsi_FDCHI2_OWNPV"    : "CHILD(BPVVDCHI2,1)",
            "Kstar_FDCHI2_OWNPV"   : "CHILD(BPVVDCHI2,2)",
            "B_FDCHI2_OWNPV"       : "BPVVDCHI2" ,
            "B_DIRA_OWNPV"         : "BPVDIRA"
            }


        """
        Bd2eeKstar
        """
        self.SelBd2eeKstar = self.createCombinationSel( OutputList = self.name + "SelBd2eeKstar",
                                                        DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc",
                                                        DaughterLists = [ self.SelDiElectron, self.SelKstar ],
                                                        PreVertexCuts  = config['BComCuts'],
                                                        PostVertexCuts = config['BMomCuts'] )

        self.MvaBd2eeKstar = self.applyMVA( self.name + "MvaBd2eeKstar",
                                            SelB        = self.SelBd2eeKstar,
                                            MVAVars     = self.Bd2LLKstarVars,
                                            MVACutValue = config['Bd2eeKstarMVACut'], 
                                            MVAxmlFile  = config['Bd2LLKstarXmlFile']
                                            )

        self.Bd2eeKstarLine = StrippingLine( self.name + '_Bd2eeKstarLine',                                                
                                             RelatedInfoTools = config[ 'RelatedInfoTools'], 
                                             algos     = [ self.MvaBd2eeKstar ],
                                             MDSTFlag  = True
                                             )

        self.registerLine( self.Bd2eeKstarLine )

        """
        Bd2mumuKstar
        """
        self.SelBd2mumuKstar = self.createCombinationSel( OutputList = self.name + "SelBd2mumuKstar",
                                                          DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc",
                                                          DaughterLists = [ self.SelDiMuon, self.SelKstar ],
                                                          PreVertexCuts  = config['BComCuts'],
                                                          PostVertexCuts = config['BMomCuts'] )

        self.MvaBd2mumuKstar = self.applyMVA( self.name + "MvaBd2mumuKstar",
                                              SelB        = self.SelBd2mumuKstar,
                                              MVAVars     = self.Bd2LLKstarVars,
                                              MVACutValue = config['Bd2mumuKstarMVACut'], 
                                              MVAxmlFile  = config['Bd2LLKstarXmlFile']
                                              )
                
        self.Bd2mumuKstarLine = StrippingLine( self.name + '_Bd2mumuKstarLine',                                                
                                               RelatedInfoTools = config[ 'RelatedInfoTools'],
                                               algos     = [ self.MvaBd2mumuKstar ],
                                               MDSTFlag  = True
                                               )

        self.registerLine( self.Bd2mumuKstarLine )

        """
        Bd2eeKs
        """
        self.SelBd2eeKs = self.createCombinationSel( OutputList = self.name + "SelBd2eeKs",
                                                     DecayDescriptor = "B0 -> J/psi(1S) KS0",
                                                     DaughterLists = [ self.SelDiElectron, self.SelKs ],
                                                     PreVertexCuts  = config['BComCuts'],
                                                     PostVertexCuts = config['BMomCuts'] )
        
        self.Bd2eeKsLine = StrippingLine( self.name + '_Bd2eeKsLine',                                                
                                          RelatedInfoTools = config[ 'RelatedInfoTools'],                                        
                                          algos     = [ self.SelBd2eeKs ],
                                          MDSTFlag  = True
                                          )
        
        self.registerLine( self.Bd2eeKsLine )


        """
        Bd2mumuKs
        """
        self.SelBd2mumuKs = self.createCombinationSel( OutputList = self.name + "SelBd2mumuKs",
                                                       DecayDescriptor = "B0 -> J/psi(1S) KS0",
                                                       DaughterLists = [ self.SelDiMuon, self.SelKs ],
                                                       PreVertexCuts  = config['BComCuts'],
                                                       PostVertexCuts = config['BMomCuts'] )
        
        self.Bd2mumuKsLine = StrippingLine( self.name + '_Bd2mumuKsLine',                                                
                                            RelatedInfoTools = config[ 'RelatedInfoTools'],
                                            algos     = [ self.SelBd2mumuKs ],
                                            MDSTFlag  = True
                                            )
        
        self.registerLine( self.Bd2mumuKsLine )


        """
        Bs2eePhi
        """
        self.SelBs2eePhi = self.createCombinationSel( OutputList = self.name + "SelBs2eePhi",
                                                      DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                                                      DaughterLists = [ self.SelDiElectron, self.SelPhi ],
                                                      PreVertexCuts  = config['BComCuts'],
                                                      PostVertexCuts = config['BMomCuts'] )
        
        self.Bs2eePhiLine = StrippingLine( self.name + '_Bs2eePhiLine',                                                
                                           RelatedInfoTools = config[ 'RelatedInfoTools'],
                                           algos     = [ self.SelBs2eePhi ],
                                           MDSTFlag  = True
                                           )
        
        self.registerLine( self.Bs2eePhiLine )

        
        """
        Bs2mumuPhi
        """
        self.SelBs2mumuPhi = self.createCombinationSel( OutputList = self.name + "SelBs2mumuPhi",
                                                        DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                                                        DaughterLists = [ self.SelDiMuon, self.SelPhi ],
                                                        PreVertexCuts  = config['BComCuts'],
                                                        PostVertexCuts = config['BMomCuts'] )
        
        self.Bs2mumuPhiLine = StrippingLine( self.name + '_Bs2mumuPhiLine',                                                
                                             RelatedInfoTools = config[ 'RelatedInfoTools'],
                                             algos     = [ self.SelBs2mumuPhi ],
                                             MDSTFlag  = True
                                             )
        
        self.registerLine( self.Bs2mumuPhiLine )


        """
        Lb2eeLambda
        """
        self.SelLb2eeLambda = self.createCombinationSel( OutputList = self.name + "SelLb2eeLambda",
                                                         DecayDescriptor = "[Lambda_b0 -> J/psi(1S) Lambda0]cc",
                                                         DaughterLists = [ self.SelDiElectron, self.SelLambda ],
                                                         PreVertexCuts  = config['LbComCuts'],
                                                         PostVertexCuts = config['LbMomCuts'] )
        
        self.Lb2eeLambdaLine = StrippingLine( self.name + '_Lb2eeLambdaLine',                                                
                                              RelatedInfoTools = config[ 'RelatedInfoTools'],                                        
                                              algos     = [ self.SelLb2eeLambda ],
                                              MDSTFlag  = True
                                              )
        
        self.registerLine( self.Lb2eeLambdaLine )


        """
        Lb2mumuLambda
        """
        self.SelLb2mumuLambda = self.createCombinationSel( OutputList = self.name + "SelLb2mumuLambda",
                                                           DecayDescriptor = "[Lambda_b0 -> J/psi(1S) Lambda0]cc",
                                                           DaughterLists = [ self.SelDiMuon, self.SelLambda ],
                                                           PreVertexCuts  = config['LbComCuts'],
                                                           PostVertexCuts = config['LbMomCuts'] )
        
        self.Lb2mumuLambdaLine = StrippingLine( self.name + '_Lb2mumuLambdaLine',                                                
                                                RelatedInfoTools = config[ 'RelatedInfoTools'],
                                                algos     = [ self.SelLb2mumuLambda ],
                                                MDSTFlag  = True
                                                )
        
        self.registerLine( self.Lb2mumuLambdaLine )

        
        #
        # Lb-> LL Kstar
        #
        self.Lb2LLPKVars = self.Bd2LLKstarVars
        
        """
        Lb2eePK
        """
        
        self.SelLb2eePK = self.createCombinationSel( OutputList = self.name + "SelLb2eePK",
                                                     DecayDescriptor = "[Lambda_b0 -> J/psi(1S) Lambda(1520)0]cc",
                                                     DaughterLists = [ self.SelDiElectron, self.SelLambdastar ],
                                                     PreVertexCuts  = config['LbComCuts'],
                                                     PostVertexCuts = config['LbMomCuts'] )
        
        self.MvaLb2eePK = self.applyMVA( self.name + "MvaLb2eePK",
                                         SelB        = self.SelLb2eePK,
                                         MVAVars     = self.Lb2LLPKVars,
                                         MVACutValue = config['Lb2eePKMVACut'], 
                                         MVAxmlFile  = config['Lb2LLPKXmlFile']
                                         )
        
        self.Lb2eePKLine = StrippingLine( self.name + '_Lb2eePKLine',
                                          RelatedInfoTools = config[ 'RelatedInfoTools'],                                        
                                          algos     = [ self.MvaLb2eePK ],
                                          MDSTFlag  = True
                                          )
        
        self.registerLine( self.Lb2eePKLine )


        """
        Lb2mumuPK
        """
        self.SelLb2mumuPK = self.createCombinationSel( OutputList = self.name + "SelLb2mumuPK",
                                                       DecayDescriptor = "[Lambda_b0 -> J/psi(1S) Lambda(1520)0]cc",
                                                       DaughterLists = [ self.SelDiMuon, self.SelLambdastar ],
                                                       PreVertexCuts  = config['LbComCuts'],
                                                       PostVertexCuts = config['LbMomCuts'] )

        self.MvaLb2mumuPK = self.applyMVA( self.name + "MvaLb2mumuPK",
                                           SelB        = self.SelLb2mumuPK,
                                           MVAVars     = self.Lb2LLPKVars,
                                           MVACutValue = config['Lb2mumuPKMVACut'], 
                                           MVAxmlFile  = config['Lb2LLPKXmlFile']
                                           )
        
        self.Lb2mumuPKLine = StrippingLine( self.name + '_Lb2mumuPKLine',       
                                            RelatedInfoTools = config[ 'RelatedInfoTools'], 
                                            algos     = [ self.MvaLb2mumuPK ],
                                            MDSTFlag  = True
                                            )
        
        self.registerLine( self.Lb2mumuPKLine )
        
        

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
                                     ReFitPVs = False)
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)

    def applyMVA( self, name, 
                  SelB,
                  MVAVars,
                  MVAxmlFile,
                  MVACutValue
                  ):
        from MVADictHelpers import addTMVAclassifierValue
        from Configurables import FilterDesktop as MVAFilterDesktop

        _FilterB = MVAFilterDesktop( name + "Filter",
                                     Code = "VALUE('LoKi::Hybrid::DictValue/" + name + "')>" + MVACutValue  )

        addTMVAclassifierValue( Component = _FilterB,
                                XMLFile   = MVAxmlFile,
                                Variables = MVAVars,
                                ToolName  = name )
        
        return Selection( name,
                          Algorithm =  _FilterB,
                          RequiredSelections = [ SelB ] )
