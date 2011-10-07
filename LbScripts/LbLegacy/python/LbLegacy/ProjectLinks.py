from LbUtils.Links import isLinkAbsolute, makeRelativeLink, printVisitor

from os.path import islink, walk, join, normpath, dirname
from os import readlink, sep, symlink, environ, remove




def getMySiteLink(afslink):
    mybase = environ["MYSITEROOT"]
    mybase = join(mybase, "lhcb")
    rel_areas = []
    rel_areas.append("/afs/cern.ch/lhcb/software/releases")
    rel_areas.append("/afs/cern.ch/sw/Gaudi/releases")
    for ra in rel_areas:
        if afslink.startswith(ra):
            pos = len(ra)
            tail = afslink[pos:]
            if tail.startswith(sep):
                tail = tail[1:]
            return join(mybase, tail)
    else :
        return afslink


def fixLink(data, linkname):
    verbose = False
    absolutemode = False
    dryrunmode = False
    if data.has_key("options"):
        opts = data["options"]
        verbose = opts.verbose
        absolutemode = opts.absolutemode
        dryrunmode = opts.dryrunmode
    linkcont = readlink(linkname)
    if isLinkAbsolute(linkname):
        mysitelink = getMySiteLink(linkcont)
    else:
        mysitelink = normpath(join(dirname(linkname), linkcont))
    if mysitelink :
        if verbose : print "Removing %s" % linkname
        if not dryrunmode: remove(linkname)
        if absolutemode:
            if verbose :
                print "Creating absolute link %s -> %s" % (linkname, mysitelink)
            if not dryrunmode:
                symlink(mysitelink, linkname)
        else:
            makeRelativeLink(mysitelink, linkname, dryrunmode)
    else:
        print "Warning: local %s doesn't exist" % linkcont

def fixVisitor(data, dirnm, filesindir):
    for f in filesindir:
        filename = join(dirnm, f)
        if islink(filename):
            fixLink(data, filename)



def fixLinks(treebase, opts=None):
    """ recursively displays the links of a tree """
    data = dict()
    data["treebase"] = treebase
    visitor = fixVisitor
    if opts:
        data["options"] = opts
        if opts.showmode:
            visitor = printVisitor
    walk(treebase, visitor, data)
