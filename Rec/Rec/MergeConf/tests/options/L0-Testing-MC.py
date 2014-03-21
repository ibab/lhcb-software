
from Gaudi.Configuration import *
from Configurables import L0App

############# Moore config taken from printout at the top of the sequence ############

L0App().TCK='0x0045' #default for testing

L0App().EvtMax=100

########### L0-related things #######################

#from Configurables import L0MuonAlg; L0MuonAlg( "L0Muon" ).L0DUConfigProviderType = "L0DUConfigProvider"



############# prepend a "input data" ############


from PRConfig import TestFileDB
TestFileDB.test_file_db["Sim08_2012_ForMoore"].run(configurable=L0App())
#sets Simulation, DataType and DDB/CondDB for me... I hope
