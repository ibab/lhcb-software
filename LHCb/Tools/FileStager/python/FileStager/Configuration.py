

def configureFileStager( keep = False, tmpdir = None, garbageCommand = 'garbage.exe' ):
    import os
    if os.name != 'posix': return
    
    from Gaudi.Configuration import ApplicationMgr, EventSelector
    from Configurables import FileStagerSvc
    from Configurables import Gaudi__StagedIODataManager as IODataManager
    from Configurables import LHCb__RawDataCnvSvc as RawDataCnvSvc

    ApplicationMgr().ExtSvc += [ 'FileStagerSvc' ]

    # Remove existing IODataManager
    name = 'Gaudi::IODataManager/IODataManager'
    if name in ApplicationMgr().ExtSvc:
        ApplicationMgr().ExtSvc.remove( name )

    # Remove existing IODataManager
    from Gaudi.Configuration import allConfigurables 
    if "IODataManager" in allConfigurables:
        del allConfigurables[ "IODataManager" ] 
    # Add our datamanager
    mgr = IODataManager( "IODataManager" )

    ApplicationMgr().ExtSvc += [ mgr.getFullName() ]

    from os import environ, listdir
    svc = FileStagerSvc()
    if tmpdir:
        svc.Tempdir = tmpdir
    svc.KeepFiles = keep
    svc.GarbageCollectorCommand = garbageCommand
    svc.CheckForLocalGarbageCollector = True

    # Configure other services to use the correct ones
    RawDataCnvSvc( 'RawDataCnvSvc' ).DataManager = mgr.getFullName() 
    EventSelector().StreamManager = "StagedStreamTool"

    return svc
