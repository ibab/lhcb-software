""" AFS helper for directories """


from LbUtils.Lock import Lock2

import re, os
from subprocess import Popen, PIPE, STDOUT
from time import sleep

class NotInAFS(Exception):
    """ Exception for non AFS directories """
    pass

class NoACL(Exception):
    """ Exception raised when clearing an acl without providing new ones """
    pass



class Directory(object):
    def __init__(self, dirpath):
        self._name = os.path.realpath(dirpath)
        self._id = self.getID()
    def name(self):
        return self._name
    def ID(self):
        return self._id
    def getID(self):
        """ get the ID of a directory """
        exp = re.compile("File\s+" + self._name + "\s+\((.*)\)")
        p = Popen(["fs","examine",self._name], stdout=PIPE, stderr=STDOUT)
        for line in p.stdout.xreadlines() :
            volmatch = exp.search(line)
            if volmatch:
                return volmatch.group(1)
        raise NotInAFS("Directory %s is not on an AFS volume" % self._name )
    def getVolumeID(self):
        """ get the volume ID of a directory """
        exp = re.compile("contained\s+in\svolume\s+(\d+)")
        p = Popen(["fs","examine",self._name], stdout=PIPE, stderr=STDOUT)
        for line in p.stdout.xreadlines() :
            volmatch = exp.search(line)
            if volmatch:
                return long(volmatch.group(1))
        raise NotInAFS, "Directory %s is not on an AFS volume"  % self._name
    def getVolumeName(self):
        """ get the volume name which contains the directory """
        exp = re.compile("Volume\s+status.*named\s+(.*)")
        p = Popen(["fs","examine",self._name], stdout=PIPE, stderr=STDOUT)
        for line in p.stdout.xreadlines() :
            volmatch = exp.search(line)
            if volmatch:
                return volmatch.group(1)
        raise NotInAFS, "Directory %s is not on an AFS volume"  % self._name
    def getACL(self) :
        """ return to list of ACL of the directory as a dictionary """
        acl_dict = {}
        p = Popen(["fs","la",self._name], stdout=PIPE, stderr=STDOUT)
        exp = re.compile("\s+(.+)\s+([rlidwka]+)\n")
        for line in p.stdout :
            rightmatch = exp.match(line)
            if rightmatch :
                acl_dict[rightmatch.group(1)] = rightmatch.group(2)
        return acl_dict
    def _setACL(self, acl_dict) :
        """ internal function to set ACL. protection against naked -clear """
        if not acl_dict :
            raise NoACL("No ACL provided when clearing the original ones")
        else :
            cmd_list = ["fs","sa","-clear", "-dir", self._name, "-acl"]
            acl_list = []
            for a in acl_dict.keys() :
                acl_list.append(a)
                acl_list.append(acl_dict[a])
            cmd_list += acl_list
            Popen(cmd_list)
    def resetACL(self, recursive=False) :
        default_acl = {}
        default_acl["system:administrators"] = "rlidwka"
        default_acl["system:anyuser"] = "rl"
        default_acl[os.environ["USER"]] = "rlidwka"        
        self._setACL(default_acl)
        if recursive :
            for data in os.walk(self._name, topdown=False) :
                root = data[0]
                dirs = data[1]
                for d in dirs :
                    dirpath = os.path.join(root, d)
                    d_inst = Directory(dirpath)
                    d_inst.resetACL(recursive=False)
    def addACL(self, acl_dict, recursive=False) :
        dir_acl = self.getACL()
        for g in acl_dict.keys() :
            if acl_dict[g] :
                if acl_dict[g] == "all" :
                    acl_dict[g] = "rlidwka"
                if g not in dir_acl.keys() :
                    dir_acl[g] = acl_dict[g]
                else :
                    for l in acl_dict[g] :
                        if l not in dir_acl[g] :
                            dir_acl[g] = dir_acl[g] + l
            else :
                raise NoACL("You cannot add an empty list for %s" % g)
        self._setACL(dir_acl)
        if recursive :
            for data in os.walk(self._name, topdown=False) :
                root = data[0]
                dirs = data[1]
                for d in dirs :
                    dirpath = os.path.join(root, d)
                    d_inst = Directory(dirpath)
                    d_inst.addACL(acl_dict, recursive=False)

    def removeACL(self, acl_dict, recursive=False) :
        dir_acl = self.getACL()
        for g in acl_dict.keys() :
            if acl_dict[g] :
                if acl_dict[g] == "all" :
                    acl_dict[g] = "rlidwka"
                if g in dir_acl.keys() :
                    for l in acl_dict[g] :
                        if l in dir_acl[g] :
                            dir_acl[g] = dir_acl[g].replace(l,"")
            else :
                raise NoACL("You cannot remove an empty list for %s" % g)
        self._setACL(dir_acl)
        if recursive :
            for data in os.walk(self._name, topdown=False) :
                root = data[0]
                dirs = data[1]
                for d in dirs :
                    dirpath = os.path.join(root, d)
                    d_inst = Directory(dirpath)
                    d_inst.removeACL(acl_dict, recursive=False)
    def lockACL(self, recursive=False) :
        dir_acl = self.getACL()
        perm_to_remove = "idwk"
        for g in dir_acl.keys() :
            if g != "system:administrators" :
                for p in perm_to_remove :
                    if p in dir_acl[g] :
                        dir_acl[g] = dir_acl[g].replace(p, "")
        self._setACL(dir_acl)
        if recursive :
            for data in os.walk(self._name, topdown=False) :
                root = data[0]
                dirs = data[1]
                for d in dirs :
                    dirpath = os.path.join(root, d)
                    d_inst = Directory(dirpath)
                    d_inst.lockACL(recursive=False)
    def unlockACL(self, recursive=False):
        dir_acl = self.getACL()
        perm_to_add = "idwk"
        for g in dir_acl.keys() :
            if g != "system:anyuser" and "a" in dir_acl[g]:
                for p in perm_to_add :
                    if p not in dir_acl[g] :
                        dir_acl[g] = dir_acl[g] + p
        self._setACL(dir_acl)
        if recursive :
            for data in os.walk(self._name, topdown=False) :
                root = data[0]
                dirs = data[1]
                for d in dirs :
                    dirpath = os.path.join(root, d)
                    d_inst = Directory(dirpath)
                    d_inst.unlockACL(recursive=False)
    def isMountPoint(self):
        """ Check if the directory is a mount point """
        exp = re.compile("is\s+a\s+mount\s+point")
        notexp = re.compile("is\s+not\s+a\s+mount\s+point")
        p = Popen(["fs","lsmount", self._name], stdout=PIPE, stderr=STDOUT)
        for line in p.stdout.xreadlines() :
            if exp.search(line):
                return True
            elif notexp.search(line):
                return False
        raise NotInAFS, "Directory %s is not on an AFS volume" % self._name
    def getParentMountPoint(self):
        """ get the directory which is the parent mount point
        of the present directory
        """
        tmpdir = str(self._name)
        while (tmpdir != "/"):
            if isMountPoint(tmpdir):
                return tmpdir
            else:
                tmpdir = "/".join(tmpdir.split("/")[:-1])
                if not tmpdir:
                    tmpdir = "/"
                    
    def flush(self):
        Popen(["fs", "flush", "-path", self._name], stdout=STDOUT, stderr=STDOUT)
        
        

                    
