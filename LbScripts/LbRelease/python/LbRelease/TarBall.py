"""
Generic Tar Ball module
"""
from LbConfiguration import Platform
from LbConfiguration import Version
from LbConfiguration import Project
from LbConfiguration import Package
from LbUtils import CMT
from LbUtils.Tar import createTarBallFromFilter
from LbUtils.Temporary import TempDir
from LbUtils.File import createMD5File, checkMD5Info

from fnmatch import fnmatch

import logging

import os
import tarfile
import shutil

class EmptyTarBallException(Exception): pass
class EmptySourceDirsException(Exception): pass
class TarBallExistException(Exception): pass

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
        return Platform.pathBinaryMatch(path, cmtconfig)
    else :
        return Platform.pathSharedMatch(path)

def generateMD5(project, version, cmtconfig=None, input_dir=None):
    log = logging.getLogger()
    prj_conf = Project.getProject(project)
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

def generateDataMD5(project, version, input_dir=None):
    log = logging.getLogger()
    prj_conf = Package.getPackage(project)
    if not input_dir :
        input_dir = prj_conf.TarBallDir()
    filename = os.path.join(input_dir, prj_conf.tarBallName(version, full=True))
    if os.path.exists(filename) :
        md5fname = os.path.join(input_dir, prj_conf.md5FileName(version))
        targetname = os.path.basename(filename)
        if os.path.exists(md5fname) :
            os.remove(md5fname)
            log.info("Replacing %s for %s" % (md5fname, filename))
        else :
            log.info("Creating %s for %s" % (md5fname, filename))                
        createMD5File(filename, md5fname, targetname)
    else :
        log.warning("The file %s doesn't exist. Skipping md5 creation." % filename)


def generateHTML(project, version, cmtconfig=None, top_dir=None, output_dir=None):
    log = logging.getLogger()
    prj_conf = Project.getProject(project)
    if not top_dir :
        top_dir = prj_conf.ReleaseArea()
    if not output_dir :
        output_dir = prj_conf.HTMLDir()
        tb_dir = prj_conf.TarBallDir()
    else :
        tb_dir = output_dir
    if not version :
        pattern = "%s_*" % prj_conf.NAME()
        maindir = os.path.join(top_dir, prj_conf.NAME())
        version = str(Version.getVersionsFromDir(maindir, pattern, reverse=True)[0])
        log.debug("Guessed version for %s is %s" % (prj_conf.Name(), version))
    filename = os.path.join(tb_dir, prj_conf.tarBallName(version, cmtconfig, full=True))
    if os.path.exists(filename) :
        print getTarDependencies(project, version, 
                                 cmtconfig, top_dir, 
                                 project_only=False, full=True)
#    prefix = prj_conf.releasePrefix(version)
#    prj_path = os.path.join(top_dir, prefix)
#    prj = CMT.Project(prj_path)
#    cmtprojectpath = os.environ["LHCBRELEASES"]
#    if os.environ["GAUDISOFT"] != cmtprojectpath :
#        cmtprojectpath = os.pathsep.join([cmtprojectpath, os.environ["GAUDISOFT"]])
#    cmtprojectpath = os.pathsep.join([cmtprojectpath, os.environ["LCG_release_area"]])
#    for p in  prj.externalPackages(cmtprojectpath=cmtprojectpath) :
#        parprj = p.parentProjectPath()
#        print p.name(), parprj.name(), parprj.version()
#    for data in CMT.walk(top=prj, cmtprojectpath=cmtprojectpath) :
#        print "%s\t\t%s\t\t@\t\t%s" % (data[0].name(), data[0].version(), data[0].fullLocation())

