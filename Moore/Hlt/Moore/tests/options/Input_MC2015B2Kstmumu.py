from Configurables import Moore
from PRConfig.TestFileDB import test_file_db
input = test_file_db['MC2015_digi_nu2.6_B2Kstmumu_L0']
input.run(configurable=Moore())
