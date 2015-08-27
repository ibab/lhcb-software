#!/usr/bin/env python
"""
"""
import sys
__author__ = "Marco Clemencic"

import os, re
import shutil
from datetime import datetime, timedelta
from subprocess import Popen, PIPE, call
from stat import ST_SIZE
from LbUtils.afs.directory import isAFSDir, isMountPoint
from LbConfiguration.External import doxygen_version as doxygen_default_version

import __builtin__
if "set" not in dir(__builtin__):
    # pylint: disable-msg=W0622
    from sets import Set as set


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
        fp = os.path.join(p, cmd)
        if os.path.exists(fp):
            return fp
    return None

_has_AFS = bool(_which("afs_admin"))
_has_kinit = bool(_which('kinit'))

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

def doxyTagsToDBM(tag, output, overwrite = False, python = False):
    """
    Extract the file paths of the reference documentation page of the classes
    from a Doxygen tag file and stores them in a DBM file.

    @param tag: path to the tag file
    @param output: path to the DBM file.
    @param overwrite: if set to True, force the creation of a new DB (default is append)
    @param python: if set to True, modify the class names to use '.' instead of '::'
    """
    if "LHCBDOC_TESTING" in os.environ:
        return
    import anydbm, xml.parsers.expat
    _log = logging.getLogger("doxyTagsToDBM")
    # Open (and create) the database
    db = anydbm.open(output, overwrite and "n" or "c")
    # Prepare the XML parser
    p = xml.parsers.expat.ParserCreate()
    # We need to analyse structures like:
    # <compound kind="class">
    #   <name>MyClass</name>
    #   <filename>d1/d70/class_my_class.html</filename>
    #   <member kind="variable" protection="private">
    #     <type>string</type>
    #     <name>m_data</name>
    #     <anchorfile>...</anchorfile>
    #     <anchor>...</anchor>
    #     <arglist></arglist>
    #   </member>
    # </compound>
    #
    _path = [] # path in the XML tree
    _data = {} # cache for the data to add
    # XML event handlers
    def start_element(name, attrs):
        # keep track of the path of elements (with the optional attibute 'kind')
        _path.append( (name, attrs.get("kind")) )
        if name == "compound":
            _data = {}
    def char_data(data):
        if len(_path) >= 2:
            # we must be in a child of <compound kind="class">
            if _path[-2] == ("compound", "class"):
                current = _path[-1][0]
                # of type <name> or <filename>
                if current in [ "name", "filename" ]:
                    _data[current] = data #store the data in the cache
    def end_element(name):
        # keep track of the path
        elem = _path.pop()
        # if we are exiting from a <compound kind="class">
        # and we collected some data
        if elem == ("compound", "class") and _data:
            try: # store the data in the database
                name = str(_data["name"])
                if python:
                    # Use Python scope notation
                    name = name.replace('::', '.')
                db[name] = str(_data["filename"])
            except Exception, x:
                # safety net for inconsistent XML
                _log.warning(x)
    p.StartElementHandler = start_element
    p.EndElementHandler = end_element
    p.CharacterDataHandler = char_data
    p.ParseFile(open(tag))
    db.close()

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

def findPackages(projdir):
    '''
    Return a generator for the package names from the specified project directory.
    '''
    for root, dirs, files in os.walk(projdir):
        if 'InstallArea' in dirs:
            dirs.remove('InstallArea')
        if root == projdir:
            continue # no need to check the top level
        if 'CMakeLists.txt' in files or 'cmt' in dirs:
            # this is a package
            dirs[:] = [] # no need to recurse further
            yield os.path.relpath(root, projdir)

