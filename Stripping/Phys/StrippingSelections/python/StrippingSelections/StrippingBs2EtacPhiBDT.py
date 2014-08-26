'''
Module for selecting Bs->eta_c phi
'''

__author__=['Morgan Martin, Jibo He']
__date__ = '20/08/2014'
__version__= '$Revision: 1.0 $'


__all__ = (
    'Bs2EtacPhiBDTConf'
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
        'PhiCuts'       : """
                          (MAXTREE(ABSID=='K+',TRGHOSTPROB) < 0.4)
                          & (MM<1.05*GeV)
                          & (PT> 800*MeV)
                          & (MIPCHI2DV(PRIMARY) > 2.)
                          & (VFASPF(VCHI2) < 9.)
                          """ ,
        'BsComCuts'     : "(ADAMASS('B_s0') < 500 *MeV)",
        'BsMomCuts'     : "(VFASPF(VCHI2/VDOF) < 25.) & (BPVDIRA> 0.99) & (BPVIPCHI2()<25)",
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
from Configurables import DaVinci__N4BodyDecays

class Bs2EtacPhiBDTConf(LineBuilder):
    
    __configuration_keys__ = (
        'HLTCuts',
        'KaonCuts',
        'PionCuts',
        'EtacComAMCuts',
        'EtacComN4Cuts',
        'EtacMomN4Cuts',
        'PhiCuts',
        'BsComCuts',
        'BsMomCuts',
        'Prescale'
        )

    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config


        """
        Eta_c
        """
        self.SelKaons = self.createSubSel( OutputList = self.name + "SelKaons",
                                           InputList =  DataOnDemand(Location = 'Phys/StdLooseKaons/Particles' ), 
                                           Cuts = config['KaonCuts']
                                           )

        self.SelPions = self.createSubSel( OutputList = self.name + "SelPions",
                                           InputList =  DataOnDemand(Location = 'Phys/StdLoosePions/Particles' ), 
                                           Cuts = config['PionCuts']
                                           )
        
        self.SelEtac2KKPiPi = self.createN4BodySel( OutputList = self.name + "SelEtac2KKPiPi",
                                                    DaughterLists = [ self.SelKaons, self.SelPions ],
                                                    DecayDescriptor = "eta_c(1S) -> K+ K- pi+ pi-",
                                                    ComAMCuts      = config['EtacComAMCuts'],
                                                    PreVertexCuts  = config['EtacComN4Cuts'], 
                                                    PostVertexCuts = config['EtacMomN4Cuts']
                                                    )

        """
        Phi 
        """
        self.SelPhi = self.createSubSel( OutputList = self.name + "SelPhi",
                                         InputList =  DataOnDemand(Location = 'Phys/StdTightPhi2KK/Particles' ), 
                                         Cuts = config['PhiCuts']
                                         )

        """
        Bs->Eta_c Phi 
        """
        self.SelBs2EtacPhi = self.createCombinationSel( OutputList = self.name + "SelBs2EtacPhi",
                                                        DecayDescriptor = "B_s0 -> eta_c(1S) phi(1020)",
                                                        DaughterLists = [ self.SelPhi, self.SelEtac2KKPiPi ],                    
                                                        PreVertexCuts  = config['BsComCuts'],
                                                        PostVertexCuts = config['BsMomCuts'] )
                
        self.Bs2EtacPhiBDTLine = StrippingLine( self.name + 'Line',                                                
                                                prescale  = config['Prescale'],
                                                HLT       = config['HLTCuts'],
                                                algos     = [ self.SelBs2EtacPhi ]
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
