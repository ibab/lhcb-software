#!/usr/bin/env python
'''
The idea behind the CondDBUI module is to simplify the usage of PyCool for LHCb users.
This means that you will find here many functions which are specific to the way LHCb
is playing with its conditions database. This means as well that these functions are
not supposed to be efficient (and they are not). For efficient manipulations, we
advise you to use directly the PyCool classes and functions, or (even better) to use
the COOL C++ API.
Please note that most of the functions of the CondDBUI module are used in the conddb
browser.
'''

import os, random, sys, re, time, datetime
from multiprocessing.managers import BaseManager
from multiprocessing import Process, cpu_count

LOG_FORMAT = "%(levelname)s: (%(name)s) %(message)s"

_coolApp = None
def coolApp():
    global _coolApp
    if _coolApp is None:
        from PyCool import cool
        # Initialize COOL Application
        _coolApp = cool.Application()

        # disable CORAL time-out thread
        _coolApp.connectionSvc().configuration().disablePoolAutomaticCleanUp()
        _coolApp.connectionSvc().configuration().setConnectionTimeOut(0)
    return _coolApp


#########################################################################################
#                                    Tag Class                                          #
#########################################################################################

class Tag:
    '''
    Basic class allowing to manipulate more easily the tags in the tag hierarchy.
    The rule is that a tag has only one child tag and can have many parent tag.
    '''
    def __init__(self, tagName, nodePath):
        '''
        Create a new tag object.
        inputs:
            tagName:  string; name of the tag
            nodePath: string; path to the node which own this tag
        outputs:
            none
        '''
        self.name = tagName
        self.path = nodePath
        self.parents = []
        self.child = None

    def connectChild(self, child):
        '''
        Connect a child tag to the current tag, and update the parent list of the child.
        inputs:
            child: Tag object; the child tag object
        outputs:
            none
        '''
        self.child = child
        child.parents.append(self)

    def printAncestors(self, branche = ''):
        '''
        Recursive function printing the relation between the tag and its ancestors.
        inputs:
            branche: string; current status of the ancestor branch. If other ancestors
                     exist, this value is updated. Otherwise, it is printed.
                     -> Default = ''
        outputs:
            none; results are sent to the standard output.
        '''
        s = branche + '/' + self.name
        if self.parents:
            for p in self.parents:
                p.printAncestors(s)
        else:
            print s
        return

    def getAncestorsBranches(self, currentBranche = [], brancheList = None):
        '''
        Recursive function returning the list of ancestor branchs of the tag.
        inputs:
            currentBranche: list of strings; stores the names of the ancestors
                            of the current branch.
                            -> Default = []
            brancheList:    list of lists of strings; variable storing the list
                            of completed ancestor branches.
                            -> Default = None
        outputs:
            brancheList: list of list of strings; each sublist contains a branch of
                         the tag "family".
        '''
        if brancheList == None:
            # Just make sure that brancheList is really a new object.
            brancheList = []

        b = currentBranche[:]
        b.append(self.name)
        if self.parents:
            for p in self.parents:
                p.getAncestorsBranches(b, brancheList)
        else:
            brancheList.append(b)
        return brancheList

    def getAncestors(self):
        '''
        Return the names of the ancestor tags.
        inputs:
            none
        outputs:
            ancestors: list of strings; the names of all the ancestor
                       tags of the current tag. This is equivalent to
                       a list of aliases for this tag.
        '''
        ancestors = []
        branches = self.getAncestorsBranches()
        for b in branches:
            for tagName in b:
                if tagName not in ancestors:
                    ancestors.append(tagName)
        if self.name in ancestors:
            ancestors.remove(self.name)
        return ancestors


    def getAncestorTagsDict(self, currentBranche = {}, brancheList = None):
        '''
        Recursive function returning the list of ancestor tags dictionaries.
        inputs:
            currentBranche: dictionary of tags; stores the the ancestors tags,
                            referenced by names, for the current branch.
                            -> Default = {}
            brancheList:    list of dictionaries; variable storing the list
                            of completed ancestor branches.
                            -> Default = None
        outputs:
            brancheList: list of dictionaries; each sublist contains a branch of
                         the tag "family".
        '''
        if brancheList == None:
            # Just make sure that brancheList is really a new object.
            brancheList = []

        b = currentBranche.copy()
        b[self.name] = self
        if self.parents:
            for p in self.parents:
                p.getAncestorTagsDict(b, brancheList)
        else:
            brancheList.append(b)
        return brancheList


    def getAncestorTags(self):
        '''
        Return the ancestor tags as a list of tag objects.
        inputs:
            none
        outputs:
            ancestors: list of tags; all the ancestor tags of the
                       current tag.
        '''
        ancestors = []
        branches = self.getAncestorTagsDict()
        for b in branches:
            for tagName in b.keys():
                if b[tagName] not in ancestors:
                    ancestors.append(b[tagName])
        if self in ancestors:
            ancestors.remove(self)
        return ancestors


    def __str__(self):
        '''
        Standard string conversion. Returns the name of the tag
        '''
        return self.name


    def __repr__(self):
        '''
        Standard object representation. Returns a string representation of all
        the object's attributes, as well as its relations with its ancestors.
        '''
        output = ' name = %s\n path = %s\n child = %s\n'%(self.name, self.path, str(self.child))

        # display the parents
        if self.parents:
            output += ' parents = '
            for p in self.parents:
                output += '%s, '%str(p)
            # Remove the last comma and space. I don't like this,
            # but adding '\b\b' doesn't work.
            output = output[:-2] + '\n'
        else:
            output += ' parents = []\n'

        # display all the ancestors
        output += ' ancestors = %s\n'%str(self.getAncestors())

        # display the relations with ancestors
        output += ' relations = %s\n'%str(self.getAncestorsBranches())

        return output


#########################################################################################
#                                  CondDB Class                                         #
#########################################################################################

class CondDB(object):
    '''
    Object allowing to manipulate a COOL database object in an LHCb way.
    This object contains a functions to open or create a database. It can then be
    manipulated either directly through the attribute 'db', or via a set of
    functions simplifying some operations, like creation and deletion of nodes, storage
    and retrieval of XML strings, etc.
    '''

    def __init__(self, connectionString = '', create_new_db = False, defaultTag = 'HEAD', readOnly = True):
        """
        Establishes the connection to the COOL database and store the object.
        inputs:
            connectionString: string; standard COOL connection string. An empty string
                              does not initialise the database handle.
                              -> Default = ''
            create_new_db:    boolean; tells the constructor what to do if the connectionString
                              doesn't open an existing database.
                              -> Default = False
            defaultTag:       string; tag which will be used by default if no other is precised.
                              -> Default = 'HEAD'
            readOnly:         boolean; open the conddb in read only mode if True, or read/write
                              mode if False.
                              -> Default = True (or False if the database is created)
        outputs:
            none
        """
        self.defaultTag = 'HEAD'
        self.db         = None
        self.connectionString = os.path.expandvars(connectionString)
        self.readOnly = readOnly
        if self.connectionString == '':
            print "CONDDBUI: WARNING: no database opened"
        else:
            self.openDatabase(self.connectionString, create_new_db, self.readOnly)

    ## REturn representation string for the CondDB instance.
    def __repr__(self):
        repr = self.__class__.__name__ + "(%r" %  self.connectionString
        if not self.defaultTag != "HEAD": repr += ",defaultTag=%r" % self.defaultTag
        if not self.readOnly: repr += ",readOnly=False"
        return repr + ")"
    #---------------------------------------------------------------------------------#

    #=================#
    # Database Access #
    #=================#

    def openDatabase(self, connectionString, create_new_db = False, readOnly = True):
        '''
        Closes the current database and connects to a new one. Creates it if asked to.
        inputs:
            connectionString: string; standard COOL connection string.
            create_new_db:    boolean; if True, creates a new database on failure to
                              connect.
                              -> Default = False
            readOnly:         boolean; open the conddb in read only mode if True, or read/write
                              mode if False.
                              -> Default = True
        outputs:
            none
        '''
        if self.db:
            self.closeDatabase()
        self.connectionString = connectionString
        self.readOnly = readOnly

        # Opening the Database access
        import cppyy
        Helpers = cppyy.gbl.CondDBUI.Helpers

        dbsvc = coolApp().databaseService()
        try:
