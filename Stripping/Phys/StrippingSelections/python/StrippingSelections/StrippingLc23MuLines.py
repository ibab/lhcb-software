'''
Module for construction of Lambda_c -> mu ll lines

Performance (without prescaling);

Full.dst:
#########

StrippingReport                                                INFO Event 500000, Good event 500000
|                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
|!StrippingLc23MuLinesLc23muLine                              |  0.0092|        46|  1.065|   0.101|
|!StrippingLc23MuLinesLc2mueeLine                             |  0.0034|        17|  1.000|   0.109|
|!StrippingLc23MuLinesLc2pmumuLine                            |  0.0232|       116|  1.052|   0.195|
|!StrippingLc23MuLinesLc2peeLine                              |  0.0108|        54|  1.093|   2.267|
|!StrippingLc23MuLinesLc2pKpiLine                             |  0.5456|      2728|  1.136|   0.461|

Lc -> 3mu (25113002) MC
#######################

StrippingReport                                                INFO Event 100000, Good event 100000
|                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
|!StrippingLc23MuLinesLc23muLine                              |  2.0760|      2076|  1.025|   0.125|
                 

Lc -> p+ mu+ mu- (25113000) MC
##############################

StrippingReport                                                INFO Event 100000, Good event 100000
|                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
|!StrippingLc23MuLinesLc2pmumuLine                            |  1.0280|      1028|  1.042|   0.176|


Lc -> p~- mu+ mu+ (25113001) MC
###############################

StrippingReport                                                INFO Event 100000, Good event 100000
|                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
|!StrippingLc23MuLinesLc2pmumuLine                            |  0.9440|       944|  1.040|   0.159|
 

Lc -> p+ K- pi+ (25103000) MC
#############################

StrippingReport                                                INFO Event 100000, Good event 100000
|                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
|!StrippingLc23MuLinesLc2pKpiLine                             |  0.6750|       675|  1.107|   0.350|
              

Exported symbols (use python help!):
   - 
'''

__author__ = ['Oliver Gruenberg']
__date__ = '31.07.2014'
__version__ = '$Revision: 2.0 $'

###################################################################################################

__all__ = ('Lc23MuLinesConf',
           'config_default',
           'makeLc23mu',
           'makeLc2muee',
           'makeLc2pmumu',
           'makeLc2pee',
           'makeLc2pKpi',
           )

###################################################################################################

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, DaVinci__N3BodyDecays
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
                                  'Lc2pmumuPrescale',
                                  'Lc2peePrescale',
                                  'Lc2pKpiPrescale',
                                  )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'Postscale'           :1,
        'Lc23muPrescale'      :1,
        'Lc2mueePrescale'     :1,
        'Lc2pmumuPrescale'    :1,
        'Lc2peePrescale'      :1,        
        'Lc2pKpiPrescale'     :0.01,
        }                
    
    
    def __init__(self, name = 'Lc23mu', config = None) :

        LineBuilder.__init__(self, name, config)
        
        Lc23mu_name   = name+'Lc23mu'
        Lc2muee_name  = name+'Lc2muee'
        Lc2pmumu_name = name+'Lc2pmumu'
        Lc2pee_name   = name+'Lc2pee'
        Lc2pKpi_name  = name+'Lc2pKpi'

        self.selLc23mu    = makeLc23mu(Lc23mu_name)
        self.selLc2muee   = makeLc2muee(Lc2muee_name)
        self.selLc2pmumu  = makeLc2pmumu(Lc2pmumu_name)
        self.selLc2pee    = makeLc2pee(Lc2pee_name)
        self.selLc2pKpi   = makeLc2pKpi(Lc2pKpi_name)        

#######################################################################################################        

        self.Lc23mu_Line = StrippingLine(Lc23mu_name+"Line",
                                         prescale = config['Lc23muPrescale'],
                                         postscale = config['Postscale'],
                                         MDSTFlag = True,
                                         algos = [ self.selLc23mu ] )

        self.Lc2muee_Line = StrippingLine(Lc2muee_name+"Line",
                                          prescale = config['Lc2mueePrescale'],
                                          postscale = config['Postscale'],
                                          MDSTFlag = True,
                                          algos = [ self.selLc2muee ] )

        self.Lc2pmumu_Line = StrippingLine(Lc2pmumu_name+"Line",
                                           prescale = config['Lc2pmumuPrescale'],
                                           postscale = config['Postscale'],
                                           MDSTFlag = True,
                                           algos = [ self.selLc2pmumu ] )

        self.Lc2pee_Line = StrippingLine(Lc2pee_name+"Line",
                                         prescale = config['Lc2peePrescale'],
                                         postscale = config['Postscale'],
                                         MDSTFlag = True,
                                         algos = [ self.selLc2pee ] )

        self.Lc2pKpi_Line = StrippingLine(Lc2pKpi_name+"Line",
                                          prescale = config['Lc2pKpiPrescale'],
                                          postscale = config['Postscale'],
                                          MDSTFlag = True,
                                          algos = [ self.selLc2pKpi ] )
        
#######################################################################################################        

        ### Signal ch.
        self.registerLine(self.Lc23mu_Line)
        self.registerLine(self.Lc2muee_Line)
        self.registerLine(self.Lc2pmumu_Line)        
        self.registerLine(self.Lc2pee_Line)
        ### Control ch.
        self.registerLine(self.Lc2pKpi_Line)        
        
#######################################################################################################

