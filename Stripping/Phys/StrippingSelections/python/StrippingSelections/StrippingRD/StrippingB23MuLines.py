"""
Module for construction of B+ -> mu l+ l- lines

Performance:

Full.dst
########
StrippingReport                                                INFO Event 1000000, Good event 1000000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |_StrippingGlobal_                                            |  0.0088|        88|       |  10.028|
 |!StrippingB23MuB23MuLine_TIMING                              |  0.0001|         1|  1.000|   0.068|
 |!StrippingB23MuB2MueeLine_TIMING                             |  0.0005|         5|  1.000|   0.079|
 |!StrippingB23MuB23PiLine_TIMING                              |  0.0103|       103|  1.097|   0.151|
       
MC: B23Pi (12103002)
####################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB23MuB23PiLine                                     | 16.0900|     16090|  1.005|   7.941|

MC: B2KPiPi (12103022)
######################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB23MuB23PiLine                                     | 10.8070|     10807|  1.003|   5.839|

MC: B23K (12103012)
###################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB23MuB23PiLine                                     |  0.1140|       114|  1.026|   1.742|
 
Exported symbols (use python help!):

"""

__author__ = ["Oliver Gruenberg"]
__date__ = "19.05.2015"
__version__ = "$Revision: 1.0 $"

#############################################################################

__all__ = ("B23MuLinesConf",
           "default_config", )

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
    # TrackCuts
    "MinTrIPChi2"          : 25.0,
    "MaxTrChi2Dof"         : 4.0,
    "MaxTrGhp"             : 0.4,
    # CombiCuts
    "MaxDoca"              : 0.3, # (mm)
    "mDiffBuLoose"         : 500, # (MeV)
    "mDiffBuTight"         : 100, # (MeV)
    # MotherCuts
    "MaxIPChi2"            : 25,
    "MinVDChi2"            : 225,
    "MaxVtxChi2Dof"        : 9,
    "MinDira"              : 0.0,
    "MinTau"               : 0.0, # (ps)
    # scalings    
    "Postscale"            : 1,
    "B23MuPrescale"        : 1,
    "B2MueePrescale"       : 1,
    "B23PiPrescale"        : 1,
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

class B23MuLinesConf(LineBuilder) :
    """
    Builder
    
    """

    __configuration_keys__ = ( # TrackCuts
                               "MinTrIPChi2",
                               "MaxTrChi2Dof",
                               "MaxTrGhp",
                               # CombiCuts
                               "MaxDoca",
                               "mDiffBuLoose",
                               "mDiffBuTight",
                               # MotherCuts
                               "MaxIPChi2",
                               "MinVDChi2",
                               "MaxVtxChi2Dof",
                               "MinDira",
                               "MinTau",
                               # scalings                                
                               "Postscale",
                               "B23MuPrescale",
                               "B2MueePrescale",
                               "B23PiPrescale",
                               "CommonRelInfoTools", )
    
    def __init__(self, name = "B23Mu", config = default_config) :

        LineBuilder.__init__(self, name, config)