#            self.db = dbsvc.openDatabase(self.connectionString, self.readOnly)
            self.db = Helpers.openDatabase(dbsvc, self.connectionString, self.readOnly)
            if not self.db: raise Exception, "Database not found: %s"%details
        except Exception, details:
            if create_new_db:
                # if opening has failed, create a new database
                self.createDatabase(self.connectionString)
                self.readOnly = False
#            else:
#                self.db = None
#                raise Exception, "Database not found: %s"%details

    def closeDatabase(self):
        '''
        Close the connection to the opened database
        inputs:
            none
        outputs:
            none
        '''
        if self.db:
            self.db = None

    def createDatabase(self, connectionString):
        '''
        Create a new database and connect to it.
        inputs:
            connectionString: string; standard COOL connection string.
        outputs:
            none
        '''
        # Opening the Database access
        dbsvc = coolApp().databaseService()
        try:
            self.db = dbsvc.createDatabase(connectionString)
        except Exception, details:
            self.db = None
            raise Exception, "Database not created: %s"%details
        else:
            self.connectionString = connectionString
            self.readOnly = False


    #---------------------------------------------------------------------------------#

    #=============================#
    # Database Reading Operations #
    #=============================#

    def isSingleVersionFolder(self, path):
        '''
        Check if path corresponds to a single version folder
        inputs:
            path: string; path to the node to check
        outputs:
            boolean; True if the node is a single version folder, False in all other cases
           (i.e. if the node is a multi version folder OR if it is a folderset or does not
           exist).
        '''
        from PyCool import cool
        assert self.db != None, "Database not connected !"
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
            return folder.versioningMode() == cool.FolderVersioning.SINGLE_VERSION
        else:
            return False

    def isMultiVersionFolder(self, path):
        '''
        Check if path corresponds to a multi version folder
        inputs:
            path: string; path to the node to check
        outputs:
            boolean; True if the node is a multi version folder, False in all other cases
           (i.e. if the node is a single version folder OR if it is a folderset or does not
           exist).
        '''
        from PyCool import cool
        assert self.db != None, "Database not connected !"
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
            return folder.versioningMode() == cool.FolderVersioning.MULTI_VERSION
        else:
            return False

    def setDefaultTag(self, tagName):
        '''
        Set the value of the default tag.
        inputs:
            tagName: string; the name of the default tag.
        outputs:
            none
        '''
        self.defaultTag = tagName

    def getFolderStorageKeys(self, path):
        '''
        Retrieve the keys of the Record Specification for the given folder.
        inputs:
            path: string; path to the folder.
        outputs:
            list of strings; the list of storage keys.
        '''
        assert self.db != None, "No database connected !"
        assert self.db.existsFolder(path), "Folder %s not found"%path
        folder = self.db.getFolder(path)
        return folder.payloadSpecification().keys()

    @staticmethod
    def storeObject(folder, fro, unt, payl, chan):
        import CondDBCompression
#        if 'data' in payl.keys():
        for key in payl:
            xmlString = payl[key]
            if (len(xmlString) and type(xmlString) is str and not xmlString[0].isdigit()):
                cstring = CondDBCompression.compress(xmlString)
                # Only do the compression when the compressed string length is shorter
                if (len(cstring) < len(xmlString)): payl[key] = cstring
        folder.storeObject(fro, unt, payl, chan)

    @staticmethod
    def payload(o):
        import CondDBCompression
        from PyCool import cool
        _payl = o.payload()
#        if (payl and 'data' in payl.keys()):
        if (_payl):
            payl = cool.Record(_payl.specification())
            for key in _payl:
                xmlString = _payl[key]
                if (len(xmlString) and type(xmlString) is str):
                    payl[key] = CondDBCompression.decompress(xmlString)
        return payl


    def getPayload(self, path, when, channelID = 0, tag = ''):
        '''
        Retrieve the payload of the condition object valid at a given time.
        inputs:
            path:      string; path to the condition data in the database.
            when:      integer; time stamp (most likely an event time) at which the
                       value of the condition is requested.
            channelID: integer; ID of the channel in which the condition data are
                       stored.
                       -> Default = 0
            tag:       string; name of the version. If empty, defaultTag is used.
                       -> Default = ''
        outputs:
            dictionary; the contents of the attribute list
        '''

        import logging

        _log = logging.getLogger( "CondDBUI.CondDB.payloadToHash" )
        _log.setLevel( logging.INFO )

        from PyCool import cool
        assert self.db != None, "No database connected !"
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
            try:
                if folder.versioningMode() == cool.FolderVersioning.MULTI_VERSION:
                    if tag == '': tag = self.defaultTag
                    if tag.upper() not in [ '', 'HEAD' ]:
                        # Detection of the existence of the needed tag 
                        obj = folder.findObject(cool.ValidityKey(when), channelID, self.resolveTag(folder, tag))
                    else:
                        obj = folder.findObject(cool.ValidityKey(when), channelID)
                else:
                    obj = folder.findObject(cool.ValidityKey(when), channelID)

            except Exception, details:
                raise Exception, details
            else:
                return dict(self.payload(obj))
        else:
            raise Exception, "Impossible to find folder %s"%path


    def getXMLString(self, path, when, channelID = 0, tag = '', payloadKey = 'data'):
        '''
        Retrieve the XML string of the condition object valid at a given time.
        inputs:
            path:      string; path to the condition data in the database.
            when:      integer; time stamp (most likely an event time) at which the
                       value of the condition is requested.
            channelID: integer; ID of the channel in which the condition data are
                       stored.
                       -> Default = 0
            tag:       string; name of the version. If empty, defaultTag is used.
                       -> Default = ''
            payloadKey: string; key of the Record element we want to retrieve.
                        -> Default = 'data'
        outputs:
            string; the contents of the condition data.
        '''
        assert self.db != None, "No database connected !"
        try:
            payload = self.getPayload(path, when, channelID, tag)
            xmlString = str(payload[payloadKey])
        except Exception, details:
            raise Exception, details
        else:
            return xmlString

    def getPayloadList(self, path, fromTime, toTime, channelID = 0, tag = ''):
        '''
        Retrieve the payload of the condition objects valid during a given time interval.
        inputs:
            path:       string; path to the condition data in the database.
            fromTime:   integer; lower bound of the studied time interval.
            toTime:     integer; upper bound of the studied time interval. Note that an object
                        with start of validity equal to this upper bound value will be returned
                        as well.
            channelIDs: integer; IDs of the channel in which the condition data are
                        stored. If None is given instead, all channels will be browsed.
                        -> Default = 0
            tag:        string; name of the version. If empty, defaultTag is used.
                        -> Default = ''
        outputs:
            list of [dictionary, integer, integer, integer, integer]; the dictionary is the payload.
            The first two integers are the since and until values of the interval of validity. The
            third integer is the channel ID, and the last integer is the insertion time.
        '''
        from PyCool import cool
        assert self.db != None, "No database connected !"
        if channelID != None:
            channelSelection = cool.ChannelSelection(channelID)
        else:
            channelSelection = cool.ChannelSelection()

        objList = []
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
            try:
                if folder.versioningMode() == cool.FolderVersioning.MULTI_VERSION:
                    if tag == '':
                        tag = self.defaultTag
                    if tag.upper() not in [ '', 'HEAD' ]:
                        localtag = Helpers.resolveTag(folder, tag)