def findFilesInSubdirs(path):
    '''
    Return a generator for all the files in a directory and its subdirs.
    The path to the files is relative to the top directory passed as argument.
    '''
    for root, _, files in os.walk(path):
        for filename in files:
            yield os.path.relpath(os.path.join(root, filename), path)

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
    _docBlacklistFile = "blacklist.txt"
    _docRebuildFlagFile = "==REBUILD=="
    _docLockTimeFmt = "%Y-%m-%dT%H:%M:%S.%f" # same as datetime.isoformat()

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
                id_n = docs[-1].split("_")[-1].lstrip("0")
                if id_n:
                    n = int(id_n) + 1
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
            for l in filter(lambda s: not s.startswith('.'), os.listdir(self.path)):
                if os.path.islink(os.path.join(self.path, l)):
                    p, v = l.split("_")
                    self.projects[p.upper()] = v # ensure that we use upper case names
                    # Update broken links count
                    if not os.path.exists(os.path.join(self.path, l)):
                        self.broken.append(l)
            self._log.debug("Found %d projects: %s", len(self.projects),
                            ", ".join(map(str, sorted(self.projects.items()))))
            self.isAfsVolume = _has_AFS and isAFSDir(self.path) and isMountPoint(self.path)

        # flag to tell if the DOC dir is locked or not
        self._lockFile = os.path.join(self.path, self._docLockFile)
        if os.path.exists(self._lockFile):
            pid, time = open(self._lockFile).read().strip().split(" - ", 1)
            self.locked = (int(pid), datetime.strptime(time,"%Y-%m-%dT%H:%M:%S.%f"))
        else:
            self.locked = None


        self._rebuildFlagFile = os.path.join(self.path, self._docRebuildFlagFile)
        # flag saying if the directory has already been built or not
        self.toBeBuilt = not os.path.isdir(self.output)
        if os.path.exists(self._rebuildFlagFile):
            self._log.warning("Re-build requested")
            self.toBeBuilt = True

        # get the list of projects that should not be used in this slot
        self._blackListFile = os.path.join(self.path, self._docBlacklistFile)
        try:
            self.blackList = set(x.upper() for x in
                                 open(self._blackListFile).read().split())
        except IOError:
            self.blackList = set()

        # check that the common links are present
        # a missing link means problems during the build
        for project, version in self.projects.items():
            project = project.lower()
            if not os.path.exists(self._doclink(project, version)):
                self._log.warning("Project %s %s in %s but not in common links dir",
                                  project, version, self.name)

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

    def _hasCpp(self):
        """
        Tell if we have to build the C++ documentation (i.e. if Gaudi is in the
        dependencies.
        """
        return "GAUDI" in self.projects

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

        to_add = set([p.upper() for p,_ in deps])
        to_add.add(project.upper())

        # Projects without dependencies (like LbScripts) cannot be added to any
        # existing documentation.
        if not deps:
            return False
        # The project must not be included
        if self.getVersion(project) is not None:
            return False
        # The project should not be blacklisted
        if self.blackList.intersection(to_add):
            return False
        # Other special case: LHCbDirac cannot go in the same doc dir as Gaudi.
        # If at least one the projects to be added (project+deps) is in the
        # conflicting list (first intersection), ensure that the one are not
        # (second intersection) among the others (difference).
        conflicts = set(["GAUDI", "LHCBDIRAC"])
        if conflicts.intersection(to_add):
            if conflicts.difference(to_add).intersection(self.projects):
                return False
        # Each dependency must be already present with the right version or
        # not present and there must be projects in the dependencies that are
        # already in the doc
        has_common = False
        for p, v in deps:
            hosted = self.getVersion(p)
            if hosted is not None: # we do have the project p
                if v == hosted: # with the same version
                    has_common = True
                else: # with another version
                    return False
        return has_common
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
            self._log.info("Adding project %s %s", project, version)
            # make link to the root directory of the project in the doc dir
            root = _projRoot(project, version)
            if not root:
                raise ValueError("Cannot find project %s %s" % (project, version))
            self.projects[project] = version
            os.symlink(root, os.path.join(self.path,
                                          "%s_%s" % (project, version)))
            #self._log.debug("Mark the directory as to be built")
            self.toBeBuilt = True

    def _ensureAFSquota(self, required):
        if self.isAfsVolume:
            output = Popen(["fs", "lq", self.path], stdout = PIPE).communicate()[0].splitlines()[-1].split()
            quota = int(output[1])
            used = int(output[2])
            reqsize = int(1.1 * (used + required))
            if quota < reqsize:
                self._log.info("Increasing AFS volume size to %d", reqsize)
                output = Popen(["afs_admin", "sq", self.path, str(reqsize)], stdout = PIPE).wait()

    def _generateDoxygenMainPage(self, depgraphsize = (0,0)):
        """
        Generate the main page file for the documentation directory and return
        it as a string.

        @param depgraphsize: tuple with the width and height of the dependency
            graph (to give the correct size to the svg image).
        """
        page = "/** \\mainpage LHCb Software Documentation\n" + \
            "\\htmlonly\n<p class='subtitle'>%(subtitle)s</p>\n\\endhtmlonly\n" + \
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
        #page += '\n\\image html dependencies.png "Graph of the dependencies between projects"\n'
        page += '\n\\htmlonly\n<div align="center">\n<p>Graph of the dependencies between projects</p>\n'
        page += '<object data="dependencies.svg" type="image/svg+xml"'
        width, height = depgraphsize
        if width and height:
            page += ' width="%d" height="%d"' % (width, height)
        page += '></object>\n</div>\n'
        page += "\\endhtmlonly\n*/\n"
        return page

    def _generateDoxyConf(self, doxygen_versions=(None, None)):
        """
        Generate the doxygen configuration file for the current doc directory.
        """
        if self.locked:
            self._log.warning("Cannot generate Doxygen configuration in a locked directory")
            return
        self._log.info("Generate Doxygen configuration")
        # Get the doxygen versions from the arguments, using the default if not defined
        cpp_version, py_version = map(lambda x: x or doxygen_default_version,
                                      doxygen_versions)
        if cpp_version != py_version:
            self._log.warning("Different versions of Doxygen for C++ and Python. Generating configuration for %s", cpp_version)

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


        doxycfg = DoxyFileCfg()
        # The order of the configuration options matches (more or less) the one
        # in the manual: http://www.doxygen.org/config.html
        #--- Project related options
        doxycfg["PROJECT_NAME"]        = "LHCb Software"
        doxycfg['OUTPUT_DIRECTORY']    = self.output
        doxycfg['CREATE_SUBDIRS']      = True
        doxycfg['CASE_SENSE_NAMES']    = False
        doxycfg['JAVADOC_AUTOBRIEF']   = True
        doxycfg['BUILTIN_STL_SUPPORT'] = True

        #--- Build related options
        doxycfg['EXTRACT_ALL']         = True
        # append the commands to document also private and static members
        doxycfg["EXTRACT_PRIVATE"]     = True
        doxycfg["EXTRACT_STATIC"]      = True
        doxycfg["EXTRACT_LOCAL_CLASSES"] = True
        doxycfg['SHOW_DIRECTORIES']    = True
        doxycfg['GENERATE_TODOLIST']   = True

        #--- Options related to warning and progress messages
        doxycfg['QUIET']               = False
        doxycfg['WARNINGS']            = True
        doxycfg['WARN_LOGFILE']        = "DoxyWarnings.log"

        #--- Input related options
        project_dirs = [ "%s_%s" % item
                         for item in self.projects.items()
                         if item[0] not in ["LCGCMT"] ] # avoid some projects
        # find a binary dir common to all projects
        configs = None
        for p in project_dirs:
            inst_area = os.path.join(self.path, p, 'InstallArea')
            if os.path.exists(inst_area):
                proj_configs = set(c for c in os.listdir(inst_area)
                                   if not c.startswith('.'))
                if configs is None:
                    configs = proj_configs
                else:
                    configs.intersection_update(proj_configs)
        if configs:
            config = configs.pop() # let's pick one up at random
        else:
            # it's unlikely that we end up with no suitable config,
            # but we have to cover the case
            config = None

        # add all subdirs of the projects, but only the include from InstallArea
        inputs = []
        excludes = []
        for p in project_dirs:
            self._log.debug('project: %s', p)
            # find all packages and add them to the search path
            packs = set(findPackages(os.path.join(self.path, p)))
            inputs.extend(os.path.join(p, subdir) for subdir in packs)
            self._log.debug('%d packages: %s', len(packs), ', '.join(packs))

            if config:
                inc_dir = os.path.join(p, 'InstallArea', config, 'include')
                if os.path.exists(os.path.join(self.path, inc_dir)):
                    self._log.debug('looking for includes in %s', inc_dir)
                    # add InstallArea include dir to the search path
                    inputs.append(inc_dir)
                    # but we need to remove the duplicates from the various packages
                    installed_headers = set(findFilesInSubdirs(os.path.join(self.path, inc_dir)))
                    # for each package, exclude from the files in the install area that
                    # are already in the package
                    for pack in packs:
                        pack_files = set(findFilesInSubdirs(os.path.join(self.path, p, pack)))
                        to_exclude = installed_headers.intersection(pack_files)
                        if to_exclude:
                            self._log.debug('except %d files found in %s', len(to_exclude), pack)
                            excludes.extend(os.path.join('*', inc_dir, f)
                                            for f in to_exclude)

        doxycfg['INPUT'] = inputs

        doxycfg['RECURSIVE']           = True
        excludes += [
            "*/dict/*",
            # Exclude tests
            "*/test/*",
            "*/Test/*",
            "*/tests/*",
            "*/Tests/*",
            "*/utest/*",
            "*/examples/*",
            "*/GaudiExamples/*",
            # Exclude binaries
            "*/slc3_*/*",
            "*/slc4_*/*",
            "*/*-slc5-*/*",
            "*/win32_*/*",
            "*/*-winxp-*/*",
            "*/osx105_*/*",
            "*/*-mac106-*/*",
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
            "*/qcustomplot.cpp",
            ]
        doxycfg["EXCLUDE_PATTERNS"] = excludes
        files = []
        for p in doxycfg["INPUT"]:
            if (p.endswith("Sys") and not p.endswith("GaudiSys")) or p.endswith("Release"):
                # FILE_PATTERNS   += *LHCB_<VERSION>/LHCbSys*requirements
                files.append("*%s*requirements" % p)
        doxycfg["FILE_PATTERNS"] = files
        doxycfg['LAYOUT_FILE'] = os.path.join("conf", "DoxygenLayout.xml")
        doxycfg['IMAGE_PATH'] = ["conf"]

        #--- Source browsing related options
        doxycfg['SOURCE_BROWSER']      = True
        doxycfg['INLINE_SOURCES']      = True

        #--- Alphabetical index options
        doxycfg['ALPHABETICAL_INDEX']  = True
        doxycfg['COLS_IN_ALPHA_INDEX'] = 3

        #--- HTML related options
        doxycfg['GENERATE_HTML']       = True
        doxycfg['HTML_ALIGN_MEMBERS']  = True
        doxycfg['HTML_TIMESTAMP']      = True
        doxycfg['SEARCHENGINE']        = True
        doxycfg['SERVER_BASED_SEARCH'] = True
        doxycfg['HTML_STYLESHEET']     = os.path.join("conf", "lhcb_doxygen.css")
        #doxycfg['GENERATE_ECLIPSEHELP']= True
        #doxycfg['ECLIPSE_DOC_ID']      = self.name

        #--- LaTeX related options
        # (useful also for formulas)
        doxycfg['GENERATE_LATEX']      = False
        # doxycfg['LATEX_BATCHMODE']     = True  # I prefer the \nonstopmode
        doxycfg["EXTRA_PACKAGES"] = [ "times", "amsmath" ]

        #--- RTF related options
        doxycfg['GENERATE_RTF']        = False

        #--- Man page related options
        doxycfg['GENERATE_MAN']        = False

        #--- Preprocessor related options
        doxycfg['SKIP_FUNCTION_MACROS'] = True

        #--- External reference options
        # doxycfg['GENERATE_TAGFILE'] # set to the temporary directory
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

        #--- Dot options
        doxycfg['CLASS_DIAGRAMS']      = True
        doxycfg["HAVE_DOT"]            = True
        doxycfg["DOT_NUM_THREADS"]     = 1 # doxygen 1.7
        doxycfg["CLASS_GRAPH"]         = True
        doxycfg["COLLABORATION_GRAPH"] = True
        doxycfg["INCLUDE_GRAPH"]       = True
        doxycfg["INCLUDED_BY_GRAPH"]   = True
        doxycfg["GRAPHICAL_HIERARCHY"] = True
        doxycfg["DOT_PATH"]            = None


        # Write the output files
        # prepare the directory
        confdir = os.path.join(self.path, "conf")
        if not os.path.isdir(confdir):
            self._log.debug("Creating directory %s", confdir)
            os.makedirs(confdir)

        # Special manipulation required for the C++ and Python versions
        orig = {}
        for k in ['FILE_PATTERNS', 'WARN_LOGFILE', 'INPUT']: # keep a copy
            orig[k] = doxycfg[k]

        if self._hasCpp():
            # C++ configuration
            doxycfg['FILE_PATTERNS'] = [ "*.h", "*.icpp", "*.cpp" ] + orig['FILE_PATTERNS']
            doxycfg['WARN_LOGFILE'] = orig['WARN_LOGFILE'].replace(".log", "Cpp.log")
            doxycfg['INPUT'] = orig['INPUT'] + ["conf/MainPageCpp.doxygen"]
            open(os.path.join(confdir, "DoxyFileCpp.cfg"), "w").write(str(doxycfg))

        # Python configuration
        doxycfg['FILE_PATTERNS'] = [ "*.py" ] + orig['FILE_PATTERNS']
        doxycfg['WARN_LOGFILE'] = orig['WARN_LOGFILE'].replace(".log", "Py.log")
        doxycfg['INPUT'] = orig['INPUT'] + ["conf/MainPagePy.doxygen"]
        doxycfg["OPTIMIZE_OUTPUT_JAVA"] = True # see http://www.doxygen.org/config.html#cfg_optimize_output_java
        open(os.path.join(confdir, "DoxyFilePy.cfg"), "w").write(str(doxycfg))

        # Create the auxiliary files in the conf directory
        from _LHCbDocResources import getString
        #  generate the dependency graph
        depgraphsize = self._genDepGraph(confdir)
        mp_data = self._generateDoxygenMainPage(depgraphsize = depgraphsize)
        mp_subtitle = '%s Code Version. <a href="%s/index.html">Go to %s</a>'
        if self._hasCpp():
            open(os.path.join(confdir, "MainPageCpp.doxygen"), "w").write(mp_data %
                 {"subtitle": mp_subtitle % ("C++", "py", "Python")})
            open(os.path.join(confdir, "MainPagePy.doxygen"), "w").write(mp_data %
                 {"subtitle": mp_subtitle % ("Python", "..", "C++")})
        else:
            open(os.path.join(confdir, "MainPagePy.doxygen"), "w").write(mp_data %
                 {"subtitle": ""}) # {"subtitle": "Python Code"})
        #  layout file
        open(os.path.join(confdir, 'DoxygenLayout.xml'), 'w').write(getString('layout.xml', cpp_version))
        #  CSS file
        open(os.path.join(confdir, 'lhcb_doxygen.css'), 'w').write(getString('stylesheet.css', cpp_version))
        #  class locator PHP script
        open(os.path.join(confdir, 'class.php'), 'w').write(getString('class.php', cpp_version))

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
        dotdata = []
        for project in self.projects:
            project = project_names.get(project.upper(), project)
            deps = [ project_names.get(p.upper(), p)
                     for p in self._projectDeps(project) ]
            deps.sort()
            dotdata.append("%s->{%s};" % (project, ";".join(deps)))
        if dotdata:
            dotdata = "digraph dependencies {\nbgcolor=transparent;\nnode [fontsize=10,style=filled,fillcolor=white];\n%s\n}\n" % ("\n".join(dotdata))
        else:
            dotdata = ""
        cmd = ["dot"]
        for format_name in ["png", "svg", "eps", "fig"]:
            if type(format_name) is tuple:
                format_name, extension = format_name
            else:
                extension = format_name
            cmd.append("-T%s" % format_name)
            cmd.append("-o%s" %
                       os.path.join(self.path, destination, "dependencies.%s" % extension))
        dotfile = os.path.join(self.path, destination, "dependencies.dot")
        cmd.append(dotfile)
        open(dotfile, "w").write(dotdata)
        Popen(cmd).wait()
        # extract the size of the image from the SVG file
        width, height = 0, 0
        if os.path.exists(os.path.join(self.path, destination, "dependencies.svg")):
            from xml.etree import ElementTree as ET
            doc = ET.parse(os.path.join(self.path, destination, "dependencies.svg"))
            attrib = doc.getroot().attrib
            if "viewBox" in attrib:
                _, _, width, height = attrib["viewBox"].split()
                # convert to pixels assuming 96dpi (default resolution in graphviz)
                width = int(round(float(width) / 72 * 96))
                height = int(round(float(height) / 72 * 96))
        return (width, height) # return the size in pixels of the image

    def _buildDox(self, conf, workdir, version = None):
        """
        Run Doxygen using a configuration file to generate the output into a
        specified directory.

        @param conf: path to the configuration file
        @param workdir: output directory for Doxygen (override configuration)
        @param version: version of Doxygen to use
        """
        retcode = 0
        if "LHCBDOC_TESTING" not in os.environ:
            # use a temporary configuration file to generate the output in the work directory
            import tempfile
            tmpFd, tmpName = tempfile.mkstemp(prefix = conf)
            try:
                tmp = os.fdopen(tmpFd, "w")
                tmp.write(open(conf).read())
                tmp.write("\nOUTPUT_DIRECTORY = %s\n" % workdir)
                tmp.write("\nGENERATE_TAGFILE = %s\n" % os.path.join(workdir, "html", "doxygen.tag"))
                tmp.close()
                doxcmd = ["doxygen"]
                if version:
                    doxcmd += ["--doxygen-version", version]
                doxcmd.append(tmpName)
                proc = Popen(doxcmd, cwd = self.path, stdin = PIPE)
                proc.stdin.write("r\n") # make latex enter \nonstopmode on the first error
                retcode = proc.wait()
            finally:
                os.remove(tmpName)
        else:
            # Fake execution for testing
            os.mkdir(os.path.join(workdir, "html"))
            open(os.path.join(workdir, "html", "index.html"), "w").write("testing\n")

        if retcode != 0:
            raise RuntimeError("Doxygen failed with error %d in %s" % (retcode, workdir))


    def _buildCpp(self, workdir, doxygen_version = None):
        """
        Build the actual doxygen documentation (C++).

        @param doxygen_version: version of Doxygen to use
        """
        self._buildDox(os.path.join(self.path, "conf", "DoxyFileCpp.cfg"), workdir, version = doxygen_version)
        self._log.info("Generate the database of classes from tags")
        doxyTagsToDBM(os.path.join(workdir, "html", "doxygen.tag"),
                      os.path.join(workdir, "html", "classes.db"))

    def _buildPy(self, workdir, doxygen_version = None):
        """
        Build the actual doxygen documentation (Python).

        @param doxygen_version: version of Doxygen to use
        """
        self._buildDox(os.path.join(self.path, "conf", "DoxyFilePy.cfg"), workdir, version = doxygen_version)
        self._log.info("Generate the database of classes from tags")
        doxyTagsToDBM(os.path.join(workdir, "html", "doxygen.tag"),
                      os.path.join(workdir, "html", "classes.db"), python = True)

    def build(self, doxygen_versions = (None, None)):
        """
        Build the doxygen documentation.
        Prepare the infrastructure to build both C++ and Python documentation,
        then call the specific methods.

        @param doxygen_versions: pair of strings defining the version of doxygen to be used for C++ and Python
        """
        if self.locked:
            self._log.warning("Cannot build in a locked directory")
            return
        # ensure we have enough space for the files we need to generate
        self._ensureAFSquota(10000) # 10MB
        # create lockfile
        self._log.debug("Creating lock file '%s'", self._lockFile)
        open(self._lockFile, "w").write("%s - %s\n" % (os.getpid(),
                                                       datetime.now().strftime(self._docLockTimeFmt)))
        try:
            # Build the documentation in a temporary directory.
            # - prepare the temporary directory
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

            # use a subdirectory of the tempdir for each of C++ and Python
            if self._hasCpp():
                cpptempdir = os.path.join(tempdir, "cpp")
                os.makedirs(cpptempdir)
            pytempdir = os.path.join(tempdir, "py")
            os.makedirs(pytempdir)

            self._generateDoxyConf(doxygen_versions)

            self._log.info("Running doxygen")
            self._log.debug(_which("doxygen"))
            # - modify the doxygen file to use a temporary directory

            if self._hasCpp():
                self._buildCpp(cpptempdir, doxygen_versions[0])
            self._buildPy(pytempdir, doxygen_versions[1])

            if os.path.exists(self.output + ".bk"):
                # Remove old backups before copying the new documentation
                shutil.rmtree(self.output + ".bk")
            if os.path.exists(self.output + ".new"):
                # Remove stale new documentation before copying
                shutil.rmtree(self.output + ".new")
            if self.isAfsVolume:
                usage = _diskUsage(tempdir) / 1024
                self._ensureAFSquota(usage)

            # copy the documentation from the temporary directory to the final place with a temporary name
            self._log.info("Copy generated files from temporary directory")
            try:
                if self._hasCpp():
                    # Copy C++ with structure
                    shutil.copytree(cpptempdir, self.output + ".new")
                    # Copy Python html as a directory in the C++ one
                    shutil.copytree(os.path.join(pytempdir, "html"), os.path.join(self.output + ".new", "html", "py"))
                else:
                    # Copy Python with structure
                    shutil.copytree(pytempdir, self.output + ".new")
                # copy files to the doxygen directory (dependency graph, class.php)
                for f in [ f for f in os.listdir(os.path.join(self.path, "conf"))
                             if f.startswith("dependencies.") or f in ["class.php"]]:
                    src = os.path.join(self.path, "conf", f)
                    shutil.copyfile(src, os.path.join(self.output + ".new", "html", f))
                    if self._hasCpp(): # we need to copy the graphs also in the Python directory
                        shutil.copyfile(src, os.path.join(self.output + ".new", "html", "py", f))
                if self.isAfsVolume:
                    # Give read access to everybody
                    self._log.debug("Give read access (recursively) to %s", self.path)
                    for dirpath, _, _ in os.walk(self.path):
                        Popen(["fs", "setacl", "-dir", dirpath, "-acl", "system:anyuser", "rlk"]).wait()
                # Swap the old and the new documentation (avoid that the users see an incomplete doc)
                # @todo: it should be done for the tag file too
                # Move away the old documentation
                if os.path.isdir(self.output):
                    old = self.output + ".bk"
                    self._log.warning("Old documentation moved to %s", old)
                    os.rename(self.output, old)
                # rename the new documentation
                os.rename(self.output + ".new", self.output)

                self._updateCommonLinks()
                self._log.debug("Documentation ready")

            except shutil.Error, exc:
                self._log.error("Failed to copy files: %s", exc)
                with open(os.path.join(self.path, 'shutil_error.txt'), 'w') as error_log:
                    error_log.write(str(exc))

            # Mark as built
            self.toBeBuilt = False
            # Since we have built the doc, remove the "rebuild flag" if present
            if os.path.exists(self._rebuildFlagFile):
                os.remove(self._rebuildFlagFile)

            shutil.rmtree(tempdir)

        finally:
            # clean up the lock
            self._log.debug("Removing lock file '%s'", self._lockFile)
            os.remove(self._lockFile)

    def _doclink(self, project, version):
        """
        Path to the link in the common directory.
        """
        return os.path.join(self.root, self._docCollDir, project, version)

    def _updateCommonLinks(self):
        """
        Update the links in the common directory.
        """
        for project, version in self.projects.items():
            project = project.lower()
            # update links in the common directory
            doclink = self._doclink(project, version)
            doclinkdir = os.path.dirname(doclink)
            # create the parent directories if missing
            if not os.path.isdir(doclinkdir):
                self._log.debug("Creating directory %s", doclinkdir)
                os.makedirs(doclinkdir)
            # make the version symlink (if necessary)
            linkdest = os.path.relpath(os.path.join(self.output, "html"), doclinkdir)
            if os.path.exists(doclink):
                try:
                    old = os.readlink(doclink).split(os.path.sep)[2]
                except:
                    old = None
                if old < self.name:
                    self._log.info("Moving link %s from %s to %s", doclink, old, self.name)
                    os.remove(doclink)
                    os.symlink(linkdest, doclink)
            else:
                self._log.info("Creating link %s to %s", doclink, self.name)
                os.symlink(linkdest, doclink)

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
    for d in sorted(os.listdir(root)):
        if Doc._nameRegexp.match(d):
            docs.append(Doc(d, root))
    return docs

