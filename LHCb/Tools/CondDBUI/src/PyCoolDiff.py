#!/usr/bin/env python
# $ Id: $
from PyCool import cool

class CondDBDiffError(RuntimeError):
    """
    Exception thrown by the PyCoolDiff.diff if a problem occurs during
    the copy.
    """
    def __init__(self,*args):
        RuntimeError.__init__(self,*args);

class CondDBWalk:
    """
    Iterator to walk through the hierarchy of a COOL database.
    It mimics the function os.walk, but the navigation cannot
    be modified changing in place the returned directories.
    
    Example:
        for root, foldersets, folders in CondDBWalk(db,'/'):
            ...
    """
    def __init__(self,db,top="/"):
        """
        Construct a new iterator instance.
        db has to be a COOL database instance, and top is the FolderSet
        path where to start the navigation.
        """
        self.db = db
        self.dirs = [ top ]
    def __iter__(self):
        return self
    def next(self):
        if self.dirs:
            # get next folderset to process
            root = self.dirs.pop()
            fs = self.db.getFolderSet(root)
            # get list of contained foldersets in reversed alfabetical order
            dirs = [ f for f in fs.listFolderSets(False) ]
            # prepend the found foldersets to the list of foldersets to process
            self.dirs = dirs + self.dirs
            
            # get the legnth of the part of the name to strip
            if root == '/':
                to_remove = 1
            else:
                to_remove = len(root)+1
            # sort alfabetically the foldersets and leave the FolderSet name
            dirs.sort()
            dirs = [  d[to_remove:] for d in dirs ]
            # get the list of folders and leave only the Folder name
            files = [ f[to_remove:] for f in fs.listFolders() ]
            return (root, dirs, files)
        else:
            # we do not have any other folderset to process
            raise StopIteration()

class BrowseObjectIterator:
    """
    Python iterator to wrap cool::IObjectIterator.
    
    Example:
        for object in BrowseObjectIterator(folder.browseObjects(since,until,
                                                                CannelSelection.all())):
            ...
    """
    def __init__(self,objectIterator):
        """
        Create a new instance of the iterator.
        objectIterator is a cool::IObjectIterator object.
        """
        self.objectIterator = objectIterator
    def __iter__(self):
        return self
    def next(self):
        if self.objectIterator.hasNext():
            return self.objectIterator.next()
        else:
            raise StopIteration()

import logging

_log = logging.getLogger( __name__ )
_log.setLevel( logging.INFO )

_handler = logging.StreamHandler()
_handler.setFormatter( logging.Formatter( "%(levelname)s:%(name)s: %(message)s" ) )
_log.addHandler( _handler )

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
    dbs = cool.DatabaseSvcFactory.databaseService()
    
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
    for root, dirs, files in CondDBWalk(dest,nodeName):
        for f in files:
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
            object_iterator = BrowseObjectIterator(
                                  dest_folder.browseObjects( since, until, channels,
                                                             dest_local_tag ) )
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
    
    _log.debug("Loop ended")
