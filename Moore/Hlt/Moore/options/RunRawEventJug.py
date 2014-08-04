"""
This script process a DST with the RawEvent split up and writes out the DAQ/RawEvent to the output DST
The test should check for the existence of DAQ/RawEvent in the TES of reocmbined.dst
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer,RawEventJuggler
from Configurables import LHCbApp
LHCbApp()
tck = "0x409f0045" #0x409f0045 #Sim08
MySeq=GaudiSequencer("MoveTCKtoNewLocation")
MyWriter=InputCopyStream("CopyToFile")
RawEventJuggler().GenericReplacePatterns={"A": "b"} #BUGFIX: This is needed until LHCb > v36r2
RawEventJuggler().TCK=tck
RawEventJuggler().Input=2.0 #expects Stripping 20 configuration
RawEventJuggler().Output=0.0 #which is this
RawEventJuggler().Sequencer=MySeq
RawEventJuggler().WriterOptItemList=MyWriter
RawEventJuggler().KillInputBanksAfter="L0*|Hlt*"
RawEventJuggler().KillExtraNodes=True #remove DAQ/RawEvent completely
ApplicationMgr().TopAlg = [MySeq]
LHCbApp().EvtMax=-1
from GaudiConf import IOHelper
IOHelper().outStream("recombined.dst",MyWriter)

