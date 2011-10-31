""" module for AFS volume creation """

from LbUtils.afs.directory import Directory
from LbUtils.Storage import Unit
from subprocess import Popen, PIPE
import re
import logging

# the overall length of a volume name is 31 characters. But it has to allow a ".readonly" extension
# when a replica is created. So the max number of characters for the volume name is 22.
MAX_NAME_LENGTH = 22

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

class BadVolumeName(Exception):
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
                # loose method: use any mountpoint that contains the directory
                if d.isMountPoint() :
                    self.addMountPoint(d.name())
                else :
                    m = Directory(d.getParentMountPoint())
                    self.addMountPoint(m.name())
            if name :
                # checks if the provided volume name is consitent with the one the mount point
                if m.getVolumeName() == name :
                    self._name = name
                else :
                    raise IsNotaMountPointOfVolume, "%s is not a mount point of %s" % (m.name(), name)
            else :
                # deduce the volume name from the mountpoint
                self._name = m.getVolumeName()
        else :
            # if no directory has been provided only use the name
            if name :
                self._name = name
            else :
                raise IsNotaVolume
    def ID(self):
        if self._mtpoints :
            return self._mtpoints[0].getVolumeID()
        else :
            raise HasNoMountPoint, "%s has no mount point" % self._name
    def name(self):
        return self._name
    def flush(self):
        log = logging.getLogger()
        for m in self._mtpoints :
            p = Popen(["fs", "flushvolume", m.name()], stdout=PIPE, stderr=PIPE)
            for line in p.stdout.xreadlines() :
                log.debug(line[:-1])
            for line in p.stderr.xreadlines() :
                log.error(line[:-1])
            if p.wait() != 0:
                raise IOError, "Could not flush volume %s" % self._name

    def usedSpace(self, display_size=None):
        log = logging.getLogger()
        used_size = 0
        exp = re.compile("%s\s+\d+\s+(\d+)\s+" % re.escape(self._name))
        p = Popen(["fs", "listquota", self._mtpoints[0].name()], stdout=PIPE, stderr=PIPE)
        for line in p.stdout.xreadlines() :
            m = exp.match(line[:-1])
            if m :
                used_size = m.group(1)
                break
        for line in p.stderr.xreadlines() :
            log.error(line[:-1])
        if p.wait() != 0:
            raise IOError, "Could not get used space of volume %s" % self._name
        if display_size :
            used_size = Unit(int(used_size), "KB")
            used_size.setDisplay(display_size)
        return used_size
    def quota(self, display_size=None):
        log = logging.getLogger()
        quota_size = 0
        exp = re.compile("%s\s+(\d+)\s+\d+\s+" % re.escape(self._name))
        p = Popen(["fs", "listquota", self._mtpoints[0].name()], stdout=PIPE, stderr=PIPE)
        for line in p.stdout.xreadlines() :
            m = exp.match(line[:-1])
            if m :
                quota_size = m.group(1)
                break
        for line in p.stderr.xreadlines() :
            log.error(line[:-1])
        if p.wait() != 0:
            raise IOError, "Could not get quota of volume %s" % self._name
        if display_size :
            quota_size = Unit(int(quota_size), "KB")
            quota_size.setDisplay(display_size)
        return quota_size
    def isFull(self):
        is_full = False
        if self.usedSpace() >= self.quota() :
            is_full = True
        return is_full
    def setQuota(self, quota):
        log = logging.getLogger()
        p = Popen(["afs_admin", "set_quota", self._mtpoints[0].name(), quota], stdout=PIPE, stderr=PIPE)
        for line in p.stdout.xreadlines() :
            log.debug(line[:-1])
        for line in p.stderr.xreadlines() :
            log.error(line[:-1])
        if p.wait() != 0:
            raise IOError, "Could not set quota of volume %s" % self._name
    def adjustQuota(self):
        actual_size = self.usedSpace()
        quota_size = self.quota()
        if actual_size != quota_size :
            self.setQuota(actual_size)
    def mountPoints(self):
        return self._mtpoints
    def addMountPoint(self, dirname):
        self._mtpoints.append(MountPoint(dirname))
    def addACL(self, acl_dict):
        for m in self._mtpoints :
            m.addACL(acl_dict, recursive=True)
    def removeACL(self, acl_dict):
        for m in self._mtpoints :
            m.removeACL(acl_dict, recursive=True)
    def lockACL(self):
        for m in self._mtpoints :
            m.lockACL(recursive=True)
    def unlockACL(self):
        for m in self._mtpoints :
            m.unlockACL(recursive=True)
    def lock(self):
        self.adjustQuota()
        self.lockACL()
    def unlock(self):
        self.unlockACL()


def createVolume(path, volume_name, quota = None, user = None, group = None):
    """
    Create an AFS volume in the specified path with the specified name.

    @param path: directory where to create the volume
    @param volume_name: name to use for the volume
    @param quota: initial quota, if specified
    @param group: group whose librarians will have access

    @return: Volume instance
    """

    if len(volume_name) > MAX_NAME_LENGTH :
        raise BadVolumeName, "Volume name %s is too long. volume names are limited to %s characters." % (volume_name, MAX_NAME_LENGTH)
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

    volume = Volume(name=volume_name, dirname=path)

    if group:
        # give write access to librarians
        vol_acl = {}
        vol_acl[group + ':librarians'] = "all"
        volume.addACL(vol_acl)

    return volume
