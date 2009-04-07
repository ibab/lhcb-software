# Minimal file for running Gauss from python prompt
#
# Syntax is:
#   gaudirun.py $GAUSSOPTS/Gauss.py OtherSteerig.{opts,py}
#
# To specify an EventType you can give it as argument to follow
#   gaudirun.py $GAUSSOPTS/Gauss.py $DECFILESROOT/options/30000000.opts
#
# Or for a slightly fancier version where the name of the file is automatically
# set look at the example Gauss-Job.py and use it as
#   gaudirun.py $GAUSSOPTS/Gauss.py $GAUSSOPTS/Gauss-Job.py
#
from Gauss.Configuration import *

#--Allow to use ns (ie. double) in options of EventClockSvc
ec = EventClockSvc()
ec.addTool(FakeEventTime(), name="EventTimeDecoder")
ec.EventTimeDecoder.StartTime = 0
ec.EventTimeDecoder.TimeStep  = 25*SystemOfUnits.ns

#--General Gauss setting and sequence
importOptions('$GAUSSOPTS/Gauss.opts')

#--Pack events and check consistency
GaudiSequencer("Simulation").Members += [ PackMCParticle(),
                                          PackMCVertex() ]
testMCV = UnpackMCVertex( "TestMCVertex", OutputName= "MC/VerticesTest" )
testMCP = UnpackMCParticle( "TestMCParticle", OutputName= "MC/ParticlesTest" )
GaudiSequencer("Simulation").Members += [ testMCV, testMCP,
                                         CompareMCParticle(),
                                         CompareMCVertex() ]

#--Configuration of output files and 'default' outputs files that can/should
#--be overwritten in Gauss-Job.py
spillOverList = ['']
setOutputs(spillOverList)

#--Geometry dependent options
LHCbApp( Simulation = True )

#--Geometry to be simulated options
giGaGeo = GiGaGeo()
giGaGeo.UseAlignment      = True
giGaGeo.AlignAllDetectors = True
importOptions('$GAUSSOPTS/SimVeloGeometry.py')  # -- To misalign VELO

