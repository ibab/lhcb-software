"""
Module for construction of baryon-lepton number violating stripping selections and lines

Performance (prescaling included):

Full.dst
########

StrippingReport                                                INFO Event 500000, Good event 500000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |_StrippingGlobal_                                            |  0.1154|       577|       |  39.341|
 |_StrippingSequenceStreamBhadron_                             |  0.1154|       577|       |  39.287|
 |!StrippingBLVLa2KmuLine                                      |  0.0270|       135|  1.007|   6.164|
 |!StrippingBLVLb2KmuLine                                      |  0.0086|        43|  1.000|   0.239|
 |!StrippingBLVLb2DmuLine                                      |  0.0110|        55|  1.291|   9.192|
 |!StrippingBLVLb2DsmuLine                                     |  0.0114|        57|  1.456|  11.127|
 |!StrippingBLVB2LcmuLine                                      |  0.0230|       115|  1.400|   7.206|
 |!StrippingBLVLb2LcpiLine                                     |  0.0070|        35|  1.286|   0.088|
 |!StrippingBLVB2DpiLine                                       |  0.0052|        26|  1.346|   0.038|
 |!StrippingBLVBs2DspiLine                                     |  0.0074|        37|  1.432|   0.039|
 |!StrippingBLVB2LcpLine                                       |  0.0290|       145|  1.697|   0.087|
 |_StrippingSequenceStreamMDST_                                |  0.1154|       577|       |   0.047|
 |!StrippingBLVLa2KmuLine                                      |  0.0270|       135|  1.007|   6.164|
 |!StrippingBLVLb2KmuLine                                      |  0.0086|        43|  1.000|   0.239|
 |!StrippingBLVLb2DmuLine                                      |  0.0110|        55|  1.291|   9.192|
 |!StrippingBLVLb2DsmuLine                                     |  0.0114|        57|  1.456|  11.127|
 |!StrippingBLVB2LcmuLine                                      |  0.0230|       115|  1.400|   7.206|
 |!StrippingBLVLb2LcpiLine                                     |  0.0070|        35|  1.286|   0.088|
 |!StrippingBLVB2DpiLine                                       |  0.0052|        26|  1.346|   0.038|
 |!StrippingBLVBs2DspiLine                                     |  0.0074|        37|  1.432|   0.039|
 |!StrippingBLVB2LcpLine                                       |  0.0290|       145|  1.697|   0.087| 

Exported symbols (use python help!):
   - 
"""

__author__ = ["Oliver Gruenberg"]
__date__ = "17.04.2015"
__version__ = "$Revision: 1.0 $"

###################################################################################################

__all__ = ("BLVLinesConf",
           "default_config",
           ### Signal channels
           "makeLa2Kmu",
           "makeLb2Kmu",
           "makeLb2Dmu",
           "makeLb2Dsmu",
           "makeB2Lcmu",
           ### Control channels
           "makeLb2Lcpi",
           "makeB2Dpi",
           "makeBs2Dspi",
           "makeB2Lcp",
           )

###################################################################################################

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

###################################################################################################

default_config = {
    "NAME"        : "BLV",
    "WGs"         : [ "RD" ],
    "STREAMS"     : [ "Bhadron" ],
    "BUILDERTYPE" : "BLVLinesConf",
    "CONFIG"      : {
           "Postscale"           :1,
           ### Signal channels
           "La2KmuPrescale"      :1,
           "Lb2KmuPrescale"      :1,
           "Lb2DmuPrescale"      :1, 
           "Lb2DsmuPrescale"     :1,     
           "B2LcmuPrescale"      :1,
           ### Control channels
           "Lb2LcpiPrescale"     :0.1,
           "B2DpiPrescale"       :0.1,
           "Bs2DspiPrescale"     :0.1,
           "B2LcpPrescale"       :0.2,
           "CommonRelInfoTools"   : [ { "Type": "RelInfoVertexIsolation", "Location":"VtxIsoInfo" },
                                      { "Type": "RelInfoVertexIsolationBDT", "Location":"VtxIsoInfoBDT" },
                                      { "Type" : "RelInfoBs2MuMuBIsolations",
                                        "RecursionLevel" : 0,
                                        "Variables" : [],
                                        "Location"  : "BsMuMuBIsolation",
                                        "tracktype" : 3,
                                        "makeTrackCuts" : False, },
                                      ] # closes CommonRelInfoTools
           }, # closes "CONFIG"
    } # closes default_config