#                        if localtag == "": raise RuntimeError('No child tag can be found in node')
                        objIter = folder.browseObjects(cool.ValidityKey(fromTime), cool.ValidityKey(toTime), channelSelection, localtag)
                    else:
                        objIter = folder.browseObjects(cool.ValidityKey(fromTime), cool.ValidityKey(toTime), channelSelection)
                else:
                    objIter = folder.browseObjects(cool.ValidityKey(fromTime), cool.ValidityKey(toTime), channelSelection)
            except Exception, details:
                raise Exception, details

            # Fill the object list
            for obj in objIter:
                payload = dict(self.payload(obj))
                since = obj.since()
                until = obj.until()
                chID = obj.channelId()
                insertTime = obj.insertionTime()
                objList.append([payload, since, until, chID, insertTime])
        return objList


    def getXMLStringList(self, path, fromTime, toTime, channelID = 0, tag = '', payloadKey = 'data'):
        '''
        Retrieve the payload of the condition objects valid during a given time interval.
        inputs:
            path:       string; path to the condition data in the database.
            fromTime:   integer; lower bound of the studied time interval.
            toTime:     integer; upper bound of the studied time interval. Note that an object
                        with start of validity equal to this upper bound value will be returned
                        as well.
            channelIDs: integer; IDs of the channel in which the condition data are
                        stored. If None is given instead, all channels will be browsed.
                        -> Default = 0
            tag:        string; name of the version. If empty, defaultTag is used.
                        -> Default = ''
            payloadKey: string; key of the record element we want to retrieve.
                        -> Default = 'data'
        outputs:
            list of [string, integer, integer, integer, integer]; the string is the payload.
            The first two integers are the since and until values of the interval of validity.
            The third integer is the channel ID, and the last integer is the insertion time.
        '''
        assert self.db != None, "No database connected !"
        try:
            objList = self.getPayloadList(path, fromTime, toTime, channelID, tag)
            for i in range(len(objList)):
                objList[i][0] = objList[i][0][payloadKey]
        except Exception, details:
            raise Exception, details
        else:
            return objList


    def getChildNodes(self, path):
        '''
        Return a list of the children of the given node.
        inputs:
            path: string; path of the parent node. Must be a folderset.
        outputs:
            list of strings; the paths of the child nodes.
        '''
        assert self.db != None, "No database connected !"
        if self.db.existsFolderSet(path):
            folderSet = self.db.getFolderSet(path)
            nodeList = list(folderSet.listFolders()) + list(folderSet.listFolderSets())
            nodeList.sort()
            return nodeList
        else:
            raise Exception, "FolderSet %s not found"%path


    def getAllChildNodes(self, path):
        '''
        Return all the nodes of the tree lying under "path"
        inputs:
            path: string; path of the parent node. Must be a folderset.
        outpus:
            list of strings; the paths of all the elements of the tree under the
            given node.
        '''
        assert self.db != None, "No database connected !"
        if self.db.existsFolderSet(path):
            folderSet = self.db.getFolderSet(path)
            # Get the lists of folders and foldersets
            folderList    = list(folderSet.listFolders())
            folderSetList = list(folderSet.listFolderSets())
            # Merge the lists
            nodeList = folderList + folderSetList
            # Start recursion
            for name in folderSetList:
                nodeList += self.getAllChildNodes(name)
            # Sort the list for easier reading, and return it
            nodeList.sort()
            return nodeList
        else:
            raise Exception, "FolderSet %s not found"%path


    def getAllNodes(self):
        '''
        Convenience function: returns all the nodes of the database.
        inputs:
            none
        outputs:
            list of strings; the paths of all the nodes of the database
        '''
        assert self.db != None, "No database connected !"
        return list(self.db.listAllNodes())

    def getCOOLNode(self, node):
        """
        Get the COOL node at path 'node'.
        This functions checks if the node is a folderset or a folder
        before getting it.
        """
        if self.db.existsFolderSet(node):
            return self.db.getFolderSet(node)
        elif self.db.existsFolder(node):
            return self.db.getFolder(node)
        else:
            return None

    def payloadToMd5(self, path = '/', tag = '', initialMd5Sum = None):
        '''
        Computes the md5 sum for the payload stored under the given node.
        inputs:
            path:   string; path to the top of the database subtree to check.
                    -> Default = '/'
            tag:    string; version of the data to check. If set to '', defaultTag
                    is used. If set to 'ALL', will check all the tags associated
                    to this node (NOT YET IMPLEMENTED !!)
                    -> Default = ''
            md5Sum: md5 object; starting point for the check. If none is given, a
                    new one is created.
                    -> Default = None
        outputs:
            md5 object; result from the md5 check sum.
        '''

        import hashlib
        return payloadToHash(hashlib.md5(), path, tag)

    def payloadToHash(self, initialHashSum = None, path = '/', tag = ''):
        '''
        Computes the hash sum with chosen algorithm for the payload(s) stored under the given node.
        inputs:
            HashSum: hash object; starting point for the check. If none is given, a
                     new one is created using md5 hashing algorithm.
                     -> Default = None
            path:    string; path to the top of the database subtree to check.
                     -> Default = '/'
            tag:     string; version of the data to check. If set to '', defaultTag
                     is used.
                     -> Default = ''
                     @todo: If set to 'ALL', will check all the tags associated
                     to this node (NOT YET IMPLEMENTED !!)

        outputs:
            hash object; result from the hash check sum.
        '''

        import logging

        _log = logging.getLogger( "CondDBUI.CondDB.payloadToHash" )
        _log.setLevel( logging.INFO )

        from PyCool import cool
        assert self.db != None, "No database connected !"

        if tag == 'ALL':
            raise Exception, "Hash checking over all tags is not yet implemented"
        elif tag == '':
            tag = self.defaultTag

        # retrieve the list of nodes to check
        _log.info("Building nodes list to hash ...")
        if self.db.existsFolderSet(path):
            nodes = self.getAllChildNodes(path)
            nodesToCheck = [n for n in nodes if self.resolveTag(n,tag)]
        else:
            nodesToCheck = [path]

        if initialHashSum:
            hashSumObj = initialHashSum.copy()
        else:
            import hashlib
            hashSumObj = hashlib.sha1()

        _log.info("Processing nodes ...")
        for nodeName in nodesToCheck:
            # The check is done only on payload, i.e. we use only the folders,
            # not the foldersets.
            if self.db.existsFolder(nodeName):
                _log.debug("\t" + nodeName)
                folder = self.db.getFolder(nodeName)
                payload = cool.Record(folder.payloadSpecification())
                # Separate the case of single version (no need for tag) and multi
                # version folders.
                if folder.versioningMode() == cool.FolderVersioning.MULTI_VERSION \
                and tag.upper() != 'HEAD':
                    try:
                        objIter = folder.browseObjects(cool.ValidityKeyMin,
                                                       cool.ValidityKeyMax,
                                                       cool.ChannelSelection(),
                                                       self.resolveTag(folder,tag))
                    except Exception, details:
                        raise Exception, details
                else:
                    try:
                        objIter = folder.browseObjects(cool.ValidityKeyMin,
                                                       cool.ValidityKeyMax,
                                                       cool.ChannelSelection())
                    except Exception, details:
                        raise Exception, details
                # Fill the hash checksum
                _log.debug("\tHashing '%s' ..."%nodeName)
                for obj in objIter:
                    for k in payload.keys():
                        hashSumObj.update((self.payload(obj))[k])
        return hashSumObj

    #---------------------------------------------------------------------------------#

    #====================#
    # Tagging Operations #
    #====================#

    def getTagList(self, path):
        '''
        Return all the tag objects defined for the given node.
        inputs:
            path: string; path to the leaf node
        outputs:
            tagList: list of Tag; the list of Tag objects defined for this node.
                     They contains links to their parent Tag objects.
        '''
        assert self.db != None, "No database connected !"
        if not self.db.existsFolderSet(path) and not self.db.existsFolder(path):
            raise Exception, "Node %s was not found"%path
        else:
            tagList = []
            headTag = Tag('HEAD', path)
            tagList.append(headTag)

            # Check if the node is a single version folder.
            if self.isSingleVersionFolder(path):
                return tagList

            # As we are going to use the os.path module, we need to be sure
            # that os.path.sep == '/'
            sep = os.path.sep
            os.path.sep = '/'

            try:
                # Create manager to manipulate shared data
                manager = BaseManager()
                manager.register('TagMP', Tag)
                manager.start()

                # Get all the nodes objects of the given path, and retrieve all the
                # tags defined for them.
                tagDictMP = {}
                nodeTagsDict = {}
                nodeNameList = path.split('/')
                nodePath = '/'
                for nodeName in nodeNameList:
                    nodePath = os.path.join(nodePath, nodeName)
                    node = self.getCOOLNode(nodePath)
                    nodeTagsDict[nodePath] = node.listTags()
                    for tagName in nodeTagsDict[nodePath]:
                        tagDictMP[tagName] = manager.TagMP(tagName, nodePath)
                        tagDictMP[tagName].path = nodePath
                        tagDictMP[tagName].child = None
                        tagDictMP[tagName].parents = []

                # Determine input data decomposition on chunks to feed processes
                chunkSize = len(tagDictMP)//cpu_count() + 1
                chunks_coords = [(k,k + chunkSize)
                                 for k in range(0,len(tagDictMP),chunkSize)]

                # Setup and start parallel processes
                jobs = []
                for coord in chunks_coords:
                    job = Process(target = self._establishTagsRelations,
                                  args = (tagDictMP, coord, nodeTagsDict))
                    jobs.append(job)
                    job.start()

                # Wait for the processes completion
                import errno
                for job in jobs:
                    notintr = False
                    while not notintr:
                        # Avoid waiting interruption due to any signal sent to os.waitpid
                        try:
                            job.join()
                            notintr = True
                        except OSError, ose:
                            if ose.errno != errno.EINTR:
                                raise ose

            finally:
                # recover the original value of os.path.sep
                os.path.sep = sep

            # We return the list of the tags defined for the given node
            for tagName in nodeTagsDict[path]:
                tagList.append(tagDictMP[tagName]._getvalue())

            return tagList

    def _establishTagsRelations(self, tagDict, tagDictCoords, nodeTagsDict):
        # Get coordinates of a tagDict chunk to process
        start, end = tagDictCoords
        # Load process' own database instance to avoid SQLite conflicts with other processes
        db = CondDB(self.connectionString)
        # Look for the parents of all the tags given in tagDict.
        for tagName in tagDict.keys()[start:end]:
            tagProxy = tagDict[tagName]
            tagPath = tagProxy.path
            if tagPath != '/':
                node = db.getCOOLNode(tagPath)
                for parentTagName in nodeTagsDict[os.path.dirname(tagPath)]:
                    try:
                        if tagProxy.child:
                            continue
                        elif tagName == self.findTagRelation(node,parentTagName):
                            tagDict[parentTagName].connectChild(tagProxy)
                    except RuntimeError, x:
                        if str(x).find("No child tag can be found") >= 0:
                            # the folder does not contain one of the parent tags
                            pass
                        else:
                            raise
            else:
                # The tags of the root node have no parents.
                continue
        return


    def createTagRelation(self, path, parentTag, tag):
        '''
        Create a relation between the tag of the given node and a tag
        of its parent node.
        inputs:
            path:      string; path of the node
            parentTag: string; a tag associated to the parent node.
            tag:       string; the tag which we want to relate to the parent tag.
        outputs:
            none
        '''
        from PyCool import cool
        import cppyy
        Helpers = cppyy.gbl.CondDBUI.Helpers

        assert self.db != None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        if type(path) is str:
            if self.db.existsFolder(path):
                node = self.db.getFolder(path)
                if node.versioningMode() == cool.FolderVersioning.SINGLE_VERSION:
                    raise Exception, "Folder %s is Single Version"%path
            elif self.db.existsFolderSet(path):
                node = self.db.getFolderSet(path)
            else:
                raise Exception, "Node %s was not found"%path
        else: node = path
        if not Helpers.createTagRelation(node,parentTag, tag):
            raise Exception, "cool::ReservedHeadTag"


    def deleteTagRelation(self, path, parentTag):
        '''
        Delete a relation between the tag of the given node and a tag
        of its parent node.
        inputs:
            path:      string; path of the node
            parentTag: string; the tag we no longer want to be related to.
        outputs:
            none
        '''
        import cppyy
        Helpers = cppyy.gbl.CondDBUI.Helpers

        assert self.db != None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        if type(path) is str:
            if self.db.existsFolder(path):
                node = self.db.getFolder(path)
            elif self.db.existsFolderSet(path):
                node = self.db.getFolderSet(path)
            else:
                raise Exception, "Node %s was not found"%path
        else: node = path
        if not Helpers.deleteTagRelation(node,parentTag):
            raise Exception, "cool::ReservedHeadTag"


    def generateUniqueTagName(self, baseName, reservedNames = []):
        """
        Generate a random tag name based on a given one.
        inputs:
            baseName:       string; idealy, this is the "parent tag" name. If this name
                            is an automatically generated one (i.e. starting with '_auto_'
                            and finishing with '-' and 6 alphanumeric characters), the
                            function will automatically strip the name from its random
                            parts.
            reservedNames:  list of strings; list of name that can't be chosen.
                            -> Default = []
        outputs:
            string; the generated tag name. Its format is:
            '_auto_' + baseName + '-' + 6 random alphanumeric characters.
        """
        assert self.db != None, "No database connected !"
        # Create the list of ASCII codes for alpha numeric characters
        alphaNumList = range(0x30, 0x3a) + range(0x41, 0x5b) + range(0x61, 0x7b)
        tagName = ''
        while tagName == '' or tagName in reservedNames or self.db.existsTag(tagName):
            tagName = '_auto_' + baseName + '-'
            for i in range(6):
                # Append the 6 alpha-numeric characters
                tagName += '%c'%random.choice(alphaNumList)
        return tagName

    def tagLeafNode(self, path, tagName, description = ''):
        '''
        Apply a new tag to the head of the given folder.
        inputs:
            path:        string; full path to the folder
            tagName:     string; name of the tag to apply. It must be unique in the database.
            description: string; details about the tagging operation.
                         -> Default = ''
        outputs:
            none
        '''
        from PyCool import cool
        assert self.db != None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
        else:
            raise cool.FolderNotFound

        if folder.versioningMode() == cool.FolderVersioning.SINGLE_VERSION:
            raise Exception, "Folder %s is Single Version"%path
        else:
            try:
                folder.tagCurrentHead(tagName, description)
            except Exception, details:
                raise Exception, details


    def recursiveTag(self, path, tagName, description = '', reserved = None):
        '''
        Tag the given node and recursively tag the child nodes and their HEAD revisions
        with randomly generated tags.
        inputs:
            path:         string; full path to the node
            tagName:      string; name of the tag to apply. It must be unique in the database.
            description:  string; details about the tagging operation.
                          -> Default = ''
            reserved:     list of strings; list of reserved tags.
                          -> Default = None
        outputs:
            none
        '''
        from PyCool import cool
        assert self.db != None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        if reserved != None:
            reservedTags = reserved[:]
        else:
            reservedTags = []
        # store the requested tag in the reserved list.
        if tagName not in reservedTags:
            reservedTags.append(tagName)
        if self.db.existsFolderSet(path):
            childNodes = self.getChildNodes(path)
            for nodeName in childNodes:
                # generate a random tag
                if '_auto_' in tagName:
                    baseName = tagName[6:-7]
                else:
                    baseName = tagName
                auto_tag = self.generateUniqueTagName(baseName, reservedTags)
                if self.db.existsFolder(nodeName):
                    # Apply the tag to the HEAD revision of the folder and link it
                    # to the parent tag.
                    folder = self.db.getFolder(nodeName)
                    if folder.versioningMode() == cool.FolderVersioning.MULTI_VERSION:
                        folder.tagCurrentHead(auto_tag, description)
                        try:
                            self.createTagRelation(folder,tagName, auto_tag)
                        except Exception, details:
                            raise Exception, details
                else:
                    # Start recursion for the foldersets and create a tag relation with
                    # the parent.
                    self.recursiveTag(nodeName, auto_tag, description, reservedTags)
                    folderSet = self.db.getFolderSet(nodeName)
                    try:
                        self.createTagRelation(folderSet,tagName, auto_tag)
                    except Exception, details:
                        raise Exception, details
        elif self.db.existsFolder(path):
            node = self.db.getFolder(path)
            if node.versioningMode() == cool.FolderVersioning.MULTI_VERSION:
                node.tagCurrentHead(tagName, description)
        else:
            raise Exception, "Node %s was not found"%path
        # To exit the recursion correctly, we need to return from this function...
        return


    def tagWithAncestorTag(self, path, ancestorTag, description = ''):
        '''
        Recursively tag (with automatically generated tag names) the given node and
        associate the tags with the ancestor tag given.
        inputs:
            path:        string; path of the node to tag
            ancestorTag: string; tag to associate the node with. It must be an existing
                         tag and a relation must exist with the parent of the given node.
                         Otherwise, an exception is raised.
            description: string; description to associate with the tagged elements. If empty,
                         the description of the closest ancestor tag will be used.
                         -> Default = ''
        outputs:
            none
        '''
        from PyCool import cool
        assert self.db != None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        # Check if the ancestor tag really exists.
        if not self.db.existsTag(ancestorTag):
            raise Exception, "Tag %s was not found"%ancestorTag
        # Retrieve the node
        node = self.getCOOLNode(path)
        if node is None:
            raise Exception, "Node %s was not found"%path
        # Check if the node is already related to the ancestor tag
        try:
            relatedTag = self.resolveTag(node,ancestorTag)
        except Exception:
            # The node is not related to the ancestor tag. We are checking if
            # its parent is (it *has* to be).
            try:
                sep = os.path.sep
                os.path.sep = '/'
                parentPath = os.path.dirname(path)
            finally:
                os.path.sep = sep
            parentNode = self.db.getFolderSet(parentPath)

            try:
                parentTag = self.resolveTag(parentNode,ancestorTag)
            except cool.NodeRelationNotFound:
                # The parent folderset doesn't know about the ancestor tag: this
                # means we don't know how to deal with its other child nodes.
                raise Exception, "No relation found between node %s and tag %s"%(parentPath, ancestorTag)
            else:
                # The parent contains a relation to the ancestor tag.
                # Create (recursively if needed) a new tag.
                auto_tag = self.generateUniqueTagName(ancestorTag)
                self.recursiveTag(path, auto_tag, description)
                # Associate with parent tag
                self.createTagRelation(node,parentTag, auto_tag)
        else:
            # The node is already related to the ancestor tag. If the node is a folder,
            # it means that we have to delete the old relation and create a new one.
            # If the node is a folderset, it means that at least one of its child node
            # is related to the ancestor tag as well.
            if self.db.existsFolder(path):
                # get the parent tag related to the ancestor tag:
                try:
                    sep = os.path.sep
                    os.path.sep = '/'
                    parentPath = os.path.dirname(path)
                finally:
                    os.path.sep = sep
                parentNode = self.db.getFolderSet(parentPath)
                parentTag = self.resolveTag(parentNode,ancestorTag)
                # delete the relation between the related tag and the ancestor tag by
                # deleting the relation to the parent tag.
                self.deleteTagRelation(node, parentTag)
                # Create a new tag and connect it to the parent tag
                auto_tag = self.generateUniqueTagName(ancestorTag)
                node.tagCurrentHead(auto_tag, description)
                self.createTagRelation(node,parentTag, auto_tag)
            else:
                # We get all the child nodes, and to avoid the problem of child tags
                # already related to the ancestor tag, we call tagWithAncestorTag
                # recursively. Watch for bugs around here...
                childNodes = self.getChildNodes(path)
                for child in childNodes:
                    self.tagWithAncestorTag(child, ancestorTag, description)
        # To exit the recursion correctly, we need to return from this function...
        return


    def deleteTag(self, path, tagName, delete_relations = True):
        '''
        Delete a tag from the database, and its relations if asked for.
        inputs:
            path:             string; path to the node
            tagName:          string; name of the tag to delete
            delete_relations: boolean; this has a meaning only for folders. If True,
                              delete also the relations with the parent tag.
                              -> Default = True
        outputs:
            none
        '''
        assert self.db != None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        # Retrieve the node
        if self.db.existsFolder(path):
            node = self.db.getFolder(path)
        elif self.db.existsFolderSet(path):
            node = self.db.getFolderSet(path)
        else:
            raise Exception, "Node %s was not found"%path

        if tagName not in list(node.listTags()):
            raise Exception, "Tag %s was not found"%tagName

        # Get the tag object to be able to access parent tags
        tagList = self.getTagList(path)
        tag = tagList.pop()
        while tagList and tag.name != tagName:
            tag = tagList.pop()

        # If the node is a folderset, deleting the tag consists in deleting
        # its relations with parents and children. If it is a folder, deleting relations
        # with the parents is optional, but advisable (and the default behaviour of the
        # function).
        if self.db.existsFolderSet(path):
            for childPath in self.getChildNodes(path):
                if not self.isSingleVersionFolder(childPath):
                    self.deleteTagRelation(childPath, tagName)
            for parentTag in tag.parents:
                self.deleteTagRelation(path, parentTag.name)
        else:
            if delete_relations:
                for parentTag in tag.parents:
                    self.deleteTagRelation(path, parentTag.name)
            node.deleteTag(tagName)

    def isTagReady(self, tagName, path = "/"):
        """
        Check if the given tag name is usable at the level of the specified path.
        inputs:
            tagName:          string; name of the tag to delete
            path:             string; path to the node
                              -> Default = '/'
        outputs:
            none
        """
        if tagName.upper() in ['','HEAD']:
            return True # HEAD tags are always valid
        if self.db.existsFolderSet(path):
            # the path points to a folderset, I check the tag in all its subnodes
            nodes = self.getChildNodes(path)
            try:
                for n in nodes:
                    f = self.getCOOLNode(n)
                    self.resolveTag(f,tagName)
            except:
                return False
        else:
            try:
                f = self.db.getFolder(path)
                self.resolveTag(f,tagName)
            except:
                return False
        return True

    def cloneTag(self, path, src_tag, dest_tag, exclude = []):
        """
        Create a copy of the tag src_tag with the name dest_tag on folderset
        path.
        The sub folder[set] in exclude (specified as fullpath) are not taken
        into account for the new tag.
        """
        if self.readOnly:
            raise "Cannot write on a database opened in read-only mode"
        if self.db.existsFolderSet(path):
            for n in [ self.getCOOLNode(p) for p in self.getChildNodes(path) if p not in exclude ]:
                try:
                    local_tag = self.resolveTag(n,src_tag)
                    #print "%s: %s"%(n.fullPath(),local_tag)
                    self.createTagRelation(n,dest_tag,local_tag)
                except RuntimeError, x:
                    print "Warning: %s"%x
        else:
            if self.db.existsFolder(path):
                print "Warning: cloneTag not supported for folders"
            else:
                raise "Node '%s' does not exist"%path

    def _moveTagOnNodes_recursion(self, nodes_tags, path, basetag, nodestree):
        """
        Internal method to move a tag on a list of nodes.
        nodes_tags: dictionary with the tags to be associated to the folder[set]s.
        path: starting directory
        basetag: tag to use on path
        nodestree: dictionary of dictionaries representing the tree of nodes that
                   need to be changed.
        """
        # Get local tag
        tag = None
        currnode = self.getCOOLNode(path)
        if path in nodes_tags:
            try:
                tag = self.resolveTag(currnode,nodes_tags[path])
            except:
                tag = nodes_tags[path]

        if nodestree: # if it has children
            for n in nodestree:
                nodes_tags[n] = self._moveTagOnNodes_recursion(nodes_tags,n,basetag,nodestree[n])

            # the base node is the only node allowed to have
            # the tag in the nodes_tags dictionary
            if tag is None:
                tag = self.generateUniqueTagName(basetag)
                self.cloneTag(path,basetag,tag,nodestree.keys())

            for n in nodestree:
                x = self.getCOOLNode(n)
                
                try:
                    # try to delete an already present relation
                    self.deleteTagRelation(x,tag)
                except:
                    # if there is not such a relation, ignore the failure
                    pass
                self.createTagRelation(x,tag, nodes_tags[n])
        else:
            # Do a recursive tag if we want HEAD
            if tag is None or tag.upper() == "HEAD":
                tag = self.generateUniqueTagName(basetag)
                self.recursiveTag(path,tag)
        # return the local tag
        return tag

    def moveTagOnNodes(self,basepath,tag,nodes_tags):
        """
        Move (or create) the tag "tag" on the folderset basepath using the tags
        specified in nodes_tags (dictionary) for the sub-folder[set]s.
        If the tag for a sub-folder[set] is "HEAD", then it is automatically
        tagged with recursiveTag.
        """
        # Dictionary representing the tree of nodes to touch
        # leaf nodes are
        basepath = basepath.strip('/')
        if basepath == '':
            bp = []
        else:
            bp = basepath.split('/')
        basepath = '/' + basepath
        nodes_to_modify = {}
        for n in nodes_tags:
            ns = n.strip('/').split('/')
            if not ns[0:len(bp)] == bp:
                raise "Error: node '%s' is not under '%s'"%(n,basepath)
            d = nodes_to_modify
            base = basepath
            for dirname in ns[len(bp):]:
                if base == '/':
                    dirname = base + dirname
                else:
                    dirname = base + '/' + dirname
                if dirname not in d:
                    d[dirname] = {}
                d = d[dirname]
                base = dirname

        nodes_tags[basepath] = tag
        self._moveTagOnNodes_recursion(nodes_tags,basepath,tag,nodes_to_modify)

    def moveTagOnLocalTag(self,basepath,tag,local_tag):
        """
        Move (or create) the tag "tag" on the folderset basepath using the tag
        specified as local_tag.
        It first collects the minimal set of folder(set)s under basepath in which
        the local_tag can be resolved, then calls moveTagOnNodes.
        """
        nodes = self.findNodesWithTag(local_tag, base = basepath, leaves = False)
        if not nodes:
            # nothing to do
            print "Warning: Tag '%s' not found"%local_tag
            return
        nodes_tags = {}
        for n in nodes:
            nodes_tags[n] = local_tag
        self.moveTagOnNodes(basepath,tag,nodes_tags)

    def findTagRelation(self,node,tag):
        """
        Return the local tag associated with the parent tag given.
        """
        import cppyy
        Helpers = cppyy.gbl.CondDBUI.Helpers
        localtag = Helpers.findTagRelation(node, tag)
        if localtag == "": 
            raise RuntimeError('No child tag can be found in node')
            return ""
        return localtag

    def resolveTag(self,path,tag,doraise = True):
        """
        Return the local tag associated with the parent tag given.
        """
        import cppyy
        Helpers = cppyy.gbl.CondDBUI.Helpers

        if type(path) is str:
            doraise = False
            n = self.getCOOLNode(path)
        else:
            n = path
        localtag = Helpers.resolveTag(n, tag)
        if localtag == "": 
            if doraise: raise RuntimeError('No child tag can be found in node')
            else: return None
        return localtag
        #Exception handeling for COOL not useful anymore here