def updateLatestLinks(root = None):
    """
    Update the links to the latest version of the projects in the common
    directory.
    """
    doclinkdir = os.path.join(Doc._root(root), Doc._docCollDir)
    all_versions = []
    for project in os.listdir(doclinkdir):
        if project == "lcgcmt":
            continue
        for version in os.listdir(os.path.join(doclinkdir, project)):
            if version == "latest":
                continue
            all_versions.append((project, version))
    for p, v in getLatestVersions(all_versions):
        latest = os.path.join(doclinkdir, p, "latest")
        if not (os.path.islink(latest) and (os.readlink(latest) == v)):
            # the link doesn't exist or doesn't point to the most recent version
            if os.path.exists(latest) or os.path.islink(latest):
                # ensure that the link is not present before creating the new one
                os.remove(latest)
            _log.info("Moving link %s to %s", os.path.join(p, "latest"), os.path.join(p, v))
            os.symlink(v, latest)

#--- Application logic
def makeDocs(projects, root = None, no_build = False, doxygen_versions = (None, None)):
    """
    @param projects: list of pairs with (name, version) for each project to use
    @param root: base directory for the documentation directories
    @param no_build: flag to prevent the execution of Doxygen (for testing)
    @param doxygen_versions: pair of strings defining the version of doxygen to be used for C++ and Python
    """
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
        # (projects without dependencies like LbScript will use their own doc directory)
        deps = _getProjDeps(project, version)
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
        if _has_kinit:
            # ensure that we have got a fresh token (if possible) before each
            # new build
            call(['kinit', '-R'])
        if no_build:
            # if we should not run doxygen, at least generate the doxygen configuration
            doc._generateDoxyConf()
            # and create the fake sym-links
            doc._updateCommonLinks()
        else:
            doc.build(doxygen_versions)
    ## @todo: probably, this step should be done inside the build step
    # Update links pointing to the latest versions
    updateLatestLinks(root)

    projects -= projects_added
    if projects:
        _log.warning("Projects not added: %s", list(projects))

