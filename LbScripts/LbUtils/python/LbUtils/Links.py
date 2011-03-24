""" 
module to fix links in a tree
    - removes broken links
    - transform absolute into relative links
    - corrects overshooting links (relative links which has targets 
      with path outside of the tree)
      
    @author: Hubert Degaudenzi 
"""    

from os.path import islink, walk, join, abspath, realpath, sep, normpath
from os.path import dirname, exists, split, pardir, splitdrive
from os import readlink, symlink, remove
from itertools import izip
import logging

def isLinkAbsolute(linkname):
    linkcont = readlink(linkname)
    return linkcont.startswith(sep)
    
def isLinkBroken(linkname):
    linkdir = dirname(linkname)
    target = abspath(join(linkdir, readlink(linkname)))
    return not exists(target)

def doesLinkPointInTree(treebase, linkname):
    linkdir = dirname(linkname)
    tbase = abspath(treebase)
    target = abspath(join(linkdir, readlink(linkname)))
    return target.startswith(tbase)

def getCommonPath(dirn, filename):
    # if the 2 components are on different drives (windows)
    if splitdrive(dirn)[0] != splitdrive(filename)[0]:
        return None
    dirl = dirn.split(sep)
    filel = filename.split(sep)
    commpth = []
    for d, f in izip(dirl, filel):
        if d == f :
            commpth.append(d)
        else :
            break
    commpth = sep.join(commpth)
    if not commpth:
        commpth = sep
    elif commpth[-1] != sep:
        commpth += sep
    return commpth


def getRelativePath(dirn, filename):
    """ calculate the relative path of filename with regards to dirn """
    # Translate the filename to the realpath of the parent directory + basename
    filepath,basename = split(filename)
    filename = join(realpath(filepath),basename)
    # Get the absolute path of the destination directory
    dirn = realpath(dirn)
    commonpath = getCommonPath(dirn, filename)
    # for windows if the 2 components are on different drives
    if not commonpath:
        return filename
    relname = filename[len(commonpath):]
    reldir = dirn[len(commonpath):]
    if reldir:
        relname = (pardir+sep)*len(reldir.split(sep)) + relname
    return relname


def makeRelativeLink(src, dest, dryrunmode):
    """ create a relative symlink (instead of an absolute one) """
    relpath = getRelativePath(dirname(dest), src)
    log = logging.getLogger()
    if not dryrunmode :
        symlink(relpath, dest)
    else :
        log.info("Creating relativelink %s -> %s" % (dest, relpath))
        
def relativizeLink(linkname):
    linkdir = dirname(linkname)
    target = abspath(join(linkdir, readlink(linkname)))
    return getRelativePath(linkdir,target)

def getLinkSignature(treebase, linkname):
    signature = ""
    if isLinkAbsolute(linkname):
        signature += "A" # absolute link
    else:
        signature += "R" # relative link
    if isLinkBroken(linkname):
        signature += "B" # broken link
    else:
        signature += "V" # valid link
    if doesLinkPointInTree(treebase, linkname):
        signature += "C" # link is contained
    else:
        signature += "O" # link is outside
    return signature

def getLinkInfo(treebase, linkname):
    dirnm = dirname(linkname)
    signature = getLinkSignature(treebase, linkname)
    msg =  "[%-3s] %s -> " % (signature, linkname)
    linkcont = readlink(linkname)
    linktarget = abspath(join(dirnm, linkcont))
    if isLinkAbsolute(linkname):
        linktarget = relativizeLink(linkname)
    else:
        linktarget = abspath(join(dirnm, linkcont))
    msg += "%s (%s)" % (linkcont, linktarget)
    return msg
    


def printVisitor(data, dirnm, filesindir):
    for f in filesindir:
        filename = join(dirnm, f)
        if islink(filename):
            print getLinkInfo(data["treebase"], filename)

    
def fixLink(data, linkname):
    log = logging.getLogger()
    absolutemode = data.get("absolute", False)
    dryrunmode = data.get("dryrun", False)
    treebase = data.get("treebase", None)
    if isLinkBroken(linkname) :
        log.warning("The link %s is broken - removing it" % linkname)
        if not dryrunmode :
            remove(linkname)
    else :
        if not doesLinkPointInTree(treebase, linkname) :
            log.warning("The link %s doesn't point in the tree base %s - removing it" % (linkname, treebase))
            if not dryrunmode :
                remove(linkname)
        else :
            tobefixed = False
            if isLinkAbsolute(linkname) :
                if dryrunmode :
                    log.info("The link %s is absolute" % linkname)
                if not absolutemode :
                    linktarget = relativizeLink(linkname)
                    tobefixed = True
            else :
                if dryrunmode :
                    log.info("The link %s is relative" % linkname)
                if absolutemode :
                    linkcont = readlink(linkname)
                    linktarget = normpath(join(dirname(linkname),linkcont))
                    tobefixed = True
            if tobefixed :
                log.warning("Making %s point to %s" % (linkname, linktarget))
                if not dryrunmode :
                    remove(linkname)
                    symlink(linktarget, linkname)

    

def fixVisitor(data, dirnm, filesindir):
    for f in filesindir:
        filename = join(dirnm, f)
        if islink(filename):
            fixLink(data, filename)
    
def fixLinks(treebase, show=False, absolute=False, dryrun=False ):
    """ recursively displays the links of a tree """
    log = logging.getLogger()
    data = dict()
    data["treebase"] = treebase
    visitor = fixVisitor
    data["show"] = show
    data["absolute"] = absolute
    data["dryrun"] = dryrun
    if show:
        visitor = printVisitor
    log.debug("Fixing Links in %s" % treebase)
    walk(treebase, visitor, data)
    


