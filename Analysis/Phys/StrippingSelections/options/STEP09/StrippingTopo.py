#
# A stripping line for the topological trigger
#
# Author: V. Gligorov 

from os import environ
from Gaudi.Configuration import *
from Configurables import CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine

importOptions('$HLTSELECTIONSROOT/options/Hlt2LineTopoTF2BodyDEV.py')
importOptions('$HLTSELECTIONSROOT/options/Hlt2LineTopoTF3BodyDEV.py')
importOptions('$HLTSELECTIONSROOT/options/Hlt2LineTopoTF4BodyDEV.py')
importOptions('$HLT2SHAREDPARTICLESROOT/python/Hlt2SharedParticles/TopoTF2Body.py')
importOptions('$HLT2SHAREDPARTICLESROOT/python/Hlt2SharedParticles/TopoTF3Body.py')
importOptions('$HLT2SHAREDPARTICLESROOT/python/Hlt2SharedParticles/TopoTFInputParticles.py')

#Point the topological to use the offline pions and kaons but still no PID!
Hlt2TopoTFInputParticles = FilterDesktop("Hlt2TopoTFInputParticles")
Hlt2TopoTFInputParticles.InputLocations  = [ "StdNoPIDsPions","StdNoPIDsKaons" ]

#The shared particles
Hlt2SharedTopoTF2Body = CombineParticles("Hlt2SharedTopoTF2Body")
Hlt2SharedTopoTF3Body = CombineParticles("Hlt2SharedTopoTF3Body")

#The topo selections
filterTopoTF2Body = FilterDesktop('Hlt2FilterTopoTF2BodyDEV')
filterTopoTF3Body = FilterDesktop('Hlt2FilterTopoTF3BodyDEV')
combine = CombineParticles('Hlt2CombineTopoTF4BodyDEV')

#The 2 body topo sequencer
TwoBodyTopoSeq = GaudiSequencer("TwoBodyTopoSeq")
TwoBodyTopoSeq.Members += [Hlt2TopoTFInputParticles] 
TwoBodyTopoSeq.Members += [Hlt2SharedTopoTF2Body]
TwoBodyTopoSeq.Members += [filterTopoTF2Body]

#The 3 body topo sequencer
ThreeBodyTopoSeq = GaudiSequencer("ThreeBodyTopoSeq")
ThreeBodyTopoSeq.Members += [Hlt2TopoTFInputParticles]
ThreeBodyTopoSeq.Members += [Hlt2SharedTopoTF2Body]
ThreeBodyTopoSeq.Members += [Hlt2SharedTopoTF3Body]
ThreeBodyTopoSeq.Members += [filterTopoTF3Body]

#The 4 body topo sequencer
FourBodyTopoSeq = GaudiSequencer("FourBodyTopoSeq")
FourBodyTopoSeq.Members += [Hlt2TopoTFInputParticles]
FourBodyTopoSeq.Members += [Hlt2SharedTopoTF2Body]
FourBodyTopoSeq.Members += [Hlt2SharedTopoTF3Body]
FourBodyTopoSeq.Members += [combine]

#The main sequencer
TopoStrippingSequence = GaudiSequencer("TopoStrippingSequence")
TopoStrippingSequence.Members += [TwoBodyTopoSeq]
TopoStrippingSequence.Members += [ThreeBodyTopoSeq]
TopoStrippingSequence.Members += [FourBodyTopoSeq]
#TopoStrippingSequence.IgnoreFilterPassed = True
TopoStrippingSequence.ModeOR = 1

# Create the lines for this selection
line = StrippingLine('Topo'
               , prescale = 1.
               ,  algos = [TopoStrippingSequence]
               ,  stream = 'Topological'
               )
