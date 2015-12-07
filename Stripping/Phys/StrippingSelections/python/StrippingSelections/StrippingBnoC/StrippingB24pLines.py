"""
Module for construction of B0 -> p+ p+ p- p- lines

Performance: using DV38r0

Full.dst (Reco14_Run125113.py)
##########################
StrippingReport                                                INFO Event 1000000, Good event 1000000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |_StrippingGlobal_                                            |  0.0475|        95|       |  18.580|
 |_StrippingSequenceStreamBhadron_                             |  0.0475|        95|       |  18.502|  
 |!StrippingB24pB24pLine_TIMING                                |  0.0235|        47|  1.128|   0.316|
 |!StrippingB24pB2JpsiKpiLine_TIMING                           |  0.0210|        42|  1.524|   0.736|
 |!StrippingB24pB2PhiKhLine_TIMING                             |  0.0030|         6|  1.833|   0.542|
   

MC: B2pppp (11104077)
#######################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB24pB24pLine_TIMING                                |  5.0250|      5025|  1.022|   0.260|

MC: B2JpsiKpi (11134010)
########################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB24pB2JpsiKpiLine_TIMING                           |  6.3320|      6332|  1.311|   0.574|

MC: B2PhiKst (11104020)
#######################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB24pB2PhiKhLine_TIMING                             |  7.8100|      7810|  1.318|   0.476|

MC: Bs2JpsiPhi (13134010)
########################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingB24pB2JpsiKpiLine_TIMING                           |  6.3960|      6396|  2.397|   0.609|


    
Exported symbols (use python help!):
   - 
"""