#        try:
#            return n.resolveTag(tag)
#        except:
#            return None

    def findNodesWithTag(self, tag, base = "/", leaves = True):
        """
        Find all the nodes that have a local tag associated with provided parent tag.

        If leaves is True (the default), it returns a list of the folders (leaves).
        If it is False, the list is reduced to a set of foldersets in which the tag
        is completely defined and folders (in case a common folderset couldn be identified).
        """
        # this finds all the leaves in which a tag can be resolved
        l = filter(lambda p: self.isMultiVersionFolder(p) and
                             self.resolveTag(p,tag),
                   self.getAllChildNodes(base))

        if leaves:
            # enough
            return l

        # let's try to find a minimal set of folder(set)s below "base"
        if base == '/':
            base = ['']
        else:
            base = base.split('/')

        base_count = len(base)

        newlist = [] # where to put the extracted bases
        # compare one element with all the others until we exaust the list
        while l:
            current = l[0]
            l.remove(current)
            # do not have to compare something with the base because is guaranteed
            # to be included in each element
            curr_split = current.split('/')[base_count:]

            lcopy = l[:]
            for elem in lcopy:
                elem_split = elem.split('/')[base_count:]
                # find common part between current and elem
                i = 0
                m = min(len(elem_split),len(curr_split))
                while i < m and elem_split[i] == curr_split[i]: i += 1
                if i: # check if they have any element in common
                    candidate = '/'.join(base+curr_split[:i])
                    # here we define the other constraints:
                    #   - it can be a folder (probably overkilling as a check)
                    #   - otherwise the tag must be defined in all the children
                    if ( self.db.existsFolder(candidate) ) \
                       or ( self.isTagReady(tag,candidate) ):
                        l.remove(elem)
                        curr_split = curr_split[:i]
                        current = candidate

            newlist.append(current)
        return newlist


    #---------------------------------------------------------------------------------#

    #=============================#
    # Database Editing Operations #
    #=============================#

    def dropDatabase(cls, connectionString):
        '''
        drop the database identified by the connection string.
        inputs:
            connectionString: string; standard COOL connection string.
        outputs:
            none
        '''
        dbsvc = coolApp().databaseService()
        try:
            dbsvc.dropDatabase(connectionString)
        except Exception, details:
            raise Exception, "Impossible to drop the database: %s"%details

    dropDatabase = classmethod(dropDatabase)

    def createNode(self, path, description = '', storageType = "XML", versionMode = "MULTI", storageKeys = {'data':'String16M'}):
        '''
        Creates a new node (folder or folderset) in the database.
        inputs:
            path:        string; full path of the new node. Parents will be created if necessary.
            description: string; short description of the node.
                         -> Default = ''
            storageType: string; data type to be stored in this node, implying the type of node
                         to create. If the node is a folder, it will contain 'XML'. If it is a
                         folderset, it will contain 'NODE'.
                         -> Default = 'XML'
            versionMode: string; applies to folders only: is it multi version ('MULTI') or single
                         version ('SINGLE') ?
                         -> Default = 'MULTI'
            storageKeys: dictionary mapping strings (names) to strings (COOL storage types);
                         fields that will be stored in the folder.
                         A list of strings can be used in alternative to the dictionary, in which
                         case the type defaults to "String16M".
                         -> Default = {'data': 'String16M'}
        outputs:
            none
        '''
        from PyCool import cool
        assert self.db != None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        if storageType == 'NODE':
            try:
                self.db.createFolderSet(path, description, True)
            except Exception, details:
                raise Exception, "Impossible to create the folderset: %s"%details
        else:
            recordSpec = cool.RecordSpecification()
            if type(storageKeys) is list:
                d = {}
                for k in storageKeys:
                    d[k] = "String16M"
                storageKeys = d
            for key in storageKeys:
                recordSpec.extend(key,
                                  getattr(cool.StorageType, storageKeys[key]))
            
            if versionMode == 'MULTI':
                folderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, recordSpec)
            else:
                folderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, recordSpec)

            # WARNING: this folderdesc stuff is VERY important for LHCb: it tells the CondDB conversion
            #          service which type of converter to call. In this case (storage_type = 7), it calls
            #          the XML converter. It has to be appended to the folder description.
            folderDesc = description + " <storage_type=7>"

            try:
                self.db.createFolder(path, folderSpec, folderDesc, True)
            except Exception, details:
                raise Exception, "Impossible to create the folder: %s"%details


    def storeXMLString(self, path, data, since, until, channelID = 0):
        '''
        Adds a new condition object to the database.
        inputs:
            path:      string; path of the folder where the condition will be stored.
            data:      dictionary; a dictionary version of the attribute list to store in the database.
            since:     integer; lower bound of the interval of validity.
            until:     integer; upper bound of the interval of validity. It is excluded from
                       the interval.
            channelID: integer; ID of the channel where to store the condition.
                       -> Default: 0

        outputs:
            none
        '''
        if type(data) is str:
            data = { 'data': data }
        objDict = {'payload': data,
                   'since':   since,
                   'until':   until,
                   'channel': channelID}
        self.storeXMLStringList(path, [objDict])


    def storeXMLStringList(self, path, XMLList):
        '''
        Allows to store a list of XML string into a given folder.
        inputs:
            path:    string; the path to the folder
            XMLList: list of dictionary of 'payload': dictionary: a dictionary version of the attribute
                                                                  list to store in the database.
                                           'since': integer:      the lower bound of the IOV.
                                           'until': integer:      the upper bound of the IOV.
                                           'channel': integer:    the channel ID.
        outputs:
            none
        '''
        from PyCool import cool
        assert self.db != None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
            # Create a payload object with the correct specifications
            payload = cool.Record(folder.payloadSpecification())
            for key in payload.keys():
                payload[key] = 'nothing'
            # Start filling the buffer...
            folder.setupStorageBuffer()
            for obj in XMLList:
                # Verify that the object can be stored in the folder
                payloadKeys = payload.keys()
                payloadKeys.sort()
                objPayloadKeys = obj['payload'].keys()
                objPayloadKeys.sort()
                assert payloadKeys == objPayloadKeys, "An object's payload specification doesn't fit the folder's one"

                for key in payloadKeys:
                    payload[key] = obj['payload'][key]
                since = cool.ValidityKey(obj['since'])
                until = cool.ValidityKey(obj['until'])
                channelID = obj['channel']
                self.storeObject(folder, since, until, payload, cool.ChannelId(channelID))

            # Write the data to the DB
            folder.flushStorageBuffer()
        else:
            raise Exception, "Folder %s was not found"%path


    def deleteNode(self, path, delete_subnodes = False):
        """
        Delete a node from the database permanently.
        inputs:
            path:            string; node's path in the database
            delete_subnodes: boolean; only useful for fodlersets. If True, all the subnodes
                             will be destroyed as well. If False, a node can be deleted only
                             if it has no children.
                             -> Default = False

        outputs:
            none
        """
        assert self.db != None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        # Deal first with the full tree deletion
        if delete_subnodes and self.db.existsFolderSet(path):
            subnodeList = self.getAllChildNodes(path)
            # Reverse the tree order to start from the subelements
            subnodeList.reverse()
            for node in subnodeList:
                try:
                    self.db.dropNode(node)
                except Exception, details:
                    raise Exception, "Impossible to delete node %s: %s"%(node, details)
        # In all cases, try to delete the given node
        try:
            self.db.dropNode(path)
        except Exception, details:
            raise Exception, "Impossible to delete node %s: %s"%(path, details)

