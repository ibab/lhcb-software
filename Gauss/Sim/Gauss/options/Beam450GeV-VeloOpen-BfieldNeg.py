# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam450GeV-VeloOpen-BfieldNeg 
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-2008.py
#              $GAUSSOPTS/Beam450GeV-VeloClose-BfieldNeg.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND tag to generate Open VELO
from DetCond.Configuration import addCondDBLayer
simCondVelo = allConfigurables["SIMCOND"].clone("VELOCOND")
simCondVelo.DefaultTAG = "velo-open"
addCondDBLayer(simCondVelo)

#--Tell to use 450 GeV beams for collisions and beam gas, with corresponding
#--beam size and luminous region, and set crossingle angle for collisions
#--for beam gas Hijing cannot take inot account crossing angle, so ignore! 
importOptions("$GAUSSOPTS/PilotRun.opts")
gen = Generation()
gen.addTool(MinimumBias,name="SampleGenerationTool")
gen.SampleGenerationTool.addTool(PythiaProduction,name="ProductionTool")
gen.SampleGenerationTool.ProductionTool.addTool(CollidingBeams,name="BeamToolName")
gen.SampleGenerationTool.ProductionTool.BeamToolName.BeamMomentum = 450.0 * GeV
gen.SampleGenerationTool.ProductionTool.BeamToolName.HorizontalCrossingAngle = 2.1 * mrad

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 110000*ns
