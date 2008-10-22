from LbUtils.castor import _getfiles, _getdirs
import os

def walk(path,visitor,data):
    filelist = _getfiles(path)
    dirlist = _getdirs(path)
    for d in dirlist:
        if path : d = path + "/" + d 
        walk(d,visitor,data)
    visitor(data,path,filelist)


def abspath(pathname=None):
    pth = ""
    if not pathname :
        pth = os.environ["CASTOR_HOME"]
    else:
        pth = pathname

    if not pth.startswith("/castor"):
        pth = os.environ["CASTOR_HOME"] + "/" + pth

    return pth
