"""
Module for construction of B0 -> p+ p+ p- p- lines

Performance:

Full.dst
########
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB24pB24pLine                                       |  0.0076|        38|  1.684|   6.642|
 |!StrippingB24pB2JpsiKpiLine                                  |  0.0162|        81|  1.667|   3.431|
 |!StrippingB24pB2PhiKhLine                                    |  0.0038|        19|  1.316|   0.492|

MC: B2JpsiKpi (11134010)
########################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB24pB2JpsiKpiLine                                  |  6.1830|      6183|  1.251|   5.817|


MC: B2JpsiPhi (13134010)
########################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB24pB2JpsiKpiLine                                  |  6.3410|      6341|  2.179|   5.827|
 
MC: B2PhiKst (11104020)
#######################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB24pB2PhiKhLine                                    |  7.7120|      7712|  1.323|   3.891|  
      
MC: Bs2PhiPhi (MC2011 13104012)
###############################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB24pB2PhiKhLine                                    |  8.4740|      8474|  3.895|   4.305|
    
Exported symbols (use python help!):
   - 
"""

__author__  = ["Oliver Gruenberg"]
__date__    = "19.05.2015"
__version__ = "$Revision: 2.0 $"

#############################################################################

__all__ = ( "B24pLinesConf",
            "default_config", )

#############################################################################

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, DaVinci__N4BodyDecays
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StrippingSelections.Utils import checkConfig
from GaudiKernel.PhysicalConstants import c_light

#############################################################################

