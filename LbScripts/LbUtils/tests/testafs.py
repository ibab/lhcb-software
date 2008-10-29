# $Id: testafs.py,v 1.3 2008-10-29 15:31:28 hmdegaud Exp $
""" test file for the AFS module """

__version__= "$Name: not supported by cvs2svn $".replace("$", "").replace("Name:", "").strip()
__revision__ = "$Revision: 1.3 $"

from os import environ, sep
from LbUtils.afs.directory import getDirID, getDirVolumeID, getDirVolumeName
from LbUtils.afs.directory import getParentMountPoint
from LbUtils.afs.directory import isMountPoint

if __name__ == '__main__':
    print isMountPoint(environ["HOME"])
    print isMountPoint(sep.join([environ["HOME"],"tmp"]))
    print isMountPoint(environ["HOME"])
    print "========================"
    print getDirID(sep.join([environ["HOME"],"tmp"]))
    print "========================"
    print getDirVolumeID(sep.join([environ["HOME"],"tmp"]))
    print "========================"
    print getParentMountPoint(environ["HOME"])
    print getParentMountPoint(sep.join([environ["HOME"], "tmp"] ))
    print getParentMountPoint(sep.join([environ["HOME"], "scratch0"] ))
    print getParentMountPoint(sep.join([environ["HOME"], 
                                   "scratch0",
                                   "GaudiDev"] ))
    print getParentMountPoint(sep.join([environ["HOME"], 
                                   "scratch0",
                                   "GaudiDev",
                                   "GaudiKernel"] ))

    print "============================================="
#    print isMountPoint("/tmp")
#    print getDirID("/tmp")
#   print getDirVolumeID("/tmp")
#    print getParentMountPoint("/tmp")
    print "============================================="
    print getDirVolumeName(environ["HOME"])
    print getDirVolumeName(sep.join([environ["HOME"], "tmp"] ))
    print getDirVolumeName(sep.join([environ["HOME"], "scratch0"] ))
    print getDirVolumeName(sep.join([environ["HOME"], 
                                   "scratch0",
                                   "GaudiDev"] ))
    print getDirVolumeName("/tmp")
    