#!/usr/bin/env python
"""
Internal common functions/utilities. 
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id: _internals.py,v 1.3 2008-09-25 22:56:16 marcocle Exp $"

# Set up the logger 
import logging
log = logging.getLogger("CondDBUI.Admin")
log.setLevel(logging.INFO)

import os
import errno
import sys
import shutil
import tempfile
import CondDBUI
from pprint import pformat

def _assertNoFile(filename):
    """Raise an exception if the file exists."""
    if os.path.exists(filename):
        raise IOError(errno.EEXIST,os.strerror(errno.EEXIST),filename)

def check_addition_db_content(original,addition):
    """
    Check if the database specified as 'addition' contains only objects that are
    not in the database 'original'.
    
    Note: original and addition have to be tuples with the format
    (connection_string,tag).
    """
    tmpdir = tempfile.mkdtemp()
    tmp_db = "sqlite_file:%s/tmp%%d.db/TMP" % tmpdir
    cmd_template = 'diff_db.py "%s" "%s" "%s" "%s" "%s"'
    cmd = cmd_template % (original+addition+(tmp_db % 1,))
    log.debug("Executing: %s"%cmd)
    r = os.system(cmd)
    if r:
        raise RuntimeError("Command '%s' failed (status = %d)"%(cmd,r))
    elif not os.path.exists(os.path.join(tmpdir,"tmp1.db")):
        raise RuntimeError("Diff DB not produced")
    
    cmd = cmd_template % (addition+(tmp_db % 1,"HEAD")+(tmp_db % 2,))
    log.debug("Executing: %s"%cmd)
    r = os.system(cmd)
    if r:
        raise RuntimeError("Command '%s' failed (status = %d)"%(cmd,r))
    result = not os.path.exists(os.path.join(tmpdir,"tmp2.db"))
    shutil.rmtree(tmpdir)
    return result

def _find_common_parent_node(lst):
    """
    Utility function that returns the common base dir of a list of paths (with '/'
    as separator).
    """
    common = lst[0]
    for l in lst:
        i = 0
        lc = len(common)
        ll = len(l)
        while i < lc and i < ll and ( common[i] == l[i] ):
            i += 1
        if i == 0:
            return ""
        else:
            common = common[:i]
    p = common.rfind('/')
    if p > 0:
        return common[:p]
    return '/'

def MergeAndTag(source, target, tag, check_addition_db = True):
    """
    Utility to merge the content of the database 'source' into the database
    'target' tagging the new data as 'tag'.
    If the tag 'tag' already exists, then it is moved to include the new data,
    otherwise a new tag is created.
    
    'source' and 'target' can be tuples (connection_string,tag) or only the
    connection string.
    """
    
    if not isinstance(target,tuple):
        target = (target,"HEAD")
    if not isinstance(source,tuple):
        source = (source,"HEAD")
    if (not check_addition_db) or check_addition_db_content(target,source):
        print "Copying %s (%s) into %s (%s)"%(source+target)
        CondDBUI.merge(source[0],target[0])

        db = CondDBUI.CondDB(source[0])
        target_db = CondDBUI.CondDB(target[0],readOnly=False)
        
        addition = filter(db.db.existsFolder,db.getAllNodes())
        local_tags = {}
        for l in addition:
            local_tags[l] = "HEAD"

        parent = _find_common_parent_node(addition)
        # find the parent node that actually contains the tag we want to move
        is_new_tag = False
        try:
            newparent = parent
            f = target_db.getCOOLNode(newparent)
            while f.resolveTag(tag) != tag:
                # up one dir
                p = newparent.rfind('/')
                if p >= 0:
                    newparent = parent[:p]
                if newparent == "":
                    newparent = "/"
                f = target_db.getCOOLNode(newparent)
        except Exception, details:
            # it should be a new tag
            newparent = parent
            is_new_tag = True
        parent = newparent

        if is_new_tag:
            msg = "Creating tag '%s' from node '%s' in:\n"%(tag,parent)
        else:
            msg = "Moving tag '%s' from node '%s' in:\n"%(tag,parent)

        log.info(msg + pformat(local_tags))
        target_db.moveTagOnNodes(parent, tag, local_tags)
    else:
        log.error("The source DB contains something already in the target DB!!!")

def MakeDBFromFiles(source, db, includes, excludes, basepath = "",
                    remove_extension = False, verbose = False,
                    since = None,
                    until = None):
    """
    Copy the content of a directory into a CondDB instance.
    """
    # prepare the list of nodes to insert
    # the format is something like:
    # {
    #   "/path/to/folderset" : {
    #                           "folder.xml" : {
    #                                           "data": { 0 : "filename",
    #                                                     ...
    #                                                   },
    #                                           ...
    #                                          },
    #                           ...
    #                          },
    #   ...
    # }
    import re
    from PyCool import cool
    if since is None: since = cool.ValidityKeyMin
    if until is None: until = cool.ValidityKeyMax
    nodes = CondDBUI._collect_tree_info(source, includes = includes, excludes = [])

    # Just count the number of folders we are going to write
    count_folders = 0
    for folderset in nodes:
        count_folders += len(nodes[folderset])
    count_folders_len = str(len(str(count_folders)))

    # initialize counters for progress output
    file_count = 0
    folder_count = 0
    foldersets = nodes.keys()
    foldersets.sort() # not needed, but it seems cleaner

    for folderset in foldersets:
        
        folders = nodes[folderset].keys()
        folders.sort()

        for folder in folders:
            keys = nodes[folderset][folder].keys()
            keys.sort()
            
            folder_path = re.sub('/+','/','/'.join([basepath,folderset,folder]))
            if remove_extension:
                folder_path = os.path.splitext(folder_path)[0]

            if not db.db.existsFolder(folder_path):
                db.createNode(path = folder_path, storageKeys = keys)
                
            collection = {}
            for key in keys:
                for channel in nodes[folderset][folder][key].keys():
                    
                    if channel not in collection:
                        collection[channel] = {}
                        for k in keys:
                            collection[channel][k] = ""
            
                    collection[channel][key] = CondDBUI._fix_xml(open(nodes[folderset][folder][key][channel],"rb").read(),
                                                                 "conddb:"+'/'.join([basepath,folderset]))
                    
            xmllist = []
            for channel in collection:
                xmllist.append({ 'payload': collection[channel],
                                 'since': since,
                                 'until': until,
                                 'channel': channel })
                
            folder_count += 1
            file_count += len(keys)
            if verbose:
                print ("%" + count_folders_len + "d %" + count_folders_len + "d  %s (%d files)")%\
                      (folder_count,count_folders-folder_count,folder_path,len(keys))
            db.storeXMLStringList(folder_path, xmllist)

    if verbose:
        print "Total folders inserted = %d"%folder_count
        print "Total files inserted = %d"%file_count

def timegm(t):
    """Inverse of time.gmtime. Implementation from Gaudi::Time."""
    import time
    if t[8] != 0: # ensure that dst is not set
        t = tuple(list(t[0:8]) + [0])
    t1 = time.mktime(t)
    gt = time.gmtime(t1)
    t2 = time.mktime(gt)
    return t1 + (t1 - t2)

def timeToValKey(tstring, default):
    if not tstring: return default
    import re
    # Format YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]
    exp = re.compile(r"^(?P<year>[0-9]{4})-(?P<month>[0-9]{2})-(?P<day>[0-9]{2})"+
                     r"(?:_(?P<hour>[0-9]{2}):(?P<minute>[0-9]{2})(?::(?P<second>[0-9]{2})(?:\.(?P<decimal>[0-9]*))?)?)?"+
                     r"(?P<utc>UTC)?$")
    m = exp.match(tstring)
    if m:
        # FIXME: check for validity ranges
        def toInt(s):
            if s: return int(s)
            return 0
        tm = tuple([ toInt(n) for n in m.groups()[0:6] ] + [ 0, 0, -1 ])
        import time
        if m.group('utc'):
            # the user specified UTC
            t = timegm(tm)
        else:
            # seconds since epoch UTC, from local time tuple
            t = time.mktime(tm)
        t = int(t) * 1000000000 # to ns
        d = m.group('decimal')
        if d:
            if len(d) < 9:
                # Add the missing 0s to the decimals
                d += '0'*(9-len(d))
            else:
                # truncate decimals
                d = d[:9]
            # add decimals to t
            t += int(d)
        return t
    return default
