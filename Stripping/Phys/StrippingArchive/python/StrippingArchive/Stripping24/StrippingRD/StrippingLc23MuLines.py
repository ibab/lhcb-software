"""
Module for construction of Lambda_c -> mu ll lines

Performance (with prescaling);

Full.dst:
#########

StrippingReport                                                INFO Event 1000000, Good event 1000000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |_StrippingGlobal_                                            |  0.0242|       242|       |   9.439|
 |!StrippingLc23MuLc23muLine_TIMING                            |  0.0056|        56|  1.071|   0.056|
 |!StrippingLc23MuLc2mueeLine_TIMING                           |  0.0041|        41|  1.073|   0.067|
 |!StrippingLc23MuLc2pmumuLine_TIMING                          |  0.0036|        36|  1.028|   0.102|
 |!StrippingLc23MuLc2peeLine_TIMING                            |  0.0075|        75|  1.027|   0.109|
 |!StrippingLc23MuLc2pKpiLine_TIMING                           |  0.0019|        19|  1.158|   0.025|
              
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
    "MaxTrChi2Dof"        : 4.0,
    "MaxTrGhp"            : 0.4,
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
                                             "Location"  : 'ConeIso05Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^mu+  mu+  l]CC"  : "ConeIso05mu1",
                                             "[Lambda_c+ ->  mu+ ^mu+  l]CC"  : "ConeIso05mu2",
                                             "[Lambda_c+ ->  mu+  mu+ ^l]CC"  : "ConeIso05mu3",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso10Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^mu+  mu+  l]CC"  : "ConeIso10mu1",
                                             "[Lambda_c+ ->  mu+ ^mu+  l]CC"  : "ConeIso10mu2",
                                             "[Lambda_c+ ->  mu+  mu+ ^l]CC"  : "ConeIso10mu3",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso15Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^mu+  mu+  l]CC"  : "ConeIso15mu1",
                                             "[Lambda_c+ ->  mu+ ^mu+  l]CC"  : "ConeIso15mu2",
                                             "[Lambda_c+ ->  mu+  mu+ ^l]CC"  : "ConeIso15mu3",
                                             }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "Variables" : 0,
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^mu+  mu+  l]CC"  : "TrackIsoBDTmu1",
                                             "[Lambda_c+ ->  mu+ ^mu+  l]CC"  : "TrackIsoBDTmu2",
                                             "[Lambda_c+ ->  mu+  mu+ ^l]CC"  : "TrackIsoBDTmu3",
                                             }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^mu+  mu+  l]CC"  : "BsMuMuTrackIsomu1",
                                             "[Lambda_c+ ->  mu+ ^mu+  l]CC"  : "BsMuMuTrackIsomu2",
                                             "[Lambda_c+ ->  mu+  mu+ ^l]CC"  : "BsMuMuTrackIsomu3",
                                             }, },
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
                                             "Location"  : 'ConeIso05Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^l  l  l]CC"  : "ConeIso05mu",
                                             "[Lambda_c+ ->  l ^l  l]CC"  : "ConeIso05e1",
                                             "[Lambda_c+ ->  l  l ^l]CC"  : "ConeIso05e2",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso10Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^l  l  l]CC"  : "ConeIso10mu",
                                             "[Lambda_c+ ->  l ^l  l]CC"  : "ConeIso10e1",
                                             "[Lambda_c+ ->  l  l ^l]CC"  : "ConeIso10e2",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso15Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^l  l  l]CC"  : "ConeIso15mu",
                                             "[Lambda_c+ ->  l ^l  l]CC"  : "ConeIso15e1",
                                             "[Lambda_c+ ->  l  l ^l]CC"  : "ConeIso15e2",
                                             }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "Variables" : 0,
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^l  l  l]CC"  : "TrackIsoBDTmu",
                                             "[Lambda_c+ ->  l ^l  l]CC"  : "TrackIsoBDTe1",
                                             "[Lambda_c+ ->  l  l ^l]CC"  : "TrackIsoBDTe2",
                                             }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^l  l  l]CC"  : "BsMuMuTrackIsomu",
                                             "[Lambda_c+ ->  l ^l  l]CC"  : "BsMuMuTrackIsoe1",
                                             "[Lambda_c+ ->  l  l ^l]CC"  : "BsMuMuTrackIsoe2",
                                             }, },
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
                                             "Location"  : 'ConeIso05Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^X  l  l]CC"  : "ConeIso05p",
                                             "[Lambda_c+ ->  X ^l  l]CC"  : "ConeIso05mu1",
                                             "[Lambda_c+ ->  X  l ^l]CC"  : "ConeIso05mu2",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso10Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^X  l  l]CC"  : "ConeIso10p",
                                             "[Lambda_c+ ->  X ^l  l]CC"  : "ConeIso10mu1",
                                             "[Lambda_c+ ->  X  l ^l]CC"  : "ConeIso10mu2",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso15Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^X  l  l]CC"  : "ConeIso15p",
                                             "[Lambda_c+ ->  X ^l  l]CC"  : "ConeIso15mu1",
                                             "[Lambda_c+ ->  X  l ^l]CC"  : "ConeIso15mu2",
                                             }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "Variables" : 0,
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^X  l  l]CC"  : "TrackIsoBDTp",
                                             "[Lambda_c+ ->  X ^l  l]CC"  : "TrackIsoBDTmu1",
                                             "[Lambda_c+ ->  X  l ^l]CC"  : "TrackIsoBDTmu2",
                                             }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^X  l  l]CC"  : "BsMuMuTrackIsop",
                                             "[Lambda_c+ ->  X ^l  l]CC"  : "BsMuMuTrackIsomu1",
                                             "[Lambda_c+ ->  X  l ^l]CC"  : "BsMuMuTrackIsomu2",
                                             }, },
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
                                             "Location"  : 'ConeIso05Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^X  l  l]CC"  : "ConeIso05p",
                                             "[Lambda_c+ ->  X ^l  l]CC"  : "ConeIso05e1",
                                             "[Lambda_c+ ->  X  l ^l]CC"  : "ConeIso05e2",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso10Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^X  l  l]CC"  : "ConeIso10p",
                                             "[Lambda_c+ ->  X ^l  l]CC"  : "ConeIso10e1",
                                             "[Lambda_c+ ->  X  l ^l]CC"  : "ConeIso10e2",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso15Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^X  l  l]CC"  : "ConeIso15p",
                                             "[Lambda_c+ ->  X ^l  l]CC"  : "ConeIso15e1",
                                             "[Lambda_c+ ->  X  l ^l]CC"  : "ConeIso15e2",
                                             }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "Variables" : 0,
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^X  l  l]CC"  : "TrackIsoBDTp",
                                             "[Lambda_c+ ->  X ^l  l]CC"  : "TrackIsoBDTe1",
                                             "[Lambda_c+ ->  X  l ^l]CC"  : "TrackIsoBDTe2",
                                             }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^X  l  l]CC"  : "BsMuMuTrackIsop",
                                             "[Lambda_c+ ->  X ^l  l]CC"  : "BsMuMuTrackIsoe1",
                                             "[Lambda_c+ ->  X  l ^l]CC"  : "BsMuMuTrackIsoe2",
                                             }, },
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
                                             "Location"  : 'ConeIso05Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^p+  K-  pi+]CC"  : "ConeIso05p",
                                             "[Lambda_c+ ->  p+ ^K-  pi+]CC"  : "ConeIso05K",
                                             "[Lambda_c+ ->  p+  K- ^pi+]CC"  : "ConeIso05pi",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso10Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^p+  K-  pi+]CC"  : "ConeIso10p",
                                             "[Lambda_c+ ->  p+ ^K-  pi+]CC"  : "ConeIso10K",
                                             "[Lambda_c+ ->  p+  K- ^pi+]CC"  : "ConeIso10pi",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso15Lc',
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^p+  K-  pi+]CC"  : "ConeIso15p",
                                             "[Lambda_c+ ->  p+ ^K-  pi+]CC"  : "ConeIso15K",
                                             "[Lambda_c+ ->  p+  K- ^pi+]CC"  : "ConeIso15pi",
                                             }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "Variables" : 0,
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^p+  K-  pi+]CC"  : "TrackIsoBDTp",
                                             "[Lambda_c+ ->  p+ ^K-  pi+]CC"  : "TrackIsoBDTK",
                                             "[Lambda_c+ ->  p+  K- ^pi+]CC"  : "TrackIsoBDTpi",
                                             }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "DaughterLocations" : {
                                             "[Lambda_c+ -> ^p+  K-  pi+]CC"  : "BsMuMuTrackIsop",
                                             "[Lambda_c+ ->  p+ ^K-  pi+]CC"  : "BsMuMuTrackIsoK",
                                             "[Lambda_c+ ->  p+  K- ^pi+]CC"  : "BsMuMuTrackIsopi",
                                             }, },
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
