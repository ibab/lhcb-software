#!/usr/bin/env python
"""
Internal common functions/utilities.
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id: _internals.py,v 1.6 2009-11-13 19:00:35 marcocle Exp $"

# Set up the logger
import logging
log = logging.getLogger("CondDBUI.Admin")
log.setLevel(logging.INFO)

import os
import errno
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
#            while f.resolveTag(tag) != tag:
            while target_db.resolveTag(f,tag) != tag:
                # up one dir
                p = newparent.rfind('/')
                if p >= 0:
                    newparent = parent[:p]
                if newparent == "":
                    newparent = "/"
                f = target_db.getCOOLNode(newparent)
        except Exception:
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
                    until = None,
                    writeDuplicate = True):
    """
    Copy the content of a directory into a CondDB instance.
    writeDuplicate is set to False for the ONLINE to avoid writing out duplicate contents
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
            db.storeXMLStringList(folder_path, xmllist, writeDuplicate)

    if verbose:
        print "Total folders inserted = %d"%folder_count
        print "Total files inserted = %d"%file_count

def CompareDBToFiles(source, db, includes, excludes, basepath = "",
                     remove_extension = False, verbose = False,
                     since = None,
                     until = None):
    """
    Check if the content of a directory matches the content of a CondDB instance.
    """
    # this is mostly a copy+paste from MakeDBFromFiles, so you can look there
    # for the details.
    import re
    from PyCool import cool
    if since is None:
        since = cool.ValidityKeyMin
    if until is None:
        until = cool.ValidityKeyMax

    nodes = CondDBUI._collect_tree_info(source, includes = includes, excludes = [])

    errors = {}
    foldersets = sorted(nodes)
    for folderset in foldersets:

        folders = sorted(nodes[folderset])
        for folder in folders:

            folder_path = re.sub('/+','/','/'.join([basepath,folderset,folder]))
            if remove_extension:
                folder_path = os.path.splitext(folder_path)[0]

            if not db.db.existsFolder(folder_path):
                errors[folder_path] = 'missing folder'
                continue

            collection = {}
            keys = sorted(nodes[folderset][folder])
            for key in keys:
                for channel in nodes[folderset][folder][key]:

                    if channel not in collection:
                        collection[channel] = {}
                        for k in keys:
                            collection[channel][k] = ""

                    collection[channel][key] = CondDBUI._fix_xml(open(nodes[folderset][folder][key][channel],"rb").read(),
                                                                 "conddb:"+'/'.join([basepath,folderset]))

            for channel in collection:
                expected = [collection[channel], # FIXME: we should allow other keys
                            since,
                            until,
                            channel]

                # The use of "until-1" is to take into account a difference
                # between insertion and retrieval.
                # Insertion is meant to be [since, until), while retrieval is
                # [since, until].
                found = db.getPayloadList(folder_path,
                                          since, until-1,
                                          channel, 'HEAD')
                if channel:
                    err_id = '%s[%d]' % (folder_path, channel)
                else:
                    err_id = folder_path
                if len(found) != 1:
                    errors[err_id] = 'bad IOVs count'
                    continue
                found = found[0][:-1]
                if expected[0] != found[0]:
                    errors[err_id] = 'bad content'
                    #if verbose:
                    #    from pprint import pprint
                    #    print 'error: bad content in', err_id
                    #    print 'expected:'
                    #    pprint(expected)
                    #    print 'found:'
                    #    pprint(found)
                    continue
    if verbose and errors:
        for err in sorted(errors.items()):
            print 'Error in %s: %s' % err
    return errors

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

## --- Imported from dump_db_to_files.py
def _make_relative(cwd,dst):
    """
    Generate a relative path that appended to cwd will point to dst.
    E.g.: cwd = '/my/source/path', dst = '/my/dest/path'
          ==> '../../dest/path'
    """
    cwdlist = cwd.split('/')
    dstlist = dst.split('/')
    # count common directories
    i = 0
    while ( i < len(cwdlist) ) and ( i < len(dstlist) ) and (cwdlist[i] == dstlist[i]):
        i += 1
    reslist = ['..']*(len(cwdlist)-i) # parent levels before a common point
    reslist += dstlist[i:] # remainig dst levels from the common point
    return '/'.join(reslist)

def _fixate_string(src,re,callback):
    # find first occurrence of regular expression
    m = re.search(src)
    while m != None:
        pos = m.start()
        newsubs = callback(m) # convert the string
        if newsubs != m.group(0):
            src = src[0:m.start()] + newsubs + src[m.end():]
        # find next occurrence of regular expression
        m = re.search(src,pos+len(newsubs))
    return src

class _relativize_url:
    def __init__(self,node,key,base):
        self.node = node
        self.key = key
        self.base = base
        self.log = logging.getLogger("_relativize_url")
    def __call__(self,match):
        src = match.group(0)
        s = match.start(1)-match.start(0)+1
        e = match.end(1)-match.start(0)-1
        subs = src[s:e]
        self.log.debug("matched '%s' -> '%s'",src,subs)
        if subs.startswith("conddb:"):
            path = subs[7:]
            if path[0] != "/": # Work-around for cases like "conddb:DTD/structure.dtd"
                path = "/" + path
            newsubs = _make_relative(self.base,path)
            self.log.debug("replacing '%s' -> '%s'",subs,newsubs)
            # replace the conddb url with a relative path
            return src[:s] + newsubs + src[e:]
        else:
            self.log.warning("ignoring non conddb URL in '%s[%s]': '%s'",
                             self.node, self.key, subs)
        return src