class ValidityKeyWrapper:
    format_re = '(?P<year>(?:19|2[01])[0-9]{2})-(?P<month>(?:0[0-9]|1[0-2]))-(?P<day>[0-3][0-9])[ T]'+ \
                '(?P<hour>(?:[01][0-9]|2[0-3])):(?P<minute>[0-5][0-9]):(?P<s>[0-5][0-9])'+ \
                '(?:\.(?P<ns>[0-9]{0,9}))?'
    def __init__(self,value=None):
        self.value = None
        self.ns = 0
        self.set(value)

    def set(self,value):
        if type(value) == type(''):
            if value.lower().find('inf') >= 0:
                self.value = datetime.datetime.max
                self.ns = 0
            else:
                m = re.match(self.format_re,value)
                if m:
                    self.value = datetime.datetime(int(m.group('year')),int(m.group('month')),int(m.group('day')),
                                                   int(m.group('hour')),int(m.group('minute')),int(m.group('s')))
                    if m.group('ns'):
                        self.ns = int(m.group('ns') + "0" * ( 9 - len(m.group('ns'))))
                    else:
                        ns = 0
                else:
                    raise ValueError("Cannot convert string '%s' to ValidityKey"%value)
        elif value is None: # means now
            self.value = datetime.datetime.now().replace(microsecond=0)
            self.ns = 0
        elif type(value) == type(0) or type(value) == type(0L):
            try:
                self.value = datetime.datetime.fromtimestamp(int(value/1000000000))
                self.ns = value%1000000000
            except ValueError, x:
                if str(x).find('out of range')>=0:
                    self.value = datetime.datetime.max
                    self.ns = 0
                else:
                    raise
        elif type(value) == type(datetime.datetime.max):
            self.value = value

    def toValidityKey(self):
        return self.__long__()

    def __long__(self):
        from PyCool import cool
        try:
            ns = int(time.mktime(self.value.timetuple())*1e9) + self.ns
            if ns < cool.ValidityKeyMin:
                return cool.ValidityKeyMin
            if ns > cool.ValidityKeyMax:
                return cool.ValidityKeyMax
            return ns
        except OverflowError, x:
            if str(x).find('out of range')>=0:
                return cool.ValidityKeyMax
            else:
                raise

    def __str__(self):
        from PyCool import cool
        if self.toValidityKey() == cool.ValidityKeyMax:
            return '+inf'
        s = str(self.value)
        if self.ns:
            s += (".%09d"%self.ns).rstrip('0')
        return s

    def __lt__(self,rhs):
        if type(self) == type(rhs):
            return self.value < rhs.value
        return self.value < rhs

    def __gt__(self,rhs):
        if type(self) == type(rhs):
            return self.value > rhs.value
        return self.value > rhs

    def __le__(self,rhs):
        if type(self) == type(rhs):
            return self.value <= rhs.value
        return self.value <= rhs

    def __ge__(self,rhs):
        if type(self) == type(rhs):
            return self.value >= rhs.value
        return self.value >= rhs

    def __eq__(self,rhs):
        if type(self) == type(rhs):
            return self.value == rhs.value
        return self.value == rhs

    def __ne__(self,rhs):
        if type(self) == type(rhs):
            return self.value != rhs.value
        return self.value != rhs

