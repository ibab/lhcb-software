#!/usr/bin/env python
"""
"""
__author__ = "Marco Clemencic"

import os, re
import shutil
from datetime import datetime
from subprocess import Popen, PIPE
from stat import ST_SIZE
from LbUtils.afs.directory import isAFSDir, isMountPoint

import logging
_log = logging.getLogger(__name__)

#--- Configuration information utils

# Prepare a dictionary mapping the project name in upper case to the actual case
from LbConfiguration.Project import project_names
project_names = dict([(p.upper(), p) for p in project_names])

def projectURL(project, version = None):
    """
    Return the URL to the web page of the project.
    """
    result = None
    project = project.lower()
    if project == "lcgcmt":
        result = "http://lcgsoft.cern.ch"
        if version:
            result += "/index.py?page=cfg_overview&cfg=" + version
    else:
        if project == "gaudi":
            result = "http://cern.ch/gaudi"
        else:
            result = "http://cern.ch/LHCb-release-area/DOC/%s" % project
        if version:
            result += "/releases/" + version
    return result

#--- Filesystem utils
def _diskUsage(root):
    """
    Compute the size of a subtree.

    @param root: directory to measure
    @return: the size of the files in bytes
    """
    _log.debug("Computing the size of %s", root)
    size = 0L
    for dirpath, _, files in os.walk(root):
        for f in files:
            size += os.stat(os.path.join(dirpath, f))[ST_SIZE]
    return size


def _which(cmd, path = None):
    """
    Locate a file in the path.
    """
    if path is None:
        path = os.environ["PATH"]
    if type(path) is str:
        path = path.split(os.path.pathsep)
    for p in path:
        if cmd in os.listdir(p):
            return p
    return None

_has_AFS = bool(_which("afs_admin"))

def _makedocdir(path):
    """
    Create the directory path.
    If the parent directory is an AFS volume, a volume is created.

    @return: True if the volume was created, False if it is only a normal directory
    """
    parent = os.path.dirname(path)
    if _has_AFS and isAFSDir(parent) and isMountPoint(parent):
        # create the volume
        from LbUtils.afs.volume import createVolume
        volume_name = "q.lhcb.%s" % os.path.basename(path)
        createVolume(path, volume_name, quota = 1000000, group = "z5")
        return True
    else:
        # use the standard system calls
        os.makedirs(path)
        return False

#--- Doxygen class
class DoxyFileCfg(object):
    """
    Dictionary-like class to simplify the generation of doxygen configuration
    files.

    This class implements the basic dictionary interface with a record of the
    order in which the elements where entered.

    The conversion to string generates the content for the doxyfile.cfg file.
    """
    def __init__(self, data = []):
        """
        Initialized the object (by default empty).

        @param data: an optional association list (list of key/value pairs)
        """
        self._keys = []
        self._data = {}
        for k, v in data:
            self[k] = v
    def __getitem__(self, key):
        """
        Dictionary interface.
        """
        return self._data[key]
    def __setitem__(self, key, value):
        """
        Dictionary interface.
        """
        if key not in self._data:
            self._keys.append(key)
        self._data[key] = value
    def __delitem__(self, key):
        """
        Dictionary interface.
        """
        del self._data[key]
        del self._keys[self._keys.index(key)]
    def keys(self):
        """
        Dictionary interface.
        """
        return self._keys
    def __iter__(self):
        """
        Dictionary interface.
        """
        return self._keys.__iter__()
    def _convertValue(self, value):
        """
        Convert a value to the corresponding doxygen string and return it.
        E.g. bool -> YES/NO, list -> space separated lists
        """
        t = type(value)
        if t is bool:
            if value: value = "YES"
            else: value = "NO"
        elif t is str:
            if " " in value:
                value = '"%s"' % value
        elif t in (list, tuple):
            value = " ".join(map(self._convertValue, value))
        elif value is None:
            value = ""
        else:
            value = str(value)
        return value
    def __repr__(self):
        """
        Representation of the object instance.
        """
        return "DoxyFileCfg(%r)" % [ (k, self[k]) for k in self ]
    def __str__(self):
        """
        Produce the doxyfile.cfg content.
        """
        return "".join(["%s = %s\n" % (k, self._convertValue(self[k]))
                        for k in self])

