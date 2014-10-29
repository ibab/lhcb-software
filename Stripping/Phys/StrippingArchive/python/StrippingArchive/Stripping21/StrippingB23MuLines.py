'''
Module for construction of B+ -> mu l+ l- lines

Performance:

Full.dst
########

StrippingReport                                                INFO Event 500000, Good event 500000
|                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
|!StrippingB23MuLinesB23MuLine                                |  0.0002|         1|  1.000|   8.153|
|!StrippingB23MuLinesB2MueeLine                               |  0.0002|         1|  1.000|   4.891|
|!StrippingB23MuLinesB2DMuLine                                |  0.0018|         9|  1.111|   0.137|
|!StrippingB23MuLinesB23PiLine                                |  0.0282|       141|  1.057|   5.465|
       
B23Pi (12103002) MC
###################
StrippingReport                                                INFO Event 100000, Good event 100000
|                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
|!StrippingB23MuLinesB23PiLine                                | 16.0890|     16089|  1.005|   4.821|
 
Exported symbols (use python help!):

'''

__author__ = ['Oliver Gruenberg']
__date__ = '31.07.2014'
__version__ = '$Revision: 1.0 $'

#############################################################################

__all__ = ('B23MuLinesConf',
           'config_default',
           'makeB23Mu',
           'makeB2Muee',
           'makeB2DMu',
           'makeB23Pi',           
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

class B23MuLinesConf(LineBuilder) :
    """
    Builder
    
    """

    __configuration_keys__ = (    'Postscale',
                                  'B23MuPrescale',
                                  'B2MueePrescale',
                                  'B2DMuPrescale',
                                  'B23PiPrescale',                                  
                                  )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'Postscale'            :1,
        'B23MuPrescale'        :1,
        'B2MueePrescale'       :1,
        'B2DMuPrescale'        :1,
        'B23PiPrescale'        :1,        
        }                
    
    
    def __init__(self, name = 'B23Mu', config = None) :

        LineBuilder.__init__(self, name, config)
        
        B23Mu_name=name+'B23Mu'
        B2Muee_name=name+'B2Muee'
        B2DMu_name=name+'B2DMu'
        B23Pi_name=name+'B23Pi'        

        self.selB23Mu = makeB23Mu(B23Mu_name)
        self.selB2Muee = makeB2Muee(B2Muee_name)
        self.selB2DMu = makeB2DMu(B2DMu_name)
        self.selB23Pi = makeB23Pi(B23Pi_name)        
        
#############################################################################

        self.B23Mu_Line = StrippingLine(B23Mu_name+"Line",
                                        prescale = config['B23MuPrescale'],
                                        postscale = config['Postscale'],
                                        MDSTFlag = True,
                                        algos = [ self.selB23Mu ] )

        self.B2Muee_Line = StrippingLine(B2Muee_name+"Line",
                                         prescale = config['B2MueePrescale'],
                                         postscale = config['Postscale'],
                                         MDSTFlag = True,
                                         algos = [ self.selB2Muee ] )

        self.B2DMu_Line = StrippingLine(B2DMu_name+"Line",
                                        prescale = config['B2DMuPrescale'],
                                        postscale = config['Postscale'],
                                        MDSTFlag = True,
                                        algos = [ self.selB2DMu ] )

        self.B23Pi_Line = StrippingLine(B23Pi_name+"Line",
                                        prescale = config['B23PiPrescale'],
                                        postscale = config['Postscale'],
                                        MDSTFlag = True,
                                        algos = [ self.selB23Pi ] )

#############################################################################

        self.registerLine(self.B23Mu_Line)
        self.registerLine(self.B2Muee_Line)
        self.registerLine(self.B2DMu_Line)
        self.registerLine(self.B23Pi_Line)
       
#############################################################################

TrackCuts = "(MIPCHI2DV(PRIMARY)>25.0) & (TRCHI2DOF<3.0) & (TRGHP<0.3)"

BCombination12Cut = "(ADOCA(1,2)<0.3*mm)"

BCombinationCut500 = "(ADAMASS('B+') < 500*MeV)"\
                     " & (ADOCA(1,3)<0.3*mm)"\
                     " & (ADOCA(2,3)<0.3*mm)"\

BCombinationCut100 = "(ADAMASS('B+') < 100*MeV)"\
                     " & (ADOCA(1,3)<0.3*mm)"\
                     " & (ADOCA(2,3)<0.3*mm)"\

