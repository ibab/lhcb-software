# $Id: StrippingBc2JpsiH.py,v 1.2 2010-09-06 16:30:22 jhe Exp $

'''
Module for construction of Bc->Jpsi(MuMu)X
'''

__author__=['Jibo He']
__date__ = '02/09/2010'
__version__= '$Revision: 2.0 $'


__all__ = (
    'Bc2JpsiHConf',
    'makeJpsi2MuMu',
    'makeBc2JpsiH'
    )

config_default =  {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.   ,
    
    'MuonTRCHI2DOF'       :    5.   ,  # adimentional 
    'MuonPT'              : 1200.   ,  # MeV
    'MuonP'               :   -5.   ,  # MeV, not applied now
    'MuMuParticleName'    : "'J/psi(1S)'", # Particle Name, like "'psi(2S)'"
    'MuMuMassWindow'      :   80.   ,  # MeV, 10 sigma, may decreased to 150
    'MuMuVtxCHI2'         :    9.   ,  # adimentional
    'MuMuPT'              : 2000.   ,  # MeV
    
    'PionTRCHI2DOF'       :    5.   ,  # adimentional     
    'PionPT'              : 2000.   ,  # MeV
    'PionP'               :   -5.   ,  # MeV, not applied now
    'BcMassWindow'        :  400.   ,  # MeV, mass window
    'BcVtxCHI2'           :    9.   ,  # adimentional
    'BcPT'                : 6000.   ,  # MeV, May incrase up to 5000 MeV if needed

    'LifetimeCut'         :   ""    
    }


config_detached =  {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.   ,
    
    'MuonTRCHI2DOF'       :    5.   ,  # adimentional 
    'MuonPT'              :  500.   ,  # MeV
    'MuonP'               :   -5.   ,  # MeV, not applied now
    'MuMuParticleName'    : "'J/psi(1S)'", # Particle Name, like "'psi(2S)'"
    'MuMuMassWindow'      :  100.   ,  # MeV, 10 sigma, may decreased to 150
    'MuMuVtxCHI2'         :    9.   ,  # adimentional
    'MuMuPT'              :  -10.   ,  # MeV, not applied 
    
    'PionTRCHI2DOF'       :    5.   ,  # adimentional     
    'PionPT'              : 1000.   ,  # MeV
    'PionP'               :   -5.   ,  # MeV, not applied now
    'BcMassWindow'        :  400.   ,  # MeV, mass window
    'BcVtxCHI2'           :    9.   ,  # adimentional
    'BcPT'                :    0.   ,  # MeV, May incrase up to 5000 MeV if needed       

    'LifetimeCut'         : " & (BPVLTIME()>0.2*ps) & (INTREE( (ABSID=='pi+') & (BPVIPCHI2()>9)))"
    }


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class Bc2JpsiHConf(LineBuilder):
    
    __configuration_keys__ = (
        'LinePrescale',
        'LinePostscale',

        'MuonTRCHI2DOF',
        'MuonPT',
        'MuonP',
        'MuMuParticleName',
        'MuMuMassWindow',
        'MuMuVtxCHI2',
        'MuMuPT',
    
        'PionTRCHI2DOF',        
        'PionPT',
        'PionP',
        'BcMassWindow',  
        'BcVtxCHI2',
        'BcPT',
        
        'LifetimeCut'
         )
    
    def __init__(self, name, config ):

        LineBuilder.__init__(self, name, config)
        
        Bc2JpsiHName = name
        self.SelJpsi2MuMu = makeJpsi2MuMu( 'Jpsi2MuMuFor'+Bc2JpsiHName,
                                           MuonPT = config['MuonPT'],
                                           MuonP  = config['MuonP'],
                                           MuonTRCHI2DOF = config['MuonTRCHI2DOF'],
                                           MuMuParticleName = config['MuMuParticleName'],
                                           MuMuVtxCHI2 = config['MuMuVtxCHI2'],
                                           MuMuMassWindow = config['MuMuMassWindow'],
                                           MuMuPT = config['MuMuPT']
                                           )
        
        self.SelBc2JpsiH = makeBc2JpsiH( "Sel_"+Bc2JpsiHName,
                                         SelJpsi2MuMu = self.SelJpsi2MuMu,
                                         PionTRCHI2DOF = config['PionTRCHI2DOF'],
                                         PionPT = config['PionPT'],
                                         PionP = config['PionP'],
                                         BcVtxCHI2 = config['BcVtxCHI2'],
                                         BcMassWindow = config['BcMassWindow'],
                                         BcPT = config['BcPT'],
                                         LifetimeCut = config['LifetimeCut']
                                         )
                                             
        self.line = StrippingLine( Bc2JpsiHName+"Line",
                                   prescale = config['LinePrescale'],
                                   postscale = config['LinePostscale'],
                                   selection = self.SelBc2JpsiH 
                                   )

        self.registerLine(self.line)
        
        
       

