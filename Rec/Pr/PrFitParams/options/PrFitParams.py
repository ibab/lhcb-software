from Configurables import Brunel, NTupleSvc, TrackSys, RecMoniConf

Brunel().InputType = "DIGI" # implies also Brunel().Simulation = True
Brunel().WithMC    = True   # implies also Brunel().Simulation = True
Brunel().OutputType   = "NONE"
Brunel().Histograms   = "Expert"
TrackSys().TrackPatRecAlgorithms = [ "Truth", "FastVelo" ]
Brunel().RecoSequence = ["Decoding", "Truth", "VELO" ]
RecMoniConf().MoniSequence = [ ]
Brunel().MCCheckSequence = [ "Fit" ]

from Configurables import DDDBConf
DDDBConf().DbRoot = "/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/static_DDDB_FT_v4/lhcb.xml"
Brunel().DDDBtag   = "MC11-20111102"
Brunel().CondDBtag = "sim-20111111-vc-md100"

NTupleSvc().Output = ["FILE1 DATAFILE='~/w0/track.root' TYP='ROOT' OPT='NEW'"]

from Configurables import LHCbApp

from Gaudi.Configuration import * 
from GaudiConf import IOHelper

IOHelper().inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/o/ocallot/Bs_mumu_v4_nu50.digi?svcClass=default'])
Brunel().EvtMax     = 1000

def doMyChanges():
   from Configurables import GaudiSequencer
   GaudiSequencer("CaloBankHandler").Members = []
   GaudiSequencer("RecoDecodingSeq").Members = [ "DecodeVeloRawBuffer/DecodeVeloClusters", "FTRawBankDecoder" ]
   GaudiSequencer("RecoTruthSeq").Members = [ "UnpackMCParticle", "UnpackMCVertex", "PrLHCbID2MCParticle" ]
   GaudiSequencer("MCLinksTrSeq").Members = ["PrTrackAssociator"] 
appendPostConfigAction( doMyChanges )


from Configurables import PrFitFwdParams, PrFitSeedParams, PrFitKsParams

PrFitFwdParams().VeloFromMC = True

GaudiSequencer("CheckFitSeq").Members += [ PrFitFwdParams() ]
#                                           PrFitSeedParams(),
#                                           PrFitKsParams() ]

PrFitKsParams().zMagnetParams = [ 5372.42, -3372.25, 370.484 ];
PrFitKsParams().momentumParams = [ 1226.32, 479.859, 2859.75 ];


PrFitSeedParams().InitialArrowParams  = [ 4.21826e-09, -8.93796e-08, 0.372981 ]
PrFitSeedParams().MomentumScaleParams = [ 40.3751, 1163.24, -682850 ];
PrFitSeedParams().zMagnetParams       = [ 5373.86, -3283.82, 379.141 ];
PrFitSeedParams().dRatioParams        = [ -3.77e-4, 4.7 ]
PrFitSeedParams().yCorrectionParams   = [ 1.25e-14 ]

#==== original
PrFitFwdParams().ZVelo      = 1640 ;
PrFitFwdParams().ZReference = 9900 ;
#PrFitFwdParams().zMagnetParams = [ 5199.31, 334.725, -1283.86, 9.59486e-06, -413.281 ];
#PrFitFwdParams().xParams = [ 16.8238, -6.35556 ];
#PrFitFwdParams().MomentumParams = [ 1.21909, 0.627841, -0.235216, 0.433811, 2.92798, -21.3909 ];

PrFitFwdParams().zMagnetParams = [ 5210.26, 330.354, -1256.88, 7.6753e-06, -457.75 ]
PrFitFwdParams().xParams = [ 14.3872, -5.07159 ]
PrFitFwdParams().MomentumParams = [ 1.20996, 0.623154, -0.205741, 0.476163, 3.21973, -26.9716 ]

PrFitFwdParams().ZReference = 9900 ;

PrFitFwdParams().zMagnetParams = [ 5308.08, 354.469, -1268.34, 1.14258e-06, -2214.26 ]
PrFitFwdParams().xParams = [ -22.9491, -10.3033 ]
PrFitFwdParams().MomentumParams = [ 1.28268, 0.613738, -0.264608, 0.284489, 1.22752, -14.531 ]

PrFitFwdParams().zMagnetParams = [ 5307.54, 322.057, -1288.82, 1.71301e-06, -2151.02 ]
PrFitFwdParams().xParams = [ -3.13548, -4.95693 ]
PrFitFwdParams().MomentumParams = [ 1.24237, 0.634094, -0.257795, 0.3933, 1.85546, -13.9831 ]


PrFitFwdParams().zMagnetParams = [ 5307.46, 331.931, -1286.92, 1.56216e-06, -2156.52 ]
PrFitFwdParams().xParams = [ -5.40404, -5.54635 ]
PrFitFwdParams().byParams = [ 0. ]
PrFitFwdParams().cyParams = [ 0. ]

PrFitFwdParams().MomentumParams = [ 1.24425, 0.644144, -0.252728, 0.349017, 1.81187, -13.2179 ]
PrFitFwdParams.zMagnetParams = [ 5307.48, 330.179, -1287, 1.58684e-06, -2155.67 ]
PrFitFwdParams.xParams = [ -4.76316, -5.36211 ]
PrFitFwdParams.byParams = [ -0.7615 ]
PrFitFwdParams.cyParams = [ -3.75835e-05 ]
PrFitFwdParams.momentumParams = [ 1.24411, 0.639831, -0.250189, 0.358935, 1.80863, -13.0067 ]


PrFitFwdParams.zMagnetParams = [ 5307.48, 330.179, -1287, 1.58684e-06, -2155.67 ]
PrFitFwdParams.xParams = [ -4.76316, -5.36211 ]
PrFitFwdParams.byParams = [ -0.7615 ]
PrFitFwdParams.cyParams = [ -3.75835e-05 ]
PrFitFwdParams.momentumParams = [ 1.24411, 0.639831, -0.250189, 0.358935, 1.80863, -13.0067 ]

