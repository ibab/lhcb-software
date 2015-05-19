"""
Module for construction of baryon-lepton number violating stripping selections and lines

Performance (prescaling included):

Full.dst
########
StrippingReport                                                INFO Event 500000, Good event 500000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |_StrippingGlobal_                                            |  0.1294|       647|       |  48.457|
 |_StrippingSequenceStreamBhadron_                             |  0.1294|       647|       |  48.368|
 |!StrippingBLVLa2KmuLine                                      |  0.0270|       135|  1.007|   7.532|
 |!StrippingBLVLb2KmuLine                                      |  0.0086|        43|  1.000|   0.294|
 |!StrippingBLVXib2KhmuLine                                    |  0.0032|        16|  1.250|   2.189|
 |!StrippingBLVB2KhhLine                                       |  0.0130|        65|  1.046|   0.613|
 |!StrippingBLVLb2DmuLine                                      |  0.0110|        55|  1.291|  10.539|
 |!StrippingBLVLb2DsmuLine                                     |  0.0114|        57|  1.456|  12.617|
 |!StrippingBLVLb2LcpiLine                                     |  0.0070|        35|  1.286|   0.954|
 |!StrippingBLVB2LcmuLine                                      |  0.0230|       115|  1.400|   7.419|
 |!StrippingBLVB2DpiLine                                       |  0.0052|        26|  1.346|   0.056|
 |!StrippingBLVBs2DspiLine                                     |  0.0074|        37|  1.432|   0.051|
 |!StrippingBLVB2LcpLine                                       |  0.0290|       145|  1.697|   0.109|


MC: Lambda -> K mu (MC11a/33112001)
###################################
StrippingReport                                                INFO Event 19600, Good event 19600
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingBLVLa2KmuLine                                      |  0.4388|        86|  1.000|   2.853|

MC: Lambda_b -> K mu (15112001)
###############################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingBLVLb2KmuLine                                      | 29.6280|     29628|  1.002|  11.520|

MC: Lambda_b -> D+ mu (15174011)
################################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingBLVLb2DmuLine                                      |  8.1930|      8193|  1.021|   8.174|

MC: Lambda_b -> Ds+ mu (15174001)
#################################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingBLVLb2DsmuLine                                     |  8.5070|      8507|  1.117|   5.941|

MC: B0 -> Lambda_c+ mu (11174001)
#################################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingBLVB2LcmuLine                                      |  7.8240|      7824|  1.080|   6.130|

MC: Bs0 -> Lambda_c+ mu (13174001)
##################################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingBLVB2LcmuLine                                      |  8.1230|      8123|  1.080|   5.323|

MC: B+ -> K*0 pi+ (12103091)
############################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingBLVB2KhhLine                                       | 13.4310|     13431|  1.004|   6.188|

MC: B+ -> Phi K+ (12103061)
###########################
StrippingReport                                                INFO Event 100000, Good event 100000
 |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |!StrippingBLVB2KhhLine                                       | 16.3140|     16314|  1.005|   7.537|
              
Exported symbols (use python help!):
   - 
"""

__author__ = ["Oliver Gruenberg"]
__date__ = "17.05.2015"
__version__ = "$Revision: 1.0 $"

###################################################################################################

__all__ = ( "BLVLinesConf",
            "default_config" )

###################################################################################################

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, DaVinci__N3BodyDecays
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

###################################################################################################

