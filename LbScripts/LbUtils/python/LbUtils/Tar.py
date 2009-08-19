""" various function to manipulate tarballs """

from LbUtils.File import copyTree, isFilePathExcluded, copyStat
from LbUtils.Temporary import TempDir
from LbUtils.Links import fixLinks
from shutil import copy2
import fnmatch
import tarfile
import logging
import os


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


def tarFilter(namelist, sel_binary=[], binary_list=[]):
    """ Select the items from the namelist matching sel_binary but not binary_list """
    selected = set()
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
    tmpdir = TempDir("name")
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
