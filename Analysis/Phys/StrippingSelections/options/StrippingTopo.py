#
# A stripping line for the topological trigger
#
# Note that this line currently implements the full topological trigger
# structure, including the robust and "post-fit" stages, despite using offline
# quality tracks. This is done since the line is supposed to be used for
# commisioning the topological trigger. In order to really do this,
# it will eventually need to be run in the HLT context, which is currently
# not done.  
#
# The IP cuts on the final state particles can be tightened to improve
# rate reduction, as can the pointing cut at the "robust" stage.
#
# For the moment the 4-body topological is included in a heavily prescaled
# form for commisioning only, because of its high rate. Once the topological
# trigger is properly optimized for the available bandwidth, this prescaling
# should be reduced.
#
# Author: V. Gligorov 

from os import environ
from Gaudi.Configuration import *
from Configurables import CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine

importOptions('$HLTSELECTIONSROOT/options/Hlt2LineTopoTF2BodyDEV.py')
importOptions('$HLTSELECTIONSROOT/options/Hlt2LineTopoTF3BodyDEV.py')
importOptions('$HLTSELECTIONSROOT/options/Hlt2LineTopoTF4BodyDEV.py')
importOptions('$HLTSELECTIONSROOT/options/Hlt2LineTopo2BodyDEV.py')
importOptions('$HLTSELECTIONSROOT/options/Hlt2LineTopo3BodyDEV.py')
importOptions('$HLTSELECTIONSROOT/options/Hlt2LineTopo4BodyDEV.py')
importOptions('$HLT2SHAREDPARTICLESROOT/python/Hlt2SharedParticles/TopoTF2Body.py')
importOptions('$HLT2SHAREDPARTICLESROOT/python/Hlt2SharedParticles/TopoTF3Body.py')
importOptions('$HLT2SHAREDPARTICLESROOT/python/Hlt2SharedParticles/TopoTFInputParticles.py')
importOptions('$HLT2SHAREDPARTICLESROOT/python/Hlt2SharedParticles/GoodParticles.py')
importOptions('$HLT2SHAREDPARTICLESROOT/python/Hlt2SharedParticles/Topo2Body.py')
importOptions('$HLT2SHAREDPARTICLESROOT/python/Hlt2SharedParticles/Topo3Body.py')

#Point the robust topological to use the offline pions and kaons but still no PID!
Hlt2GoodKaons = FilterDesktop("Hlt2GoodKaons")
Hlt2GoodKaons.InputLocations  = [ "StdNoPIDsKaons" ]
Hlt2GoodPions = FilterDesktop("Hlt2GoodPions")
Hlt2GoodPions.InputLocations  = [ "StdNoPIDsPions" ]

#The robust shared particles
Hlt2SharedTopo2Body = CombineParticles("Hlt2SharedTopo2Body")
Hlt2SharedTopo3Body = CombineParticles("Hlt2SharedTopo3Body")

#The robust topo selections
filterTopo2Body = FilterDesktop('Hlt2FilterTopo2BodyDEV')
filterTopo3Body = FilterDesktop('Hlt2FilterTopo3BodyDEV')
filterTopo3Body.InputLocations = ["Hlt2SharedTopo3Body"]
combinerobust = CombineParticles('Hlt2CombineTopo4BodyDEV')
combinerobust.InputLocations = ["Hlt2GoodPions","Hlt2SharedTopo3Body"]

#The cuts which can be tightened to improve rate reduction
#Hlt2GoodKaons.Code += " & (MIPDV(PRIMARY)>0.1*mm)"
#Hlt2GoodPions.Code += " & (MIPDV(PRIMARY)>0.1*mm)"
#filterTopo2Body.Code += "& (M > 4.5*GeV)"
#filterTopo2Body.Code += "& (BPVTRGPOINTINGWPT<0.1)"
#filterTopo3Body.Code += "& (M > 4.5*GeV)"
#filterTopo3Body.Code += "& (BPVTRGPOINTINGWPT<0.1)"
#combinerobust.MotherCut += "& (M>4.5*GeV)"
#combinerobust.MotherCut += "& (BPVTRGPOINTINGWPT<0.1)"

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

#The robust 2 body topo sequencer
TwoBodyTopoSeq = GaudiSequencer("TwoBodyTopoSeq")
TwoBodyTopoSeq.Members += [Hlt2GoodPions]
TwoBodyTopoSeq.Members += [Hlt2GoodKaons]
TwoBodyTopoSeq.Members += [Hlt2SharedTopo2Body]
TwoBodyTopoSeq.Members += [filterTopo2Body]

