#** @file 
#
#  Redo Hlt exclusive selections particle making with refit tracks.
#  Based on HltConf/Hlt2Particles.opts
#
#---------------------------------------------------------------------
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import CombineParticles, PhysDesktop

#---------------------------------------------------------------------
SeqHlt2KShortForDX = GaudiSequencer('SeqHlt2KShortForDX')
#---------------------------------------------------------------------

from Configurables import PatSeeding, PatSeedingTool
from Configurables import PatDownstream

#The Seeding
recoSeeding = PatSeeding()
recoSeeding.OutputTracksName="Hlt/Track/Seeding"
recoSeeding.addTool(PatSeedingTool, name="PatSeedingTool")
recoSeeding.PatSeedingTool.UseForward = True
recoSeeding.PatSeedingTool.ForwardCloneMergeSeg = True
recoSeeding.PatSeedingTool.InputTracksName = "Hlt/Track/Forward"

PatDownstream = PatDownstream()
PatDownstream.InputLocation="Hlt/Track/Seeding"
PatDownstream.OutputLocation="Hlt/Track/SeedTT"
## for TT: 
PatDownstream.UseForward = True
PatDownstream.SeedFilter = True

SeqHlt2KShortForDX.Members += [ recoSeeding ]
SeqHlt2KShortForDX.Members += [ PatDownstream ]




