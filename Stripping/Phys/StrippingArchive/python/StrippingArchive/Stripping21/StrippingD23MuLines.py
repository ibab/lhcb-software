'''
Module for construction of D+ -> mu l+ l- lines

Performance (without any prescaling):

Full.dst:
#########

StrippingReport                                                INFO Event 500000, Good event 500000
|                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
|!StrippingD23MuLinesD23MuLine                                |  0.0014|         7|  1.000|   0.110|
|!StrippingD23MuLinesD2MueeLine                               |  0.0026|        13|  1.000|   0.136|
|!StrippingD23MuLinesD23PiLine                                |  1.2966|      6483|  1.138|   0.463|

D+ -> 3pi (21163012) MC
#######################

StrippingReport                                                INFO Event 100000, Good event 100000
|                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
|!StrippingD23MuLinesD23PiLine                                | 64.2100|     64210|  1.017|   0.653|
            

Exported symbols (use python help!):
   - 
'''

__author__ = ['Oliver Gruenberg']
__date__ = '31.07.2014'
__version__ = '$Revision: 1.0 $'

#############################################################################

__all__ = ('D23MuLinesConf',
           'config_default',
           'makeD23Mu', 
           'makeD2Muee', 
           'makeD23Pi', 
           )

#############################################################################

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, DaVinci__N3BodyDecays
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StrippingSelections.Utils import checkConfig
from GaudiKernel.PhysicalConstants import c_light

#############################################################################

class D23MuLinesConf(LineBuilder) :
    """
    Builder
    
    """

    __configuration_keys__ = (    'Postscale',
                                  'D23MuPrescale',
                                  'D2MueePrescale',
                                  'D23PiPrescale',                                  
                                  )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'Postscale'            :1,
        'D23MuPrescale'        :1,
        'D2MueePrescale'       :1,
        'D23PiPrescale'        :0.01,
        }                
    
    
    def __init__(self, name = 'D23Mu', config = None) :

        LineBuilder.__init__(self, name, config)
        
        D23Mu_name=name+'D23Mu'
        D2Muee_name=name+'D2Muee'
        D23Pi_name=name+'D23Pi'        

        self.selD23Mu = makeD23Mu(D23Mu_name)
        self.selD2Muee = makeD2Muee(D2Muee_name)
        self.selD23Pi = makeD23Pi(D23Pi_name)
        
#############################################################################

        self.D23Mu_Line = StrippingLine(D23Mu_name+"Line",
                                        prescale = config['D23MuPrescale'],
                                        postscale = config['Postscale'],
                                        MDSTFlag = True,
                                        algos = [ self.selD23Mu ] )

        self.D2Muee_Line = StrippingLine(D2Muee_name+"Line",
                                         prescale = config['D2MueePrescale'],
                                         postscale = config['Postscale'],
                                         MDSTFlag = True,
                                         algos = [ self.selD2Muee ] )

        self.D23Pi_Line = StrippingLine(D23Pi_name+"Line",
                                        prescale = config['D23PiPrescale'],
                                        postscale = config['Postscale'],
                                        MDSTFlag = True,
                                        algos = [ self.selD23Pi ] )

#############################################################################

        self.registerLine(self.D23Mu_Line)
        self.registerLine(self.D2Muee_Line)
        self.registerLine(self.D23Pi_Line)        
       
#############################################################################

#TrackCuts = "(MIPCHI2DV()>1.0) & (TRCHI2DOF<5.0) & (PT>1.0*GeV)"

#CombinationCut = "(ADAMASS(1920*MeV) < 300*MeV)"

#MotherCut = "(ADMASS(1920*MeV) < 100*MeV) & (VFASPF(VCHI2)<16) & (BPVLTIME(16) > 0.1*ps)"

###########

TrackCuts = "(MIPCHI2DV(PRIMARY)>25.0) & (TRCHI2DOF<3.0) & (TRGHP<0.3)"

Combination12Cut = "(ADOCA(1,2)<0.3*mm)"

CombinationCut = "(ADAMASS(1920*MeV) < 150*MeV)"\
                 " & (ADOCA(1,3)<0.3*mm)"\
                 " & (ADOCA(2,3)<0.3*mm)"\

MotherCut = "(BPVIPCHI2() < 25)"\
             " & (BPVVDCHI2 > 225)"\
             " & (VFASPF(VCHI2/VDOF)<9)"\
             " & (BPVDIRA > 0.0)"\
             " & (BPVLTIME() > 0.1*ps)"\

#############################################################################

def makeD23Mu(name):
    
    D23Mu = DaVinci__N3BodyDecays("Combine"+name)
    D23Mu.DecayDescriptors = [ "[D+ -> mu+ mu+ mu-]cc","[D+ -> mu+ mu+ mu+]cc" ]
    D23Mu.DaughtersCuts = { "mu+" : TrackCuts }
    D23Mu.Combination12Cut = Combination12Cut
    D23Mu.CombinationCut = CombinationCut
    D23Mu.MotherCut = MotherCut

    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
   
    return Selection (name, Algorithm = D23Mu, RequiredSelections = [ _myMuons ])

#############################################################################

def makeD2Muee(name):
    
    D2Muee = DaVinci__N3BodyDecays("Combine"+name)
    D2Muee.DecayDescriptors = [ "[D+ -> mu+ e+ e-]cc","[D+ -> mu- e+ e+]cc","[D+ -> mu+ e+ e+]cc" ]
    D2Muee.DaughtersCuts = { "mu+" : TrackCuts, "e+" : TrackCuts }
    D2Muee.Combination12Cut = Combination12Cut
    D2Muee.CombinationCut = CombinationCut
    D2Muee.MotherCut = MotherCut

    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    _myElectrons = DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")
    
    return Selection (name, Algorithm = D2Muee, RequiredSelections = [ _myMuons, _myElectrons ])

#############################################################################

def makeD23Pi(name):
    
    D23Pi = DaVinci__N3BodyDecays("Combine"+name)
    D23Pi.DecayDescriptors = [ "[D+ -> pi+ pi+ pi-]cc" ]
    D23Pi.DaughtersCuts = { "pi+" : TrackCuts }
    D23Pi.Combination12Cut = Combination12Cut
    D23Pi.CombinationCut = CombinationCut
    D23Pi.MotherCut = MotherCut

    _myPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    
    return Selection (name, Algorithm = D23Pi, RequiredSelections = [ _myPions ])

#############################################################################