###################################################################################################

class BLVLinesConf(LineBuilder) :
    """
    Builder
    
    """

    #######################################################
    
    __configuration_keys__ = ( "Postscale",
                               ### Signal channels
                               "La2KmuPrescale",
                               "Lb2KmuPrescale",
                               "Lb2DmuPrescale",
                               "Lb2DsmuPrescale",
                               "B2LcmuPrescale",
                               ### Control channels
                               "Lb2LcpiPrescale",
                               "B2DpiPrescale",
                               "Bs2DspiPrescale",
                               "B2LcpPrescale",
                               "CommonRelInfoTools", )                               

    #######################################################
    
    def __init__(self, name = "BLV", config = None) :

        LineBuilder.__init__(self, name, config)

        ### Signal channels
        La2Kmu_name=name+"La2Kmu"
        Lb2Kmu_name=name+"Lb2Kmu"
        Lb2Dmu_name=name+"Lb2Dmu"
        Lb2Dsmu_name=name+"Lb2Dsmu"
        B2Lcmu_name=name+"B2Lcmu"

        self.selLa2Kmu  = makeLa2Kmu(La2Kmu_name)
        self.selLb2Kmu  = makeLb2Kmu(Lb2Kmu_name)
        self.selLb2Dmu  = makeLb2Dmu(Lb2Dmu_name)
        self.selLb2Dsmu = makeLb2Dsmu(Lb2Dsmu_name)
        self.selB2Lcmu  = makeB2Lcmu(B2Lcmu_name)

        ### Control channels
        Lb2Lcpi_name=name+"Lb2Lcpi"
        B2Dpi_name=name+"B2Dpi"
        Bs2Dspi_name=name+"Bs2Dspi"
        B2Lcp_name=name+"B2Lcp"

        self.selLb2Lcpi = makeLb2Lcpi(Lb2Lcpi_name)
        self.selB2Dpi  = makeB2Dpi(B2Dpi_name)
        self.selBs2Dspi  = makeBs2Dspi(Bs2Dspi_name)
        self.selB2Lcp  = makeB2Lcp(B2Lcp_name)