#########################################################################################
#                                Utility functions                                      #
#########################################################################################

def _collect_tree_info(source_dir, includes = [], excludes = [],
                       include_dirs = [], exclude_dirs = [], includesFirst = True):
    """
    Create a list of folders and foldersets to create starting from a filesystem tree.
        inputs:
            source_dir: string; root node were to start scanning
            includes:      list regular expressions an accepted path must match
            excludes:      list regular expressions to exclude files matching them
            include_dirs:  list regular expressions an accepted dir must match
            exclude_dirs:  list regular expressions to exclude dirs matching them
            includesFirst: if True, first check includes, then excludes; vice-versa if
                           False
        outputs:
            list of folders and foldersets to be created

    """
    # add to the exclude list CVS and back-up files
    exclude_dirs += [ x for x in ['CVS'] if x not in exclude_dirs ]
    excludes += [ x for x in ['.*~', r'.*\.bak'] if x not in excludes ]
    # convert to regular expression objects
    includes = [ re.compile(x) for x in includes]
    include_dirs = [ re.compile(x) for x in include_dirs]
    excludes = [ re.compile(x) for x in excludes]
    exclude_dirs = [ re.compile(x) for x in exclude_dirs]

    name_format = re.compile("(?:([a-zA-Z0-9_.-]*)@)?([a-zA-Z0-9_.-]*)(?::([0-9]+))?$")
    nodes = {}
    for root, dirs, files in os.walk(source_dir):
        base_path = root.replace(source_dir,"")
        if base_path == '': base_path = '/'

        # Check if the base_path is ok or not
        include_match, exclude_match = False, False # default
        for p in include_dirs:
            if p.search(base_path):
                include_match = True
                break
        if len(include_dirs) == 0: include_match = True

        for p in exclude_dirs:
            if p.search(base_path):
                exclude_match = True
                break

        if includesFirst:
            is_good = include_match and not exclude_match
        else:
            is_good = include_match

        if not is_good : continue # ignore the whole dir

        nodes[base_path] = {}

        for f in files :
            # Check if the file_path is ok or not
            include_match, exclude_match = False, False # default
            file_path = os.path.join(base_path,f)

            for p in includes:
                if p.search(file_path):
                    include_match = True
                    break
            if len(includes) == 0: include_match = True

            for p in excludes:
                if p.search(file_path):
                    exclude_match = True
                    break

            if includesFirst:
                is_good = include_match and not exclude_match
            else:
                is_good = include_match

            if not is_good : continue # ignore the file

            m = name_format.match(f)
            if m:
                key,folder,chid = m.groups()
                if not key : key = 'data'
                if not chid : chid = 0

                if folder not in nodes[base_path]:
                    nodes[base_path][folder] = {}

                if key not in nodes[base_path][folder]:
                    nodes[base_path][folder][key] = {}

                nodes[base_path][folder][key][chid] = os.path.join(root,f)
            else:
                print "WARNING: '%s' does not seem in the format [key@]folder[:channel]"%file_path

    return nodes

