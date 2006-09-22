#!/usr/bin/env python
'''
The idea behind the CondDBUI module is to simplify the usage of PyCool for LHCb users.
This means that you will find here many functions which are specific to the way LHCb
is playing with its conditions database. This means as well that these functions are
not supposed to be efficient (and they are not). For efficient manipulations, we
advise you to use directly the PyCool classes and functions, or (even better) to use
the COOL C++ API.
Please note that most of the functions of the conddbui module are used in the conddb
browser.
'''

import os, md5, random, sys

# PyROOT is intercepting the options passed on the command line
# it is better not to have any option while importing PyCool
_tmp_argv = sys.argv
sys.argv = sys.argv[0:1]

from PyCoolCopy import copy, append, Selection, PyCoolCopy
from PyCool import cool, coral

# revert to the original command line options
sys.argv = _tmp_argv

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

class CondDB:
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
                              -> Default = True
        outputs:
            none
        """
        self.defaultTag = 'HEAD'
        self.db         = None
        self.connectionString = connectionString
        self.readOnly = readOnly
        if self.connectionString == '':
            print "CONDDBUI: WARNING: no database opened"
        else:
            self.openDatabase(self.connectionString, create_new_db, self.readOnly)

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
        dbsvc = cool.DatabaseSvcFactory.databaseService()
        try:
            self.db = dbsvc.openDatabase(self.connectionString, self.readOnly)
        except Exception, details:
            if create_new_db:
                # if opening has failed, create a new database
                self.createDatabase(self.connectionString)
            else:
                self.db = None
                raise Exception, "Database not found: %s"%details

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
        dbsvc = cool.DatabaseSvcFactory.databaseService()
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
           (i.e. if the node is a multi version folder OR if it is a folderset or doesn't
           exist).
        '''
        assert self.db <> None, "Database not connected !"
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
            return folder.versioningMode() == cool.FolderVersioning.SINGLE_VERSION
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
        Retrieve the keys of the Coral Attribute List Specification for the given folder.
        inputs:
            path: string; path to the folder.
        outputs:
            list of strings; the list of storage keys.
        '''
        assert self.db <> None, "No database connected !"
        assert self.db.existsFolder(path), "Folder %s not found"%path
        folder = self.db.getFolder(path)
        payloadSpec = folder.payloadSpecification()
        keyList = []
        for i in range(payloadSpec.size()):
            keyList.append(payloadSpec[i].name())
        return keyList


    def getAttributeList(self, path, when, channelID = 0, tag = ''):
        '''
        Retrieve the Coral Attribute List of the condition object valid at a given time.
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
        assert self.db <> None, "No database connected !"
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
            if tag == '':
                if folder.versioningMode() == cool.FolderVersioning.MULTI_VERSION:
                    tag = self.defaultTag
            try:
                obj = folder.findObject(cool.ValidityKey(when), channelID, tag)
            except Exception, details:
                raise Exception, details
            else:
                return dict(obj.payload())
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
            payloadKey: string; key of the coral attribute list element we want to
                        retrieve.
                        -> Default = 'data'
        outputs:
            string; the contents of the condition data.
        '''
        assert self.db <> None, "No database connected !"
        try:
            attList = self.getAttributeList(path, when, channelID, tag)
            xmlString = str(attList[payloadKey])
        except Exception, details:
            raise Exception, details
        else:
            return xmlString

    def getAttributeListList(self, path, fromTime, toTime, channelID = 0, tag = ''):
        '''
        Retrieve the coral attribute list of the condition objects valid during a given time interval.
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
            list of [dictionary, integer, integer, integer, integer]; the dictionary is the coral
            attribute list. The first two integers are the since and until values of the interval
            of validity. The third integer is the channel ID, and the last integer is the insertion
            time.
        '''
        assert self.db <> None, "No database connected !"
        if channelID <> None:
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
                    objIter = folder.browseObjects(cool.ValidityKey(fromTime), cool.ValidityKey(toTime), channelSelection, tag)
                else:
                    objIter = folder.browseObjects(cool.ValidityKey(fromTime), cool.ValidityKey(toTime), channelSelection)
            except Exception, details:
                raise Exception, details

            # Fill the object list
            for i in range(objIter.size()):
                obj = objIter.next()
                attList = dict(obj.payload())
                since = obj.since()
                until = obj.until()
                chID = obj.channelId()
                insertTime = obj.insertionTime()
                objList.append([attList, since, until, chID, insertTime])
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
            payloadKey: string; key of the coral attribute list element we want to
                        retrieve.
                        -> Default = 'data'
        outputs:
            list of [string, integer, integer, integer, integer]; the string is the payload.
            The first two integers are the since and until values of the interval of validity.
            The third integer is the channel ID, and the last integer is the insertion time.
        '''
        assert self.db <> None, "No database connected !"
        try:
            objList = self.getAttributeListList(path, fromTime, toTime, channelID, tag)
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
        assert self.db <> None, "No database connected !"
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
        assert self.db <> None, "No database connected !"
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
        assert self.db <> None, "No database connected !"
        return list(self.db.listAllNodes())


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
        assert self.db <> None, "No database connected !"
        # retrieve the list of nodes to check
        if self.db.existsFolderSet(path):
            nodesToCheck = self.getAllChildNodes(path)
        else:
            nodesToCheck = [path,]
        # Initialise the md5 sum
        if initialMd5Sum:
            md5Sum = initialMd5Sum.copy()
        else:
            md5Sum = md5.new()

        if tag == 'ALL':
            raise Exception, "MD5 check over all tags is not yet implemented"
        elif tag == '':
            tag = self.defaultTag

        for nodeName in nodesToCheck:
            # The check is done only on payload, i.e. we use only the folders,
            # not the foldersets.
            if self.db.existsFolder(nodeName):
                folder = self.db.getFolder(nodeName)
                # Separate the case of single version (no need for tag) and multi
                # version folders.
                if folder.versioningMode() == cool.FolderVersioning.MULTI_VERSION:
                    try:
                        objIter = folder.browseObjects(cool.ValidityKeyMin, cool.ValidityKeyMax, cool.ChannelSelection(), tag)
                    except Exception, details:
                        raise Exception, details
                else:
                    try:
                        objIter = folder.browseObjects(cool.ValidityKeyMin, cool.ValidityKeyMax, cool.ChannelSelection())
                    except Exception, details:
                        raise Exception, details
                # Fill the md5 checksum
                for i in range(objIter.size()):
                    obj = objIter.next()
                    md5Sum.update(obj.payload()['data'])
        return md5Sum

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
        assert self.db <> None, "No database connected !"
        if not self.db.existsFolderSet(path) and not self.db.existsFolder(path):
            raise Exception, "Node %s was not found"%path
        else:
            tagList = []
            headTag = Tag('HEAD', path)
            tagList.append(headTag)

            # we check if the node is a single version folder.
            if self.isSingleVersionFolder(path):
                return tagList

            # As we are going to use the os.path module, we need to be sure
            # that os.path.sep == '/'
            try:
                sep = os.path.sep
                os.path.sep = '/'
                # we get all the nodes objects of the given path, and retrieve all the
                # tags defined for them.
                nodeDict = {}
                tagDict  = {}
                nodeNameList = path.split('/')
                nodePath = '/'
                for nodeName in nodeNameList:
                    nodePath = os.path.join(nodePath, nodeName)
                    if self.db.existsFolderSet(nodePath):
                        nodeDict[nodePath] = self.db.getFolderSet(nodePath)
                    else:
                        nodeDict[nodePath] = self.db.getFolder(nodePath)
                    for tagName in list(nodeDict[nodePath].listTags()):
                        tagDict[tagName] = Tag(tagName, nodePath)
                # We look for the parents of all the retrieved tags.
                for tagName in tagDict.keys():
                    tag = tagDict[tagName]
                    if tag.path == '/':
                        # The tags of the root node have no parents.
                        continue
                    else:
                        node = nodeDict[tag.path]
                        parentNode = nodeDict[os.path.dirname(tag.path)]
                        for parentTagName in parentNode.listTags():
                            if tagDict[parentTagName].child:
                                continue
                            elif tag.name == node.findTagRelation(parentTagName):
                                tagDict[parentTagName].connectChild(tag)
            finally:
                # recover the original value of os.path.sep
                os.path.sep = sep

            # We return the list of the tags defined for the given node
            for tagName in nodeDict[path].listTags():
                tagList.append(tagDict[tagName])
            return tagList


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
        assert self.db <> None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        if self.db.existsFolder(path):
            node = self.db.getFolder(path)
            if node.versioningMode() == cool.FolderVersioning.SINGLE_VERSION:
                raise Exception, "Folder %s is Single Version"%path
        elif self.db.existsFolderSet(path):
            node = self.db.getFolderSet(path)
        else:
            raise Exception, "Node %s was not found"%path
        node.createTagRelation(parentTag, tag)


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
        assert self.db <> None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        if self.db.existsFolder(path):
            node = self.db.getFolder(path)
        elif self.db.existsFolderSet(path):
            node = self.db.getFolderSet(path)
        else:
            raise Exception, "Node %s was not found"%path
        node.deleteTagRelation(parentTag)


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
        assert self.db <> None, "No database connected !"
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
        assert self.db <> None, "No database connected !"
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
        assert self.db <> None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        if reserved <> None:
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
                            folder.createTagRelation(tagName, auto_tag)
                        except Exception, details:
                            raise Exception, details
                else:
                    # Start recursion for the foldersets and create a tag relation with
                    # the parent.
                    self.recursiveTag(nodeName, auto_tag, description, reservedTags)
                    folderSet = self.db.getFolderSet(nodeName)
                    try:
                        folderSet.createTagRelation(tagName, auto_tag)
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
        assert self.db <> None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        # Check if the ancestor tag really exists.
        if not self.db.existsTag(ancestorTag):
            raise Exception, "Tag %s was not found"%ancestorTag
        # Retrieve the node
        if self.db.existsFolder(path):
            node = self.db.getFolder(path)
        elif self.db.existsFolderSet(path):
            node = self.db.getFolderSet(path)
        else:
            raise Exception, "Node %s was not found"%path
        # Check if the node is already related to the ancestor tag
        try:
            relatedTag = node.resolveTag(ancestorTag)
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
                parentTag = parentNode.resolveTag(ancestorTag)
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
                node.createTagRelation(parentTag, auto_tag)
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
                parentTag = parentNode.resolveTag(ancestorTag)
                # delete the relation between the related tag and the ancestor tag by
                # deleting the relation to the parent tag.
                node.deleteTagRelation(parentTag, relatedTag)
                # Create a new tag and connect it to the parent tag
                auto_tag = self.generateUniqueTagName(ancestorTag)
                node.tagCurrentHead(auto_tag, description)
                node.createTagRelation(parentTag, auto_tag)
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
        assert self.db <> None, "No database connected !"
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
        while tagList and tag.name <> tagName:
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
        dbsvc = cool.DatabaseSvcFactory.databaseService()
        try:
            dbsvc.dropDatabase(connectionString)
        except Exception, details:
            raise Exception, "Impossible to drop the database: %s"%details

    dropDatabase = classmethod(dropDatabase)

    def createNode(self, path, description = '', storageType = "XML", versionMode = "MULTI", storageKeys = ['data']):
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
            storageKeys: list of strings; the keys of the attribute list that will be stored in the folder.
                         -> Default = ['data']
        outputs:
            none
        '''
        assert self.db <> None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        if storageType == 'NODE':
            try:
                self.db.createFolderSet(path, description, True)
            except Exception, details:
                raise Exception, "Impossible to create the folderset: %s"%details
        else:
            if versionMode == 'MULTI':
                folderVersion = cool.FolderVersioning.MULTI_VERSION
            else:
                folderVersion = cool.FolderVersioning.SINGLE_VERSION

            folderSpec = cool.ExtendedAttributeListSpecification()
            for key in storageKeys:
                folderSpec.push_back(key,"string",cool.PredefinedStorageHints.STRING_MAXSIZE_16M)

            # WARNING: this folderdesc stuff is VERY important for LHCb: it tells the CondDB conversion
            #          service which type of converter to call. In this case (storage_type = 7), it calls
            #          the XML converter. It has to be appended to the folder description.
            folderDesc = description + " <storage_type=7>"

            try:
                self.db.createFolderExtended(path, folderSpec, folderDesc, folderVersion, True)
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
        assert self.db <> None, "No database connected !"
        assert not self.readOnly , "The database is in Read Only mode."
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
            # Create a payload object with the correct specifications
            payload = coral.AttributeList(folder.payloadSpecification())
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
                folder.storeObject(since, until, payload, channelID)

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
        assert self.db <> None, "No database connected !"
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

#########################################################################################
#                                       TESTS                                           #
#########################################################################################

def testDBAccess(connectionString):
    '''
    Create a CondDB object, connect to a database and show its contents
    '''
    bb = CondDB(connectionString, readOnly = False)

    print
    print "-> List of all database nodes: ", bb.getAllNodes()
    print
    CondDB.dropDatabase(connectionString)


def testNodeCreation(connectionString):
    '''
    Connect to a database and create the nodes given in the nodeList
    '''
    nodeList = []
    nodeList.append(['/a','Folderset','NODE',''])
    nodeList.append(['/a/multi','Folder','XML','MULTI'])
    nodeList.append(['/a/single','Folder','XML','SINGLE'])
    nodeList.append(['/b/c','Folder','XML','MULTI'])
    nodeList.append(['/b/c/d','Folder','XML','MULTI']) # this one must fail: /b/c is a folder
    bb = CondDB(connectionString, readOnly = False)

    for nodeParams in nodeList:
        path        = nodeParams[0]
        description = nodeParams[1]
        storageType = nodeParams[2]
        versionMode = nodeParams[3]
        try:
            bb.createNode(path, description, storageType, versionMode)
        except Exception, details:
            print details
    print
    print "-> List of all database nodes: ", bb.getAllNodes()
    print
    CondDB.dropDatabase(connectionString)


def testXMLStorage(connectionString):
    '''
    connect to a db, create a folder and store a condition object in it
    '''
    folder = ['/a/b/c', 'Folder to store XML strings', 'XML', 'MULTI']
    obj    = ['value = 1', 0, 10, 0]
    bb     = CondDB(connectionString, readOnly = False)

    try:
        bb.createNode(folder[0], folder[1], folder[2], folder[3])
    except Exception, details:
        print details
    path      = folder[0]
    data      = obj[0]
    since     = obj[1]
    until     = obj[2]
    channelID = obj[3]
    try:
        bb.storeXMLString(path, data, since, until, channelID)
    except Exception, details:
        print details
    try:
        value = bb.getXMLString(path, (since + until)/2, channelID, 'HEAD')
    except Exception, details:
        print details
    else:
        print
        print "-> Condition value at time ", (since + until)/2, ": ", value
        print
    CondDB.dropDatabase(connectionString)


def testXMLListStorage(connectionString):
    '''
    connect to a db, create a folder and store a condition object list in it
    '''
    folder  = ['/a/b/c', 'Folder to store a list of conditions', 'XML', 'MULTI']
    objList = []
    for i in range(100):
        objList.append(['value '+str(i), 10*i, 10*(i+1), 0])
    for i in range(100):
        objList.append(['value '+str(i), 15*i, 15*(i+1), 1])
    bb = CondDB(connectionString, readOnly = False)

    try:
        bb.createNode(folder[0], folder[1], folder[2], folder[3])
    except Exception, details:
        print details
    path = folder[0]
    try:
        bb.storeXMLStringList(path, objList)
    except Exception, details:
        print details

    objList = bb.getXMLStringList(path, cool.ValidityKeyMin, cool.ValidityKeyMax, None, 'HEAD')
    print
    for obj in objList:
        print "-> Condition value in channel ", obj[3], ": ", obj[0]
    print
    CondDB.dropDatabase(connectionString)


def testMD5(connectionString):
    '''
    connect to a db and compute an md5 checksum
    '''
    folder  = ['/a/b/c', 'Folder to store a list of conditions', 'XML', 'MULTI']
    objList = []
    for i in range(100):
        objList.append(['value '+str(i), 10*i, 10*(i+1), 0])
    for i in range(100):
        objList.append(['value '+str(i), 15*i, 15*(i+1), 1])
    bb = CondDB(connectionString, readOnly = False)

    try:
        bb.createNode(folder[0], folder[1], folder[2], folder[3])
    except Exception, details:
        print details
    path = folder[0]
    try:
        bb.storeXMLStringList(path, objList)
    except Exception, details:
        print details

    md5Sum = bb.payloadToMd5('/', 'HEAD')
    print
    print "-> md5 CheckSum = ", md5Sum.hexdigest()
    print
    CondDB.dropDatabase(connectionString)


def testRemoveNode(connectionString):
    '''
    connect to a db and remove a node from it
    '''
    bb = CondDB(connectionString, readOnly = False)

    bb.createNode('/a', '', 'XML', 'MULTI')
    bb.createNode('/b/c', '', 'XML', 'MULTI')
    bb.createNode('/d', '', 'XML', 'MULTI')
    print
    print "-> Nodes of the database before removal: ", bb.getAllNodes()
    bb.deleteNode('/b', True)
    print "-> Nodes of the database after removal: ", bb.getAllNodes()
    print
    CondDB.dropDatabase(connectionString)


def testTagLeafNode(connectionString):
    '''
    Connect to a db, create a dummy folder with dummy condition and tag the HEAD
    '''
    tagName = 'dummyTag'
    bb = CondDB(connectionString, readOnly = False)

    bb.createNode('/a', "Dummy Folder", 'XML', 'MULTI')
    bb.storeXMLString('/a', "dummy condition value", cool.ValidityKeyMin, cool.ValidityKeyMax, 0)
    bb.tagLeafNode('/a', tagName, "dummy tag")
    bb.storeXMLString('/a', "another dummy condition value", cool.ValidityKeyMin, cool.ValidityKeyMax, 0)
    print
    print "-> HEAD value = ", bb.getXMLString('/a', 1000, 0, 'HEAD')
    print "-> %s value = "%tagName, bb.getXMLString('/a', 1000, 0, tagName)
    print
    CondDB.dropDatabase(connectionString)

def testRecursiveTag(connectionString):
    '''
    Connect to a db, create a node architecture and recursively tag the root folderset
    and its children.
    '''
    bb = CondDB(connectionString, readOnly = False)

    bb.createNode('/a/b/c', "Folder for recursive tagging test", 'XML', 'MULTI')
    bb.storeXMLString('/a/b/c', "condition value", cool.ValidityKeyMin, cool.ValidityKeyMax, 0)
    bb.recursiveTag('/', 'version1', "recursive tag")
    bb.recursiveTag('/', 'blabla2', "other recursive tag")
    allNodes = bb.getAllNodes()
    print
    for nodeName in allNodes:
        if bb.db.existsFolder(nodeName):
            node = bb.db.getFolder(nodeName)
        else:
            node = bb.db.getFolderSet(nodeName)
        print "-> Tags for ", nodeName, ": ", list(node.listTags())
    print
    CondDB.dropDatabase(connectionString)

def testTagWithAncestorTag(connectionString):
    '''
    Connect to the db, create a node architecture and tag some tree elements using
    ancestor tags.
    '''
    bb = CondDB(connectionString, readOnly = False)

    bb.createNode('/a/b/c', "Folder for ancestor tagging test", 'XML', 'MULTI')
    bb.createNode('/a/bb', "FolderSet for ancestor tagging test", 'NODE')
    bb.storeXMLString('/a/b/c', "value 1 for /a/b/c", cool.ValidityKeyMin, cool.ValidityKeyMax, 0)
    bb.recursiveTag('/', 'version1', "version 1")

    allNodes = bb.getAllNodes()
    print
    for nodeName in allNodes:
        if bb.db.existsFolder(nodeName):
            node = bb.db.getFolder(nodeName)
        else:
            node = bb.db.getFolderSet(nodeName)
        print "-> Tags for ", nodeName, ": ", list(node.listTags())
    print

    bb.createNode('/a/bb/d', "Folder for ancestor tagging test", 'XML', 'MULTI')
    bb.storeXMLString('/a/bb/d', "value 1 for /a/b/d", cool.ValidityKeyMin, cool.ValidityKeyMax, 0)
    bb.tagWithAncestorTag('/a/bb', 'version1')
    allNodes = bb.getAllNodes()
    print
    for nodeName in allNodes:
        if bb.db.existsFolder(nodeName):
            node = bb.db.getFolder(nodeName)
        else:
            node = bb.db.getFolderSet(nodeName)
        print "-> Tags for ", nodeName, ": ", list(node.listTags())
    print
    CondDB.dropDatabase(connectionString)

def testGetTagList(connectionString):
    '''
    Connect to the db, create a node architecture and tag some tree elements, then
    retrieve some tag list.
    '''
    bb = CondDB(connectionString, readOnly = False)

    bb.createNode('/a/b/c', "Folder for ancestor tagging test", 'XML', 'MULTI')
    bb.storeXMLString('/a/b/c', "value 1 for /a/b/c", cool.ValidityKeyMin, cool.ValidityKeyMax, 0)
    bb.tagLeafNode('/a/b/c', 'c-v1', 'first version')
    bb.storeXMLString('/a/b/c', "value 2 for /a/b/c", cool.ValidityKeyMin, cool.ValidityKeyMax, 0)
    bb.tagLeafNode('/a/b/c', 'c-v2', 'second version')

    bb.createTagRelation('/a/b/c', 'b-v1', 'c-v1')
    bb.createTagRelation('/a/b/c', 'b-v2', 'c-v1')
    bb.createTagRelation('/a/b/c', 'b-v3', 'c-v2')

    bb.createTagRelation('/a/b', 'a-v1', 'b-v1')
    bb.createTagRelation('/a/b', 'a-v3', 'b-v1')
    bb.createTagRelation('/a/b', 'a-v2', 'b-v2')
    bb.createTagRelation('/a/b', 'a-v4', 'b-v3')

    bb.createTagRelation('/a', 'v1', 'a-v1')
    bb.createTagRelation('/a', 'v2', 'a-v3')

    tagList = bb.getTagList('/a/b')
    print
    print '-> list of tags:'
    for tag in tagList:
        print repr(tag)
    print

    CondDB.dropDatabase(connectionString)


#########################################################################################
#                                        MAIN                                           #
#########################################################################################

def run_tests():
    import os
    # create a temporary file to use for the tests
    from tempfile import mkstemp
    (filehandle, filepath) = mkstemp(dir="/tmp")
    os.close(filehandle) # the returned file is already open, so I close it
    
    # prepare the connection string
    connectionString = "sqlite://;schema=%s;dbname=TEST"%filepath
    try:

#         print "***************************\n"
#         print "1st test: Database Access\n"
#         testDBAccess(connectionString)
#         print "***************************\n"
        
#         print "**************************\n"
#         print "2nd test: Node Creation\n"
#         testNodeCreation(connectionString)
#         print "**************************\n"
        
#         print "**************************\n"
#         print "3rd test: XML storage\n"
#         testXMLStorage(connectionString)
#         print "**************************\n"
        
#         print "**************************\n"
#         print "4th test: XML list storage\n"
#         testXMLListStorage(connectionString)
#         print "**************************\n"
        
#         print "**************************\n"
#         print "5th test: md5Checksum\n"
#         testMD5(connectionString)
#         print "**************************\n"
        
#         print "**************************\n"
#         print "6th test: node removal\n"
#         testRemoveNode(connectionString)
#         print "**************************\n"
        
#         print "**************************\n"
#         print "7th test: leaf tagging\n"
#         testTagLeafNode(connectionString)
#         print "**************************\n"
        
#         print "**************************\n"
#         print "8th test: recursive tagging\n"
#         testRecursiveTag(connectionString)
#         print "**************************\n"
        
#         print "**************************\n"
#         print "9th test: ancestor tagging\n"
#         testTagWithAncestorTag(connectionString)
#         print "**************************\n"
        
        print "**************************\n"
        print "10th test: tag listing\n"
        testGetTagList(connectionString)
        print "**************************\n"
        
    finally:
        # delete the temporary file once finished
        os.remove(filepath)
        
if __name__ == '__main__':
    run_tests()
