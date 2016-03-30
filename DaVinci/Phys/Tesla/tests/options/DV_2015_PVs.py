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

tes = gaudi.evtsvc()

vertLoc = "Turbo/Primary"

version='v10r0_0x00fa0051'
from TurboStreamProd.helpers import *
from TurboStreamProd import prodDict
lines = streamLines(prodDict,version,'DiMuon',debug=True)
lines += streamLines(prodDict,version,'Charm',debug=True)
lines += streamLines(prodDict,version,'CharmSpec',debug=True)

while True:
    gaudi.run(1)
    #
    if not tes['/Event/DAQ/RawEvent']:
        print "End of file"
        break
    #
    n=0
    for line in lines:
        rep = tes["Hlt2/SelReports"].selReport(line+"Decision")
        if rep:
            n+=1
    if n==0:
        continue

    if not tes[vertLoc]:
        print tes["Hlt2/SelReports"]
        print "ERROR: PVs have not been created"
        break