def _fix_xml(xml_data,folderset_path):
    """
    Function used to clean up the XML files before inserting them in the database.
    It corrects:
     - paths to system ids
     - environment variable expansion
     - encoding (we need iso-8859-1)
    """
    sysIdRE = re.compile('SYSTEM[^>"\']*("[^">]*"|'+"'[^'>]*')")
    def fix_system_ids(xml_data,path):
        data = xml_data
        m = sysIdRE.search(data)
        while m != None:
            pos = m.start()
            s = m.start(1)+1
            e = m.end(1)-1
            if not data[s:e].startswith("conddb:"):
                # replace the system id only if needed
                p = os.path.join(path,data[s:e])
                p = os.path.normpath(p)
                data = data[0:s] + p + data[e:]
            m = sysIdRE.search(data,pos+1)
        return data

    envVarRE = re.compile('\$([A-Za-z][A-Za-z0-9_]*)')
    #cvs_vars = [ 'Id', 'Name', 'Log' ]
    def fix_env_vars(xml_data):
        data = xml_data
        m = envVarRE.search(data)
        while m != None:
            pos = m.start()
            s = m.start(1)
            e = m.end(1)
            name = m.group(1)

            if os.environ.has_key(name):
                val = os.environ[name]
            else:
                val = '$'+name

            data = data[0:pos] + val + data[e:]
            m = envVarRE.search(data,pos+1)
        return data

    import codecs
    encodingRE = re.compile('encoding="([^"]*)"')
    def fix_encoding(xml_data):
        data = xml_data
        m = encodingRE.search(data)
        if m:
            name = m.group(1).lower().replace('utf-','utf_')
            if name != 'iso-8859-1':
                dec = codecs.getdecoder(name)
                enc = codecs.getencoder('iso-8859-1')
                data = enc(dec(data)[0])[0].replace(m.group(1),'ISO-8859-1')
        return data

    xml_data = fix_encoding(xml_data)
    xml_data = fix_system_ids(xml_data,folderset_path)
    xml_data = fix_env_vars(xml_data)
    return xml_data

