#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------

ApplicationMgr.ExtSvc = [ 'ParticlePropertySvc',
                          'SicbEventCnvSvc', 
                          'EventSelector' ]

Service('AuditorSvc').Auditors = [ 'ChronoAuditor' ]

#--------------------------------------------------------------
# Persistency services
#--------------------------------------------------------------

Service('EventPersistencySvc').CnvServices  = [ 'SicbEventCnvSvc' ]  