#--- CMT Interfaces
def _projRoot(proj, version):
    """
    Return the root directory of a project/version in the LHCBPROJECTPATH.
    """
    projDir = os.path.join(proj.upper(),
                           "%s_%s" % (proj.upper(), version))
    for reldir in os.environ["LHCBPROJECTPATH"].split(os.path.pathsep):
        root = os.path.join(reldir, projDir)
        if os.path.isdir(root):
            return root
    return None

def _cmt_show_projects(root):
    """
    Run "cmt show projects" in the project rooted in the specified directory
    and return the output.
    """
    cmt = os.path.join(root, "cmt")
    _log.debug("'cmt show projects' in %s", cmt)
    return Popen(["cmt","show","projects"], cwd = cmt, stdout = PIPE).communicate()[0]

def _getProjDeps(project, version):
    """
    Get all the dependencies of a project.

    @param project: name of the project
    @param version: version of the project

    @return: list of pairs (project, version)
    """
    pr = _projRoot(project, version)
    if not pr:
        raise ValueError("Project %s %s does not exist" % (project, version))
    return set([ (l[0], l[1].split("_")[1])
                 for l in [ l.strip().split()
                            for l in
                            _cmt_show_projects(pr).splitlines()[1:] ]
                 if l and l[1] != "(in" ])

from LbUtils.CMT.Project import Project
def _getProjDepsX(project, version):
    deps = set()
    to_process = [Project(_projRoot(project, version))]
    while to_process:
        proj = to_process.pop()
        for b in proj.base(cmtprojectpath = os.environ["LHCBPROJECTPATH"]):
            v = b.version()
            if v:
                v = v.split("_")[1]
                deps.add((b.name(), v))
                to_process.append(b)
    return deps

