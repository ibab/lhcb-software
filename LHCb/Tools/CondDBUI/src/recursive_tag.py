#!/usr/bin/env python
from PyCool import cool,pool

from optparse import OptionParser
import sys

def tag_folder(folder, tag, force = False):
    if folder.versioningMode() == cool.FolderVersioning.MULTI_VERSION:
        real_tag = "%s-%s"%(folder.fullPath(),tag)
        if force and ( real_tag in folder.listTags() ):
            folder.deleteTag(real_tag)
            print "Deleted tag '%s' from folder '%s'"%(tag,folder.fullPath())
        folder.tagCurrentHead(real_tag)
        #print "Tagged folder '%s' with tag '%s'"%(folder.fullPath(),tag)

def tag(db,path,tagname, force = False):
    # check if path correspond to a folder (tag and stop recursion)
    if db.existsFolder(path):
        node = db.getFolder(path)
        tag_folder(node,tagname,force)

    elif db.existsFolderSet(path):
        node = db.getFolderSet(path)
        for p in node.listFolders():
            tag_folder(db.getFolder(p),tagname,force)
        for p in node.listFolderSets():
            tag(db,p,tagname,force)

    else:
        raise "Path '%s' does not exists in current database"%path


def tag_delete_folder(folder, tag):
    if folder.versioningMode() == cool.FolderVersioning.MULTI_VERSION:
        real_tag = "%s-%s"%(folder.fullPath(),tag)
        if real_tag in folder.listTags():
            folder.deleteTag(real_tag)
            #print "Deleted tag '%s' from folder '%s'"%(tag,folder.fullPath())

def tag_delete(db,path,tagname):
    # check if path correspond to a folder (tag and stop recursion)
    if db.existsFolder(path):
        node = db.getFolder(path)
        tag_delete_folder(node,tagname)

    elif db.existsFolderSet(path):
        node = db.getFolderSet(path)
        for p in node.listFolders():
            tag_delete_folder(db.getFolder(p),tagname)
        for p in node.listFolderSets():
            tag_delete(db,p,tagname)

    else:
        raise "Path '%s' does not exists in current database"%path

def main():
    usage = "usage: \%prog [options] TAG"
    parser = OptionParser(usage)
    parser.add_option("-c", "--connect-string",
                      dest="connectString", type="string",
                      help="cool::DatabaseId to use for the connection",
                      default="sqlite://none;schema=sqlite.db;user=none;password=none;dbname=DDDB")
    parser.add_option("-d", "--dir",
                      dest="node", type="string",
                      help="HVS node which the tag has to be applied to (default = '/')",
                      default="/")
    parser.add_option("-F", "--force",
                      dest="force", action="store_true",
                      help="Move tag if necessary",
                      default=False)
    parser.add_option("-D", "--delete",
                      dest="delete", action="store_true",
                      help="Only delete the tag from the tree",
                      default=False)
#    parser.add_option("-x","--channel",
#                      dest="channel", type="int",
#                      help="Channel Id for the tag",
#                      default=0)

    (options, args) = parser.parse_args()
    if len(args) != 1:
        parser.error("Bad arguments, use -h for help.")

    #### initialize COOL
    dbs = cool.DatabaseSvcFactory.databaseService()
    db = dbs.openDatabase(options.connectString)

    if options.delete:
        tag_delete(db,options.node,args[0])
    else:
        tag(db,options.node,args[0],options.force)


if __name__ == '__main__':
    main()
