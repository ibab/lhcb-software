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
                           & (INTREE( (ID=='e+') & (PT>200*MeV) & (MIPCHI2DV(PRIMARY)>1.) & (PIDe>-2) & (TRGHOSTPROB<0.5) ))
                           & (INTREE( (ID=='e-') & (PT>200*MeV) & (MIPCHI2DV(PRIMARY)>1.) & (PIDe>-2) & (TRGHOSTPROB<0.5) ))
                          """,
        'DiMuonCuts'    : """
                           (HASVERTEX) & (VFASPF(VCHI2)<16) & (MM<5.0*GeV)
                           & (INTREE( (ID=='mu+') & (PT>200*MeV) & (MIPCHI2DV(PRIMARY)>1.) & (TRGHOSTPROB<0.5) ))
                           & (INTREE( (ID=='mu-') & (PT>200*MeV) & (MIPCHI2DV(PRIMARY)>1.) & (TRGHOSTPROB<0.5) ))
                          """,
        'PionCuts'      : "(PROBNNpi> 0.2) & (PT>250*MeV) & (TRGHOSTPROB<0.4)",
        'KaonCuts'      : "(PROBNNk > 0.1) & (PT>300*MeV) & (TRGHOSTPROB<0.4)",
        'ProtonCuts'    : "(PROBNNp> 0.05) & (PT>300*MeV) & (TRGHOSTPROB<0.4)",

        'Pion4LPCuts'   : "(PROBNNpi> 0.2) & (PT>100*MeV) & (TRGHOSTPROB<0.4) & (MIPCHI2DV(PRIMARY)>9.)",

        'KstarCuts'     : "(VFASPF(VCHI2/VDOF)<16) & (ADMASS('K*(892)0')< 300*MeV)",

        'KsDDCuts'      : "(ADMASS('KS0') < 30.*MeV) & (BPVVDCHI2>25)",
        'KsLLComCuts'   : "(ADAMASS('KS0') < 50.*MeV) & (ADOCACHI2CUT(25, ''))",  
        'KsLLCuts'      : "(ADMASS('KS0') < 30.*MeV) & (BPVVDCHI2>25) & (VFASPF(VCHI2) < 25.)",
        
        'PhiCuts'       : """
                          (HASVERTEX) & (VFASPF(VCHI2)<16) & (MM<1.05*GeV) & (MIPCHI2DV(PRIMARY)>2.)
                          & (INTREE( (ID=='K+') & (PT>200*MeV) & (TRGHOSTPROB<0.4) ))
                          & (INTREE( (ID=='K-') & (PT>200*MeV) & (TRGHOSTPROB<0.4) ))
                          """ ,

        'LambdaDDCuts'  : "(ADMASS('Lambda0') < 30.*MeV) & (BPVVDCHI2>25)",

        'LambdaLLComCuts': "(ADAMASS('Lambda0')<50*MeV) & (ADOCACHI2CUT(30, ''))",
        'LambdaLLCuts'   : "(ADMASS('Lambda0') < 30.*MeV) & (BPVVDCHI2>25) & (VFASPF(VCHI2) < 25.)",

        'LambdastarComCuts' : "(AM < 5.6*GeV)",
        'LambdastarCuts': "(VFASPF(VCHI2) < 25.)",
        
        'BComCuts'      : "(in_range(3.7*GeV, AM, 6.8*GeV))",
        'BMomCuts'      : "(in_range(4.0*GeV,  M, 6.5*GeV)) & (VFASPF(VCHI2/VDOF) < 25.) & (BPVDIRA> 0.999) & (BPVDLS>0) & (BPVIPCHI2()<400)",

        'LbComCuts'     : "(in_range(3.7*GeV, AM, 7.1*GeV))",
        'LbMomCuts'     : "(in_range(4.0*GeV,  M, 6.8*GeV)) & (VFASPF(VCHI2/VDOF) < 25.) & (BPVDIRA> 0.999) & (BPVDLS>0) & (BPVIPCHI2()<400)",

        'Bu2eeKMVACut'       :  "-0.05",
        'Bu2mumuKMVACut'     :  "-0.05",
        'Bu2LLKXmlFile'      :  '$TMVAWEIGHTSROOT/data/Bu2eeK_BDT_v1r0.xml',

        'Bd2eeKstarMVACut'   :  "-0.05",
        'Bd2mumuKstarMVACut' :  "-0.05",
        'Bd2LLKstarXmlFile'  :  '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDT_v1r0.xml',

        'Bd2eeKsMVACut'      :  "-0.07",
        'Bd2mumuKsMVACut'    :  "-0.07",
        'Bd2LLKsXmlFile'     :  '$TMVAWEIGHTSROOT/data/Bd2eeKs_BDT_v1r0.xml',

        'Bs2eePhiMVACut'     :  "-0.06",
        'Bs2mumuPhiMVACut'   :  "-0.08",
        'Bs2LLPhiXmlFile'    :  '$TMVAWEIGHTSROOT/data/Bs2eePhi_BDT_v1r0.xml',

        'Lb2eeLambdaMVACut'  :  "-0.11",
        'Lb2mumuLambdaMVACut':  "-0.15",
        'Lb2LLLambdaXmlFile' :  '$TMVAWEIGHTSROOT/data/Lb2eeLambda_BDT_v1r0.xml',

        'Lb2eePKMVACut'      :  "-0.09",
        'Lb2mumuPKMVACut'    :  "-0.11",
        'Lb2LLPKXmlFile'     :  '$TMVAWEIGHTSROOT/data/Lb2eePK_BDT_v1r0.xml',

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
        
        'Pion4LPCuts',
        
        'KstarCuts',
        'KsDDCuts',
        'KsLLComCuts',
        'KsLLCuts',
        
        'PhiCuts',

        'LambdaDDCuts',
        'LambdaLLComCuts',
        'LambdaLLCuts',

        'LambdastarComCuts',
        'LambdastarCuts',
        
        'BComCuts',
        'BMomCuts',

        'LbComCuts',
        'LbMomCuts',

        'Bu2eeKMVACut',
        'Bu2mumuKMVACut',
        'Bu2LLKXmlFile',
        
        'Bd2eeKstarMVACut',
        'Bd2mumuKstarMVACut',
        'Bd2LLKstarXmlFile',

        'Bd2eeKsMVACut',
        'Bd2mumuKsMVACut',
        'Bd2LLKsXmlFile',

        'Bs2eePhiMVACut',
        'Bs2mumuPhiMVACut',
        'Bs2LLPhiXmlFile',

        'Lb2eeLambdaMVACut',
        'Lb2mumuLambdaMVACut',
        'Lb2LLLambdaXmlFile',

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

        from StandardParticles import StdAllLooseANNPions
        self.SelPions4LP = self.createSubSel( OutputList = self.name + "SelPions4LP",
                                              InputList =  StdAllLooseANNPions , 
                                              Cuts = config['Pion4LPCuts']
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
        
        """ 
        self.SelKsLL = self.createSubSel( OutputList = self.name + "SelKsLL",
                                          InputList = StdLooseKsLL,
                                          Cuts = config['KsLLCuts'] )
        """
        
        self.SelKsLL = self.createCombinationSel( OutputList = self.name + "SelKsLL",
                                                  DecayDescriptor = "KS0 -> pi+ pi-",
                                                  DaughterLists = [ self.SelPions4LP ],                    
                                                  PreVertexCuts  = config['KsLLComCuts'],
                                                  PostVertexCuts = config['KsLLCuts'] )
                
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

        self.SelLambdaLL = self.createCombinationSel( OutputList = self.name + "SelLambdaLL",
                                                      DecayDescriptor = "[Lambda0 -> p+ pi-]cc",
                                                      DaughterLists = [ self.SelProtons, self.SelPions4LP ],   
                                                      PreVertexCuts  = config['LambdaLLComCuts'],
                                                      PostVertexCuts = config['LambdaLLCuts'] )
        
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
        self.B2LLHVars = {
            "sqrt(L1_IPCHI2_OWNPV)"      : "sqrt(CHILD(MIPCHI2DV(), 1, 1))",
            "sqrt(L2_IPCHI2_OWNPV)"      : "sqrt(CHILD(MIPCHI2DV(), 1, 2))",
            "sqrt(Jpsi_IPCHI2_OWNPV}"    : "sqrt(CHILD(MIPCHI2DV(), 1 ))",
            "sqrt(H_IPCHI2_OWNPV}"       : "sqrt(CHILD(MIPCHI2DV(), 2 ))",
            "sqrt(B_IPCHI2_OWNPV)"       : "sqrt(BPVIPCHI2())" ,
            "log(L1_PT)"                 : "log(CHILD(PT, 1, 1))",
            "log(L2_PT)"                 : "log(CHILD(PT, 1, 2))", 
            "log(Jpsi_PT)"               : "log(CHILD(PT, 1))",
            "log(H_PT)"                  : "log(CHILD(PT, 2))",
            "log(B_PT)"                  : "log(PT)",
            "sqrt(Jpsi_FDCHI2_OWNPV)"    : "sqrt(CHILD(BPVVDCHI2,1))",
            "sqrt(B_FDCHI2_OWNPV)"       : "sqrt(BPVVDCHI2)" ,
            "B_DIRA_OWNPV"               : "BPVDIRA"
            }

        
        """
        Bu2eeK
        """
        self.SelBu2eeK = self.createCombinationSel( OutputList = self.name + "SelBu2eeK",
                                                    DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                                    DaughterLists = [ self.SelDiElectron, self.SelKaons ],
                                                    PreVertexCuts  = config['BComCuts'],
                                                    PostVertexCuts = config['BMomCuts'] )

        self.MvaBu2eeK = self.applyMVA( self.name + "MvaBu2eeK",
                                        SelB        = self.SelBu2eeK,
                                        MVAVars     = self.B2LLHVars,
                                        MVACutValue = config['Bu2eeKMVACut'], 
                                        MVAxmlFile  = config['Bu2LLKXmlFile']
                                        )

        self.Bu2eeKLine = StrippingLine( self.name + '_Bu2eeKLine',                                                
                                         RelatedInfoTools = config['RelatedInfoTools'],                                        
                                         algos     = [ self.MvaBu2eeK ],
                                         MDSTFlag  = True
                                         )

        self.registerLine( self.Bu2eeKLine )


        """
        Bu2mumuK
        """
        self.SelBu2mumuK = self.createCombinationSel( OutputList = self.name + "SelBu2mumuK",
                                                      DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                                      DaughterLists = [ self.SelDiMuon, self.SelKaons ],                    
                                                      PreVertexCuts  = config['BComCuts'],
                                                      PostVertexCuts = config['BMomCuts'] )

        self.MvaBu2mumuK = self.applyMVA( self.name + "MvaBu2mumuK",
                                          SelB        = self.SelBu2mumuK,
                                          MVAVars     = self.B2LLHVars,
                                          MVACutValue = config['Bu2mumuKMVACut'], 
                                          MVAxmlFile  = config['Bu2LLKXmlFile']
                                          )
                
        self.Bu2mumuKLine = StrippingLine( self.name + '_Bu2mumuKLine',                                                
                                           RelatedInfoTools = config['RelatedInfoTools'],
                                           algos     = [ self.MvaBu2mumuK ],
                                           MDSTFlag  = True
                                           )

        self.registerLine( self.Bu2mumuKLine )
        

        #
        # Bd2LLKstar
        #
        """
        B2LLXVars
        """
        self.B2LLXVars = {
            "sqrt(L1_IPCHI2_OWNPV)"      : "sqrt(CHILD(MIPCHI2DV(), 1, 1))",
            "sqrt(L2_IPCHI2_OWNPV)"      : "sqrt(CHILD(MIPCHI2DV(), 1, 2))",
            "sqrt(Jpsi_IPCHI2_OWNPV}"    : "sqrt(CHILD(MIPCHI2DV(), 1 ))",
            "sqrt(H1_IPCHI2_OWNPV)"      : "sqrt(CHILD(MIPCHI2DV(), 2, 1))",
            "sqrt(H2_IPCHI2_OWNPV)"      : "sqrt(CHILD(MIPCHI2DV(), 2, 2))",
            "sqrt(X_IPCHI2_OWNPV}"       : "sqrt(CHILD(MIPCHI2DV(), 2 ))",
            "sqrt(B_IPCHI2_OWNPV)"       : "sqrt(BPVIPCHI2())" ,
            "log(L1_PT)"                 : "log(CHILD(PT, 1, 1))",
            "log(L2_PT)"                 : "log(CHILD(PT, 1, 2))", 
            "log(Jpsi_PT)"               : "log(CHILD(PT, 1))",
            "log(H1_PT)"                 : "log(CHILD(PT, 2, 1))",
            "log(H2_PT)"                 : "log(CHILD(PT, 2, 2))", 
            "log(X_PT)"                  : "log(CHILD(PT, 2))",
            "log(B_PT)"                  : "log(PT)",
            "sqrt(Jpsi_FDCHI2_OWNPV)"    : "sqrt(CHILD(BPVVDCHI2,1))",
            "sqrt(X_FDCHI2_OWNPV)"       : "sqrt(CHILD(BPVVDCHI2,2))",
            "sqrt(B_FDCHI2_OWNPV)"       : "sqrt(BPVVDCHI2)" ,
            "B_DIRA_OWNPV"               : "BPVDIRA"
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
                                            MVAVars     = self.B2LLXVars,
                                            MVACutValue = config['Bd2eeKstarMVACut'], 
                                            MVAxmlFile  = config['Bd2LLKstarXmlFile']
                                            )
        
        self.Bd2eeKstarLine = StrippingLine( self.name + '_Bd2eeKstarLine',                                                
                                             RelatedInfoTools = config['RelatedInfoTools'], 
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
                                              MVAVars     = self.B2LLXVars,
                                              MVACutValue = config['Bd2mumuKstarMVACut'], 
                                              MVAxmlFile  = config['Bd2LLKstarXmlFile']
                                              )
                
        self.Bd2mumuKstarLine = StrippingLine( self.name + '_Bd2mumuKstarLine',                                                
                                               RelatedInfoTools = config['RelatedInfoTools'],
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

        self.MvaBd2eeKs = self.applyMVA( self.name + "MvaBd2eeKs",
                                         SelB        = self.SelBd2eeKs,
                                         MVAVars     = self.B2LLXVars,
                                         MVACutValue = config['Bd2eeKsMVACut'], 
                                         MVAxmlFile  = config['Bd2LLKsXmlFile']
                                         )
        
        self.Bd2eeKsLine = StrippingLine( self.name + '_Bd2eeKsLine',                                                
                                          RelatedInfoTools = config['RelatedInfoTools'],                                        
                                          algos     = [ self.MvaBd2eeKs ],
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

        self.MvaBd2mumuKs = self.applyMVA( self.name + "MvaBd2mumuKs",
                                           SelB        = self.SelBd2mumuKs,
                                           MVAVars     = self.B2LLXVars,
                                           MVACutValue = config['Bd2mumuKsMVACut'], 
                                           MVAxmlFile  = config['Bd2LLKsXmlFile']
                                           )
        
        self.Bd2mumuKsLine = StrippingLine( self.name + '_Bd2mumuKsLine',                                                
                                            RelatedInfoTools = config['RelatedInfoTools'],
                                            algos     = [ self.MvaBd2mumuKs ],
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

        self.MvaBs2eePhi = self.applyMVA( self.name + "MvaBs2eePhi",
                                          SelB        = self.SelBs2eePhi,
                                          MVAVars     = self.B2LLXVars,
                                          MVACutValue = config['Bs2eePhiMVACut'], 
                                          MVAxmlFile  = config['Bs2LLPhiXmlFile']
                                          )
        
        self.Bs2eePhiLine = StrippingLine( self.name + '_Bs2eePhiLine',                                                
                                           RelatedInfoTools = config['RelatedInfoTools'],
                                           algos     = [ self.MvaBs2eePhi ],
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

        self.MvaBs2mumuPhi = self.applyMVA( self.name + "MvaBs2mumuPhi",
                                            SelB        = self.SelBs2mumuPhi,
                                            MVAVars     = self.B2LLXVars,
                                            MVACutValue = config['Bs2mumuPhiMVACut'], 
                                            MVAxmlFile  = config['Bs2LLPhiXmlFile']
                                            )
        
        self.Bs2mumuPhiLine = StrippingLine( self.name + '_Bs2mumuPhiLine',                                                
                                             RelatedInfoTools = config['RelatedInfoTools'],
                                             algos     = [ self.MvaBs2mumuPhi ],
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

        self.MvaLb2eeLambda = self.applyMVA( self.name + "MvaLb2eeLambda",
                                             SelB        = self.SelLb2eeLambda,
                                             MVAVars     = self.B2LLXVars,
                                             MVACutValue = config['Lb2eeLambdaMVACut'], 
                                             MVAxmlFile  = config['Lb2LLLambdaXmlFile']
                                             )
        
        self.Lb2eeLambdaLine = StrippingLine( self.name + '_Lb2eeLambdaLine',                                                
                                              RelatedInfoTools = config['RelatedInfoTools'],                                        
                                              algos     = [ self.MvaLb2eeLambda ],
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

        self.MvaLb2mumuLambda = self.applyMVA( self.name + "MvaLb2mumuLambda",
                                               SelB        = self.SelLb2mumuLambda,
                                               MVAVars     = self.B2LLXVars,
                                               MVACutValue = config['Lb2mumuLambdaMVACut'], 
                                               MVAxmlFile  = config['Lb2LLLambdaXmlFile']
                                               )
        
        self.Lb2mumuLambdaLine = StrippingLine( self.name + '_Lb2mumuLambdaLine',                                                
                                                RelatedInfoTools = config['RelatedInfoTools'],
                                                algos     = [ self.MvaLb2mumuLambda ],
                                                MDSTFlag  = True
                                                )
        
        self.registerLine( self.Lb2mumuLambdaLine )

        
        #
        # Lb-> LL Kstar
        #        
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
                                         MVAVars     = self.B2LLXVars,
                                         MVACutValue = config['Lb2eePKMVACut'], 
                                         MVAxmlFile  = config['Lb2LLPKXmlFile']
                                         )
        
        self.Lb2eePKLine = StrippingLine( self.name + '_Lb2eePKLine',
                                          RelatedInfoTools = config['RelatedInfoTools'],                                        
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
                                           MVAVars     = self.B2LLXVars,
                                           MVACutValue = config['Lb2mumuPKMVACut'], 
                                           MVAxmlFile  = config['Lb2LLPKXmlFile']
                                           )
        
        self.Lb2mumuPKLine = StrippingLine( self.name + '_Lb2mumuPKLine',       
                                            RelatedInfoTools = config['RelatedInfoTools'], 
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
