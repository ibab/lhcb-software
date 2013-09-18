'''
Module for construction of stripping selections for some rare strange decays
Exported symbols (use python help!):
'''

__author__ = ['Francesco Dettori','Andrea Contu']
__date__ = '19/08/2013'
__version__ = '$Revision: 0.1 $'

__all__ = ('RareStrangeLinesConf',
           'config_default',
           'makeSigmaPMuMu',
           'makeSigmaPEE',
           'makeSigmaPMuMuLFV'
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.PhysicalConstants import c_light

#============================================================
# Line builder 
class RareStrangeLinesConf(LineBuilder) :
    """
    Builder 
 
    """

    __configuration_keys__ = (    'Postscale',
                                  'SigmaPMuMuPrescale',
                                  'SigmaPEEPrescale',
                                  'SigmaPMuMuLFVPrescale',
                                  'K3PiPrescale',
                                  'SigmaMinTauPs',
                                  'SigmaMinPt',
                                  'SigmaMaxDOCA',
                                  'SigmaMassWin',
                                  'SigmaMinDIRA',
                                  'SigmaMaxIpChi2',
                                  'SigmaVtxChi2',
                                  'muonMinIpChi2',
                                  'protonPIDp',
                                  'electronPIDe',
                                  'electronMinIpChi2',
                                  )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'Postscale'           :1,
        'SigmaPMuMuPrescale'      :1,
        'SigmaPEEPrescale'      :1,
        'SigmaPMuMuLFVPrescale'      :1,
        'K3PiPrescale' : 0,
        'SigmaMinTauPs' : 6,
        'SigmaMinPt' : 500.,
        'SigmaMaxDOCA' : 2.,
        'SigmaMassWin' : 500.,
        'SigmaMinDIRA' : 0.9,
        'SigmaMaxIpChi2' : 36.,
        'SigmaVtxChi2' : 36,
        'muonMinIpChi2' : 4.,
        'protonPIDp': 5.,
        'electronPIDe':2.,
        'electronMinIpChi2': 9
        }                
    
    
    def __init__(self, 
                 name = 'RareStrange',
                 config = None) :

        LineBuilder.__init__(self, name, config)
        

        spmumu_name=name+'SigmaPMuMu'
        spee_name=name+'SigmaPEE'
        spmumulfv_name=name+'SigmaPMuMuLFV'
        k3pi_name=name+'K3Pi'

        self.selSigmaPMuMu = makeSigmaPMuMu(spmumu_name, config)
        self.selSigmaPEE   = makeSigmaPEE(spee_name, config)
        self.selSigmaPMuMuLFV = makeSigmaPMuMuLFV(spmumulfv_name, config)
        self.selK3Pi = makeK3Pi(k3pi_name)
        
	self.SigmaPMuMuLine = StrippingLine(spmumu_name+"Line",
                                            prescale = config['SigmaPMuMuPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selSigmaPMuMu ]
                                     )
	self.SigmaPEELine = StrippingLine(spee_name+"Line",
                                            prescale = config['SigmaPEEPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selSigmaPEE ]
                                     )

	self.SigmaPMuMuLFVLine = StrippingLine(spmumulfv_name+"Line",
                                               prescale = config['SigmaPMuMuLFVPrescale'],
                                               postscale = config['Postscale'],
                                               algos = [ self.selSigmaPMuMuLFV ]
                                     )
        self.K3PiLine = StrippingLine(k3pi_name+"Line",
                                               prescale = config['K3PiPrescale'],
                                               postscale = config['Postscale'],
                                               algos = [ self.selK3Pi ]
                                     )
              
              

        self.registerLine(self.SigmaPMuMuLine)
        self.registerLine(self.SigmaPEELine)
        self.registerLine(self.SigmaPMuMuLFVLine)
        self.registerLine(self.K3PiLine)



#============================================================
# Sigma+ -> p mu mu 
def makeSigmaPMuMu(name, config):
    """
    Line for the selection of Sigma+ -> p+ mu+ mu-,  
    Before prescaling this line, please contact Francesco Dettori
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    from Configurables import OfflineVertexFitter
    SigmaPMuMu = CombineParticles("Combine"+name)
    SigmaPMuMu.DecayDescriptor = "[Sigma+ -> p+ mu+ mu-]cc"
    SigmaPMuMu.addTool( OfflineVertexFitter )
    SigmaPMuMu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    SigmaPMuMu.OfflineVertexFitter.useResonanceVertex = False
    SigmaPMuMu.DaughtersCuts = { "mu+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)"%config,
                                 "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDp)s)"%config
                                 }

    SigmaPMuMu.CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config

    SigmaPMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaMassWin)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  

    # AllLooseMuons are needed in order to avoid the pt and ip cuts used in StdLooseMuons: do not change this!
    _stdAllLooseMuons   = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    
    return Selection (name,
                      Algorithm = SigmaPMuMu,
                      RequiredSelections = [ _stdAllLooseMuons,
                                             _stdLooseProtons ])

#============================================================
# Sigma+ > p- mu+ mu+ 
def makeSigmaPMuMuLFV(name, config):
    """
    Line for the selection of Sigma+ -> p- mu+ mu+ for background checks 
    Before prescaling this line, please contact Francesco Dettori
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    from Configurables import OfflineVertexFitter
    SigmaPMuMu = CombineParticles("Combine"+name)
    SigmaPMuMu.DecayDescriptor = "[Sigma+ -> p~- mu+ mu+]cc"
    SigmaPMuMu.addTool( OfflineVertexFitter )
    SigmaPMuMu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    SigmaPMuMu.OfflineVertexFitter.useResonanceVertex = False
    SigmaPMuMu.DaughtersCuts = { "mu+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)"%config,
                                 "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDp)s)"%config
                                 }
    SigmaPMuMu.CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config

    SigmaPMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaMassWin)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  


    # AllLooseMuons are needed in order to avoid the pt and ip cuts used in StdLooseMuons: do not change this!
    _stdAllLooseMuons   = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")
    
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    
    return Selection (name,
                      Algorithm = SigmaPMuMu,
                      RequiredSelections = [ _stdAllLooseMuons,
                                             _stdLooseProtons ])

#============================================================
#  Sigma+ -> p pi0 (->e+ e- gamma)
def makeSigmaPEE(name, config):
    """
    Line for the selection of Sigma+ -> p+ e+ e- gamma,  (with photon undetected)
    for normalisation purposes
    Before prescaling this line, please contact Francesco Dettori
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    from Configurables import OfflineVertexFitter
    SigmaPEE = CombineParticles("Combine"+name)
    SigmaPEE.DecayDescriptor = "[Sigma+ -> p+ e+ e-]cc"
    SigmaPEE.addTool( OfflineVertexFitter )
    SigmaPEE.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    SigmaPEE.OfflineVertexFitter.useResonanceVertex = False

    
    SigmaPEE.DaughtersCuts = { "e+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDe > %(electronPIDe)s ) &  (MIPCHI2DV(PRIMARY)>%(electronMinIpChi2)s)"%config,
                               "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDp)s)"%config
                               }
    
    SigmaPEE.CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config

    SigmaPEE.MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaMassWin)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  
    
    # AllLooseElectrons are needed in order to avoid the pt and ip cuts used in StdLooseMuons: do not change this!
    _stdAllLooseElectrons   = DataOnDemand(Location = "Phys/StdAllLooseElectrons/Particles")
    
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")

    
    return Selection (name,
                      Algorithm = SigmaPEE,
                      RequiredSelections = [ _stdAllLooseElectrons,
                                             _stdLooseProtons ])


#============================================================
# K+-> pi+ pi+ pi-
def makeK3Pi(name):
    """
    Line for the selection of K+ -> pi+ pi+ pi-
    Before prescaling this line, please contact Francesco Dettori
    
    Arguments:
    name        : name of the Selection.
    """
    
    from Configurables import OfflineVertexFitter
    K3Pi = CombineParticles("Combine"+name)
    K3Pi.DecayDescriptor = "[K+ -> pi+ pi+ pi-]cc"
    K3Pi.addTool( OfflineVertexFitter )
    K3Pi.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    K3Pi.OfflineVertexFitter.useResonanceVertex = False
    K3Pi.DaughtersCuts = { "pi+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>9)"
                                 }

    K3Pi.CombinationCut = "(ADAMASS('K+')<100*MeV)"\
                                "& (AMAXDOCA('')<2*mm)"

    K3Pi.MotherCut = "(VFASPF(VCHI2/VDOF)<9)  & (PT>100*MeV)"\
                              "& (ADMASS('K+') < 100*MeV )"\
                              "& (BPVDIRA > 0.999) "\
                              "& (BPVIPCHI2()< 9)"\
                              "& (BPVLTIME()>10*ps)"   

                              #"& (BPVVDCHI2> 9)"\
                                 

    _stdAllLoosePions = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")
    
    return Selection (name,
                      Algorithm = K3Pi,
                      RequiredSelections = [ _stdAllLoosePions ])


#============================================================
