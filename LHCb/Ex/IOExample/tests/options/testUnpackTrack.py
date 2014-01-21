# Test of UnpackTrack on SDST contaning a track with more than 65k LHCbIDs
from PRConfig import TestFileDB
from Gaudi.Configuration import ApplicationMgr
from Configurables import DumpTracks, UnpackTrack

upkTr = UnpackTrack()
ApplicationMgr().TopAlg += [ upkTr, DumpTracks() ]

TestFileDB.test_file_db["Reco10-sdst-10events"].run()