default_config = {
    "NAME"        : "BLV",
    "WGs"         : [ "RD" ],
    "STREAMS"     : [ "Bhadron" ],
    "BUILDERTYPE" : "BLVLinesConf",
    "CONFIG"      : { # TrackCuts
                      "MinTrIPChi2"         : 25.0,
                      "MaxTrChi2Dof"        : 3.0,
                      "MaxTrGhp"            : 0.3,
                      # CombiCuts
                      "MaxDoca"             : 0.3,
                      # MassCuts 
                      "mDiffD"              : 100.0,
                      "mDiffDs"             : 100.0,
                      "mDiffbLoose"         : 500.0,
                      "mDiffbStand"         : 400.0,
                      "mDiffbTight"         : 300.0,
                      "mDiffLa"             : 400.0,
                      "mKst"                : 1200.0,
                      # MotherCuts
                      "MaxIPChi2"           : 25,
                      "MinVDChi2Loose"      : 100,
                      "MinVDChi2Tight"      : 225,
                      "MaxVtxChi2Dof"       : 9,
                      "MinDira"             : 0.0, 
                      "MinTauLoose"         : 0.0, # (ps)
                      "MinTauTight"         : 1.0, # (ps)
                      "MinTauLambda"        : 10.0, # (ps)
                      # scalings
                      "Postscale"           : 1,
                      ### Lambda_(b)0 -> K- mu+
                      "La2KmuPrescale"      : 1,
                      "Lb2KmuPrescale"      : 1,
                      ### Xi_b- -> K*0 mu-
                      "Xib2KhmuPrescale"    : 1, 
                      "B2KhhPrescale"       : 1,                      
                      ### Lb -> D(s)+ mu-
                      "Lb2DmuPrescale"      : 1,
                      "Lb2DsmuPrescale"     : 1,
                      "B2LcmuPrescale"      : 1,
                      ### B(s)0 -> Lc+ mu- 
                      "Lb2LcpiPrescale"     : 0.1,
                      "B2DpiPrescale"       : 0.1,
                      "Bs2DspiPrescale"     : 0.1,
                      "B2LcpPrescale"       : 0.2,
                      ### RelatedInfoTools
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
    
    __configuration_keys__ = ( # TrackCuts
                               "MinTrIPChi2",
                               "MaxTrChi2Dof",
                               "MaxTrGhp",
                               # CombiCuts
                               "MaxDoca",
                               # MassCuts 
                               "mDiffD",
                               "mDiffDs",
                               "mDiffbLoose",
                               "mDiffbStand",
                               "mDiffbTight",
                               "mDiffLa",
                               "mKst",
                               # MotherCuts
                               "MaxIPChi2",
                               "MinVDChi2Loose",
                               "MinVDChi2Tight",
                               "MaxVtxChi2Dof",
                               "MinDira",
                               "MinTauLoose",
                               "MinTauTight",
                               "MinTauLambda",
                               ### scalings
                               "Postscale",
                               ### Lambda_(b)0 -> K- mu+ 
                               "La2KmuPrescale",
                               "Lb2KmuPrescale",
                               ### Xi_b- -> K*0 mu- 
                               "Xib2KhmuPrescale",                               
                               "B2KhhPrescale",
                               "CommonRelInfoTools",                               
                               ### Lb -> D(s)+ mu- 
                               "Lb2DmuPrescale",
                               "Lb2DsmuPrescale",
                               "Lb2LcpiPrescale",
                               ### B(s)0 -> Lc+ mu- 
                               "B2LcmuPrescale",
                               "B2DpiPrescale",
                               "Bs2DspiPrescale",
                               "B2LcpPrescale", ) 

    #######################################################
    
    def __init__(self, name = "BLV", config = default_config) :

        LineBuilder.__init__(self, name, config)

#######################################################################################################

        ### TrackCuts
        #############
        self.TrackCuts = """
                         ( MIPCHI2DV(PRIMARY) > %(MinTrIPChi2)s )
                         & ( TRCHI2DOF < %(MaxTrChi2Dof)s )
                         & ( TRGHP < %(MaxTrGhp)s )
                         """ %config

        ### DaughterCuts
        ################
        self.DCut  = "(ADMASS('D+') < %(mDiffD)s*MeV)" %config
        self.DsCut = "(ADMASS('D_s+') < %(mDiffDs)s*MeV)" %config

        ### CombinationCuts
        ###################
        self.DocaCut  = "(AMAXDOCA('') < %(MaxDoca)s*mm)" %config
        self.bCoCutS  = "(ADAMASS(5323*MeV) < %(mDiffbStand)s*MeV)" %config
        self.BdCoCutS = "(ADAMASS('B0') < %(mDiffbStand)s*MeV)" %config
        self.BsCoCutS = "(ADAMASS('B_s0') < %(mDiffbStand)s*MeV)" %config
        self.LbCoCutL = "(ADAMASS('Lambda_b0') < %(mDiffbLoose)s*MeV)" %config        
        self.LbCoCutS = "(ADAMASS('Lambda_b0') < %(mDiffbStand)s*MeV)" %config
        self.LbCoCutT = "(ADAMASS('Lambda_b0') < %(mDiffbTight)s*MeV)" %config

        ### MotherCuts
        ################
        self.bCutS   = "(ADMASS(5323*MeV) < %(mDiffbStand)s*MeV)" %config
        self.BdCutS  = "(ADMASS('B0') < %(mDiffbStand)s*MeV)" %config
        self.BuCutT  = "(ADMASS('B+') < %(mDiffbTight)s*MeV)" %config      
        self.BsCutS  = "(ADMASS('B_s0') < %(mDiffbStand)s*MeV)" %config
        self.LbCutL  = "(ADMASS('Lambda_b0') < %(mDiffbLoose)s*MeV)" %config
        self.LbCutS  = "(ADMASS('Lambda_b0') < %(mDiffbStand)s*MeV)" %config
        self.LbCutT  = "(ADMASS('Lambda_b0') < %(mDiffbTight)s*MeV)" %config
        self.XibCutL = "(ADMASS('Xi_b-') < %(mDiffbLoose)s*MeV )" %config

        self.bMotherCutsLoose = """
                                (BPVIPCHI2() < %(MaxIPChi2)s)
                                & (BPVVDCHI2 > %(MinVDChi2Loose)s)
                                & (VFASPF(VCHI2/VDOF) < %(MaxVtxChi2Dof)s)
                                & (BPVDIRA > %(MinDira)s)
                                & (BPVLTIME() > %(MinTauLoose)s*ps)
                                """ %config
        
        self.bMotherCutsTight = """
                                (BPVIPCHI2() < %(MaxIPChi2)s)
                                & (BPVVDCHI2 > %(MinVDChi2Tight)s)
                                & (VFASPF(VCHI2/VDOF) < %(MaxVtxChi2Dof)s)
                                & (BPVDIRA > %(MinDira)s)
                                & (BPVLTIME() > %(MinTauTight)s*ps)
                                """ %config

        ### Lambda0 -> K- mu+
        #####################
        self.La2KmuCombinationCut = """
                                    (ADAMASS('Lambda0') < %(mDiffLa)s*MeV)
                                    & (AMAXDOCA('') < %(MaxDoca)s*mm)
                                    """ %config
        
        self.La2KmuMotherCut = """
                               (ADMASS('Lambda0') < %(mDiffLa)s*MeV)
                               & (BPVIPCHI2() < %(MaxIPChi2)s)
                               & (BPVVDCHI2 > %(MinVDChi2Tight)s)
                               & (VFASPF(VCHI2/VDOF) < %(MaxVtxChi2Dof)s)
                               & (BPVDIRA > %(MinDira)s)
                               & (BPVLTIME() > %(MinTauLambda)s*ps)
                               """ %config

        ### Lambda_b0 -> K- mu+
        #######################
        self.Lb2KmuCombinationCut = self.LbCoCutL + " & " + self.DocaCut
        self.Lb2KmuMotherCut = self.LbCutL + " & " + self.bMotherCutsLoose

        ### Xi_b- -> K*0 mu-
        ####################

        self.Xib2KhmuCombination12Cut = """
                                         (AM12 < %(mKst)s*MeV)
                                         & (ADOCA(1,2) < %(MaxDoca)s*mm)
                                         """ %config

        self.Xib2KhmuCombinationCut = """
                                      (ADOCA(1,3) < %(MaxDoca)s*mm)
                                      & (ADOCA(2,3) < %(MaxDoca)s*mm)
                                      """ %config
        
        self.Xib2KhmuMotherCut = self.XibCutL + " & " + self.bMotherCutsLoose

        ###### B+ -> K*0 pi+
        ####################
        self.Bu2KhhMotherCut = self.BuCutT + " & " + self.bMotherCutsLoose                

        
        ### Lb -> D(s)+ mu- and Lb -> Lc+ pi-
        #####################################
        self.Lb2DmuCombinationCut = self.LbCoCutS + " & " + self.DocaCut
        self.Lb2DmuMotherCut      = self.LbCutS + " & " + self.bMotherCutsTight

        ### B(s)0 -> LambdaC+ mu-
        #########################
        self.B2LcmuCombinationCut = self.bCoCutS + " & " + self.DocaCut
        self.B2LcmuMotherCut      = self.bCutS + " & " + self.bMotherCutsTight

        ###### B0 -> D- pi+
        ###################
        self.Bd2DpiCombinationCut = self.BdCoCutS + " & " + self.DocaCut
        self.Bd2DpiMotherCut      = self.BdCutS + " & " + self.bMotherCutsTight

        ###### B_s0 -> D_s- pi+
        #######################        
        self.Bs2DspiCombinationCut = self.BsCoCutS + " & " + self.DocaCut
        self.Bs2DspiMotherCut      = self.BsCutS + " & " + self.bMotherCutsTight      

#######################################################################################################

        ### Lambda0 -> K- mu+
        La2Kmu_name      = name+"La2Kmu"
        self.selLa2Kmu   = self.makeLa2Kmu(La2Kmu_name)      

        ### Lambda_b0 -> K- mu+
        Lb2Kmu_name      = name+"Lb2Kmu"
        self.selLb2Kmu   = self.makeLb2Kmu(Lb2Kmu_name)

        ### Xi_b- -> K*0 mu- 
        Xib2Khmu_name    = name+"Xib2Khmu"         
        self.selXib2Khmu = self.makeXib2Khmu(Xib2Khmu_name)
        B2Khh_name     = name+"B2Khh" 
        self.selB2Khh  = self.makeB2Khh(B2Khh_name) 
        
        ### Lb -> D(s)+ mu-         
        Lb2Dmu_name      = name+"Lb2Dmu"
        self.selLb2Dmu   = self.makeLb2Dmu(Lb2Dmu_name)
        Lb2Dsmu_name     = name+"Lb2Dsmu"
        self.selLb2Dsmu  = self.makeLb2Dsmu(Lb2Dsmu_name)
        Lb2Lcpi_name     = name+"Lb2Lcpi"
        self.selLb2Lcpi  = self.makeLb2Lcpi(Lb2Lcpi_name)

        ### B(s)0 -> Lc+ mu- 
        B2Lcmu_name      = name+"B2Lcmu"
        self.selB2Lcmu   = self.makeB2Lcmu(B2Lcmu_name)
        B2Dpi_name       = name+"B2Dpi"
        self.selB2Dpi    = self.makeB2Dpi(B2Dpi_name)
        Bs2Dspi_name     = name+"Bs2Dspi"
        self.selBs2Dspi  = self.makeBs2Dspi(Bs2Dspi_name)
        B2Lcp_name       = name+"B2Lcp"
        self.selB2Lcp    = self.makeB2Lcp(B2Lcp_name) 
        
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

####################################################################################################### 
### Xi_b- -> K*0 mu-
####################################################################################################### 

        self.Xib2KhmuLine = StrippingLine(Xib2Khmu_name+"Line",
                                          prescale = config["Xib2KhmuPrescale"],
                                          postscale = config["Postscale"],
                                          MDSTFlag = True,
                                          selection = self.selXib2Khmu,
                                          RelatedInfoTools = [                                        
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selXib2Khmu : "ConeIso05Xib",
                                                              "Phys/StdAllLooseKaons"  : "ConeIso05K",
                                                              "Phys/StdAllNoPIDsPions" : "ConeIso05pi",
                                                              "Phys/StdAllLooseMuons"  : "ConeIso05mu", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selXib2Khmu : "ConeIso10Xib",
                                                              "Phys/StdAllLooseKaons"  : "ConeIso10K",
                                                              "Phys/StdAllNoPIDsPions" : "ConeIso10pi",
                                                              "Phys/StdAllLooseMuons"  : "ConeIso10mu", }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selXib2Khmu : "ConeIso15Xib",
                                                              "Phys/StdAllLooseKaons"  : "ConeIso15K",
                                                              "Phys/StdAllNoPIDsPions" : "ConeIso15pi",
                                                              "Phys/StdAllLooseMuons"  : "ConeIso15mu", }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 1,
                                             "Variables" : 0,
                                             "Locations" : { "Phys/StdAllLooseKaons"  : "TrackIsoBDTK",
                                                             "Phys/StdAllNoPIDsPions" : "TrackIsoBDTpi", 
                                                             "Phys/StdAllLooseMuons"  : "TrackIsoBDTmu", }, },
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 1,
                                             "Variables"  : [],
                                             "IsoTwoBody" : True,
                                             "Locations"  : { "Phys/StdAllLooseKaons"  : "BsMuMuTrackIsoK",
                                                              "Phys/StdAllNoPIDsPions" : "BsMuMuTrackIsopi",
                                                              "Phys/StdAllLooseMuons"  : "BsMuMuTrackIsomu", }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                          ) # closes Strippingline 


        self.B2KhhLine = StrippingLine(B2Khh_name+"Line",
                                       prescale = config["B2KhhPrescale"],
                                       postscale = config["Postscale"],
                                       MDSTFlag = True,
                                       selection = self.selB2Khh,
                                       RelatedInfoTools = [ 
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 0.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB2Khh : "ConeIso05Bp",
                                                              "Phys/StdAllLooseKaons"  : "ConeIso05K",
                                                              "Phys/StdAllNoPIDsPions" : ["ConeIso05pi1","ConeIso05pi2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.0,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB2Khh : "ConeIso10Bp",
                                                              "Phys/StdAllLooseKaons"  : "ConeIso10K",
                                                              "Phys/StdAllNoPIDsPions" : ["ConeIso10pi1","ConeIso10pi2"], }, },
                                           { "Type" : "RelInfoConeVariables",
                                             "ConeAngle" : 1.5,
                                             "Variables" : [],
                                             "RecursionLevel" : 1,
                                             "Locations"  : { self.selB2Khh : "ConeIso15Bp",
                                                              "Phys/StdAllLooseKaons"  : "ConeIso15K",
                                                              "Phys/StdAllNoPIDsPions" : ["ConeIso15pi1","ConeIso15pi2"], }, },
                                           { "Type": "RelInfoTrackIsolationBDT",
                                             "RecursionLevel" : 1,
                                             "Variables" : 0,
                                             "Locations" : { "Phys/StdAllLooseKaons"  : "TrackIsoBDTK",
                                                             "Phys/StdAllNoPIDsPions" : ["TrackIsoBDTpi1","TrackIsoBDTpi2"], }, }, 
                                           { "Type" : "RelInfoBs2MuMuTrackIsolations",
                                             "RecursionLevel" : 1,
                                             "Variables"  : [],
                                             "IsoTwoBody" : True,
                                             "Locations"  : { "Phys/StdAllLooseKaons"  : "BsMuMuTrackIsoK",
                                                              "Phys/StdAllNoPIDsPions" :
                                                              ["BsMuMuTrackIsopi1","BsMuMuTrackIsopi2"], }, },
                                           ] + config["CommonRelInfoTools"] # end of RelatedInfoTools 
                                         ) # closes Strippingline 

