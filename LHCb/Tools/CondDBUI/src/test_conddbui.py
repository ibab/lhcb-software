from conddbui import *

#########################################################################################
#                                       TESTS                                           #
#########################################################################################

def testDBAccess(connectionString):
    '''
    Create a CondDB object, connect to a database and show its contents
    '''
    bb = CondDB(connectionString, create_new_db = True, readOnly = False)

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
    bb = CondDB(connectionString, create_new_db = True, readOnly = False)

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
    bb     = CondDB(connectionString, create_new_db = True,  readOnly = False)

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
    bb = CondDB(connectionString, create_new_db = True, readOnly = False)

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
    bb = CondDB(connectionString, create_new_db = True, readOnly = False)

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
    bb = CondDB(connectionString, create_new_db = True, readOnly = False)

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
    bb = CondDB(connectionString, create_new_db = True, readOnly = False)

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
    bb = CondDB(connectionString, create_new_db = True, readOnly = False)

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
    bb = CondDB(connectionString, create_new_db = True, readOnly = False)

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
    bb = CondDB(connectionString, create_new_db = True, readOnly = False)

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
    connectionString = "sqlite_file:%s/TEST"%filepath
    try:

        print "***************************\n"
        print "1st test: Database Access\n"
        testDBAccess(connectionString)
        print "***************************\n"
      
        print "**************************\n"
        print "2nd test: Node Creation\n"
        testNodeCreation(connectionString)
        print "**************************\n"
      
        print "**************************\n"
        print "3rd test: XML storage\n"
        testXMLStorage(connectionString)
        print "**************************\n"
      
        print "**************************\n"
        print "4th test: XML list storage\n"
        testXMLListStorage(connectionString)
        print "**************************\n"
      
        print "**************************\n"
        print "5th test: md5Checksum\n"
        testMD5(connectionString)
        print "**************************\n"
      
        print "**************************\n"
        print "6th test: node removal\n"
        testRemoveNode(connectionString)
        print "**************************\n"
      
        print "**************************\n"
        print "7th test: leaf tagging\n"
        testTagLeafNode(connectionString)
        print "**************************\n"
      
        print "**************************\n"
        print "8th test: recursive tagging\n"
        testRecursiveTag(connectionString)
        print "**************************\n"
      
        print "**************************\n"
        print "9th test: ancestor tagging\n"
        testTagWithAncestorTag(connectionString)
        print "**************************\n"
        
        print "**************************\n"
        print "10th test: tag listing\n"
        testGetTagList(connectionString)
        print "**************************\n"
        
    finally:
        # delete the temporary file once finished
        os.remove(filepath)
        
if __name__ == '__main__':
    run_tests()