def generateDataHTML(package, version, top_dir=None, output_dir=None) :
    log = logging.getLogger()
    pkg_conf = Package.getPackage(package)
    if not top_dir :
        top_dir = pkg_conf.ReleaseArea()
    if not output_dir :
        output_dir = pkg_conf.HTMLDir()
    if not version :
        maindir = os.path.join(top_dir, pkg_conf.releasePrefix())
        version = str(Version.getVersionsFromDir(maindir, pattern=None, reverse=True)[0])
        log.debug("Guessed version for %s is %s" % (pkg_conf.Name(), version))
    prefix = pkg_conf.releasePrefix(version)
    pkg_path = os.path.join(top_dir, prefix)
    pkg = CMT.Package(pkg_path)
    
def generateTar(project, version=None, cmtconfig=None, 
                top_dir=None, output_dir=None, overwrite=False,
                update=False, md5=True, html=True):
    log = logging.getLogger()
    prj_conf = Project.getProject(project)
    status = 0
    if not top_dir :
        top_dir = prj_conf.ReleaseArea()
    if not output_dir :
        output_dir = prj_conf.TarBallDir()
    if not version :
        pattern = "%s_*" % prj_conf.NAME()
        maindir = os.path.join(top_dir, prj_conf.NAME())
        version = str(Version.getVersionsFromDir(maindir, pattern, reverse=True)[0])
        log.debug("Guessed version for %s is %s" % (prj_conf.Name(), version))
    prefix = prj_conf.releasePrefix(version)
    srcdirs = [os.path.join(top_dir, prefix)]
    log.debug("="*100)
    filename = os.path.join(output_dir, prj_conf.tarBallName(version, cmtconfig, full=True))
    if os.path.exists(filename) and not (overwrite or update) :
        log.info("The file %s already exists. Skipping." % filename)
        status = 2
    else :
        if overwrite :
            os.remove(filename)
        if cmtconfig :
            pathfilter = lambda x : projectFilter(x, cmtconfig)
        else :
            pathfilter = projectFilter
        prefix = prj_conf.releasePrefix(version)
        status = createTarBallFromFilter(srcdirs, filename, pathfilter,
                                prefix=prefix, dereference=False, update=update)
        if status != 0 :
            if status == 1 :
                log.fatal("The source directories do not exist")
        else :
            if md5 :
                generateMD5(project, version, cmtconfig, output_dir)
            if html :
                generateHTML(project, version, cmtconfig, top_dir, output_dir)
    return status

def generateDataTar(package, version=None, top_dir=None, 
                    output_dir=None, overwrite=False, 
                    md5=True, html=True):
    log = logging.getLogger()
    pkg_conf = Package.getPackage(package)
    if not top_dir :
        top_dir = pkg_conf.ReleaseArea()
    if not output_dir :
        output_dir = pkg_conf.TarBallDir()
    if not version :
        maindir = os.path.join(top_dir, pkg_conf.releasePrefix())
        version = str(Version.getVersionsFromDir(maindir, pattern=None, reverse=True)[0])
        log.debug("Guessed version for %s is %s" % (pkg_conf.Name(), version))
    prefix = pkg_conf.releasePrefix(version)
    srcdirs = [os.path.join(top_dir, prefix)]
    log.debug("="*100)
    filename = os.path.join(output_dir, pkg_conf.tarBallName(version, full=True))
    pathfilter = projectFilter
    if os.path.exists(filename) and not overwrite :
        log.info("The file %s already exists. Skipping." % filename)
        status = 2
    else :
        status = createTarBallFromFilter(srcdirs, filename, pathfilter,
                                         prefix=prefix, dereference=pkg_conf.dereferenceTar(), 
                                         update=False)
        if status == 1 :
            log.fatal("The source directories do not exist")
            return status
        if md5 :
            generateDataMD5(package, version, output_dir)
        if html :
            generateDataHTML(package, version, top_dir, output_dir)
    return status