def checkDocs(projects, root = None, timeout = None):
    """
    Check if there are no obvious problems in the documentation directories.

    @param projects: list of pairs with (name, version) for each project to use
    @param root: base directory for the documentation directories
    @param timeout: number of hours to wait befor considering a Doc directory as
           too old.
    @return: 0 in case of no problem, 1 if stale directories, 2 if unknownProjects
             3 if both stale and unknown
    """
    retval = 0

    docs = allDocs(root)

    stale = []
    if timeout:
        # stale doc builds
        now = datetime.now()
        dt = timedelta(seconds = timeout * 60 *60) # 23 hours
        stale = [doc for doc in docs if doc.locked and ((now - doc.locked[1]) > dt)]

    if stale:
        _log.warning("Documentation directories which are locked since more than %d hours", timeout)
        for doc in stale:
            _log.warning("\t%s", doc)
        docs = [doc for doc in docs if doc not in set(stale)]
        retval |= 1

    # This is tricky removes from the the set of projects passed to the list
    # ('set(projects)') the projects present in any of the documentations.
    unknownProjects = reduce(lambda s, v: s.difference(v),
                             [doc.projects.items() for doc in docs],
                             set(projects))
    if unknownProjects:
        _log.warning("Projects without documentation")
        for proj, version in sorted(unknownProjects):
            _log.warning("\t%s %s", proj, version)
        retval |= 2

    return retval

