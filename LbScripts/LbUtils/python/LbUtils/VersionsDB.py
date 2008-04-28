"""
Module providing a database of software releases.
The database is based on an XML file containing the list of (project,version)s
for each version of the complete software stack.
"""
__author__ = "Marco Clemencic <Marco.Clemencic@cern.ch>"
__version__ = "$Id: VersionsDB.py,v 1.2 2008-04-28 10:40:23 marcocle Exp $"

# Hack to simplify the usage of sets with older versions of Python.
import sys
if sys.version_info < (2,4):
    from sets import Set as set

#--- Error definitions
class VersionsDBError(RuntimeError):
    """
    Generic VersionsDB error.
    """
    pass
class DuplicatedReleaseError(VersionsDBError):
    """
    Error raised when a new released is created with the same name of an existing
    one.
    """
    def __init__(self,rel):
        VersionsDBError.__init__(self,"Duplicated release '%s'"%(rel))
        self.release = rel
class DuplicatedProjectError(VersionsDBError):
    """
    Error raised when a new project is added to a release, with the same name of
    an already existing project in that release.
    """
    def __init__(self,rel,proj):
        VersionsDBError.__init__(self,"Duplicated project '%s' in release '%s'"%(proj,rel))
        self.release = rel
        self.project = proj

#--- Main classes
import re
class Version:
    """
    Handle version numbers.
    """
    __regexp__ = re.compile(r"v([0-9]+)r([0-9]+)(?:p([0-9]+))?")
    def __init__(self,*args):
        """
        Construct a version object from a version string like "v1r2p3" or
        from 2-3 integers .
        """
        self._version = ()
        if len(args) < 1:
            raise TypeError("__init__() takes at least 1 argument (0 given)")
        elif len(args) == 1:
            if type(args[0]) is str:
                import re
                m = self.__regexp__.match(args[0])
                if m:
                    self._version = tuple([int(i)
                                           for i in m.groups()
                                           if i is not None])
                else:
                    raise ValueError("wrong version string: '%s'"%args[0])
            elif type(args[0]) is self.__class__:
                # copy constructor
                self._version = args[0]._version                
            else:
                raise TypeError("__init__() needs a string as single argument ('%s' given)"%type(args[0]))
        elif len(args) <= 3:
            self._version = tuple([int(i) for i in args]) 
        else:
            raise TypeError("__init__() takes at most 3 arguments (%d given)"%len(args))
    def __repr__(self):
        """
        Returns a string that can be evaluated to get a version object
        equivalent to the current one.
        """
        return self.__class__.__name__ + repr(self._version)
    def __str__(self):
        """
        Return a the version as version string like "v1r2p3".
        """
        if len(self._version) == 2:
            s = "v%dr%d"
        else:
            s = "v%dr%dp%d"
        return s%self._version
    def __hash__(self):
        return hash(self._version)
    def __cmp__(self,other):
        if other.__class__ == self.__class__:
            other = other._version
        if self._version < other: return -1
        elif self._version == other: return 0
        else: return 1

class Project:
    """
    Informations about a project.
    """
    def __init__(self,name,version,buildtimedeps=[],runtimedeps=[]):
        self.name = name
        self.version = version
        self.buildtimedeps = buildtimedeps[:]
        self.runtimedeps = runtimedeps[:]
    def addBuildTimeDep(self,proj_name):
        if proj_name not in self.buildtimedeps:
            self.buildtimedeps.append(proj_name)
    def addRunTimeDep(self,proj_name):
        if proj_name not in self.runtimedeps:
            self.runtimedeps.append(proj_name)
    def __repr__(self):
        return "Project(name=%s,version=%s,buildtimedeps=%s,runtimedeps=%s)" % \
                (repr(self.name),
                 repr(self.version),
                 repr(self.buildtimedeps),
                 repr(self.runtimedeps))
    
class Release:
    """
    Class implementing the versions database storage.
    The name of the instance of this class is unique.
    Instances of this class look like dictionaries where the project name is the
    key and the version is the value.
    A release can inherit the same versions of the projects from another release
    (option base of the constructor).
    """
    __releases__ = {}
    def __init__(self, name, projects = [], base = None):
        if name in self.__releases__:
            raise DuplicatedReleaseError(name)
        self.name = name
        self.base = base
        self.projects = {}
        for (k,v) in projects:
            self.projects[k] = v
        self.__releases__[name] = self
        
    def __repr__(self):
        return "Release(name=%s,projects=%s,base=%s)"%(repr(self.name),
                                                       repr(self.projects.items()),
                                                       repr(self.base))
    def __contains__(self,key):
        return ( key in self.projects ) or \
               ( self.base is not None and key in self.__releases__[self.base] )
    def __getitem__(self,key):
        if key in self.projects:
            return self.projects[key]
        elif self.base:
            return self.__releases__[self.base][key]
        else:
            raise KeyError(key)
    def __setitem__(self,key,value):
        if key in self.projects:
            raise DuplicatedProjectError(self.name,key)
        self.projects[key] = Project(name=key,version=value)
    def add(self,project):
        if project.name in self.projects:
            raise DuplicatedProjectError(self.name,project.name)
        self.projects[project.name] = project
    def items(self):
        return self.projects.items()
    def values(self):
        return self.projects.values()
    def _expandDeps(self,project,function):
        deps = function(self[project])
        deps += reduce(lambda a,b: a+b,
                       [self._expandDeps(p,function) for p in deps],
                       [])
        return deps
    def expandBuildTimeDeps(self,project):
        return list(set(self._expandDeps(project,lambda p: p.buildtimedeps)))
    def expandRunTimeDeps(self,project):
        deps = reduce(lambda a,b: a+b,
                      [ self.expandRunTimeDeps(p)
                        for p in self.expandBuildTimeDeps(project) ],
                      [])
        return list(set(deps + self._expandDeps(project,lambda p: p.runtimedeps)))
    
