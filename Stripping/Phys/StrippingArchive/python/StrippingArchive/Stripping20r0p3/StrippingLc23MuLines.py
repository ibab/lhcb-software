'''
Module for construction of Lambda_c -> mu ll lines

Exported symbols (use python help!):
   - 
'''

__author__ = ['Oliver Gruenberg']
__date__ = '15.03.2013'
__version__ = '$Revision: 1.0 $'

###################################################################################################

__all__ = ('Lc23MuLinesConf',
           'config_default',
           'makeLc23mu',
           'makeLc2muee',            
           'makeLc2pKpi',         
           )

###################################################################################################

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.PhysicalConstants import c_light

###################################################################################################

class Lc23MuLinesConf(LineBuilder) :
    """
    Builder
    
    """

    __configuration_keys__ = (    'Postscale',
                                  'Lc23muPrescale',
                                  'Lc2mueePrescale',                                  
                                  'Lc2pKpiPrescale',                                   
                                  )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'Postscale'           :1,
        'Lc23muPrescale'      :1,
        'Lc2mueePrescale'     :1,        
        'Lc2pKpiPrescale'     :0.01,
        }                
    
    
    def __init__(self, name = 'Lc23mu', config = None) :

        LineBuilder.__init__(self, name, config)
        
        Lc23mu_name=name+'Lc23mu'
        Lc2muee_name=name+'Lc2muee'
        Lc2pKpi_name=name+'Lc2pKpi'        

        self.selLc23mu   = makeLc23mu(Lc23mu_name)
        self.selLc2muee  = makeLc2muee(Lc2muee_name)
        self.selLc2pKpi  = makeLc2pKpi(Lc2pKpi_name)        

#######################################################################################################        

        self.Lc23mu_Line = StrippingLine(Lc23mu_name+"Line",
                                     prescale = config['Lc23muPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selLc23mu ]
                                     )

        self.Lc2muee_Line = StrippingLine(Lc2muee_name+"Line",
                                     prescale = config['Lc2mueePrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selLc2muee ]
                                     )

        self.Lc2pKpi_Line = StrippingLine(Lc2pKpi_name+"Line",
                                     prescale = config['Lc2pKpiPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selLc2pKpi ]
                                     )
        
#######################################################################################################        

        ### Signal ch.
        self.registerLine(self.Lc23mu_Line)
        #self.registerLine(self.Lc2muee_Line)        
        ### Control ch.
        self.registerLine(self.Lc2pKpi_Line)        
        
#######################################################################################################

TrackCuts = "(PT > 300*MeV) & (BPVIPCHI2()>9.0) & (TRCHI2DOF<3.0) & (TRGHP<0.3)"

########################### 

LambdaCCombiCut300 = "(ADAMASS('Lambda_c+')<300*MeV)"\

LambdaCCombiCut200 = "(ADAMASS('Lambda_c+')<200*MeV)"\

CommonMotherCut = "( VFASPF(VCHI2) < 15 )"\
                   " & ( (BPVLTIME()*c_light) > 100*micrometer )"\
                   " & ( BPVIPCHI2() < 225 )"\
                   
#######################################################################################################

def makeLc23mu(name):
    
    Lc23mu = CombineParticles("Combine"+name)
    Lc23mu.DecayDescriptors = [ "[Lambda_c+ -> mu+ mu+ mu-]cc","[Lambda_c+ -> mu+ mu+ mu+]cc" ]
    Lc23mu.DaughtersCuts = { "mu+" : TrackCuts }

    Lc23mu.CombinationCut = LambdaCCombiCut300
     
    Lc23mu.MotherCut = CommonMotherCut

    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
    return Selection (name, Algorithm = Lc23mu, RequiredSelections = [ _myMuons ])

#######################################################################################################

def makeLc2muee(name):
    
    Lc2muee = CombineParticles("Combine"+name)
    Lc2muee.DecayDescriptors = [ "[Lambda_c+ -> mu+ e+ e-]cc",
                                "[Lambda_c+ -> mu- e+ e+]cc",
                                "[Lambda_c+ -> mu+ e+ e+]cc"]
    Lc2muee.DaughtersCuts = { "mu+" : TrackCuts,
                             "e+" : TrackCuts + " & ((PIDe-PIDpi)>2)" }

    Lc2muee.CombinationCut = LambdaCCombiCut300
     
    Lc2muee.MotherCut = CommonMotherCut

    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    _myElectrons = DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")    
    
    return Selection (name, Algorithm = Lc2muee, RequiredSelections = [ _myMuons, _myElectrons ])


#######################################################################################################
#######################################################################################################

def makeLc2pKpi(name):
    
    Lc2pKpi = CombineParticles("Combine"+name)
    Lc2pKpi.DecayDescriptors = [ "[Lambda_c+ -> p+ K- pi+]cc" ]
    Lc2pKpi.DaughtersCuts = { "p+"  : TrackCuts + " & ((PIDp-PIDpi)>5)" + " & ((PIDp-PIDK)>0)",
                              "K-"  : TrackCuts + " & ((PIDK-PIDpi)>5)" + " & ((PIDK-PIDp)>0)",
                              "pi+" : TrackCuts }

    Lc2pKpi.CombinationCut = LambdaCCombiCut200
     
    Lc2pKpi.MotherCut = CommonMotherCut

    _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    _myKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")
    _myPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    
    return Selection (name, Algorithm = Lc2pKpi, RequiredSelections = [ _myProtons, _myKaons, _myPions ])

#######################################################################################################
