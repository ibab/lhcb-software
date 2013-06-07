
from Gaudi.Configuration import *
from Configurables import L0App

############# Moore config taken from printout at the top of the sequence ############

L0App().ReplaceL0Banks=True

L0App().DDDBtag= 'head-20120413'; #from RunDB
L0App().CondDBtag= 'head-20120420'; #from RunDB

L0App().TCK='0x0045' #default for testing

L0App().EvtMax=100

########### L0-related things #######################

#from Configurables import L0MuonAlg; L0MuonAlg( "L0Muon" ).L0DUConfigProviderType = "L0DUConfigProvider"



############# prepend a "input data" ############


from PRConfig import TestFileDB
TestFileDB.test_file_db["2012_raw_default"].run(withDB=False,configurable=L0App())
#sets Simulation, DataType and DDB/CondDB for me... I hope
