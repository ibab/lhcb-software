# Test of UnpackTrack on SDST contaning a track with more than 65k LHCbIDs
from GaudiConf.IOHelper import IOHelper

from Gaudi.Configuration import *
from Configurables import DstConf, DumpTracks, UnpackTrack

upkTr = UnpackTrack()
ApplicationMgr().TopAlg += [ upkTr, DumpTracks() ]

ioh = IOHelper('POOL')
ioh.setupServices()
ioh.inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/00011652_00000001_1-evt-18641to18650.sdst?svcClass=default'])
