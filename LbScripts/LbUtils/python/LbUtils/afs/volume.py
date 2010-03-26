""" module for AFS volume creation """

from LbUtils.afs.directory import Directory
from subprocess import Popen, PIPE

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
        


class Volume(object):
    def __init__(self, name, dirname=None):
        self._mtpoints = []
        if dirname:
            self.addMountPoint(dirname)
        self._name = name
        self._quota = 0
        self._used = 0
        self._id = 0
        self.update()
    def ID(self):
        return self._id 
    def name(self):
        return self._name 
    def mountPoints(self):
        return self._mtpoints
    def addMountPoint(self, dirname):
        self._mtpoints += MountPoint(dirname)
    def update(self):
        if len(self._mtpoints) == 0:
            raise HasNoMountPoint, "Volume %s has not mount point" % self._name
        else:
            pass


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
