import os
import recursive_tag
from PyCool  import cool, pool
from guitree import dbTree, guiFolderSet, guiFolder, guiChannel

class GUIBridge:
    '''
    This will be a generic bridge between the CondDB and the GUI.
    '''
    def __init__(self):
        pass

class CoolBridge(GUIBridge):
    '''
    A COOL implementation of bridge between the CondDB Browser and data.
    '''
    def __init__(self, connectionString, create_new_db):
        '''
        Create a bridge between the User Interface and the CondDB.
        The "connectionString" allows to create a pointer to the CondDB.
        '''
        GUIBridge.__init__(self)
        self.db = None
        
        #--- Opening the Database access ---#
        self.dbsvc = cool.DatabaseSvcFactory.databaseService()
        try:
            #connectionString += ';password=Pherine9'
            self.db = self.dbsvc.openDatabase(connectionString)
        except:
            if create_new_db:
                try:
                    self.db = self.dbsvc.createDatabase(connectionString)
                except:
                    print "ERROR: impossible to create the database"
                    self.db = None
            else:
                print "ERROR: the database was not found"
                self.db = None
            
    def writeToDB(self, folderName, objectList):
        '''
        Writes a list of objects to a given folder in the database.
        '''
        folder = self.db.getFolder(folderName)
        folderspec = cool.ExtendedAttributeListSpecification()
        folderspec.push_back("data","string",cool.PredefinedStorageHints.STRING_MAXSIZE_16M)
        
        payload = pool.AttributeList(folderspec.attributeListSpecification())
        payload["data"] = "XML stuff" # dummy value, just to start

        folder.setupStorageBuffer()
        for obj in objectList:
            channelID = obj[0]
            since = cool.ValidityKey(obj[1])
            until = cool.ValidityKey(obj[2])
            payload["data"] = obj[3]
            folder.storeObject(since, until, payload, channelID)

        try:
            folder.flushStorageBuffer()
        except Exception, details:
            return details
        else:
            return True

    def createFolder(self, folderName, is_folderset, description = '', is_singleVersion = False, createParents = False):
        '''
        Create a new folder or folderset in the db
        '''
        if is_folderset:
            try:
                self.db.createFolderSet(folderName)
            except Exception, details:
                return details
            else:
                return True
        else:
            if is_singleVersion:
                versionType = cool.FolderVersioning.SINGLE_VERSION
            else:
                versionType = cool.FolderVersioning.MULTI_VERSION
                
            folderspec = cool.ExtendedAttributeListSpecification()
            folderspec.push_back("data","string",cool.PredefinedStorageHints.STRING_MAXSIZE_16M)

            # WARNING: this folderdesc stuff is VERY important for LHCb: it tells the CondDB conversion
            #          service which type of converter to call. In this case (storage_type = 7), it calls
            #          the XML converter. It has to be appended to the folder description.
            folderdesc = "<storage_type=7>"

            try:
                self.db.createFolderExtended(folderName, folderspec, description + folderdesc, versionType, createParents)
            except Exception, details:
                return details
            else:
                return True

    def deleteFolder(self, folderName):
        '''
        Delete a folder or folderset from the db
        '''
        try:
            folder_deleted = self.db.dropNode(folderName)
        except Exception, details:
            return details
        else:
            return folder_deleted

    def createTag(self, tagName, folderName, isGlobal):
        '''
        Create a new tag in the CondDB. If "isGlobal" is true, the function is
        recursively assigning tags to all the database, and store the tag in the
        /TAGS folder (create it if it does not exist).
        '''
        if isGlobal:
            folderspec = cool.ExtendedAttributeListSpecification()
            folderspec.push_back("data","string",cool.PredefinedStorageHints.STRING_MAXSIZE_16M)
            folderdesc = "<storage_type=7>"
            payload = pool.AttributeList(folderspec.attributeListSpecification())                
            if not self.db.existsFolder('/TAGS'):
                tagFolder = self.db.createFolderExtended('/TAGS', folderspec, folderdesc, cool.FolderVersioning.MULTI_VERSION,True)
            else:
                tagFolder = self.db.getFolder('/TAGS')

            # Tag the database
            try:
                recursive_tag.tag(self.db, "/", tagName, False)
            except Exception, details:
                return details
            else:
                # insert the last tag into the corresponding folder
                payload["data"] = tagName
                tagFolder.storeObject(cool.ValidityKeyMin, cool.ValidityKeyMax, payload, 0)
                # re-tag TAGS folder
                try:
                    recursive_tag.tag(self.db, "/TAGS", tagName, True)
                except Exception, details:
                    return details
                else:
                    return True
        else:
            try:
                folder = self.db.getFolder(folderName)
            except Exception, details:
                return details
            else:
                try:
                    folder.tagCurrentHead(tagName)
                except Exception, details:
                    return details
                else:
                    return True

    def deleteTag(self, tagName, folderName, is_global):
        '''
        Delete a tag from the CondDB. If the tag is an LHCb global tag,
        automatically removes it from the whole database.
        '''
        if is_global:
            try:
                recursive_tag.tag_delete(self.db, '/', tagName)
            except Exception, details:
                return details
            else:
                return True
        else:
            try:
                folder = self.db.getFolder(folderName)
                folder.deleteTag(tagName)
            except Exception, details:
                return details
            else:
                return True

                

if __name__ == '__main__':
    pass




