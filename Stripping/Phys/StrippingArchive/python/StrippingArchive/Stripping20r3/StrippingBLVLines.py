'''
Module for construction of baryon-lepton number violating stripping selections and lines

Exported symbols (use python help!):
   - 
'''

__author__ = ['Oliver Gruenberg']
__date__ = '14.03.2013'
__version__ = '$Revision: 1.0 $'

###################################################################################################

__all__ = ('BLVLinesConf',
           'config_default',
           ### Signal Ch.
           'makeLa2Kmu',
           'makeLb2Kmu',
           'makeLb2Dmu',
           'makeLb2Dsmu',
           'makeB2Lcmu',
           ### Control Ch.
           'makeLb2Lcpi',
           'makeB2Dpi',
           'makeBs2Dspi',
           )

###################################################################################################

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

###################################################################################################

class BLVLinesConf(LineBuilder) :
    """
    Builder
    
    """

    __configuration_keys__ = (    'Postscale',
                                  ### Signal Ch.
                                  'La2KmuPrescale',
                                  'Lb2KmuPrescale',
                                  'Lb2DmuPrescale',
                                  'Lb2DsmuPrescale',
                                  'B2LcmuPrescale',
                                  ### Control Ch.
                                  'Lb2LcpiPrescale',
                                  'B2DpiPrescale',
                                  'Bs2DspiPrescale',
                                  )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'Postscale'           :1,
        ### Signal Ch.
        'La2KmuPrescale'      :1,
        'Lb2KmuPrescale'      :1,
        'Lb2DmuPrescale'      :1, 
        'Lb2DsmuPrescale'     :1,     
        'B2LcmuPrescale'      :1,
        ### Control Ch.
        'Lb2LcpiPrescale'     :0.1,
        'B2DpiPrescale'       :0.1,
        'Bs2DspiPrescale'     :0.1,
        }                
    
    
    def __init__(self, name = 'BLV', config = None) :

        LineBuilder.__init__(self, name, config)
        ### Signal Ch.
        La2Kmu_name=name+'La2Kmu'
        Lb2Kmu_name=name+'Lb2Kmu'
        Lb2Dmu_name=name+'Lb2Dmu'
        Lb2Dsmu_name=name+'Lb2Dsmu'
        B2Lcmu_name=name+'B2Lcmu'

        self.selLa2Kmu  = makeLa2Kmu(La2Kmu_name)
        self.selLb2Kmu  = makeLb2Kmu(Lb2Kmu_name)
        self.selLb2Dmu  = makeLb2Dmu(Lb2Dmu_name)
        self.selLb2Dsmu = makeLb2Dsmu(Lb2Dsmu_name)
        self.selB2Lcmu  = makeB2Lcmu(B2Lcmu_name)

        ### Control Ch.
        Lb2Lcpi_name=name+'Lb2Lcpi'
        B2Dpi_name=name+'B2Dpi'
        Bs2Dspi_name=name+'Bs2Dspi'

        self.selLb2Lcpi = makeLb2Lcpi(Lb2Lcpi_name)
        self.selB2Dpi  = makeB2Dpi(B2Dpi_name)
        self.selBs2Dspi  = makeBs2Dspi(Bs2Dspi_name)


