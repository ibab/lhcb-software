""" 
module to fix links in a tree
    - removes broken links
    - transform absolute into relative links
    - corrects overshooting links (relative links which has targets 
      with path outside of the tree)
      
    @author: Hubert Degaudenzi 
"""    

from os.path import islink, walk, join, abspath, realpath
from os.path import dirname, exists
from os import readlink, sep, pardir, symlink
from itertools import izip

def isLinkAbsolute(linkname):
    linkcont = readlink(linkname)
    return linkcont.startswith(sep)
    
def isLinkBroken(linkname):
    linkdir = dirname(linkname)
    target = abspath(join(linkdir, readlink(linkname)))
    return not exists(target)

def doesLinkPointInTree(treebase, linkname):
    linkdir = dirname(linkname)
    target = abspath(join(linkdir, readlink(linkname)))
    return target.startswith(treebase)

def getCommonPath(dirnm, filename):
    """ calculate the common path of the 2 entries """
    dirl = dirnm.split(sep)
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


def getRelativePath(dirnm, filename):
    """ calculate the relative path of filename with regards to dirnm """
    filename = realpath(filename)
    dirnm = realpath(dirnm)
    commonpath = getCommonPath(dirnm, filename)
    relname = filename[len(commonpath):]
    reldir = dirname[len(commonpath):]
    if reldir:
        relname = (pardir+sep)*len(reldir.split(sep)) + relname  
    return relname

def makeRelativeLink(src,dest):
    """ create a relative symlink (instead of an absolute one) """
    relpath = getRelativePath(dest, src)
    symlink(relpath, dest)

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
    

def printVisitor(treebase, dirnm, filesindir):
    for f in filesindir:
        filename = join(dirnm, f)
        if islink(filename):
            print getLinkInfo(treebase, filename)
    

def showLinks(treebase):
    """ recursively displays the links of a tree """
    walk(treebase, printVisitor, treebase)
    
def fixLink(treebase, linkname):
    if isLinkBroken(linkname):
        print "%s is broken" % linkname
    else:
        if not doesLinkPointInTree(treebase, linkname):
            print "%s does not point in tree" % linkname
        else:
            if isLinkAbsolute(linkname):
                print "%s is relativezed" % linkname

                
            
    
def fixVisitor(treebase, dirnm, filesindir):
    for f in filesindir:
        filename = join(dirnm, f)
        if islink(filename):
            fixLink(treebase, filename)
    
def fixLinks(treebase):
    walk(treebase, fixVisitor, treebase)
    
def removeBrokenLinks(treebase):
    """ removes links with non-existing targets """
    # @todo: implement
    print treebase


def relativizeLinks(treebase):
    """ transforms absolute into relative links. The links are 
    calculted with regards to the tree base. """
    # @todo: implement
    print treebase


def correctOvershoot(treebase):
    """ removes excedental '..' from relative links with regards
    to the tree base."""
    # @todo: implement
    print treebase