#######################################################################################################
### signal channels
#######################################################################################################        

        self.La2KmuLine = StrippingLine(La2Kmu_name+"Line",
                                        prescale = config["La2KmuPrescale"],
                                        postscale = config["Postscale"],
                                        MDSTFlag = True,
                                        selection = self.selLa2Kmu,
                                        RelatedInfoTools = [
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [], # writes out all variables
                                             "RecursionLevel" : 1, # mother and direct daughters
                                             "Locations"  : { self.selLa2Kmu : "ConeIso05La",
                                                              "Phys/StdAllLooseKaons" : "ConeIso05K",
                                                              "Phys/StdAllLooseMuons" : "ConeIso05mu", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLa2Kmu : "ConeIso10La",
                                                              "Phys/StdAllLooseKaons" : "ConeIso10K",
                                                              "Phys/StdAllLooseMuons" : "ConeIso10mu", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLa2Kmu : "ConeIso15La",
                                                              "Phys/StdAllLooseKaons" : "ConeIso15K",
                                                              "Phys/StdAllLooseMuons" : "ConeIso15mu", }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 1,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseKaons" : "TrackIsoBDTK",
                                                            "Phys/StdAllLooseMuons" : "TrackIsoBDTmu", }, },
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 1,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseKaons" : "BsMuMuTrackIsoK",
                                                             "Phys/StdAllLooseMuons" : "BsMuMuTrackIsomu", }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                        ) # closes Strippingline
            
        self.Lb2KmuLine = StrippingLine(Lb2Kmu_name+"Line",
                                        prescale = config["Lb2KmuPrescale"],
                                        postscale = config["Postscale"],
                                        MDSTFlag = True,
                                        selection = self.selLb2Kmu,
                                        RelatedInfoTools = [
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLb2Kmu : "ConeIso05Lb",
                                                              "Phys/StdAllLooseKaons" : "ConeIso05K",
                                                              "Phys/StdAllLooseMuons" : "ConeIso05mu", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLb2Kmu : "ConeIso10Lb",
                                                              "Phys/StdAllLooseKaons" : "ConeIso10K",
                                                              "Phys/StdAllLooseMuons" : "ConeIso10mu", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selLb2Kmu : "ConeIso15Lb",
                                                              "Phys/StdAllLooseKaons" : "ConeIso15K",
                                                              "Phys/StdAllLooseMuons" : "ConeIso15mu", }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 1,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseKaons" : "TrackIsoBDTK",
                                                            "Phys/StdAllLooseMuons" : "TrackIsoBDTmu", }, },
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 1,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseKaons" : "BsMuMuTrackIsoK",
                                                             "Phys/StdAllLooseMuons" : "BsMuMuTrackIsomu", }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                        ) # closes Strippingline                                        
 
        self.Lb2DmuLine = StrippingLine(Lb2Dmu_name+"Line",
                                        prescale = config["Lb2DmuPrescale"],
                                        postscale = config["Postscale"],
                                        MDSTFlag = True,
                                        selection = self.selLb2Dmu,
                                        RelatedInfoTools = [                                        
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selLb2Dmu : "ConeIso05Lb",
                                                              "Phys/StdAllLooseMuons" : "ConeIso05mu",
                                                              "Phys/StdAllLooseKaons" : "ConeIso05K",
                                                              "Phys/StdAllLoosePions" : ["ConeIso05pi1", "ConeIso05pi2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selLb2Dmu : "ConeIso10Lb",
                                                              "Phys/StdAllLooseMuons" : "ConeIso10mu",
                                                              "Phys/StdAllLooseKaons" : "ConeIso10K",
                                                              "Phys/StdAllLoosePions" : ["ConeIso10pi1", "ConeIso10pi2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selLb2Dmu : "ConeIso15Lb",
                                                              "Phys/StdAllLooseMuons" : "ConeIso15mu",
                                                              "Phys/StdAllLooseKaons" : "ConeIso15K",
                                                              "Phys/StdAllLoosePions" : ["ConeIso15pi1", "ConeIso15pi2"], }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 2,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseMuons" : "TrackIsoBDTmu",
                                                            "Phys/StdAllLooseKaons" : "TrackIsoBDTK",
                                                            "Phys/StdAllLoosePions" : ["TrackIsoBDTpi1", "TrackIsoBDTpi2"], }, },
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 2,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseKaons" : "BsMuMuTrackIsoK",
                                                             "Phys/StdAllLooseMuons" : "BsMuMuTrackIsomu",
                                                             "Phys/StdAllLoosePions" :
                                                             ["BsMuMuTrackIsopi1","BsMuMuTrackIsopi2"] ,}, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools
                                         ) # closes Strippingline

        self.Lb2DsmuLine = StrippingLine(Lb2Dsmu_name+"Line",
                                         prescale = config["Lb2DsmuPrescale"],
                                         postscale = config["Postscale"],
                                         MDSTFlag = True,
                                         selection = self.selLb2Dsmu,
                                         RelatedInfoTools = [                                        
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selLb2Dsmu : "ConeIso05Lb",
                                                              "Phys/StdAllLooseMuons" : "ConeIso05mu",
                                                              "Phys/StdAllLooseKaons" : ["ConeIso05K1", "ConeIso05K2"],
                                                              "Phys/StdAllLoosePions" : "ConeIso05pi", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selLb2Dsmu : "ConeIso10Lb",
                                                              "Phys/StdAllLooseMuons" : "ConeIso10mu",
                                                              "Phys/StdAllLooseKaons" : ["ConeIso10K1", "ConeIso10K2"],
                                                              "Phys/StdAllLoosePions" : "ConeIso10pi", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selLb2Dsmu : "ConeIso15Lb",
                                                              "Phys/StdAllLooseMuons" : "ConeIso15mu",
                                                              "Phys/StdAllLooseKaons" : ["ConeIso15K1", "ConeIso15K2"],
                                                              "Phys/StdAllLoosePions" : "ConeIso15pi", }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 2,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseMuons" : "TrackIsoBDTmu",
                                                            "Phys/StdAllLooseKaons" : ["TrackIsoBDTK1","TrackIsoBDTK2"],
                                                            "Phys/StdAllLoosePions" : "TrackIsoBDTpi", }, },
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 2,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseMuons" : "BsMuMuTrackIsomu",
                                                             "Phys/StdAllLoosePions" : "BsMuMuTrackIsopi", 
                                                             "Phys/StdAllLooseKaons" :
                                                             ["BsMuMuTrackIsoK1","BsMuMuTrackIsoK2"], }, }, 
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools
                                         ) # closes Strippingline                                         

        self.b2LcmuLine = StrippingLine(B2Lcmu_name+"Line",
                                        prescale = config["B2LcmuPrescale"],
                                        postscale = config["Postscale"],
                                        MDSTFlag = True,
                                        selection = self.selB2Lcmu,
                                        RelatedInfoTools = [                                        
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selB2Lcmu : "ConeIso05B0",
                                                              "Phys/StdAllLooseMuons"   : "ConeIso05mu",
                                                              "Phys/StdAllLooseProtons" : "ConeIso05p", 
                                                              "Phys/StdAllLooseKaons"   : "ConeIso05K",
                                                              "Phys/StdAllLoosePions"   : "ConeIso05pi", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selB2Lcmu : "ConeIso10B0",
                                                              "Phys/StdAllLooseMuons"   : "ConeIso10mu",
                                                              "Phys/StdAllLooseProtons" : "ConeIso10p",
                                                              "Phys/StdAllLooseKaons"   : "ConeIso10K",
                                                              "Phys/StdAllLoosePions"   : "ConeIso10pi", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selB2Lcmu : "ConeIso15B0",
                                                              "Phys/StdAllLooseMuons"   : "ConeIso15mu",
                                                              "Phys/StdAllLooseProtons" : "ConeIso15p",
                                                              "Phys/StdAllLooseKaons"   : "ConeIso15K",
                                                              "Phys/StdAllLoosePions"   : "ConeIso15pi", }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 2,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseMuons"   : "TrackIsoBDTmu",
                                                            "Phys/StdAllLooseProtons" : "TrackIsoBDTp",
                                                            "Phys/StdAllLooseKaons"   : "TrackIsoBDTK",
                                                            "Phys/StdAllLoosePions"   : "TrackIsoBDTpi", }, },
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 2,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseMuons"   : "BsMuMuTrackIsomu",
                                                             "Phys/StdAllLooseProtons" : "BsMuMuTrackIsop",
                                                             "Phys/StdAllLooseKaons"   : "BsMuMuTrackIsoK",
                                                             "Phys/StdAllLoosePions"   : "BsMuMuTrackIsopi", }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                         ) # closes Strippingline 

