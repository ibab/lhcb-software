from PRConfig import TestFileDB
TestFileDB.test_file_db["2011_smallfiles_EW"].run()

from GaudiConf import IOExtension
IOExtension().inputFiles(TestFileDB.test_file_db["2011_smallfiles_EW"].filenames[:5]+TestFileDB.test_file_db["2010_justFSR_EW"].filenames+TestFileDB.test_file_db["2011_smallfiles_EW"].filenames[5:], clear=True)
