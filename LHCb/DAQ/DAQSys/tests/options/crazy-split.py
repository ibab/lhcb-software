import RawEventCompat
import RawEventCompat.Configuration as conf

from Configurables import InputCopyStream
writer=InputCopyStream("SplitStream")

from GaudiConf import IOHelper

IOHelper().outStream("crazy-split.dst",writer)

juggle=conf.RawEventJuggler()
juggle.Input=0.1
juggle.Output=999
juggle.KillExtraNodes=True
juggle.WriterItemList=writer

from Gaudi.Configuration import *
from Configurables import GaudiSequencer

juggle.Sequencer=GaudiSequencer("SPAM")

from Configurables import LHCbApp
LHCbApp()

ApplicationMgr().TopAlg=[GaudiSequencer("SPAM")]
