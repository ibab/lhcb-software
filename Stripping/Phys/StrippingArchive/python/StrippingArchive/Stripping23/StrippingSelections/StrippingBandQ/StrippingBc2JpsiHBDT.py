# $Id: StrippingBc2JpsiH.py,v 1.2 2010-09-06 16:30:22 jhe Exp $

'''
Module for construction of Bc->Jpsi(MuMu)H BDT
'''

__author__=['Jibo He']
__date__ = '02/11/2013'
__version__= '$Revision: 2.0 $'


__all__ = (
    'Bc2JpsiHBDTConf',
    'makeJpsi2MuMu',
    'makeBc2JpsiH',
    'default_config'
    )

default_config =  {
    'NAME'        : 'Bc2JpsiHBDT',
    'WGs'         : ['BandQ'],
    'BUILDERTYPE' : 'Bc2JpsiHBDTConf',
    'CONFIG'      : {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.   ,    
    'MuonCuts'            : "(MINTREE('mu+'==ABSID,PT)>500*MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF)<3) & (MINTREE('mu+'==ABSID,PIDmu)>0.)",
    'JpsiCuts'            : '((MM>3.0*GeV) & (MM<3.2*GeV) & (VFASPF(VCHI2PDOF)<16))',
    'PionCuts'            : '((TRCHI2DOF<3) & (TRGHOSTPROB<0.6) & (PT>1.0*GeV))',
    'BcComCuts'           : '(in_range(5.8*GeV, AM, 7.0*GeV))',
    'BcMomCuts'           : """(VFASPF(VCHI2/VDOF)<16) 
                             & (in_range(6.0*GeV, DTF_FUN(M,True,strings( ['J/psi(1S)'])), 6.75*GeV))
                             & (BPVIPCHI2()<25) 
                            """,
    'BDTCutValue'         :  0.6 ,
    'BDTWeightsFile'      : '$TMVAWEIGHTSROOT/data/Bc2JpsiH_BDTG_v1r0.xml'    
    },
    'STREAMS'   : [ 'Dimuon' ]
    }


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class Bc2JpsiHBDTConf(LineBuilder):
    
    __configuration_keys__ = (
        'LinePrescale',
        'LinePostscale',

        'MuonCuts',
        
        'JpsiCuts',
        
        'PionCuts',        

        'BcComCuts',
        'BcMomCuts',
        
        'BDTCutValue',
        'BDTWeightsFile'
        )
    
    def __init__(self, name, config ):

        LineBuilder.__init__(self, name, config)
        
        Bc2JpsiHName = name
        Bc2JpsiHLineName = Bc2JpsiHName + "Line"
        
        self.SelJpsi2MuMu = makeJpsi2MuMu( 'Jpsi2MuMuFor'+Bc2JpsiHName,
                                           MuonCuts = config['MuonCuts'],
                                           JpsiCuts = config['JpsiCuts']
                                           )
        
        self.SelBc2JpsiH = makeBc2JpsiH( "Sel_"+Bc2JpsiHName,
                                         SelJpsi2MuMu = self.SelJpsi2MuMu,
                                         PionCuts = config['PionCuts'],
                                         BcComCuts = config['BcComCuts'],
                                         BcMomCuts = config['BcMomCuts']
                                         )
        
        self.CutBDTBc2JpsiH  = applyBDT( "CutBDT_" + Bc2JpsiHName,
                                         LineName  = Bc2JpsiHLineName,
                                         SelBc2JpsiH = self.SelBc2JpsiH,
                                         BDTCutValue = config['BDTCutValue'],
                                         BDTWeightsFile = config['BDTWeightsFile']
                                         )
        
        self.line = StrippingLine( Bc2JpsiHLineName,
                                   prescale = config['LinePrescale'],
                                   postscale = config['LinePostscale'],
                                   selection = self.CutBDTBc2JpsiH,
                                   RequiredRawEvents = [ "Trigger", "Muon", "Calo", "Rich", "Velo","Tracker" ]
                                   )
                                             
        self.registerLine(self.line)
        
        
       

def makeJpsi2MuMu( name,
                   MuonCuts,
                   JpsiCuts
                   ):

    _StdLooseDiMuon = DataOnDemand(Location = "Phys/StdLooseDiMuon/Particles")
    
    MuonCut = MuonCuts     
    MuMuCut = JpsiCuts
    
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut )
    
    return Selection( name + "_SelP2MuMu",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuon ]
                      )


def makeBc2JpsiH( name,
                  SelJpsi2MuMu,
                  PionCuts,                  
                  BcComCuts,
                  BcMomCuts
                  ):

    #---------------------------
    # Pion or Kaon
    #---------------------------        
    from StandardParticles import StdAllNoPIDsPions as NoPIDsPionsForBc2JpsiH

    # Pion Cut
    PionCut = PionCuts 

    _PionFilter = FilterDesktop( Code = PionCut )
    
    SelPion = Selection("SelPion_"+name,
                        Algorithm = _PionFilter,
                        RequiredSelections = [ NoPIDsPionsForBc2JpsiH ])

    
    #---------------------------
    # Bc -> J/psi(MuMu) H
    #---------------------------            
    _Bc2JpsiH = CombineParticles( DecayDescriptor = "[ B_c+ -> J/psi(1S) pi+ ]cc",
                                  CombinationCut = BcComCuts,
                                  MotherCut      = BcMomCuts,
                                  ReFitPVs = False )
    
    return Selection( name,
                      Algorithm = _Bc2JpsiH,
                      RequiredSelections = [ SelJpsi2MuMu, SelPion  ]
                      )


def applyBDT( name,
              LineName,
              SelBc2JpsiH,
              BDTCutValue,
              BDTWeightsFile ):
    
    _FilterBc2JpsiH = FilterDesktop( Code = "FILTER('Bc2JpsiHBDTSelection/Bc2JpsiHBDT')" )
    
    BDTSel = Selection( name,
                        Algorithm = _FilterBc2JpsiH,
                        RequiredSelections = [ SelBc2JpsiH ]
                        )

    """
    Name is special here, since this is the last algorithm,
    whose name seems to be the one of the stripping line....
    """
    from Configurables import Bc2JpsiHBDTSelection

    MyBDT = Bc2JpsiHBDTSelection( LineName + '.Bc2JpsiHBDT' )
    MyBDT.BDTCut = BDTCutValue
    MyBDT.WeightsFile = BDTWeightsFile
    
    return BDTSel 
