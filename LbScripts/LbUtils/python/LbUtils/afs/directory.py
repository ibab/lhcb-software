""" AFS helper for directories """

import re
from subprocess import Popen, PIPE, STDOUT

class NotInAFS(Exception):
    """ Exception for non AFS directories """
    pass

class Directory(object):
    def __init__(self, dirpath):
        self._name = dirpath
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
    def isMountPoint(self):
        """ Check if the directory is a mount point """
        exp = re.compile("is\s+a\s+mount\s+point")
        notexp = re.compile("is\s+not\s+a\s+mount\s+point")
        p = Popen(["fs","lsmount",self._name], stdout=PIPE, stderr=STDOUT)
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
    else:
        return True