default_config = {
    "NAME"        : "B24p",
    "WGs"         : [ "RD" ],
    "STREAMS"     : [ "Bhadron" ],
    "BUILDERTYPE" : "B24pLinesConf",
    "CONFIG"      : {
    # TrackCuts
    "MinTrIPChi2"        : 25.0,
    "MaxTrChi2Dof"       : 3.0,
    "MaxTrGhp"           : 0.3,
    # CommonCombiCuts
    "MaxDoca"            : 0.3, # (mm)
    "mDiffb"             : 500, # (MeV)
    # MassCuts
    "mJpsiMin"           : 2990, # (MeV)
    "mJpsiMax"           : 3200, # (MeV)
    "mKstMax"            : 1200, # (MeV)
    # MotherCuts
    "MaxIPChi2"          : 25,
    "MinVDChi2"          : 225,
    "MaxVtxChi2Dof"      : 9,
    "MinDira"            : 0.0,
    "MinTau"             : 1.0, # (ps)
    # scalings
    "Postscale"          : 1, 
    "B24pPrescale"       : 1, 
    "B2JpsiKpiPrescale"  : 1,
    "B2PhiKhPrescale"    : 1, 
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

    __configuration_keys__ = ( # TrackCuts
                               "MinTrIPChi2",
                               "MaxTrChi2Dof",
                               "MaxTrGhp",
                               # CommonCombiCuts
                               "MaxDoca",
                               "mDiffb",
                               # MassCuts
                               "mJpsiMin",
                               "mJpsiMax",
                               "mKstMax",
                               # MotherCuts
                               "MaxIPChi2",
                               "MinVDChi2",
                               "MaxVtxChi2Dof",
                               "MinDira",
                               "MinTau",
                               # scalings 
                               "Postscale",
                               "B24pPrescale",
                               "B2JpsiKpiPrescale",
                               "B2PhiKhPrescale",
                               "CommonRelInfoTools", )
 
    def __init__(self, name = "B24p", config = default_config) :

        LineBuilder.__init__(self, name, config)
        
        B24p_name      = name+"B24p"
        B2JpsiKpi_name = name+"B2JpsiKpi"
        B2PhiKh_name   = name+"B2PhiKh"

#############################################################################

        self.TrackCuts = """
                         ( MIPCHI2DV(PRIMARY) > %(MinTrIPChi2)s )
                         & ( TRCHI2DOF < %(MaxTrChi2Dof)s )
                         & ( TRGHP < %(MaxTrGhp)s )
                         """ %config

        self.CommonCombi12Cuts = "(ADOCA(1,2) < %(MaxDoca)s*mm)" %config

        self.CommonCombi123Cuts = """
                                  (ADOCA(1,3) < %(MaxDoca)s*mm)
                                  & (ADOCA(2,3) < %(MaxDoca)s*mm)
                                  """ %config

        self.CommonCombiCuts = """
                               (ADAMASS(5323*MeV) < %(mDiffb)s*MeV)
                               & (ADOCA(1,4) < %(MaxDoca)s*mm)
                               & (ADOCA(2,4) < %(MaxDoca)s*mm)
                               & (ADOCA(3,4) < %(MaxDoca)s*mm)                               
                               """ %config
        
        self.MotherCuts = """
                          (BPVIPCHI2() < %(MaxIPChi2)s )
                          & (BPVVDCHI2 > %(MinVDChi2)s )
                          & (VFASPF(VCHI2/VDOF) < %(MaxVtxChi2Dof)s )
                          & (BPVDIRA > %(MinDira)s )
                          & (BPVLTIME() > %(MinTau)s*ps )
                          """ %config  

        self.B2JpsiKpiCombi12Cut = "(ADOCA(1,2) < %(MaxDoca)s*mm) & ( in_range(2990*MeV,AM12,3200*MeV) )"

#############################################################################

        self.selB24p      = self.makeB24p(B24p_name)
        self.selB2JpsiKpi = self.makeB2JpsiKpi(B2JpsiKpi_name,config)
        self.selB2PhiKh   = self.makeB2PhiKh(B2PhiKh_name,config)

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
                                                             ["BsMuMuTrackIsop1","BsMuMuTrackIsop2",
                                                              "BsMuMuTrackIsop3","BsMuMuTrackIsop4"] ,}, },
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
                                                                   "Phys/StdAllLooseKaons"   : "ConeIso05K",
                                                                   "Phys/StdAllNoPIDsPions"  : "ConeIso05pi", }, },
                                                { "Type" : "RelInfoConeVariables",
                                                  "ConeAngle" : 1.0,
                                                  "Variables" : [],
                                                  "RecursionLevel" : 1,
                                                  "Locations"  : { self.selB2JpsiKpi : "ConeIso10B0",
                                                                   "Phys/StdAllLooseProtons" : ["ConeIso10p1", "ConeIso10p2"],
                                                                   "Phys/StdAllLooseKaons"   : "ConeIso10K",
                                                                   "Phys/StdAllNoPIDsPions"  : "ConeIso10pi", }, },
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

        self.B2PhiKh_Line = StrippingLine(B2PhiKh_name+"Line",
                                          prescale = config["B2PhiKhPrescale"],
                                          postscale = config["Postscale"],
                                          MDSTFlag = True, 
                                          selection = self.selB2PhiKh,
                                          RelatedInfoTools = [
                                                { "Type" : "RelInfoConeVariables",
                                                  "ConeAngle" : 0.5,
                                                  "Variables" : [],
                                                  "RecursionLevel" : 1,
                                                  "Locations"  : { self.selB2PhiKh : "ConeIso05B0",
                                                                   "Phys/StdAllNoPIDsPions"  : "ConeIso05pi", 
                                                                   "Phys/StdAllLooseKaons"   :
                                                                   ["ConeIso05K1","ConeIso05K2","ConeIso05K3"],
                                                                   }, },
                                                { "Type" : "RelInfoConeVariables",
                                                  "ConeAngle" : 1.0,
                                                  "Variables" : [],
                                                  "RecursionLevel" : 1,
                                                  "Locations"  : { self.selB2PhiKh : "ConeIso10B0",
                                                                   "Phys/StdAllNoPIDsPions"  : "ConeIso10pi",
                                                                   "Phys/StdAllLooseKaons"   :
                                                                   ["ConeIso10K1","ConeIso10K2","ConeIso10K3"], }, },
                                                { "Type" : "RelInfoConeVariables",
                                                  "ConeAngle" : 1.5,
                                                  "Variables" : [],
                                                  "RecursionLevel" : 1,
                                                  "Locations"  : { self.selB2PhiKh : "ConeIso15B0",
                                                                   "Phys/StdAllNoPIDsPions"  : ["ConeIso15pi"],
                                                                   "Phys/StdAllLooseKaons"   :
                                                                   ["ConeIso15K1","ConeIso15K2","ConeIso15K3"], }, },
                                                { "Type": "RelInfoTrackIsolationBDT",
                                                  "RecursionLevel" : 1,
                                                  "Variables" : 0,
                                                  "Locations": { "Phys/StdAllNoPIDsPions"  : "TrackIsoBDTpi",
                                                                 "Phys/StdAllLooseKaons"   :
                                                                 ["TrackIsoBDTK1","TrackIsoBDTK2","TrackIsoBDTK3"], }, },
                                                { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                                  "RecursionLevel" : 1,
                                                  "Variables" : [],
                                                  "IsoTwoBody" : True,
                                                  "Locations" : { "Phys/StdAllNoPIDsPions" : "BsMuMuTrackIsopi",
                                                                  "Phys/StdAllLooseKaons"  :
                                                                  ["BsMuMuTrackIsoK1","BsMuMuTrackIsoK2","BsMuMuTrackIsoK3"], }, },
                                                ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                            ) # closes Strippingline

#############################################################################

        self.registerLine(self.B24p_Line)
        self.registerLine(self.B2JpsiKpi_Line)
        self.registerLine(self.B2PhiKh_Line)
       
#############################################################################

    def makeB24p(self,name): # to select B(s) -> p p p p 

        B24p = DaVinci__N4BodyDecays("Combine"+name)
        B24p.DecayDescriptors = [ "[B0 -> p+ p+ p~- p~-]cc","[B0 -> p+ p+ p+ p~-]cc" ]    
        B24p.DaughtersCuts = { "p+" : self.TrackCuts + " & ((PIDp-PIDpi)>0)" + " & ((PIDp-PIDK)>0)" }
            
        B24p.Combination12Cut  = self.CommonCombi12Cuts 
        B24p.Combination123Cut = self.CommonCombi123Cuts    
        B24p.CombinationCut    = self.CommonCombiCuts    
        B24p.MotherCut         = self.MotherCuts
    
        _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    
        return Selection(name, Algorithm = B24p, RequiredSelections = [ _myProtons ])

#############################################################################
        
    def makeB2JpsiKpi(self,name,config): # to select B(s) -> J/psi K+ pi- , Bd -> J/psi K*0 , Bs -> J/psi Phi 
        
        B2JpsiKpi = DaVinci__N4BodyDecays("Combine"+name)
        B2JpsiKpi.DecayDescriptors = [ "[B0 -> p+ p~- K+ pi-]cc" ]
        B2JpsiKpi.DaughtersCuts = { "p+"  : self.TrackCuts + " & ((PIDp-PIDpi)>-10)" + " & ((PIDp-PIDK)>-10)",
                                    "K+"  : self.TrackCuts + " & ((PIDK-PIDpi)>-10)" + " & ((PIDK-PIDp)>-10)",
                                    "pi-" : self.TrackCuts }
    
        B2JpsiKpi.Combination12Cut  = self.CommonCombi12Cuts + " & ( in_range(%(mJpsiMin)s*MeV,AM12,%(mJpsiMax)s*MeV) )" %config
        B2JpsiKpi.Combination123Cut = self.CommonCombi123Cuts
        B2JpsiKpi.CombinationCut    = self.CommonCombiCuts
        B2JpsiKpi.MotherCut         = self.MotherCuts
       
        _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
        _myKaons   = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")    
        _myPions   = DataOnDemand(Location = "Phys/StdNoPIDsPions/Particles")
    
        return Selection(name, Algorithm = B2JpsiKpi, RequiredSelections = [ _myProtons, _myKaons, _myPions ])

#############################################################################

    def makeB2PhiKh(self,name,config): # to select Bs -> Phi Phi and Bd -> Phi K*0
    
        B2PhiKh = DaVinci__N4BodyDecays("Combine"+name)
        B2PhiKh.DecayDescriptors = [ "[B0 -> K+ K- K+ pi-]cc" ]
        B2PhiKh.DaughtersCuts = { "K+"  : self.TrackCuts,
                                  "pi-" : self.TrackCuts }
    
        B2PhiKh.Combination12Cut  = self.CommonCombi12Cuts
        B2PhiKh.Combination123Cut = self.CommonCombi123Cuts + " & ( (AM12 < %(mKstMax)s*MeV) | (AM23 < %(mKstMax)s*MeV) )" %config
        B2PhiKh.CombinationCut    = self.CommonCombiCuts +    " & ( (AM14 < %(mKstMax)s*MeV) | (AM34 < %(mKstMax)s*MeV) )" %config
        B2PhiKh.MotherCut         = self.MotherCuts
        
        _myKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")    
        _myPions = DataOnDemand(Location = "Phys/StdNoPIDsPions/Particles")
            
        return Selection(name, Algorithm = B2PhiKh, RequiredSelections = [ _myKaons, _myPions ])
       
#############################################################################
