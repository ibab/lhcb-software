
## @package CKThetaResolution
#  Configurables for RICH CK theta resolution tools
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: CKThetaResolution.py,v 1.2 2008-08-15 14:41:23 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *

# ----------------------------------------------------------------------------------

## @class CKThetaResolutionConfig
#  Configurable for RICH CK theta resolution tools
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class CKThetaResolutionConfig(RichConfigurableUser):

    __slots__ = { "context":  "Offline" }

    ## @brief Apply the configuration
    #
    def applyConf(self):

        ckRes = RichTools().ckResolution()
        if ckRes.getType() == "Rich::Rec::BinnedCKResVthetaForRecoTracks" :
            self.setBinnedProperties(ckRes)
        elif ckRes.getType() == "Rich::Rec::InterpCKResVthetaForRecoTracks" :
            self.setInterpProperties(ckRes)

    ## @brief Set properties for the interpolated resolution tool
    #  @todo Find a better way to handle this
    def setInterpProperties(self,gEff):
        
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

    ## @brief Set properties for the binned resolution tool
    #  @todo Find a better way to handle this
    def setBinnedProperties(self,gEff):

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


