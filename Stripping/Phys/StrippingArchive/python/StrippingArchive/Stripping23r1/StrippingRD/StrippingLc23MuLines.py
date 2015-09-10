"""
Module for construction of Lambda_c -> mu ll lines

Performance (with prescaling);

Full.dst:
#########

StrippingReport                                                INFO Event 500000, Good event 500000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingLc23MuLc23muLine                                   |  0.0092|        46|  1.065|   0.119|
 |!StrippingLc23MuLc2mueeLine                                  |  0.0038|        19|  1.000|   0.135|
 |!StrippingLc23MuLc2pmumuLine                                 |  0.0236|       118|  1.051|   0.266|
 |!StrippingLc23MuLc2peeLine                                   |  0.0112|        56|  1.054|   1.445|
 |!StrippingLc23MuLc2pKpiLine                                  |  0.0044|        22|  1.091|   0.034|
              
MC: Lc -> 3mu (25113002)
########################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingLc23MuLc23muLine                                   |  2.0770|      2077|  1.025|   5.726|                 

MC: Lc -> p+ mu+ mu- (25113000)
###############################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingLc23MuLc2pmumuLine                                 |  1.0300|      1030|  1.042|   1.736|
     

MC: Lc -> p~- mu+ mu+ (25113001)
################################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingLc23MuLc2pmumuLine                                 |  0.9420|       942|  1.041|   1.720| 

MC: Lc -> p+ K- pi+ (25103000)
##############################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingLc23MuLc2pKpiLine                                  |  0.0080|         8|  1.000|   0.061|              

Exported symbols (use python help!):
   - 
"""

__author__ = ["Oliver Gruenberg"]
__date__ = "19.05.2015"
__version__ = "$Revision: 1.0 $"

###################################################################################################

