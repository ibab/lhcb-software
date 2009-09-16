
## @package PhotonCreator
#  Photon creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: PhotonCreator.py,v 1.9 2009-09-16 13:37:28 jonrob Exp $"
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
        "Context": "Offline" # The context within which to run
       ,"Radiators":  [] # The radiators to use (Aerogel/Rich1Gas/Rich2Gas)
       ,"SelectionMode" : ""
       ,"OutputLevel"   : INFO    # The output level to set all algorithms and tools to use
        }

## @brief Set OutputLevel 
    def setOutputLevel(self,conponent):
        if self.isPropertySet("OutputLevel") :
            conponent.OutputLevel = self.getProp("OutputLevel")

    ## Initialize 
    def initialize(self):
        # default values
        self.setRichDefault("Radiators","Offline",[True,True,True])
        self.setRichDefault("Radiators","HLT",    [True,True,True])

    ## Apply configurations
    def applyConf(self):

        # Context
        context = self.getProp("Context")

        # Photon selection criteria
        selMode = self.getProp("SelectionMode")
        if selMode == "" : selMode = "Tight"
        
        # -----------------------------------------------------------------------
        # Photon maker
        creator = RichTools().photonCreator()

        # OutputLevel
        self.setOutputLevel(creator)

        # -----------------------------------------------------------------------
        # Photon predictor (a.k.a. pre-selection)

        # Configure creator with the correct predictor
        creator.addTool( RichTools().photonPredictor(), name = "Predictor" )
 
        # -----------------------------------------------------------------------
        # Photon reconstruction (i.e. calculating the CK theta/phi values)

        photonreco_nickname = "PhotonParams"
        reco = RichTools().photonReco(photonreco_nickname)

        # Configure creator with the correct reco tool
        creator.addTool( reco, name = photonreco_nickname )

        # -----------------------------------------------------------------------

        if selMode == "Tight" :
            
            # Default cuts are 'tight', so do nothing
            pass
        
        elif selMode == "Loose" :
            
            # ================================================================
            # Photon Predictor cuts
            # ================================================================
            # No # sigma cut
            creator.Predictor.NSigma         = [ 99999, 99999, 99999 ]
            # ================================================================

            # ================================================================
            # Photon Creator cuts
            # ================================================================
            # No # sigma cut
            creator.NSigma                   = [ 99999, 99999, 99999 ]
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
            creator.MaxAllowedCherenkovTheta = [ 99999, 99999, 99999 ]
            creator.MinAllowedCherenkovTheta = [ 0,     0,     0     ]
            # No # sigma cut
            creator.NSigma                   = [ 99999, 99999, 99999 ]
            # Any probability
            creator.MinPhotonProbability     = [ 0,     0,     0     ]
            # ================================================================
                      
        else:
            
            raise RuntimeError("ERROR : Unknown selection mode '%s'"%selMode)
