from Gaudi.Configuration import *
from Configurables import Brunel
from PRConfig import TestFileDB
TestFileDB.test_file_db['2015_raw_full'].run(configurable=Brunel())
Brunel().EvtMax = 2000
Brunel().DatasetName="2015magdown"
Brunel().Monitors=["SC","FPE"]


def doIt():
    from Configurables import FastSTDecoding, FastVeloDecoding
    GaudiSequencer("RecoDecodingSeq").Members += [ FastSTDecoding("FastTTDecoding", CompareResult = True, DetectorName = "TT") ]
    GaudiSequencer("RecoDecodingSeq").Members += [ FastSTDecoding("FastITDecoding", CompareResult = True, DetectorName = "IT") ]
    GaudiSequencer("RecoDecodingSeq").Members += [ FastVeloDecoding("FastVeloDecoding", CompareResult = True) ]


appendPostConfigAction( doIt )
