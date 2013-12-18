
## @package PhotonCreator
#  Photon creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: PhotonCreator.py,v 1.12 2010-02-12 16:47:26 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *

# ----------------------------------------------------------------------------------

## @class RichPhotonCreatorConfig
#  Photon creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichPhotonCreatorConfig(RichConfigurableUser):

    ## Default options
    __slots__ = {
        "Context"       : "Offline" # The context within which to run
       ,"DataType"      : ""        # Type of data, propagated from application
       ,"Radiators"     : []        # The radiators to use (Aerogel/Rich1Gas/Rich2Gas)
       ,"SelectionMode" : "Tight"
       ,"SpecialData"   : []
       ,"Simulation"    : False     # Simulated data
       ,"OutputLevel"   : INFO      # The output level to set all algorithms and tools to use
       ,"MaxPhotons"    : None      # Maximum number of photon candidates
        }

    ## Initialize 
    def initialize(self):
        # default values
        self.setRichDefaults ( "Radiators", { "Offline" : [True,True,True], 
                                              "HLT"     : [False,True,True] } )
        self.setRichDefaults ( "MaxPhotons", { "Offline" : 250000, 
                                               "HLT"     : 250000 } )

    ## @brief Apply any tweaks to the default configuration that vary by DataType
    def dataTypeTweaks(self):

        if not self.getProp("Simulation") :
            self.richTools().photonReco().CKThetaQuartzRefractCorrections = [ 0,0,0 ]

        # Get the DataType
        dataType = self.getProp("DataType")

        ## if dataType == "Upgrade" :
        ##     # Shouldn't be needed really. Needs some tuning for the upgrade
        ##     if not self.isPropertySet("SelectionMode") :
        ##         self.setProp("SelectionMode","All")

        # Relaxed GECs for pA data, tuning for 2013 Reco14r1 
        if "pA" in self.getProp("SpecialData"):
            if not self.isPropertySet("MaxPhotons") : self.setProp("MaxPhotons", 900000)

    ## Apply configurations
    def applyConf(self):

        # DataType specific tweaks
        self.dataTypeTweaks()

        # Context
        context = self.getProp("Context")
        # DataType
        dType = self.getProp("DataType")

        # Photon selection criteria
        selMode = self.getProp("SelectionMode")
        
        # -----------------------------------------------------------------------
        # Photon maker
        creator = self.richTools().photonCreator()

        # OutputLevel
        self.setOutputLevel(creator)

        # Number photon candidates
        creator.MaxPhotons = self.getProp("MaxPhotons")

        # -----------------------------------------------------------------------
        # Photon predictor (a.k.a. pre-selection)

        # Configure creator with the correct predictor
        creator.addTool( self.richTools().photonPredictor(), name = "Predictor" )
 
        # -----------------------------------------------------------------------
        # Photon reconstruction (i.e. calculating the CK theta/phi values)

        # Configure creator with the correct reco tool
        creator.addTool( self.richTools().photonReco(), name = "PhotonParams" )

        # -----------------------------------------------------------------------

        if selMode == "Tight-DEV" :

            # Change CK resolution calculation parameters
            # Will be committed to C++ in a future release
            self.richTools().CkResType = "Functional" # Default, but to be sure
            self.richTools().ckResolution().ScaleFactor   = [ 3.5,  1.5,    1.25   ]
            self.richTools().ckResolution().MaxCKThetaRes = [ 0.01, 0.0035, 0.0015 ]
            
            # ================================================================
            # Photon Predictor cuts
            # ================================================================
            # No # sigma cut 
            creator.Predictor.NSigma         = [  5,      4,     8    ]
            # Min and max search window for track centre - hit seperation
            creator.Predictor.MinTrackROI    = [  230,    0,     0    ]
            creator.Predictor.MaxTrackROI    = [  540,   86,    165   ]
            # ================================================================

            # ================================================================
            # Photon Creator cuts
            # ================================================================
            # Allow all photon CK theta values
            creator.MinAllowedCherenkovTheta = [ 0.200, 0.005, 0.005 ]
            creator.MaxAllowedCherenkovTheta = [ 0.310, 0.075, 0.035 ]
            # No # sigma cut
            creator.NSigma                   = [ 2.75,   3.5,  4.0   ]
            # Any probability
            creator.MinPhotonProbability     = [ 1e-15, 1e-15, 1e-15 ]
            # ================================================================

        elif selMode == "Tight" :
            
            # ================================================================
            # Photon Predictor cuts
            # ================================================================
            # No # sigma cut 
            creator.Predictor.NSigma         = [  17,     6,    10    ]
            # Min and max search window for track centre - hit seperation
            creator.Predictor.MinTrackROI    = [  230,    0,     0    ]
            creator.Predictor.MaxTrackROI    = [  540,   86,    165   ]
            # ================================================================

            # ================================================================
            # Photon Creator cuts
            # ================================================================
            # Allow all photon CK theta values
            creator.MinAllowedCherenkovTheta = [ 0.150, 0.005, 0.005 ]
            creator.MaxAllowedCherenkovTheta = [ 0.310, 0.075, 0.035 ]
            # No # sigma cut
            creator.NSigma                   = [ 9.0,   5.25,  5.0   ]
            # Any probability
            creator.MinPhotonProbability     = [ 1e-15, 1e-15, 1e-15 ]
            # ================================================================

        elif selMode == "Wide" :

            # ================================================================
            # Photon Predictor cuts
            # ================================================================
            # No # sigma cut 
            creator.Predictor.NSigma         = [  40,    35,     25    ]
            # Min and max search window for track centre - hit seperation
            creator.Predictor.MinTrackROI    = [  230,    0,     0    ]
            creator.Predictor.MaxTrackROI    = [  540,   90,    165   ]
            # ================================================================

            # ================================================================
            # Photon Creator cuts
            # ================================================================
            # Allow all photon CK theta values
            creator.MinAllowedCherenkovTheta = [ 0.150, 0.005, 0.005 ]
            creator.MaxAllowedCherenkovTheta = [ 0.340, 0.075, 0.035 ]
            # No # sigma cut
            creator.NSigma                   = [ 30,    25,   12   ]
            # Any probability
            creator.MinPhotonProbability     = [ 1e-15, 1e-15, 1e-15 ]
            # ================================================================
        
        elif selMode == "Loose" :

            # ================================================================
            # Photon Predictor cuts
            # ================================================================
            # Min and max search window for track centre - hit seperation
            creator.Predictor.MinTrackROI    = [ 0,     0,     0     ]
            creator.Predictor.MaxTrackROI    = [ 99999, 99999, 99999 ]
            # No # sigma cut
            creator.Predictor.NSigma         = [ 99999, 99999, 99999 ]
            # ================================================================

            # ================================================================
            # Photon Creator cuts
            # ================================================================
            # No # sigma cut
            creator.NSigma                   = [ 99999, 99999, 99999 ]
            # Any probability
            creator.MinPhotonProbability     = [ 0,     0,     0     ]
            # ================================================================
        
        elif selMode == "All" :

            # ================================================================
            # Photon Predictor cuts
            # ================================================================
            # Min and max search window for track centre - hit seperation
            creator.Predictor.MinTrackROI    = [ 0,     0,     0     ]
            creator.Predictor.MaxTrackROI    = [ 99999, 99999, 99999 ]
            # No # sigma cut
            creator.Predictor.NSigma         = [ 99999, 99999, 99999 ]
            # ================================================================

            # ================================================================
            # Photon Creator cuts
            # ================================================================
            # Allow all photon CK theta values
            creator.MinAllowedCherenkovTheta = [ 0,     0,     0     ]
            creator.MaxAllowedCherenkovTheta = [ 99999, 99999, 99999 ]
            # No # sigma cut
            creator.NSigma                   = [ 99999, 99999, 99999 ]
            # Any probability
            creator.MinPhotonProbability     = [ 0,     0,     0     ]
            # ================================================================

        elif selMode == "ParticleSearch" :

            # ================================================================
            # Photon Predictor cuts
            # ================================================================
            # Min and max search window for track centre - hit seperation
            creator.Predictor.MinTrackROI    = [ 0,     0,     0     ]
            creator.Predictor.MaxTrackROI    = [ 99999, 99999, 99999 ]
            # No # sigma cut
            creator.Predictor.NSigma         = [ 99999, 99999, 99999 ]
            # ================================================================

            # ================================================================
            # Photon Creator cuts
            # ================================================================
            # Allow all photon CK theta values
            creator.MinAllowedCherenkovTheta = [ 0.02,  0,     0     ]
            creator.MaxAllowedCherenkovTheta = [ 0.40,  0.070,  0.045]
            # No # sigma cut
            creator.NSigma                   = [ 99999, 99999, 99999 ]
            # Any probability
            creator.MinPhotonProbability     = [ 0,     0,     0     ]
            # ================================================================
                      
        else:
            
            raise RuntimeError("ERROR : Unknown selection mode '%s'"%selMode)