from xml.sax.handler import ContentHandler
class _ReleaseSAXHandler(ContentHandler):
    """
    SAX content handler used to fill the database of releases from the XML file.
    Note: the database is purged before a 
    """
    def startDocument(self):
        #print "startDocument"
        # reset the DB of releases
        Release.__releases__ = {} 
        self.releases = Release.__releases__
        self._currentRelease = None
        self._currentProject = None
    def endDocument(self):
        #print "endDocument"
        #pprint(self.releases)
        pass
    def startElement(self,name,attrs):
        #print "start element %s:"%name
        if name == u'releases_db':
            return
        name_attr = attrs.getValue(u'name')
        if name == u'release':
            base = attrs.get(u'base',None)
            self._currentRelease = Release(name_attr, base = base)
        elif name == u'project':
            self._currentProject = Project(name = name_attr,
                                           version = attrs.getValue(u'version'))
            self._currentRelease.add(self._currentProject)
        elif name == u'buildtimedep':
            self._currentProject.addBuildTimeDep(name_attr)
        elif name == u'runtimedep':
            self._currentProject.addRunTimeDep(name_attr)
        else:
            print "Warning: unknown element '%s'"%name
    def endElement(self,name):
        #print "end element %s:"%name
        if name == u'release':
            self._currentRelease = None
        elif name == u'project':
            self._currentProject = None

#--- Initialization functions
def load(source):
    """
    Load the database from an XML source (filename or file object).
    """
    from xml.sax import parse
    parse(source,_ReleaseSAXHandler())
    
def loadString(source):
    """
    Load the database from an XML string.
    """
    from xml.sax import parseString as parse
    parse(str(source),_ReleaseSAXHandler())

#--- Internal query functions
def _findReleases(project,version):
    """
    Find all the releases that feature the specified pair (project,version).
    """
    return [ r.name
             for r in Release.__releases__.values()
             if project in r and r[project].version == version ]

def _findAllReleases(project,versions):
    """
    Find all the releases that feature the specified project in any of the
    specified versions.
    """
    tmp = set()
    for v in versions:
        tmp.update(_findReleases(project,v))
    return list(tmp)

def _getVersions(project,releases):
    """
    Given a set of releases and a project, find all the versions of the project
    used in those releases.
    """
    versions = [ getRelease(r)[project].version
                 for r in releases
                 if project in getRelease(r) ] # do we want this check?
    return list(set(versions))

#--- Public query functions
def getCompatibleVersions(project,version,otherproject,keyproj="LHCb"):
    """
    Given a project and versions, find all the versions of 'otherproject' that
    share the same versions of 'keyproj' as (project,version).
    """
    return _getVersions(otherproject,
                        _findAllReleases(keyproj,
                                         _getVersions(keyproj,
                                                      _findReleases(project, version))))

def getRuntimeVersions(project,version,keyproj="LHCb"):
    deps = []
    proj_releases = _findReleases(project, version)
    #proj_releases.sort()
    if proj_releases:
        rt_deps =proj_releases[-1].expandRuntimeDeps(project)
        comp_releases = _findAllReleases(keyproj,
                                         _getVersions(keyproj,
                                                      proj_releases))
        for p in rt_deps:
            vers = _getVersions(p,comp_releases)
            if vers:
                vers.sort()
                deps.append(p,vers[-1])
    return deps

def getRelease(name):
    """
    Retrieve a release instance by name. 
    """
    return Release.__releases__[name]

def getReleases():
    """
    Return a list of the available release names. 
    """
    return Release.__releases__.keys()

def releaseExists(name):
    """
    Tell if a release with the given name is known. 
    """
    return name in Release.__releases__

# @TODO: Use some alphabetical order.
def generateXML(withDTD = False, withSchema = True):
    # header
    xml = '<?xml version="1.0" encoding="UTF-8"?>\n'
    if withDTD:
        xml += '<!DOCTYPE releases_db SYSTEM "releases_db.dtd">\n<releases_db>\n'
    elif withSchema:
        xml += '<releases_db\n' + \
               ' xmlns="http://cern.ch/lhcb-comp/"\n' + \
               ' xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"' + \
               ' xsi:schemaLocation="http://cern.ch/lhcb-comp/ releases_db.xsd ">\n'
    else:
        xml += '<releases_db>\n'
    # body
    for rel in Release.__releases__.values():
        xml += '  <release name="' + rel.name
        if rel.base is not None:
            xml += '" base="' + rel.base
        xml += '">\n'
        for p in rel.values():
            xml += '    <project name="%s" version="%s">\n'%(p.name,p.version)
            for dep in p.buildtimedeps:
                xml += '      <buildtimedep name="%s"/>\n'%dep
            for dep in p.runtimedeps:
                xml += '      <runtimedep name="%s"/>\n'%dep
            xml += '    </project>\n'
        xml += '  </release>\n'
    xml += '</releases_db>\n'
    return xml
