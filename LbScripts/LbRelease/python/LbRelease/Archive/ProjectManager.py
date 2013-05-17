'''
Class that represents a specific version of a project, with all associated
artifacts in the release area and in the distribution directory.

To use it just:

p = ProjectManager.loadFromDisk('Ganga', 'v506r11')
p.loadHTMLDeps()   # The HTML dependency files are not parsed automatically
p.checkDistConsistency() # Sample consistency checks in the repository

Created on Mar 14, 2012
@author: Ben Couturier
'''

from LbConfiguration import Project
from LbConfiguration import Platform
from LbUtils import HTMLDependencyManager
import os
import re

ARCHIVE_SUFFIX  = ".tar.gz"
CHECKSUM_SUFFIX = ".md5"
HTML_SUFFIX = ".html"

KNOWN_SUFFIXES = [ARCHIVE_SUFFIX, CHECKSUM_SUFFIX, HTML_SUFFIX]


# TODO: This should not be hardcoded...
LFN_SOFTWARE_BASE = "/lhcb/software/"
LFN_DIST = LFN_SOFTWARE_BASE + "tar/"
LFN_RELEASE_AREA = LFN_SOFTWARE_BASE + "backup/"

# CASTOR dir for backup
CASTOR_DIR="/castor/cern.ch/lhcb/soft-backup/"

SOFTWARE_SE = "CERN-SOFTWARE"
ARCHIVE_SE  = "CERN-ARCHIVE"

ARCH_CASTOR="castor"
ARCH_DIRAC ="dirac"
ARCHIVES = [ ARCH_CASTOR, ARCH_DIRAC]

#
# Utility methods to parse paths
###############################################################################
def stripSuffix(filename):
    if filename:
        for suffix in KNOWN_SUFFIXES:
            pos =  filename.find(suffix)
            if pos >= 0:
                filename = filename[:pos]
                break
    return filename

def parseConfig(prefix, filename):
    """ Extract the CMT config from a filename
    Expects files of the form MOORE_MOORE_v12r8g1_x86_64-slc5-gcc43-opt.tar.gz
    and returns the CMTCONFIG
    """
    # First remove the known prefix...
    tmp = filename.replace(prefix, "")

    # removing the suffix
    if tmp:
        tmp = stripSuffix(tmp)
    # Removing "-" or "_" at the front if present
    if tmp:
        if tmp[0] == '-' or tmp[0] == '_':
            tmp=tmp[1:]
    else:
        # string is empty, that was a noarch tar !
        tmp = "noarch"

    return tmp


def parseConfigFromFileList(prefix, filelist):
    """ Looks though the list to extract the configs """
    return [ parseConfig(prefix, t) for t in filelist]

def fileMatches(expectedPrefix, filename):
    """ Util function for matching the files in the at directories
    e.g. to make sure that version v13r2 doesn't return a match with
    v13r2p1 """
    # We have two cases for those:
    # PROJ_PROJ_vXrY.tar.gz or
    # PROJ_PROJ_vXrY_cmtconfig.tar.gz or
    # Therefor check that the letter matches the prefix and is followed by
    # . or _
    tmpreg = "^%s[\._]" % expectedPrefix
    return re.match(tmpreg, filename)
#
# Classes representing build artifacts
###############################################################################
class DistFile(object):
    """ Ancestor for all representation of distribution files """
    def __init__(self, name, project):
        """ Main constructor for the ancestor
        Should NOT be called directly"""
        self.name = name
        self.project = project

    def getLFN(self):
        """ Builds the LFN for a DIST file """
        return LFN_DIST + self.getDistFileName()

    def getCastorFilename(self):
        """ Builds the CASTOR filename for a DIST file """
        return CASTOR_DIR + self.getDistFileName()

    def getCastorDirname(self):
        """ Builds the CASTOR filename for a DIST file """
        return os.path.dirname(CASTOR_DIR + self.getDistFileName())

    def getFullPath(self):
        """ Builds the full path """
        return os.path.join(self.project.DistLocation(), self.getDistFileName())