#--- Documentation class
class Doc(object):
    """
    Class to describe and manipulate documentation directory.

    A couple of special files can be used to tune the behavior of the documentation
    directory:
     '.locked' : prevent the build of the documentation (used internally to mark if a
                 documentation is being built)
     '==REBUILD==' : force a re-build of the documentation
    """
    _nameRegexp = re.compile("^DOC_[0-9]{6}$")
    _namePattern = "DOC_%06d"
    _docSubdir = "doxygen"
    _docCollDir = "docs"
    _docLockFile = ".locked"
    _docRebuildFlagFile = "==REBUILD=="

    @classmethod
    def _root(cls, root = None):
        """
        Return the root directory or the default.
        """
        return root or os.environ["DOCROOT"]

    def __init__(self, name = None, root = None):
        """
        Initialize the object.
        If the directory doesn't exist it is created.

        @param name: name of the documentation directory, if not specified, a new one is created
        @param root: base directory of the documentations, by default taken from the environment variable DOCROOT
        """
        # base directory of the documentation directories
        self.root = self._root(root)

        # name of the requested doc object
        if name:
            self.name = name
        else: # if the name is not specified, create a new one
            self._log = _log # temporarily use the main logger
            self._log.debug("New documentation dir requested")
            # get all doc names
            docs = self._allDocNames()
            if not docs:
                n = 0
            else:
                # convert the number at the end of the last doc name
                # to a number and increment it by 1
                id = docs[-1].split("_")[-1].lstrip("0")
                if id:
                    n = int(id) + 1
                else:
                    # this happens when the latest is "DOC_000000"
                    n = 1
            self.name = self._namePattern % n
            self._log.debug("Automatic directory name %s", self.name)

        # get our logger instance
        self._log = logging.getLogger(self.name)

        # full path to the directory of the object
        self.path = os.path.join(self.root, self.name)

        # full path to the doxygen destination directory
        self.output = os.path.join(self.path, self._docSubdir)

        # Broken links in the directory
        self.broken = []

        # projects in the directory
        self.projects = {}
        if not os.path.isdir(self.path):
            self._log.info("Creating directory %s", self.path)
            self.isAfsVolume = _makedocdir(self.path)
        else:
            self._log.debug("Listing projects in %s", self.path)
            for l in os.listdir(self.path):
                if os.path.islink(os.path.join(self.path, l)):
                    p, v = l.split("_")
                    self.projects[p.upper()] = v # ensure that we use upper case names
                    # Update broken links count
                    if not os.path.exists(os.path.join(self.path, l)):
                        self.broken.append(l)
            self._log.debug("Found %d projects: %s", len(self.projects),
                            " ,".join(map(str, self.projects.items())))
            self.isAfsVolume = _has_AFS and isAFSDir(self.path) and isMountPoint(self.path)

        # flag to tell if the DOC dir is locked or not
        self._lockFile = os.path.join(self.path, self._docLockFile)
        self.locked = os.path.exists(self._lockFile)

        self._rebuildFlagFile = os.path.join(self.path, self._docRebuildFlagFile)
        # flag saying if the directory has already been built or not
        self.toBeBuilt = not os.path.isdir(self.output)
        if os.path.exists(self._rebuildFlagFile):
            self._log.warning("Re-build requested")
            self.toBeBuilt = True

    def _allDocNames(self):
        """
        Get all doc names in the same top-level directory as this one
        (including it).
        """
        self._log.debug("Looking for documentation directories in %s", self.root)
        docs = filter(lambda d: os.path.isdir(os.path.join(self.root, d)),
                      filter(self._nameRegexp.match,
                             os.listdir(self.root)))
        docs.sort()
        return docs

    def getVersion(self, project):
        """
        Return the version of the project contained, or None if the project is
        not present.
        """
        project = project.upper()
        return self.projects.get(project, None)

    def canHost(self, project, version, deps):
        """
        Helper function to tell if a project with its dependencies can be
        included in this directory.
        """
        # Note: if the directory is locked, we can assume we could still host the
        #       file, but it will not be actually added

        # The project must not be included
        if self.getVersion(project) is not None:
            return False
        # each dependency must be already present with the right version or
        # not present
        for p, v in deps:
            if not self.getVersion(p) in [v, None]:
                return False
        return True
    def add(self, project, version):
        """
        Add a project to the list of contained ones.

        Raise and exception if the project is already there with a different
        version or it doesn't exist.
        """
        # do not add projects if the directory is locked
        if self.locked:
            self._log.warning("Trying to add project %s %s to a locked directory", project, version)
            return
        project = project.upper() # ensure upper case
        # check if the project is already there
        if project in self.projects:
            ## @todo: check if the version is the same as the contained one
            raise ValueError("Project %s already in %s" % (project, self.name))
        else:
            self._log.debug("Adding project %s %s", project, version)
            # make link to the root directory of the project in the doc dir
            root = _projRoot(project, version)
            if not root:
                raise ValueError("Cannot find project %s %s" % (project, version))
            self.projects[project] = version
            os.symlink(root, os.path.join(self.path,
                                          "%s_%s" % (project, version)))
            #self._log.debug("Mark the directory as to be built")
            self.toBeBuilt = True

    def _generateDoxygenMainPage(self):
        """
        Generate the main page file for the documentation directory and return
        it as a string.
        """
        page = "/** \\mainpage LHCb Software Documentation\n" + \
            "Documentation for the projects:\n<ul>\n"
        projects = self.projects.keys()
        projects.sort()

        item = '<a href="%(homeurl)s">%(project)s</a> <a href="%(versurl)s">%(version)s</a>'
        for p in projects:
            if p == "LCGCMT":
                continue
            page += "<li>%s</li>\n" % (
                    item % {'project': project_names.get(p.upper(), p), # get actual case of the project
                            'homeurl': projectURL(p), # url to the project web page
                            'version': self.getVersion(p), # version of the project
                            'versurl': projectURL(p, self.getVersion(p)), # url to the version of the project
                            }
                    )
        page += "</ul>\n"
        if "LCGCMT" in self.projects:
            page += "Based on %s.\n" % (
                    item % {'project': "LCGCMT",
                            'homeurl': projectURL("LCGCMT"),
                            'version': self.getVersion("LCGCMT"),
                            'versurl': projectURL("LCGCMT", self.getVersion("LCGCMT"))
                            }
                    )
        page += '\n\\image html dependencies.png "Graph of the dependencies between projects"\n'
        page += "*/\n"
        return page

    def _generateDoxyFile(self):
        """
        Generate the doxygen configuration file for the current doc directory.
        """
        if self.locked:
            self._log.warning("Cannot generate Doxygen configuration in a locked directory")
            return
        self._log.info("Generate DoxyFile.cfg")
        doxycfg = DoxyFileCfg([("PROJECT_NAME", "LHCb Software"),
                               ('OUTPUT_DIRECTORY', self.output),
                               ('GENERATE_TAGFILE', 'LHCbSoft.tag'),
                               ('CREATE_SUBDIRS', True),
                               ])
        doxycfg['INPUT'] = [ "%s_%s" % item
                             for item in self.projects.items()
                             if item[0] not in ["LCGCMT"] ] # avoid some projects

        doxycfg['GENERATE_HTML']       = True
        doxycfg['GENERATE_TODOLIST']   = True
        doxycfg['GENERATE_LATEX']      = False
        doxycfg['GENERATE_MAN']        = False
        doxycfg['GENERATE_RTF']        = False
        doxycfg['CASE_SENSE_NAMES']    = False
        doxycfg['RECURSIVE']           = True
        doxycfg['EXTRACT_ALL']         = True
        doxycfg['SOURCE_BROWSER']      = True
        doxycfg['INLINE_SOURCES']      = True
        doxycfg['HTML_ALIGN_MEMBERS']  = True
        doxycfg['QUIET']               = False
        doxycfg['WARNINGS']            = True
        doxycfg['WARN_LOGFILE']        = "DoxyWarnings.log"
        doxycfg['ALPHABETICAL_INDEX']  = True
        doxycfg['COLS_IN_ALPHA_INDEX'] = 3
        doxycfg['JAVADOC_AUTOBRIEF']   = True
        doxycfg['CLASS_DIAGRAMS']      = True
        doxycfg['SHOW_DIRECTORIES']    = True
        doxycfg['SEARCHENGINE']        = True
        doxycfg['BUILTIN_STL_SUPPORT'] = True

        # EXCLUDE_PATTERNS
        excludes = [
            "*/dict/*", "*/InstallArea/*",
            # Exclude tests
            "*/test/*",
            "*/Test/*",
            "*/tests/*",
            "*/Tests/*",
            "*/examples/*",
            "*/GaudiExamples/*",
            # Exclude binaries
            "*/slc3_*/*",
            "*/slc4_*/*",
            "*/*-slc5-*/*",
            "*/win32_*/*",
            "*/osx105_*/*",
            # Exclude version control metadata
            "*/CVS/*",
            "*/.svn/*",
            # Exclude some generated files
            "*/html/*",
            #"*/genConf/*",
            # Exclude problematic files
            "*/doc/MainPage.h",
            "*/Panoramix/doc/doxygen/*",
            "*/Panoramix/doc/h/*",
            ]
        doxycfg["EXCLUDE_PATTERNS"] = excludes

        files = [ "*.cpp", "*.h", "*.icpp", "*.py" ]
        #files = [ "requirements" ]
        for p in doxycfg["INPUT"]:
            for d in [ d
                       for d in os.listdir(os.path.join(self.path, p))
                       if (d.endswith("Sys") and d != "GaudiSys") or d.endswith("Release")]:
                files.append("*%s*requirements" % d)
        # FILE_PATTERNS   += *LHCbSys*requirements

        # Configurations details to generate the main page
        doxycfg['INPUT'].append("conf/MainPage.doxygen")
        doxycfg['IMAGE_PATH'] = ["conf"]

        doxycfg["FILE_PATTERNS"] = files

        extra_packages = [ "times", "amsmath" ]
        doxycfg["EXTRA_PACKAGES"] = extra_packages

        # append the commands to use dot tool
        doxycfg["HAVE_DOT"]              = True
        doxycfg["DOT_PATH"] = None
        doxycfg["COLLABORATION_GRAPH"]   = True
        doxycfg["CLASS_GRAPH"]           = True
        doxycfg["GRAPHICAL_HIERARCHY"]   = True
        doxycfg["INCLUDE_GRAPH"]         = True
        doxycfg["INCLUDED_BY_GRAPH"]     = True

        # append the commands to document also private and static members
        doxycfg["EXTRACT_PRIVATE"]       = True
        doxycfg["EXTRACT_STATIC"]        = True
        doxycfg["EXTRACT_LOCAL_CLASSES"] = True

        # get externals versions (used for the tag files)
        gaudipath = os.path.join(self.path, "GAUDI_%s" % self.getVersion("GAUDI"), "GaudiRelease", "cmt")
        if os.path.isdir(gaudipath): # we use Gaudi as entry point, if it is not there, no external versions
            gaudipath = os.path.join(self.path, "GAUDI_%s" % self.getVersion("GAUDI"), "GaudiRelease", "cmt")
            config_versions = Popen(["cmt", "show", "macros", "_config_version"],
                                    cwd = gaudipath, stdout = PIPE).communicate()[0]
            # Translate the cmt output
            #  LCG_config_version='55b'
            #  COOL_config_version='COOL_2_6_0'
            #  CORAL_config_version='CORAL_2_1_0'
            #  POOL_config_version='POOL_2_8_1'
            # into a dictionary
            #  { 'LCG': '55b',
            #    'COOL': 'COOL_2_6_0',
            #    'CORAL': 'CORAL_2_1_0',
            #    'POOL': 'POOL_2_8_1' }
            config_versions = dict([ (k.replace("_config_version", ""), v.strip("'"))
                                     for k, v in [ l.split("=") for l in config_versions.splitlines() ] ])
        else:
            config_versions = {}

        # add tags
        doxycfg["TAGFILES"] = []
        # libstdc++
        tagline = os.path.join(os.environ["LCG_release_area"],
                               os.path.pardir, os.path.pardir,
                               "external", "gcc", "4.3.2",
                               "doc", "libstdc++.tag")
        tagline = os.path.normpath(tagline) + "=http://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3"
        doxycfg["TAGFILES"].append(tagline)
        # add tag entries for the Application Area projects
        for aa_project in ["ROOT", "CORAL", "COOL", "POOL"]:
            if aa_project in config_versions:
                tagline = os.path.join(os.environ["LCG_release_area"], aa_project,
                                       config_versions[aa_project], "doc", "doxygen",
                                       "html", aa_project + "_reference_tags.xml")
                tagline += "=http://lcgapp.cern.ch/doxygen/%s/%s/doxygen/html" % (aa_project, config_versions[aa_project])
                doxycfg["TAGFILES"].append(tagline)
        #doxycfg["TAGFILES"] = []

        # write the output file
        confdir = os.path.join(self.path, "conf")
        if not os.path.isdir(confdir):
            self._log.debug("Creating directory %s", confdir)
            os.makedirs(confdir)
        open(os.path.join(confdir, "DoxyFile.cfg"), "w").write(str(doxycfg))
        open(os.path.join(confdir, "MainPage.doxygen"), "w").write(self._generateDoxygenMainPage())
        # generate the dependency graph
        self._genDepGraph(confdir)

    def _projectDeps(self, project, recursive = False):
        """
        Return the list of projects a project depend on (by parsing the project.cmt).
        If the flag 'recursive' is set to True, return the complete (recursive) set
        of dependencies.
        """
        uses = set()
        project = project.upper()
        if project in self.projects:
            proj_cmt = os.path.join(self.path,
                                    "%s_%s" % (project, self.getVersion(project)),
                                    "cmt", "project.cmt")
            for l in open(proj_cmt):
                l = l.strip()
                if l and l[0] != "#": # this is not a comment
                    l = l.split() # tokenize
                    if l[0] == "use" and len(l) > 1: # we care only about 'use' statements
                        uses.add(l[1])
            if recursive:
                for p in list(uses):
                    uses.update(self._projectDeps(p, True))
        return uses

    def _genDepGraph(self, destination = ""):
        """
        Generate the project dependency graph.

        @param destination: optional destination path of the generated files,
            relative to the documentation directory or absolute.
        """
        self._log.info("Generating project dependency graph in '%s'", destination)
        gvdata = []
        for project in self.projects:
            project = project_names.get(project.upper(), project)
            deps = [ project_names.get(p.upper(), p)
                     for p in self._projectDeps(project) ]
            deps.sort()
            gvdata.append("%s->{%s};" % (project, ";".join(deps)))
        if gvdata:
            gvdata = "digraph dependencies {\nnode [fontsize=10];\n%s\n}\n" % ("\n".join(gvdata))
        cmd = ["dot"]
        for format in ["png", "svgz", "eps", "fig"]:
            if type(format) is tuple:
                format, extension = format
            else:
                extension = format
            cmd.append("-T%s" % format)
            cmd.append("-o%s" %
                       os.path.join(self.path, destination, "dependencies.%s" % extension))
        gvfile = os.path.join(self.path, destination, "dependencies.gv")
        cmd.append(gvfile)
        open(gvfile, "w").write(gvdata)
        Popen(cmd).wait()

    def build(self):
        """
        Build the actual doxygen documentation.
        """
        if self.locked:
            self._log.warning("Cannot build in a locked directory")
            return
        # create lockfile
        self._log.info("Creating lock file '%s'", self._lockFile)
        open(self._lockFile, "w").write("%s - %s\n" % (os.getpid(), datetime.now()))
        try:
            self._generateDoxyFile()
            self._log.info("Running doxygen")
            # @todo: build the documentation in a temporary directory
            # modify the doxygen file to use a temporary directory
            import getpass, tempfile
            username = getpass.getuser()
            tempdirs = filter(os.path.isdir,
                              [os.path.join(os.path.sep, "build", "tmp"),
                               os.path.join(os.path.sep, "build", username, "tmp"),
                               os.path.join(os.path.sep, "build", username)])
            if tempdirs:
                tempdir = tempfile.mkdtemp("doxygen", dir = tempdirs[0])
            else:
                tempdir = tempfile.mkdtemp("doxygen")
            # use a temporary configuration file to generate the output in a temporary directory
            shutil.copyfile(os.path.join(self.path, "conf", "DoxyFile.cfg"),
                            os.path.join(self.path, "conf", "DoxyFileTmp.cfg"))
            open(os.path.join(self.path, "conf", "DoxyFileTmp.cfg"), "a").write("OUTPUT_DIRECTORY = %s\n" % tempdir)
            proc = Popen(["doxygen", os.path.join("conf", "DoxyFileTmp.cfg")],
                         cwd = self.path, stdin = PIPE)
            proc.stdin.write("r\n") # make latex enter \nonstopmode on the first error
            retcode = proc.wait()
            if retcode != 0:
                raise RuntimeError("Doxygen failed with error %d in %s" % (retcode, tempdir))
            if os.path.exists(self.output + ".bk"):
                # Remove old backups before copying the new documentation
                shutil.rmtree(self.output + ".bk")
            if self.isAfsVolume:
                usage = _diskUsage(tempdir) / 1024
                output = Popen(["fs", "lq", self.path], stdout = PIPE).communicate()[0].splitlines()[-1].split()
                quota = int(output[1])
                used = int(output[2])
                reqsize = int(1.1 * (used + usage))
                if quota < reqsize:
                    self._log.info("Increasing AFS volume size to %d", reqsize)
                    output = Popen(["afs_admin", "sq", self.path, str(reqsize)], stdout = PIPE).wait()
            # copy the documentation from the temporary directory to the final place with a temporary name
            self._log.info("Copy generated files from temporary directory")
            shutil.copytree(tempdir, self.output + ".new")
            if self.isAfsVolume:
                # Give read access to everybody
                self._log.debug("Give read access (recursively) to %s", self.path)
                for dirpath, _, _ in os.walk(self.path):
                    Popen(["fs", "setacl", "-dir", dirpath, "-acl", "system:anyuser", "rl"]).wait()
            # Swap the old and the new documentation (avoid that the users see an incomplete doc)
            # @todo: it should be done for the tag file too
            # Move away the old documentation
            if os.path.isdir(self.output):
                old = self.output + ".bk"
                self._log.warning("Old documentation moved to %s", old)
                os.rename(self.output, old)
            # rename the new documentation
            os.rename(self.output + ".new", self.output)
            shutil.rmtree(tempdir)
            os.remove(os.path.join(self.path, "conf", "DoxyFileTmp.cfg"))
            self._updateCommonLinks()
            # Mark as built
            self.toBeBuilt = False
            # Since we have built the doc, remove the "rebuild flag" if present
            if os.path.exists(self._rebuildFlagFile):
                os.remove(self._rebuildFlagFile)
            self._log.debug("Documentation ready")
        finally:
            # clean up the lock
            os.remove(self._lockFile)

    def _updateCommonLinks(self):
        """
        Update the links in the common directory.
        """
        # update links in the common directory
        doclinkdir = os.path.join(self.root, self._docCollDir)
        if not os.path.isdir(doclinkdir):
            self._log.debug("Creating directory %s", doclinkdir)
            os.makedirs(doclinkdir)
        for project, version in self.projects.items():
            doclink = os.path.join(doclinkdir, "%s_%s" % (project, version))
            if os.path.exists(doclink):
                old = os.path.basename(os.readlink(doclink))
                if old < self.name:
                    self._log.info("Moving link %s from %s to %s", doclink, old, self.name)
                    os.remove(doclink)
                    os.symlink(os.path.join("..", self.name), doclink)
            else:
                self._log.info("Creating link %s to %s", doclink, self.name)
                os.symlink(os.path.join("..", self.name), doclink)

    def __len__(self):
        """
        Returns the number of projects hosted.
        """
        return len(self.projects)

    def __cmp__(self, other):
        """
        Comparison operator.
        """
        return cmp((len(self), self.name), (len(other), other.name))