# FIXME: this wrapper should not be needed anymore
def copy( sourceDb, targetDb,
          nodeName = '/',
          since = None,
          until = None,
          channels = None,
          tags = []
          ):
    """
    Wrapper around PyCoolCopy.copy.
    Was needed because PyCoolCopy did not support LFCReplicaSvc.
    """
    # set defaults
    from PyCool import cool
    if since is None:
        since = cool.ValidityKeyMin
    if until is None:
        until = cool.ValidityKeyMax
    if channels is None:
        channels = cool.ChannelSelection.all()

    import PyCoolCopy
    if type(sourceDb) is str:
        sourceDb = CondDB(sourceDb).db
    if type(targetDb) is str:
        targetDb = CondDB(targetDb,create_new_db=True,readOnly=False).db
    PyCoolCopy.copy(sourceDb, targetDb, nodeName, since, until, channels, tags)

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

def merge( sourceDB, targetDB,
           nodeName = "/",
           since = None,
           until = None,
           channels = None,
           originalTAG = ""
           ):
    """
    Merge the data found in the source database into the target database.
    If the destination folder does not exists, it is created with the same
    metadata of the original one. If it exists, we check if the folders
    have the same metatadata.
    The data are then copied from the source folder into the destination.
    """
    import logging

    _log = logging.getLogger( "CondDBUI.merge" )
    _log.setLevel( logging.INFO )

    # set defaults
    from PyCool import cool
    if since is None:
        since = cool.ValidityKeyMin
    if until is None:
        until = cool.ValidityKeyMax
    if channels is None:
        channels = cool.ChannelSelection.all()

    import PyCool
    from PyCoolDiff import CondDBDiffError

    src = CondDB(sourceDB)
    tgt = CondDB(targetDB, create_new_db = True, readOnly = False)
    # loop over source DB hierarchy
    for root, foldersets, folders in PyCool.walk(src.db,'/'):
        for f in folders:
            if root[-1] == '/':
                folderPath = root + f
            else:
                folderPath = root + '/' + f

            src_folder = src.db.getFolder(folderPath)
            is_single_version = \
                src_folder.versioningMode() == cool.FolderVersioning.SINGLE_VERSION

            _log.info("Processing folder %s",folderPath)

            # check if the folder is new or not
            if tgt.db.existsFolder(folderPath):
                _log.debug("Dest folder exists")
                tgt_folder = tgt.db.getFolder(folderPath)
                # compare folder metadata
                # if ( orig_folder.folderSpecification() !=
                #      dest_folder.folderSpecification() ):
                if ( not payloadSpecEq( src_folder.payloadSpecification(),
		                        tgt_folder.payloadSpecification())
                     ) or (
                     src_folder.versioningMode() !=
                     tgt_folder.versioningMode() ):
                    raise CondDBDiffError("Original and modified folder " +
                                          "metadata (specification and " +
                                          "versioning) do not match",
                                          folderPath)
            else:
                _log.debug("Create dest folder")
                tgt_folder = tgt.db.createFolder(folderPath,
                                                 src_folder.folderSpecification(),
                                                 src_folder.description(),
                                                 True)

            _log.debug("prepare the storage buffer")
            tgt_folder.setupStorageBuffer()

            # get ready to iterate over new data
            if is_single_version or originalTAG.upper() in [ "", "HEAD" ]:
                localTAG = ''
            else:
                localTAG = self.resolveTag(src_folder,originalTAG)

            object_iterator = src_folder.browseObjects( since, until, channels,
                                                        localTAG )
            # loop over the content of the source folder
            for obj in object_iterator:
                _log.debug(str(obj.since()))
                CondDB.storeObject(tgt_folder, obj.since(), obj.until(),
                                       obj.payload(), obj.channelId())
            tgt_folder.flushStorageBuffer()

