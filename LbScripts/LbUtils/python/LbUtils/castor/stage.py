#!/usr/bin/env python

from common import isfile, ismigrated, isdir
import path
import os

stagepool = os.environ.get("STAGE_POOL", "")


def getstatus(filename):
    status = ""
    if isfile(filename):
        status = "NotPresent"
        stcmd  = "stageqry --noheader"
        stcmd += " -p %s " % stagepool
        stcmd += "-M " + path.abspath(filename)
        out = os.popen(stcmd).read().split()
        if out : status = out[7]
    return status


def stageinvisitor(data,dirname,fileindir):#IGNORE:W0613
    if len(fileindir)>0:
        stcmd  = "stagein -A deferred --nowait --rdonly "
        stcmd += " -p %s " % stagepool
        for f in fileindir:
            fulname = dirname + "/" + f
            if getstatus(fulname) == "NotPresent" and ismigrated(fulname):
                stcmd += " -M " + path.abspath(fulname)
        print stcmd
        os.system(stcmd)

def stagedir(pathname):
    if isdir(pathname):
        data = None
        path.walk(pathname,stageinvisitor,data)
    else:
        stcmd  = "stagein -A deferred --nowait --rdonly "
        stcmd += " -p %s " % stagepool
        stcmd += " -M " + path.abspath(pathname)
        print stcmd
        os.system(stcmd)

def shorthelp(argv):
    print """
    %s [-p poolname] castor_path
    """ % argv[0]



if __name__ == "__main__":
    import getopt
    import sys
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'p')
    except getopt.GetoptError:
        shorthelp(sys.argv)
        sys.exit(2)
    for op,ar in opts :
        if op == '-p': stagepool = ar

    for p in args:
        stagedir(p)



    
