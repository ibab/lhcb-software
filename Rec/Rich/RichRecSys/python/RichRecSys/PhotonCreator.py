
"""
Photon creator options for RICH Reconstruction
"""
__version__ = "$Id: PhotonCreator.py,v 1.1 2008-08-07 20:56:54 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( Rich__Rec__SimplePhotonPredictor,
                            Rich__Rec__CKthetaBandsPhotonPredictor,
                            Rich__Rec__PhotonCreator,
                            Rich__Rec__PhotonRecoUsingQuarticSoln,
                            Rich__Rec__PhotonRecoUsingCKEstiFromRadius )

# ----------------------------------------------------------------------------------

class PhotonCreator(RichConfigurableUser):

    DefaultNSigma   = [ 3.8,   3.3,   3.3    ]
    DefaultMinCKT   = [ 0.075, 0.005, 0.005  ]
    DefaultMaxCKT   = [ 0.29,  0.075, 0.035  ]

    __slots__ = {
        "context":    "Offline"
       ,"minCKTheta": []
       ,"maxCKTheta": []
       ,"nSigma": []      # N sigma cuts
        }

    def checkConfiguration(self):
        self.checkRadData("minCKTheta",self.DefaultMinCKT)
        self.checkRadData("maxCKTheta",self.DefaultMaxCKT)
        self.checkRadData("nSigma",self.DefaultNSigma)

    def createInstance(self,parent,nickname):
        self.checkConfiguration()
        # Create and configure
        type    = "Rich::Rec::PhotonCreator"
        context = parent.getProp("context")
        self.setProp("context",context)
        creator = Rich__Rec__PhotonCreator("ToolSvc."+context+"_"+nickname)
        creator.PhotonRecoTool  = "PhotonParams"
        creator.MinAllowedCherenkovTheta = self.getProp("minCKTheta")
        creator.MaxAllowedCherenkovTheta = self.getProp("maxCKTheta")
        creator.NSigma                   = self.getProp("nSigma")
        # Add to registry
        self.toolRegistry().Tools += [type+"/"+nickname]
        return creator

# ----------------------------------------------------------------------------------

class QuarticPhotonReco(RichConfigurableUser):

    DefaultNQIts = [ 3,3,3 ]
    __slots__ = {
         "context":    "Offline"
        ,"nQuarticIterations": [ ]
        }

    def createInstance(self,parent,nickname):
        self.checkRadData("nQuarticIterations",self.DefaultNQIts)
        # Create and configure
        type = "Rich::Rec::PhotonRecoUsingQuarticSoln"
        parent.setOtherProp(self,"context")
        reco = Rich__Rec__PhotonRecoUsingQuarticSoln(nickname)
        reco.NQuarticIterationsForSecMirrors = self.getProp("nQuarticIterations")
        # Add to registry
        self.toolRegistry().Tools += [type+"/"+nickname]
        # return
        return reco

# ----------------------------------------------------------------------------------

class CKEstiPhotonReco(RichConfigurableUser):
    
    __slots__ = {
        "context":    "Offline"
        }

    def createInstance(self,parent,nickname):
        # Create and configure
        type = "Rich::Rec::PhotonRecoUsingCKEstiFromRadius"
        parent.setOtherProp(self,"context")
        reco = Rich__Rec__PhotonRecoUsingCKEstiFromRadius(nickname)
        # Add to registry
        self.toolRegistry().Tools += [type+"/"+nickname]
        # return
        return reco

# ----------------------------------------------------------------------------------

class SimplePhotonPredictor(RichConfigurableUser):

    # Default settings     Aero      R1Gas    R2Gas
    DefaultMinROI       = [ 0.0,     0.0,     0.0     ]
    DefaultMaxROI       = [ 9999999, 9999999, 9999999 ]
   
    __slots__ = {
         "context":    "Offline"
        ,"minROI": []
        ,"maxROI": []
        }

    def createInstance(self,parent,nickname):
        self.checkRadData("minROI",self.DefaultMinROI)
        self.checkRadData("maxROI",self.DefaultMaxROI)
        parent.setOtherProp(self,"context")
        # Create and configure
        type = "Rich::Rec::SimplePhotonPredictor"
        pred = Rich__Rec__SimplePhotonPredictor(nickname)
        pred.MinTrackROI = self.getProp("minROI")
        pred.MaxTrackROI = self.getProp("maxROI")
        # Add to registry
        self.toolRegistry().Tools += [type+"/"+nickname]
        # return
        return pred

# ----------------------------------------------------------------------------------

class CKthetaBandsPhotonPredictor(RichConfigurableUser):

    # Default settings     Aero   R1Gas  R2Gas
    DefaultNSigma      = [ 5.5,   5.5,   11.5   ]

    __slots__ = {
        "context":    "Offline"
        ,"nSigma": [] # N sigma cuts
        }

    def createInstance(self,parent,nickname):
        self.checkRadData("nSigma",self.DefaultNSigma)
        parent.setOtherProp(self,"context")
        # Create and configure
        type = "Rich::Rec::CKthetaBandsPhotonPredictor"
        pred = Rich__Rec__CKthetaBandsPhotonPredictor(nickname)
        pred.NSigma = self.getProp("nSigma")
        # Add to registry
        self.toolRegistry().Tools += [type+"/"+nickname]
        # return
        return pred

# ----------------------------------------------------------------------------------

class RichPhotonCreatorConf(RichConfigurableUser):

    __slots__ = {
        "context":    "Offline"        # The context within which to run the reco sequences
       ,"radiators":  [True,True,True] # The radiators to use (Aerogel/Rich1Gas/Rich2Gas)
       ,"photonPredictor": "CKthetaBands" # The photon predictor to use
       ,"photonCreator": "RecoPhotons"    # The photon creator to use
       ,"photonReco": "Quartic"           # Photon reconstruction tool
        }

    def applyConf(self):

        # -----------------------------------------------------------------------
        # Photon maker

        creatorType = self.getProp("photonCreator")
        creator_nickname = "RichPhotonCreator"

        if creatorType == "RecoPhotons" :
            creator = PhotonCreator().createInstance(self,creator_nickname)
        else:
            raise RuntimeError("Unknown photon creator '%s'"%creatorType)

        # -----------------------------------------------------------------------
        # Photon predictor (a.k.a. pre-selection)

        predType = self.getProp("photonPredictor")
        predictor_nickname = "RichPhotonPredictor"

        if predType == "SelectAll" :
            pred = SimplePhotonPredictor().createInstance(self,predictor_nickname)
        elif predType == "CKthetaBands" :
            pred = CKthetaBandsPhotonPredictor().createInstance(self,predictor_nickname)
        else:
            raise RuntimeError("Unknown photon predictor '%s'"%predType)

        # Configure creator with the correct predictor
        creator.addTool( pred, name = "Predictor" )

        # -----------------------------------------------------------------------
        # Photon reconstruction (i.e. calculating the CK theta/phi values)

        recoType = self.getProp("photonReco")
        reco_nickname = "PhotonParams"

        if recoType == "Quartic" :
            reco = QuarticPhotonReco().createInstance(self,reco_nickname)
        elif recoType == "EstiFromRadius" :
            reco = CKEstiPhotonReco().createInstance(self,reco_nickname)
        else:
            raise RuntimeError("Unknown photon reconstruction tool '%s'"%recoType)

        # Configure creator with the correct reco tool
        creator.addTool( reco, name = reco_nickname )

        # -----------------------------------------------------------------------
