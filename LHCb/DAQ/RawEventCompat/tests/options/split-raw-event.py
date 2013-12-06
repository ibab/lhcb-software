# ============ Auto-generated dummy options file ======
from Gaudi.Configuration import *
from Configurables import LHCbApp
LHCbApp()

from Configurables import RawEventFormatConf, RecombineRawEvent, RawEventJuggler, RawEventMapCombiner;
from Configurables import GaudiSequencer
from Configurables import InputCopyStream as ics

MySeq=GaudiSequencer("MoveRawEventAround")
MyWriter=InputCopyStream("CopyToFile")

#RawEventFormatConf().__apply_configuration__()

RawEventJuggler().Input=0.0 #From original location
RawEventJuggler().Output=2.0 #go to Stripping20 format
RawEventJuggler().Sequencer=MySeq
RawEventJuggler().KillExtraNodes=True #remove DAQ/RawEvent completely
RawEventJuggler().KillExtraDirectories=True
RawEventJuggler().WriterOptItemList=MyWriter

ApplicationMgr().TopAlg=[MySeq]
from Configurables import StoreExplorerAlg
ApplicationMgr().TopAlg+=[StoreExplorerAlg()]

from GaudiConf import IOHelper
IOHelper().outStream("split-raw-event.dst",MyWriter)
