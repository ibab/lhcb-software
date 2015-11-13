'''
Module for selecting charmonium -> p pbar pi pi 
'''

__author__=['Jibo He']
__date__ = '12/11/2015'
__version__= '$Revision: 1.0 $'


__all__ = (
    'Ccbar2PPPiPiConf',
    'default_config'
    )

default_config = {
    'NAME'              :  'Ccbar2PPPiPi',
    'BUILDERTYPE'       :  'Ccbar2PPPiPiConf',
    'CONFIG'    : {
        'HLTCuts'       : "(HLT_PASS_RE('Hlt2Topo.*Decision'))",
        'ProtonCuts'    : "(PROBNNp  > 0.1) & (PT > 300*MeV) & (TRGHOSTPROB<0.4) & (MIPCHI2DV(PRIMARY) > 9.)",
        'PionCuts'      : "(PROBNNpi > 0.2) & (PT > 250*MeV) & (TRGHOSTPROB<0.4) & (MIPCHI2DV(PRIMARY) > 9.)",
        'EtacComAMCuts' : "(AM<4.3*GeV)",
        'EtacComN4Cuts' : """
                          (AM > 2.7 *GeV)
                          & ( (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) ) > 2.5 *GeV)
                          & ( (ACHILD(MIPCHI2DV(), 1) + ACHILD(MIPCHI2DV(), 2) + ACHILD(MIPCHI2DV(), 3) + ACHILD(MIPCHI2DV(), 4))>30)
                          """,
        'EtacMomN4Cuts' : "(VFASPF(VCHI2/VDOF) < 9.) & (in_range(2.8*GeV, MM, 4.2*GeV)) & (BPVDLS>5)",        
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
from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays

class Ccbar2PPPiPiConf(LineBuilder):
    
    __configuration_keys__ = (
        'HLTCuts',
        'ProtonCuts',
        'PionCuts',
        'EtacComAMCuts',
        'EtacComN4Cuts',
        'EtacMomN4Cuts',        
        'Prescale'
        )

    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config


        self.SelProtons = self.createSubSel( OutputList = self.name + "SelProtons",
                                           InputList =  DataOnDemand(Location = 'Phys/StdLooseANNProtons/Particles' ), 
                                           Cuts = config['ProtonCuts']
                                           )

        self.SelPions = self.createSubSel( OutputList = self.name + "SelPions",
                                           InputList =  DataOnDemand(Location = 'Phys/StdLoosePions/Particles' ), 
                                           Cuts = config['PionCuts']
                                           )

        """
        J/psi -> p pbar Pi Pi
        """
        self.SelCcbar2PPPiPi = self.createN4BodySel( OutputList = self.name + "SelCcbar2PPPiPi",
                                                     DaughterLists = [ self.SelProtons, self.SelPions ],
                                                     DecayDescriptor = "J/psi(1S) -> p+ p~- pi+ pi-",
                                                     ComAMCuts      = config['EtacComAMCuts'],
                                                     PreVertexCuts  = config['EtacComN4Cuts'], 
                                                     PostVertexCuts = config['EtacMomN4Cuts']
                                                     )
        
        self.Ccbar2PPPiPiLine = StrippingLine( self.name + 'Line',                                                
                                               prescale  = config['Prescale'],
                                               HLT2      = config['HLTCuts'],
                                               algos     = [ self.SelCcbar2PPPiPi ],
                                               MDSTFlag  = True
                                               )
        self.registerLine( self.Ccbar2PPPiPiLine )
        
    def createSubSel( self, OutputList, InputList, Cuts ) :
        '''create a selection using a FilterDesktop'''
        filter = FilterDesktop(Code = Cuts)
        return Selection( OutputList,
                          Algorithm = filter,
                          RequiredSelections = [ InputList ] )
    
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
