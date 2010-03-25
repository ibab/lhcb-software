#!/usr/bin/env python
"""
"""
__author__ = "Marco Clemencic"

import os, re

from subprocess import Popen, PIPE

from stat import ST_SIZE

import logging
_log = logging.getLogger(__name__)

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
    old = os.getcwd()
    retval = ""
    try:
        cmt = os.path.join(root, "cmt")
        _log.debug("'cmt show projects' in %s", cmt)
        os.environ["PWD"] = cmt
        retval = Popen(["cmt","show","projects"], cwd = cmt, stdout = PIPE).communicate()[0]
    finally:
        os.environ["PWD"] = old
    return retval

def _getProjDeps(project, version, exclude = None):
    """
    Get all the dependencies of a project, excluding the projects in the list
    passed as exclude (by default ["LCGCMT"]).
    
    @param project: name of the project
    @param version: version of the project
    @param exclude: list of projects to exclude from the result
    
    @return: list of pairs (project, version)
    """
    if exclude is None:
        exclude = ["LCGCMT"] # ignore LCGCMT
    pr = _projRoot(project, version)
    if not pr:
        raise ValueError("Project %s %s does not exist" % (project, version))
    return set([ (l[0], l[1].split("_")[1])
                 for l in [ l.strip().split()
                            for l in
                            _cmt_show_projects(pr).splitlines()[1:] ]
                 if l and l[1] != "(in" and l[0] not in exclude ])

