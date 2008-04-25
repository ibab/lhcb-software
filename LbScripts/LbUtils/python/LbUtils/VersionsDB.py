"""
Module providing a database of software releases.
The database is based on an XML file containing the list of (project,version)s
for each version of the complete software stack.
"""
__author__ = "Marco Clemencic <Marco.Clemencic@cern.ch>"
__version__ = "$Id: VersionsDB.py,v 1.1 2008-04-25 15:33:51 marcocle Exp $"

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
        self.projects[key] = value
    
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
            self._currentRelease[name_attr] = attrs.getValue(u'version')
    def endElement(self,name):
        #print "end element %s:"%name
        if name == u'release':
            self._currentRelease = None

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
    parse(source,_ReleaseSAXHandler())

#--- Internal query functions
def _findReleases(project,version):
    """
    Find all the releases that feature the specified pair (project,version).
    """
    return [ r.name
             for r in Release.__releases__.values()
             if project in r and r[project] == version ]

def _findAllReleases(project,versions):
    """
    Find all the releases that feature the specified project in any of the
    specified versions.
    """
    tmp = []
    for v in versions:
        tmp += _findReleases(project,v)
    tmp.sort()
    return tmp

def _getVersions(project,releases):
    """
    Given a set of releases and a project, find all the versions of the project
    used in those releases.
    """
    versions = [ getRelease(r)[project]
                 for r in releases
                 if project in getRelease(r) ] # do we want this check?
    versions.sort()
    tmp = versions
    versions = []
    for i in tmp:
        if i not in versions:
            versions.append(i)
    return versions

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

def getRelease(name):
    """
    Retrieve a release instance by name. 
    """
    return Release.__releases__[name]

def releaseExists(name):
    """
    Tell if a release with the given name is known. 
    """
    return name in Release.__releases__

# @TODO: Use some alphabetical order.
def generateXML():
    # header
    xml = """<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE releases_db SYSTEM "releases_db.dtd">
<releases_db>
"""
    # body
    for rel in Release.__releases__.values():
        xml += '  <release name="' + rel.name
        if rel.base is not None:
            xml += '" base="' + rel.base
        xml += '">\n'
        for p_v in rel.projects.items():
            xml += '    <project name="%s" version="%s"/>\n'%p_v
        xml += '</release>\n'
    xml += '</releases_db>\n'
    return xml