def getLatestVersions(versions):
    """
    Given a list of project versions in the form of (project, version),
    return a list of the latest version for each project.
    """
    from LbConfiguration.Version import CoreVersion
    projects = {} # dictionary with a list of versions per project
    for p, v in [ (p.lower(),      # ensure we use the lowercase name
                   CoreVersion(v)) # Sortable class to wrap a version string
                  for p, v in versions
                  if CoreVersion.version_style.match(v) ]:
        try:
            projects[p].append(v)
        except:
            projects[p] = [v]
    # sort the versions of each project and get the last one
    for p in projects:
        projects[p].sort()
        projects[p] = str(projects[p][-1])
    return projects.items()

def findProjects(exclude = None):
    """
    Find all the available (project, version) pairs available in the release area.
    """
    releases = os.environ["LHCBRELEASES"]
    logging.debug("Looking for projects in '%s'", releases)
    if exclude is None: # default exclusion
        exclude = set([ "GANGA", "DIRAC", "LHCBGRID", "CURIE", "GEANT4", "COMPAT" ])
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
                if os.path.exists(os.path.join(projdir, versdir, "cmt", "project.cmt")) and \
                  not os.path.exists(os.path.join(projdir, versdir, 'NOT_READY')): # Ignore projects that are being released
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
    # If the doclinkdir doesn't exists, it's still to be created and we can ignore
    # it.
    if not os.path.isdir(doclinkdir):
        return []
    referenced = set()
    # the structure of the common directory is:
    #   docs/<project>/<version>
    # so we have to iterate over the 2 levels
    for project in os.listdir(doclinkdir):
        projectdir = os.path.join(doclinkdir, project)
        for version in os.listdir(projectdir):
            path = os.path.join(projectdir, version)
            try:
                relpath = os.readlink(path).split(os.path.sep)
                referenced.add(relpath[2])
            except:
                # the previous block can fail if path is not a link or in the case
                # of a "latest" link:
                #  - a normal link is "../../DOC_123456/doxygen/html"
                #  - a "latest" link is "<version>"
                pass
    return [d for d in allDocs(root) if d.name not in referenced]

