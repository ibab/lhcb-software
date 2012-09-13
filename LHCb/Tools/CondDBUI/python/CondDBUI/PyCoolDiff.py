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


def payloadSpecEq(pl1, pl2):
    if len(pl1) != len(pl2):
        return False
    pl1k, pl2k = pl1.keys(), pl2.keys()
    for k in pl1k:
        if not k in pl2k:
            return False
        if pl1[k] != pl2[k]:
            return False
    return True

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
                    if ( not payloadSpecEq(orig_folder.payloadSpecification(),
                                           dest_folder.payloadSpecification())
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

                            # Get the list of current objects to be covered by new one
                            orig_objects = list(orig_folder.browseObjects( obj.since(),
                                                                           obj.until(),
                                                                           channels,
                                                                           orig_local_tag ))
                            # Remove superfluous object to avoid half-closed
                            # time interval effect while getting objects by IOV
                            if obj.until() == orig_objects[-1].since():
                                del orig_objects[-1]

                            # Skip adding new object on top of current set of
                            # objects if and only if this set is composed of one
                            # object with its payload equal to the new payload.
                            if not (len(orig_objects) == 1 and
                                    orig_objects[0].payload() == obj.payload()):
                                new_data.append(obj)
                        except:
                            # the original object cannot be found, add the new one
                            new_data.append(obj)
                else:
                    # is a new folder: we have to copy everything
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
                        diff_folder.storeObject(max(obj.since(), since),
                                                min(obj.until(), until),
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
