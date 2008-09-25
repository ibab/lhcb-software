"""
Management utilities: Commit, TAG, etc. 
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id: Management.py,v 1.4 2008-09-25 22:56:16 marcocle Exp $"


# Exported symbols
__all__ = []

import os, shutil, errno, tempfile

import CondDBUI
import CondDBUI.PyCoolDiff
from PyCool import cool

from _internals import _assertNoFile, MakeDBFromFiles, log

def _SQLiteFileName(dirname, partition):
    """Return the standard filename of the SQLite database containing the
    specified partition (COOL database instance)."""
    return os.path.join(dirname, partition+".db")
    
def _coolSQLiteURL(filename, partition):
    """Given the filename and the partition name, returns the SQLite COOL
    URI."""
    return "sqlite_file:%s/%s"%(filename, partition)

def _masterCoolURL(partition):
    """Returns the URI of the master database containing the given partition.""" 
    dirname = os.environ["SQLITEDBPATH"]
    return _coolSQLiteURL(_SQLiteFileName(dirname, partition), partition)

def _createDB(dirname, partition):
    """Shortcut to create an SQLite database for the given partition in the
    specified directory (using default filename)."""
    filename = _SQLiteFileName(dirname, partition)
    _assertNoFile(filename)
    return CondDBUI.CondDB(_coolSQLiteURL(filename, partition), create_new_db = True)

def _isSQLite(path):
    """Function to check if path points to an SQLite file."""
    if not os.path.isfile(path): return False
    return open(path, "rb").read(6) == "SQLite"

# Prepare changes DB with minimal changes
__all__.append("prepareChangesDB")
def prepareChangesDB(srcpath, partition, reftag, usertag = "HEAD", destdir = None,
                     two_pass_diff = True,
                     since = cool.ValidityKeyMin,
                     until = cool.ValidityKeyMax):
    """Given an input path, produces a database with only the differences that
    have to be added to the DB.
    If two_pass_diff is True (default), the difference is computed against the
    reftag to extract the requested differences, then against the HEAD to extract
    the minimal set of differences.
    Returns the COOL connection string to the created DB.
    """
    if destdir is None:
        destdir = os.getcwd()
    changesfn = _SQLiteFileName(destdir, partition + "_changes")
    first_pass_file = _SQLiteFileName(destdir, partition + "_pass1")
    # we do not want to overwrite the changes file
    _assertNoFile(changesfn)
    if two_pass_diff: _assertNoFile(first_pass_file)

    tmpdir = None
    try:
        # make temporary copy of the source
        if _isSQLite(srcpath):
            #shutil.copyfile(srcpath, filename)
            filename = srcpath
            # db = CondDBUI.CondDB(_coolSQLiteURL(filename, partition))
        elif os.path.isdir(srcpath):
            tmpdir = tempfile.mkdtemp()
            filename = _SQLiteFileName(tmpdir, partition)
            db = _createDB(tmpdir, partition)
            MakeDBFromFiles(srcpath, db, includes = [], excludes = [])
            del db
        else:
            raise RuntimeError("source path '%s' is neither an SQlite file or a directory"%srcpath)
        
        if two_pass_diff:
            # Create the 1st pass diff file
            first_pass_file = _SQLiteFileName(destdir, partition + "_pass1")
            log.debug("First pass diff: comparing %s (%s) to %s (%s)" % \
                      (_coolSQLiteURL(filename, partition), usertag,
                       _masterCoolURL(partition), reftag))
            CondDBUI.PyCoolDiff.diff(_masterCoolURL(partition),
                                     _coolSQLiteURL(filename, partition),
                                     _coolSQLiteURL(first_pass_file, partition),
                                     since = since, until = until,
                                     originalTAG = reftag, modifiedTAG = usertag)
            if not os.path.exists(first_pass_file):
                # this is needed because PyCoolDiff refuses to create empty diffs
                CondDBUI.CondDB(_coolSQLiteURL(first_pass_file, partition),
                                create_new_db = True)
            filename = first_pass_file
            reftag = usertag = "HEAD"
        
        CondDBUI.PyCoolDiff.diff(_masterCoolURL(partition),
                                 _coolSQLiteURL(filename, partition),
                                 _coolSQLiteURL(changesfn, partition),
                                 since = since, until = until,
                                 originalTAG = reftag, modifiedTAG = usertag)
        if not os.path.exists(changesfn):
            # this is needed because PyCoolDiff refuses to create empty diffs
            CondDBUI.CondDB(_coolSQLiteURL(changesfn, partition),
                            create_new_db = True)
        
    finally:
        # dispose of the temporary directory, if any
        if tmpdir: shutil.rmtree(tmpdir, ignore_errors = True)
    
    if two_pass_diff:
        return _coolSQLiteURL(changesfn, partition), _coolSQLiteURL(first_pass_file, partition)
    return _coolSQLiteURL(changesfn, partition), None
    
def _divideBySubdir(nodes, subdirs = ["/Conditions/Online","/Conditions"], default = "/"):
    """Returns a dictionary where the list of nodes ar divided by subdirectory,
    using the subdirectories specified."""
    res = {default:[]}
    subdirs = list(subdirs)
    subdirs.sort(reverse = True)
    for d in subdirs: res[d] = []
    for n in nodes:
        found = False
        for d in subdirs:
            if n.startswith(d) and ( len(n) == len(d) or n[len(d)] == "/" ):
                res[d].append(n)
                found = True
                break
        if not found:
            res[default].append(n)
    return res 

def _checkPartitionBoundaries(partition,modifiedNodes,addedNodes):
    """Check if the modified and added files belong to the correct partition.
    Returns the list of partitions in which the files could go."""
    changes = _divideBySubdir(set(modifiedNodes).union(addedNodes))
    partDirs = { "/Conditions/Online": ["SIMCOND"],
                 "/Conditions": ["LHCBCOND", "SIMCOND"],
                 "/": ["DDDB"],
                 }
    # check modified
    unexpected = set()
    for d in partDirs:
        if changes[d] and partition not in partDirs[d]:
            unexpected.update(partDirs[d])
    return list(unexpected)

__all__.append("analyzeChanges")
def analyzeChanges(partition, changesURL):
    """Runs a simple analysis/check of the changes compared to the content of
    the head database. 
    """
    problems = []
    
    master = CondDBUI.CondDB(_masterCoolURL(partition))
    changes = CondDBUI.CondDB(changesURL)
    
    masterNodes = set(master.getAllNodes())
    changesNodes = set(changes.getAllNodes())
    # extract the list of folders that exists on both master and changes
    modifiedNodes = set([ n
                          for n in masterNodes.intersection(changesNodes)
                          if changes.db.existsFolder(n) ])
    # extract the list of new folders that exists changes
    addedNodes = set([ n
                       for n in changesNodes.difference(modifiedNodes)
                       if changes.db.existsFolder(n) ])
    
    res = _checkPartitionBoundaries(partition, modifiedNodes, addedNodes)
    if res:
        problems.append("Found changes for other partitions: %s" % str(res))
    
    # I prefer that the results are sorted lists
    modifiedNodes = list(modifiedNodes)
    modifiedNodes.sort()
    addedNodes = list(addedNodes)
    addedNodes.sort()
    return modifiedNodes, addedNodes, problems