from LbUtils.CMT.Project import Project
def _getProjDepsX(project, version, exclude = None):
    if exclude is None:
        exclude = ["LCGCMT"] # ignore LCGCMT
    deps = set()
    to_process = [Project(_projRoot(project, version))]
    while to_process:
        proj = to_process.pop()
        for b in proj.base(cmtprojectpath = os.environ["LHCBPROJECTPATH"]):
            if b.name() not in exclude:
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
    """
    _nameRegexp = re.compile("^DOC_[0-9]{6}$")
    _namePattern = "DOC_%06d"
    _docSubdir = "doxygen"
    _docCollDir = "docs"

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
            _log.debug("Automatic directory name %s", self.name)
        
        # full path to the directory of the object
        self.path = os.path.join(self.root, self.name)
        
        # full path to the doxygen destination directory
        self.output = os.path.join(self.path, self._docSubdir)
        
        # projects in the directory
        self.projects = {}
        if not os.path.isdir(self.path):
            _log.info("Creating directory %s", self.path)
            os.makedirs(self.path)
        else:
            _log.debug("Listing projects in %s", self.path)
            for l in os.listdir(self.path):
                if os.path.islink(os.path.join(self.path, l)):
                    p, v = l.split("_")
                    self.projects[p.upper()] = v # ensure that we use upper case names
            _log.debug("Found %d projects: %s", len(self.projects),
                       " ,".join(map(str, self.projects.items())))
    
    def _allDocNames(self):
        """
        Get all doc names in the same top-level directory as this one
        (including it).
        """
        docs = filter(os.path.isdir,
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
        project = project.upper() # ensure upper case
        # check if the project is already there
        if project in self.projects:
            # @todo: check if the version is the same as the contained one
            raise ValueError("Project %s already in %s" % (project, self.name))
        else:
            _log.debug("Adding project %s %s", project, version)
            # make link to the root directory of the project in the doc dir 
            root = _projRoot(project, version)
            if not root:
                raise ValueError("Cannot find project %s %s" % (project, version))
            self.projects[project] = version
            os.symlink(root, os.path.join(self.path,
                                          "%s_%s" % (project, version)))
            # make the link to the doc directory in the common one
            doclinkdir = os.path.join(self.root, self._docCollDir)
            if not os.path.isdir(doclinkdir):
                _log.debug("Create directory %s", doclinkdir)
                os.makedirs(doclinkdir)
            doclink = os.path.join(doclinkdir,
                                   "%s_%s" % (project, version))
            if os.path.exists(doclink):
                old = os.path.basename(os.readlink(doclink))
                if old < self.name:
                    _log.info("Moving link %s from %s to %s", doclink, old, self.name)
                    os.remove(doclink)
                    os.symlink(os.path.join("..", self.name), doclink)
            else:
                _log.info("Create link %s to %s", doclink, self.name)
                os.symlink(os.path.join("..", self.name), doclink)
            
    
    def _generateDoxyFile(self):
        """
        Generate the doxygen configuration file for the current doc directory.
        """
        _log.info("Generate DoxyFile.cfg")
        doxycfg = DoxyFileCfg([("PROJECT_NAME", "LHCb Software"),
                               ('OUTPUT_DIRECTORY', self.output),
                               ('GENERATE_TAGFILE', 'LHCbSoft.tag'),
                               ('CREATE_SUBDIRS', True),
                               ])
        doxycfg['INPUT'] = [ "%s_%s" % item
                             for item in self.projects.items() ]
        
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
            "*/genConf/*",
            ]
        doxycfg["EXCLUDE_PATTERNS"] = excludes
        
        files = [ "*.cpp", "*.h", "*.icpp", "*.py" ]
        for p in doxycfg["INPUT"]:
            for d in [ d
                       for d in os.listdir(os.path.join(self.path, p))
                       if (d.endswith("Sys") and d != "GaudiSys") or d.endswith("Release")]:
                files.append("*%s*requirements" % d)
        # FILE_PATTERNS   += *LHCbSys*requirements
    
        doxycfg["FILE_PATTERNS"] = files
        
        extra_packages = [ "times", "amsmath" ]
        doxycfg["EXTRA_PACKAGES"] = extra_packages
    
        doxycfg["INPUT_FILTER"] = "doxypy"
        doxycfg["FILTER_SOURCE_FILES"] = True
    
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
    
        # write the output file
        open(os.path.join(self.path, "DoxyFile.cfg"), "w").write(str(doxycfg))

    def build(self):
        """
        Build the actual doxygen documentation.
        """
        self._generateDoxyFile()
        old = None
        if os.path.isdir(self.output):
            old = self.output + ".bk"
            _log.warning("Old documentation moved to %s", old)
            os.rename(self.output, old)
        _log.info("Running doxygen")
        # @todo build the documentation in a temporary directory
        proc = Popen(["doxygen", "DoxyFile.cfg"], cwd = self.path, stdin = PIPE)
        proc.stdin.write("r\n") # make latex enter \nonstopmode on the first error
        return proc.wait()

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
def main():
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] project version")
    parser.add_option("--verbose", action = "store_true",
                      help = "Print more messages")
    parser.add_option("--debug", action = "store_true",
                      help = "Print debugging messages")
    parser.add_option("--root", action = "store",
                      help = "Override the root directory of the documentation")
    parser.add_option("--no-build", action = "store_true",
                      help = "Do not run doxygen")
    
    opts, args = parser.parse_args()
    if len(args) != 2:
        parser.error("Wrong number of arguments")
    project = args[0].upper()
    version = args[1]
    
    log_level = logging.WARNING
    if opts.verbose:
        log_level = logging.INFO
    if opts.debug:
        log_level = logging.DEBUG
    logging.basicConfig(level = log_level)
    
    docs = allDocs(opts.root)
    
    # check if the project is already known
    for d in docs:
        if d.getVersion(project) == version:
            _log.warning("Project %s %s is already in %s", project, version, d.name)
            return

    # Get all the dependencies of the project
    deps = _getProjDeps(project, version)
    # get the candidates, i.e. the doc directories that can include the requested
    # project with its dependencies
    candidates = [ d for d in docs
                   if d.canHost(project, version, deps) ]
    if not candidates:
        # no candidates available, create a new doc dir
        _log.debug("Create new doc dir")
        doc = Doc()
    elif len(candidates) == 1:
        # only one candidate, use it
        doc = candidates[0]
        _log.debug("Using %s", doc.name)
    else:
        # more than one candidate
        _log.error("More than one candidate doc dir: %s",
                   ", ".join([d.name for d in candidates]))
        return
    # First add the dependencies (not yet there)
    for p, v in deps:
        if doc.getVersion(p) is None:
            doc.add(p, v)
    # Then add the project
    doc.add(project, version)
    if opts.no_build:
        # if we should not run doxygen, at least generate the doxygen file
        doc._generateDoxyFile()
    else:
        doc.build()

if __name__ == '__main__':
    main()