def makeJpsi2MuMu( name,
                   MuonPT,
                   MuonP,
                   MuonTRCHI2DOF,
                   MuMuParticleName, 
                   MuMuVtxCHI2,
                   MuMuMassWindow,
                   MuMuPT
                   ):

    _StdLooseDiMuon = DataOnDemand(Location = "Phys/StdLooseDiMuon/Particles")
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    
    MuMuCut = "(ADMASS(%(MuMuParticleName)s) < %(MuMuMassWindow)s *MeV) & (VFASPF(VCHI2PDOF)< %(MuMuVtxCHI2)s) & (PT > %(MuMuPT)s *MeV)" % locals()
    
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut )
    
    return Selection( name + "_SelP2MuMu",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuon ]
                      )


def makeBc2JpsiH( name,
                  SelJpsi2MuMu,
                  PionTRCHI2DOF,
                  PionPT,
                  PionP,
                  BcVtxCHI2,
                  BcMassWindow,
                  BcPT,
                  LifetimeCut
                  ):

    #---------------------------
    # Pion or Kaon
    #---------------------------        
    from StandardParticles import StdAllNoPIDsPions as NoPIDsPionsForBc2JpsiH

    # Pion Cut
    PionCut = "(PT > %(PionPT)s *MeV) & (P > %(PionP)s *MeV) & (TRCHI2DOF < %(PionTRCHI2DOF)s)" % locals()

    _PionFilter = FilterDesktop( Code = PionCut )
    
    SelPion = Selection("SelPion_"+name,
                        Algorithm = _PionFilter,
                        RequiredSelections = [ NoPIDsPionsForBc2JpsiH ])

    
    #---------------------------
    # Bc -> J/psi(MuMu) H
    #---------------------------    
    # Comb cut
    combCut = "(ADAMASS('B_c+') < %(BcMassWindow)s *MeV)" % locals()
    
    # Bc Cut
    BcCut = "(VFASPF(VCHI2PDOF)< %(BcVtxCHI2)s ) & (PT > %(BcPT)s *MeV)" % locals()

    # Moved to conf dict
    ##LifetimeBiasedCut = "(MINTREE('pi+'==ABSID,BPVIPCHI2()) > %(PionIPCHI2)s) & (MINTREE('mu+'==ABSID,BPVIPCHI2()) > %(MuonIPCHI2)s) & (MINTREE('J/psi(1S)'==ABSID,BPVIPCHI2()) > %(MuMuIPCHI2)s) & (BPVIPCHI2() < %(BcIPCHI2)s) & (BPVLTIME()> %(BcLTIME)s*ps)" % locals()
    
    
    _Bc2JpsiH = CombineParticles( DecayDescriptor = "[ B_c+ -> J/psi(1S) pi+ ]cc",
                                  CombinationCut = combCut,
                                  MotherCut = BcCut + LifetimeCut,
                                  ReFitPVs = True )
    
    return Selection( name,
                      Algorithm = _Bc2JpsiH,
                      RequiredSelections = [ SelJpsi2MuMu, SelPion  ]
                      )
