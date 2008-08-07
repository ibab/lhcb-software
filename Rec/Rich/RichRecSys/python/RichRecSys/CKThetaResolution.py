"""
Geometrical Efficiency options for RICH Reconstruction
"""
__version__ = "$Id: CKThetaResolution.py,v 1.1 2008-08-07 20:56:54 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( Rich__Rec__BinnedCKResVthetaForRecoTracks,
                            Rich__Rec__InterpCKResVthetaForRecoTracks,
                            Rich__Rec__FunctionalCKResForRecoTracks )

# ----------------------------------------------------------------------------------

class CKThetaResolutionConf(RichConfigurableUser):

    __slots__ = {
        "context":  "Offline"
        ,"type":    "Binned"
        }

    def applyConf(self):

        type = self.getProp("type")
        nickname = "RichCherenkovResolution"

        if type == "Binned" :
            CKThetaResBinned().createInstance(self,nickname)
        elif type == "Interpolated" :
            CKThetaResInterp().createInstance(self,nickname)
        elif type == "Functional" :
            CKThetaResFunctional().createInstance(self,nickname)
        else:
            raise RuntimeError("Unknown CK theta resolution tool '%s'"%type)

# ----------------------------------------------------------------------------------

class CKThetaResFunctional(RichConfigurableUser):

    def createInstance(self,parent,nickname):
        type = "Rich::Rec::FunctionalCKResForRecoTracks"
        context = parent.getProp("context")
        self.setProp("context",context)
        gEff = Rich__Rec__FunctionalCKResForRecoTracks("ToolSvc."+context+"_"+nickname)
        self.toolRegistry().Tools += [type+"/"+nickname]
        return gEff       

# ----------------------------------------------------------------------------------

class CKThetaResInterp(RichConfigurableUser):

    def createInstance(self,parent,nickname):
        type = "Rich::Rec::InterpCKResVthetaForRecoTracks"
        context = parent.getProp("context")
        self.setProp("context",context)
        gEff = Rich__Rec__InterpCKResVthetaForRecoTracks("ToolSvc."+context+"_"+nickname)
        self.setDefaultOptions(gEff)
        self.toolRegistry().Tools += [type+"/"+nickname]
        return gEff

    def setDefaultOptions(self,gEff):
        
        # CRJ Need to find a better way to handle this
        gEff.ForwardAerogel = [ (0.113636,0.00524541), (0.159584,0.0038708), (0.214024,0.00267393), (0.237273,0.0023797) ]
        gEff.ForwardRich1Gas = [ (0.0245328,0.00172102), (0.0414095,0.00150918), (0.0505255,0.00138705) ]
        gEff.ForwardRich2Gas = [ (0.0149033,0.000977774), (0.0242266,0.0008275), (0.0278957,0.000685761) ]
        gEff.KsTrackAerogel = [ (0.111344,0.00560132), (0.159924,0.00401517), (0.214074,0.00301323), (0.237765,0.00271771) ]
        gEff.KsTrackRich1Gas = [ (0.0243501,0.0022413), (0.0416362,0.00174685), (0.0511916,0.00156123) ]
        gEff.KsTrackRich2Gas = [ (0.015014,0.000967986), (0.0239729,0.000855588), (0.0281323,0.00068772) ]
        gEff.MatchAerogel = [ (0.113604,0.00590846), (0.15991,0.00379323), (0.214619,0.00264066), (0.237324,0.00239153) ]
        gEff.MatchRich1Gas = [ (0.0243042,0.00184286), (0.0416253,0.00150318), (0.0504754,0.00138152) ]
        gEff.MatchRich2Gas = [ (0.0149695,0.000989144), (0.0240658,0.000831526), (0.0280196,0.000671958) ]
        gEff.SeedRich2Gas = [ (0.0150586,0.00233702), (0.0242994,0.00111318), (0.0282822,0.000935434) ]
        gEff.VeloTTAerogel = [ (0.114533,0.0172506), (0.16356,0.0378092), (0.216632,0.00961744), (0.236653,0.00272289) ]
        gEff.VeloTTRich1Gas = [ (0.0249591,0.00245855), (0.0417991,0.00339135), (0.0504094,0.00169416) ]

# ----------------------------------------------------------------------------------

class CKThetaResBinned(RichConfigurableUser):

    def createInstance(self,parent,nickname):
        type = "Rich::Rec::BinnedCKResVthetaForRecoTracks"
        context = parent.getProp("context")
        self.setProp("context",context)
        gEff = Rich__Rec__BinnedCKResVthetaForRecoTracks("ToolSvc."+context+"_"+nickname)
        self.setDefaultOptions(gEff)
        self.toolRegistry().Tools += [type+"/"+nickname]
        return gEff

    def setDefaultOptions(self,gEff):

        gEff.NormaliseRes = True

        # CRJ Need to find a better way to handle this
        gEff.NAerogelResBins = [ (0,0.13), (0.13,0.18), (0.18,0.23), (0.23,1) ]
        gEff.NRich1GasResBins = [ (0,0.0275), (0.0275,0.047), (0.047,1) ]
        gEff.NRich2GasResBins = [ (0,0.018), (0.018,0.026), (0.026,1) ]
        
        gEff.ForwardAerogelRes = [ 0.00524541, 0.0038708, 0.00267393, 0.0023797 ]
        gEff.ForwardRich1GasRes = [ 0.00172102, 0.00150918, 0.00138705 ]
        gEff.ForwardRich2GasRes = [ 0.000977774, 0.0008275, 0.000685761 ]
        
        gEff.KsTrackAerogelRes = [ 0.00560132, 0.00401517, 0.00301323, 0.00271771 ]
        gEff.KsTrackRich1GasRes = [ 0.0022413, 0.00174685, 0.00156123 ]
        gEff.KsTrackRich2GasRes = [ 0.000967986, 0.000855588, 0.00068772 ]
        
        gEff.MatchAerogelRes = [ 0.00590846, 0.00379323, 0.00264066, 0.00239153 ]
        gEff.MatchRich1GasRes = [ 0.00184286, 0.00150318, 0.00138152 ]
        gEff.MatchRich2GasRes = [ 0.000989144, 0.000831526, 0.000671958 ]
        
        gEff.SeedRich2GasRes = [ 0.00233702, 0.00111318, 0.000935434 ]
        
        gEff.VeloTTAerogelRes = [ 0.0172506, 0.0378092, 0.00961744, 0.00272289 ]
        gEff.VeloTTRich1GasRes = [ 0.00245855, 0.00339135, 0.00169416 ]
        
        # Following are needed for MC data, when it is possible that a reco track type is
        # different to its MC type (i.e. a reco seed track is in fact a long MC track and
        # thus has info in RICH1)
        
        gEff.SeedAerogelRes    = [ 0.00233702, 0.00111318, 0.000935434, 0.000935434 ]
        gEff.SeedRich1GasRes   = [ 0.00233702, 0.00111318, 0.000935434 ]
        gEff.VeloTTRich2GasRes = [ 0.00245855, 0.00339135, 0.00169416  ]
        
        # MCRichTrack options
        gEff.MCRichTrackAerogelRes = [ 0.00524541, 0.0038708, 0.00267393, 0.0023797 ]
        gEff.MCRichTrackRich1GasRes = [ 0.00172102, 0.00150918, 0.00138705 ]
        gEff.MCRichTrackRich2GasRes = [ 0.000977774, 0.0008275, 0.000685761 ]


