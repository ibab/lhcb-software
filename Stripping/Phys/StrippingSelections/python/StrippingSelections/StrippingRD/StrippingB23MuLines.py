"""
Module for construction of B+ -> mu l+ l- lines

Performance:

Full.dst
########

StrippingReport                                                INFO Event 500000, Good event 500000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |_StrippingGlobal_                                            |  0.0288|       144|       |  15.690|
 |_StrippingSequenceStreamLeptonic_                            |  0.0288|       144|       |  15.633|
 |!StrippingB23MuB23MuLine                                     |  0.0002|         1|  1.000|   5.209|
 |!StrippingB23MuB2MueeLine                                    |  0.0002|         1|  1.000|   3.071|
 |!StrippingB23MuB23PiLine                                     |  0.0284|       142|  1.056|   2.018|
 |_StrippingSequenceStreamMDST_                                |  0.0288|       144|       |   0.050|
 |!StrippingB23MuB23MuLine                                     |  0.0002|         1|  1.000|   5.209|
 |!StrippingB23MuB2MueeLine                                    |  0.0002|         1|  1.000|   3.071|
 |!StrippingB23MuB23PiLine                                     |  0.0284|       142|  1.056|   2.018|
       
B23Pi (12103002) MC
###################
StrippingReport                                                INFO Event 100000, Good event 100000
|                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
|!StrippingB23MuLinesB23PiLine                                | 16.0890|     16089|  1.005|   4.821|
 
Exported symbols (use python help!):

"""

__author__ = ["Oliver Gruenberg"]
__date__ = "03.06.2014"
__version__ = "$Revision: 1.0 $"

#############################################################################

__all__ = ("B23MuLinesConf",
           "default_config", 
           "makeB23Mu",
           "makeB2Muee",
           "makeB2DMu",
           "makeB23Pi", )

#############################################################################

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, DaVinci__N3BodyDecays
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StrippingSelections.Utils import checkConfig
from GaudiKernel.PhysicalConstants import c_light

#############################################################################

default_config = {
    "NAME"        : "B23Mu",
    "WGs"         : [ "RD" ],
    "STREAMS"     : [ "Leptonic" ],
    "BUILDERTYPE" : "B23MuLinesConf",
    "CONFIG"      : {
    "Postscale"            :1, 
    "B23MuPrescale"        :1,
    "B2MueePrescale"       :1,
    "B2DMuPrescale"        :1,
    "B23PiPrescale"        :1,
    "CommonRelInfoTools"   : [ { "Type": "RelInfoVertexIsolation", "Location":"VtxIsoInfo" },
                               { "Type": "RelInfoVertexIsolationBDT", "Location":"VtxIsoInfoBDT" },
                               { "Type" : "RelInfoBs2MuMuBIsolations",
                                 "RecursionLevel" : 0,
                                 "Variables" : [],
                                 "Location"  : "BsMuMuBIsolation",
                                 "tracktype" : 3,
                                 "makeTrackCuts" : False, },
                               ] # closes CommonRelInfoTools
    } # closes CONFIG
    } # closes default_config

class B23MuLinesConf(LineBuilder) :
    """
    Builder
    
    """

    __configuration_keys__ = ( "Postscale",
                               "B23MuPrescale",
                               "B2MueePrescale",
                               "B2DMuPrescale",
                               "B23PiPrescale",
                               "CommonRelInfoTools", )
    
    def __init__(self, name = "B23Mu", config = None) :

        LineBuilder.__init__(self, name, config)
        
        B23Mu_name=name+"B23Mu"
        B2Muee_name=name+"B2Muee"
        B2DMu_name=name+"B2DMu"
        B23Pi_name=name+"B23Pi"        

        self.selB23Mu = makeB23Mu(B23Mu_name)
        self.selB2Muee = makeB2Muee(B2Muee_name)
        self.selB2DMu = makeB2DMu(B2DMu_name)
        self.selB23Pi = makeB23Pi(B23Pi_name)        
        
