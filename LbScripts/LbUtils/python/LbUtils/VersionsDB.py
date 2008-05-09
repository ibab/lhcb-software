"""
Module providing a database of software releases.
The database is based on an XML file containing the list of (project,version)s
for each version of the complete software stack.
"""
__author__ = "Marco Clemencic <Marco.Clemencic@cern.ch>"
__version__ = "$Id: VersionsDB.py,v 1.15 2008-05-09 09:57:49 marcocle Exp $"

# Hack to simplify the usage of sets with older versions of Python.
import sys
if sys.version_info < (2,4):
    # pylint: disable-msg=W0622
    from sets import Set as set
#------------------------------------------------------------------------------ 
#--- Constants
#------------------------------------------------------------------------------ 
DEFAULT_KEYPROJECT = "LHCb"
DEFAULT_RELEASEDATE = (1970, 1, 1)
#------------------------------------------------------------------------------ 
#--- Error definitions
#------------------------------------------------------------------------------ 
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
    def __init__(self, rel):
        VersionsDBError.__init__(self,"Duplicated release '%s'"%(rel))
        self.release = rel
        
class DuplicatedProjectError(VersionsDBError):
    """
    Error raised when a new project is added to a release, with the same name of
    an already existing project in that release.
    """
    def __init__(self, rel, proj):
        VersionsDBError.__init__(self,"Duplicated project '%s' in release '%s'"%(proj,rel))
        self.release = rel
        self.project = proj
class DependencyLoopError(VersionsDBError):
    """
    Error raised when a possible dependency loop is detected.
    """
    def __init__(self):
        VersionsDBError.__init__(self,"detected possible dependency loop.")
#------------------------------------------------------------------------------ 
#--- Utility functions
#------------------------------------------------------------------------------ 
def _sortProjects(pl):
    """
    Sort a list of project objects according to the their dependencies.
    """
    weights = {}
    modified = True
    loops_maxcount = 100
    while modified:
        if loops_maxcount < 0:
            raise DependencyLoopError()
        modified = False
        for p in pl:
            w = max([ weights[w]
                      for w in p.buildtimedeps + p.runtimedeps
                      if w in weights ] + [0]) + 1
            if p.name not in weights or weights[p.name] != w:
                modified = True
                weights[p.name] = w
        loops_maxcount -= 1
    l = [ (weights[p.name],p.name,p) for p in pl ]
    l.sort()
    return [ i[2] for i in l ]
#------------------------------------------------------------------------------ 
#--- Main classes
#------------------------------------------------------------------------------ 
import re
class Version:
    """
    Handle version numbers.
    """
    __regexp__ = re.compile(r"v([0-9]+)r([0-9]+)(?:p([0-9]+))?")
    __regexp_lcg__ = re.compile(r"([0-9]+)([a-z])?")
    def __init__(self, vers_string):
        """
        Construct a version object from a version string, like "v1r2p3" or "54a".
        An empty version string identifies a project without versions.
        """
        self._version = ()
        self._lcg_style = False
        vers_string = str(vers_string)
        m = self.__regexp__.match(vers_string) or \
            self.__regexp_lcg__.match(vers_string) 
        if m:
            if len(m.groups()) == 3: # vXrY[pZ]
                self._lcg_style = False
                self._version = tuple([int(i)
                                       for i in m.groups()
                                       if i is not None])
            else: # XX[y]
                self._lcg_style = True
                if m.group(2):
                    self._version = (int(m.group(1)),m.group(2))
                else:
                    self._version = (int(m.group(1)),)
        elif not vers_string: # unversioned project
            pass
        else:
            raise ValueError("wrong version string: '%s'"%vers_string)
    def __repr__(self):
        """
        Returns a string that can be evaluated to get a version object
        equivalent to the current one.
        """
        return "%s(%r)"%(self.__class__.__name__,str(self))
    def __str__(self):
        """
        Return a the version as version string like "v1r2p3".
        """
        if not self._version:
            return ""
        if self._lcg_style:
            s = "%d"%self._version[0]
            if len(self._version) == 2:
                s += self._version[1]
        else:
            if len(self._version) == 2:
                s = "v%dr%d"
            else:
                s = "v%dr%dp%d"
            s %= self._version
        return s
    def __hash__(self):
        return hash(self._version)
    def __iter__(self):
        return iter(self._version)
    def __cmp__(self, other):
        if type(other) is str:
            other = tuple(Version(other))
        else:
            other = tuple(other)
        if self._version < other: return -1
        elif self._version == other: return 0
        else: return 1