####################################################################################################### 
### control ch. 
####################################################################################################### 

        self.Lb2LcpiLine = StrippingLine(Lb2Lcpi_name+"Line",
                                         prescale = config["Lb2LcpiPrescale"],
                                         postscale = config["Postscale"],
                                         MDSTFlag = True,
                                         selection = self.selLb2Lcpi,
                                         RelatedInfoTools = [                                        
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selLb2Lcpi : "ConeIso05Lb",
                                                              "Phys/StdAllLooseProtons" : "ConeIso05p", 
                                                              "Phys/StdAllLooseKaons"   : "ConeIso05K",
                                                              "Phys/StdAllLoosePions"   :
                                                              ["ConeIso05pi1","ConeIso05pi2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selLb2Lcpi : "ConeIso10Lb",
                                                              "Phys/StdAllLooseProtons" : "ConeIso10p",
                                                              "Phys/StdAllLooseKaons"   : "ConeIso10K",
                                                              "Phys/StdAllLoosePions"   :
                                                              ["ConeIso10pi1","ConeIso10pi2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selLb2Lcpi : "ConeIso15Lb",
                                                              "Phys/StdAllLooseProtons" : "ConeIso15p",
                                                              "Phys/StdAllLooseKaons"   : "ConeIso15K",
                                                              "Phys/StdAllLoosePions"   :
                                                              ["ConeIso15pi1","ConeIso15pi2"], }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 2,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseProtons" : "TrackIsoBDTp",
                                                            "Phys/StdAllLooseKaons"   : "TrackIsoBDTK",
                                                            "Phys/StdAllLoosePions"   :
                                                            ["TrackIsoBDTpi1","TrackIsoBDTpi2"], }, },
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 2,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseProtons" : "BsMuMuTrackIsop",
                                                             "Phys/StdAllLooseKaons"   : "BsMuMuTrackIsoK",
                                                             "Phys/StdAllLoosePions"   :
                                                             ["BsMuMuTrackIsopi1","BsMuMuTrackIsopi2"], }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                         ) # closes Strippingline 

        self.B2DpiLine = StrippingLine(B2Dpi_name+"Line",
                                       prescale = config["B2DpiPrescale"],
                                       postscale = config["Postscale"],
                                       MDSTFlag = True,
                                       selection = self.selB2Dpi,
                                       RelatedInfoTools = [                                        
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selB2Dpi : "ConeIso05B0",
                                                              "Phys/StdAllLooseKaons" : "ConeIso05K",
                                                              "Phys/StdAllLoosePions" :
                                                              ["ConeIso05pi1","ConeIso05pi2","ConeIso05pi3"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selB2Dpi : "ConeIso10B0",
                                                              "Phys/StdAllLooseKaons" : "ConeIso10K",
                                                              "Phys/StdAllLoosePions" :
                                                              ["ConeIso10pi1","ConeIso10pi2","ConeIso10pi3"], }, }, 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selB2Dpi : "ConeIso15B0",
                                                              "Phys/StdAllLooseKaons" : "ConeIso15K",
                                                              "Phys/StdAllLoosePions" :
                                                              ["ConeIso15pi1","ConeIso15pi2","ConeIso15pi3"], }, }, 
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 2,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseKaons" : "TrackIsoBDTK",
                                                            "Phys/StdAllLoosePions" :
                                                            ["TrackIsoBDTpi1","TrackIsoBDTpi2","TrackIsoBDTpi3"], }, },
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 2,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseKaons" : "BsMuMuTrackIsoK",
                                                             "Phys/StdAllLoosePions" :
                                                             ["BsMuMuTrackIsopi1","BsMuMuTrackIsopi2","BsMuMuTrackIsopi3"], }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools
                                         ) # closes Strippingline 

        self.Bs2DspiLine = StrippingLine(Bs2Dspi_name+"Line",
                                         prescale = config["Bs2DspiPrescale"],
                                         postscale = config["Postscale"],
                                         MDSTFlag = True,
                                         selection = self.selBs2Dspi,
                                         RelatedInfoTools = [                                        
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selBs2Dspi : "ConeIso05Bs",
                                                              "Phys/StdAllLooseKaons" : ["ConeIso05K1","ConeIso05K2"],
                                                              "Phys/StdAllLoosePions" : ["ConeIso05pi1","ConeIso05pi2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selBs2Dspi : "ConeIso10Bs",
                                                              "Phys/StdAllLooseKaons" : ["ConeIso10K1","ConeIso10K2"],
                                                              "Phys/StdAllLoosePions" : ["ConeIso10pi1","ConeIso10pi2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selBs2Dspi : "ConeIso15Bs",
                                                              "Phys/StdAllLooseKaons" : ["ConeIso15K1","ConeIso15K2"],
                                                              "Phys/StdAllLoosePions" : ["ConeIso15pi1","ConeIso15pi2"], }, }, 
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 2,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseKaons" : ["TrackIsoBDTK1","TrackIsoBDTK2"],
                                                            "Phys/StdAllLoosePions" : ["TrackIsoBDTpi1","TrackIsoBDTpi2"], }, },
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 2,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseKaons" : ["BsMuMuTrackIsoK1","BsMuMuTrackIsoK2"],
                                                             "Phys/StdAllLoosePions" : ["BsMuMuTrackIsopi1","BsMuMuTrackIsopi2"], }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools
                                         ) # closes Strippingline                                          
                                                                                                      
        self.b2LcpLine = StrippingLine(B2Lcp_name+"Line",                                           
                                       prescale = config["B2LcpPrescale"], 
                                       postscale = config["Postscale"],
                                       MDSTFlag = True,
                                       selection = self.selB2Lcp,
                                         RelatedInfoTools = [                                        
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selB2Lcp : "ConeIso05B0",
                                                              "Phys/StdAllLooseKaons"   : "ConeIso05K",
                                                              "Phys/StdAllLoosePions"   : "ConeIso05pi",
                                                              "Phys/StdAllLooseProtons" :
                                                              ["ConeIso05p1","ConeIso05p2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selB2Lcp : "ConeIso10B0",
                                                              "Phys/StdAllLooseKaons"   : "ConeIso10K",
                                                              "Phys/StdAllLoosePions"   : "ConeIso10pi",
                                                              "Phys/StdAllLooseProtons" :
                                                              ["ConeIso10p1","ConeIso10p2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 2,
                                             "Locations"  : { self.selB2Lcp : "ConeIso15B0",
                                                              "Phys/StdAllLooseKaons"   : "ConeIso15K",
                                                              "Phys/StdAllLoosePions"   : "ConeIso15pi",
                                                              "Phys/StdAllLooseProtons" :
                                                              ["ConeIso15p1","ConeIso15p2"], }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 2,
                                             "Variables" : 0,
                                             "Locations" : { "Phys/StdAllLooseKaons"   : "TrackIsoBDTK",
                                                             "Phys/StdAllLoosePions"   : "TrackIsoBDTpi", 
                                                             "Phys/StdAllLooseProtons" :
                                                             ["TrackIsoBDTp1","TrackIsoBDTp2"], }, },
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 2,
                                             "Variables"  : [],
                                             "IsoTwoBody" : True,
                                             "Locations"  : { "Phys/StdAllLooseKaons"   : "BsMuMuTrackIsoK",
                                                              "Phys/StdAllLoosePions"   : "BsMuMuTrackIsopi",
                                                              "Phys/StdAllLooseProtons" :
                                                              ["BsMuMuTrackIsop1","BsMuMuTrackIsop2"], }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                         ) # closes Strippingline                                        
                                                                                                      
####################################################################################################### 

        ### Signal channels
        self.registerLine(self.La2KmuLine)
        self.registerLine(self.Lb2KmuLine)
        self.registerLine(self.Lb2DmuLine)
        self.registerLine(self.Lb2DsmuLine)
        self.registerLine(self.b2LcmuLine)
        ### Control channels
        self.registerLine(self.Lb2LcpiLine)        
        self.registerLine(self.B2DpiLine)
        self.registerLine(self.Bs2DspiLine)
        self.registerLine(self.b2LcpLine)

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

def makeB2Lcp(name):

    B2Lcp = CombineParticles("Combine"+name)
    B2Lcp.DecayDescriptors = [ "[B0 -> Lambda_c+ p~-]cc","[B0 -> Lambda_c+ p+]cc" ]
    B2Lcp.DaughtersCuts = { "p~-" : TrackCuts + "& (PIDp>5)"}

    B2Lcp.CombinationCut = BCombinationCut

    B2Lcp.MotherCut = BMotherCut

    _myLambdaC= DataOnDemand(Location = "Phys/StdLooseLambdac2PKPi/Particles")
    _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")

    return Selection (name, Algorithm = B2Lcp, RequiredSelections = [ _myLambdaC, _myProtons ])


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
