
import os
from subprocess import Popen, PIPE
import logging
import shutil

class Module(object):
    def __init__(self, name=None):
        self._name = name
    def __repr__(self):
        return self._name


class Root(object):
    def __init__(self, cvsroot):
        self._fullname = cvsroot
        self._modules = None
        self.setModules()
    def setModules(self):
        self._modules = []
        p = Popen(["cvs", "-d", self._fullname, "co", "-s"],
              stdout=PIPE, close_fds=True)
        for line in p.stdout:
            self._modules.append(Module(line[:-1].split()[0]))
        os.waitpid(p.pid, 0)
    def Modules(self):
        return self._modules

class Repository(Root):
    def __init__(self, cvsroot):
        super(Repository, self).__init__(cvsroot)
        rootdetails = self._fullname.split(":")
        if len(rootdetails) < 2 :
            self._directory = rootdetails[-1]
        elif len(rootdetails) < 3 :
            self._server = rootdetails[-2]
            if len(self._server.split("@")) !=1 :
                self._user = self._server.split("@")[0]
                self._server = self._server.plit("@")[1]
        elif len(rootdetails) < 5 and rootdetails[0] == '':
            self._method = rootdetails[1]
    def fullName(self):
        return self._fullname
    def directory(self):
        return self._directory
    def server(self):
        return self._server
    def user(self):
        return self._user
    def setFullName(self, fullname):
        self._fullname = fullname
        super(Repository, self).__init__(fullname)
    def setDirectory(self, dirname):
        pass


def fixRootFile(directory, fromtxt, totxt, dryrun=False):
    log = logging.getLogger()
    rootfile = os.path.join(directory, "CVS", "Root")
    if os.path.exists(rootfile):
        infile = open(rootfile,"r")
        incont = infile.read()
        infile.close() 
        if incont.find(fromtxt) == -1 :
            log.info("No \"%s\" string in %s" % (fromtxt, rootfile)) 
        else :
            log.info("Changing \"%s\" to \"%s\" in %s", fromtxt, totxt, rootfile)
            if dryrun == False :
                newrootfile = rootfile + ".new"
                outfile = open(newrootfile, "w")
                outfile.write(incont.replace(fromtxt,totxt))
                outfile.close()
                log.debug("Written %s file" % newrootfile)
                bakrootfile = rootfile + ".bak"
                shutil.move(rootfile, bakrootfile)
                log.debug("Moved %s to %s" % (rootfile, bakrootfile))
                shutil.move(newrootfile, rootfile)
                log.debug("Moved %s to %s" % (newrootfile, rootfile))
            else :
                log.debug("previous change simulated")
                    
    else :
        log.warning("%s file doesn't exist" % rootfile)
        return 1

def fixRoot(basedirectory, fromtxt, totxt, recursive=False, dryrun=False):
    if recursive == False :
        rc = fixRootFile(basedirectory, fromtxt, totxt, dryrun)
    else :
        rc = 0
        for data in os.walk(basedirectory):
            root, dirs = data[0], data[1]
            if 'CVS' in dirs:
                dirs.remove('CVS')  # don't visit CVS directories
                if os.path.exists(os.path.join(root, "CVS", "Root")) :
                    rc = fixRootFile(root, fromtxt, totxt, dryrun)
    
    return rc
    