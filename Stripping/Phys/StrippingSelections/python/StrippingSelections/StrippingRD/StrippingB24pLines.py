"""
Module for construction of B0 -> p+ p+ p- p- lines

Performance:

Full.dst
########

StrippingReport                                                INFO Event 500000, Good event 500000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |_StrippingGlobal_                                            |  0.0236|       118|       |  15.881|
 |_StrippingSequenceStreamBhadron_                             |  0.0236|       118|       |  15.817|
 |!StrippingB24pB24pLine                                       |  0.0076|        38|  1.684|   6.651|
 |!StrippingB24pB2JpsiKpiLine                                  |  0.0162|        81|  1.667|   3.642|
 |_StrippingSequenceStreamMDST_                                |  0.0236|       118|       |   0.055|
 |!StrippingB24pB24pLine                                       |  0.0076|        38|  1.684|   6.651|
 |!StrippingB24pB2JpsiKpiLine                                  |  0.0162|        81|  1.667|   3.642|

B2JpsiKpi (11134010) MC
#######################

StrippingReport                                                INFO Event 100000, Good event 100000
|                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
|!StrippingB24pLinesB2JpsiKpiLine                             |  6.1800|      6180|  1.251|   8.040|


Exported symbols (use python help!):
   - 
"""

__author__ = ["Oliver Gruenberg"]
__date__ = "20.04.2015"
__version__ = "$Revision: 1.0 $"

#############################################################################

__all__ = ("B24pLinesConf",
           "default_config",
           "makeB24p",
           "makeB2JpsiKpi",           
           )

#############################################################################

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, DaVinci__N4BodyDecays
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StrippingSelections.Utils import checkConfig
from GaudiKernel.PhysicalConstants import c_light

#############################################################################