class Project:
    """
    Informations about a project.
    """
    def __init__(self, name, version, tag = None, buildtimedeps = None, runtimedeps = None):
        self.name = name
        self.version = Version(version)
        self.tag = tag
        if buildtimedeps is None:
            self.buildtimedeps = []
        else:
            self.buildtimedeps = buildtimedeps
        if runtimedeps is None:
            self.runtimedeps = []
        else:
            self.runtimedeps = runtimedeps
    def addBuildTimeDep(self, proj_name):
        if proj_name not in self.buildtimedeps:
            self.buildtimedeps.append(proj_name)
    def addRunTimeDep(self, proj_name):
        if proj_name not in self.runtimedeps:
            self.runtimedeps.append(proj_name)
    def __repr__(self):
        return "Project(name=%r,version=%r,tag=%r,buildtimedeps=%r,runtimedeps=%r)" % \
                (self.name,
                 str(self.version),
                 self.tag,
                 self.buildtimedeps,
                 self.runtimedeps)
    
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
    __unversioned_projects__ = {}
    def __init__(self, name, projects = None, base = None, date = None):
        if name in self.__releases__:
            raise DuplicatedReleaseError(name)
        self.name = name
        self.base = base
        self.projects = {}
        if projects is None:
            projects = []
        for (k,v) in projects:
            self.projects[k] = v
        if date:
            self.date = date
        else:
            self.date = DEFAULT_RELEASEDATE
        self.__releases__[name] = self
    def __cmp__(self,other):
        if self.date < other.date: return -1
        elif self.date == other.date: return 0
        else: return 1
        
    def __repr__(self):
        return "Release(name=%r,projects=%r,base=%r,date=%r)"%(self.name,
                                                               self.projects.items(),
                                                               self.base,
                                                               self.date)
    def allProjects(self):
        """
        Returns a list of all the projects available in the release (either directly or
        through the bases), sorted by dependencies. 
        """
        # get all projects from current and base releases
        all_projs = set(self.projects.keys())
        base = self.base
        while base:
            r = self.__releases__[base]
            all_projs.update(r.projects.keys())
            base = r.base
        all_projs.update(Release.__unversioned_projects__.keys())
        # put them in alphabetical order
        all_projs = list(all_projs)
        all_projs.sort()
        # get the actual projects from the names to be able to sort them
        # according their dependencies
        all_projs = [ self[p] for p in all_projs ]
        return _sortProjects(all_projs)
        
    def __str__(self):
        s = "Release '%s'"%self.name
        if self.date > DEFAULT_RELEASEDATE:
            s += " (%4d-%02d-%02d)"%self.date
        s += ":"
        for p in self.allProjects():
            if p.tag:
                s += "\n\t%s\t%s\t(%s)"%(p.name,p.version,p.tag)
            else:
                s += "\n\t%s\t%s"%(p.name,p.version)
        return s
    def __contains__(self, key):
        return ( key in self.projects ) or \
               ( self.base is not None and key in self.__releases__[self.base] ) or \
               ( key in self.__unversioned_projects__ )
    def __getitem__(self, key):
        if key in self.projects:
            return self.projects[key]
        elif self.base:
            return self.__releases__[self.base][key]
        elif key in self.__unversioned_projects__:
            return self.__unversioned_projects__[key]
        else:
            raise KeyError(key)
    def __setitem__(self, key, value):
        if key in self.projects:
            raise DuplicatedProjectError(self.name, key)
        self.projects[key] = Project(name=key, version=value)
    def add(self, project):
        if project.name in self.projects:
            raise DuplicatedProjectError(self.name, project.name)
        self.projects[project.name] = project
    def items(self):
        return self.projects.items()
    def values(self):
        return _sortProjects(self.projects.values())
    def _expandDeps(self, project, function):
        deps = function(self[project])
        deps += reduce(lambda a,b: a+b,
                       [self._expandDeps(p,function) for p in deps],
                       [])
        return deps
    def expandBuildTimeDeps(self, project):
        """
        Return a list of projects on which the requested project depends for building
        (either directly or indirectly).
        """
        return list(set(self._expandDeps(project,lambda p: p.buildtimedeps)))
    def expandRunTimeDeps(self, project):
        """
        Return a list of projects on which the requested project depends at run-time
        (either directly or indirectly, through both build-time and run-time dependencies).
        """
        local_deps = self._expandDeps(project,lambda p: p.runtimedeps)
        deps = reduce(lambda a,b: a+b,
                      [ self.expandRunTimeDeps(p)
                        for p in self.expandBuildTimeDeps(project) + local_deps ],
                      local_deps)
        return list(set(deps))
    
def addUnversionedProject(project):
    if type(project) is not Project:
        project = Project(project,"")
    if project.name in Release.__unversioned_projects__:
        raise DuplicatedProjectError("UnversionedProjects",project.name)
    Release.__unversioned_projects__[project.name] = project
    return project