__author__  = ["Oliver Gruenberg"]
__date__    = "07.12.2015"
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
    "MaxTrChi2Dof"       : 4.0,
    "MaxTrGhp"           : 0.4,
    # CommonCombiCuts
    "MaxDoca"            : 0.3, # (mm)
    "mDiffb"             : 400, # (MeV)
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
                                             "Location"  : 'ConeIso05B0',
                                             "DaughterLocations" : {
                                             "Beauty -> ^X  X  X  X"  : "ConeIso05p1",
                                             "Beauty ->  X ^X  X  X"  : "ConeIso05p2",
                                             "Beauty ->  X  X ^X  X"  : "ConeIso05p3",
                                             "Beauty ->  X  X  X ^X"  : "ConeIso05p4", 
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso10B0',
                                             "DaughterLocations" : {
                                             "Beauty -> ^X  X  X  X"  : "ConeIso10p1",
                                             "Beauty ->  X ^X  X  X"  : "ConeIso10p2",
                                             "Beauty ->  X  X ^X  X"  : "ConeIso10p3",
                                             "Beauty ->  X  X  X ^X"  : "ConeIso10p4", 
                                             }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "Location"  : 'ConeIso15B0',
                                             "DaughterLocations" : {
                                             "Beauty -> ^X  X  X  X"  : "ConeIso15p1",
                                             "Beauty ->  X ^X  X  X"  : "ConeIso15p2",
                                             "Beauty ->  X  X ^X  X"  : "ConeIso15p3",
                                             "Beauty ->  X  X  X ^X"  : "ConeIso15p4",
                                             }, }, 
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "Variables" : 0,
                                             "DaughterLocations" : {
                                             "Beauty -> ^X  X  X  X"  : "TrackIsoBDTp1",
                                             "Beauty ->  X ^X  X  X"  : "TrackIsoBDTp2",
                                             "Beauty ->  X  X ^X  X"  : "TrackIsoBDTp3",
                                             "Beauty ->  X  X  X ^X"  : "TrackIsoBDTp4",
                                             }, },
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "Variables" : [],
                                             "IsoTwoBody" : True,
                                             "DaughterLocations" : {
                                             "Beauty -> ^X  X  X  X"  : "BsMuMuTrackIsop1",
                                             "Beauty ->  X ^X  X  X"  : "BsMuMuTrackIsop2",
                                             "Beauty ->  X  X ^X  X"  : "BsMuMuTrackIsop3",
                                             "Beauty ->  X  X  X ^X"  : "BsMuMuTrackIsop4",
                                             }, },
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
                                                  "Location"  : "ConeIso05B0",
                                                  "DaughterLocations" : {
                                                  "[B0 -> ^p+  p~-  K+  pi-]CC" : "ConeIso05p1",
                                                  "[B0 ->  p+ ^p~-  K+  pi-]CC" : "ConeIso05p2",
                                                  "[B0 ->  p+  p~- ^K+  pi-]CC" : "ConeIso05K",
                                                  "[B0 ->  p+  p~-  K+ ^pi-]CC" : "ConeIso05pi",
                                                  }, },
                                                { "Type" : "RelInfoConeVariables",
                                                  "ConeAngle" : 1.0,
                                                  "Variables" : [],
                                                  "Location"  : "ConeIso10B0",
                                                  "DaughterLocations" : {
                                                  "[B0 -> ^p+  p~-  K+  pi-]CC" : "ConeIso10p1",
                                                  "[B0 ->  p+ ^p~-  K+  pi-]CC" : "ConeIso10p2",
                                                  "[B0 ->  p+  p~- ^K+  pi-]CC" : "ConeIso10K",
                                                  "[B0 ->  p+  p~-  K+ ^pi-]CC" : "ConeIso10pi",
                                                  }, },
                                                { "Type" : "RelInfoConeVariables",
                                                  "ConeAngle" : 1.5,
                                                  "Variables" : [],
                                                  "Location"  : "ConeIso15B0",
                                                  "DaughterLocations" : {
                                                  "[B0 -> ^p+  p~-  K+  pi-]CC" : "ConeIso15p1",
                                                  "[B0 ->  p+ ^p~-  K+  pi-]CC" : "ConeIso15p2",
                                                  "[B0 ->  p+  p~- ^K+  pi-]CC" : "ConeIso15K",
                                                  "[B0 ->  p+  p~-  K+ ^pi-]CC" : "ConeIso15pi",
                                                  }, },
                                                { "Type": "RelInfoTrackIsolationBDT",
                                                  "Variables" : 0,
                                                  "DaughterLocations" : {
                                                  "[B0 -> ^p+  p~-  K+  pi-]CC" : "TrackIsoBDTp1",
                                                  "[B0 ->  p+ ^p~-  K+  pi-]CC" : "TrackIsoBDTp2",
                                                  "[B0 ->  p+  p~- ^K+  pi-]CC" : "TrackIsoBDTK",
                                                  "[B0 ->  p+  p~-  K+ ^pi-]CC" : "TrackIsoBDTpi",
                                                  }, },
                                                { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                                  "Variables" : [],
                                                  "IsoTwoBody" : True,
                                                  "DaughterLocations" : {
                                                  "[B0 -> ^p+  p~-  K+  pi-]CC" : "BsMuMuTrackIsop1",
                                                  "[B0 ->  p+ ^p~-  K+  pi-]CC" : "BsMuMuTrackIsop2",
                                                  "[B0 ->  p+  p~- ^K+  pi-]CC" : "BsMuMuTrackIsoK",
                                                  "[B0 ->  p+  p~-  K+ ^pi-]CC" : "BsMuMuTrackIsopi",
                                                  }, }, 
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
                                                  "Location"  : "ConeIso05B0",
                                                  "DaughterLocations" : {
                                                  "[B0 -> ^K+  K-  K+  pi-]CC" : "ConeIso05K1",
                                                  "[B0 ->  K+ ^K-  K+  pi-]CC" : "ConeIso05K2",
                                                  "[B0 ->  K+  K- ^K+  pi-]CC" : "ConeIso05K3",
                                                  "[B0 ->  K+  K-  K+ ^pi-]CC" : "ConeIso05pi",
                                                  }, },
                                                { "Type" : "RelInfoConeVariables",
                                                  "ConeAngle" : 1.0,
                                                  "Variables" : [],
                                                  "Location"  : "ConeIso10B0",
                                                  "DaughterLocations" : {
                                                  "[B0 -> ^K+  K-  K+  pi-]CC" : "ConeIso10K1",
                                                  "[B0 ->  K+ ^K-  K+  pi-]CC" : "ConeIso10K2",
                                                  "[B0 ->  K+  K- ^K+  pi-]CC" : "ConeIso10K3",
                                                  "[B0 ->  K+  K-  K+ ^pi-]CC" : "ConeIso10pi",
                                                  }, },
                                                { "Type" : "RelInfoConeVariables",
                                                  "ConeAngle" : 1.5,
                                                  "Variables" : [],
                                                  "Location"  : "ConeIso15B0",
                                                  "DaughterLocations" : {
                                                  "[B0 -> ^K+  K-  K+  pi-]CC" : "ConeIso15K1",
                                                  "[B0 ->  K+ ^K-  K+  pi-]CC" : "ConeIso15K2",
                                                  "[B0 ->  K+  K- ^K+  pi-]CC" : "ConeIso15K3",
                                                  "[B0 ->  K+  K-  K+ ^pi-]CC" : "ConeIso15pi",
                                                  }, },
                                                { "Type": "RelInfoTrackIsolationBDT",
                                                  "Variables" : 0,
                                                  "DaughterLocations" : {
                                                  "[B0 -> ^K+  K-  K+  pi-]CC" : "TrackIsoBDTK1",
                                                  "[B0 ->  K+ ^K-  K+  pi-]CC" : "TrackIsoBDTK2",
                                                  "[B0 ->  K+  K- ^K+  pi-]CC" : "TrackIsoBDTK3",
                                                  "[B0 ->  K+  K-  K+ ^pi-]CC" : "TrackIsoBDTpi",
                                                  }, },                                               
                                                { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                                  "Variables" : [],
                                                  "IsoTwoBody" : True,
                                                  "DaughterLocations" : {
                                                  "[B0 -> ^K+  K-  K+  pi-]CC" : "BsMuMuTrackIsoK1",
                                                  "[B0 ->  K+ ^K-  K+  pi-]CC" : "BsMuMuTrackIsoK2",
                                                  "[B0 ->  K+  K- ^K+  pi-]CC" : "BsMuMuTrackIsoK3",
                                                  "[B0 ->  K+  K-  K+ ^pi-]CC" : "BsMuMuTrackIsopi",
                                                  }, },
                                                ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                            ) # closes Strippingline

