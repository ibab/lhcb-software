""" various function to manipulate tarballs """

from LbUtils.File import copyTree, isFilePathExcluded, copyStat
from LbUtils.afs.directory import AFSLock
from LbUtils.Temporary import TempDir
from LbUtils.Links import fixLinks
from shutil import copy2
import fnmatch
import tarfile
import logging
import os

import __builtin__
if "set" not in dir(__builtin__):
    # pylint: disable-msg=W0622
    from sets import Set as set

_tar_exclusion_list = []


def tarIgnore(src, files):
    ignored_list = set()
    for f in files :
        fullf = os.path.join(src, f)
        if isFilePathExcluded(fullf, _tar_exclusion_list) :
            ignored_list.add(f)
    return ignored_list


def _singleTarFilter(namelist, sel=None, binary_set=set()):
    """ Helper function to select one binary """
    # filter out the name of the selected binary
    selected = set()
    this_binary_set = binary_set.copy()
    if sel :
        namelist = fnmatch.filter(namelist, "*%s*" % sel)
    this_binary_set.discard(sel)
    for f in namelist :
        tobearched = True
        for b in this_binary_set :
            if sel :
                if not fnmatch.fnmatch(sel, "*%s*" % b) and fnmatch.fnmatch(f, "*%s*" % b) :
                    tobearched = False
                    break
            else :
                if fnmatch.fnmatch(f, "*%s*" % b) :
                    tobearched = False
                    break
        if tobearched :
            selected.add(f)

    return selected


def tarFilter(namelist, sel_binary=None, binary_list=None):
    """ Select the items from the namelist matching sel_binary but not binary_list """
    selected = set()
    if not sel_binary :
        sel_binary = []
    if not binary_list :
        binary_list = []
    this_sel_set = set(sel_binary[:])
    if not this_sel_set :
        this_sel_set.add(None)
    this_binary_set = set(binary_list[:])
    this_binary_set |= this_sel_set

    for s in this_sel_set :
        selected |= _singleTarFilter(namelist, s, this_binary_set)
    return list(selected)



def createTarBall(dirname, filename, binary=None, binary_list=[],
                  symlinks=True, prefix=None):
    log = logging.getLogger()
    this_binary_list = binary_list[:]
    if binary and binary not in this_binary_list :
        this_binary_list.append(binary)

    if filename.endswith(".tar.gz") :
        tarf = tarfile.open(filename, "w:gz")
    elif filename.endswith(".tar.bz2") :
        tarf = tarfile.open(filename, "w:bz2")
    elif filename.endswith(".tar") :
        tarf = tarfile.open(filename, "w")
    else :
        log.error("No such tar format")

    if dirname[-1] != os.sep :
        dirname = dirname + os.sep
    for root, dirs, files in os.walk(dirname, topdown=True) :
        relroot = root.replace(dirname,"")
        for f in tarFilter(files, [binary], this_binary_list) :
            fullf = os.path.join(root, f)
            relf  = os.path.join(relroot, f)
            if prefix :
                relf = os.path.join(prefix, relf)
            tarf.add(fullf, relf)
            log.debug("added the file %s to the %s file" % (relf, filename) )

        for d in tarFilter(dirs, [binary], this_binary_list) :
            fulld = os.path.join(root, d)
            reld  = os.path.join(relroot, d)
            if prefix :
                reld = os.path.join(prefix, reld)
            if binary :
                tarf.add(fulld, reld, recursive=True)
                log.debug("added recursively the directory %s to the %s file" % (reld, filename) )
            else :
                tarf.add(fulld, reld, recursive=False)
                log.debug("added the directory %s to the %s file" % (reld, filename) )

        dirstoremove = tarFilter(dirs, this_binary_list)

        for d in dirstoremove :
            dirs.remove(d)


    tarf.close()

def openTar(filename, tar_mode="r"):
    log = logging.getLogger()
    if filename.endswith(".tar.gz") :
        tar_mode += ":gz"
    elif filename.endswith(".tar.bz2") :
        tar_mode += ":bz2"
    elif filename.endswith(".tar") :
        pass
    else :
        log.error("No such tar format. Using plain tar uncompressed")
    lock = AFSLock(filename)
    lock.lock(force=False)
    tarf = tarfile.open(filename, tar_mode)

    return tarf, lock

def closeTar(tarf, lock, _filename=None):
    tarf.close()
    lock.unlock()


