
import os
from subprocess import Popen, PIPE

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
        retcode = os.waitpid(p.pid, 0)
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
            