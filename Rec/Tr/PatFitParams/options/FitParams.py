from Configurables import ( Brunel, NTupleSvc )

Brunel().InputType = "DIGI" # implies also Brunel().Simulation = True
Brunel().WithMC    = True   # implies also Brunel().Simulation = True
Brunel().OutputType   = "NONE"
Brunel().Histograms   = "Expert"
Brunel().RecoSequence = [ "VELO" ]
Brunel().MoniSequence = [ "VELO" ]
Brunel().MCLinksSequence = [ ]
Brunel().MCCheckSequence = [ "Fit" ]

NTupleSvc().Output = ["FILE1 DATAFILE='~/w0/track.root' TYP='ROOT' OPT='NEW'"]

from Configurables import LHCbApp

LHCbApp().DDDBtag   = "head-20090330"
LHCbApp().CondDBtag = "sim-20090402-vc-mu100"

#-- GAUDI jobOptions generated on Wed Aug 19 13:24:16 2009
#-- Contains event types : 
#--   30000000 - 13 files - 239981 events - 130.61 GBytes

from Gaudi.Configuration import * 

EventSelector().Input   = [
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000001_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000002_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000003_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000004_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000005_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000006_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000007_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000008_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000009_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000010_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000011_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000012_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/XDST/00004918/0000/00004918_00000013_1.xdst' TYP='POOL_ROOTTREE' OPT='READ'"]

Brunel().EvtMax     = 1000

from Configurables import ( GaudiSequencer, DecodeVeloRawBuffer, TrackAssociator,
                            Tf__PatVeloRTracking, Tf__PatVeloSpaceTool,
                            Tf__PatVeloSpaceTracking, Tf__PatVeloGeneralTracking, NewVeloSpaceTool,
                            UnpackMCParticle, UnpackMCVertex, MCParticleSelector )

GaudiSequencer("RecoVELOSeq").Members = [ DecodeVeloRawBuffer(),
                                          Tf__PatVeloRTracking("PatVeloRTracking"),
                                          Tf__PatVeloSpaceTracking("PatVeloSpaceTracking"),
                                          Tf__PatVeloGeneralTracking("PatVeloGeneralTracking"),
                                          UnpackMCParticle(),
                                          UnpackMCVertex(),
                                          TrackAssociator("AssocVeloRZ"),
                                          TrackAssociator("AssocVelo")

                                          ]
from Configurables import ( FwdFitParams, SeedFitParams, KsFitParams )


GaudiSequencer("CheckFitSeq").Members += [ FwdFitParams(),
                                           SeedFitParams(),
                                           KsFitParams() ]

KsFitParams().zMagnetParams = [ 5372.42, -3372.25, 370.484 ];
KsFitParams().momentumParams = [ 1226.32, 479.859, 2859.75 ];


SeedFitParams().InitialArrowParams  = [ 4.21826e-09, -8.93796e-08, 0.372981 ]
SeedFitParams().MomentumScaleParams = [ 40.3751, 1163.24, -682850 ];
SeedFitParams().zMagnetParams       = [ 5373.86, -3283.82, 379.141 ];
SeedFitParams().dRatioParams        = [ -3.77e-4, 4.7 ]
SeedFitParams().yCorrectionParams   = [ 1.25e-14 ]

FwdFitParams().ZVelo      = 1640 ;
FwdFitParams().ZReference = 8520 ;
FwdFitParams().zMagnetParams = [ 5199.31, 334.725, -1283.86, 9.59486e-06, -413.281 ];
FwdFitParams().xParams = [ 16.8238, -6.35556 ];
FwdFitParams().yParams = [ -970.89 ];
FwdFitParams().y1Params = [ -0.686028 ];
FwdFitParams().MomentumParams = [ 1.21909, 0.627841, -0.235216, 0.433811, 2.92798, -21.3909 ];