BMotherCut = "(BPVIPCHI2() < 25)"\
             " & (BPVVDCHI2 > 225)"\
             " & (VFASPF(VCHI2/VDOF)<9)"\
             " & (BPVDIRA > 0.0)"\
             " & (BPVLTIME() > 0.0*ps)"\
             
#############################################################################

DTrackCuts = "(MIPCHI2DV()>1.0) & (TRCHI2DOF<5.0) & (PT>1.0*GeV)"

DCombinationCut = "(ADAMASS('D0') < 110*MeV)"\

DMotherCut = "(ADMASS('D0') < 100*MeV)"\
             " & (VFASPF(VCHI2)<16)"\
             " & (BPVLTIME(16) > 0.1*ps)"\

#############################################################################

BDTrackCuts = "(MIPCHI2DV(PRIMARY)>25.0) & (TRCHI2DOF<3.0) & (TRGHP<0.3)"

#BDCombinationCut = "(ADAMASS('B+') < 2000*MeV)"\

BDMotherCut ="(BPVVDCHI2 > 25)"\
             " & (VFASPF(VCHI2/VDOF)<9)"\
             " & (BPVDIRA > 0.0)"\

#############################################################################

def makeB23Mu(name):
    
    B23Mu = DaVinci__N3BodyDecays("Combine"+name)
    
    B23Mu.DecayDescriptors = [ "[B+ -> mu+ mu+ mu-]cc","[B+ -> mu+ mu+ mu+]cc" ]
    B23Mu.DaughtersCuts = { "mu+" : TrackCuts }
    B23Mu.Combination12Cut = BCombination12Cut
    B23Mu.CombinationCut = BCombinationCut500
     
    B23Mu.MotherCut = BMotherCut

    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
    return Selection (name, Algorithm = B23Mu, RequiredSelections = [ _myMuons ])

#############################################################################

def makeB2Muee(name):
    
    B2Muee = DaVinci__N3BodyDecays("Combine"+name)
    B2Muee.DecayDescriptors = [ "[B+ -> mu+ e+ e-]cc","[B+ -> mu- e+ e+]cc","[B+ -> mu+ e+ e+]cc" ]
    B2Muee.DaughtersCuts = { "mu+" : TrackCuts, "e+" : TrackCuts }
    B2Muee.Combination12Cut = BCombination12Cut
    B2Muee.CombinationCut = BCombinationCut500
     
    B2Muee.MotherCut = BMotherCut

    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    _myElectrons = DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")
    
    return Selection (name, Algorithm = B2Muee, RequiredSelections = [ _myMuons, _myElectrons ])

#############################################################################

def makeB2DMu(name):

    D2MuMu = CombineParticles("MyD2MuMuCombination")
    D2MuMu.DecayDescriptors = [ "[D0 -> mu+ mu-]cc","[D0 -> mu+ mu+]cc" ]
    D2MuMu.DaughtersCuts = { "mu+" : DTrackCuts }
    D2MuMu.CombinationCut = DCombinationCut
    D2MuMu.MotherCut = DMotherCut

    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
    D2MuMuSelection = Selection ("MyD2MuMuSelection", Algorithm = D2MuMu, RequiredSelections = [ _myMuons ])

    B2DMu = CombineParticles("Combine"+name)
    B2DMu.DecayDescriptors = [ "[B+ -> D0 mu+]cc" ]
    B2DMu.DaughtersCuts = { "mu+" : TrackCuts }
    #B2DMu.CombinationCut = BDCombinationCut
    B2DMu.MotherCut = BDMotherCut

    return Selection (name, Algorithm = B2DMu, RequiredSelections = [ D2MuMuSelection, _myMuons ])

#############################################################################

def makeB23Pi(name):
    
    B23Pi = DaVinci__N3BodyDecays("Combine"+name)
    B23Pi.DecayDescriptors = [ "[B+ -> pi+ pi+ pi-]cc" ]
    B23Pi.DaughtersCuts = { "pi+" : TrackCuts }
    B23Pi.Combination12Cut = BCombination12Cut
    B23Pi.CombinationCut = BCombinationCut100
     
    B23Pi.MotherCut = BMotherCut

    _myPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    
    return Selection (name, Algorithm = B23Pi, RequiredSelections = [ _myPions ])

#############################################################################
