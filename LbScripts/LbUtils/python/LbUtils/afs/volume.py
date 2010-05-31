""" module for AFS volume creation """

from LbUtils.afs.directory import Directory
from subprocess import Popen

class HasNoMountPoint(Exception):
    """ Exception for volume without any mount point """
    pass

class IsNotaMountPoint(Exception):
    """ Exception for directories which are not mount points """
    pass

class MountPoint(Directory):
    def __init__(self, dirpath):
        super(MountPoint, self).__init__(dirpath)
        if not self.isMountPoint():
            raise IsNotaMountPoint, "%s is not a mount point" % self._name

class IsNotaVolume(Exception):
    pass        

class IsNotaMountPointOfVolume(Exception):
    pass


class Volume(object):
    def __init__(self, name=None, dirname=None, loose=False):
        self._mtpoints = []
        if dirname:
            d = Directory(dirname)
            m = d
            if not loose :
                self.addMountPoint(d.name())
            else :
                if d.isMountPoint() :
                    self.addMountPoint(d.name())
                else :
                    m = Directory(d.getParentMountPoint())
                    self.addMountPoint(m.name())
            if name :
                if m.getVolumeName() == name :
                    self._name = name
                else :
                    raise IsNotaMountPointOfVolume, "%s is not a mount point of %s" % (m.name(), name) 
            else :
                self._name = m.getVolumeName()
        else :
            if name :
                self._name = name
            else :
                raise IsNotaVolume
    def ID(self):
        if self._mtpoints :
            self._mtpoints[0].getVolumeID()
        else :
            raise HasNoMountPoint, "%s has no mount point" % self._name
            
    def name(self):
        return self._name 
    def flush(self):
        pass
    def usedSpace(self):
        pass
    def quota(self):
        pass
    def setQuota(self):
        pass
    def increaseQuota(self):
        pass
    def decreaseQuota(self):
        pass
    def adjustQuota(self):
        pass
    def mountPoints(self):
        return self._mtpoints
    def addMountPoint(self, dirname):
        self._mtpoints += MountPoint(dirname)


def createVolume(path, volume_name, quota = None, user = None, group = None):
    """
    Create an AFS volume in the specified path with the specified name.
    
    @param path: directory where to create the volume
    @param volume_name: name to use for the volume
    @param quota: initial quota, if specified
    @param group: group whose librarians will have access
    
    @return: Volume instance
    """
    if not user:
        import getpass
        user = getpass.getuser()
    
    # create the new volume
    cmd = ["afs_admin", "create", "-u", user]
    if quota:
        cmd += ["-q", str(quota)]
        
    proc = Popen(cmd + [path, volume_name])
    if proc.wait() != 0:
        raise IOError, "Could not create volume %s in %s" % (volume_name, path)
    
    if group:
        # give write access to librarians
        Popen(["fs", "setacl", path, group + ':librarians', "all"]).wait()
    return Directory(path)