def listTarBallObjects(dirname, pathfilter=None, prefix=None, top_most=False):
    if dirname[-1] != os.sep :
        dirname += os.sep

    for root, dirs, files in os.walk(dirname, topdown=True) :
        relroot = root.replace(dirname,"")
        objs = files + dirs
        if top_most :
            dirs_to_remove = []
        for o in objs :
            fullo = os.path.join(root, o)
            relo  = os.path.join(relroot, o)
            if prefix :
                relo = os.path.join(prefix, relo)
            if pathfilter :
                if pathfilter(relo) :
                    if top_most and o in dirs :
                        dirs_to_remove.append(o)
                    yield fullo, relo
            else :
                yield fullo, relo
        if top_most :
            for d in dirs_to_remove :
                dirs.remove(d)

def updateTarBallFromFilter(srcdirs, filename, pathfilter=None,
                            prefix=None, dereference=False):
    """ Front end to updateTarBallFromFilterDict if the src dirs have all the same
    prefix
    """
    srcdict = {}
    for s in srcdirs :
        srcdict[s] = prefix
    return updateTarBallFromFilterDict(srcdict, filename, pathfilter, dereference)

def createTarBallFromFilter(srcdirs, filename, pathfilter=None,
                            prefix=None, dereference=False, update=False):
    """ Front end to createTarBallFromFilterDict if the src dirs have all the same
    prefix
    """
    srcdict = {}
    for s in srcdirs :
        srcdict[s] = prefix
    return createTarBallFromFilterDict(srcdict, filename, pathfilter, dereference, update)

# ------------------------------------------------------------------------------------------------

def updateTarBallFromFilterDict(srcdict, filename, pathfilter=None, dereference=False):
    """ function to update an already existing tarball. The srcdict parameter is a dictionary
    containing the path of the source as the key and the prefix in the tarball as value.
    @param srcdict: dictionary with the src directory and their prefix
    @param filename: output file name
    @param pathfilter: filter function that takes as input a path and returns True or False if
                       the path as to be included in the tarball or not.
    @param dereference: dereference links in the tarball or not
    """
    log = logging.getLogger()
    status = 0
    if not os.path.exists(filename) :
        status = createTarBallFromFilterDict(srcdict, filename, pathfilter, dereference, update=False)
    else :
        log.debug("Updating %s" % filename)
        tarf, lock = openTar(filename, tar_mode="r")
        tmpdir = TempDir(suffix="tempdir", prefix="updateTarballFromFilter")
        tarf.extractall(tmpdir.getName())
        extracted_objs = [ x[1] for x in listTarBallObjects(tmpdir.getName()) ]
        closeTar(tarf, lock, filename)
        tobeupdated = False
        srcexist = False
        for dirname in srcdict.keys() :
            if os.path.exists(dirname) :
                srcexist = True
                break
        if not srcexist :
            return 1
        for dirname in srcdict.keys() :
            if os.path.exists(dirname) :
                prefix = srcdict[dirname]
                if prefix :
                    log.debug("Adding %s to %s" % (dirname, prefix))
                else :
                    log.debug("Adding %s" % dirname)
                if prefix :
                    dstprefix = os.path.join(tmpdir.getName(), prefix)
                    if not os.path.exists(dstprefix) :
                        os.makedirs(dstprefix)
                for y in listTarBallObjects(dirname, pathfilter, prefix) :
                    if y[1] not in extracted_objs :
                        tobeupdated = True
                        src = y[0]
                        dst = os.path.join(tmpdir.getName(), y[1])
                        if not dereference and os.path.islink(src) :
                            linkto = os.readlink(src)
                            os.symlink(linkto, dst)
                        elif os.path.isdir(src) :
                            os.mkdir(dst)
                        else :
                            copy2(src, dst)
            else :
                log.debug("The directory %s doesn't exist. Skipping." % dirname)
        if tobeupdated :
            log.debug("Updating tarball %s" % filename)
            status = createTarBallFromFilter([tmpdir.getName()], filename, pathfilter=None,
                                    prefix=None, dereference=dereference, update=False)
    return status

