'''
Module for selecting Bs->eta_c phi
'''

__author__=['Morgan Martin, Jibo He']
__date__ = '20/08/2014'
__version__= '$Revision: 1.0 $'


__all__ = (
    'Bs2EtacPhiBDTConf',
    'default_config'
    )

default_config = {
    'NAME'              :  'Bs2EtacPhiBDT',
    'BUILDERTYPE'       :  'Bs2EtacPhiBDTConf',
    'CONFIG'    : {
        'HLTCuts'       : "(HLT_PASS_RE('Hlt2Topo.*Decision') | HLT_PASS_RE('Hlt2IncPhi.*Decision'))",
        'KaonCuts'      : "(PROBNNk > 0.13) & (PT > 300*MeV) & (TRGHOSTPROB<0.4)",
        'PionCuts'      : "(PROBNNpi > 0.2) & (PT > 250*MeV) & (TRGHOSTPROB<0.4)",
        'EtacComAMCuts' : "(AM<3.25*GeV)",
        'EtacComN4Cuts' : """
                          (in_range(2.75*GeV, AM, 3.25*GeV))
                          & ( (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) ) > 2.5 *GeV)
                          & ( (ACHILD(MIPCHI2DV(), 1) + ACHILD(MIPCHI2DV(), 2) + ACHILD(MIPCHI2DV(), 3) + ACHILD(MIPCHI2DV(), 4))>30)
                          """,
        'EtacMomN4Cuts' : "(VFASPF(VCHI2/VDOF) < 9.) & (in_range(2.8*GeV, MM, 3.2*GeV)) & (MIPCHI2DV(PRIMARY) > 2.)" ,
        'EtacComCuts'   : "(in_range(2.75*GeV, AM, 3.25*GeV))",
        'PhiCuts'       : """
                          (MAXTREE(ABSID=='K+',TRGHOSTPROB) < 0.4)
                          & (MM<1.05*GeV)
                          & (PT> 800*MeV)
                          & (MIPCHI2DV(PRIMARY) > 2.)
                          & (VFASPF(VCHI2) < 9.)
                          """ ,
        'KsCuts'        : "(ADMASS('KS0') < 30.*MeV) & (BPVDLS>5)",
        'BsComCuts'     : "(ADAMASS('B_s0') < 500 *MeV)",
        'BsMomCuts'     : "(VFASPF(VCHI2/VDOF) < 25.) & (BPVDIRA> 0.99) & (BPVIPCHI2()<25) & (BPVDLS>0)",

        'Bs2EtacPhiMVACut'   :  "0.",
        'Bs2EtacPhiXmlFile'  :  "$TMVAWEIGHTSROOT/data/Bs2EtacPhi_BDT_v1r0.xml", 
        
        'Prescale'      : 1.
        },
    'STREAMS'           : ['Bhadron' ],
    'WGs'               : ['B2CC'],
    }


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles 
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays

