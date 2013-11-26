import RawEventCompat
import RawEventCompat.Configuration as conf

from RawEventFormat import Raw_location_db

Raw_location_db[1.0]['DeliberatelyBadBank']="DAQ/RawEvent"

banks=[]
for v in Raw_location_db:
    banks=banks+[b for b in Raw_location_db[v] if b not in banks]

Raw_location_db[0.1]={}
Raw_location_db[999]={}
for b in banks:
    Raw_location_db[0.1][b]="DAQ/RawEvent"
    Raw_location_db[999][b]="Crazy/RawEvent"



conf.RawEventFormatConf().loadIfRequired()

juggle=conf.RawEventJuggler()
juggle.Input=0.1
juggle.Output=999

from Gaudi.Configuration import *
from Configurables import GaudiSequencer

juggle.Sequencer=GaudiSequencer("SPAM")

from Configurables import LHCbApp
LHCbApp()

ApplicationMgr().TopAlg=[GaudiSequencer("SPAM")]