TrackCuts = "(PT > 300*MeV) & (BPVIPCHI2()>9.0) & (TRCHI2DOF<3.0) & (TRGHP<0.3)"

########################### 

Combination12Cut = "(ADOCA(1,2)<0.3*mm)"

CombinationCut200 = "(ADAMASS('Lambda_c+')<200*MeV)"\
                    " & (ADOCA(1,3)<0.3*mm)"\
                    " & (ADOCA(2,3)<0.3*mm)"\

CombinationCut150 = "(ADAMASS('Lambda_c+')<150*MeV)"\
                    " & (ADOCA(1,3)<0.3*mm)"\
                    " & (ADOCA(2,3)<0.3*mm)"\

MotherCut = "( VFASPF(VCHI2) < 15 )"\
            " & ( (BPVLTIME()*c_light) > 70*micrometer )"\
            " & ( BPVIPCHI2() < 100 )"\
                   
#######################################################################################################

def makeLc23mu(name):
    
    Lc23mu = DaVinci__N3BodyDecays("Combine"+name)
    Lc23mu.DecayDescriptors = [ "[Lambda_c+ -> mu+ mu+ mu-]cc","[Lambda_c+ -> mu+ mu+ mu+]cc" ]
    Lc23mu.DaughtersCuts = { "mu+" : TrackCuts }
    Lc23mu.Combination12Cut = Combination12Cut
    Lc23mu.CombinationCut = CombinationCut200
     
    Lc23mu.MotherCut = MotherCut

    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
    return Selection (name, Algorithm = Lc23mu, RequiredSelections = [ _myMuons ])

#######################################################################################################

def makeLc2muee(name):
    
    Lc2muee = DaVinci__N3BodyDecays("Combine"+name)
    Lc2muee.DecayDescriptors = [ "[Lambda_c+ -> mu+ e+ e-]cc",
                                 "[Lambda_c+ -> mu- e+ e+]cc",
                                 "[Lambda_c+ -> mu+ e+ e+]cc"]
    Lc2muee.DaughtersCuts = { "mu+" : TrackCuts,
                              "e+"  : TrackCuts + " & ((PIDe-PIDpi)>2)" }
    Lc2muee.Combination12Cut = Combination12Cut
    Lc2muee.CombinationCut = CombinationCut200
     
    Lc2muee.MotherCut = MotherCut

    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    _myElectrons = DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")    
    
    return Selection (name, Algorithm = Lc2muee, RequiredSelections = [ _myMuons, _myElectrons ])


#######################################################################################################

def makeLc2pmumu(name):
    
    Lc2pmumu = DaVinci__N3BodyDecays("Combine"+name)
    Lc2pmumu.DecayDescriptors = [ "[Lambda_c+ -> p+ mu+ mu-]cc",
                                "[Lambda_c+ -> p~- mu+ mu+]cc",
                                "[Lambda_c+ -> p+ mu+ mu+]cc"]
    Lc2pmumu.DaughtersCuts = { "p+"  : TrackCuts + " & ((PIDp-PIDpi)>5)" + " & ((PIDp-PIDK)>0)",
                               "mu+" : TrackCuts + " & ((PIDmu-PIDpi)>-5)" + " & ((PIDmu-PIDK)>-5)" }
    Lc2pmumu.Combination12Cut = Combination12Cut
    Lc2pmumu.CombinationCut = CombinationCut200
     
    Lc2pmumu.MotherCut = MotherCut

    _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
    return Selection (name, Algorithm = Lc2pmumu, RequiredSelections = [ _myProtons, _myMuons ])

#######################################################################################################

def makeLc2pee(name):
    
    Lc2pee = DaVinci__N3BodyDecays("Combine"+name)
    Lc2pee.DecayDescriptors = [ "[Lambda_c+ -> p+ e+ e-]cc",
                                "[Lambda_c+ -> p~- e+ e+]cc",
                                "[Lambda_c+ -> p+ e+ e+]cc"]
    Lc2pee.DaughtersCuts = { "p+" : TrackCuts + " & ((PIDp-PIDpi)>5)" + " & ((PIDp-PIDK)>0)",
                             "e+" : TrackCuts + " & ((PIDe-PIDpi)>2)" }
    Lc2pee.Combination12Cut = Combination12Cut
    Lc2pee.CombinationCut = CombinationCut200
    Lc2pee.MotherCut = MotherCut

    _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    _myElectrons = DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")    
    
    return Selection (name, Algorithm = Lc2pee, RequiredSelections = [ _myProtons, _myElectrons ])


#######################################################################################################

def makeLc2pKpi(name):
    
    Lc2pKpi = DaVinci__N3BodyDecays("Combine"+name)
    Lc2pKpi.DecayDescriptors = [ "[Lambda_c+ -> p+ K- pi+]cc" ]
    Lc2pKpi.DaughtersCuts = { "p+"  : TrackCuts + " & ((PIDp-PIDpi)>5)" + " & ((PIDp-PIDK)>0)",
                              "K-"  : TrackCuts + " & ((PIDK-PIDpi)>5)" + " & ((PIDK-PIDp)>0)",
                              "pi+" : TrackCuts }
    Lc2pKpi.Combination12Cut = Combination12Cut
    Lc2pKpi.CombinationCut = CombinationCut150
    Lc2pKpi.MotherCut = MotherCut

    _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    _myKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")
    _myPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    
    return Selection (name, Algorithm = Lc2pKpi, RequiredSelections = [ _myProtons, _myKaons, _myPions ])

#######################################################################################################
