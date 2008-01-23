#!/usr/bin/env python
"""
Module containing some utility functions for CondDB administrative tasks. 
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"

import os
import sys
import shutil
import tempfile
import CondDBUI
from pprint import pprint

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
    print "Executing: %s"%cmd
    r = os.system(cmd)
    if r:
        raise RuntimeError("Command '%s' failed (status = %d)"%(cmd,r))
    elif not os.path.exists(os.path.join(tmpdir,"tmp1.db")):
        raise RuntimeError("Diff DB not produced")
    
    cmd = cmd_template % (addition+(tmp_db % 1,"HEAD")+(tmp_db % 2,))
    print "Executing: %s"%cmd
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

def merge_and_tag(source,target,tag):
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
    if check_addition_db_content(target,source):
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
            print "Creating tag '%s' from node '%s' in:"%(tag,parent)
        else:
            print "Moving tag '%s' from node '%s' in:"%(tag,parent)

        pprint(local_tags)
        target_db.moveTagOnNodes(parent, tag, local_tags)
    else:
        print "ERROR: the source DB contains something already in the target DB!!!"