def getParentProjects(project, version=None, cmtconfig=None, top_dir=None):
    log = logging.getLogger()
    if Project.isProject(project) :
        pcfg = Project.getProject(project)
        if not top_dir :
            top_dir = pcfg.ReleaseArea()
        if not version :
            pattern = "%s_*" % pcfg.NAME()
            maindir = os.path.join(top_dir, pcfg.NAME())
            version = str(Version.getVersionsFromDir(maindir, pattern, reverse=True)[0])
            log.debug("Guessed version for %s is %s" % (pcfg.Name(), version))
    elif Package.isPackage(project) :
        pcfg = Package.getPackage(project)
        if not top_dir :
            top_dir = pcfg.ReleaseArea()
        if not version :
            maindir = os.path.join(top_dir, pcfg.releasePrefix())
            version = str(Version.getVersionsFromDir(maindir, pattern=None, reverse=True)[0])
            log.debug("Guessed version for %s is %s" % (pcfg.Name(), version))
    prj_dict = getTarDependencies(project, version, cmtconfig, 
                                  top_dir, project_only=False, full=False)
    par_dict = {}
    fulllist = [ x.upper() for x in Project.project_names ]
    fulllist += [ x.upper() for x in Package.package_names ]
    for p in prj_dict.keys() :
        if not prj_dict[p] :
            continue
        if p.upper() not in fulllist :
            continue
        par_dict[p] = prj_dict[p]
    return par_dict

def getTarDependencies(project, version=None, cmtconfig=None, top_dir=None,
                       project_only=False, full=True):
    prj_dict = {}
    log = logging.getLogger()
    cmtprojectpath = os.environ["CMTPROJECTPATH"]
    if Project.isProject(project) :
        pcfg = Project.getProject(project)
        if not top_dir :
            top_dir = pcfg.ReleaseArea()
        if not version :
            pattern = "%s_*" % pcfg.NAME()
            maindir = os.path.join(top_dir, pcfg.NAME())
            version = str(Version.getVersionsFromDir(maindir, pattern, reverse=True)[0])
            log.debug("Guessed version for %s is %s" % (pcfg.Name(), version))
        prefix = pcfg.releasePrefix(version)
        ppath = os.path.join(top_dir, prefix)
        prj = CMT.Project(ppath)
        if not full :
            for p in prj.base(cmtprojectpath=cmtprojectpath) :
                prj_dict[p.name()] = p.version()
        else :
            for data in CMT.walk(top=prj, cmtprojectpath=cmtprojectpath) :
                prj_dict[data[0].name()] = data[0].version()
        for p in prj_dict.keys() :
            if p == "LCGCMT" :
                if prj_dict[p] :
                    prj_dict[p] = prj_dict[p].split("_")[1]
            elif p == "CMTUSERCONTEXT" :
                del prj_dict[p]
                continue
            elif p.upper() == project.upper() :
                del prj_dict[p]
                continue                
            else :
                ver = Version.extractVersion(prj_dict[p])
                if ver :
                    prj_dict[p] = ver.name()
        if not project_only :
            has_dataproj = False
            for p in Package.project_names :
                if p in prj_dict.keys() :
                    has_dataproj = True
                    break
            if has_dataproj :
                for p in  prj.externalPackages(cmtprojectpath=cmtprojectpath) :
                    parprj = p.parentProject()
                    if parprj.name() in Package.project_names :
                        prj_dict[p.coreName()] = p.version()
    elif Package.isPackage(project) :
        pcfg = Package.getPackage(project)
        if not top_dir :
            top_dir = pcfg.ReleaseArea()
        if not version :
            maindir = os.path.join(top_dir, pcfg.releasePrefix())
            version = str(Version.getVersionsFromDir(maindir, pattern=None, reverse=True)[0])
            log.debug("Guessed version for %s is %s" % (pcfg.Name(), version))
            prefix = pcfg.releasePrefix(version)
    return prj_dict

