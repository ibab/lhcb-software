# $Id: StrippingBc2JpsiMuX.py,v 1.2 2010-09-06 16:30:22 jhe Exp $

'''
Module for construction of Bc->Jpsi(MuMu)X
'''

__author__=['Jibo He']
__date__ = '02/09/2010'
__version__= '$Revision: 2.0 $'


__all__ = (
    'Bc2JpsiMuXConf',
    'makeJpsi2MuMu',
    'makeBc2JpsiMu'
    )

default_config = {
    'NAME'        : 'Bc2JpsiMu',
    'BUILDERTYPE' : 'Bc2JpsiMuXConf',
    'CONFIG' : {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.   ,
   
    'MuonTRCHI2DOF'       :    5.   ,  # adimentional
    'MuonPT'              : 1400.   ,  # MeV
    'MuonP'               :   -5.   ,  # MeV, not applied now
    'MuMuParticleName'    : "'J/psi(1S)'", # Particle Name, like "'psi(2S)'"
    'MuMuMassWindow'      :  150.   ,  # MeV, 10 sigma, may decreased to 150
    'MuMuVtxCHI2'         :    9.   ,  # adimentional
    'MuMuPT'              :  -10.   ,  # MeV, not applied
   
    'MuonBcTRCHI2DOF'     :    5.   ,  # adimentional    
    'MuonBcPT'            : 2500.   ,  # MeV
    'MuonBcP'             :   -5.   ,  # MeV, not applied now
    'BcUpperMass'         : 6400.   ,  # MeV, Upper limit for partial rec.
    'BcLowerMass'         : 3200.   ,  # MeV, Lower limit for partial rec.
    'BcVtxCHI2'           :    9.   ,  # adimentional
    'BcPT'                : 6000.      # MeV, May incrase up to 5000 MeV if needed      
    },
    'STREAMS' : [ 'Dimuon' ] ,
    'WGs'    : [ 'BandQ' ]
  }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class Bc2JpsiMuXConf(LineBuilder):
    
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
        
        'MuonBcTRCHI2DOF',        
        'MuonBcPT',
        'MuonBcP',
        'BcVtxCHI2',
        'BcUpperMass',
        'BcLowerMass',
        'BcPT'
         )
    
    def __init__(self, name, config ):

        LineBuilder.__init__(self, name, config)
        
        Bc2JpsiMuXName = name
        self.SelJpsi2MuMu = makeJpsi2MuMu( 'Jpsi2MuMuFor'+Bc2JpsiMuXName,
                                           MuonPT = config['MuonPT'],
                                           MuonP  = config['MuonP'],
                                           MuonTRCHI2DOF = config['MuonTRCHI2DOF'],
                                           MuMuParticleName = config['MuMuParticleName'],
                                           MuMuVtxCHI2 = config['MuMuVtxCHI2'],
                                           MuMuMassWindow = config['MuMuMassWindow'],
                                           MuMuPT = config['MuMuPT']
                                           )
        
        self.SelBc2JpsiMuX = makeBc2JpsiMu( "Sel_"+Bc2JpsiMuXName,
                                            SelJpsi2MuMu = self.SelJpsi2MuMu,
                                            MuonBcTRCHI2DOF = config['MuonBcTRCHI2DOF'],
                                            MuonBcPT = config['MuonBcPT'],
                                            MuonBcP = config['MuonBcP'],
                                            BcVtxCHI2 = config['BcVtxCHI2'],
                                            BcUpperMass = config['BcUpperMass'],
                                            BcLowerMass = config['BcLowerMass'],
                                            BcPT = config['BcPT']                                           
                                            )
                                             
        self.line = StrippingLine( Bc2JpsiMuXName+"Line",
                                   prescale = config['LinePrescale'],
                                   postscale = config['LinePostscale'],
                                   RequiredRawEvents = ['Muon'],
                                   selection = self.SelBc2JpsiMuX 
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
    
    #MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    MuonCut = "(CHILDCUT((TRCHI2DOF < %(MuonTRCHI2DOF)s),1)) & (CHILDCUT((TRCHI2DOF < %(MuonTRCHI2DOF)s),2)) & (CHILDCUT((PT > %(MuonPT)s *MeV),1))  & (CHILDCUT((PT > %(MuonPT)s *MeV),2))" % locals()
    
    MuMuCut = "(ADMASS(%(MuMuParticleName)s) < %(MuMuMassWindow)s *MeV) & (VFASPF(VCHI2PDOF)< %(MuMuVtxCHI2)s) & (PT > %(MuMuPT)s *MeV)" % locals()
    
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut )
    
    return Selection( name + "_SelP2MuMu",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuon ]
                      )


def makeBc2JpsiMu( name,
                   SelJpsi2MuMu,
                   MuonBcTRCHI2DOF,
                   MuonBcPT,
                   MuonBcP,
                   BcVtxCHI2,
                   BcUpperMass,
                   BcLowerMass,
                   BcPT
                   ):

    #---------------------------
    # Muon
    #---------------------------        
    from StandardParticles import StdAllNoPIDsMuons as NoPIDsMuonsForBc2JpsiMu
    
    # MuBc Cut
    MuonBcCut = "(PT > %(MuonBcPT)s *MeV) & (P > %(MuonBcP)s *MeV) & (TRCHI2DOF < %(MuonBcTRCHI2DOF)s) & (TRGHOSTPROB<0.4)" % locals()

    _MuonBcFilter = FilterDesktop( Code = MuonBcCut )
    
    SelMuonBc = Selection("SelMuonBc_"+name,
                          Algorithm = _MuonBcFilter,
                          RequiredSelections = [ NoPIDsMuonsForBc2JpsiMu ])
    
    
    #---------------------------
    # Bc -> J/psi(MuMu) Mu X
    #---------------------------         
    # Comb cut
    combCut = "(in_range( %(BcLowerMass)s *MeV, AM, %(BcUpperMass)s *MeV))" % locals()
    
    # Bc Cut
    BcCut = "(VFASPF(VCHI2PDOF)< %(BcVtxCHI2)s ) & (PT > %(BcPT)s *MeV)" % locals()

    _Bc2JpsiMuX = CombineParticles( DecayDescriptor = "[ B_c+ -> J/psi(1S) mu+ ]cc",
                                    CombinationCut = combCut,
                                    MotherCut = BcCut,
                                    ReFitPVs = True )
    
    return Selection( name,
                      Algorithm = _Bc2JpsiMuX,
                      RequiredSelections = [ SelJpsi2MuMu, SelMuonBc ]
                      )
