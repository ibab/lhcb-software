'''
Module for selecting Lb->eta_c K p
'''

__author__=['Liming Zhang']
__date__ = '13/10/2015'
__version__= '$Revision: 1.0 $'


__all__ = (
    'Lb2EtacKpConf',
    'default_config'
    )

default_config = {
    'NAME'              :  'Lb2EtacKp',
    'BUILDERTYPE'       :  'Lb2EtacKpConf',
    'CONFIG'    : {
        'KaonCuts'      : "(PROBNNk > 0.1) & (PT > 300*MeV) & (TRGHOSTPROB<0.4)",
        'ProtonCuts'    : "(PROBNNp > 0.1) & (PT > 300*MeV) & (P > 10*GeV) & (TRGHOSTPROB<0.4)",        
        'PionCuts'      : "(PROBNNpi > 0.1) & (PT > 250*MeV) & (TRGHOSTPROB<0.4)",
        'EtacComAMCuts' : "(AM<3.25*GeV)",
        'EtacComN4Cuts' : """
                          (in_range(2.75*GeV, AM, 3.25*GeV))
                          & ( (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) ) > 2.5 *GeV)
                          & ( (ACHILD(MIPCHI2DV(), 1) + ACHILD(MIPCHI2DV(), 2) + ACHILD(MIPCHI2DV(), 3) + ACHILD(MIPCHI2DV(), 4))>30)
                          """,
        'EtacMomN4Cuts' : """
                           (VFASPF(VCHI2/VDOF) < 9.) 
                           & (in_range(2.8*GeV, MM, 3.2*GeV)) 
                           & (MIPCHI2DV(PRIMARY) > 4.) 
                           & (BPVVDCHI2>10) 
                           & (BPVDIRA>0.9)
                           """,
        'EtacComCuts'   : "(in_range(2.75*GeV, AM, 3.25*GeV))",
        'LambdaSComCuts': "(ACHILD(PT,1)+ACHILD(PT,2) > 900.*MeV) & (AM < 4.0 *GeV) & (ADOCACHI2CUT(20., ''))",
        'LambdaSMomCuts': """
                          (MIPCHI2DV(PRIMARY) > 4.)
                          & (BPVVDCHI2 > 10.)
                          & (VFASPF(VCHI2) < 9.)
                          & (BPVDIRA>0.9)
                          """,
        'KsCuts'        : "(ADMASS('KS0') < 30.*MeV) & (BPVDLS>5)",
        'LbComCuts'     : "(ADAMASS('Lambda_b0') < 500 *MeV)",
        'LbMomCuts'     : """
                          (VFASPF(VCHI2/VDOF) < 10.) 
                          & (BPVDIRA> 0.9999) 
                          & (BPVIPCHI2()<25) 
                          & (BPVVDCHI2>250)
                          & (BPVVDRHO>0.1*mm) 
                          & (BPVVDZ>2.0*mm)
                          """,
        'Prescale'      : 1.
        },
    'STREAMS'           : ['Bhadron' ],
    'WGs'               : ['BandQ'],
    }


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles 
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Configurables import DaVinci__N4BodyDecays

