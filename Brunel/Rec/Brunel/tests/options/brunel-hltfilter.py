from Configurables import Brunel
from PRConfig import TestFileDB

TestFileDB.test_file_db['2015_pbpb_raw_full'].run(configurable=Brunel())

Brunel().EvtMax = 16
Brunel().DatasetName = "2015pbpb"
Brunel().Monitors = ["SC", "FPE"]

Brunel().Hlt1FilterCode = ""
Brunel().Hlt2FilterCode = "HLT_PASS_RE('Hlt2BBPassThroughDecision')"
