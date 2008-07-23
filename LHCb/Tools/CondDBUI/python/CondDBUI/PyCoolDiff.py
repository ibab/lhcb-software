#!/usr/bin/env python
# $ Id: $
from PyCool import cool, walk

class CondDBDiffError(RuntimeError):
    """
    Exception thrown by the PyCoolDiff.diff if a problem occurs during
    the copy.
    """
    def __init__(self,*args):
        RuntimeError.__init__(self,*args);

import logging

_log = logging.getLogger( __name__ )
_log.setLevel( logging.INFO )

# COOL application
_app = None

def diff( originalDB, modifiedDB, diffDB,
          nodeName = "/",
          since = cool.ValidityKeyMin,
          until = cool.ValidityKeyMax,
          channels = cool.ChannelSelection.all(),
          originalTAG = "",
          modifiedTAG = ""):
    """
    Extracts the difference between two tags of one or two databases.
    The result is a database that contains the objects that added to the
    original database will reproduce the data of the second database.
    originalDB, modifiedDB and diffDB have to be COOL connection strings.
    """
    _log.debug("Get COOL Database Service")
    global _app
    if _app is None:
        _app = cool.Application()
        import os
        if 'CORAL_LFC_BASEDIR' in os.environ and 'LFC_HOST' in os.environ and not 'COOL_IGNORE_LFC' in os.environ:
            # Load CORAL LFCReplicaService into the context of cool::Application
            LFCRepSvcName = "CORAL/Services/LFCReplicaService"
            if hasattr(_app,"loadComponent"):
                _app.loadComponent(LFCRepSvcName)
            elif  hasattr(_app,"connectionSvc") and hasattr(_app.connectionSvc(),"configuration"):
                _app.connectionSvc().configuration().setLookupService(LFCRepSvcName)
                _app.connectionSvc().configuration().setAuthenticationService(LFCRepSvcName)
            del LFCRepSvcName

    dbs = _app.databaseService()
    
    _log.info("Opening original database '%s'",originalDB)
    orig = dbs.openDatabase(originalDB)
    
    if originalDB == modifiedDB:
        _log.info("Operating on two tags of the same database")
        dest = orig
    else:
        _log.info("Opening modified database '%s'",modifiedDB)
        dest = dbs.openDatabase(modifiedDB)
    
    diff = None # the DB for differences is opened/created only if needed
    
    _log.debug("Entering the loop over the modified database")
    # walk throughout the database
    for root, dirs, files in walk(dest,nodeName):
        for f in files:
            try:
                if root[-1] == '/':
                    folderPath = root + f
                else:
                    folderPath = root + '/' + f
                
                # vector containing the cool objects to add to the diff DB.
                new_data = []
                
                dest_folder = dest.getFolder(folderPath)
                is_single_version = \
                    dest_folder.versioningMode() == cool.FolderVersioning.SINGLE_VERSION
                
                _log.debug("Processing folder %s",folderPath)
                
                # get ready to iterate over new data
                if is_single_version or modifiedTAG.upper() in [ "", "HEAD" ]:
                    dest_local_tag = ''
                else:
                    dest_local_tag = dest_folder.resolveTag(modifiedTAG)
                    
                object_iterator = dest_folder.browseObjects( since, until, channels,
                                                             dest_local_tag )
                # check if the folder is new or not
                if orig.existsFolder(folderPath):
                    orig_folder = orig.getFolder(folderPath)
                    # compare folder metadata
                    # if ( orig_folder.folderSpecification() !=
                    #      dest_folder.folderSpecification() ):
                    if ( orig_folder.payloadSpecification() !=
                         dest_folder.payloadSpecification()
                         ) or (
                         orig_folder.versioningMode() !=
                         dest_folder.versioningMode() ):
                        raise CondDBDiffError("Original and modified folder " +
                                              "metadata (specification and " +
                                              "versioning) do not match",
                                              folderPath)
                    # compare the content of the folders
                    for obj in object_iterator:
                        try:
                            if is_single_version or originalTAG.upper() in [ "", "HEAD" ]:
                                orig_local_tag = ''
                            else:
                                orig_local_tag = orig_folder.resolveTag(originalTAG)
                            orig_obj = orig_folder.findObject( obj.since(),
                                                               obj.channelId(),
                                                               orig_local_tag )
                            # if the objects are different, prepare to add the new
                            # object
                            if orig_obj.payload() != obj.payload():
                                new_data.append(obj)
                        except:
                            # the original object cannot be found, add the new one
                            new_data.append(obj)
                else:
                    # is a new folder: whe have to copy everything
                    for obj in object_iterator:
                        new_data.append(obj)
            
                # if we have anything to add 
                if new_data:
                    _log.debug("Found %d objects to insert",len(new_data))
                    if diff is None:
                        _log.info("Opening differences database '%s'",diffDB)
                        try:
                            diff = dbs.openDatabase(diffDB,False)
                        except:
                            _log.info("Differences database has to be created")
                            diff = dbs.createDatabase(diffDB)
                    # check if the destination DB contains the new folder
                    if diff.existsFolder(folderPath):
                        diff_folder = diff.getFolder(folderPath)
                        # compare folder metadata
                        # if ( diff_folder.folderSpecification() !=
                        #      dest_folder.folderSpecification() ):
                        if ( diff_folder.payloadSpecification() !=
                             dest_folder.payloadSpecification()
                            ) or (
                             diff_folder.versioningMode() !=
                             dest_folder.versioningMode() ):
                            raise CondDBDiffError("Cannot write to a folder " +
                                                  "that does not match the " +
                                                  "metadata of the source",
                                                  folderPath)
                    else:
                        # otherwise create it
                        # diff_folder = diff.createFolder(folderPath,
                        #                   dest_folder.folderSpecification(),
                        #                   dest_folder.description(), True)
                        diff_folder = diff.createFolder(folderPath,
                                          dest_folder.payloadSpecification(),
                                          dest_folder.description(),
                                          dest_folder.versioningMode(),
                                          True)
                    diff_folder.setupStorageBuffer()
                    for obj in new_data:
                        diff_folder.storeObject(obj.since(),obj.until(),
                                                obj.payload(),obj.channelId())
                    diff_folder.flushStorageBuffer()
                else:
                    _log.debug("No objects to insert")
    
            except Exception, e:
                s = str(e)
                if "No child tag can be found in node" in s \
                  or 'Attempt to access data of NULL attribute "NODE_PARENTID"' in s:
                    pass
                else:
                    raise

    _log.debug("Loop ended")