####################################################################################################### 
### Lb -> D(s)+ mu-
####################################################################################################### 
 
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

####################################################################################################### 
### B(s)0 -> Lc+ mu-
####################################################################################################### 

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
                                                             "Phys/StdAllLoosePions" :
                                                             ["BsMuMuTrackIsopi1","BsMuMuTrackIsopi2"], }, },
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

        ### Lambda_(b)0 -> K- mu+ 
        self.registerLine(self.La2KmuLine)
        self.registerLine(self.Lb2KmuLine)

        ### Xi_b- -> K*0 mu-
        self.registerLine(self.Xib2KhmuLine) 
        self.registerLine(self.B2KhhLine)

        ### Lb -> D(s)+ mu-
        self.registerLine(self.Lb2DmuLine)
        self.registerLine(self.Lb2DsmuLine)
        self.registerLine(self.Lb2LcpiLine)

        ### B(s)0 -> Lc+ mu-
        self.registerLine(self.b2LcmuLine)
        self.registerLine(self.B2DpiLine)
        self.registerLine(self.Bs2DspiLine)        
        self.registerLine(self.b2LcpLine)

#######################################################################################################

    def makeLa2Kmu(self,name):
    
        La2Kmu = CombineParticles("Combine"+name)
        La2Kmu.DecayDescriptors = [ "[Lambda0 -> K+ mu-]cc","[Lambda0 -> K+ mu+]cc" ]
        La2Kmu.DaughtersCuts = { "K+" : self.TrackCuts + " & ((PIDK-PIDpi)>5)" + " & ((PIDK-PIDp)>0)",
                                 "mu-" : self.TrackCuts }
                             
        La2Kmu.CombinationCut = self.La2KmuCombinationCut  
        La2Kmu.MotherCut      = self.La2KmuMotherCut

        _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
        _myKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")
    
        return Selection (name, Algorithm = La2Kmu, RequiredSelections = [ _myKaons, _myMuons ])

