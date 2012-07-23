# $Id$

from Configurables import Gauss, Generation, Special, GenXiccProduction, XiccDaughtersInLHCb, ToolSvc

gen = Generation()
gen.addTool( Special , name = "Special" )
gen.Special.ProductionTool = "GenXiccProduction"
gen.Special.addTool( GenXiccProduction , name = "GenXiccProduction" )

gen.Special.GenXiccProduction.BaryonState = "Xi_bc+"
gen.Special.GenXiccProduction.BeamMomentum = Gauss().BeamMomentum
GenXiccListOfCommands = [
  "mixevnt imix 1" ,
  "loggrade ivegasopen 0",
  "loggrade igrade 1",
  "vegasbin nvbin 300",  # grade files generated with these setting    
  "counter xmaxwgt 5000000",
  "confine pscutmin 1.9",
  "confine pscutmin 5.0"

]
gen.Special.GenXiccProduction.GenXiccCommands += GenXiccListOfCommands
gen.PileUpTool = "FixedLuminosity";


gen.Special.CutTool = "XiccDaughtersInLHCb"
gen.Special.addTool(XiccDaughtersInLHCb)
gen.Special.XiccDaughtersInLHCb.BaryonState = gen.Special.GenXiccProduction.BaryonState

myOutputLevel = 3

gen.OutputLevel = myOutputLevel
gen.Special.OutputLevel = myOutputLevel
gen.Special.GenXiccProduction.OutputLevel = myOutputLevel
ToolSvc().XiccDaughtersInLHCb.OutputLevel = myOutputLevel

#temporary patch to ParticlePropertySvc
from Configurables import LHCb__ParticlePropertySvc

LHCb__ParticlePropertySvc().Particles = [
    "Xi_bc+   532  5242   1. 6.9 1.29e-12 Xi_bc+       5242 0.",
    "Xi_bc~-  533 -5242  -1. 6.9 1.29e-12 anti-Xi_bc- -5242 0.",
    "Xi_bc0   522  5142   0. 6.9 1.29e-12 Xi_bc0       5142 0.",
    "Xi_bc~0  523 -5142   0. 6.9 1.29e-12 anti-Xi_bc0 -5142 0."]           
