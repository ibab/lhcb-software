"""
Module for construction of D+ -> mu l+ l- lines

Performance

Full.dst:
######### 
StrippingReport                                                INFO Event 500000, Good event 500000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingD23MuD23MuLine                                     |  0.0014|         7|  1.000|   0.112|
 |!StrippingD23MuD2MueeLine                                    |  0.0030|        15|  1.000|   0.131|
 |!StrippingD23MuD23PiLine                                     |  0.0130|        65|  1.354|   0.033|

     
MC: D+ -> 3pi (21163012)
########################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingD23MuD23PiLine                                     |  0.6500|       650|  1.008|   0.569| 

MC: D+ -> K 2pi (21163020)
##########################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|      
 |!StrippingD23MuD23PiLine                                     |  0.0130|        13|  1.077|   0.266|
 
Exported symbols (use python help!):
   - 
"""

__author__ = ["Oliver Gruenberg"]
__date__ = "19.05.2015"
__version__ = "$Revision: 1.0 $"

#############################################################################

__all__ = ("D23MuLinesConf",
           "config_default", )

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
    "NAME"        : "D23Mu",
    "WGs"         : [ "RD" ],
    "STREAMS"     : [ "Leptonic" ],
    "BUILDERTYPE" : "D23MuLinesConf",
    "CONFIG"      : {
    # TrackCuts
    "MinTrIPChi2"          : 25.0,
    "MaxTrChi2Dof"         : 3.0,
    "MaxTrGhp"             : 0.3,
    # CombiCuts
    "MaxDoca"              : 0.3, # (mm)
    "mDiffDLoose"          : 150, # (MeV)
    "mDiffDTight"          : 150, # (MeV)    
    # MotherCuts
    "MaxIPChi2"            : 25,
    "MinVDChi2"            : 225,
    "MaxVtxChi2Dof"        : 9,
    "MinDira"              : 0.0,
    "MinTau"               : 0.1, # (ps)
    # scalings
    "Postscale"            : 1, 
    "D23MuPrescale"        : 1,
    "D2MueePrescale"       : 1,
    "D23PiPrescale"        : 0.01,
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

class D23MuLinesConf(LineBuilder) :
    """
    Builder
    
    """

    __configuration_keys__ = ( # TrackCuts
                               "MinTrIPChi2",
                               "MaxTrChi2Dof",
                               "MaxTrGhp",
                               # CombiCuts
                               "MaxDoca",
                               "mDiffDLoose",
                               "mDiffDTight",
                               # MotherCuts
                               "MaxIPChi2",
                               "MinVDChi2",
                               "MaxVtxChi2Dof",
                               "MinDira",
                               "MinTau",
                               # scalings
                               "Postscale",
                               "D23MuPrescale",
                               "D2MueePrescale",
                               "D23PiPrescale",
                               "CommonRelInfoTools", )   
    
    def __init__(self, name = "D23Mu", config = default_config) :

        LineBuilder.__init__(self, name, config)

#############################################################################

        self.TrackCuts = """
                         (MIPCHI2DV(PRIMARY) > %(MinTrIPChi2)s)
                         & (TRCHI2DOF < %(MaxTrChi2Dof)s)
                         & (TRGHP < %(MaxTrGhp)s)
                         """ %config

        self.Combination12Cuts = "(ADOCA(1,2) < %(MaxDoca)s*mm)" %config

        self.CombinationCutsLoose = """
                                    (ADAMASS(1920*MeV) < %(mDiffDLoose)s*MeV)
                                    & (ADOCA(1,3) < %(MaxDoca)s*mm)
                                    & (ADOCA(2,3) < %(MaxDoca)s*mm)
                                    """ %config

        self.CombinationCutsTight = """
                                    (ADAMASS(1920*MeV) < %(mDiffDTight)s*MeV)
                                    & (ADOCA(1,3) < %(MaxDoca)s*mm)
                                    & (ADOCA(2,3) < %(MaxDoca)s*mm)
                                    """ %config

        self.MotherCuts = """
                          (BPVIPCHI2() < %(MaxIPChi2)s )
                          & (BPVVDCHI2 > %(MinVDChi2)s )
                          & (VFASPF(VCHI2/VDOF) < %(MaxVtxChi2Dof)s )
                          & (BPVDIRA > %(MinDira)s )
                          & (BPVLTIME() > %(MinTau)s*ps )
                          """ %config

#############################################################################
        
        D23Mu_name  = name+"D23Mu"
        D2Muee_name = name+"D2Muee"
        D23Pi_name  = name+"D23Pi"        

        self.selD23Mu  = self.makeD23Mu(D23Mu_name)
        self.selD2Muee = self.makeD2Muee(D2Muee_name)
        self.selD23Pi  = self.makeD23Pi(D23Pi_name)
        
#############################################################################

        self.D23Mu_Line = StrippingLine(D23Mu_name+"Line",
                                        prescale = config["D23MuPrescale"],
                                        postscale = config["Postscale"],
                                        MDSTFlag = True,
                                        selection = self.selD23Mu,
                                        RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selD23Mu : "ConeIso05Dp",
                                                              "Phys/StdAllLooseMuons" :
                                                              ["ConeIso05mu1", "ConeIso05mu2", "ConeIso05mu3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selD23Mu : "ConeIso10Dp",
                                                              "Phys/StdAllLooseMuons" :
                                                              ["ConeIso10mu1", "ConeIso10mu2", "ConeIso10mu3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selD23Mu : "ConeIso15Dp",
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

        self.D2Muee_Line = StrippingLine(D2Muee_name+"Line",
                                         prescale = config["D2MueePrescale"],
                                         postscale = config["Postscale"],
                                         MDSTFlag = True,
                                         selection = self.selD2Muee,
                                         RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selD2Muee : "ConeIso05Dp",
                                                              "Phys/StdAllLooseMuons" : "ConeIso05mu",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso05e1", "ConeIso05e2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selD2Muee : "ConeIso10Dp",
                                                              "Phys/StdAllLooseMuons" : "ConeIso10mu",
                                                              "Phys/StdAllLooseElectrons" : ["ConeIso10e1", "ConeIso10e2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selD2Muee : "ConeIso15Dp",
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

        self.D23Pi_Line = StrippingLine(D23Pi_name+"Line",
                                        prescale = config["D23PiPrescale"],
                                        postscale = config["Postscale"],
                                        MDSTFlag = True,
                                        selection = self.selD23Pi,
                                        RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selD23Pi : "ConeIso05Dp",
                                                              "Phys/StdAllLoosePions" :
                                                              ["ConeIso05pi1", "ConeIso05pi2", "ConeIso05pi3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selD23Pi : "ConeIso10Dp",
                                                              "Phys/StdAllLoosePions" :
                                                              ["ConeIso10pi1", "ConeIso10pi2", "ConeIso10pi3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selD23Pi : "ConeIso15Dp",
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

        self.registerLine(self.D23Mu_Line)
        self.registerLine(self.D2Muee_Line)
        self.registerLine(self.D23Pi_Line)        
       
#############################################################################

    def makeD23Mu(self,name):
    
        D23Mu = DaVinci__N3BodyDecays("Combine"+name)
        D23Mu.DecayDescriptors = [ "[D+ -> mu+ mu+ mu-]cc","[D+ -> mu+ mu+ mu+]cc" ]
        D23Mu.DaughtersCuts = { "mu+" : self.TrackCuts }
        
        D23Mu.Combination12Cut = self.Combination12Cuts
        D23Mu.CombinationCut   = self.CombinationCutsLoose
        D23Mu.MotherCut        = self.MotherCuts

        _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
   
        return Selection (name, Algorithm = D23Mu, RequiredSelections = [ _myMuons ])

#############################################################################

    def makeD2Muee(self,name):
    
        D2Muee = DaVinci__N3BodyDecays("Combine"+name)
        D2Muee.DecayDescriptors = [ "[D+ -> mu+ e+ e-]cc","[D+ -> mu- e+ e+]cc","[D+ -> mu+ e+ e+]cc" ]
        D2Muee.DaughtersCuts = { "mu+" : self.TrackCuts, "e+" : self.TrackCuts }
        
        D2Muee.Combination12Cut = self.Combination12Cuts
        D2Muee.CombinationCut   = self.CombinationCutsLoose
        D2Muee.MotherCut        = self.MotherCuts

        _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
        _myElectrons = DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")
    
        return Selection (name, Algorithm = D2Muee, RequiredSelections = [ _myMuons, _myElectrons ])

#############################################################################

    def makeD23Pi(self,name):
    
        D23Pi = DaVinci__N3BodyDecays("Combine"+name)
        D23Pi.DecayDescriptors = [ "[D+ -> pi+ pi+ pi-]cc" ]
        D23Pi.DaughtersCuts = { "pi+" : self.TrackCuts }
        
        D23Pi.Combination12Cut = self.Combination12Cuts
        D23Pi.CombinationCut   = self.CombinationCutsTight
        D23Pi.MotherCut        = self.MotherCuts

        _myPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    
        return Selection (name, Algorithm = D23Pi, RequiredSelections = [ _myPions ])

#############################################################################
