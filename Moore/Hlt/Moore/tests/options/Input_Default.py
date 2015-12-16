from Configurables import Moore
from PRConfig.TestFileDB import test_file_db
input = test_file_db['2015HLTValidationData_L0filtered_0x0050']
input.run(configurable=Moore())
