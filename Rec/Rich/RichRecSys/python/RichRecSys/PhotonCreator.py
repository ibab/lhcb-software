
## @package PhotonCreator
#  Photon creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: PhotonCreator.py,v 1.2 2008-08-15 14:41:23 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *

# ----------------------------------------------------------------------------------

## @class RichPhotonCreatorConfig
#  Photon creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichPhotonCreatorConfig(RichConfigurableUser):

    ## Default radiators to use for each context
    DefaultRadiators = { "Offline" : [True,True,True],
                         "HLT"     : [True,True,True] }

    ## Default options
    __slots__ = {
        "context": "Offline" # The context within which to run
       ,"radiators":  [] # The radiators to use (Aerogel/Rich1Gas/Rich2Gas)
        }

    ## Apply configurations
    def applyConf(self):

        context = self.getProp("context")

        # -----------------------------------------------------------------------
        # Check configuration
        self.checkRadData("radiators",self.DefaultRadiators[context])
      
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
