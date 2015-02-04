#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: $
# =============================================================================
## @file
#  Configuration of Hlt Reconstruction Algorithms
#  @author Sebastian Neubert, sebastian.neubert@cern.ch
#  @date 2014-11-24
# =============================================================================
"""
 Parameters for the HLT reconstruction algorithms
 Note: the actual definition of the tracking sequences can be found in
 HltSharedTracking.py
 Hlt1Tracking.py
 and
 Hlt2Tracking.py
 Line-specific parameters should go into the respectiv HltLine configurables
"""
# =============================================================================
__author__  = "Sebastian Neubert, sebastian.neubert@cern.ch"
__version__ = ""
# =============================================================================

from Gaudi.Configuration import * 
from LHCbKernel.Configuration import *

from GaudiKernel.SystemOfUnits import MeV

class HltRecoConf(LHCbConfigurableUser):
   __slots__ = { "Forward_HPT_MinPt"            : 500. * MeV
                 ,"Forward_HPT_MinP"            : 3000. * MeV
                 ,"Forward_LPT_MinPt"           : 80. * MeV  
                 ,"Forward_LPT_MinP"            : 1000. * MeV 
                 ,"Forward_MaxOTHits"           : 15000
                 ,"MatchVeloMuon_MinP"          : 6000. * MeV
                 ,"GoodTrCHI2PDOF"              : 5.0
                 ,"VeloSelectionCut"            : "(~TrBACKWARD) & ( TrNVELOMISS < 100 )"
                 ,"InitFits"                    : False
                 ,"SimplifiedMaterialFit"       : False
               }

   def getConfiguredForward(Name) :
      
      pass

   def __apply_configuration__(self):
        """
        Apply Hlt configuration
        """
        log.debug("Hlt configuration")
        import GaudiKernel.ProcessJobOptions
        GaudiKernel.ProcessJobOptions.PrintOff()


MaxOTHits = 15000 #--> move to CommonForwardOptions, this one is used e.g. in Hlt1GECs.py or Hlt1BeamGasLines.py

CommonMatchVeloMuonOptions = {"MaxChi2DoFX" : 10.,
                              "XWindow" : 200.,
                              "YWindow" : 200.  }

VeloTTToolOptions = {"minMomentum" : 0.0,
                     "minPT" : 100.0 ,
                     "PassTracks" : True,
                     }
                          
VeloTTOptions = { "fitTracks" : False }

CommonForwardOptions = { "MaxOTHits" : 150000 , #Why do we have this three times?
                         "MaxITHits" : 999999 ,
                         "MaxNVelo" : 999999 }

#Sascha: Rename this to tool,
CommonForwardTrackingOptions = { "MaxChi2" : 40.,
                                 "MaxChi2Track" : 40.,
                                 "MinHits" : 12,  
                                 "MinOTHits" : 14  }

#Sascha: Rename this to tool and hlt1
ForwardTrackingOptions_MomEstimate = { "UseMomentumEstimate" : True,
                                       "UseWrongSignWindow" : True,
                                       "WrongSignPT" : 2000.,
                                       "Preselection" : True,
                                       "PreselectionPT" : 400.,
                                       "AddTTClusterName" : "PatAddTTCoord", # needs fix...
                                       "SecondLoop" : False}

ComplementForwardToolOptions = { "AddTTClusterName" : "PatAddTTCoord"
                                 ,"SecondLoop" : False} # to be decided

OnlineSeedingToolOptions = { "NDblOTHitsInXSearch" : 2,
                             "MinMomentum" : 3000.
                             #"UseForward" : True , # Flag hits from Forward tracks
                             }

# for now synchronize forward and matching
MatchToolOptions = { "MinMomentum": HltRecoConf().getProp("Forward_LPT_MinP"),
                     "MinPt": HltRecoConf().getProp("Forward_LPT_MinPt")}

DownstreamOptions = { "MinMomentum": 0.,
                      "MinPt": 0.}

## CommonForwardTrackingOptions_EarlyData = { "MaxChi2" : 40., 
##                                  "MaxChi2Track" : 40, 
##                                  "MinHits" : 12,  
##                                  "MinOTHits" : 14,
##                                  "MinXPlanes" : 4,
##                                  "MinPlanes" : 8,
##                                  "MaxSpreadX" : 1.5,
##                                  "MaxSpreadY" : 3.0  }
## CommonSeedingTrackingOptions_EarlyData = { "OTNHitsLowThresh" : 12,
##                                            "MinTotalPlanes" : 7,
##                                            "MaxMisses" : 2,
##                                            "MaxTrackChi2LowMult" : 10,
##                                            "MaxFinalTrackChi2" : 20,
##                                            "MaxFinalChi2" : 30,
##                                            "MaxTrackChi2" : 40,
##                                            "MaxChi2HitIT" : 10,
##                                            "MaxChi2HitOT" : 30}    
## CommonDownstreamTrackingOptions_EarlyData = { "xPredTol2" : 20.0,
##                                               "TolMatch" : 1.5,
##                                               "TolUV" : 2.0,
##                                               "maxWindowSize" : 10.0,
##                                               "MaxChisq" : 20.0,
##                                               "MaxDistance" : 0.3,
##                                               "deltaP" : 2.0,
##                                               "errorZMagnet" : 30.0 } 



   
   
