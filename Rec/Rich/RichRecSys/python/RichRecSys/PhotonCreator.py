
## @package PhotonCreator
#  Photon creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: PhotonCreator.py,v 1.5 2008-11-14 17:14:05 jonrob Exp $"
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
        }

    ## Known selection modes
    KnowwnSelectionModes =  [ "Tight", "Loose", "All" ]

    ## Initialize 
    def initialize(self):
        # default values
        self.setRichDefault("Radiators","Offline",[True,True,True])
        self.setRichDefault("Radiators","HLT",    [True,True,True])

    ## Apply configurations
    def applyConf(self):

        context = self.getProp("Context")
      
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
        if self.getProp("SelectionMode") == "" : self.setProp("SelectionMode","Tight")
        selMode = self.getProp("SelectionMode")
        if selMode == "Tight" :
            # DO nothing as yet
            pass
        elif selMode == "Loose" :
            # Options to be defined ...
            raise RuntimeError("Photon selection mode Loose not yet defined. Bug CRJ")
        elif selMode == "All" :
            creator.MaxAllowedCherenkovTheta = [ 99999, 99999, 99999 ]
            creator.MinAllowedCherenkovTheta = [ 0,     0,     0     ]
            creator.NSigma                   = [ 99999, 99999, 99999 ]
            creator.MinPhotonProbability     = [ 0,     0,     0     ]
        else:
            raise RuntimeError("ERROR : Unknown selection mode '%s'"%selMode)