from xml.sax.handler import ContentHandler
class _ReleaseSAXHandler(ContentHandler):
    """
    SAX content handler used to fill the database of releases from the XML file.
    Note: the database is purged before a 
    """
    def __init__(self):
        ContentHandler.__init__(self)
        self.releases = Release.__releases__
        self._currentRelease = None
        self._currentProject = None
        
    def startDocument(self):
        #print "startDocument"
        # reset the DB of releases
        clean()
        self.releases = Release.__releases__
        self._currentRelease = None
        self._currentProject = None
    def endDocument(self):
        #print "endDocument"
        #pprint(self.releases)
        pass
    def startElement(self, name, attrs):
        #print "start element %s:"%name
        if u'releases_db' in name: # ignore top element
            return
        name_attr = attrs.getValue(u'name')
        if name == u'release':
            if u'date' in attrs:
                # convert string like "2008-05-07" to tuple like (2008,5,7)
                date = tuple([ int(i) for i in attrs[u'date'].split('-')])
            else:
                date = DEFAULT_RELEASEDATE
            self._currentRelease = Release(name_attr,
                                           base = attrs.get(u'base', None),
                                           date = date )
        elif name == u'project':
            self._currentProject = Project(name = name_attr,
                                           version = attrs.getValue(u'version'),
                                           tag  = attrs.get(u'tag',  None))
            self._currentRelease.add(self._currentProject)
        elif name == u'unversioned_project':
            self._currentProject = Project(name = name_attr,
                                           version = "")
            Release.__unversioned_projects__[name_attr] = self._currentProject
        elif name == u'buildtimedep':
            self._currentProject.addBuildTimeDep(name_attr)
        elif name == u'runtimedep':
            self._currentProject.addRunTimeDep(name_attr)
        else:
            print "Warning: unknown element '%s'"%name
    def endElement(self, name):
        #print "end element %s:"%name
        if name == u'release':
            self._currentRelease = None
        elif name == u'project':
            self._currentProject = None
        elif name == u'unversioned_project':
            self._currentProject = None
#------------------------------------------------------------------------------ 
#--- Initialization functions
#------------------------------------------------------------------------------ 
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

def clean():
    """
    Remove all entries from the database (in memory).
    """
    Release.__releases__ = {} 
    Release.__unversioned_projects__ = {} 
#------------------------------------------------------------------------------ 
#--- Internal query functions
#------------------------------------------------------------------------------ 
def _findReleases(project, version):
    """
    Find all the releases that feature the specified pair (project,version).
    """
    return [ r.name
             for r in Release.__releases__.values()
             if project in r and r[project].version == version ]

def _findAllReleases(project, versions):
    """
    Find all the releases that feature the specified project in any of the
    specified versions.
    """
    tmp = set()
    for v in versions:
        tmp.update(_findReleases(project,v))
    tmp = list(tmp)
    tmp.sort()
    return tmp

def _getVersions(project, releases):
    """
    Given a set of releases and a project, find all the versions of the project
    used in those releases.
    """
    versions = [ getRelease(r)[project].version
                 for r in releases
                 if project in getRelease(r) ] # do we want this check?
    versions = list(set(versions)) # remove duplicates
    versions.sort() 
    return versions
#------------------------------------------------------------------------------ 
#--- Public query functions
#------------------------------------------------------------------------------ 
def getCompatibleVersions(project, version, otherproject, keyproj = DEFAULT_KEYPROJECT):
    """
    Given a project and versions, find all the versions of 'otherproject' that
    share the same versions of 'keyproj' as (project,version).
    """
    return _getVersions(otherproject,
                        _findAllReleases(keyproj,
                                         _getVersions(keyproj,
                                                      _findReleases(project, version))))

def getRuntimeVersions(project, version, keyproj = DEFAULT_KEYPROJECT):
    """
    Get a list of projects needed by the specified project and compatible with
    the specified version of it. 
    """
    deps = []
    proj_releases = _findReleases(project, version)
    #proj_releases.sort()
    if proj_releases:
        rt_deps = getRelease(proj_releases[-1]).expandRunTimeDeps(project)
        comp_releases = _findAllReleases(keyproj,
                                         _getVersions(keyproj,
                                                      proj_releases))
        for p in rt_deps:
            vers = _getVersions(p,comp_releases)
            if vers:
                deps.append((p,vers[-1]))
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

def unversionedProject(name):
    """
    Return the set of unversioned projects known in the database. 
    """
    return Release.__unversioned_projects__[name]

def unversionedProjects():
    """
    Return the set of unversioned projects known in the database. 
    """
    return Release.__unversioned_projects__.values()