def getDirID(dirpath):
    """ get the ID of a directory """
    tmpdir = Directory(dirpath)
    return tmpdir.ID()
    
def getDirVolumeID(dirpath):
    """ get the volume ID of a directory """
    tmpdir = Directory(dirpath)
    return tmpdir.getVolumeID()

def getDirVolumeName(dirpath):
    """ get the volume name which contains the directory """
    tmpdir = Directory(dirpath)
    return tmpdir.getVolumeName()

def isMountPoint(dirpath):
    """ Check if the directory is a mount point """
    tmpdir = Directory(dirpath)
    return tmpdir.isMountPoint()

def getParentMountPoint(dirpath):
    """ get the directory which is the parent mount point
    of the present directory
    """
    tmpdir = Directory(dirpath)
    return tmpdir.getParentMountPoint()
        
def isAFSDir(dirpath):
    """ test if the directory is an afs one """
    try:
        tmpdir = Directory(dirpath)
        tmpdir.ID()
    except NotInAFS :
        return False
    except OSError :
        return False
    else:
        return True

def isAFSFile(filepath):
    parent = os.path.dirname(filepath)
    isafs = False
    if isAFSDir(parent) :
        isafs = True
    return isafs

def isAFSPath(path):
    if os.path.isdir(path) :
        return isAFSDir(path)
    else :
        return isAFSFile(path)


def flushPath(path, delay=1.0):
    if isAFSPath(path) :
        Popen(["fs", "flush", "-path", path])
    if delay :
        sleep(delay)

class AFSLock(Lock2):
    def lock(self, force=True):
        flushPath(self.filename)
        name = self._mungedname()
        if os.path.exists(name) :
            flushPath(name)
        else :
            flushPath(os.path.dirname(name))
        super(AFSLock, self).lock(force)
    def unlock(self, ignore=True):
        flushPath(self.filename)
        name = self._mungedname()
        if os.path.exists(name) :
            flushPath(name)
        else :
            flushPath(os.path.dirname(name))
        super(AFSLock, self).unlock(ignore)

class AFSLockFile(AFSLock):
    """ plain copy of the LockFile class 
    @todo: change the way the Lock class is used. It should be passed as argument of 
    the constructor
    """
    def __init__(self, filename, mode='r', bufsize=-1, timeout=5, step=0.1, force=True):
        AFSLock.__init__(self, filename, timeout, step)
        # may raise an error if lock is ``False``
        self.lock(force)
        # may also raise an error
        self._file = open(filename, mode, bufsize)
    def close(self, ignore=True):
        """
        close the file and release the lock.
        ignore has the same meaning as for ``Lock2.unlock``
        """
        self._file.flush()
        os.fsync(self._file.fileno())
        self._file.close()
        self.unlock(ignore)
    def __getattr__(self, name):
        """delegate appropriate method/attribute calls to the file."""
        return getattr(self._file, name)
    def __setattr__(self, name, value):
        """Only allow attribute setting that don't clash with the file."""
        if not '_file' in self.__dict__:
            AFSLock.__setattr__(self, name, value)
        elif hasattr(self._file, name):
            return setattr(self._file, name, value)
        else:
            AFSLock.__setattr__(self, name, value)

    def __del__(self):
        """Auto unlock (and close file) when object is deleted."""
        if self.locked:
            self.unlock()
            self._file.close()

    