#######################################################################################################

    def makeLb2Kmu(self,name):
    
        Lb2Kmu = CombineParticles("Combine"+name)
        Lb2Kmu.DecayDescriptors = [ "[Lambda_b0 -> K+ mu-]cc","[Lambda_b0 -> K+ mu+]cc" ]
        Lb2Kmu.DaughtersCuts = { "K+" : self.TrackCuts + " & ((PIDK-PIDpi)>0)",
                                 "mu+" : self.TrackCuts }

        Lb2Kmu.CombinationCut = self.Lb2KmuCombinationCut
        Lb2Kmu.MotherCut      = self.Lb2KmuMotherCut
 
        _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
        _myKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")
    
        return Selection (name, Algorithm = Lb2Kmu, RequiredSelections = [ _myKaons, _myMuons ])

#######################################################################################################
### Xi_b- -> K*0 mu-
####################################################################################################### 

    def makeXib2Khmu(self,name):
    
        Xib2Khmu = DaVinci__N3BodyDecays("Combine"+name)
        Xib2Khmu.DecayDescriptors = [ "[Xi_b- -> K+ pi- mu-]cc","[Xi_b- -> K- pi+ mu-]cc","[Xi_b- -> K- pi- mu+]cc" ]
        Xib2Khmu.DaughtersCuts = { "K+"  : self.TrackCuts + " & ((PIDK-PIDpi)>0)",
                                   "pi-" : self.TrackCuts,
                                   "mu-" : self.TrackCuts }

        Xib2Khmu.Combination12Cut = self.Xib2KhmuCombination12Cut
        Xib2Khmu.CombinationCut   = self.Xib2KhmuCombinationCut
        Xib2Khmu.MotherCut        = self.Xib2KhmuMotherCut
 
        _myKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")
        _myPions = DataOnDemand(Location = "Phys/StdNoPIDsPions/Particles")
        _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")    
    
        return Selection (name, Algorithm = Xib2Khmu, RequiredSelections = [ _myKaons, _myPions, _myMuons ])

