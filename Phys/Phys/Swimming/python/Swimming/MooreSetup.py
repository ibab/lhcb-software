#General options for swimming the trigger
from Configurables import Moore,HltConfigSvc
#Global configuration
Moore().UseTCK = True
Moore().L0 = True
Moore().ReplaceL0BanksWithEmulated = False
Moore().UseDBSnapshot = False
Moore().EnableRunChangeHandler = False
Moore().CheckOdin = False
Moore().Verbose = False
Moore().EnableDataOnDemand = True
HltConfigSvc().optionsfile = '/tmp/dump.opts'