def buildTar(project, version=None, cmtconfig=None, 
             top_dir=None, output_dir=None, 
             overwrite=False, overwrite_shared=False, update=False,
             md5=True, html=True, recursive=False):
    log = logging.getLogger()
    status = 0
    if Project.isProject(project) :
        log.debug("%s is a software project" % project)
        prj_conf = Project.getProject(project)
        if not top_dir :
            top_dir = prj_conf.ReleaseArea()
        if not output_dir :
            output_dir = prj_conf.TarBallDir()
        if not version :
            pattern = "%s_*" % prj_conf.NAME()
            maindir = os.path.join(top_dir, prj_conf.NAME())
            version = str(Version.getVersionsFromDir(maindir, pattern, reverse=True)[0])
            log.debug("Guessed version for %s is %s" % (prj_conf.Name(), version))
        if not cmtconfig :
            fixed_cmtconfig = False
            cmtconfig = Platform.binary_list
            if overwrite :
                overwrite_shared = True
        else :
            fixed_cmtconfig = True
        update_shared = False
        for c in cmtconfig :
            status = generateTar(project, version, c, top_dir, output_dir, 
                                 overwrite, update, md5, html)
            if status == 1 :
                return status
            if status != 2 :
                update_shared = True
        if update_shared :
            if overwrite_shared :
                status = generateTar(project, version, cmtconfig=None, 
                                     top_dir=top_dir, output_dir=output_dir, 
                                     overwrite=True, update=True, md5=md5, html=html)
            else :
                status = generateTar(project, version, cmtconfig=None, 
                                     top_dir=top_dir, output_dir=output_dir, 
                                     overwrite=False, update=True, md5=md5, html=html)
    elif Package.isPackage(project) :
        log.debug("%s is a data package" % project)
        status = generateDataTar(project, version, top_dir, output_dir, overwrite, md5, html)
    else :
        log.fatal("%s is neither a software project nor a data package")
        status = 1
    if recursive :
        par_prj = getParentProjects(project, version, None, top_dir)
        for p in par_prj.keys() :
            status = buildTar(p, par_prj[p], cmtconfig, top_dir, output_dir, overwrite, overwrite_shared, update, md5, html, recursive)
    return status

def checkDataMD5(package, version=None, input_dir=None):
    log = logging.getLogger()
    pkg_conf = Package.getPackage(package)
    good = False
    if not input_dir :
        input_dir = pkg_conf.TarBallDir()
    if not version :
        pattern = "%s_*" % pkg_conf.tarBallName()
        version = str(Version.getVersionsFromDir(input_dir, pattern, reverse=True)[0])
        log.debug("Guessed version for %s is %s" % (pkg_conf.Name(), version))
    filename = os.path.join(input_dir, pkg_conf.tarBallName(version, full=True))
    if os.path.exists(filename) :
        md5fname = os.path.join(input_dir, pkg_conf.md5FileName(version))
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


def checkMD5(project, version=None, cmtconfig=None, input_dir=None):
    log = logging.getLogger()
    prj_conf = Project.getProject(project)
    good = False
    if not input_dir :
        input_dir = prj_conf.TarBallDir()
    if not version :
        pattern = "%s_*" % prj_conf.NAME()
        version = str(Version.getVersionsFromDir(input_dir, pattern, reverse=True)[0])
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
    good = True
    if Project.isProject(project) :
        log.debug("%s is a software project" % project)
        prj_conf = Project.getProject(project)
        if not input_dir :
            input_dir = prj_conf.TarBallDir()
        if not version :
            pattern = "%s_*" % prj_conf.NAME()
            version = str(Version.getVersionsFromDir(input_dir, pattern, reverse=True)[0])
            log.debug("Guessed version for %s is %s" % (prj_conf.Name(), version))
        if not cmtconfig :
            cmtconfig = Platform.binary_list
    
        for c in cmtconfig :
            log.debug("="*100)
            status = checkMD5(project, version, c, input_dir)
            if not status :
                good = False 
                if not keep_going and not good:
                    return good
        log.debug("="*100)
        status = checkMD5(project, version, None, input_dir)
    elif Package.isPackage(project) :
        log.debug("%s is a data package" % project)
        status = checkDataMD5(project, version, input_dir)
    else :
        log.fatal("%s is neither a software project nor a data package")
        status = 1
        
    if not status :
        good = False
    return good

if __name__ == '__main__':
    pass