class Bs2EtacPhiBDTConf(LineBuilder):
    
    __configuration_keys__ = (
        'HLTCuts',
        'KaonCuts',
        'PionCuts',
        'EtacComAMCuts',
        'EtacComN4Cuts',
        'EtacMomN4Cuts',
        'EtacComCuts',
        'PhiCuts',
        'KsCuts',
        'BsComCuts',
        'BsMomCuts',
        
        'Bs2EtacPhiMVACut',
        'Bs2EtacPhiXmlFile',
        
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
        Eta_c
        """
        from PhysSelPython.Wrappers import MergedSelection

        self.SelEtac = MergedSelection( self.name + "SelEtac",
                                        RequiredSelections =  [ self.SelEtac2KKPiPi, 
                                                                self.SelEtac2KKKK,
                                                                self.SelEtac2PiPiPiPi
                                                                ])
        
        
        """
        Phi 
        """
        self.SelPhi = self.createSubSel( OutputList = self.name + "SelPhi",
                                         InputList =  DataOnDemand(Location = 'Phys/StdTightPhi2KK/Particles' ), 
                                         Cuts = config['PhiCuts']
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
        Bs->Eta_c(KsKPi) Phi 
        """
        self.SelBs2EtacPhi_KsKPi = self.createCombinationSel( OutputList = self.name + "SelBs2EtacPhi_KsKPi",
                                                              DecayDescriptor = "B_s0 -> eta_c(1S) phi(1020)",
                                                              DaughterLists = [ self.SelPhi, self.SelEtac2KsKPi ],                    
                                                              PreVertexCuts  = config['BsComCuts'],
                                                              PostVertexCuts = config['BsMomCuts'] )

        self.Bs2EtacPhi_KsKPiLine = StrippingLine( self.name + '_KsKPiLine',                                                
                                                   prescale  = config['Prescale'],
                                                   HLT2      = config['HLTCuts'],
                                                   algos     = [ self.SelBs2EtacPhi_KsKPi ],
                                                   EnableFlavourTagging = True,
                                                   MDSTFlag = True
                                                   )

        self.registerLine( self.Bs2EtacPhi_KsKPiLine )
        


        """
        Bs->Eta_c Phi 
        """
        self.SelBs2EtacPhi = self.createCombinationSel( OutputList = self.name + "SelBs2EtacPhi",
                                                        DecayDescriptor = "B_s0 -> eta_c(1S) phi(1020)",
                                                        DaughterLists = [ self.SelPhi, self.SelEtac ],                    
                                                        PreVertexCuts  = config['BsComCuts'],
                                                        PostVertexCuts = config['BsMomCuts'] )

        """
        Apply MVA
        """
        self.Bs2EtacPhiVars = {
            'sqrt(Kp_Etac_IPCHI2_OWNPV)'     : "sqrt( CHILD(MIPCHI2DV(), 1, 1) )",
            'sqrt(Km_Etac_IPCHI2_OWNPV)'     : "sqrt( CHILD(MIPCHI2DV(), 1, 2) )",
            'sqrt(Pip_Etac_IPCHI2_OWNPV)'    : "sqrt( CHILD(MIPCHI2DV(), 1, 3) )",
            'sqrt(Pim_Etac_IPCHI2_OWNPV)'    : "sqrt( CHILD(MIPCHI2DV(), 1, 4) )",           
            'sqrt(Kp_Phi_IPCHI2_OWNPV)'      : "sqrt( CHILD(MIPCHI2DV(), 2, 1) )",
            'sqrt(Km_Phi_IPCHI2_OWNPV)'      : "sqrt( CHILD(MIPCHI2DV(), 2, 2) )",

            'EtacSumIPCHI2'                  : "sqrt( CHILD(MIPCHI2DV(), 1, 1)+CHILD(MIPCHI2DV(), 1, 2)+CHILD(MIPCHI2DV(), 1, 3)+CHILD(MIPCHI2DV(), 1, 4))",
            'sqrt(Etac_IPCHI2_OWNPV)'        : "sqrt( CHILD(MIPCHI2DV(), 1 ) )",
            'sqrt(Phi_IPCHI2_OWNPV)'         : "sqrt( CHILD(MIPCHI2DV(), 2 ) )",
            

            'log(Kp_Etac_PT)'      : "log( CHILD(PT, 1, 1) )",
            'log(Km_Etac_PT)'      : "log( CHILD(PT, 1, 2) )",
            'log(Pip_Etac_PT)'     : "log( CHILD(PT, 1, 3) )",
            'log(Pim_Etac_PT)'     : "log( CHILD(PT, 1, 4) )",
            'log(Kp_Phi_PT)'       : "log( CHILD(PT, 2, 1) )",
            'log(Km_Phi_PT)'       : "log( CHILD(PT, 2, 2) )",            
            'log(Etac_PT)'         : "log( CHILD(PT, 1 ) )",
            'log(Phi_PT)'          : "log( CHILD(PT, 2 ) )",
            
            'Etac_ENDVERTEX_CHI2'   :   "CHILD(VFASPF(VCHI2/VDOF), 1)",
            'Phi_ENDVERTEX_CHI2'    :   "CHILD(VFASPF(VCHI2/VDOF), 2)", 

            'sqrt(B_IPCHI2_OWNPV)'  :   "sqrt( BPVIPCHI2() )", 
            'B_DIRA_OWNPV'          :   "BPVDIRA",
            'B_LOKI_FDS'            :   "BPVDLS",
            'B_PVFit_chi2[0]'       :   "DTF_CHI2 ( True  )"
            }


        self.MvaBs2EtacPhi = self.applyMVA( self.name + "MvaBs2EtacPhi",
                                             SelB        = self.SelBs2EtacPhi,
                                             MVAVars     = self.Bs2EtacPhiVars,
                                             MVACutValue = config['Bs2EtacPhiMVACut'], 
                                             MVAxmlFile  = config['Bs2EtacPhiXmlFile']
                                             )
        
                
        self.Bs2EtacPhiBDTLine = StrippingLine( self.name + 'Line',                                                
                                                prescale  = config['Prescale'],
                                                HLT2      = config['HLTCuts'],
                                                algos     = [ self.MvaBs2EtacPhi ],
                                                EnableFlavourTagging = True,
                                                MDSTFlag = True
                                                )
        self.registerLine( self.Bs2EtacPhiBDTLine )
        

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