#######################################################################################################

    def makeB2Khh(self,name):
    
        B2Khh = DaVinci__N3BodyDecays("Combine"+name)
        B2Khh.DecayDescriptors = [ "[B+ -> K+ pi- pi+]cc" ]
        B2Khh.DaughtersCuts = { "K+"  : self.TrackCuts + " & ((PIDK-PIDpi)>0)",
                                "pi-" : self.TrackCuts }

        B2Khh.Combination12Cut = self.Xib2KhmuCombination12Cut
        B2Khh.CombinationCut   = self.Xib2KhmuCombinationCut
        B2Khh.MotherCut        = self.Bu2KhhMotherCut
    
        _myKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")
        _myPions = DataOnDemand(Location = "Phys/StdNoPIDsPions/Particles")    
    
        return Selection (name, Algorithm = B2Khh, RequiredSelections = [ _myKaons, _myPions ])

####################################################################################################### 
### Lb -> D(s)+ mu- 
####################################################################################################### 

    def makeLb2Dmu(self,name):
    
        Lb2Dmu = CombineParticles("Combine"+name)
        Lb2Dmu.DecayDescriptors = [ "[Lambda_b0 -> D+ mu-]cc","[Lambda_b0 -> D+ mu+]cc" ]
        Lb2Dmu.DaughtersCuts = { "D+" : self.DCut, "mu-" : self.TrackCuts }

        Lb2Dmu.CombinationCut = self.Lb2DmuCombinationCut
        Lb2Dmu.MotherCut      = self.Lb2DmuMotherCut

        _myDplus = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi/Particles")
        _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
   
        return Selection (name, Algorithm = Lb2Dmu, RequiredSelections = [ _myDplus, _myMuons ])
 
