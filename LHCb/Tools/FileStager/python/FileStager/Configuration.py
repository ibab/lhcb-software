

def configureFileStager( keep = False, tmpdir = None ):
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
    if tmpdir:
        tempdir = tmpdir
    elif "TMPDIR" in environ.keys():
        tempdir = environ[ "TMPDIR" ]
    else:
        tempdir = "/tmpdir"
    FileStagerSvc().Tempdir = tempdir
    files = listdir( '.' )
    command = "garbage.exe"
    if command in files:
        command = "./" + command
    FileStagerSvc().GarbageCollectorCommand = command
    FileStagerSvc().KeepFiles = keep

    # Configure other services to use the correct ones
    RawDataCnvSvc( 'RawDataCnvSvc' ).DataManager = mgr.getFullName() 
    EventSelector().StreamManager = "StagedStreamTool"
