
## @package PhotonCreator
#  Photon creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: PhotonCreator.py,v 1.4 2008-10-21 19:26:23 jonrob Exp $"
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
        "context": "Offline" # The context within which to run
       ,"radiators":  [] # The radiators to use (Aerogel/Rich1Gas/Rich2Gas)
       ,"selectionMode" : ""
        }

    ## Known selection modes
    KnowwnSelectionModes =  [ "Tight", "Loose", "All" ]

        ## Initialise 
    def initialise(self):
        # default values
        self.setRichDefault("radiators","Offline",[True,True,True])
        self.setRichDefault("radiators","HLT",    [True,True,True])

    ## Apply configurations
    def applyConf(self):

        context = self.getProp("context")
      
        # -----------------------------------------------------------------------
        # Photon maker

        creator = RichTools().photonCreator()

        # -----------------------------------------------------------------------
        # Photon predictor (a.k.a. pre-selection)

        pred = RichTools().photonPredictor()

        # Configure creator with the correct predictor
        creator.addTool( pred, name = "Predictor" )

        # -----------------------------------------------------------------------
        # Photon reconstruction (i.e. calculating the CK theta/phi values)

        photonreco_nickname = "PhotonParams"
        reco = RichTools().photonReco(photonreco_nickname)

        # Configure creator with the correct reco tool
        creator.addTool( reco, name = photonreco_nickname )

        # -----------------------------------------------------------------------

        # Photon selection criteria
        if self.getProp("selectionMode") == "" : self.setProp("selectionMode","Tight")
        if self.getProp("selectionMode") == "Tight" :
            # DO nothing as yet
            None
        elif self.getProp("selectionMode") == "Loose" :
            # Options to be defined ...
            raise RuntimeError("Photon selection mode Loose not yet defined. Bug CRJ")
        elif self.getProp("selectionMode") == "All" :
            creator.MaxAllowedCherenkovTheta = [ 99999, 99999, 99999 ]
            creator.MinAllowedCherenkovTheta = [ 0,     0,     0     ]
            creator.NSigma                   = [ 99999, 99999, 99999 ]
            creator.MinPhotonProbability     = [ 0,     0,     0     ]
        else:
            raise RuntimeError("ERROR : Unknown selection mode '%s'"%self.getProp("selectionMode"))