class TarFile(DistFile):
    """ TAR File """
    def __init__(self, name, project):
        super(TarFile, self).__init__(name, project)

    def getDistFileName(self):
        return os.path.join(self.project.NAME(), self.name)

class LCGTarFile(DistFile):
    """ LCG TAR File """
    def __init__(self, name, project):
        super(LCGTarFile, self).__init__(name, project)

    def getDistFileName(self):
        # LCG tars are in a subdirectory called "source"
        # TODO: take this from Project instead
        return os.path.join("source", self.name)

class HTMLFile(DistFile):
    """ HTML File """
    def __init__(self, name, project):
        super(HTMLFile, self).__init__(name, project)

    def getDistFileName(self):
        # HTML files are in a subdirectory called "shtml"
        # TODO: take this from Project instead
        return os.path.join("html", self.name)

#
# Main class representing a project as released
###############################################################################
class ProjectManager(object):
    """ Class representing a project and all the artefacts associated """

    @classmethod
    def loadFromDisk(cls, name, version):
        """ Constructor for the Archiver based on the information found on the
         file system. """
        # Creating the instance
        o = cls(name, version)
        p = o.project

        o.distfiles = [ f for f in os.listdir(o.distdir) if fileMatches(p.tarBallName(version), f)]

        # Looking up the html files
        # They all start by PROJECT_PROJECT_VERSION == p.tarballName
        o.htmldir = p.HTMLDir()
        o.htmlfiles = [ f for f in os.listdir(o.htmldir) if fileMatches(p.tarBallName(version), f)]

        # Extract the list of configs from the list of tarballs
        # We take this as the definite list...
        o.releasedConfigs =  parseConfigFromFileList(o.tarPrefix, \
                            [ t for t in o.distfiles if t.endswith(ARCHIVE_SUFFIX)])

        # Now checking the LCG Releases...
        o.lcgdistdir = p.LCGTarBallDir()
        if p._lcgtarballname:
            o.lcgdistfiles = [ f for f in os.listdir(o.lcgdistdir) if fileMatches(p.LCGTarBallName(version), f)]
        else:
            o.lcgdistfiles = []

        o.lcgreleasedConfigs =  parseConfigFromFileList(o.tarPrefix, \
                            [ t for t in o.lcgdistfiles if t.endswith(ARCHIVE_SUFFIX)])

        # Return the constructed instance
        return o

    def __init__(self, name, version):
        """ Main constructor for the class that initializes the configuration
        with the default value for the directories, project names """
        # Loading the project from configuration
        p = Project.getProject(name, svn_fallback=True, raise_exception=False)
        self.project = p

        if p == None:
            raise Exception("Could not locate project %s %s " % (name, version))

        # using the name with fixed case
        self.projectName = p.Name()
        self.version = version
        self.tarPrefix = p.tarBallName(version)

        # Full path the project in the release area
        # e.g. LHCBRELEASES/GANGA/GANGA_v507r10
        self.releaseAreaDir = os.path.join(p.ReleaseArea(), p.releasePrefix(version))
        # Name of the parent directory
        # e.g. $LHCBRELEASES/GANGA
        self.releaseAreaParent = os.path.join(p.ReleaseArea(), p.NAME())
        # Name of the directory inside the sepcific per project volume
        # e.g GANGA_v507r10
        self.releaseAreaProjectName = "_".join([p.NAME(),version])

        # Location for the dist directory
        self.distdir = os.path.join(p.DistLocation(), p.DistPrefix())

        # HTML dir
        self.htmldir = p.HTMLDir()

        # By default we cannot know what the expected configs are
        self.releasedConfigs = []

        # HTML dependencies are loaded on demand
        self.htmlDepsLoaded = False

    #
    # Util method to load dependencies
    ###########################################################################
    def loadHTMLDeps(self):
        """ Parse the HTML dependency files """
        if self.htmlDepsLoaded:
            # Already done
            return

        self.htmlDeps = []
        for h in self.htmlfiles:
            if not h.endswith(".html"):
                continue
            path = os.path.join(self.htmldir, h)
            pf = HTMLDependencyManager.ProjectFile.loadFromFile(self.distdir, path)
            self.htmlDeps.append(pf)
        self.htmlDepsLoaded = True


    #
    # Util method to deal with the distribution area
    ###########################################################################
    def getDistLocation(self):
        """ Returns the location for the dist files (a.k.a LHCBTAR) """
        return self.project.DistLocation()

    def getDistFiles(self):
        """ returns all files for the project, with URLs relative
        to the DIST directory """
        # Building appropriate file objects for each type of file
        ret = map(lambda s: TarFile(s, self.project), self.distfiles)
        ret += map(lambda s: LCGTarFile(s, self.project), self.lcgdistfiles)
        ret += map(lambda s:  HTMLFile(s, self.project), self.htmlfiles)
        # List should now be complete...
        return ret

    #
    # Util method to deal with the release area
    ###########################################################################
    def getReleaseAreaArchiveName(self):
        """ Name of the tarball for the release area archive """
        uname = self.projectName.upper()
        return "%s_%s%s" % (uname, self.version, ARCHIVE_SUFFIX)

    def getReleaseAreaArchiveNameCASTOR(self):
        """ Builds the name of the CASTOR archive of the release area """
        uname = self.projectName.upper()
        fname = "%s_%s%s" % (uname, self.version, ARCHIVE_SUFFIX)
        return CASTOR_DIR + fname

    def getReleaseAreaArchiveNameDIRAC(self):
        """ Builds the LFN for the archive of the release area """
        uname = self.projectName.upper()
        fname = "%s_%s%s" % (uname, self.version, ARCHIVE_SUFFIX)
        return LFN_RELEASE_AREA + fname

    #
    # Common utils
    ###########################################################################
    def __str__(self):
        """ Pretty printer for the object """
        s = "Project: %s %s\n" % (self.projectName, self.version)
        s+= "Configs\n"
        for f in self.releasedConfigs:
            s+= "    %s\n" % f
        s+= "Release area: %s\n" % self.releaseAreaDir
        s+= "===============================================\n"
        s+= "HTML Files in: %s\n" % self.htmldir
        for f in self.htmlfiles:
            s+= "    %60s\n" % f
        s+= "===============================================\n"
        s+= "DIST Files in %s:\n" % self.distdir
        for f in self.distfiles:
            s+= "    %30s\n" % f
        s+= "===============================================\n"
        s+= "LCGDIST Files in %s:\n" % self.lcgdistdir
        for f in self.lcgdistfiles:
            s+= "    %30s\n" % f
        if self.htmlDepsLoaded:
            s+= "===============================================\n"
            s+= "HTML Dependencies:\n"
            for pf in self.htmlDeps:
                s+= str(pf)
        return s


    def report(self):
        """ Like __str__ but with file sizes """
        s = "Project: %s %s\n" % (self.projectName, self.version)
        s+= "Configs\n"
        s+= "===============================================\n"
        for f in sorted(self.releasedConfigs):
            s+= "    %s\n" % f
        s+= "\nRelease area: %s\n" % self.releaseAreaDir
        s+= "\n===============================================\n"
        disp =  "{0:<65} {1:>10}\n"
        s+= "HTML Files in: %s\n" % self.htmldir

        for f in sorted(self.htmlfiles):
            s+= disp.format(f, os.path.getsize(os.path.join(self.htmldir, f)))
        s+= "\n===============================================\n"
        s+= "DIST Files in %s:\n" % self.distdir
        for f in self.distfiles:
            s+= disp.format(f, os.path.getsize(os.path.join(self.distdir, f)))
        s+= "\n===============================================\n"
        s+= "LCGDIST Files in %s:\n" % self.lcgdistdir
        for f in self.lcgdistfiles:
            s+= disp.format(f, os.path.getsize(os.path.join(self.lcgdistdir, f)))
        if self.htmlDepsLoaded:
            s+= "\n===============================================\n"
            s+= "HTML Dependencies:\n"
            for pf in self.htmlDeps:
                s+= str(pf)
        return s

if __name__ == '__main__':
    pass