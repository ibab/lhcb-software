import rundb_params, mondb_params
home        = '/home/frankm/HLT2mon'
credentials = 'sqlite:'+home+'/HLT2MONITORING.db'
rundb       = 'oracle:'+rundb_params.login+'@'+rundb_params.tns+'/'+rundb_params.pwd
mondb       = 'oracle:'+mondb_params.login+'@'+mondb_params.tns+'/'+mondb_params.pwd
mondb       = 'sqlite:OFFLINE_MON.db'
