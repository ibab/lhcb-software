"""
Module for construction of Lambda_c -> mu ll lines

Performance (with prescaling);

Full.dst:
#########

StrippingReport                                                INFO Event 500000, Good event 500000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |_StrippingGlobal_                                            |  0.0516|       258|       |  16.412|
 |_StrippingSequenceStreamLeptonic_                            |  0.0516|       258|       |  16.343|
 |!StrippingLc23MuLc23muLine                                   |  0.0092|        46|  1.065|   5.301|
 |!StrippingLc23MuLc2mueeLine                                  |  0.0038|        19|  1.000|   2.723|
 |!StrippingLc23MuLc2pmumuLine                                 |  0.0236|       118|  1.051|   1.461|
 |!StrippingLc23MuLc2peeLine                                   |  0.0112|        56|  1.054|   1.412|
 |!StrippingLc23MuLc2pKpiLine                                  |  0.0044|        22|  1.091|   0.060|
 |_StrippingSequenceStreamMDST_                                |  0.0516|       258|       |   0.059|
 |!StrippingLc23MuLc23muLine                                   |  0.0092|        46|  1.065|   5.301|
 |!StrippingLc23MuLc2mueeLine                                  |  0.0038|        19|  1.000|   2.723|
 |!StrippingLc23MuLc2pmumuLine                                 |  0.0236|       118|  1.051|   1.461|
 |!StrippingLc23MuLc2peeLine                                   |  0.0112|        56|  1.054|   1.412|
 |!StrippingLc23MuLc2pKpiLine                                  |  0.0044|        22|  1.091|   0.060|
              
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
"""

__author__ = ["Oliver Gruenberg"]
__date__ = "24.06.2014"
__version__ = "$Revision: 1.0 $"

###################################################################################################

__all__ = ("Lc23MuLinesConf",
           "config_default",
           "makeLc23mu",
           "makeLc2muee",
           "makeLc2pmumu",
           "makeLc2pee",
           "makeLc2pKpi",
           )

###################################################################################################

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, DaVinci__N3BodyDecays
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.PhysicalConstants import c_light

###################################################################################################

