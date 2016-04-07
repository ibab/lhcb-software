from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer,RawEventJuggler
from Configurables import DaVinci
from Configurables import DecodeRawEvent

from Configurables import DstConf
DstConf().Turbo=True

DaVinci()
DaVinci().EvtMax=-1
DaVinci().DataType="2015"

from GaudiConf import IOHelper
IOHelper().inputFiles( ["tesla_2015_TCK.dst"] , clear=True ) 

import GaudiPython
from Gaudi.Configuration import ApplicationMgr
from Configurables import LoKiSvc

from GaudiKernel import ROOT6WorkAroundEnabled
if ROOT6WorkAroundEnabled('ROOT-7492'):
    # trigger autoloading of LHCbKernel dictionary
    GaudiPython.gbl.LHCb.LHCbID
    # trigger autoloading of DigiEvent dictionary
    GaudiPython.gbl.LHCb.CaloDigit

gaudi = GaudiPython.AppMgr()
gaudi.initialize()

TES = gaudi.evtsvc()

version='v10r0_0x00fa0051'
from TurboStreamProd.helpers import *
from TurboStreamProd import prodDict
lines = streamLines(prodDict,version,'DiMuon',debug=True)
lines += streamLines(prodDict,version,'Charm',debug=True)
lines += streamLines(prodDict,version,'CharmSpec',debug=True)

n=0
passed=True
while True:
    gaudi.run(1)
    if not TES['DAQ/RawEvent']: break # probably end of file
    
    for line in lines:
        parts = TES["Turbo/"+line+"/Particles"]
        if hasattr(parts,"__iter__"):
            for p in parts:
                if p.proto():
                    if p.proto().track():
                        ### Charged particle should have an associated RICH PID with track
                        if p.proto().track()!=p.proto().richPID().track():
                            passed=False
                            break
                        n+=1
        if passed==False:
            break
print "Analysed "+str(n)+" particles successfully"