def allDocs(root = None):
    """
    Get all the doc objects for a root directory.
    """
    root = Doc._root(root)
    docs = []
    for d in os.listdir(root):
        if Doc._nameRegexp.match(d):
            docs.append(Doc(d, root))
    return docs

#--- Application logic
def makeDocs(projects, root = None, no_build = False):
    if "PWD" in os.environ:
        # This is needed because PWD is not updated by Popen and cmt gets confused
        # if it is set
        del os.environ["PWD"]

    docs = allDocs(root)

    # keep only projects that are not already known
    def projectNotKnown(pv):
        project, version = pv
        for d in docs:
            if d.getVersion(project) == version:
                _log.info("Project %s %s is already in %s", project, version, d.name)
                return False
        return True
    projects = set(filter(projectNotKnown, projects))

    # list to keep track of the projects that have been added as
    # dependencies of others
    projects_added = set()
    for project, version in projects:
        if (project, version) in projects_added:
            continue
        # Get all the dependencies of the project
        # @todo: projects that do not depend on LCGCMT (like LbScripts) should not join any existing doc
        deps = _getProjDeps(project, version)
        if not deps: # ignore projects without dependencies
            _log.info("Ignoring %s %s, no dependencies", project, version)
            projects_added.add((project, version))
            continue
        # get the candidates, i.e. the doc directories that can include the requested
        # project with its dependencies
        candidates = [ d for d in docs
                       if d.canHost(project, version, deps) ]
        if not candidates:
            # no candidates available, create a new doc dir
            _log.debug("Creating new doc dir")
            doc = Doc(root = root)
            docs.append(doc) # add the new doc to the list of known docs
        elif len(candidates) == 1:
            # only one candidate, use it
            doc = candidates[0]
            _log.debug("Using %s", doc.name)
        else:
            # more than one candidate
            _log.warning("More than one candidate doc dir for %s %s: %s",
                         project, version,
                         ", ".join([d.name for d in candidates]))
            # select the best candidate
            candidates.sort()
            doc = candidates[-1]
            _log.warning("Using %s", doc.name)
        # First add the dependencies (not yet there)
        for p, v in deps:
            if doc.getVersion(p) is None:
                doc.add(p, v)
        projects_added.update(deps)
        # Then add the project
        doc.add(project, version)
        projects_added.add((project, version))

    # Build all the documentations marked as to be built
    for doc in filter(lambda d: d.toBeBuilt, docs):
        if no_build:
            # if we should not run doxygen, at least generate the doxygen configuration
            doc._generateDoxyFile()
            # and create the fake sym-links
            doc._updateCommonLinks()
        else:
            doc.build()
    ## @todo: probably, this step should be done inside the build step
    # Update links pointing to the latest versions
    doclinkdir = os.path.join(Doc._root(root), Doc._docCollDir)
    for p in getLatestVersions([ x for x in os.listdir(doclinkdir)
                                 if not x.startswith("LCGCMT") and not x.endswith("_latest") ]):
        latest = p.split("_")[0].upper() + "_latest"
        latest = os.path.join(doclinkdir, latest)
        if not (os.path.islink(latest) and (os.readlink(latest) == p)):
            # the link doesn't exist or doesn't point to the most recent version
            if os.path.exists(latest) or os.path.islink(latest):
                # ensure that the link is not present before creating the new one
                os.remove(latest)
            _log.debug("Moving link %s to %s", os.path.basename(latest), p)
            os.symlink(p, latest)

    projects -= projects_added
    if projects:
        _log.warning("Projects not added: %s", list(projects))

