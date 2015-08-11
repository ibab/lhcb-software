"""
Module for construction of B+ -> mu l+ l- lines

Performance:

Full.dst
########
StrippingReport                                                INFO Event 500000, Good event 500000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB23MuB23MuLine                                     |  0.0002|         1|  1.000|   0.123|
 |!StrippingB23MuB2MueeLine                                    |  0.0002|         1|  1.000|   2.913|
 |!StrippingB23MuB23PiLine                                     |  0.0284|       142|  1.056|   0.351|  
       
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
    "MaxTrChi2Dof"         : 3.0,
    "MaxTrGhp"             : 0.3,
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
    "B2DMuPrescale"        : 1,
    "B23PiPrescale"        : 1,
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
                               "B2DMuPrescale",
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
        #B2DMu_name  = name+"B2DMu"        

        self.selB23Mu  = self.makeB23Mu(B23Mu_name)
        self.selB2Muee = self.makeB2Muee(B2Muee_name)
        self.selB23Pi  = self.makeB23Pi(B23Pi_name)
        #self.selB2DMu  = self.makeB2DMu(B2DMu_name)
        
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

#        self.B2DMu_Line = StrippingLine(B2DMu_name+"Line",
#                                        prescale = config["B2DMuPrescale"],
#                                        postscale = config["Postscale"],
#                                        MDSTFlag = True,
#                                        selection = self.selB2DMu,
#                                        RelatedInfoTools = [
#                                           { "Type" : "RelInfoConeVariables",
#                                             "ConeAngle" : 0.5,
#                                             "Variables" : [],
#                                             "RecursionLevel" : 2,
#                                             "Locations"  : { self.selB2DMu : "ConeIso05Bp",
#                                                              "Phys/StdAllLooseMuons" :
#                                                              ["ConeIso05mu1", "ConeIso05mu2", "ConeIso05mu3"], }, },
#                                           { "Type" : "RelInfoConeVariables",
#                                             "ConeAngle" : 1.0,
#                                             "Variables" : [],
#                                             "RecursionLevel" : 2,
#                                             "Locations"  : { self.selB2DMu : "ConeIso10Bp",
#                                                              "Phys/StdAllLooseMuons" :
#                                                              ["ConeIso10mu1", "ConeIso10mu2", "ConeIso10mu3"], }, },
#                                           { "Type" : "RelInfoConeVariables",
#                                             "ConeAngle" : 1.5,
#                                             "Variables" : [],
#                                             "RecursionLevel" : 2,
#                                             "Locations"  : { self.selB2DMu : "ConeIso15Bp",
#                                                              "Phys/StdAllLooseMuons" :
#                                                              ["ConeIso15mu1", "ConeIso15mu2", "ConeIso15mu3"], }, },
#                                           { "Type": "RelInfoTrackIsolationBDT",
#                                             "RecursionLevel" : 2,
#                                             "Variables" : 0,
#                                             "Locations": { "Phys/StdAllLooseMuons" :
#                                                            ["TrackIsoBDTmu1","TrackIsoBDTmu2","TrackIsoBDTmu3"], }, },
#                                          { "Type" : "RelInfoBs2MuMuTrackIsolations",
#                                             "RecursionLevel" : 2,
#                                             "Variables" : [],
#                                             "IsoTwoBody" : True,
#                                             "Locations" : { "Phys/StdAllLooseMuons" :
#                                                             ["BsMuMuTrackIsomu1","BsMuMuTrackIsomu2","BsMuMuTrackIsomu3"], }, },
#                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools
#                                         ) # closes Strippingline
                                                             
#############################################################################

        self.registerLine(self.B23Mu_Line)
        self.registerLine(self.B2Muee_Line)
        self.registerLine(self.B23Pi_Line)
#        self.registerLine(self.B2DMu_Line)        

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
#
#        def makeB2DMu(self,name):
#
#            DTrackCuts = "(MIPCHI2DV()>1.0) & (TRCHI2DOF<5.0) & (PT>1.0*GeV)"
#            DCombinationCut = "(ADAMASS('D0') < 110*MeV)"
#
#            DMotherCut = """
#                         (ADMASS('D0') < 100*MeV)
#                         & (VFASPF(VCHI2)<16)
#                         & (BPVLTIME(16) > 0.1*ps)
#                         """
#
#
#            BDTrackCuts = """
#                          (MIPCHI2DV(PRIMARY)>25.0)
#                          & (TRCHI2DOF<3.0)
#                          & (TRGHP<0.3)
#                          """
#
#            #BDCombinationCut = "(ADAMASS('B+') < 2000*MeV)"\
#
#            BDMotherCut = """
#                          (BPVVDCHI2 > 25)
#                          & (VFASPF(VCHI2/VDOF)<9)
#                          & (BPVDIRA > 0.0)
#                          """
#
#
#            D2MuMu = CombineParticles("MyD2MuMuCombination")
#            D2MuMu.DecayDescriptors = [ "[D0 -> mu+ mu-]cc","[D0 -> mu+ mu+]cc" ]
#            D2MuMu.DaughtersCuts = { "mu+" : DTrackCuts }
#            D2MuMu.CombinationCut = DCombinationCut
#            D2MuMu.MotherCut = DMotherCut
#
#            _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
#    
#            D2MuMuSelection = Selection ("MyD2MuMuSelection", Algorithm = D2MuMu, RequiredSelections = [ _myMuons ])
#
#            B2DMu = CombineParticles("Combine"+name)
#            B2DMu.DecayDescriptors = [ "[B+ -> D0 mu+]cc" ]
#            B2DMu.DaughtersCuts = { "mu+" : self.TrackCuts }
#            #B2DMu.CombinationCut = BDCombinationCut
#            B2DMu.MotherCut = BDMotherCut
#
#            return Selection (name, Algorithm = B2DMu, RequiredSelections = [ D2MuMuSelection, _myMuons ])
#
#############################################################################