#######################################################################################################

    def makeLb2Dsmu(self,name):
    
        Lb2Dsmu = CombineParticles("Combine"+name)
        Lb2Dsmu.DecayDescriptors = [ "[Lambda_b0 -> D_s+ mu-]cc","[Lambda_b0 -> D_s+ mu+]cc" ]
        Lb2Dsmu.DaughtersCuts = { "D_s+" : self.DsCut, "mu-" : self.TrackCuts } 

        Lb2Dsmu.CombinationCut = self.Lb2DmuCombinationCut
        Lb2Dsmu.MotherCut      = self.Lb2DmuMotherCut

        _myDsplus = DataOnDemand(Location = "Phys/StdLooseDsplus2KKPi/Particles")
        _myMuons  = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
        return Selection (name, Algorithm = Lb2Dsmu, RequiredSelections = [ _myDsplus, _myMuons ])

#######################################################################################################

    def makeLb2Lcpi(self,name):
    
        Lb2Lcpi = CombineParticles("Combine"+name)
        Lb2Lcpi.DecayDescriptors = [ "[Lambda_b0 -> Lambda_c+ pi-]cc","[Lambda_b0 -> Lambda_c+ pi+]cc" ]
        Lb2Lcpi.DaughtersCuts = { "pi-" : self.TrackCuts }

        Lb2Lcpi.CombinationCut = self.Lb2DmuCombinationCut
        Lb2Lcpi.MotherCut      = self.Lb2DmuMotherCut    
 
        _myLambdaC = DataOnDemand(Location = "Phys/StdLooseLambdac2PKPi/Particles")
        _myPions   = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    
        return Selection (name, Algorithm = Lb2Lcpi, RequiredSelections = [ _myLambdaC, _myPions ])

