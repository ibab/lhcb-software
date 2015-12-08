"""
Module for construction of D+ -> mu l+ l- lines

Performance

Full.dst:
######### 
StrippingReport                                                INFO Event 1000000, Good event 1000000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |_StrippingGlobal_                                            |  0.0142|       142|       |   8.622|
 |!StrippingD23MuD23MuLine_TIMING                              |  0.0004|         4|  1.000|   0.065|
 |!StrippingD23MuD2MueeLine_TIMING                             |  0.0029|        29|  1.103|   0.076|
 |!StrippingD23MuD23PiLine_TIMING                              |  0.0058|        58|  1.328|   0.024|

     
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
__date__ = "08.12.2015"
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
    "MDSTflag"             : False, # True or False
    # TrackCuts
    "MinTrIPChi2"          : 25.0,
    "MaxTrChi2Dof"         : 4.0,
    "MaxTrGhp"             : 0.4,
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

    __configuration_keys__ = ( "MDSTflag",
                               # TrackCuts
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
                                        MDSTFlag = config["MDSTflag"],
                                        selection = self.selD23Mu,
                                        RelatedInfoTools = [
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso05Dp',
                                             "DaughterLocations" : {
                                             "[D+ -> ^mu+  mu+  l]CC"  : "ConeIso05mu1",
                                             "[D+ ->  mu+ ^mu+  l]CC"  : "ConeIso05mu2",
                                             "[D+ ->  mu+  mu+ ^l]CC"  : "ConeIso05mu3",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso10Dp',
                                             "DaughterLocations" : {
                                             "[D+ -> ^mu+  mu+  l]CC"  : "ConeIso10mu1",
                                             "[D+ ->  mu+ ^mu+  l]CC"  : "ConeIso10mu2",
                                             "[D+ ->  mu+  mu+ ^l]CC"  : "ConeIso10mu3",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso15Dp',
                                             "DaughterLocations" : {
                                             "[D+ -> ^mu+  mu+  l]CC"  : "ConeIso15mu1",
                                             "[D+ ->  mu+ ^mu+  l]CC"  : "ConeIso15mu2",
                                             "[D+ ->  mu+  mu+ ^l]CC"  : "ConeIso15mu3",
                                             }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "Variables" : 0,
                                             "DaughterLocations" : {
                                             "[D+ -> ^mu+  mu+  l]CC"  : "TrackIsoBDTmu1",
                                             "[D+ ->  mu+ ^mu+  l]CC"  : "TrackIsoBDTmu2",
                                             "[D+ ->  mu+  mu+ ^l]CC"  : "TrackIsoBDTmu3",
                                             }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "DaughterLocations" : {
                                             "[D+ -> ^mu+  mu+  l]CC"  : "BsMuMuTrackIsomu1",
                                             "[D+ ->  mu+ ^mu+  l]CC"  : "BsMuMuTrackIsomu2",
                                             "[D+ ->  mu+  mu+ ^l]CC"  : "BsMuMuTrackIsomu3",
                                             }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                        )# closes Strippingline 

        self.D2Muee_Line = StrippingLine(D2Muee_name+"Line",
                                         prescale = config["D2MueePrescale"],
                                         postscale = config["Postscale"],
                                         MDSTFlag = config["MDSTflag"],
                                         selection = self.selD2Muee,
                                         RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso05Dp',
                                             "DaughterLocations" : {
                                             "[D+ -> ^l  l  l]CC"  : "ConeIso05mu",
                                             "[D+ ->  l ^l  l]CC"  : "ConeIso05e1",
                                             "[D+ ->  l  l ^l]CC"  : "ConeIso05e2",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso10Dp',
                                             "DaughterLocations" : {
                                             "[D+ -> ^l  l  l]CC"  : "ConeIso10mu",
                                             "[D+ ->  l ^l  l]CC"  : "ConeIso10e1",
                                             "[D+ ->  l  l ^l]CC"  : "ConeIso10e2",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso15Dp',
                                             "DaughterLocations" : {
                                             "[D+ -> ^l  l  l]CC"  : "ConeIso15mu",
                                             "[D+ ->  l ^l  l]CC"  : "ConeIso15e1",
                                             "[D+ ->  l  l ^l]CC"  : "ConeIso15e2",
                                             }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "Variables" : 0,
                                             "DaughterLocations" : {
                                             "[D+ -> ^l  l  l]CC"  : "TrackIsoBDTmu",
                                             "[D+ ->  l ^l  l]CC"  : "TrackIsoBDTe1",
                                             "[D+ ->  l  l ^l]CC"  : "TrackIsoBDTe2",
                                             }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "DaughterLocations" : {
                                             "[D+ -> ^l  l  l]CC"  : "BsMuMuTrackIsomu1",
                                             "[D+ ->  l ^l  l]CC"  : "BsMuMuTrackIsomu2",
                                             "[D+ ->  l  l ^l]CC"  : "BsMuMuTrackIsomu3",
                                             }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                         ) # closes Strippingline

        self.D23Pi_Line = StrippingLine(D23Pi_name+"Line",
                                        prescale = config["D23PiPrescale"],
                                        postscale = config["Postscale"],
                                        MDSTFlag = config["MDSTflag"],
                                        selection = self.selD23Pi,
                                        RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso05Dp',
                                             "DaughterLocations" : {
                                             "[D+ -> ^pi+  pi+  pi-]CC"  : "ConeIso05pi1",
                                             "[D+ ->  pi+ ^pi+  pi-]CC"  : "ConeIso05pi2",
                                             "[D+ ->  pi+  pi+ ^pi-]CC"  : "ConeIso05pi3",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso10Dp',
                                             "DaughterLocations" : {
                                             "[D+ -> ^pi+  pi+  pi-]CC"  : "ConeIso10pi1",
                                             "[D+ ->  pi+ ^pi+  pi-]CC"  : "ConeIso10pi2",
                                             "[D+ ->  pi+  pi+ ^pi-]CC"  : "ConeIso10pi3",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso15Dp',
                                             "DaughterLocations" : {
                                             "[D+ -> ^pi+  pi+  pi-]CC"  : "ConeIso15pi1",
                                             "[D+ ->  pi+ ^pi+  pi-]CC"  : "ConeIso15pi2",
                                             "[D+ ->  pi+  pi+ ^pi-]CC"  : "ConeIso15pi3",
                                             }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "Variables" : 0,
                                             "DaughterLocations" : {
                                             "[D+ -> ^pi+  pi+  pi-]CC"  : "TrackIsoBDTpi1",
                                             "[D+ ->  pi+ ^pi+  pi-]CC"  : "TrackIsoBDTpi2",
                                             "[D+ ->  pi+  pi+ ^pi-]CC"  : "TrackIsoBDTpi3",
                                             }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "DaughterLocations" : {
                                             "[D+ -> ^pi+  pi+  pi-]CC"  : "TrackIsoBDTpi1",
                                             "[D+ ->  pi+ ^pi+  pi-]CC"  : "TrackIsoBDTpi2",
                                             "[D+ ->  pi+  pi+ ^pi-]CC"  : "TrackIsoBDTpi3",
                                             }, },
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
