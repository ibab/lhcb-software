#
# Raw event juggler to split DAQ/RawEvent into FULL.DST format
#
from Configurables import GaudiSequencer, RawEventJuggler
jseq=GaudiSequencer("RawEventSplitSeq")
juggler=RawEventJuggler("rdstJuggler")
juggler.Sequencer=jseq
juggler.Input=0.3  # 2015 Online (Moore) format 
juggler.Output=4.2 # Reco15 format

from Configurables import DaVinci
DaVinci().prependToMainSequence( [jseq] )