#############################################################################

        self.B23Mu_Line = StrippingLine(B23Mu_name+"Line",
                                        prescale = config["B23MuPrescale"],
                                        postscale = config["Postscale"],
                                        MDSTFlag = True,
                                        selection = self.selB23Mu,
                                        RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB23Mu : "ConeIso05Bp",
                                                              "Phys/StdAllLooseMuons" :
                                                              ["ConeIso05mu1","ConeIso05mu2","ConeIso05mu3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB23Mu : "ConeIso10Bp",
                                                              "Phys/StdAllLooseMuons" :
                                                              ["ConeIso10mu1","ConeIso10mu2","ConeIso10mu3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB23Mu : "ConeIso15Bp",
                                                              "Phys/StdAllLooseMuons" :
                                                              ["ConeIso15mu1","ConeIso15mu2","ConeIso15mu3"], }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 1,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseMuons" :
                                                            ["TrackIsoBDTmu1","TrackIsoBDTmu2","TrackIsoBDTmu3"], }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 1,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseMuons" :
                                                             ["BsMuMuTrackIsomu1","BsMuMuTrackIsomu2","BsMuMuTrackIsomu3"], }, }, 
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                        )# closes Strippingline

        self.B2Muee_Line = StrippingLine(B2Muee_name+"Line",
                                         prescale = config["B2MueePrescale"],
                                         postscale = config["Postscale"],
                                         MDSTFlag = True,
                                         selection = self.selB2Muee,
                                         RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB2Muee : "ConeIso05Bp",
                                                              "Phys/StdAllLooseMuons" : "ConeIso05mu",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso05e1","ConeIso05e2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB2Muee : "ConeIso10Bp",
                                                              "Phys/StdAllLooseMuons" : "ConeIso10mu",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso10e1","ConeIso10e2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB2Muee : "ConeIso15Bp",
                                                              "Phys/StdAllLooseMuons" : "ConeIso15mu",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso15e1","ConeIso15e2"], }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 1,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseMuons" : "TrackIsoBDTmu",
                                                            "Phys/StdAllLooseElectrons" : ["TrackIsoBDTe1","TrackIsoBDTe2"], }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 1,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseMuons" : "BsMuMuTrackIsomu",
                                                             "Phys/StdAllLooseElectrons" :
                                                             ["BsMuMuTrackIsoe1","BsMuMuTrackIsoe2"], }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                         ) # closes Strippingline

        self.B2DMu_Line = StrippingLine(B2DMu_name+"Line",
                                        prescale = config["B2DMuPrescale"],
                                        postscale = config["Postscale"],
                                        MDSTFlag = True,
                                        selection = self.selB2DMu,
                                        RelatedInfoTools = [
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selB2DMu : "ConeIso05Bp",
                                                              "Phys/StdAllLooseMuons" :
                                                              ["ConeIso05mu1", "ConeIso05mu2", "ConeIso05mu3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selB2DMu : "ConeIso10Bp",
                                                              "Phys/StdAllLooseMuons" :
                                                              ["ConeIso10mu1", "ConeIso10mu2", "ConeIso10mu3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selB2DMu : "ConeIso15Bp",
                                                              "Phys/StdAllLooseMuons" :
                                                              ["ConeIso15mu1", "ConeIso15mu2", "ConeIso15mu3"], }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 2,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseMuons" :
                                                            ["TrackIsoBDTmu1","TrackIsoBDTmu2","TrackIsoBDTmu3"], }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 2,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseMuons" :
                                                             ["BsMuMuTrackIsomu1","BsMuMuTrackIsomu2","BsMuMuTrackIsomu3"], }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools
                                         ) # closes Strippingline

        self.B23Pi_Line = StrippingLine(B23Pi_name+"Line",
                                        prescale = config["B23PiPrescale"],
                                        postscale = config["Postscale"],
                                        MDSTFlag = True,
                                        selection = self.selB23Pi,
                                        RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB23Pi : "ConeIso05Bp",
                                                              "Phys/StdAllLoosePions" :
                                                              ["ConeIso05pi1", "ConeIso05pi2", "ConeIso05pi3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB23Pi : "ConeIso10Bp",
                                                              "Phys/StdAllLoosePions" :
                                                              ["ConeIso10pi1", "ConeIso10pi2", "ConeIso10pi3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB23Pi : "ConeIso15Bp",
                                                              "Phys/StdAllLoosePions" :
                                                              ["ConeIso15pi1", "ConeIso15pi2", "ConeIso15pi3"], }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 1,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLoosePions" :
                                                            ["TrackIsoBDTpi1","TrackIsoBDTpi2","TrackIsoBDTpi3"], }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 1,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLoosePions" :
                                                             ["BsMuMuTrackIsopi1","BsMuMuTrackIsopi2","BsMuMuTrackIsopi3"] ,}, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                        ) # closes Strippingline 

#############################################################################

        self.registerLine(self.B23Mu_Line)
        self.registerLine(self.B2Muee_Line)
#        self.registerLine(self.B2DMu_Line)
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