#The robust 3 body topo sequencer
ThreeBodyTopoSeq = GaudiSequencer("ThreeBodyTopoSeq")
ThreeBodyTopoSeq.Members += [Hlt2GoodPions] 
ThreeBodyTopoSeq.Members += [Hlt2GoodKaons]
ThreeBodyTopoSeq.Members += [Hlt2SharedTopo2Body]
ThreeBodyTopoSeq.Members += [Hlt2SharedTopo3Body]
ThreeBodyTopoSeq.Members += [filterTopo3Body]

#The robust 4 body topo sequencer
FourBodyTopoSeq = GaudiSequencer("FourBodyTopoSeq")
FourBodyTopoSeq.Members += [Hlt2GoodPions] 
FourBodyTopoSeq.Members += [Hlt2GoodKaons]
FourBodyTopoSeq.Members += [Hlt2SharedTopo2Body]
FourBodyTopoSeq.Members += [Hlt2SharedTopo3Body]
FourBodyTopoSeq.Members += [combinerobust]

#The 2 body topo sequencer
TwoBodyTFTopoSeq = GaudiSequencer("TwoBodyTFTopoSeq")
TwoBodyTFTopoSeq.Members += [Hlt2TopoTFInputParticles] 
TwoBodyTFTopoSeq.Members += [Hlt2SharedTopoTF2Body]
TwoBodyTFTopoSeq.Members += [filterTopoTF2Body]

#The 3 body topo sequencer
ThreeBodyTFTopoSeq = GaudiSequencer("ThreeBodyTFTopoSeq")
ThreeBodyTFTopoSeq.Members += [Hlt2TopoTFInputParticles]
ThreeBodyTFTopoSeq.Members += [Hlt2SharedTopoTF2Body]
ThreeBodyTFTopoSeq.Members += [Hlt2SharedTopoTF3Body]
ThreeBodyTFTopoSeq.Members += [filterTopoTF3Body]

#The 4 body topo sequencer
FourBodyTFTopoSeq = GaudiSequencer("FourBodyTFTopoSeq")
FourBodyTFTopoSeq.Members += [Hlt2TopoTFInputParticles]
FourBodyTFTopoSeq.Members += [Hlt2SharedTopoTF2Body]
FourBodyTFTopoSeq.Members += [Hlt2SharedTopoTF3Body]
FourBodyTFTopoSeq.Members += [combine]

#The robust sequencer without 4-body
TopoStrippingSequenceWout4Body = GaudiSequencer("TopoStrippingSequenceWout4Body")
TopoStrippingSequenceWout4Body.Members += [TwoBodyTopoSeq]
TopoStrippingSequenceWout4Body.Members += [ThreeBodyTopoSeq]
TopoStrippingSequenceWout4Body.ModeOR = 1

#The robust sequencer with 4-body
TopoStrippingSequenceWith4Body = GaudiSequencer("TopoStrippingSequenceWith4Body")
TopoStrippingSequenceWith4Body.Members += [TwoBodyTopoSeq]
TopoStrippingSequenceWith4Body.Members += [ThreeBodyTopoSeq]
TopoStrippingSequenceWith4Body.Members += [FourBodyTopoSeq]
TopoStrippingSequenceWith4Body.ModeOR = 1

#The sequencer without 4-body
TopoTFStrippingSequenceWout4Body = GaudiSequencer("TopoTFStrippingSequenceWout4Body")
TopoTFStrippingSequenceWout4Body.Members += [TwoBodyTFTopoSeq]
TopoTFStrippingSequenceWout4Body.Members += [ThreeBodyTFTopoSeq]
TopoTFStrippingSequenceWout4Body.ModeOR = 1

#The sequencer with 4-body
TopoTFStrippingSequenceWith4Body = GaudiSequencer("TopoTFStrippingSequenceWith4Body")
TopoTFStrippingSequenceWith4Body.Members += [TwoBodyTFTopoSeq]
TopoTFStrippingSequenceWith4Body.Members += [ThreeBodyTFTopoSeq]
TopoTFStrippingSequenceWith4Body.Members += [FourBodyTFTopoSeq]
TopoTFStrippingSequenceWith4Body.ModeOR = 1

# Create the lines for this selection
line = StrippingLine('TopoWout4Body'
               , prescale = 1.
               , algos = [TopoStrippingSequenceWout4Body,TopoTFStrippingSequenceWout4Body]
               , stream = 'Topological'
               )

line = StrippingLine('TopoWith4Body'
               , prescale = 1.
               , algos = [TopoStrippingSequenceWith4Body,TopoTFStrippingSequenceWith4Body]
               , stream = 'Topological'
               , postscale = 0.1
               )