def cleanArchivedProjects(root = None):
    """
    Delete from the docs directories the broken links.
    """
    root = Doc._root(root)
    logging.warning("Cleaning archived project docs from %s", root)
    broken_docs = findBrokenDocs(root)
    # Collect the list of archived projects
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
            except OSError, x:
                logging.warning("OSError: %s", x) # ignore exceptions during removal
    # For each archived project, remove the entry in the common directory
    # (docs/<project>/<version>)
    for p in archived_project:
        try:
            p = p.split("_")
            l = os.path.join(root, Doc._docCollDir, p[0].lower(), p[1])
            logging.info("Removing %s", l)
            os.remove(l)
        except IOError, x:
            logging.warning("IOError: %s", x) # ignore exceptions during removal
        except OSError, x:
            logging.warning("OSError: %s", x) # ignore exceptions during removal

def removeUnusedDocs(root = None):
    """
    Delete the doc directories that are not referenced by the common doc directory.
    """
    root = Doc._root(root)
    logging.warning("Removing unused doc directory from %s", root)
    for d in findUnusedDocs(root):
        if d.isAfsVolume:
            logging.warning("Removal of AFS volumes not yet implemented! (%s)", d.path)
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
    parser.add_option("--doxygen-version", action = "store",
                      help = "Version of doxygen to use. Note: it works only "
                             "when using the LbScript doxygen wrapper and if "
                             "the given version exists in the LCG externals")
    parser.add_option("--doxygen-py-version", action = "store",
                      help = "Same as --doxygen-version, but allows to "
                             "specify a version of doxygen for the Python "
                             "documentation (the default is to use the version "
                             "specified with --doxygen-version)")
    parser.add_option("--timeout", action = "store",
                      metavar = "HOURS",
                      type = "int",
                      help = "Do not allow the script to run more than the "
                             "specified number of hours, 0 (default) means no "
                             "timeout.")
    parser.add_option("--check", action = "store_true",
                      help = "Check if there are problems in the Doc directories")


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

    if opts.check:
        sys.exit(checkDocs(projects, opts.root, opts.timeout))

    # Ensure that the version of doxygen used for Python is the same as the one
    # used for C++ unless specified differently.
    if opts.doxygen_version and not opts.doxygen_py_version:
        opts.doxygen_py_version = opts.doxygen_version

    # Clean-up functions
    if opts.clean_archived:
        cleanArchivedProjects(opts.root)
    if opts.remove_unused:
        removeUnusedDocs(opts.root)
    # Main function
    makeDocs(projects, opts.root, opts.no_build,
             doxygen_versions = (opts.doxygen_version, opts.doxygen_py_version))

if __name__ == '__main__':
    main()