def getLatestVersions(versions):
    """
    Given a list of project versions in the form of "<PROJECT>_<version>",
    return a list of the latest version for each project.
    """
    from LbConfiguration.Version import CoreVersion
    projects = {} # dictionary with a list of versions per project
    for p, v in [ (p.upper(),      # ensure we use the uppercase name
                   CoreVersion(v)) # Sortable class to wrap a version string
                  for p, v in [ pv.split("_") for pv in versions ] ]:
        try:
            projects[p].append(v)
        except:
            projects[p] = [v]
    # sort the versions of each project and get the last one
    for p in projects:
        projects[p].sort()
        projects[p] = projects[p].pop()
    return [ "%s_%s" % t for t in projects.items() ]

def findProjects(exclude = None):
    """
    Find all the available (project, version) pairs available in the release area.
    """
    releases = os.environ["LHCBRELEASES"]
    logging.debug("Looking for projects in '%s'", releases)
    if exclude is None: # default exclusion
        exclude = set([ "GANGA", "DIRAC", "LHCBGRID", "CURIE", "GEANT4" ])
    else:
        exclude = set([ p.upper() for p in exclude ])
    projects = []
    for project in os.listdir(releases):
        projdir = os.path.join(releases, project)
        if project not in exclude and os.path.isdir(projdir):
            version_re = re.compile(r"^%s_(v\d+r\d+(p\d+)?)$" % project)
            for versdir, version in [ (m.group(0), m.group(1))
                                      for m in map(version_re.match, os.listdir(projdir))
                                      if m ]:
                if os.path.exists(os.path.join(projdir, versdir, "cmt", "project.cmt")):
                    projects.append((project, version))
    projects.sort()
    logging.debug("Found %s", projects)
    return projects

