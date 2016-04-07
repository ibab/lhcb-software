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
    ### Check all hypos and clusters have valid positions
    for line in lines:
        parts = TES["Turbo/"+line+"/Particles"]
        if hasattr(parts,"__iter__"):
            for p in parts:
                if p.proto():
                    for hypo in p.proto().calo():
                        hypopos = hypo.target().position()
                        if ( hypopos.x()**2 + hypopos.y()**2 + hypopos.z()**2 + hypopos.e()**2 ) < 5.0:
                            passed=False
                            print "Invalid hypo position"
                            break
                        for cluster in hypo.target().clusters():
                            clusterpos = cluster.target().position()
                            if ( clusterpos.x()**2 + clusterpos.y()**2 + clusterpos.z()**2 + clusterpos.e()**2 ) < 5.0:
                                passed=False
                                print "Invalid cluster position"
                                break
                            n+=1
                        n+=1
                if passed==False:
                    break
        if passed==False:
            break
print "Analysed "+str(n)+" calo classes successfully"


