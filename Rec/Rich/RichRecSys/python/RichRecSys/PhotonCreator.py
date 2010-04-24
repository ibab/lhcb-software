
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
                                              "HLT"     : [True,True,True] } )
        self.setRichDefaults ( "MaxPhotons", { "Offline" : 1000000, 
                                               "HLT"     : 1000000 } )

    ## Apply configurations
    def applyConf(self):

        from Configurables import RichTools

        # First data options. To be removed eventually...
        if "earlyData" in self.getProp("SpecialData") :
            if not self.getProp("Simulation") :
                self.richTools().photonReco().CKThetaQuartzRefractCorrections = [ -0.00325,0,0 ]

        # Context
        context = self.getProp("Context")

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

        if selMode == "Tight" :
            
            # ================================================================
            # Photon Predictor cuts
            # ================================================================
            # No # sigma cut
            creator.Predictor.NSigma         = [ 25,   8,   12  ]
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
            creator.NSigma                   = [ 15,    6.5,   5.5   ]
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
                      
        else:
            
            raise RuntimeError("ERROR : Unknown selection mode '%s'"%selMode)