#######################################################################################################

        self.La2KmuLine = StrippingLine(La2Kmu_name+"Line",
                                     prescale = config['La2KmuPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selLa2Kmu ]
                                     )

#######################################################################################################

        self.Lb2KmuLine = StrippingLine(Lb2Kmu_name+"Line",
                                     prescale = config['Lb2KmuPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selLb2Kmu ]
                                     )
        
#######################################################################################################

        self.Lb2DmuLine = StrippingLine(Lb2Dmu_name+"Line",
                                     prescale = config['Lb2DmuPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selLb2Dmu ]
                                     )

#######################################################################################################

        self.Lb2DsmuLine = StrippingLine(Lb2Dsmu_name+"Line",
                                     prescale = config['Lb2DsmuPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selLb2Dsmu ]
                                     )

#######################################################################################################

        self.b2LcmuLine = StrippingLine(B2Lcmu_name+"Line",
                                     prescale = config['B2LcmuPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selB2Lcmu ]
                                     )

#######################################################################################################
#######################################################################################################

        self.Lb2LcpiLine = StrippingLine(Lb2Lcpi_name+"Line",
                                     prescale = config['Lb2LcpiPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selLb2Lcpi ]
                                     )

#######################################################################################################

        self.B2DpiLine = StrippingLine(B2Dpi_name+"Line",
                                     prescale = config['B2DpiPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selB2Dpi ]
                                     )

#######################################################################################################

        self.Bs2DspiLine = StrippingLine(Bs2Dspi_name+"Line",
                                     prescale = config['Bs2DspiPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selBs2Dspi ]
                                     )

#######################################################################################################        

        ### Signal Ch.
        self.registerLine(self.La2KmuLine)
        self.registerLine(self.Lb2KmuLine)
        self.registerLine(self.Lb2DmuLine)
        self.registerLine(self.Lb2DsmuLine)
        self.registerLine(self.b2LcmuLine)
        ### Control Ch.
        self.registerLine(self.Lb2LcpiLine)        
        self.registerLine(self.B2DpiLine)
        self.registerLine(self.Bs2DspiLine)
        
#######################################################################################################

TrackCuts = "(MIPCHI2DV(PRIMARY)>25.0) & (TRCHI2DOF<3.0) & (TRGHP<0.3)"

DCut = "ADMASS('D+') < 100*MeV"
DsCut = "ADMASS('D_s+') < 100*MeV"

########################### Lambda0 -> K+ mu-

LambdaCombinationCut = "(ADAMASS('Lambda0')<400*MeV)"\
                       " & (AMAXDOCA('') < 0.3*mm)"\

LambdaMotherCut = "(ADMASS('Lambda0') < 400*MeV )"\
                  " & (BPVIPCHI2() < 25)"\
                  " & (BPVVDCHI2 > 225)"\
                  " & (VFASPF(VCHI2/VDOF)<9)"\
                  " & (BPVDIRA > 0.0)"\
                  " & (BPVLTIME() > 10*ps)"\

########################### LambdaB -> K+ mu-

Lb2KmuCombinationCut = "(ADAMASS('Lambda_b0')<500*MeV)"\
                       " & (AMAXDOCA('') < 0.3*mm)"\
                 
Lb2KmuMotherCut = "(ADMASS('Lambda_b0') < 500*MeV )"\
                  " & (BPVIPCHI2() < 25)"\
                  " & (BPVVDCHI2 > 100)"\
                  " & (VFASPF(VCHI2/VDOF)<9)"\
                  " & (BPVDIRA > 0.0)"\
                  " & (BPVLTIME() > 0.0*ps)"\

########################### LambdaB -> D(s)+ mu- , LambdaB -> LambdaC+ pi-

Lb2DmuCombinationCut = "(ADAMASS('Lambda_b0')<400*MeV)"\
                       " & (AMAXDOCA('') < 0.3*mm)"\

Lb2DmuMotherCut = "(ADMASS('Lambda_b0') < 400*MeV )"\
                  " & (BPVIPCHI2() < 25)"\
                  " & (BPVVDCHI2 > 225)"\
                  " & (VFASPF(VCHI2/VDOF)<9)"\
                  " & (BPVDIRA > 0.0)"\
                  " & (BPVLTIME() > 1.0*ps)"\

########################### B(s)0 -> LambdaC+ mu-

BCombinationCut = "(ADAMASS(5323*MeV) < 400*MeV)"\
                  " & (AMAXDOCA('') < 0.3*mm)"\

BMotherCut = "(ADMASS(5323*MeV) < 400*MeV )"\
             " & (BPVIPCHI2() < 25)"\
             " & (BPVVDCHI2 > 225)"\
             " & (VFASPF(VCHI2/VDOF)<9)"\
             " & (BPVDIRA > 0.0)"\
             " & (BPVLTIME() > 1.0*ps)"\

########################### B0 -> D- pi+

B0CombinationCut = "(ADAMASS('B0') < 400*MeV)"\
                  " & (AMAXDOCA('') < 0.3*mm)"\

B0MotherCut = "(ADMASS('B0') < 400*MeV )"\
             " & (BPVIPCHI2() < 25)"\
             " & (BPVVDCHI2 > 225)"\
             " & (VFASPF(VCHI2/VDOF)<9)"\
             " & (BPVDIRA > 0.0)"\
             " & (BPVLTIME() > 1.0*ps)"\

########################### B_s0 -> D_s- pi+

BsCombinationCut = "(ADAMASS('B_s0') < 400*MeV)"\
                  " & (AMAXDOCA('') < 0.3*mm)"\

BsMotherCut = "(ADMASS('B_s0') < 400*MeV )"\
             " & (BPVIPCHI2() < 25)"\
             " & (BPVVDCHI2 > 225)"\
             " & (VFASPF(VCHI2/VDOF)<9)"\
             " & (BPVDIRA > 0.0)"\
             " & (BPVLTIME() > 1.0*ps)"\

#######################################################################################################
#######################################################################################################

def makeLa2Kmu(name):
    
    La2Kmu = CombineParticles("Combine"+name)
    La2Kmu.DecayDescriptors = [ "[Lambda0 -> K+ mu-]cc","[Lambda0 -> K+ mu+]cc" ]
    La2Kmu.DaughtersCuts = { "K+" : TrackCuts + " & ((PIDK-PIDpi)>5)" + " & ((PIDK-PIDp)>0)",
                             "mu-" : TrackCuts }
                             

    La2Kmu.CombinationCut = LambdaCombinationCut
    
    La2Kmu.MotherCut = LambdaMotherCut

    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    _myKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")
    
    return Selection (name, Algorithm = La2Kmu, RequiredSelections = [ _myKaons, _myMuons ])

#######################################################################################################

def makeLb2Kmu(name):
    
    Lb2Kmu = CombineParticles("Combine"+name)
    Lb2Kmu.DecayDescriptors = [ "[Lambda_b0 -> K+ mu-]cc","[Lambda_b0 -> K+ mu+]cc" ]
    Lb2Kmu.DaughtersCuts = { "K+" : TrackCuts + " & ((PIDK-PIDpi)>0)",
                             "mu+" : TrackCuts }

    Lb2Kmu.CombinationCut = Lb2KmuCombinationCut
    
    Lb2Kmu.MotherCut = Lb2KmuMotherCut
 
    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    _myKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")
    
    return Selection (name, Algorithm = Lb2Kmu, RequiredSelections = [ _myKaons, _myMuons ])

#######################################################################################################

def makeLb2Dmu(name):
    
    Lb2Dmu = CombineParticles("Combine"+name)
    Lb2Dmu.DecayDescriptors = [ "[Lambda_b0 -> D+ mu-]cc","[Lambda_b0 -> D+ mu+]cc" ]
    Lb2Dmu.DaughtersCuts = { "D+" : DCut, "mu-" : TrackCuts }

    Lb2Dmu.CombinationCut = Lb2DmuCombinationCut
 
    Lb2Dmu.MotherCut = Lb2DmuMotherCut

    _myDplus = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi/Particles")
    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
   
    return Selection (name, Algorithm = Lb2Dmu, RequiredSelections = [ _myDplus, _myMuons ])
 
#######################################################################################################

def makeLb2Dsmu(name):
    
    Lb2Dsmu = CombineParticles("Combine"+name)
    Lb2Dsmu.DecayDescriptors = [ "[Lambda_b0 -> D_s+ mu-]cc","[Lambda_b0 -> D_s+ mu+]cc" ]
    Lb2Dsmu.DaughtersCuts = { "D_s+" : DsCut, "mu-" : TrackCuts } 

    Lb2Dsmu.CombinationCut = Lb2DmuCombinationCut
    
    Lb2Dsmu.MotherCut = Lb2DmuMotherCut

    _myDsplus = DataOnDemand(Location = "Phys/StdLooseDsplus2KKPi/Particles")
    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
    return Selection (name, Algorithm = Lb2Dsmu, RequiredSelections = [ _myDsplus, _myMuons ])

#######################################################################################################

def makeB2Lcmu(name):
    
    B2Lcmu = CombineParticles("Combine"+name)
    B2Lcmu.DecayDescriptors = [ "[B0 -> Lambda_c+ mu-]cc","[B0 -> Lambda_c+ mu+]cc" ]
    B2Lcmu.DaughtersCuts = { "mu-" : TrackCuts }

    B2Lcmu.CombinationCut = BCombinationCut
    
    B2Lcmu.MotherCut = BMotherCut

    _myLambdaC= DataOnDemand(Location = "Phys/StdLooseLambdac2PKPi/Particles")
    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
    return Selection (name, Algorithm = B2Lcmu, RequiredSelections = [ _myLambdaC, _myMuons ])

#######################################################################################################
#######################################################################################################

def makeLb2Lcpi(name):
    
    Lb2Lcpi = CombineParticles("Combine"+name)
    Lb2Lcpi.DecayDescriptors = [ "[Lambda_b0 -> Lambda_c+ pi-]cc","[Lambda_b0 -> Lambda_c+ pi+]cc" ]
    Lb2Lcpi.DaughtersCuts = { "pi-" : TrackCuts }

    Lb2Lcpi.CombinationCut =  Lb2DmuCombinationCut 
    
    Lb2Lcpi.MotherCut = Lb2DmuMotherCut
 
    _myLambdaC= DataOnDemand(Location = "Phys/StdLooseLambdac2PKPi/Particles")
    _myPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    
    return Selection (name, Algorithm = Lb2Lcpi, RequiredSelections = [ _myLambdaC, _myPions ])

#######################################################################################################

def makeB2Dpi(name):
    
    B2Dpi = CombineParticles("Combine"+name)
    B2Dpi.DecayDescriptors = [ "[B0 -> D- pi+]cc" ]
    B2Dpi.DaughtersCuts = { "pi+" : TrackCuts, "D-" : DCut }

    B2Dpi.CombinationCut = B0CombinationCut
    
    B2Dpi.MotherCut = B0MotherCut

    _myDplus = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi/Particles")
    _myPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    
    return Selection (name, Algorithm = B2Dpi, RequiredSelections = [ _myDplus, _myPions ])

#######################################################################################################

def makeBs2Dspi(name):
    
    Bs2Dspi = CombineParticles("Combine"+name)
    Bs2Dspi.DecayDescriptors = [ "[B_s0 -> D_s- pi+]cc" ]
    Bs2Dspi.DaughtersCuts = { "pi+" : TrackCuts, "D_s-" : DsCut }

    Bs2Dspi.CombinationCut = BsCombinationCut
    
    Bs2Dspi.MotherCut = BsMotherCut

    _myDsplus = DataOnDemand(Location = "Phys/StdLooseDsplus2KKPi/Particles")
    _myPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")

    
    return Selection (name, Algorithm = Bs2Dspi, RequiredSelections = [ _myDsplus, _myPions ])

#######################################################################################################
#######################################################################################################