## Dummy class for progress monitoring.
#  It implements the subset of the interface of QProgressDialog
#  (http://doc.trolltech.com/4.4/qprogressdialog.html) needed by umpToFiles.
class _dummyMonitor(object):
    def setMaximum(self, max):
        pass
    def setValue(self, value):
        pass
    def value(self):
        return 0
    def reset(self):
        return 0
    def wasCanceled(self):
        return False
    def setLabelText(self, text):
        pass

## Dump the content of the database at a given time and tag to XML files.
def DumpToFiles(database, time=0, tag="HEAD", srcs=['/'],
                destroot='DDDB', force=False, addext=False,
                monitor = None):
    _log = logging.getLogger("CondDBUI.Admin.DumpToFiles")
    _log.debug("called with arguments: %s",repr((database,time,tag,srcs,
                                                destroot,force,addext)))
    if monitor is None:
        monitor = _dummyMonitor()

    from CondDBUI import CondDB
    import re

    if type(database) is str:
        # Connect to the database
        db = CondDB(database, defaultTag = tag)
        _log.debug("connected to database '%s'", database)
    else:
        # Let's assume we were given a CondDBUI.CondDB instance
        db = database

    # @note: This piece of code is needed if we want to allow only global tags
    # # Check the validity of the tag
    # if not db.isTagReady(tag):
    #     raise RuntimeError("Tag '%s' is not a valid global tag."%tag)

    # Collect the list of folders we are going to use.
    nodes = []
    for s in srcs:
        if db.db.existsFolder(s):
            nodes.append(s)
        elif db.db.existsFolderSet(s):
            nodes += db.getAllChildNodes(s)
        else:
            _log.warning("Node '%s' does not exist. Ignored",s)

    nodes = [ n for n in set(nodes) if db.db.existsFolder(n) ]
    nodes.sort()
    monitor.setMaximum(len(nodes))

    # matching: SYSTEM "blah"
    sysIdRE = re.compile('SYSTEM[^>"\']*("[^">]*"|'+"'[^'>]*')")
    # matching: href "conddb:blah"
    hrefRE = re.compile('href *= *("conddb:[^">]*"|'+"'conddb:[^'>]*')")
    value = 0
    for node in ( n for n in nodes if db.db.existsFolder(n) ):
        if monitor.wasCanceled():
            break
        monitor.setValue(value)
        monitor.setLabelText(node)
        _log.debug("retrieve data from '%s'",node)
        f = db.getCOOLNode(node)
        channels = [ i for i in f.listChannels() ]
        if len(channels) > 1:
            _log.debug("processing %d channels",len(channels))
        for ch in channels:
            monitor.setValue(value)
            try:

                data = db.getPayload(node, time, channelID = ch, tag = tag)

                nodesplit = node.split('/')
                nodebase = '/'.join(nodesplit[:-1])
                nodename = nodesplit[-1]

                if '/' != os.sep:
                    tmppath = nodebase.replace('/',os.sep)
                else:
                    tmppath = nodebase
                if tmppath and (tmppath[0] == os.sep):
                    tmppath = tmppath[1:]
                dirname = os.path.join(destroot,tmppath)

                if not os.path.isdir(dirname):
                    _log.debug("create directory '%s'",dirname)
                    os.makedirs(dirname)

                _log.debug("looping over data keys")

                for key,xml in data.items():

                    _log.debug("key: '%s'",key)
                    filename = nodename
                    if key != 'data':
                        filename = '%s@%s'%(key,nodename)
                    # Let's assume that if there is more than 1 channel also the "0" is used explicitly
                    if ch != 0 or len(channels) > 1:
                        filename += ':%d'%ch
                    tmppath = os.path.join(dirname,filename)

                    if not force and os.path.exists(tmppath):
                        _log.warning("file '%s' already exists: skipping",tmppath)
                        continue

                    _log.debug("fixating XML")

                    # fix system IDs
                    xml = _fixate_string(xml, sysIdRE, _relativize_url(node,key,nodebase))
                    # fix hrefs pointing to absolute conddb urls
                    xml = _fixate_string(xml, hrefRE, _relativize_url(node,key,nodebase))
                    if tmppath.endswith(os.path.join("Conditions","MainCatalog.xml")):
                        # make the href to Online point to the DB
                        xml = xml.replace('"Online"', '"conddb:/Conditions/Online"')

                    _log.debug("write '%s'",tmppath)
                    open(tmppath,'w').write(xml)

            except RuntimeError, x:
                desc = str(x)
                if "Object not found" in desc:
                    _log.info("no data in %s", node)
                elif "No child tag" in desc:
                    _log.info("tag not found in %s", node)
                elif "not a child of" in desc:
                    # tag exists in a folderset not containing the current one
                    _log.info("tag not found in %s", node)
                elif 'Attempt to access data of NULL attribute "NODE_PARENTID"' in desc:
                    pass
                else:
                    raise
        value += 1
    monitor.setValue(value)
