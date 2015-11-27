import re
import os

def stringVersion2Tuple(strver):
    """ convert string into a tuple of number and strings """
    sl = re.split("(\d+)", strver)
    nsl = []
    for i in sl :
        if i :
            if re.match("\d+", i) :
                nsl.append(int(i))
            else :
                nsl.append(i)
    return tuple(nsl)


def versionSort(strlist, reverse=False):
    """ Generic sorting of strings containing numbers aka versions """
    versionlist = [ (stringVersion2Tuple(str(s)), s) for s in strlist ]
    versionlist.sort()
    if reverse :
        versionlist.reverse()
    return [ x[1] for x in versionlist ]


## Write the generic Makefile for CMT projects.
#  @param filename: the name of the destination file
#  @param overwrite: flag to decide if an already present file has to
#   be kept or not (default is False)
def createProjectMakefile(dest, overwrite = False):
    import os
    if overwrite or not os.path.exists(dest):
        f = open(dest, "w")
        f.write("include ${LBUTILSROOT}/data/Makefile\n")
        f.close()


def which(cmd, path = None):
    """
    Locate a file in the path.
    """
    if path is None:
        path = os.environ["PATH"]
    if type(path) is str:
        path = path.split(os.path.pathsep)
    for p in path:
        fp = os.path.join(p, cmd)
        if os.path.exists(fp):
            return fp
    return None
