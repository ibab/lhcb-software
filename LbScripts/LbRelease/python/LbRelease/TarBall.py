"""
Generic Tar Ball module
"""
from LbConfiguration.Project import getProject
from LbConfiguration.Platform import pathBinaryMatch, pathSharedMatch
from LbConfiguration.Platform import binary_list
from LbConfiguration.Version import getVersionsFromDir
from LbUtils.Tar import createTarBallFromFilter
from LbUtils.Temporary import TempDir
from LbUtils.File import createMD5File, checkMD5Info

from fnmatch import fnmatch

import logging

import os, sys
import tarfile
import shutil

class TarBall(object):
    def __init__(self, name, srcloc=None):
        self._name = ""
        self.setName(name)
        self._srcLoc = []
        self.addSourceLocation(srcloc)
        self._exclude = list()
        self._converter = dict()
        self._destLoc = None
        if not self._destLoc:
            self._destLoc = os.path.abspath(os.getcwd())
        self._tmpdir = TempDir(self._name)
        print self._tmpdir
    def setName(self, name):
        self._name = name
    def getName(self):
        return self._name
    def addSourceLocation(self, srcloc):
        self._srcLoc += os.path.abspath(srcloc)
    def getSourceLocation(self):
        return self._srcLoc
    def addExclusion(self, pattern):
        self._exclude += pattern
    def addConverter(self, target, source):
        """ will convert the source into the target before the 
            creation of the tar ball. A target can have many sources """
        self._converter.setdefault(target, []).append(source)
    def explodeSources(self):
        for s in self._srcLoc:
            if os.path.isfile(s):
                if tarfile.is_tarfile(s):
                    srctar = tarfile.open(s,"r:*")
                    srctar.extractall(self._tmpdir.getName())
                    srctar.close()
                else:
                    print "%s is not a tar file. Skipping ..." % s
            else:
                shutil.copytree(s, self._tmpdir.getName())

    def create(self):
        curdir = os.getcwd()
        os.chdir(self._tmpdir.getName())
        os.chdir(curdir)


class BinaryTarBall(TarBall):
    """ Binary tar ball. Only include the selected binary. Thus
        filter out the unwanted other binaries """
    def __init__(self, name, srcloc, binary):
        super(BinaryTarBall, self).__init__(name, srcloc)
        self._binaries = [binary]
    def addBinary(self, binary):
        self._binaries += binary

def projectFilter(path, cmtconfig=None):
    exclude_file_list = ["install.history", "core.*", "*.pyc", "*.pyo", ".glimpse_*",
                         "*.o", "lib*.a", "*.obj" ]
    exclude_dir_list = ["html", "Visual"]

    exclude_list = [ os.path.join("*", x) for x in exclude_file_list]
    exclude_list += [ os.path.join("*", x, "*") for x in exclude_dir_list]
    for e in exclude_list :
        if fnmatch(path, e) :
            return False
    
    if cmtconfig :
        return pathBinaryMatch(path, cmtconfig)
    else :
        return pathSharedMatch(path)

def generateMD5(project, version, cmtconfig=None, input_dir=None):
    log = logging.getLogger()
    prj_conf = getProject(project)
    if not input_dir :
        input_dir = prj_conf.TarBallDir()
    filename = os.path.join(input_dir, prj_conf.tarBallName(version, cmtconfig, full=True))
    if os.path.exists(filename) :
        md5fname = os.path.join(input_dir, prj_conf.md5FileName(version, cmtconfig))
        targetname = os.path.basename(filename)
        if os.path.exists(md5fname) :
            os.remove(md5fname)
            log.info("Replacing %s for %s" % (md5fname, filename))
        else :
            log.info("Creating %s for %s" % (md5fname, filename))                
        createMD5File(filename, md5fname, targetname)
    else :
        log.warning("The file %s doesn't exist. Skipping md5 creation." % filename)