#############################################################################

        self.registerLine(self.B24p_Line)
        self.registerLine(self.B2JpsiKpi_Line)
        self.registerLine(self.B2PhiKh_Line)
       
#############################################################################

    def makeB24p(self,name): # to select B(s) -> p p p p 

        B24p = DaVinci__N4BodyDecays("Combine"+name)
        B24p.DecayDescriptors = [ "B0 -> p+ p+ p~- p~-","[B0 -> p+ p+ p+ p~-]cc" ]
        B24p.DaughtersCuts = { "p+" : self.TrackCuts + " & ((PIDp-PIDpi)>0)" + " & ((PIDp-PIDK)>-5)" }
            
        B24p.Combination12Cut  = self.CommonCombi12Cuts 
        B24p.Combination123Cut = self.CommonCombi123Cuts    
        B24p.CombinationCut    = self.CommonCombiCuts    
        B24p.MotherCut         = self.MotherCuts
    
        _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
        #_myProtons = DataOnDemand(Location = "Phys/StdNoPIDsProtons/Particles")
    
        return Selection(name, Algorithm = B24p, RequiredSelections = [ _myProtons ])

#############################################################################
        
    def makeB2JpsiKpi(self,name,config): # to select B(s) -> J/psi K+ pi- , Bd -> J/psi K*0 , Bs -> J/psi Phi 
        
        B2JpsiKpi = DaVinci__N4BodyDecays("Combine"+name)
        B2JpsiKpi.DecayDescriptors = [ "[B0 -> p+ p~- K+ pi-]cc" ]
        B2JpsiKpi.DaughtersCuts = { "p+"  : self.TrackCuts,
                                    "K+"  : self.TrackCuts,
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