def generateXML(withSchema = True, stylesheet = "releases_db.xsl"):
    # header
    xml = '<?xml version="1.0" encoding="UTF-8"?>\n'
    if stylesheet:
        xml += '<?xml-stylesheet type="text/xsl" href="%s"?>\n'%stylesheet
    if withSchema:
        xml += '<lhcb:releases_db\n' + \
               ' xmlns:lhcb="http://cern.ch/lhcbproject/dist/"\n' + \
               ' xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"\n' + \
               ' xsi:schemaLocation="http://cern.ch/lhcbproject/dist/ releases_db.xsd ">\n'
    else:
        xml += '<releases_db>\n'
    # body
    rel_names = Release.__releases__.keys()
    rel_names.sort()
    for rel_name in rel_names:
        rel = Release.__releases__[rel_name]
        xml += '  <release name="%s"'%rel.name
        if rel.base: xml += ' base="%s"'%rel.base
        if rel.date > DEFAULT_RELEASEDATE: xml += ' date="%4d-%02d-%02d"'%rel.date
        xml += '>\n'
        for p in rel.values():
            xml += '    <project name="%s" version="%s"'%(p.name,p.version)
            if p.tag: xml += ' tag="%s"'%p.tag
            xml += '>\n'
            for dep in p.buildtimedeps:
                xml += '      <buildtimedep name="%s"/>\n'%dep
            for dep in p.runtimedeps:
                xml += '      <runtimedep name="%s"/>\n'%dep
            xml += '    </project>\n'
        xml += '  </release>\n'
    unversioned = list(unversionedProjects())
    _sortProjects(unversioned)
    for p in unversioned:
        xml += '  <unversioned_project name="%s">\n'%(p.name)
        for dep in p.buildtimedeps:
            xml += '    <buildtimedep name="%s"/>\n'%dep
        for dep in p.runtimedeps:
            xml += '    <runtimedep name="%s"/>\n'%dep
        xml += '  </unversioned_project>\n'
    if withSchema: 
        xml += '</lhcb:releases_db>\n'
    else: 
        xml += '</releases_db>\n'
    return xml

def generateHTML():
    lhcb_base = "http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC"
    def proj_home(p):
        if p == "LCGCMT":
            return "https://twiki.cern.ch/twiki/bin/view/SPI"
        elif p == "Gaudi":
            return "http://proj-gaudi.web.cern.ch/proj-gaudi"
        else:
            return '/'.join([lhcb_base,p.lower()]) 
    def proj_version(p,v):
        h = proj_home(p)
        if p == "LCGCMT":
            return '/'.join([h,'LcgConfiguration%s'%v])
        else:
            return '/'.join([h,'releases',v])
    def hyper_link(url,text):
        return '<a href="%s">%s</a>'%(url,text)
    def proj_link(p, v = None):
        if not v:
            return hyper_link(proj_home(p), p)
        else:
            return hyper_link(proj_version(p,str(v)), v)
    def ordered_list_merge(orig,other):
        a = orig[:]
        b = other
        r = []
        for x in b:
            if x in a:
                p = a.index(x)
                r,a = (r+a[:p+1]),a[p+1:]
            else:
                r.append(x)
        return r + a
    all_proj_names = reduce(ordered_list_merge,[ [ p.name for p in getRelease(r).allProjects() ]
                                                 for r in getReleases() ])
    unversioned_names = [ p.name for p in unversionedProjects() ]
    all_proj_names = [ p
                       for p in all_proj_names
                       if p not in unversioned_names ]
    html = '''<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns:lhcb="http://cern.ch/lhcbproject/dist/">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" href="%s/css/lhcb.css" type="text/css" media="screen">
<title>Releases Database</title>
</head>
<body>
  <div class="ctitle">
    <table id="pagetitle">
      <tbody>
        <tr>
        <td valign="middle" align="center"><h1>Releases Database</h1></td>
        </tr>
      </tbody>
    </table>
  </div>
  <table border="1">
'''%lhcb_base
    rels = [getRelease(r) for r in getReleases()]
    rels.sort(reverse=True)
    html += '<tr><td></td>'
    for pn in all_proj_names:
        html += '<th>%s</th>'%proj_link(pn)
    html += '</tr>'
    for r in rels:
        html += '<tr><td><b>Release "%s"</b>'%r.name
        if r.base:
            html += '<br/>based on: &quot;%s&quot;'%r.base 
        if r.date != DEFAULT_RELEASEDATE:
            html += '<br>date: %4d-%02d-%02d</td>'%r.date
        for pn in all_proj_names:
            html += '<td>'
            if pn in r:
                if pn not in r.projects: 
                    html += '<i>'
                p = r[pn]
                html += proj_link(p.name,p.version)
                if p.tag:
                    html += "<br/>(%s)"%proj_link(p.name,p.tag)
                if pn not in r.projects: 
                    html += '</i>'
            html += '</td>'
        html += '</tr>'
    html += '</table></body></html>'
    return html