default_config = {
    "NAME"        : "Lc23Mu",
    "WGs"         : [ "RD" ],
    "STREAMS"     : [ "Leptonic" ],
    "BUILDERTYPE" : "Lc23MuLinesConf",
    "CONFIG"      : {
    "Postscale"           :1,
    "Lc23muPrescale"      :1,
    "Lc2mueePrescale"     :1,
    "Lc2pmumuPrescale"    :1,
    "Lc2peePrescale"      :1,        
    "Lc2pKpiPrescale"     :0.01,
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

class Lc23MuLinesConf(LineBuilder) :
    """
    Builder
    
    """

    __configuration_keys__ = (    "Postscale",
                                  "Lc23muPrescale",
                                  "Lc2mueePrescale",
                                  "Lc2pmumuPrescale",
                                  "Lc2peePrescale",
                                  "Lc2pKpiPrescale",
                                  "CommonRelInfoTools"
                                  )
    
    def __init__(self, name = "Lc23mu", config = None) :

        LineBuilder.__init__(self, name, config)
        
        Lc23mu_name   = name+"Lc23mu"
        Lc2muee_name  = name+"Lc2muee"
        Lc2pmumu_name = name+"Lc2pmumu"
        Lc2pee_name   = name+"Lc2pee"
        Lc2pKpi_name  = name+"Lc2pKpi"

        self.selLc23mu    = makeLc23mu(Lc23mu_name)
        self.selLc2muee   = makeLc2muee(Lc2muee_name)
        self.selLc2pmumu  = makeLc2pmumu(Lc2pmumu_name)
        self.selLc2pee    = makeLc2pee(Lc2pee_name)
        self.selLc2pKpi   = makeLc2pKpi(Lc2pKpi_name)        

#######################################################################################################        

        self.Lc23mu_Line = StrippingLine(Lc23mu_name+"Line",
                                         prescale = config["Lc23muPrescale"],
                                         postscale = config["Postscale"],
                                         MDSTFlag = True,
                                         selection = self.selLc23mu,
                                         RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc23mu : "ConeIso05Lc",
                                                              "Phys/StdAllLooseMuons" :
                                                              ["ConeIso05mu1", "ConeIso05mu2", "ConeIso05mu3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc23mu : "ConeIso10Lc",
                                                              "Phys/StdAllLooseMuons" :
                                                              ["ConeIso10mu1", "ConeIso10mu2", "ConeIso10mu3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc23mu : "ConeIso15Lc",
                                                              "Phys/StdAllLooseMuons" :
                                                              ["ConeIso15mu1", "ConeIso15mu2", "ConeIso15mu3"], }, },
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
                                                             ["BsMuMuTrackIsomu1","BsMuMuTrackIsomu2","BsMuMuTrackIsomu3"] ,}, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                        )# closes Strippingline 
                                         


        self.Lc2muee_Line = StrippingLine(Lc2muee_name+"Line",
                                          prescale = config["Lc2mueePrescale"],
                                          postscale = config["Postscale"],
                                          MDSTFlag = True,
                                          selection = self.selLc2muee,
                                          RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2muee : "ConeIso05Lc",
                                                              "Phys/StdAllLooseMuons" : "ConeIso05mu",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso05e1", "ConeIso05e2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2muee : "ConeIso10Lc",
                                                              "Phys/StdAllLooseMuons" : "ConeIso10mu",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso10e1", "ConeIso10e2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2muee : "ConeIso15Lc",
                                                              "Phys/StdAllLooseMuons" : "ConeIso15mu",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso15e1", "ConeIso15e2"], }, },
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
                                                             ["BsMuMuTrackIsoe1","BsMuMuTrackIsoe2"] ,}, }, 
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                         ) # closes Strippingline


        self.Lc2pmumu_Line = StrippingLine(Lc2pmumu_name+"Line",
                                           prescale = config["Lc2pmumuPrescale"],
                                           postscale = config["Postscale"],
                                           MDSTFlag = True,
                                           selection = self.selLc2pmumu,
                                           RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pmumu : "ConeIso05Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso05p",
                                                              "Phys/StdAllLooseMuons" : ["ConeIso05mu1", "ConeIso05mu2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pmumu : "ConeIso10Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso10p",
                                                              "Phys/StdAllLooseMuons" : ["ConeIso10mu1", "ConeIso10mu2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pmumu : "ConeIso15Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso15p",
                                                              "Phys/StdAllLooseMuons" : ["ConeIso15mu1", "ConeIso15mu2"], }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 1,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseProtons" : "TrackIsoBDTp",
                                                            "Phys/StdAllLooseMuons" : ["TrackIsoBDTmu1","TrackIsoBDTmu2"], }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 1,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseProtons" : "BsMuMuTrackIsoBDTp",
                                                             "Phys/StdAllLooseMuons" :
                                                             ["BsMuMuTrackIsomu1","BsMuMuTrackIsomu2"] ,}, }, 
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                        )# closes Strippingline 

        self.Lc2pee_Line = StrippingLine(Lc2pee_name+"Line",
                                         prescale = config["Lc2peePrescale"],
                                         postscale = config["Postscale"],
                                         MDSTFlag = True,
                                         selection = self.selLc2pee,
                                         RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pmumu : "ConeIso05Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso05p",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso05e1", "ConeIso05e2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pmumu : "ConeIso10Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso10p",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso10e1", "ConeIso10e2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pmumu : "ConeIso15Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso15p",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso15e1", "ConeIso15e2"], }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 1,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseProtons" : "TrackIsoBDTp",
                                                            "Phys/StdAllLooseElectrons" : ["TrackIsoBDTe1","TrackIsoBDTe2"], }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 1,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseProtons" : "BsMuMuTrackIsoBDTp",
                                                             "Phys/StdAllLooseElectrons" :
                                                             ["BsMuMuTrackIsoe1","BsMuMuTrackIsoe2"] ,}, }, 
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                        )# closes Strippingline                                          

        self.Lc2pKpi_Line = StrippingLine(Lc2pKpi_name+"Line",
                                          prescale = config["Lc2pKpiPrescale"],
                                          postscale = config["Postscale"],
                                          MDSTFlag = True,
                                          selection = self.selLc2pKpi,
                                          RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pmumu : "ConeIso05Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso05p",
                                                              "Phys/StdAllLooseKaons"   : "ConeIso05K",
                                                              "Phys/StdAllLoosePions"   : "ConeIso05pi", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pmumu : "ConeIso10Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso10p",
                                                              "Phys/StdAllLooseKaons"   : "ConeIso10K",
                                                              "Phys/StdAllLoosePions"   : "ConeIso10pi", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pmumu : "ConeIso15Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso15p",
                                                              "Phys/StdAllLooseKaons"   : "ConeIso15K",
                                                              "Phys/StdAllLoosePions"   : "ConeIso15pi", }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 1,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseProtons" : "TrackIsoBDTp",
                                                            "Phys/StdAllLooseKaons"   : "TrackIsoBDTK",
                                                            "Phys/StdAllLoosePions"   : "TrackIsoBDTpi", }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 1,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseProtons" : "BsMuMuTrackIsoBDTp",
                                                             "Phys/StdAllLooseKaons"   : "BsMuMuTrackIsoBDTK",
                                                             "Phys/StdAllLoosePions"   : "BsMuMuTrackIsoBDTpi", }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                        )# closes Strippingline 

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