class Lb2EtacKpConf(LineBuilder):
    
    __configuration_keys__ = (
        'KaonCuts',
        'ProtonCuts',
        'PionCuts',
        'EtacComAMCuts',
        'EtacComN4Cuts',
        'EtacMomN4Cuts',
        'EtacComCuts',
        'LambdaSComCuts',
        'LambdaSMomCuts',
        'KsCuts',
        'LbComCuts',
        'LbMomCuts',
        'Prescale'
        )

    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config


        self.SelKaons = self.createSubSel( OutputList = self.name + "SelKaons",
                                           InputList =  DataOnDemand(Location = 'Phys/StdLooseKaons/Particles' ), 
                                           Cuts = config['KaonCuts']
                                           )

        self.SelPions = self.createSubSel( OutputList = self.name + "SelPions",
                                           InputList =  DataOnDemand(Location = 'Phys/StdLoosePions/Particles' ), 
                                           Cuts = config['PionCuts']
                                           )

        self.SelProtons = self.createSubSel( OutputList = self.name + "SelProtons",
                                           InputList =  DataOnDemand(Location = 'Phys/StdLooseProtons/Particles' ), 
                                           Cuts = config['PionCuts']
                                           )

        """
        Etac-> K K Pi Pi
        """
        self.SelEtac2KKPiPi = self.createN4BodySel( OutputList = self.name + "SelEtac2KKPiPi",
                                                    DaughterLists = [ self.SelKaons, self.SelPions ],
                                                    DecayDescriptor = "eta_c(1S) -> K+ K- pi+ pi-",
                                                    ComAMCuts      = config['EtacComAMCuts'],
                                                    PreVertexCuts  = config['EtacComN4Cuts'], 
                                                    PostVertexCuts = config['EtacMomN4Cuts']
                                                    )

        """
        Etac-> K K K K
        """
        self.SelEtac2KKKK = self.createN4BodySel( OutputList = self.name + "SelEtac2KKKK",
                                                  DaughterLists = [ self.SelKaons ],
                                                  DecayDescriptor = "eta_c(1S) -> K+ K- K+ K-",
                                                  ComAMCuts      = config['EtacComAMCuts'],
                                                  PreVertexCuts  = config['EtacComN4Cuts'], 
                                                  PostVertexCuts = config['EtacMomN4Cuts']
                                                  )

        
        """
        Etac-> Pi Pi Pi Pi
        """
        self.SelEtac2PiPiPiPi = self.createN4BodySel( OutputList = self.name + "SelEtac2PiPiPiPi",
                                                      DaughterLists = [ self.SelPions ],
                                                      DecayDescriptor = "eta_c(1S) -> pi+ pi- pi+ pi-",
                                                      ComAMCuts      = config['EtacComAMCuts'],
                                                      PreVertexCuts  = config['EtacComN4Cuts'], 
                                                      PostVertexCuts = config['EtacMomN4Cuts']
                                                      )

        """
        Etac-> p pbar Pi Pi
        """
        self.SelEtac2PPbarPiPi = self.createN4BodySel( OutputList = self.name + "SelEtac2PPbarPiPi",
                                                      DaughterLists = [ self.SelPions, self.SelProtons ],
                                                      DecayDescriptor = "eta_c(1S) -> p+ p~- pi+ pi-",
                                                      ComAMCuts      = config['EtacComAMCuts'],
                                                      PreVertexCuts  = config['EtacComN4Cuts'], 
                                                      PostVertexCuts = config['EtacMomN4Cuts']
                                                      )

        """
        Etac-> p pbar
        """
        self.SelEtac2PPbar = self.createCombinationSel( OutputList = self.name + "SelEtac2PPbar",
                                                        DecayDescriptor = "eta_c(1S) -> p+ p~-",
                                                        DaughterLists = [ self.SelProtons ],                                                        
                                                        PreVertexCuts  = config['EtacComCuts'], 
                                                        PostVertexCuts = config['EtacMomN4Cuts']
                                                        )
        """
        Eta_c
        """
        from PhysSelPython.Wrappers import MergedSelection

        self.SelEtac = MergedSelection( self.name + "SelEtac",
                                        RequiredSelections =  [ self.SelEtac2KKPiPi, 
                                                                self.SelEtac2KKKK,
                                                                self.SelEtac2PiPiPiPi,
                                                                self.SelEtac2PPbarPiPi,
                                                                ])
        
        """
        Lambda*
        """
        self.SelLambdaS = self.createCombinationSel( OutputList = self.name + "SelLambdaS",
                                         DecayDescriptor = "[Lambda(1520)0 -> p+ K-]cc", 
                                         DaughterLists = [self.SelProtons, self.SelKaons],
                                         PreVertexCuts  = config['LambdaSComCuts'],     
                                         PostVertexCuts = config['LambdaSMomCuts']
                                         )
        
        """
        Kshort 
        """
        # Both LL and DD 
        self.InputKs = MergedSelection( self.name + "InputKs",
                                        RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKsDD/Particles"),
                                                              DataOnDemand(Location = "Phys/StdLooseKsLL/Particles")] )

        self.SelKs = self.createSubSel( OutputList = self.name + "SelKs",
                                        InputList = self.InputKs,
                                        Cuts = config['KsCuts'] ) 

        # Eta_c -> KS0 K Pi 
        self.SelEtac2KsKPi = self.createCombinationSel( OutputList = self.name + "SelEtac2KsKPi",
                                                        DecayDescriptor = "[eta_c(1S) -> KS0 K+ pi-]cc",
                                                        DaughterLists = [ self.SelKs, self.SelKaons, self.SelPions ],                                                        
                                                        PreVertexCuts  = config['EtacComCuts'], 
                                                        PostVertexCuts = config['EtacMomN4Cuts']
                                                        )

        """
        Lb->Eta_c(KsKPi) Lambda* 
        """
        self.SelLb2EtacKp_KsKPi = self.createCombinationSel( OutputList = self.name + "SelLb2EtacKp_KsKPi",
                                                              DecayDescriptor = "[Lambda_b0 -> eta_c(1S) Lambda(1520)0]cc",
                                                              DaughterLists = [ self.SelLambdaS, self.SelEtac2KsKPi ],                    
                                                              PreVertexCuts  = config['LbComCuts'],
                                                              PostVertexCuts = config['LbMomCuts'] )

        self.Lb2EtacKp_KsKPiLine = StrippingLine( self.name + '_KsKPiLine',                                                
                                                   prescale  = config['Prescale'],
                                                 #  HLT       = config['HLTCuts'],
                                                   algos     = [ self.SelLb2EtacKp_KsKPi ],
                                                   EnableFlavourTagging = False,
                                                   MDSTFlag = True
                                                   )

        self.registerLine( self.Lb2EtacKp_KsKPiLine )
        
        """
        Lb->Eta_c(p pbar) Lambda* 
        """
        self.SelLb2EtacKp_PPbar = self.createCombinationSel( OutputList = self.name + "SelLb2EtacKp_PPbar",
                                                              DecayDescriptor = "[Lambda_b0 -> eta_c(1S) Lambda(1520)0]cc",
                                                              DaughterLists = [ self.SelLambdaS, self.SelEtac2PPbar ],                    
                                                              PreVertexCuts  = config['LbComCuts'],
                                                              PostVertexCuts = config['LbMomCuts'] )

        self.Lb2EtacKp_PPbarLine = StrippingLine( self.name + '_PPbarLine',                                                
                                                   prescale  = config['Prescale'],
                                                  # HLT       = config['HLTCuts'],
                                                   algos     = [ self.SelLb2EtacKp_PPbar ],
                                                   EnableFlavourTagging = False,
                                                   MDSTFlag = True
                                                   )

        self.registerLine( self.Lb2EtacKp_PPbarLine )
        


        """
        Lb->Eta_c Lambda* 
        """
        self.SelLb2EtacKp = self.createCombinationSel( OutputList = self.name + "SelLb2EtacKp",
                                                        DecayDescriptor = "[Lambda_b0 -> eta_c(1S) Lambda(1520)0]cc",
                                                        DaughterLists = [ self.SelLambdaS, self.SelEtac ],                    
                                                        PreVertexCuts  = config['LbComCuts'],
                                                        PostVertexCuts = config['LbMomCuts'] )

        self.Lb2EtacKp_4hLine            = StrippingLine( self.name + 'Lb2EtacKp_4hLine',                                                
                                                   prescale  = config['Prescale'],
                                                 #  HLT       = config['HLTCuts'],
                                                   algos     = [ self.SelLb2EtacKp ],
                                                   EnableFlavourTagging = False,
                                                   MDSTFlag = True
                                                   )

        self.registerLine( self.Lb2EtacKp_4hLine )
        


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


    def createN4BodySel( self, OutputList,
                         DecayDescriptor,
                         DaughterLists,
                         DaughterCuts = {} ,
                         ComAMCuts      = "AALL",
                         PreVertexCuts  = "AALL",
                         PostVertexCuts = "ALL" ) :
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = DaVinci__N4BodyDecays ( DecayDescriptor = DecayDescriptor,      
                                           DaughtersCuts = DaughterCuts,
                                           Combination12Cut  = ComAMCuts + "&" + "( ACHI2DOCA(1,2)<20 )",
                                           Combination123Cut = ComAMCuts + "&" + "( ACHI2DOCA(1,3)<20 ) & ( ACHI2DOCA(2,3)<20 )",
                                           CombinationCut = "( ACHI2DOCA(1,4)<20 ) & ( ACHI2DOCA(2,4)<20 ) & ( ACHI2DOCA(3,4)<20 )" + " & " + PreVertexCuts,
                                           MotherCut = PostVertexCuts,
                                           ReFitPVs = False )
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