def findBrokenDocs(root = None):
    """
    Find all broken doc directories.
    A doc directory is broken if it contains (those containing broken symlinks, i.e.
    """
    return [d for d in allDocs(root) if d.broken]

def findUnusedDocs(root = None):
    """
    Find all doc directories that are not referenced from the common directory.
    """
    doclinkdir = os.path.join(Doc._root(root), Doc._docCollDir)
    referenced = set()
    for path in [os.path.join(doclinkdir, p) for p in os.listdir(doclinkdir)]:
        if os.path.islink(path):
            referenced.add(os.readlink(path).split(os.path.sep)[-1])
    return [d for d in allDocs(root) if d.name not in referenced]

def cleanArchivedProjects(root = None):
    """
    Delete from the docs directories the broken links.
    """
    root = Doc._root(root)
    logging.warning("Cleaning archived project docs from %s", root)
    broken_docs = findBrokenDocs(root)
    archived_project = set()
    for d in broken_docs:
        for b in d.broken:
            archived_project.add(b)
            try:
                l = os.path.join(d.path, b)
                logging.info("Removing %s", l)
                os.remove(l)
            except IOError, x:
                logging.warning("IOError: %s", x) # ignore exceptions during removal
    for p in archived_project:
        try:
            l = os.path.join(root, Doc._docCollDir, p)
            logging.info("Removing %s", l)
            os.remove(l)
        except IOError, x:
            logging.warning("IOError: %s", x) # ignore exceptions during removal

