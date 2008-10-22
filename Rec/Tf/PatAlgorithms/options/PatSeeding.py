from TrackSys.Configuration import *
from Configurables import (PatSeeding, PatSeedingTool)

if TrackSys().fieldOff():
   PatSeeding("PatSeeding").addTool(PatSeedingTool, name="PatSeedingTool")
   PatSeeding("PatSeeding").PatSeedingTool.xMagTol = 4e2;
   PatSeeding("PatSeeding").PatSeedingTool.zMagnet = 0.;
   PatSeeding("PatSeeding").PatSeedingTool.FieldOff = True;
   PatSeeding("PatSeeding").PatSeedingTool.MinMomentum = 5e4;