__all__ = ( "Lc23MuLinesConf",
            "config_default", )

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
    # TrackCuts
    "MinTrPT"             : 300, # (MeV)
    "MinTrIPChi2"         : 9,
    "MaxTrChi2Dof"        : 3,
    "MaxTrGhp"            : 0.3,
    # CombinationCuts
    "MaxDoca"             : 0.3, # (mm)
    "mDiffLcLoose"        : 200, # (MeV) 
    "mDiffLcTight"        : 150, # (MeV)
    # MotherCuts
    "MaxVtxChi2"          : 15,
    "MinVD"               : 70, # (micrometer)
    "MaxIPChi2"           : 100,
    # scalings
    "Postscale"           : 1,
    "Lc23muPrescale"      : 1,
    "Lc2mueePrescale"     : 1,
    "Lc2pmumuPrescale"    : 1,
    "Lc2peePrescale"      : 1,        
    "Lc2pKpiPrescale"     : 0.01,
    "CommonRelInfoTools"  : [ { "Type": "RelInfoVertexIsolation", "Location":"VtxIsoInfo" },
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

    __configuration_keys__ = ( # TrackCuts
                               "MinTrPT",
                               "MinTrIPChi2",
                               "MaxTrChi2Dof",
                               "MaxTrGhp",
                               # CombinationCuts
                               "MaxDoca",
                               "mDiffLcLoose",
                               "mDiffLcTight",
                               # MotherCuts
                               "MaxVtxChi2",
                               "MinVD",
                               "MaxIPChi2",
                               # scalings
                               "Postscale",
                               "Lc23muPrescale",
                               "Lc2mueePrescale",
                               "Lc2pmumuPrescale",
                               "Lc2peePrescale",
                               "Lc2pKpiPrescale",
                               "CommonRelInfoTools"
                               )
    
    def __init__(self, name = "Lc23mu", config = default_config) :

        LineBuilder.__init__(self, name, config)


#######################################################################################################        

        self.TrackCuts = """
                         (PT > %(MinTrPT)s*MeV)
                         & (BPVIPCHI2() > %(MinTrIPChi2)s*MeV)
                         & (TRCHI2DOF < %(MaxTrChi2Dof)s)
                         & (TRGHP < %(MaxTrGhp)s)
                         """ %config

        self.Combination12Cuts = "(ADOCA(1,2) < %(MaxDoca)s*mm)" %config

        self.CombinationCutsLoose = """
                                 (ADAMASS('Lambda_c+') < %(mDiffLcLoose)s*MeV)
                                 & (ADOCA(1,3) < %(MaxDoca)s*mm)
                                 & (ADOCA(2,3) < %(MaxDoca)s*mm)
                                 """ %config
        

        self.CombinationCutsTight = """
                                 (ADAMASS('Lambda_c+') < %(mDiffLcTight)s*MeV)
                                 & (ADOCA(1,3) < %(MaxDoca)s*mm)
                                 & (ADOCA(2,3) < %(MaxDoca)s*mm)
                                 """ %config

        self.MotherCuts = """
                          ( VFASPF(VCHI2) < %(MaxVtxChi2)s )
                          & ( (BPVLTIME()*c_light) > %(MinVD)s*micrometer )                         
                          & ( BPVIPCHI2() < %(MaxIPChi2)s )
                          """ %config
                   
#######################################################################################################
        
        Lc23mu_name   = name+"Lc23mu"
        Lc2muee_name  = name+"Lc2muee"
        Lc2pmumu_name = name+"Lc2pmumu"
        Lc2pee_name   = name+"Lc2pee"
        Lc2pKpi_name  = name+"Lc2pKpi"

        self.selLc23mu    = self.makeLc23mu(Lc23mu_name)
        self.selLc2muee   = self.makeLc2muee(Lc2muee_name)
        self.selLc2pmumu  = self.makeLc2pmumu(Lc2pmumu_name)
        self.selLc2pee    = self.makeLc2pee(Lc2pee_name)
        self.selLc2pKpi   = self.makeLc2pKpi(Lc2pKpi_name)        

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
                                             "Locations"  : { self.selLc2pee : "ConeIso05Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso05p",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso05e1", "ConeIso05e2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pee : "ConeIso10Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso10p",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso10e1", "ConeIso10e2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pee : "ConeIso15Lc",
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
                                             "Locations"  : { self.selLc2pKpi : "ConeIso05Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso05p",
                                                              "Phys/StdAllLooseKaons"   : "ConeIso05K",
                                                              "Phys/StdAllLoosePions"   : "ConeIso05pi", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pKpi : "ConeIso10Lc",
                                                              "Phys/StdAllLooseProtons" : "ConeIso10p",
                                                              "Phys/StdAllLooseKaons"   : "ConeIso10K",
                                                              "Phys/StdAllLoosePions"   : "ConeIso10pi", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLc2pKpi : "ConeIso15Lc",
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

    def makeLc23mu(self,name):
    
        Lc23mu = DaVinci__N3BodyDecays("Combine"+name)
        Lc23mu.DecayDescriptors = [ "[Lambda_c+ -> mu+ mu+ mu-]cc","[Lambda_c+ -> mu+ mu+ mu+]cc" ]

        Lc23mu.DaughtersCuts = { "mu+" : self.TrackCuts }
    
        Lc23mu.Combination12Cut = self.Combination12Cuts
        Lc23mu.CombinationCut   = self.CombinationCutsLoose
     
        Lc23mu.MotherCut = self.MotherCuts

        _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
        return Selection (name, Algorithm = Lc23mu, RequiredSelections = [ _myMuons ])

#######################################################################################################

    def makeLc2muee(self,name):
    
        Lc2muee = DaVinci__N3BodyDecays("Combine"+name)
        Lc2muee.DecayDescriptors = [ "[Lambda_c+ -> mu+ e+ e-]cc",
                                     "[Lambda_c+ -> mu- e+ e+]cc",
                                     "[Lambda_c+ -> mu+ e+ e+]cc"]
    
        Lc2muee.DaughtersCuts = { "mu+" : self.TrackCuts,
                                  "e+"  : self.TrackCuts + " & ((PIDe-PIDpi)>2)" }
    
        Lc2muee.Combination12Cut = self.Combination12Cuts
        Lc2muee.CombinationCut   = self.CombinationCutsLoose     
        Lc2muee.MotherCut        = self.MotherCuts

        _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
        _myElectrons = DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")    
    
        return Selection (name, Algorithm = Lc2muee, RequiredSelections = [ _myMuons, _myElectrons ])


#######################################################################################################

    def makeLc2pmumu(self,name):
    
        Lc2pmumu = DaVinci__N3BodyDecays("Combine"+name)
        Lc2pmumu.DecayDescriptors = [ "[Lambda_c+ -> p+ mu+ mu-]cc",
                                      "[Lambda_c+ -> p~- mu+ mu+]cc",
                                      "[Lambda_c+ -> p+ mu+ mu+]cc"]
    
        Lc2pmumu.DaughtersCuts = { "p+"  : self.TrackCuts + " & ((PIDp-PIDpi)>5)" + " & ((PIDp-PIDK)>0)",
                                   "mu+" : self.TrackCuts + " & ((PIDmu-PIDpi)>-5)" + " & ((PIDmu-PIDK)>-5)" }
    
        Lc2pmumu.Combination12Cut = self.Combination12Cuts
        Lc2pmumu.CombinationCut   = self.CombinationCutsLoose     
        Lc2pmumu.MotherCut        = self.MotherCuts

        _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
        _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
        return Selection (name, Algorithm = Lc2pmumu, RequiredSelections = [ _myProtons, _myMuons ])

#######################################################################################################

    def makeLc2pee(self,name):
    
        Lc2pee = DaVinci__N3BodyDecays("Combine"+name)
        Lc2pee.DecayDescriptors = [ "[Lambda_c+ -> p+ e+ e-]cc",
                                "[Lambda_c+ -> p~- e+ e+]cc",
                                "[Lambda_c+ -> p+ e+ e+]cc"]
    
        Lc2pee.DaughtersCuts = { "p+" : self.TrackCuts + " & ((PIDp-PIDpi)>5)" + " & ((PIDp-PIDK)>0)",
                                 "e+" : self.TrackCuts + " & ((PIDe-PIDpi)>2)" }
    
        Lc2pee.Combination12Cut = self.Combination12Cuts
        Lc2pee.CombinationCut   = self.CombinationCutsLoose
        Lc2pee.MotherCut        = self.MotherCuts

        _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
        _myElectrons = DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")    
    
        return Selection (name, Algorithm = Lc2pee, RequiredSelections = [ _myProtons, _myElectrons ])

#######################################################################################################

    def makeLc2pKpi(self,name):
    
        Lc2pKpi = DaVinci__N3BodyDecays("Combine"+name)
        Lc2pKpi.DecayDescriptors = [ "[Lambda_c+ -> p+ K- pi+]cc" ]
    
        Lc2pKpi.DaughtersCuts = { "p+"  : self.TrackCuts + " & ((PIDp-PIDpi)>5)" + " & ((PIDp-PIDK)>0)",
                                  "K-"  : self.TrackCuts + " & ((PIDK-PIDpi)>5)" + " & ((PIDK-PIDp)>0)",
                                  "pi+" : self.TrackCuts }
    
        Lc2pKpi.Combination12Cut = self.Combination12Cuts
        Lc2pKpi.CombinationCut   = self.CombinationCutsTight
        Lc2pKpi.MotherCut        = self.MotherCuts

        _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
        _myKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")
        _myPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    
        return Selection (name, Algorithm = Lc2pKpi, RequiredSelections = [ _myProtons, _myKaons, _myPions ])

#######################################################################################################