def createTarBallFromFilterDict(srcdict, filename, pathfilter=None,
                                dereference=False, update=False,
                                use_tmp=False):
    """ function to create a tarball. The srcdict parameter is a dictionary
    containing the path of the source as the key and the prefix in the tarball as value.
    @param srcdict: dictionary with the src directory and their prefix
    @param filename: output file name
    @param pathfilter: filter function that takes as input a path and returns True or False if
                       the path as to be included in the tarball or not.
    @param dereference: dereference links in the tarball or not
    @param update: if the tarball has to be either updated or replaced
    @param use_tmp: if True, creates the file in a temporary directory and move it.
    """
    log = logging.getLogger()
    status = 0
    if not update :
        log.debug("Creating %s" % filename )
        if use_tmp :
            tmpdir = TempDir(suffix="tempdir", prefix="createTarBallFromFilter")
            real_name = filename
            filename = os.path.join(tmpdir.getName(), os.path.basename(filename))
            log.debug("Using temporary file %s" % filename)
        tarf, lock = openTar(filename, tar_mode="w")
        keep_tar = False
        srcexist = False
        for dirname in srcdict.keys() :
            if os.path.exists(dirname) :
                srcexist = True
                break
        if not srcexist :
            os.remove(filename)
            status = 1 # status=1 means that the src directories do not exist
        else :
            for dirname in srcdict.keys() :
                if os.path.exists(dirname) :
                    prefix = srcdict[dirname]
                    if prefix :
                        log.debug("Adding %s to %s" % (dirname, prefix))
                    else :
                        log.debug("Adding %s" % dirname)
                    for fullo, relo in listTarBallObjects(dirname, pathfilter, prefix) :
                        keep_tar = True
                        tarf.add(fullo, relo, recursive=False)
                else :
                    log.debug("The directory %s doesn't exist. Skipping." % dirname)
            closeTar(tarf, lock, filename)
            if not keep_tar :
                log.warning("%s file is empty. Removing it." % filename)
                os.remove(filename)
                status = 2 # status=2 means that the tarball was empty
            if use_tmp :
                copy2(filename, real_name)
                log.debug("Copying %s to %s" % (filename, real_name))
    else :
        status = updateTarBallFromFilterDict(srcdict, filename, pathfilter, dereference)
    return status

# ------------------------------------------------------------------------------------------------

def getTarFileName(basename, binaryname=None, tar_type="gz", dirname=None):
    if binaryname :
        binm = binaryname
    else :
        binm = "share"
    filename = ".".join(["_".join([basename, binm]), "tar"])
    if tar_type != "plain" :
        filename = ".".join([filename, tar_type])
    if dirname :
        filename = os.path.join(dirname, filename)
    return filename

def copySources(sourcedirs, dest, symlinks=True, exclude=[]):
    global _tar_exclusion_list
    for e in exclude :
        _tar_exclusion_list.append(e)
    for d in sourcedirs :
        for c in os.listdir(d) :
            src = os.path.join(d,c)
            dst = os.path.join(dest, c)
            if os.path.isdir(src) :
                copyTree(src, dst, symlinks=symlinks, ignore=tarIgnore)
            else :
                if symlinks and os.path.islink(src) :
                    linkto = os.readlink(src)
                    os.symlink(linkto, dst)
                    copyStat(src, dst)
                else :
                    copy2(src, dst)

def createTarBalls(sourcedirs, output_dir=os.getcwd(),
                  tar_name = "package", get_share=True,
                  binary_list=[], tar_type="gz",
                  symlinks=True, prefix=None,
                  exclude=[],
                  top_dir=False, top_exclude=None):
    tmpdir = TempDir(suffix="tempdir", prefix="name")
    tmpdirname = tmpdir.getName()
    copySources(sourcedirs, tmpdirname, symlinks, exclude)
    # facility to select this binary from the first level of subdirectories
    if top_dir :
        for t in os.listdir(tmpdirname) :
            add_to = True
            # remove the entries caught by the top filter
            for f in top_exclude :
                if fnmatch.fnmatch(t, f) :
                    add_to = False
                    break
            if add_to :
                binary_list.append(t)

    # remove outside links and transform absolute ones into relative links
    fixLinks(tmpdirname)

    # the binary tar ball
    for b in binary_list :
        filename = getTarFileName(tar_name, b, tar_type, output_dir)
        createTarBall(tmpdirname, filename, b, binary_list, symlinks, prefix)

    # the shared tarball contains whatever remains
    if get_share :
        filename = getTarFileName(tar_name, None, tar_type, output_dir)
        createTarBall(tmpdirname, filename, None, binary_list, symlinks, prefix)
