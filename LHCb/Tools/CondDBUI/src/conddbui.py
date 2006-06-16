#!/usr/bin/env python

import os, md5, random, sys

# PyROOT is intercepting the options passed on the command line
# it is better not to have any option while importing PyCool
_tmp_argv = sys.argv
sys.argv = sys.argv[0:1]

from PyCoolCopy import copy, Selection, PyCoolCopy
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
            brancheList:    list of lists of strings; variable storing the list
                            of completed ancestor branches.
            currentBranche: list of strings; stores the names of the ancestors
                            of the current branch.
                            -> Default = []
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
    Object allowing to manipulate a COOL database object on an LHCb way.
    This object contains a functions to open or create a database. It can then be
    manipulated either directly through the attribute 'db', or via a set of
    functions simplifying some operations, like creation and deletion of nodes, storage
    and retrieval of XML strings, etc.
    '''

    def __init__(self, connectionString = '', create_new_db = True, defaultTag = 'HEAD'):
        """
        Establishes the connection to the COOL database and store the object.
        inputs:
            connectionString: string; standard COOL connection string. An empty string
                              does not initialise the database handle.
                              -> Default = ''
            create_new_db:    boolean; tells the constructor what to do if the connectionString
                              doesn't open an existing database.
                              -> Default = True
            defaultTag:       string; tag which will be used by default if no other is precised.
                              -> Default = 'HEAD'
        outputs:
            none
        """
        self.defaultTag = 'HEAD'
        self.db         = None
        if connectionString == '':
            print "CONDDBUI: WARNING: no database opened"
        else:
            self.openDatabase(connectionString, create_new_db)

    #---------------------------------------------------------------------------------#

    #=================#
    # Database Access #
    #=================#

    def openDatabase(self, connectionString, create_new_db = False):
        '''
        Closes the current database and connects to a new one. Creates it if asked to.
        inputs:
            connectionString: string; standard COOL connection string.
            create_new_db:    boolean; if True, creates a new database on failure to
                              connect.
                              -> Default = False
        outputs:
            none
        '''
        if self.db:
            self.closeDatabase()

        # Opening the Database access
        dbsvc = cool.DatabaseSvcFactory.databaseService()
        try:
            self.db = dbsvc.openDatabase(connectionString)
        except Exception, details:
            if create_new_db:
                # if opening has failed, create a new database
                self.createDatabase(connectionString)
            else:
                self.db = None
                raise Exception, "CONDDBUI: the database was not found: %s"%details

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
            raise Exception, "CONDDBUI: Impossible to create the database: %s"%details
        else:
            print "CONDDBUI: a new database was created."


    #---------------------------------------------------------------------------------#

    #=============================#
    # Database Reading Operations #
    #=============================#

    def setDefaultTag(self, tagName):
        '''
        Set the value of the default tag.
        inputs:
            tagName: string; the name of the default tag.
        outputs:
            none
        '''
        self.defaultTag = tagName

    def getXMLString(self, path, when, channelID = 0, tag = ''):
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
        outputs:
            string; the contents of the condition data.
        '''
        assert self.db <> None, "CONDDBUI: no database connected !"
        if tag == '':
            tag = self.defaultTag
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
            obj = folder.findObject(cool.ValidityKey(when), channelID, tag)
            return obj.payloadValue("data")
        else:
            raise Exception, "CONDDBUI: no folder named %s found"%path

    def getXMLStringList(self, path, fromTime, toTime, channelID = 0, tag = ''):
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
            list of [string, integer, integer, integer, integer]; the string is the payload.
            The first two integers are the since and until values of the interval of validity.
            The third integer is the channel ID, and the last integer is the insertion time.
        '''
        assert self.db <> None, "CONDDBUI: no database connected !"
        if channelID <> None:
            channelSelection = cool.ChannelSelection(channelID)
        else:
            channelSelection = cool.ChannelSelection()
        if tag == '':
            tag = self.defaultTag

        objList = []
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
            # Separate the Multi version and single version cases.
            if folder.versioningMode() == cool.FolderVersioning.MULTI_VERSION:
                try:
                    objIter = folder.browseObjects(cool.ValidityKey(fromTime), cool.ValidityKey(toTime), channelSelection, tag)
                except Exception, details:
                    raise Exception, "CONDDBUI: impossible to browse folder %s for objects: %s"%(node, details)
            else:
                try:
                    objIter = folder.browseObjects(cool.ValidityKey(fromTime), cool.ValidityKey(toTime), channelSelection)
                except Exception, details:
                    raise Exception, "CONDDBUI: impossible to browse folder %s for objects: %s"%(node, details)
            # Fill the object list
            for i in range(objIter.size()):
                obj = objIter.next()
                payload = obj.payload()['data']
                since = obj.since()
                until = obj.until()
                chID = obj.channelId()
                insertTime = obj.insertionTime()
                objList.append([payload, since, until, chID, insertTime])

        return objList


    def getChildNodes(self, path):
        '''
        Return a list of the children of the given node.
        inputs:
            path: string; path of the parent node. Must be a folderset.
        outputs:
            list of strings; the paths of the child nodes.
        '''
        assert self.db <> None, "CONDDBUI: no database connected !"
        if self.db.existsFolderSet(path):
            folderSet = self.db.getFolderSet(path)
            nodeList = list(folderSet.listFolders()) + list(folderSet.listFolderSets())
            nodeList.sort()
            return nodeList
        else:
            raise Exception, "CONDDBUI: no folderset named %s found"%path


    def getAllChildNodes(self, path):
        '''
        Return all the nodes of the tree lying under "path"
        inputs:
            path: string; path of the parent node. Must be a folderset.
        outpus:
            list of strings; the paths of all the elements of the tree under the
            given node.
        '''
        assert self.db <> None, "CONDDBUI: no database connected !"
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
            raise Exception, "CONDDBUI: no folderset named %s found"%path


    def getAllNodes(self):
        '''
        Convenience function: returns all the nodes of the database.
        inputs:
            none
        outputs:
            list of strings; the paths of all the nodes of the database
        '''
        assert self.db <> None, "CONDDBUI: no database connected !"
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
        assert self.db <> None, "CONDDBUI: no database connected !"
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
            raise Exception, "CONDDBUI: Sorry, md5 check over all tags is not implemented"
        elif tag == '':
            tag = self.defaultTag

        for node in nodesToCheck:
            # The check is done only on payload, i.e. we use only the folders,
            # not the foldersets.
            if self.db.existsFolder(node):
                folder = self.db.getFolder(node)
                # Separate the case of single version (no need for tag) and multi
                # version folders.
                if folder.versioningMode() == cool.FolderVersioning.MULTI_VERSION:
                    try:
                        objIter = folder.browseObjects(cool.ValidityKeyMin, cool.ValidityKeyMax, cool.ChannelSelection(), tag)
                    except Exception, details:
                        raise Exception, "CONDDBUI: impossible to browse folder %s for objects: %s"%(node, details)
                else:
                    try:
                        objIter = folder.browseObjects(cool.ValidityKeyMin, cool.ValidityKeyMax, cool.ChannelSelection())
                    except Exception, details:
                        raise Exception, "CONDDBUI: impossible to browse folder %s for objects: %s"%(node, details)
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
        assert self.db <> None, "CONDDBUI: no database connected !"
        if not self.db.existsFolderSet(path) and not self.db.existsFolder(path):
            raise cool.NodeNotFound, "CONDDBUI: node %s was not found in the database"%path
        else:
            tagList = []
            headTag = Tag('HEAD', path)
            tagList.append(headTag)

            # we check if the node is a single version folder.
            if self.db.existsFolder(path):
                folder = self.db.getFolder(path)
                if folder.versioningMode() == cool.FolderVersioning.SINGLE_VERSION:
                    return tagList

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
        assert self.db <> None, "CONDDBUI: no database connected !"
        if self.db.existsFolder(path):
            node = self.db.getFolder(path)
        elif self.db.existsFolderSet(path):
            node = self.db.getFolderSet(path)
        else:
            raise cool.NodeNotFound, "CONDDBUI: node %s was not found in the database"%path
        node.createTagRelation(parentTag, tag)


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
        assert self.db <> None, "CONDDBUI: no database connected !"
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
        assert self.db <> None, "CONDDBUI: no database connected !"
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
        else:
            raise Exception, "CONDDBUI: impossible to tag %s: no folder of this name found"%path
        if folder.versioningMode() == cool.FolderVersioning.SINGLE_VERSION:
            raise Exception, "CONDDBUI: impossible to tag %s: it is a single version folder"%path
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
        assert self.db <> None, "CONDDBUI: no database connected !"
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
                        folder.createTagRelation(tagName, auto_tag)
                else:
                    # Start recursion for the foldersets and create a tag relation with
                    # the parent.
                    self.recursiveTag(nodeName, auto_tag, description, reservedTags)
                    folderSet = self.db.getFolderSet(nodeName)
                    folderSet.createTagRelation(tagName, auto_tag)

        elif self.db.existsFolder(path):
            node = self.db.getFolder(path)
            if node.versioningMode() == cool.FolderVersioning.MULTI_VERSION:
                node.tagCurrentHead(tagName, description)

        else:
            raise cool.NodeNotFound, "CONDDBUI: node %s was not found in the database"%path

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
        assert self.db <> None, "CONDDBUI: no database connected !"
        # Check if the ancestor tag really exists.
        if not self.db.existsTag(ancestorTag):
            raise cool.TagNotFound, "CONDDBUI: the tag %s is not defined in the database"%ancestorTag
        # Retrieve the node
        if self.db.existsFolder(path):
            node = self.db.getFolder(path)
        elif self.db.existsFolderSet(path):
            node = self.db.getFolderSet(path)
        else:
            raise cool.NodeNotFound, "CONDDBUI: node %s was not found in the database"%path
        # Check if the node is already related to the ancestor tag
        try:
            relatedTag = node.resolveTag(ancestorTag)
        except Exception: #cool.TagRelationNotFound:
            # The node is not related to the ancestor tag. We are checking if
            # its parent is (it *has* to be).
            parentPath = os.path.dirname(path)
            parentNode = self.db.getFolderSet(parentPath)
            try:
                parentTag = parentNode.resolveTag(ancestorTag)
            except cool.NodeRelationNotFound:
                # The parent folderset doesn't know about the ancestor tag: this
                # means we don't know how to deal with its other child nodes.
                raise cool.NodeRelationNotFound, "CONDDBUI: the parent folderset %s isn't related to the ancestor tag %s."%(parentPath, ancestorTag)
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
                parentPath = os.path.dirname(path)
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


    #---------------------------------------------------------------------------------#

    #=============================#
    # Database Editing Operations #
    #=============================#
    
    def dropDatabase(cls, connectionString):
        '''