####################################################################################################### 
### B(s)0 -> Lc+ mu-
####################################################################################################### 

    def makeB2Lcmu(self,name):
    
        B2Lcmu = CombineParticles("Combine"+name)
        B2Lcmu.DecayDescriptors = [ "[B0 -> Lambda_c+ mu-]cc","[B0 -> Lambda_c+ mu+]cc" ]
        B2Lcmu.DaughtersCuts = { "mu-" : self.TrackCuts }

        B2Lcmu.CombinationCut = self.B2LcmuCombinationCut
        B2Lcmu.MotherCut      = self.B2LcmuMotherCut

        _myLambdaC = DataOnDemand(Location = "Phys/StdLooseLambdac2PKPi/Particles")
        _myMuons   = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
        return Selection (name, Algorithm = B2Lcmu, RequiredSelections = [ _myLambdaC, _myMuons ])

#######################################################################################################

    def makeB2Lcp(self,name):

        B2Lcp = CombineParticles("Combine"+name)
        B2Lcp.DecayDescriptors = [ "[B0 -> Lambda_c+ p~-]cc","[B0 -> Lambda_c+ p+]cc" ]
        B2Lcp.DaughtersCuts = { "p~-" : self.TrackCuts + "& (PIDp>5)"}

        B2Lcp.CombinationCut = self.B2LcmuCombinationCut
        B2Lcp.MotherCut      = self.B2LcmuMotherCut

        _myLambdaC = DataOnDemand(Location = "Phys/StdLooseLambdac2PKPi/Particles")
        _myProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")

        return Selection (name, Algorithm = B2Lcp, RequiredSelections = [ _myLambdaC, _myProtons ])

#######################################################################################################

    def makeB2Dpi(self,name):
    
        B2Dpi = CombineParticles("Combine"+name)
        B2Dpi.DecayDescriptors = [ "[B0 -> D- pi+]cc" ]
        B2Dpi.DaughtersCuts = { "pi+" : self.TrackCuts, "D-" : self.DCut }

        B2Dpi.CombinationCut = self.Bd2DpiCombinationCut
        B2Dpi.MotherCut      = self.Bd2DpiMotherCut

        _myDplus = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi/Particles")
        _myPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    
        return Selection (name, Algorithm = B2Dpi, RequiredSelections = [ _myDplus, _myPions ])

#######################################################################################################

    def makeBs2Dspi(self,name):
    
        Bs2Dspi = CombineParticles("Combine"+name)
        Bs2Dspi.DecayDescriptors = [ "[B_s0 -> D_s- pi+]cc" ]
        Bs2Dspi.DaughtersCuts = { "pi+" : self.TrackCuts, "D_s-" : self.DsCut }
    
        Bs2Dspi.CombinationCut = self.Bs2DspiCombinationCut
        Bs2Dspi.MotherCut      = self.Bs2DspiMotherCut

        _myDsplus = DataOnDemand(Location = "Phys/StdLooseDsplus2KKPi/Particles")
        _myPions  = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    
        return Selection (name, Algorithm = Bs2Dspi, RequiredSelections = [ _myDsplus, _myPions ])

#######################################################################################################
#######################################################################################################
