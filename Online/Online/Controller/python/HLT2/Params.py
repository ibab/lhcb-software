import rundb_params
home        = '/home/frankm/HLT2mon'
credentials = 'sqlite:'+home+'/HLT2MONITORING.db'
rundb       = 'oracle:'+rundb_params.login+'@'+rundb_params.tns+'/'+rundb_params.pwd