def buildTar(project, version=None, cmtconfig=None, 
             top_dir=None, output_dir=None, 
             overwrite=False, overwrite_shared=False, update=False,
             md5=True, html=True):
    log = logging.getLogger()
    prj_conf = getProject(project)
    if not top_dir :
        top_dir = prj_conf.ReleaseArea()
    if not output_dir :
        output_dir = prj_conf.TarBallDir()
    if not version :
        pattern = "%s_*" % prj_conf.NAME()
        maindir = os.path.join(top_dir, prj_conf.NAME())
        version = str(getVersionsFromDir(maindir, pattern, reverse=True)[0])
        log.debug("Guessed version for %s is %s" % (prj_conf.Name(), version))
    if not cmtconfig :
        cmtconfig = binary_list
        if overwrite :
            overwrite_shared = True
    prefix = prj_conf.releasePrefix(version)
    srcdirs = [os.path.join(top_dir, prefix)]
    for c in cmtconfig :
        filename = os.path.join(output_dir, prj_conf.tarBallName(version, c, full=True))
        if os.path.exists(filename) and not (overwrite or update) :
            sys.exit("The file %s already exists. Please remove it first." % filename)
        pathfilter = lambda x : projectFilter(x, c)
        prefix = prj_conf.releasePrefix(version)
        if update :
            log.info("Updating %s with %s" % (filename, ", ".join(srcdirs)) )
        else :
            log.info("Creating %s with %s" % (filename, ", ".join(srcdirs)) )        
        createTarBallFromFilter(srcdirs, filename, pathfilter,
                                prefix=prefix, dereference=False, update=update)
        if md5 :
            generateMD5(project, version, c, output_dir)

    filename = os.path.join(output_dir, prj_conf.tarBallName(version, cmtconfig=None, full=True))
    pathfilter = projectFilter
    if overwrite_shared :
        if os.path.exists(filename) :
            os.remove(filename)
            log.info("Replacing %s with %s" % (filename, ", ".join(srcdirs)) )
        else :
            log.info("Creating %s with %s" % (filename, ", ".join(srcdirs)) )            
        createTarBallFromFilter(srcdirs, filename, pathfilter,
                            prefix=prefix, dereference=False, update=False)        
        if md5 :
            generateMD5(project, version, None, output_dir)
    else :
        if os.path.exists(filename) :
            log.info("Updating %s with %s" % (filename, ", ".join(srcdirs)) )
        else :
            log.info("Creating %s with %s" % (filename, ", ".join(srcdirs)) )
        createTarBallFromFilter(srcdirs, filename, pathfilter,
                            prefix=prefix, dereference=False, update=True)
        if md5 :
            generateMD5(project, version, None, output_dir)

def checkMD5(project, version=None, cmtconfig=None, input_dir=None):
    log = logging.getLogger()
    prj_conf = getProject(project)
    good = False
    if not input_dir :
        input_dir = prj_conf.TarBallDir()
    if not version :
        pattern = "%s_*" % prj_conf.NAME()
        version = str(getVersionsFromDir(input_dir, pattern, reverse=True)[0])
        log.debug("Guessed version for %s is %s" % (prj_conf.Name(), version))
    filename = os.path.join(input_dir, prj_conf.tarBallName(version, cmtconfig, full=True))
    if os.path.exists(filename) :
        md5fname = os.path.join(input_dir, prj_conf.md5FileName(version, cmtconfig))
        target_name = os.path.basename(filename)
        if os.path.exists(md5fname) :
            log.info("Checking %s for %s" % (md5fname, filename))
            if checkMD5Info(md5fname, target_name) :
                log.info("The file %s is OK" % md5fname)
                good = True
            else :
                log.error("The file %s is wrong" % md5fname)
                good = False
        else :
            log.error("The file %s doesn't exist" % md5fname)
            good = False                
    else :
        log.warning("The file %s doesn't exist. Skipping md5 check." % filename)
        good = True
    return good

def checkTar(project, version=None, cmtconfig=None, input_dir=None, 
             keep_going=False):
    log = logging.getLogger()
    prj_conf = getProject(project)
    good = True
    if not input_dir :
        input_dir = prj_conf.TarBallDir()
    if not version :
        pattern = "%s_*" % prj_conf.NAME()
        version = str(getVersionsFromDir(input_dir, pattern, reverse=True)[0])
        log.debug("Guessed version for %s is %s" % (prj_conf.Name(), version))
    if not cmtconfig :
        cmtconfig = binary_list

    for c in cmtconfig :
        status = checkMD5(project, version, c, input_dir)
        if not status :
            good = False 
            if not keep_going and not good:
                return good
    status = checkMD5(project, version, None, input_dir)
    if not status :
        good = False
    return good

if __name__ == '__main__':
    pass