def removeUnusedDocs(root = None):
    """
    Delete the doc directories that are not referenced by the common doc directory.
    """
    root = Doc._root(root)
    logging.warning("Removing unused doc directory from %s", root)
    for d in findUnusedDocs(root):
        if d.isAfsVolume:
            logging.warning("Removal of AFS volumes not yet implemented!")
        else:
            logging.warning("Removing %s", d.path)
            shutil.rmtree(d.path)

def main():
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] project version [project version ...]")
    parser.add_option("--verbose", action = "store_true",
                      help = "Print more messages")
    parser.add_option("--debug", action = "store_true",
                      help = "Print debugging messages")
    parser.add_option("--root", action = "store",
                      help = "Override the root directory of the documentation")
    parser.add_option("--no-build", action = "store_true",
                      help = "Do not run doxygen")
    parser.add_option("--clean-archived", action = "store_true",
                      help = "Remove all the links of archived projects")
    parser.add_option("--remove-unused", action = "store_true",
                      help = "Delete unused documentations")
    parser.add_option("-x", "--exclude", action = "append",
                      help = "Delete unused documentations")

    opts, args = parser.parse_args()

    log_level = logging.WARNING
    if opts.verbose:
        log_level = logging.INFO
    if opts.debug:
        log_level = logging.DEBUG
    logging.basicConfig(level = log_level)

    if not args:
        projects = findProjects(opts.exclude)
    elif (len(args)%2 == 0): # we accept only pairs
        projects = set()
        for i in range(0, len(args), 2):
            projects.add((args[i].upper(), args[i + 1]))
    else:
        parser.error("Wrong number of arguments")

    # Clean-up functions
    if opts.clean_archived:
        cleanArchivedProjects(opts.root)
    if opts.remove_unused:
        removeUnusedDocs(opts.root)
    # Main function
    makeDocs(projects, opts.root, opts.no_build)

if __name__ == '__main__':
    main()