#############################################################################

        self.TrackCuts = """
                         ( MIPCHI2DV(PRIMARY) > %(MinTrIPChi2)s )
                         & ( TRCHI2DOF < %(MaxTrChi2Dof)s )
                         & ( TRGHP < %(MaxTrGhp)s )
                         """ %config

        self.Combination12Cuts = "(ADOCA(1,2) < %(MaxDoca)s*mm)" %config

        self.CombinationCutsLoose = """
                                    (ADAMASS('B+') < %(mDiffBuLoose)s*MeV)
                                    & (ADOCA(1,3) < %(MaxDoca)s*mm)
                                    & (ADOCA(2,3) < %(MaxDoca)s*mm)
                                    """ %config
        
        self.CombinationCutsTight = """
                                    (ADAMASS('B+') < %(mDiffBuTight)s*MeV)
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
        
        B23Mu_name  = name+"B23Mu"
        B2Muee_name = name+"B2Muee"
        B23Pi_name  = name+"B23Pi"

        self.selB23Mu  = self.makeB23Mu(B23Mu_name)
        self.selB2Muee = self.makeB2Muee(B2Muee_name)
        self.selB23Pi  = self.makeB23Pi(B23Pi_name)
        
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
                                             "Location"  : 'ConeIso05Bp',
                                             "DaughterLocations" : {
                                             "[B+ -> ^mu+  mu+  l]CC"  : "ConeIso05mu1",
                                             "[B+ ->  mu+ ^mu+  l]CC"  : "ConeIso05mu2",
                                             "[B+ ->  mu+  mu+ ^l]CC"  : "ConeIso05mu3",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso10Bp',
                                             "DaughterLocations" : {
                                             "[B+ -> ^mu+  mu+  l]CC"  : "ConeIso10mu1",
                                             "[B+ ->  mu+ ^mu+  l]CC"  : "ConeIso10mu2",
                                             "[B+ ->  mu+  mu+ ^l]CC"  : "ConeIso10mu3",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso15Bp',
                                             "DaughterLocations" : {
                                             "[B+ -> ^mu+  mu+  l]CC"  : "ConeIso15mu1",
                                             "[B+ ->  mu+ ^mu+  l]CC"  : "ConeIso15mu2",
                                             "[B+ ->  mu+  mu+ ^l]CC"  : "ConeIso15mu3",
                                             }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "Variables" : 0,
                                             "DaughterLocations" : {
                                             "[B+ -> ^mu+  mu+  l]CC"  : "TrackIsoBDTmu1",
                                             "[B+ ->  mu+ ^mu+  l]CC"  : "TrackIsoBDTmu2",
                                             "[B+ ->  mu+  mu+ ^l]CC"  : "TrackIsoBDTmu3",
                                             }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "DaughterLocations" : {
                                             "[B+ -> ^mu+  mu+  l]CC"  : "BsMuMuTrackIsomu1",
                                             "[B+ ->  mu+ ^mu+  l]CC"  : "BsMuMuTrackIsomu2",
                                             "[B+ ->  mu+  mu+ ^l]CC"  : "BsMuMuTrackIsomu3",
                                             }, },
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
                                             "Location"  : 'ConeIso05Bp',
                                             "DaughterLocations" : {
                                             "[B+ -> ^l  l  l]CC"  : "ConeIso05mu",
                                             "[B+ ->  l ^l  l]CC"  : "ConeIso05e1",
                                             "[B+ ->  l  l ^l]CC"  : "ConeIso05e2",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso10Bp',
                                             "DaughterLocations" : {
                                             "[B+ -> ^l  l  l]CC"  : "ConeIso10mu",
                                             "[B+ ->  l ^l  l]CC"  : "ConeIso10e1",
                                             "[B+ ->  l  l ^l]CC"  : "ConeIso10e2",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso15Bp',
                                             "DaughterLocations" : {
                                             "[B+ -> ^l  l  l]CC"  : "ConeIso15mu",
                                             "[B+ ->  l ^l  l]CC"  : "ConeIso15e1",
                                             "[B+ ->  l  l ^l]CC"  : "ConeIso15e2",
                                             }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "Variables" : 0,
                                             "DaughterLocations" : {
                                             "[B+ -> ^l  l  l]CC"  : "TrackIsoBDTmu",
                                             "[B+ ->  l ^l  l]CC"  : "TrackIsoBDTe1",
                                             "[B+ ->  l  l ^l]CC"  : "TrackIsoBDTe2",
                                             }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "DaughterLocations" : {
                                             "[B+ -> ^l  l  l]CC"  : "BsMuMuTrackIsomu1",
                                             "[B+ ->  l ^l  l]CC"  : "BsMuMuTrackIsomu2",
                                             "[B+ ->  l  l ^l]CC"  : "BsMuMuTrackIsomu3",
                                             }, },
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
                                             "Location"  : 'ConeIso05Bp',
                                             "DaughterLocations" : {
                                             "[B+ -> ^pi+  pi+  pi-]CC"  : "ConeIso05pi1",
                                             "[B+ ->  pi+ ^pi+  pi-]CC"  : "ConeIso05pi2",
                                             "[B+ ->  pi+  pi+ ^pi-]CC"  : "ConeIso05pi3",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso10Bp',
                                             "DaughterLocations" : {
                                             "[B+ -> ^pi+  pi+  pi-]CC"  : "ConeIso10pi1",
                                             "[B+ ->  pi+ ^pi+  pi-]CC"  : "ConeIso10pi2",
                                             "[B+ ->  pi+  pi+ ^pi-]CC"  : "ConeIso10pi3",
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso15Bp',
                                             "DaughterLocations" : {
                                             "[B+ -> ^pi+  pi+  pi-]CC"  : "ConeIso15pi1",
                                             "[B+ ->  pi+ ^pi+  pi-]CC"  : "ConeIso15pi2",
                                             "[B+ ->  pi+  pi+ ^pi-]CC"  : "ConeIso15pi3",
                                             }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "Variables" : 0,
                                             "DaughterLocations" : {
                                             "[B+ -> ^pi+  pi+  pi-]CC"  : "TrackIsoBDTpi1",
                                             "[B+ ->  pi+ ^pi+  pi-]CC"  : "TrackIsoBDTpi2",
                                             "[B+ ->  pi+  pi+ ^pi-]CC"  : "TrackIsoBDTpi3",
                                             }, },
                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "DaughterLocations" : {
                                             "[B+ -> ^pi+  pi+  pi-]CC"  : "TrackIsoBDTpi1",
                                             "[B+ ->  pi+ ^pi+  pi-]CC"  : "TrackIsoBDTpi2",
                                             "[B+ ->  pi+  pi+ ^pi-]CC"  : "TrackIsoBDTpi3",
                                             }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                        ) # closes Strippingline 
                                                             
#############################################################################

        self.registerLine(self.B23Mu_Line)
        self.registerLine(self.B2Muee_Line)
        self.registerLine(self.B23Pi_Line)

#############################################################################

    def makeB23Mu(self,name):
    
        B23Mu = DaVinci__N3BodyDecays("Combine"+name)
        
        B23Mu.DecayDescriptors = [ "[B+ -> mu+ mu+ mu-]cc","[B+ -> mu+ mu+ mu+]cc" ]
        B23Mu.DaughtersCuts = { "mu+" : self.TrackCuts }
            
        B23Mu.Combination12Cut = self.Combination12Cuts
        B23Mu.CombinationCut   = self.CombinationCutsLoose    
        B23Mu.MotherCut        = self.MotherCuts

        _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
        return Selection (name, Algorithm = B23Mu, RequiredSelections = [ _myMuons ])

#############################################################################

    def makeB2Muee(self,name):
    
        B2Muee = DaVinci__N3BodyDecays("Combine"+name)
        B2Muee.DecayDescriptors = [ "[B+ -> mu+ e+ e-]cc","[B+ -> mu- e+ e+]cc","[B+ -> mu+ e+ e+]cc" ]
        B2Muee.DaughtersCuts = { "mu+" : self.TrackCuts, "e+" : self.TrackCuts }
        
        B2Muee.Combination12Cut = self.Combination12Cuts
        B2Muee.CombinationCut   = self.CombinationCutsLoose     
        B2Muee.MotherCut        = self.MotherCuts

        _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
        _myElectrons = DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")
        
        return Selection (name, Algorithm = B2Muee, RequiredSelections = [ _myMuons, _myElectrons ])

#############################################################################

    def makeB23Pi(self,name):
    
        B23Pi = DaVinci__N3BodyDecays("Combine"+name)
        B23Pi.DecayDescriptors = [ "[B+ -> pi+ pi+ pi-]cc" ]
        B23Pi.DaughtersCuts = { "pi+" : self.TrackCuts }
        
        B23Pi.Combination12Cut = self.Combination12Cuts
        B23Pi.CombinationCut   = self.CombinationCutsTight
        B23Pi.MotherCut        = self.MotherCuts
        
        _myPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    
        return Selection (name, Algorithm = B23Pi, RequiredSelections = [ _myPions ])

#############################################################################