<<<<<<< conddbui.py
        drop the database.
=======
        drop the database identified by the connection string.
>>>>>>> 1.4
        inputs:
            connectionString: string; standard COOL connection string.
        outputs:
            none
        '''
        dbsvc = cool.DatabaseSvcFactory.databaseService()
        try:
            dbsvc.dropDatabase(connectionString)
        except Exception, details:
            raise Exception, "CONDDBUI: Impossible to drop the database: %s"%details
        else:
            print "CONDDBUI: the database was droped."
    dropDatabase = classmethod(dropDatabase)

    def createNode(self, path, description = '', storageType = "XML", versionMode = "MULTI"):
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
        outputs:
            none
        '''
        assert self.db <> None, "CONDDBUI: no database connected !"
        if storageType == 'NODE':
            try:
                self.db.createFolderSet(path, description, True)
            except Exception, details:
                raise Exception, "CONDDBUI: impossible to create the folderset: %s"%details
        else:
            if versionMode == 'MULTI':
                folderVersion = cool.FolderVersioning.MULTI_VERSION
            else:
                folderVersion = cool.FolderVersioning.SINGLE_VERSION

            folderSpec = cool.ExtendedAttributeListSpecification()
            folderSpec.push_back("data","string",cool.PredefinedStorageHints.STRING_MAXSIZE_16M)

            # WARNING: this folderdesc stuff is VERY important for LHCb: it tells the CondDB conversion
            #          service which type of converter to call. In this case (storage_type = 7), it calls
            #          the XML converter. It has to be appended to the folder description.
            folderDesc = description + " <storage_type=7>"

            try:
                self.db.createFolderExtended(path, folderSpec, folderDesc, folderVersion, True)
            except Exception, details:
                raise Exception, "CONDDBUI: impossible to create the folder: %s"%details

    def storeXMLString(self, path, data, since, until, channelID = 0):
        '''
        Adds a new condition object to the database.
        inputs:
            path:      string; path of the folder where the condition will be stored.
            data:      string; XML string to store in the database.
            since:     integer; lower bound of the interval of validity.
            until:     integer; upper bound of the interval of validity. It is excluded from
                       the interval.
            channelID: integer; ID of the channel where to store the condition.

        outputs:
            none
        '''
        assert self.db <> None, "CONDDBUI: no database connected !"
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
            # Create the fodler specifications
            spec = cool.ExtendedAttributeListSpecification()
            spec.push_back("data","string",cool.PredefinedStorageHints.STRING_MAXSIZE_16M)
            # Create a payload object with the correct specifications
            payload = coral.AttributeList(spec.attributeListSpecification())
            payload["data"] = data

            # Store the data in the DB
            folder.storeObject(cool.ValidityKey(since), cool.ValidityKey(until), payload, channelID)
        else:
            raise Exception, "CONDDBUI: impossible to find a folder named %s"%path


    def storeXMLStringList(self, path, XMLList):
        '''
        Allows to store a list of XML string into a given folder.
        inputs:
            path:    string; path of the folder where the condition will be stored.
            XMLList: list of (string, integer, integer, integer); the first element of the
                     tupple (or list) is the XML string to store, the second is the lower
                     bound of the interval of validity, the third is the upper bound of the
                     interval of validity and the fourth is the channel ID.

        outputs:
            none
        '''
        assert self.db <> None, "CONDDBUI: no database connected !"
        if self.db.existsFolder(path):
            folder = self.db.getFolder(path)
            # Create the fodler specifications
            spec = cool.ExtendedAttributeListSpecification()
            spec.push_back("data","string",cool.PredefinedStorageHints.STRING_MAXSIZE_16M)
            # Create a payload object with the correct specifications
            payload = coral.AttributeList(spec.attributeListSpecification())
            payload["data"] = ''

            # Start filling the buffer...
            folder.setupStorageBuffer()
            for obj in XMLList:
                payload["data"] = obj[0]
                since = cool.ValidityKey(obj[1])
                until = cool.ValidityKey(obj[2])
                channelID = obj[3]
                folder.storeObject(since, until, payload, channelID)
            # Write the data to the DB
            folder.flushStorageBuffer()
        else:
            raise Exception, "CONDDBUI: impossible to find a folder named %s"%path


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
        assert self.db <> None, "CONDDBUI: no database connected !"
        # Deal first with the full tree deletion
        if delete_subnodes and self.db.existsFolderSet(path):
            subnodeList = self.getAllChildNodes(path)
            # Reverse the tree order to start from the subelements
            subnodeList.reverse()
            for node in subnodeList:
                try:
                    self.db.dropNode(node)
                except Exception, details:
                    raise Exception, "CONDDBUI: impossible to delete node%s: %s"%(node, details)
        # In all cases, try to delete the given node
        try:
            self.db.dropNode(path)
        except Exception, details:
            raise Exception, "CONDDBUI: impossible to delete node%s: %s"%(path, details)

#########################################################################################
#                                       TESTS                                           #
#########################################################################################

def testDBAccess(connectionString):
    '''
    Create a CondDB object, connect to a database and show its contents
    '''
    bb = CondDB(connectionString)

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
    bb = CondDB(connectionString)

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
    bb     = CondDB(connectionString)

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
    bb = CondDB(connectionString)

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
    bb = CondDB(connectionString)

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
    bb = CondDB(connectionString)

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
    bb      = CondDB(connectionString)

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
    bb = CondDB(connectionString)

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
    bb = CondDB(connectionString)

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
    bb = CondDB(connectionString)

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