default_config = {
    "NAME"        : "B24p",
    "WGs"         : [ "RD" ],
    "STREAMS"     : [ "Bhadron" ],
    "BUILDERTYPE" : "B24pLinesConf",
    "CONFIG"      : {
    "Postscale"          : 1, 
    "B24pPrescale"       : 1, 
    "B2JpsiKpiPrescale"  : 1,
    "CommonRelInfoTools" : [ { "Type": "RelInfoVertexIsolation", "Location":"VtxIsoInfo" },
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

class B24pLinesConf(LineBuilder) :
    """
    Builder
    
    """

    __configuration_keys__ = (    "Postscale",
                                  "B24pPrescale",
                                  "B2JpsiKpiPrescale",
                                  "CommonRelInfoTools", )
 
    def __init__(self, name = "B24p", config = None) :

        LineBuilder.__init__(self, name, config)
        
        B24p_name=name+"B24p"
        B2JpsiKpi_name=name+"B2JpsiKpi"        

        self.selB24p = makeB24p(B24p_name)
        self.selB2JpsiKpi = makeB2JpsiKpi(B2JpsiKpi_name)
        
#############################################################################

        self.B24p_Line = StrippingLine(B24p_name+"Line",
                                       prescale = config["B24pPrescale"],
                                       postscale = config["Postscale"],
                                       MDSTFlag = True,                                       
                                       selection = self.selB24p,
                                       RelatedInfoTools = [                                       
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB24p : "ConeIso05B0",
                                                              "Phys/StdAllLooseProtons" :
                                                              ["ConeIso05p1", "ConeIso05p2", "ConeIso05p3", "ConeIso05p4"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB24p : "ConeIso10B0",
                                                              "Phys/StdAllLooseProtons" :
                                                              ["ConeIso10p1", "ConeIso10p2", "ConeIso10p3", "ConeIso10p4"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB24p : "ConeIso15B0",
                                                              "Phys/StdAllLooseProtons" :
                                                              ["ConeIso15p1", "ConeIso15p2", "ConeIso15p3", "ConeIso15p4"], }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 1,
                                             "Variables" : 0,
                                             "Locations": { "Phys/StdAllLooseProtons" :
                                                            ["TrackIsoBDTp1","TrackIsoBDTp2","TrackIsoBDTp3","TrackIsoBDTp4"], }, },
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 1,
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "Locations" : { "Phys/StdAllLooseProtons" :
                                                             ["BsMuMuTrackIsop1","BsMuMuTrackIsop2","BsMuMuTrackIsop3","BsMuMuTrackIsop4"] ,}, }, 
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                       ) # closes Strippingline

        
        self.B2JpsiKpi_Line = StrippingLine(B2JpsiKpi_name+"Line",
                                            prescale = config["B2JpsiKpiPrescale"],
                                            postscale = config["Postscale"],
                                            MDSTFlag = True,
                                            selection = self.selB2JpsiKpi,                                          
                                            RelatedInfoTools = [
                                                { "Type" : "RelInfoConeVariables",
                                                  "ConeAngle" : 0.5,
                                                  "Variables" : [],
                                                  "RecursionLevel" : 1,
                                                  "Locations"  : { self.selB2JpsiKpi : "ConeIso05B0",
                                                                   "Phys/StdAllLooseProtons" : ["ConeIso05p1", "ConeIso05p2"],
                                                                   "Phys/StdAllLooseKaons"   : ["ConeIso05K"],
                                                                   "Phys/StdAllNoPIDsPions"  : ["ConeIso05pi"], }, },
                                                { "Type" : "RelInfoConeVariables",
                                                  "ConeAngle" : 1.0,
                                                  "Variables" : [],
                                                  "RecursionLevel" : 1,
                                                  "Locations"  : { self.selB2JpsiKpi : "ConeIso10B0",
                                                                   "Phys/StdAllLooseProtons" : ["ConeIso10p1", "ConeIso10p2"],
                                                                   "Phys/StdAllLooseKaons"   : ["ConeIso10K"],
                                                                   "Phys/StdAllNoPIDsPions"  : ["ConeIso10pi"], }, },
                                                { "Type" : "RelInfoConeVariables",
                                                  "ConeAngle" : 1.5,
                                                  "Variables" : [],
                                                  "RecursionLevel" : 1,
                                                  "Locations"  : { self.selB2JpsiKpi : "ConeIso15B0",
                                                                   "Phys/StdAllLooseProtons" : ["ConeIso15p1", "ConeIso15p2"],
                                                                   "Phys/StdAllLooseKaons"   : ["ConeIso15K"],
                                                                   "Phys/StdAllNoPIDsPions"  : ["ConeIso15pi"], }, },
                                                { "Type": "RelInfoTrackIsolationBDT",
                                                  "RecursionLevel" : 1,
                                                  "Variables" : 0,
                                                  "Locations": { "Phys/StdAllLooseProtons" : ["TrackIsoBDTp1","TrackIsoBDTp2" ],
                                                                 "Phys/StdAllLooseKaons"   : "TrackIsoBDTK",
                                                                 "Phys/StdAllNoPIDsPions"  : "TrackIsoBDTpi", }, },
                                                { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                                  "RecursionLevel" : 1,
                                                  "Variables" : [],
                                                  "IsoTwoBody" : True,
                                                  "Locations" : { "Phys/StdAllLooseKaons" : "BsMuMuTrackIsoK",
                                                                  "Phys/StdAllNoPIDsPions" : "BsMuMuTrackIsopi",
                                                                  "Phys/StdAllLooseProtons" :
                                                                  ["BsMuMuTrackIsop1","BsMuMuTrackIsop2"] ,}, },
                                                ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                            ) # closes Strippingline

#############################################################################

        self.registerLine(self.B24p_Line)
        self.registerLine(self.B2JpsiKpi_Line)        
       
#############################################################################

TrackCuts = "(MIPCHI2DV(PRIMARY)>25.0) & (TRCHI2DOF<3.0) & (TRGHP<0.3)"

p4Combination12Cut = "(ADOCA(1,2)<0.3*mm)"

JpsiCombination12Cut = "(AM>2990*MeV) & (AM<3200*MeV) & (ADOCA(1,2)<0.3*mm)"

BCombination123Cut = "(ADOCA(1,3)<0.3*mm) & (ADOCA(2,3)<0.3*mm)"

BCombinationCut = "(ADAMASS(5323*MeV) < 500*MeV)"\
                  " & (ADOCA(1,4) < 0.3*mm)"\
                  " & (ADOCA(2,4) < 0.3*mm)"\
                  " & (ADOCA(3,4) < 0.3*mm)"\

BMotherCut = "(BPVIPCHI2() < 25)"\
             " & (BPVVDCHI2 > 225)"\
             " & (VFASPF(VCHI2/VDOF)<9)"\
             " & (BPVDIRA > 0.0)"\
             " & (BPVLTIME() > 1.0*ps)"\

#############################################################################

def makeB24p(name):

    B24p = DaVinci__N4BodyDecays("Combine"+name)
    B24p.DecayDescriptors = [ "[B0 -> p+ p+ p~- p~-]cc","[B0 -> p+ p+ p+ p~-]cc" ]    
    B24p.DaughtersCuts = { "p+" : TrackCuts + " & ((PIDp-PIDpi)>0)" + " & ((PIDp-PIDK)>0)"}
    B24p.Combination12Cut = p4Combination12Cut
    B24p.Combination123Cut = BCombination123Cut
    B24p.CombinationCut = BCombinationCut
    B24p.MotherCut = BMotherCut

    _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    
    return Selection(name, Algorithm = B24p, RequiredSelections = [ _myProtons ])

#############################################################################

def makeB2JpsiKpi(name):
    
    B2JpsiKpi = DaVinci__N4BodyDecays("Combine"+name)
    B2JpsiKpi.DecayDescriptors = [ "[B0 -> p+ p~- K+ pi-]cc" ]
    B2JpsiKpi.DaughtersCuts = { "p+" : TrackCuts + " & ((PIDp-PIDpi)>-10)" + " & ((PIDp-PIDK)>-10)",
                                "K+" : TrackCuts + " & ((PIDK-PIDpi)>-10)" + " & ((PIDK-PIDp)>-10)",
                                "pi-" : TrackCuts }
    B2JpsiKpi.Combination12Cut = JpsiCombination12Cut
    B2JpsiKpi.Combination123Cut = BCombination123Cut
    B2JpsiKpi.CombinationCut = BCombinationCut
    B2JpsiKpi.MotherCut = BMotherCut

    _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    _myKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")    
    _myPions = DataOnDemand(Location = "Phys/StdNoPIDsPions/Particles")
    
    return Selection(name, Algorithm = B2JpsiKpi, RequiredSelections = [ _myProtons, _myKaons, _myPions ])

#############################################################################
